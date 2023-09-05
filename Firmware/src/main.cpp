/**
 * @brief Todo!
 */

#include "Arduino.h"

const uint8_t dir = 12;
const uint8_t en = 11;

void setup()
{
  pinMode(dir, OUTPUT);
  pinMode(en, OUTPUT);
}

void loop()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    digitalWrite(dir, i);
    delay(250);
    digitalWrite(en, true);
    delay(10 * 1000);
    digitalWrite(en, true);
    digitalWrite(dir, false);
    delay(10 * 1000);
  }
}