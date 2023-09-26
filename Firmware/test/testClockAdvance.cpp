#include <unity.h>
#include "Clock.h"
#include "boardPins.h"

// Objects
Clock clock(EN_PIN, DIR_PIN);

void setUp(void)
{
    // set stuff up here
}

void tearDown(void)
{
    // clean stuff up here
}

/* === Tests === */

void test_time_set_hour(void)
{
    // Set the current time
    clock.setTime(0, 3);                   // Set to 3:00
    TEST_ASSERT_EQUAL(3, clock.getHour()); // current time should equal 3
}

void test_advance_in_range(void)
{
    // Set the time and hands
    clock.setTime(0, 3);    // Set to 3:00
    clock.setTarget(30, 3); // Target is 3:30
    TEST_ASSERT_TRUE(clock.needAdvance());
}

void test_advance_out_of_range(void)
{
    // Set the time and hands
    clock.setTime(0, 3);    // Set to 3:00
    clock.setTarget(30, 9); // Target is 9:30
    TEST_ASSERT_FALSE(clock.needAdvance());
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_time_set_hour);
    RUN_TEST(test_advance_in_range);
    RUN_TEST(test_advance_out_of_range);
    return UNITY_END();
}

/* === Test code! === */

/**
 * For Arduino framework
 */
void setup()
{
    // Wait ~2 seconds before the Unity test runner
    // establishes connection with a board Serial interface
    delay(2000);

    // Setup clock
    clock.begin();

    runUnityTests();
}

void loop() {}