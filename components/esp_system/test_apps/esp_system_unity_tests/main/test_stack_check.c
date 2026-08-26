/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "unity.h"

#if CONFIG_COMPILER_STACK_CHECK

static void recur_and_smash(void)
{
    static int cnt;
    volatile uint8_t buf[50];
    volatile int num = sizeof(buf) + 10;

    if (cnt++ < 1) {
        recur_and_smash();
    }
    for (int i = 0; i < num; i++) {
        buf[i] = 0;
    }
}

/* Test is marked as ignore, since we cannot run it as a regular unity test case
   Instead we run it in a seperate test case and check output with pytest,
   see test_stack_smash_protection in the pytest file.
 */
TEST_CASE("stack smashing protection", "[stack_check] [ignore]")
{
    recur_and_smash();
}

#endif
