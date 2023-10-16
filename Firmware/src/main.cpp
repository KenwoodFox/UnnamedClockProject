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

// Config
static const uint32_t GPSBaud = 9600;
static const int mvmtTime = 8 * 1000; // Movement time in ms

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

// Timezone
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
  clock.begin();
  // rtc.begin();

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

// Todo move this somewhere else!
// int getLocalHour()
// {
//   // Adjust the RTC to the current GPS time. (eventually move this to a sync function)
//   rtc.adjust(myTZ.toLocal(DateTime(gps.date.year(), gps.date.month(), gps.date.day(), gps.time.hour(), gps.time.minute(), gps.time.second()).unixtime()));

//   return rtc.now().hour();
// }

void TaskDriver(void *pvParameters)
{
  (void)pvParameters;

  // Begin
  Log.infoln(F("Created %s Task, heap size is %d"), pcTaskGetName(NULL), uxTaskGetStackHighWaterMark(NULL));

  // Setup for this task
  pinMode(LED_BUILTIN, OUTPUT);

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
        xTaskDelayUntil(&prevTime, mvmtTime / portTICK_PERIOD_MS);                         // Resume in x seconds
        clock.next();                                                                      // Auto advance to the next
      }
      else
      {
        clock.move(false, false); // Stop moving
      }
    }

    clock.setTarget(gps.time.minute(), gps.time.hour() % 12); // Set the target

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
  Menu menuIdx = Status;
  char _lineBuf[17]; // Screen is 16 long but null term is extra undrawn byte

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
      // Clock Setting Menu
      lcd.setCursor(0, 0);
      lcd.print(F("Set Time    "));
      lcd.setCursor(0, 1);
      sprintf(_lineBuf, "%02d:%02d %02d:%02d (%02d)", gps.time.hour() % 12, gps.time.minute(), clock.getHour(), clock.getMinute(), gps.time.second());
      lcd.print(_lineBuf);

      lcd.cursor();
      lcd.setCursor(0, 1);
      lcd.noBlink();
      lcd.setCursor(1, 1);
      lcd.noBlink();

      if (upBtn.pressed())
      {
        clock.next();
        clock.setMovementEnabled(true);
      }

      if (downBtn.pressed())
      {
        clock.previous();
        clock.setMovementEnabled(true);
      }
      break;

    case ManualMode:;
      lcd.setCursor(0, 0);
      lcd.print(F("Manual Mode"));

      // Update manual override
      manualOverride = upBtn.isPressed();
      clock.setMovementEnabled(true);

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
    if (clock.needAdvance())
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