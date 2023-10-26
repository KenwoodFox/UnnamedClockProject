/**
 * @brief Source code for the UNCC project.
 */

#include <Clock.h>
#include <ArduinoLog.h>
#include <TinyGPSPlus.h>

#include "boardPins.h"

// Config
static const uint32_t GPSBaud = 9600;
static const int mvmtTime = 10 * 1000; // Movement time in ms
static const int pauseTime = 2 * 1000; // Time between moves

// Objects
TinyGPSPlus gps;

void setup()
{
  // Begin
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln(F("Starting version %s"), REVISION);

  pinMode(LED_BUILTIN, OUTPUT);

  Log.infoln(F("Done."));
}

void loop()
{
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
}
