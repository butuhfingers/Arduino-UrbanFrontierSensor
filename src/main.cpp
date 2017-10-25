#define isDebugging
#include <Arduino.h>
#include "../lib/SensorProtocol/SensorProtocol.h"
#include "../lib/Timer/Timer.h"

SensorProtocol protocol;
Timer newTimer(100);
byte fooBar[3];
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
    for(int i = 0;i < 3;i++){
      fooBar[i] = rand() % 254;
    }
    protocol.Send(fooBar, 3);
    Serial.print("Count: ");
    Serial.println(count++);
    Serial.print("Sending: ");
    for(int i = 0;i < 3;i++){
      Serial.println(fooBar[i], DEC);
    }
    newTimer.Reset();
  }


  if(protocol.BytesToRead() > 0){
    // Serial.print("Bytes to REad: ");
    // Serial.println(protocol.BytesToRead());
    // protocol.TestCodeHere();
    Serial.print("Receiving: ");
    Serial.println(protocol.ReadByte(), DEC);
    Serial.println();

  }

  protocol.TransmitAndReceive();
}
