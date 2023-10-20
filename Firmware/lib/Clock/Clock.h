/**
 * @file Clock.h
 * @author Joe
 * @brief Synchronized Clock Driver Code
 */

#include <Arduino.h>
#include <EEPROM.h>

/**
 * @brief Display object used to manipulate the clock controller.
 *
 * @author Joe (joe.sedutto@silvertech.com)
 */
class Clock
{
private:
    // Current Positions
    uint8_t curHourAddr = 0;   // Remove me!
    uint8_t curMinuteAddr = 1; // Remove me!
    uint8_t curHour = 0;       // Need to make eeprom rotate automatically!
    uint8_t curMinute = 0;

    // Target Positions
    uint8_t setHour = 0;
    uint8_t setMinute = 0;

    // Pins
    uint8_t enPin;
    uint8_t dirPin;

    // Settings
    bool isLeftHanded = false;

public:
    // Locks
    bool movementEnabled = false;

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
     * @brief Set if left-handed true or false
     *
     * @param set whether to toggle the value
     * @return true is currently left handed
     * @return false is not left handed
     */
    bool setLeftHanded(bool set = false);

    /**
     * @brief Set or unset movement
     *
     * @param _set True or False
     */
    void setMovementEnabled(bool _set);

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
     * @brief Automatically move the clock to the next minute
     *
     * @param enable Enable movement or not.
     */
    void autoMove(bool enable);

    /**
     * @brief Advances to the next stored minute
     *
     * @see Clock#previous:
     */
    void next();

    /**
     * @brief Inverse of next
     *
     * @see Clock#next:
     */
    void previous();

    /**
     * @brief True when the algorithm is running behind and needs to catch up
     *
     * @return true   Clock needs to advance
     * @return false  Normal operation
     */
    bool needAdvance();
};
