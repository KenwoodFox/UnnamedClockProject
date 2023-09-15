/**
 * @file Clock.h
 * @author Joe
 * @brief Synchronized Clock Driver Code
 */

#include "Arduino.h"

/**
 * @brief Display object used to manipulate the clock controller.
 *
 * @author Joe (joe.sedutto@silvertech.com)
 */
class Clock
{
private:
    // Current Positions
    uint8_t hour = 0; // Settable via eeprom
    uint8_t minute = 0;

public:
    // Override the currently stored time.
    void setTime(uint8_t _min, uint8_t _hr);
};
