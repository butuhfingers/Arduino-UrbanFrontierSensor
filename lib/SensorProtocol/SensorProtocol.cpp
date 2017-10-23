#include "SensorProtocol.h"
#include "../BitsCode/BitsCode.h"

Timer transmitTimer(100);

SensorProtocol::SensorProtocol(){
  sendPinPositive = 5;
  sendPinNegative = 6;
  receivePinPositive = A1;
  receivePinNegative = A0;
  inputThreshhold = (1024 / 2);

  sentBit = false;
  transmitBufferHead, transmitBufferTail = 0;
  receiveBufferHead, receiveBufferTail = 0;
  receivedSpace = true;
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

}

//We need to store data to transmit
void Send(){

}


//This will handle all of our transmitting and receiving
void SensorProtocol::TransmitAndReceive(){
  Receive();
  Transmit();

  return;
}

void SensorProtocol::Transmit(){
  //Check if the head and tail are equal, if so do not continue
  if(transmitBufferHead == transmitBufferTail || transmitTimer.OverTime()){
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
  }else if(transmitBitBuffer[transmitBufferHead % 64] == 0){
    LowBitPulse();
  }else if(transmitBitBuffer[transmitBufferHead % 64] == 1){
    HighBitPulse();
  }

  sentBit = true;
  transmitBufferHead++;
}

void SensorProtocol::Receive(){
  int posPinValue = analogRead(receivePinPositive);
  int negPinValue = analogRead(receivePinNegative);

  int bitType = GetPulseType(posPinValue, negPinValue);

  if(bitType == BitsCode::Code::Space){
    receivedSpace = true;
    return;
  }else if(bitType == BitsCode::Code::On && receivedSpace == true){
    receiveBitBuffer[receiveBufferTail % 64] = 1;
    receiveBufferTail++;
  }else if(bitType == BitsCode::Code::Off && receivedSpace == true){
    receiveBitBuffer[receiveBufferTail % 64] = 0;
    receiveBufferTail++;
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
    bitWrite(newByte, i, receiveBitBuffer[receiveBufferHead++]);
  }

  //There are no bytes left to return
  return newByte;
}

int SensorProtocol::BytesToRead(){
  return floor((receiveBufferTail - receiveBufferHead) / 8);
}


void SensorProtocol::SendPulse(int settingOne, int settingTwo){
  //We need to write to both the positive and negative pins
  analogWrite(sendPinPositive, settingOne);
  analogWrite(sendPinNegative, settingTwo);
}

void SensorProtocol::LowBitPulse(){
  SendPulse(LOW, HIGH);
}

void SensorProtocol::HighBitPulse(){
  SendPulse(HIGH, LOW);
}

void SensorProtocol::StartPulse(){
  SendPulse(HIGH, HIGH);
}

void SensorProtocol::SpacePulse(){
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
