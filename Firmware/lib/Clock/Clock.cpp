#include "Clock.h"

Clock::Clock(uint8_t _enPin, uint8_t _dirPin)
{
    enPin = _enPin;
    dirPin = _dirPin;
}

void Clock::setTime(uint8_t _min, uint8_t _hr)
{
    curHour = _hr;
    curMinute = _min;
}
