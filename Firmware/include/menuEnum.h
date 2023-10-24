/**
 * @brief A little bit of custom override code for menu
 *
 */

#include <Arduino.h>

const uint8_t numMenuItems = 4;

enum Menu
{
    Status,
    ClockSet,
    ManualMode,
    LeftHandedNess,
};

const uint8_t numPosItems = 3;

enum SetPos
{
    None,
    Hour,
    Minute,
};

inline SetPos operator++(SetPos &eDOW, int)
{
    const SetPos ePrev = eDOW;
    const int i = static_cast<int>(eDOW);
    eDOW = static_cast<SetPos>((i + 1) % numPosItems);
    return ePrev;
}

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
