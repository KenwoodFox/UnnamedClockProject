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
    uint8_t curHour = 0; // Eeprom at some point?
    uint8_t curMinute = 0;

    // Pins
    uint8_t enPin;
    uint8_t dirPin;

public:
    Clock(uint8_t _enPin, uint8_t _dirPin);

    /**
     * @brief Override the currently stored time.
     *
     * @param _min Minute (0 - 59)
     * @param _hr  Hour   (0 - 11)
     */
    void setTime(uint8_t _min, uint8_t _hr);

    /**
     * @brief Advances to the next minute
     *
     * @return true  Done
     * @return false Continue
     */
    bool next();
};
