#include <WiFi.h>
#include <WebServer.h>
#include <RCSwitch.h>

const char* ssid = "network_name";
const char* password = "secret";

WebServer server(80);
RCSwitch mySwitch = RCSwitch();

const int TX_PIN = 27;
const int RX_PIN = 32;

const unsigned long stopCode = 15254232; 
const unsigned long upCode = 15254226;
const unsigned long downCode = 15254228;

unsigned long downTimerStart = 0;
bool isDownTimerActive = false;
const unsigned long DOWN_DURATION = 38000; 

const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 RF Controller</title>
  <style>
    body { text-align:center; font-family: Arial, sans-serif; margin-top: 50px; background-color: #f4f4f4;}
    button { font-size: 24px; padding: 15px 40px; margin: 10px; cursor: pointer; border-radius: 8px; border: none; background-color: #007bff; color: white;}
    button:active { background-color: #0056b3; }
  </style>
</head>
<body>
  <h1>שלט מסך מקרן</h1>
  <button onclick="fetch('/up')">Up</button><br>
  <button onclick="fetch('/stop')">Stop</button><br>
  <button onclick="fetch('/down')">Down</button>
</body>
</html>
)rawliteral";


void sendUpCommand() {
  Serial.println("--> Command: Up");
  mySwitch.send(upCode, 24);
  isDownTimerActive = false; 
}

void sendStopCommand() {
  Serial.println("--> Command: Stop");
  mySwitch.send(stopCode, 24);
  isDownTimerActive = false;
}

void sendDownCommand() {
  Serial.println("--> Command: Down");
  mySwitch.send(downCode, 24);
  
  downTimerStart = millis();
  isDownTimerActive = true;
}

// --------------------------------------------------

void setup() {
  Serial.begin(115200);
  mySwitch.enableTransmit(TX_PIN);
  mySwitch.enableReceive(digitalPinToInterrupt(RX_PIN));
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []() {
    server.send(200, "text/html", htmlPage);
  });

  server.on("/up", []() {
    sendUpCommand();
    server.send(200, "text/plain", "Up sent");
  });

  server.on("/stop", []() {
    sendStopCommand();
    server.send(200, "text/plain", "Stop sent");
  });

  server.on("/down", []() {
    sendDownCommand();
    server.send(200, "text/plain", "Down sent, 38s timer started");
  });

  server.begin();
  Serial.println("Web Server Started. RF 433MHz Ready!");
}

void loop() {
  server.handleClient();

  if (isDownTimerActive && (millis() - downTimerStart >= DOWN_DURATION)) {
    Serial.println("--> Auto Stop: 38 seconds reached!");
    mySwitch.send(stopCode, 24);
    isDownTimerActive = false; 
  }

  if (mySwitch.available()) {
    Serial.print("Received Value: ");
    Serial.print(mySwitch.getReceivedValue());
    Serial.print(" | Bit length: ");
    Serial.print(mySwitch.getReceivedBitlength());
    Serial.print(" | Protocol: ");
    Serial.println(mySwitch.getReceivedProtocol());

    mySwitch.resetAvailable(); 
  }

  if (Serial.available() > 0) {
    char inChar = Serial.read();

    switch(inChar) {
      case 'u':
        sendUpCommand();
        break;
      case 's':
        sendStopCommand();
        break;
      case 'd':
        sendDownCommand();
        break;
    }
  }
}
