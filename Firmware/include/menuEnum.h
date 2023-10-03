/**
 * @brief A little bit of custom override code for menu
 *
 */

#include <Arduino.h>

const uint8_t numMenuItems = 3;

enum Menu
{
    Status,
    ClockSet,
    ManualMode,
};

inline Menu operator++(Menu &eDOW, int)
{
    const Menu ePrev = eDOW;
    const int i = static_cast<int>(eDOW);
    eDOW = static_cast<Menu>((i + 1) % numMenuItems);
    return ePrev;
}

inline Menu operator--(Menu &eDOW, int)
{
    const Menu ePrev = eDOW;
    const int i = static_cast<int>(eDOW);
    eDOW = static_cast<Menu>((i - 1) % numMenuItems);
    return ePrev;
}
