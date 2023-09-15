/**
 * @file customChars.h
 * @brief Custom LCD Chars
 */

#include <Arduino.h>

uint8_t upChar[8] = {
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
};

uint8_t downChar[8] = {
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b00100,
    0b11111,
    0b01110,
    0b00100,
};

uint8_t clockAChar[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b10101,
    0b10101,
    0b10001,
    0b01110,
    0b00000,
};

uint8_t clockBChar[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b10101,
    0b10111,
    0b10001,
    0b01110,
    0b00000,
};

uint8_t clockCChar[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b10101,
    0b10101,
    0b10101,
    0b01110,
    0b00000,
};

uint8_t clockDChar[8] = {
    0b00000,
    0b01110,
    0b10001,
    0b10101,
    0b11101,
    0b10001,
    0b01110,
    0b00000,
};