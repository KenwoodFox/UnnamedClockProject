#ifndef ARDUINO_H
#include <Arduino.h>
#endif

// Todo move this somewhere else!
int getLocalHour(uint8_t utcTime)
{
    // Adjust the RTC to the current GPS time. (eventually move this to a sync function)
    // rtc.adjust(myTZ.toLocal(DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()).unixtime()));

    // Most efficent way to do this (i hope)
    // time_t _local = myTZ.toLocal(DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()).unixtime());

    // return hour(_local);

    // Hardcoding for EST
    uint8_t _est = 4;
    uint8_t _edt = 5;
    return (12 + (utcTime - _est)) % 12; // This needs to be changed!
}