/**
 * @brief Todo!
 */

#include "Arduino.h"

#include "boardPins.h"

void setup()
{
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);
}

void loop()
{
  for (uint8_t i = 0; i < 2; i++)
  {
    digitalWrite(DIR_PIN, i);
    delay(250);
    digitalWrite(EN_PIN, true);
    delay(10 * 1000);
    digitalWrite(EN_PIN, false);
    digitalWrite(DIR_PIN, false);
    delay(10 * 1000);
  }
}