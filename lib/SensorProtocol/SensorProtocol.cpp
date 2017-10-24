#include "SensorProtocol.h"
#include "../BitsCode/BitsCode.h"

Timer transmitTimer(1);
Timer testTimer(1000);

SensorProtocol::SensorProtocol(){
  sendPinPositive = 5;
  sendPinNegative = 6;
  receivePinPositive = A0;
  receivePinNegative = A1;
  inputThreshhold = (512);

  sentBit = false;
  transmitBufferHead = 0;
  transmitBufferTail = 0;
  receiveBufferHead = 0;
  receiveBufferTail = 0;
  receivedSpace = true;
}

int SensorProtocol::Mod(int a, int b){
  return a >= 0 ? a % b : ((b - abs(a % b)) % b);
}

//Setup for the protocol
void SensorProtocol::Setup(){
  pinMode(sendPinPositive, OUTPUT);
  pinMode(sendPinNegative, OUTPUT);
  pinMode(receivePinPositive, INPUT);
  pinMode(receivePinNegative, INPUT);

  TestCodeHere();
}

void SensorProtocol::TestCodeHere(){
  int variable = 9;

  Serial.print(" Buffers: ");
  Serial.print(receiveBufferHead);
  Serial.print(" | ");
  Serial.print(receiveBufferTail);
  Serial.print(" | ");
  Serial.print("Send Buffer: ");
  Serial.print(transmitBufferHead);
  Serial.print(" | ");
  Serial.print(transmitBufferTail);
  Serial.print(" | ");
  Serial.print(floor((receiveBufferTail - receiveBufferHead) / 8));
  Serial.println();
}

//We need to store data to transmit
void SensorProtocol::Send(byte *byteArray, int arraySize){
  //Go through the bytes and stores them for transmission
  for(int i = 0;i < arraySize;i++){
    //We need to go through the individual bits
    for(int bit = 0;bit < 8;bit++){
      transmitBitBuffer[transmitBufferTail] = bitRead(byteArray[i], bit);
      transmitBufferTail++;
    }
  }
}


//This will handle all of our transmitting and receiving
void SensorProtocol::TransmitAndReceive(){
  Transmit();
  Receive();

  if(testTimer.OverTime()){
    testTimer.Reset();
  }

  return;
}

void SensorProtocol::Transmit(){
  //Check if the head and tail are equal, if so do not continue
  if(transmitBufferHead == transmitBufferTail || (!transmitTimer.OverTime())){
    return;
  }
  //We need to reset our Timer
  transmitTimer.Reset();

  //We need to send a space bit if a bit was sent previously
  if(sentBit){
    SpacePulse();
    sentBit = false;
    return;
  //We need to get the bit at the counter and send it
  //But first we need to see if it is on or off
}else if(transmitBitBuffer[Mod(transmitBufferHead, bufferSize)] == 0){
    LowBitPulse();
    sentBit = true;
    transmitBufferHead++;
      Serial.print(" T+ ");
  }else if(transmitBitBuffer[Mod(transmitBufferHead, bufferSize)] == 1){
    HighBitPulse();
    sentBit = true;
    transmitBufferHead++;
      Serial.print(" T+ ");
  }
}

void SensorProtocol::Receive(){
  int posPinValue = analogRead(receivePinPositive);
  int negPinValue = analogRead(receivePinNegative);

  int bitType = GetPulseType(posPinValue, negPinValue);

  if(bitType == BitsCode::Code::Space && !receivedSpace){
    receivedSpace = true;
//    Serial.println("R Space");
    return;
  }else if(bitType == BitsCode::Code::On && receivedSpace){
    receiveBitBuffer[Mod(receiveBufferTail, bufferSize)] = 1;
    receiveBufferTail++;
//    Serial.println("R1");
    receivedSpace = false;
  }else if(bitType == BitsCode::Code::Off && receivedSpace){
    receiveBitBuffer[Mod(receiveBufferTail, bufferSize)] = 0;
    receiveBufferTail++;
//  Serial.println("R0");
    receivedSpace = false;
  }
}

//We need to read the current byte
byte SensorProtocol::ReadByte(){
  //We need to check if there any bits to write
  //Remember we need a full byte for this to work
  if(BytesToRead() < 1){
    return 0;
  }

  byte newByte;
  //Go through the 8 bits and write them to the byte
  for(int i = 0;i < 8;i++){
    //Increment our buffer head as we read it to the bytes
    bitWrite(newByte, i, receiveBitBuffer[Mod(receiveBufferHead, bufferSize)]);
    receiveBufferHead++;
  }

  //There are no bytes left to return
  return newByte;
}

int SensorProtocol::BytesToRead(){
  return floor((receiveBufferTail - receiveBufferHead) / 8);
}


void SensorProtocol::SendPulse(int settingOne, int settingTwo){
  //We need to write to both the positive and negative pins
  digitalWrite(sendPinPositive, settingOne);
  digitalWrite(sendPinNegative, settingTwo);
}

void SensorProtocol::LowBitPulse(){
//  Serial.println("S0");
  SendPulse(LOW, HIGH);
}

void SensorProtocol::HighBitPulse(){
//  Serial.println("S1");
  SendPulse(HIGH, LOW);
}

void SensorProtocol::StartPulse(){
  SendPulse(HIGH, HIGH);
}

void SensorProtocol::SpacePulse(){
//  Serial.println("Space");
  SendPulse(LOW, LOW);
}

int SensorProtocol::GetPulseType(int posPulse, int negPulse){
  //Check if our positive pulse is HIGH
  if(posPulse > inputThreshhold){
    //Check if we are a positive bit
    if(negPulse < inputThreshhold){
      return BitsCode::Code::On;
    //Check if we are starting a block of data/packet
    }else{
      return BitsCode::Code::Start;
    }
  }else{
    //Check if we are a negative bit
    if(negPulse > inputThreshhold){
      return BitsCode::Code::Off;
      //Check if we are a space
    }else{
      return BitsCode::Code::Space;
    }
  }

}
