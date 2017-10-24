#define isDebugging
#include <Arduino.h>
#include "../lib/SensorProtocol/SensorProtocol.h"
#include "../lib/Timer/Timer.h"

SensorProtocol protocol;
Timer newTimer(1000);
byte fooBar[1];
int count = 1;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    protocol.Setup();
    delay(2000);
    Serial.println();
}

void loop() {
  //Create a random number every second
  if(newTimer.OverTime()){
    fooBar[0] = rand() % 254;
    protocol.Send(fooBar, 1);
    Serial.print("Count: ");
    Serial.println(count++);
    Serial.print("Sending: ");
    Serial.println(fooBar[0], DEC);
    newTimer.Reset();
  }


  if(protocol.BytesToRead() > 0){
    // Serial.print("Bytes to REad: ");
    // Serial.println(protocol.BytesToRead());
    protocol.TestCodeHere();
    Serial.print("Receiving: ");
    Serial.println(protocol.ReadByte(), DEC);
    Serial.println();

  }

  protocol.TransmitAndReceive();
}
