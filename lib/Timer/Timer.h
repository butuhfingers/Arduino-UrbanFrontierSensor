#ifndef Timer_h
#define Timer_h
#include <Arduino.h>

class Timer{
public:
  Timer(int duration);
  void Reset();
  bool OverTime();
private:
  long startTime; //These values will be in miliseconds
  int duration;
};

#endif
