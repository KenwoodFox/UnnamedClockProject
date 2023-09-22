#include "Clock.h"

Clock::Clock(uint8_t _enPin, uint8_t _dirPin)
{
    enPin = _enPin;
    dirPin = _dirPin;
}

void Clock::begin()
{
    // Setup pins
    pinMode(enPin, OUTPUT);
    pinMode(dirPin, OUTPUT);
}

uint8_t Clock::getHour()
{
    return curHour;
}

uint8_t Clock::getMinute()
{
    return curMinute;
}

void Clock::setTime(uint8_t _min, uint8_t _hr)
{
    curHour = _hr;
    curMinute = _min;
}

void Clock::setTarget(uint8_t _min, uint8_t _hr)
{
    setHour = _hr;
    setMinute = _min;
}

void Clock::autoMove(bool enable)
{
    digitalWrite(dirPin, curMinute % 2 == 0); // Set the dir if even/odd
    digitalWrite(enPin, enable);
}

void Clock::move(bool enable, bool dir)
{
    digitalWrite(dirPin, dir); // Set the dir if even/odd
    digitalWrite(enPin, enable);
}

void Clock::next()
{
    if (curMinute + 1 >= 60)
    {
        curMinute = 0;
        curHour++;
    }
    else
    {
        curMinute++;
        return;
    }

    if (curHour > 11)
    {
        curHour = 0;
    }
}

void Clock::previous()
{
    if (curMinute - 1 < 0)
    {
        curMinute = 59;
        curHour--;
    }
    else
    {
        curMinute--;
        return;
    }

    if (curHour < 0)
    {
        curHour = 11;
    }
}

// show adj time not gps

bool Clock::needAdvance()
{
    if (setHour - curHour < 4)
    {
        // We're not too far behind
        return setMinute > curMinute || setHour > curHour;
    }
}
