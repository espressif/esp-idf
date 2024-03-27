/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "unity.h"
#include "esp_intr_alloc.h"

#if defined(__XTENSA__) && CONFIG_FREERTOS_CORETIMER_0
#include "xtensa/config/core-isa.h"
#include "xtensa/hal.h"
#if defined(XCHAL_HAVE_WINDOWED)
/* Regression test for a0 register being corrupted in _xt_context_save.
 *
 * The idea in this test is to have a function which recursively calls itself
 * with call4, eventually filling up all the register windows. At that point,
 * it does some lengthy operation. If an interrupt occurs at that point, and
 * corrupts a0 register of one of the windows, this will cause an exception
 * when the recursive function returns.
 */

/* See test_context_save_clober_func.S */
extern void test_context_save_clober_func(void);

static void int_timer_handler(void *arg)
{
    xthal_set_ccompare(1, xthal_get_ccount() + 10000);
    (*(int*) arg)++;
}

TEST_CASE("context save doesn't corrupt return address register", "[freertos]")
{
    /* set up an interrupt */
    intr_handle_t ih;
    int int_triggered = 0;
    TEST_ESP_OK(esp_intr_alloc(ETS_INTERNAL_TIMER1_INTR_SOURCE, 0, int_timer_handler, &int_triggered, &ih));
    xthal_set_ccompare(1, xthal_get_ccount() + 10000);

    /* fill all the windows and delay a bit, waiting for an interrupt to happen */
    test_context_save_clober_func();

    esp_intr_free(ih);
    TEST_ASSERT_GREATER_THAN(0, int_triggered);
}

#endif // XCHAL_HAVE_WINDOWED
#endif // __XTENSA__ && CONFIG_FREERTOS_CORETIMER_0
