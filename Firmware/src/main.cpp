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
    digitalWrite(DIR_PIN, i);    // Set the dir
    delay(250);                  // Delay while dir is latched
    digitalWrite(EN_PIN, true);  // Enable movement
    delay(10 * 1000);            // Wait 10 seconds
    digitalWrite(EN_PIN, false); // Disable movement
    digitalWrite(DIR_PIN, false);
    delay(25 * 1000); // Wait for next minute
    delay(25 * 1000);
  }
}