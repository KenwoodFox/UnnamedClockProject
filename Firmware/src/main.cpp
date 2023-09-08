/**
 * @brief Todo!
 */

#include "Arduino.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <ArduinoLog.h>

#include "boardPins.h"

// Config
static const uint32_t GPSBaud = 9600;

// Objects
SoftwareSerial ss(RX_PIN, TX_PIN);
TinyGPSPlus gps;

// Headers
void displayInfo();

void setup()
{
  // Pins
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

  // Begin
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  //
  Log.info("Starting version %s", REVISION);
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

  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while (true)
      ;
  }
}

void displayInfo()
{
  Serial.print(F("Location: "));
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10)
      Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}