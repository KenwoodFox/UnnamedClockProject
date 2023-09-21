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

    // Target Positions
    uint8_t setHour = 0;
    uint8_t setMinute = 0;

    // Pins
    uint8_t enPin;
    uint8_t dirPin;

public:
    Clock(uint8_t _enPin, uint8_t _dirPin);

    /**
     * @brief Important setup functions for clock.
     *
     */
    void begin();

    /**
     * @brief Override the currently stored time.
     *
     * @param _min Minute (0 - 59)
     * @param _hr  Hour   (0 - 11)
     */
    void setTime(uint8_t _min, uint8_t _hr);

    /**
     * @brief Override the currently stored time.
     *
     * @param _min Minute (0 - 59)
     * @param _hr  Hour   (0 - 11)
     */
    void setTarget(uint8_t _min, uint8_t _hr);

    /**
     * @brief Get the current hour
     *
     * @return uint8_t Hour (0 - 11)
     */
    uint8_t getHour();

    /**
     * @brief Get the current minute
     *
     * @return uint8_t Minute (0 - 59)
     */
    uint8_t getMinute();

    /**
     * @brief Moves the clock
     *
     * @param enable  Enable Output
     * @param dir     Movement Direction
     */
    void move(bool enable, bool dir);

    /**
     * @brief Advances to the next stored minute
     *
     */
    void next();

    /**
     * @brief True when the algorithm is running behind and needs to catch up
     *
     * @return true   Clock needs to advance
     * @return false  Normal operation
     */
    bool needAdvance();
};
