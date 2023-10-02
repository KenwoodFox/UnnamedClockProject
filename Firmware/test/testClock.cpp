#include <unity.h>
#include <LiquidCrystal.h>

#include "Clock.h"
#include "boardPins.h"

// Objects
Clock clock(EN_PIN, DIR_PIN);
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

/* === Tests === */

/**
 * @brief Tests to make sure that the hour set is the hour read back.
 *
 */
void test_time_set_hour(void)
{
    // Set the current time
    clock.setTime(0, 3);                   // Set to 3:00
    TEST_ASSERT_EQUAL(3, clock.getHour()); // current time should equal 3
}

/**
 * @brief Confirms that clock will advance when the error is in range
 *
 */
void test_advance_in_range(void)
{
    // Set the time and hands
    clock.setTime(0, 3);    // Set to 3:00
    clock.setTarget(30, 3); // Target is 3:30
    TEST_ASSERT_TRUE(clock.needAdvance());
}

/**
 * @brief Confirms that the clock will NOT advance when the error is out of range
 *
 */
void test_advance_out_of_range(void)
{
    // Set the time and hands
    clock.setTime(0, 3);    // Set to 3:00
    clock.setTarget(30, 9); // Target is 9:30
    TEST_ASSERT_FALSE(clock.needAdvance());
}

/**
 * @brief Tests that the clock advance is cyclic (it wraps around)
 *
 */
void test_advance_cyclical(void)
{
    // Set the time and hands
    clock.setTime(59, 11); // Set to 11:59
    clock.setTarget(0, 0); // Target is 12:00
    TEST_ASSERT_TRUE(clock.needAdvance());
}

/**
 * @brief Tests that the LCD works
 *
 */
void test_LCD(void)
{
    // Set the time and hands
    lcd.setCursor(0, 1);

    TEST_ASSERT_GREATER_OR_EQUAL(1, lcd.print(F("Test Mode...")));
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_time_set_hour);
    RUN_TEST(test_advance_in_range);
    RUN_TEST(test_advance_out_of_range);
    RUN_TEST(test_advance_cyclical);
    RUN_TEST(test_LCD);
    return UNITY_END();
}

/* === Test code! === */

/**
 * Arduino simulator setup code
 */
void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    // Setup clock
    clock.begin();

    // Hi
    lcd.begin(16, 2);
    lcd.print(REVISION);
    lcd.setCursor(0, 1);
    lcd.print(F("Testing..."));

    runUnityTests();
}

void loop() {}