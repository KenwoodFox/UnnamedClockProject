/**
 * @brief Todo!
 */

#include "Arduino.h"
#include <Arduino_FreeRTOS.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <ArduinoLog.h>
#include <Bounce2.h>
#include <LiquidCrystal.h>
// #include <Timezone.h>
// #include <RTClib.h>

#include "Clock.h"
#include "boardPins.h"
#include "menuEnum.h"
#include "customChars.h"
#include "timeTools.h"

// Config
static const uint32_t GPSBaud = 9600;
static const int mvmtTime = 10 * 1000; // Movement time in ms
static const int pauseTime = 2 * 1000; // Time between moves

// Objects
SoftwareSerial ss(RX_PIN, TX_PIN);
TinyGPSPlus gps;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
Clock clock(EN_PIN, DIR_PIN);
// RTC_DS3231 rtc;

// Buttons
Bounce2::Button upBtn = Bounce2::Button();
Bounce2::Button downBtn = Bounce2::Button();
Bounce2::Button leftBtn = Bounce2::Button();
Bounce2::Button righBtn = Bounce2::Button();
Bounce2::Button menuBtn = Bounce2::Button();

// Task Handlers
TaskHandle_t TaskDriver_Handler;
TaskHandle_t TaskInterface_Handler;

// Special modes
bool manualOverride = false; // Used when manually overriding

// // Timezone
// TimeChangeRule myDST = {"EDT", Second, Sun, Mar, 2, -240}; // Daylight time = UTC - 4 hours
// TimeChangeRule mySTD = {"EST", First, Sun, Nov, 2, -300};  // Standard time = UTC - 5 hours
// Timezone myTZ(myDST, mySTD);

// Defs
void displayInfo();
void TaskDriver(void *pvParameters);
void TaskInterface(void *pvParameters);

void setup()
{
  // Begin
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);

  Log.infoln(F("Starting version %s"), REVISION);

  /**
   * Because memory is limited, lets try and have
   * as few tasks as possible running.
   */

  // Setup tasks
  Log.infoln(F("Starting Task Driver"));
  xTaskCreate(
      TaskDriver,           // A pointer to this task in memory
      "Driver",             // A name just for humans
      160,                  // This stack size can be checked & adjusted by reading the Stack Highwater
      NULL,                 // Parameters passed to the task function
      2,                    // Priority, with 2 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &TaskDriver_Handler); // Task handle

  Log.infoln(F("Starting Task Interface"));
  xTaskCreate(
      TaskInterface, // Handles human interface and serial interface
      "Interface",
      200,
      NULL,
      2,
      &TaskInterface_Handler);
}

void loop()
{
  // Empty. Things are done in Tasks.
  // vTaskDelete(NULL); // Exit task
}

void TaskDriver(void *pvParameters)
{
  (void)pvParameters;

  // Begin
  Log.infoln(F("Created %s Task, heap size is %d"), pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));

  // Setup for this task
  pinMode(LED_BUILTIN, OUTPUT);
  clock.begin();
  // rtc.begin();

  // Prev time
  TickType_t prevTime;

  // Local
  int watermark = 99999;

  // "Loop"
  for (;;)
  {
    // Move clock
    if (gps.time.isValid())
    { // Ready condition here
      if (clock.needAdvance() || manualOverride)
      {
        Log.verboseln(F("Moving clock, %d to %d."), clock.getMinute(), gps.time.minute()); // The minute will never change
        clock.autoMove(true);                                                              // Begin move
        xTaskDelayUntil(&prevTime, mvmtTime / portTICK_PERIOD_MS);                         // Time spent moving
        clock.next();                                                                      // Auto advance to the next
        xTaskDelayUntil(&prevTime, pauseTime / portTICK_PERIOD_MS);                        // Time between moves
      }
      else
      {
        clock.move(false, false); // Stop moving
      }
    }

    clock.setTarget(gps.time.minute(), getLocalHour(gps.time.hour())); // Set the target

    // Blinky
    digitalWrite(LED_BUILTIN, gps.time.second() % 2 == 0); // TODO: Move me, misc function

    // Waterline
    if (uxTaskGetStackHighWaterMark(NULL) < watermark) // TODO: Move me, misc function
    {
      watermark = uxTaskGetStackHighWaterMark(NULL);
      Log.warningln(F("%s: Watermark dropped to %d bytes."), pcTaskGetName(NULL), watermark);
    }

    xTaskDelayUntil(&prevTime, 100 / portTICK_PERIOD_MS); // Wait to resume
  }
}

// Daylight savings code automatic
// christinsen timezone code <- have to get this working right eventually
// Do the catchup/set code!
// Hardcode the timeozne library! <- Christen
// Wire in the RTC today <-

