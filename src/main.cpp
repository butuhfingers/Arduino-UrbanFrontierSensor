#define isDebugging
#include <Arduino.h>
#include "../lib/SensorProtocol/SensorProtocol.h"
#include "../lib/Timer/Timer.h"

SensorProtocol protocol;
Timer newTimer(2000);

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);

    protocol.Setup();
}

void loop() {
    // put your main code here, to run repeatedly:
    if(newTimer.OverTime()){
      Serial.print(millis());
      Serial.println(": Over Time");
      newTimer.Reset();
      Serial.println(protocol.ReadByte());
    }
}
