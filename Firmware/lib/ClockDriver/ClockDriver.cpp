#include "ClockDriver.h"

ClockDriver::ClockDriver(uint8_t _enPin, uint8_t _dirPin)
{
    enPin = _enPin;
    dirPin = _dirPin;
}

void ClockDriver::begin()
{
    // Setup pins
    pinMode(enPin, OUTPUT);
    pinMode(dirPin, OUTPUT);

    // Load inital eeprom values
    curHour = EEPROM.read(curHourAddr);
    curMinute = EEPROM.read(curMinuteAddr);
}

uint8_t ClockDriver::getHour()
{
    return curHour;
}

uint8_t ClockDriver::getMinute()
{
    return curMinute;
}

void ClockDriver::setTime(uint8_t _min, uint8_t _hr)
{
    curHour = _hr;
    curMinute = _min;

    // Write EEP
    EEPROM.write(curHourAddr, curHour);
    EEPROM.write(curMinuteAddr, curMinute);
}

void ClockDriver::setTarget(uint8_t _min, uint8_t _hr)
{
    setHour = _hr;
    setMinute = _min;
}

void ClockDriver::setMovementEnabled(bool _set)
{
    movementEnabled = _set;
}

void ClockDriver::autoMove(bool enable)
{
    digitalWrite(dirPin, !(curMinute % 2 == 0)); // Set the dir if even/odd
    digitalWrite(enPin, enable && movementEnabled);
}

void ClockDriver::move(bool enable, bool dir)
{
    digitalWrite(dirPin, dir); // Set the dir if even/odd
    digitalWrite(enPin, enable && movementEnabled);
}

void ClockDriver::next()
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

    // Write EEP
    EEPROM.write(curHourAddr, curHour);
    EEPROM.write(curMinuteAddr, curMinute);
}

void ClockDriver::previous()
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

bool ClockDriver::needAdvance()
{
    if (!movementEnabled)
    {
        return false;
    }

    if (((setHour - curHour + 12) % 12) < 4)
    {
        // We're not too far behind
        return setMinute > curMinute || setHour != curHour;
    }
    return false;
}
