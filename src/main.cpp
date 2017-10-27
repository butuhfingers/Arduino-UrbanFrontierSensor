#define isDebugging
#include <Arduino.h>
#include "../lib/SensorProtocol/SensorProtocol.h"
#include "../lib/Timer/Timer.h"
#include "../lib/RequestTypes/RequestTypes.h"
#include "../lib/SensorType/SensorType.h"

SensorProtocol protocol;
Timer newTimer(100);

byte sensorAddress = 15;
int dataCount = 0;
byte dataLength = 0;
int dataValue = 0;
byte dataCode = 0;
byte dataSensorType = 0;
byte dataAddress = 0;

void SendSensorData(byte toAddress, byte requestCode, byte sensorType, int sensorValue){
  byte length = 6;
  //Structure of the data
  //Address 8-bit
  //Length  8-bit
  //Request Code  8-bit
  //Sensor Type 8-bit
  //Sensor's value 16-bit
  //48-bit total
  byte sendData[6];
  //Define our send data
  sendData[0] = toAddress;  //Where is the data going to?
  sendData[1] = length;  //The length of our sendData as a whole
  sendData[2] = requestCode;  //What are we requesting? information or sending it
  sendData[3] = sensorType; //The sensor type: humidity, temperature, water level
  sendData[4] = sensorValue;  //The right half of the sensor value
  sendData[5] = sensorValue >> 8; //We need to shift right 8 to store the data on the left

  //Send the data out
  protocol.Send(sendData, length);
}

void RequestSensorData(){
  SendSensorData(15, RequestTypes::Code::sendMeNodeData, SensorType::Code::none, 357);
}

void ResetReceivedData(){
  dataCount = 0;
  dataLength = 0;
  dataValue = 0;
  dataCode = 0;
  dataSensorType = 0;
  dataAddress = 0;
}

void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    protocol.Setup();
    delay(2000);
    RequestSensorData();
}

void loop() {
  //Create the current byte
  byte currentByte = 0;
  bool haveByte = false;
  //Get the bytes that are being read to us
  if(protocol.BytesToRead() > 0){
    currentByte = protocol.ReadByte();
    dataCount++;
    haveByte = true;
    Serial.println(currentByte);
  }

  //Check if our count is greater than 0
  if(dataCount > 0 && haveByte){
    switch (dataCount) {
      //Set our data address
      case 1:
        dataAddress = currentByte;
        break;
      //Set our data length
      case 2:
        dataLength = currentByte;
        break;
      //Set our data code
      case 3:
        dataCode = currentByte;
        break;
      //Set our data sensor type
      case 4:
        dataSensorType = currentByte;
        break;
      //First half fo the sensor value
      case 5:
        dataValue = currentByte;
        break;
      //second half of the sensor value
      case 6:
        dataValue = (dataValue) + (currentByte << 8);

        Serial.print("#");
        Serial.print(dataAddress);
        Serial.print("#");
        Serial.print(dataSensorType);
        Serial.print("#");
        Serial.print(dataValue);
        Serial.println("#");

        break;
      default:
        break;
    }
  }

  protocol.TransmitAndReceive();
}
