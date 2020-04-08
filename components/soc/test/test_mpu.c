#include <stdio.h>
#include <stdbool.h>
#include "unity.h"

#include "esp_attr.h"

#include "hal/mpu_hal.h"

volatile static int RTC_NOINIT_ATTR access = 0;

static void trigger_illegal_access(void)
{
    access = 0;
    intptr_t addr = 0x60000000;
    volatile int __attribute__((unused)) val = 0;

    val = *((int*) addr);
    ++access;
    TEST_ASSERT_EQUAL(1, access);
    printf("Sucessfully accessed location %p\r\n", (void*)addr);

    mpu_hal_set_region_access(3, MPU_REGION_ILLEGAL); // 0x60000000
    ++access;

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