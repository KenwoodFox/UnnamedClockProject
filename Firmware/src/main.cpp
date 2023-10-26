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

// Heap/global memory
bool timeIsSet = false;   // True when the user has set the time
int controllerHour = 0;   // The hour the controller thinks it is (0-11)
int controllerMinute = 0; // The minute the controller thinks it is (0-59)

// States
enum State
{
  Paused,
  MoveL1,
  StopL1,
  MoveL2,
  StopL2
};

State currentState = Paused; // Global state

void setup()
{
  // Begin
  Serial.begin(115200);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln(F("Starting version %s"), REVISION);

  // Configure hardware
  pinMode(LED_BUILTIN, OUTPUT);

  // Configure software
  Serial.setTimeout(5000);

  Log.infoln(F("Done."));
}

void loop()
{
  switch (currentState)
  {
  case Paused:
    /* what to do when paused */
    break;

  default:
    // How did we get here?..
    currentState = Paused;
    break;
  }

  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);

  Log.infoln(F("Hour is %d, minute is %d"), controllerHour, controllerMinute);
}

// Super simple serial event (called from loop)
void serialEvent()
{
  Log.warningln(F("Please enter hour [0-11]:"));
  int _hourToSet = Serial.parseInt();

  Log.warningln(F("Please enter minute [0-59]:"));
  int _minToSet = Serial.parseInt();

  // clear buf
  while (Serial.available())
  {
    Serial.read();
  }

  controllerHour = _hourToSet;
  controllerMinute = _minToSet;
}