// Move me somewhere else!
void TaskInterface(void *pvParameters)
{
  (void)pvParameters;

  // Begin
  Log.infoln(F("Created %s Task, heap size is %d"), pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));

  // Setup (lcd)
  lcd.begin(16, 2);
  lcd.print(F("UCC Version"));
  lcd.setCursor(0, 1);
  lcd.print(REVISION);

  // Prev time
  TickType_t prevTime;

  // Local
  int watermark = 99999;

  // Setup (custom chars)
  lcd.createChar(0, upChar);
  lcd.createChar(1, downChar);
  lcd.createChar(2, clockAChar);
  lcd.createChar(3, clockBChar);
  lcd.createChar(4, clockCChar);
  lcd.createChar(5, clockDChar);

  // Setup (Buttons)
  upBtn.attach(UP_PIN, INPUT_PULLUP);
  downBtn.attach(DOWN_PIN, INPUT_PULLUP);
  leftBtn.attach(LEFT_PIN, INPUT_PULLUP);
  righBtn.attach(RIGH_PIN, INPUT_PULLUP);
  menuBtn.attach(MENU_PIN, INPUT_PULLUP);

  // Menu index/temp values
  Menu menuIdx = ClockSet;
  char _lineBuf[17]; // Screen is 16 long but null term is extra undrawn byte
  SetPos pos = None;
  int _HrToSet = 0;   // The hour we will set
  int _MinToSet = 0;  // The minute we will set
  uint8_t _hrpos = 0; // The x location of the hour number (always 6 when we start running this menu)

  for (uint8_t i = 0; i < 5; i++)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  // "Loop"
  for (;;)
  {
    // Update buttons
    upBtn.update();
    downBtn.update();
    leftBtn.update();
    righBtn.update();
    menuBtn.update();

    switch (menuIdx)
    {
    case Status:;
      lcd.setCursor(0, 0);
      if (gps.date.isValid() && gps.satellites.value() >= 4)
      {
        lcd.print(F("GPS Ready."));
      }
      else
      {
        lcd.print(F("Wait for GPS..."));
        lcd.setCursor(0, 1);
        lcd.print(gps.satellites.value());
        lcd.print(F("/4 Satellites."));
      }
      break;

    case ClockSet:;
      // Check if we're setting something

      _hrpos = 6;

      switch (pos)
      {
      case Minute:;
        _hrpos += 3; // Scoot to the right to cover the minute

        if (upBtn.pressed())
        {
          _MinToSet++;
          _MinToSet = _MinToSet % 60;
        }

        if (downBtn.pressed())
        {
          _MinToSet--;
          _MinToSet < 0 ? _MinToSet = 59 : _MinToSet = _MinToSet;
        }

      case Hour:;
        // Dont move if we're setting time!
        clock.setMovementEnabled(false);

        // One last check to make sure this loop is simple (dont overrun this segment)
        if (pos != Minute)
        {
          if (upBtn.pressed())
          {
            _HrToSet++;
            _HrToSet = _HrToSet % 12;
          }

          if (downBtn.pressed())
          {
            _HrToSet--;
            _HrToSet < 0 ? _HrToSet = 12 : _HrToSet = _HrToSet;
          }
        }

        lcd.setCursor(_hrpos, 1);
        if (millis() % 1000 < 500)
        {
          // Blank over
          lcd.print("  ");
        }
        else
        {
          // Print the blinking 2 digit number
          sprintf(_lineBuf, "%02d", pos == Minute ? _MinToSet : _HrToSet);
          lcd.print(_lineBuf);
        }
        break;

      case None:;
        // Check if inputs were set
        if (_HrToSet + _MinToSet > 0)
        {
          // Push them to the clock
          clock.setTime(_MinToSet, _HrToSet);
          _MinToSet = 0;
          _HrToSet = 0;
          clock.setMovementEnabled(true);
        }

      default:
        lcd.setCursor(0, 0);
        lcd.print(F("Set Time    "));
        lcd.setCursor(0, 1);
        sprintf(_lineBuf, "%02d:%02d %02d:%02d (%02d)", getLocalHour(gps.time.hour()), gps.time.minute(), clock.getHour(), clock.getMinute(), gps.time.second());
        lcd.print(_lineBuf);
        break;
      }

      // Accept button inputs
      if (menuBtn.pressed())
      {
        pos++;
        Log.infoln(F("Pos is now %d"), pos);
      }
      break;

    case ManualMode:;
      lcd.setCursor(0, 0);
      lcd.print(F("Manual Mode"));

      // Update manual override
      manualOverride = !upBtn.isPressed();
      clock.setMovementEnabled(true);

      break;

    case LeftHandedNess:;
      lcd.setCursor(0, 0);
      lcd.print(F("Left hand L1L2?"));
      lcd.setCursor(0, 1);
      sprintf(_lineBuf, "isLeftHanded %d", clock.setLeftHanded());
      lcd.print(_lineBuf);

      if (upBtn.pressed() || downBtn.pressed())
      {
        clock.setLeftHanded(true);
      }

      break;

    default:
      /* Not to be confused with Status (the enum) this is the fallthrough, we should never get here! */
      lcd.setCursor(0, 0);
      lcd.print(F("Error: No Menu!"));
      break;
    }

    if (righBtn.pressed())
    {
      menuIdx++;
      lcd.clear();
    }

    if (leftBtn.pressed())
    {
      menuIdx--;
      lcd.clear();
    }

    // Overlays!
    lcd.setCursor(15, 0);
    if (clock.needAdvance() || manualOverride)
    {
      lcd.write((byte)((gps.time.second() % 4) + 2));
    }
    else
    {
      lcd.write((byte)3);
    }

    // Process GPS
    while (ss.available() > 0)
    {
      if (gps.encode(ss.read()))
        ;
      // displayInfo();
    }

    // Other stuff
    // Waterline
    if (uxTaskGetStackHighWaterMark(NULL) < watermark)
    {
      watermark = uxTaskGetStackHighWaterMark(NULL);
      Log.warningln(F("%s: Watermark dropped to %d bytes."), pcTaskGetName(NULL), watermark);
    }

    xTaskDelayUntil(&prevTime, 40 / portTICK_PERIOD_MS);
  }
}
