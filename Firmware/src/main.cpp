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

#include "boardPins.h"
#include "menuEnum.h"
#include "customChars.h"

// Config
static const uint32_t GPSBaud = 9600;

// Objects
SoftwareSerial ss(RX_PIN, TX_PIN);
TinyGPSPlus gps;
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Buttons
Bounce2::Button upBtn = Bounce2::Button();
Bounce2::Button downBtn = Bounce2::Button();
Bounce2::Button leftBtn = Bounce2::Button();
Bounce2::Button righBtn = Bounce2::Button();
Bounce2::Button menuBtn = Bounce2::Button();

// Task Handlers
TaskHandle_t TaskDriver_Handler;
TaskHandle_t TaskInterface_Handler;

// Defs
void displayInfo();
void TaskDriver(void *pvParameters);
void TaskInterface(void *pvParameters);

void setup()
{
  // Pins
  pinMode(DIR_PIN, OUTPUT);
  pinMode(EN_PIN, OUTPUT);

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
  xTaskCreate(
      TaskDriver,           // A pointer to this task in memory
      "Driver",             // A name just for humans
      128,                  // This stack size can be checked & adjusted by reading the Stack Highwater
      NULL,                 // Parameters passed to the task function
      2,                    // Priority, with 2 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      &TaskDriver_Handler); // Task handle

  xTaskCreate(
      TaskInterface, // Handles human interface and serial interface
      "Interface",
      256,
      NULL,
      2,
      &TaskInterface_Handler);
}

void loop()
{
  // Empty. Things are done in Tasks.
  // vTaskDelete(NULL); // Exit task
}

//   /* TODO:
//    * Lots of upgrades have to happen in here, namely:
//    * - Move the CLOCK interface to its own object
//    *  - Need to centralize components like safe power-on-reset (eeprom)
//    *  - Need to ensure that we dont move unless we're 100% sure we can record what we're doing (so we never drift!)
//    *  - RTOS functions like async gps reading, we need to be sure we dont race condition and loose time!
//    */

void TaskDriver(void *pvParameters)
{
  (void)pvParameters;

  // Setup for this task
  pinMode(LED_BUILTIN, OUTPUT);

  // Prev time
  TickType_t prevTime;

  // "Loop"
  for (;;)
  {
    // Blinky
    digitalWrite(LED_BUILTIN, gps.time.second() % 2 == 0);

    // We're going to do this here temporarily at least until we make a new dedicated task for all clock sync items
    digitalWrite(DIR_PIN, gps.time.minute() % 2 == 0); // Set the dir if even/odd
    delay(250);                                        // Delay while dir is latched
    digitalWrite(EN_PIN, gps.time.second() < 10);      // Enable movement if first 10 seconds (Change this so that, on the minute mark, the hands have nearly finished moving)

    xTaskDelayUntil(&prevTime, 60 / portTICK_PERIOD_MS); // Wait to resume
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
  Menu menuIdx = Default;
  char _lineBuf[16];

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
    case Default:;
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
      lcd.setCursor(0, 0);
      lcd.print(F("Set Time"));
      lcd.setCursor(0, 1);
      sprintf(_lineBuf, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
      lcd.print(_lineBuf);
      break;

    case ManualMode:;
      lcd.setCursor(0, 0);
      lcd.print(F("Manual Mode"));
      break;

    default:
      /* Not to be confused with Default (the enum) this is the fallthrough, we should never get here! */
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
    if (true) // todo.. do something with this later..
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