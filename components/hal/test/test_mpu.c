#include <stdio.h>
#include <stdbool.h>
#include "unity.h"

#include "esp_attr.h"

#include "hal/mpu_hal.h"

// TODO ESP32-C3 IDF-2375
// LL still not implemented

#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C3)
#if !TEMPORARY_DISABLED_FOR_TARGETS(ESP32C2, ESP32C6, ESP32H2)
//IDF-5058

volatile static int RTC_NOINIT_ATTR access = 0;

static void trigger_illegal_access(void)
{
    access = 0;
    intptr_t addr = 0x80000000; // MPU region 4
    volatile int __attribute__((unused)) val = 0;

    // Marked as an illegal access region at startup in ESP32, ESP32S2.
    // Make accessible temporarily.
    mpu_hal_set_region_access(4, MPU_REGION_RW);

    val = *((int*) addr);
    ++access;
    TEST_ASSERT_EQUAL(1, access);
    printf("Sucessfully accessed location %p\r\n", (void*)addr);

    // Make access to region illegal again.
    mpu_hal_set_region_access(4, MPU_REGION_ILLEGAL);
    ++access;

    // Since access to region is illegal, this should fail (causing a reset), and the increment
    // to access count is not performed.
    val = *((int*) addr);
    ++access;
}

void check_access(void)
{
    TEST_ASSERT_EQUAL(2, access);
}

TEST_CASE_MULTIPLE_STAGES("Can set illegal access regions", "[soc][mpu]",
        trigger_illegal_access,
        check_access);

#endif //!TEMPORARY_DISABLED_FOR_TARGETS(...)
#endif //!TEMPORARY_DISABLED_FOR_TARGETS(ESP32C3)
