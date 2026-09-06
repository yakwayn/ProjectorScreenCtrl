library rc-switch (sui77).

Hailege 315Mhz RF.
Transmitter is the squer module. Antenna 17.3 cm.

#include <RCSwitch.h>

const int TX_PIN = 27;
const int RX_PIN = 32;

RCSwitch mySwitch = RCSwitch();

  mySwitch.enableTransmit(TX_PIN);
  mySwitch.enableReceive(digitalPinToInterrupt(RX_PIN));

if (mySwitch.available()) {
    Serial.print("Received Value: ");
    Serial.print(mySwitch.getReceivedValue());
    Serial.print(" | Bit length: ");
    Serial.print(mySwitch.getReceivedBitlength());
    Serial.print(" | Protocol: ");
    Serial.println(mySwitch.getReceivedProtocol());

    mySwitch.resetAvailable(); 
  }

mySwitch.send(up, 24);

**TO DO**
- Change to ESP8266 ESP-01S WiFi Module
- Soldering on pcb board.
