#include "Timer.h"
#include <limits.h>

//Constructor
Timer::Timer(int newDuration){
  duration = newDuration;
}

void Timer::Reset(){
  startTime = millis();
}

bool Timer::OverTime(){
  //Grab the time so it is constant
  long currentTime = millis();
  if(currentTime - startTime >= duration){
    return true;
  //This statement is to check if the clock has wrapped around on the arduino
}else if(currentTime < startTime){
    if(LONG_MAX - startTime + currentTime > duration){
      return true;
    }
  }

  return false;
}
