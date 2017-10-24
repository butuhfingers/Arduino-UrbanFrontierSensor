#ifndef SensorProtocol_h
#define SensorProtocol_h
#include <Arduino.h>
#include "../Timer/Timer.h"

class SensorProtocol{
public:
  SensorProtocol();
  void Setup();
  void TransmitAndReceive();
  void Send(byte *byteArray, int arraySize);
  byte ReadByte();
  int BytesToRead();
  void TestCodeHere();
protected:
private:
  int bufferSize = 64;

  //Private variables
  int sendPinPositive;
  int sendPinNegative;
  int receivePinPositive;
  int receivePinNegative;
  int inputThreshhold;

  int transmitBitBuffer[64];
  int transmitBufferHead; //This is the head of the bit buffer that we start from
  int transmitBufferTail; //This is the end of the bit buffer so we can wrap around the array
  bool sentBit; //This will tell us if we sent a bit or a space previously

  int receiveBitBuffer[64];
  int receiveBufferHead;
  int receiveBufferTail;
  bool receivedSpace;

  //Private properties
  //Private Methods / Functions
  void Transmit();
  void Receive();
  void SendPulse(int settingOne, int settingTwo); //This is the main function for the next pulse senders
  void LowBitPulse();
  void HighBitPulse();
  void StartPulse();
  void SpacePulse();
  int GetPulseType(int posPulse, int negPulse);
  int Mod(int a, int b);
};

#endif
