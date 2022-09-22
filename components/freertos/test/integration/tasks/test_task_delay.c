/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "unity.h"
#include "freertos_test_utils.h"
#include "portTestMacro.h"

/* ------------------------------------------------------------------------------------------------------------------ */

/*
Test vTaskDelay

Purpose:
    - Test that vTaskDelay is accurate
Procedure:
    - The test contains TEST_VTASKDELAY_ITERATIONS number of iterations. For each iteration...
        - vTaskDelay(1) to align to next tick boundary
        - Store current tick count and current time (using ref clock)
        - vTaskDelay for TEST_VTASKDELAY_TICKS
        - Get post delay tick count and ref clock time
    - For single core, run the test directly from the UnityTask
    - For SMP, run the test once on each core (using vTestOnAllCores())
Expected:
    - The elapsed ticks should be TEST_VTASKDELAY_TICKS, with 1 tick of error allowed (in case ref clock functions last
      long enough to cross a tick boundary).
    - The elapsed time should be equivalent to TEST_VTASKDELAY_TICKS tick periods, with 1 tick period of error allowed
      (in case ref clock functions last longer that a tick period).
*/

#if ( INCLUDE_vTaskDelay == 1 )

#define TEST_VTASKDELAY_TICKS           5   // Number of ticks to delay in test
#define TEST_VTASKDELAY_ITERATIONS      5   // Number of iterations in test

static void test_vTaskDelay(void *arg)
{
    for (int i = 0; i < TEST_VTASKDELAY_ITERATIONS; i++) {
        TickType_t tick_start, tick_end;
        portTEST_REF_CLOCK_TYPE ref_clock_start, ref_clock_end;

        /* Delay until the next tick boundary */
        vTaskDelay(1);

        /* Get the current tick count and ref clock time */
        tick_start = xTaskGetTickCount();
        ref_clock_start = portTEST_REF_CLOCK_GET_TIME();

        vTaskDelay(TEST_VTASKDELAY_TICKS);

        /* Get the post delay tick count and ref clock time */
        tick_end = xTaskGetTickCount();
        ref_clock_end = portTEST_REF_CLOCK_GET_TIME();

        /* Check that elapsed ticks and ref clock is accurate. We allow 1 tick of error in case vTaskDelay() was called
         * right before/after the tick boundary. */
        #if ( configUSE_16_BIT_TICKS == 1 )
            TEST_ASSERT_UINT16_WITHIN(1, TEST_VTASKDELAY_TICKS, tick_end - tick_start);
        #else
            TEST_ASSERT_UINT32_WITHIN(1, TEST_VTASKDELAY_TICKS, tick_end - tick_start);
        #endif
        TEST_ASSERT_UINT32_WITHIN(portTEST_TICKS_TO_REF_CLOCK(1),
                                  portTEST_TICKS_TO_REF_CLOCK(TEST_VTASKDELAY_TICKS),
                                  ref_clock_end - ref_clock_start);
    }
}

TEST_CASE("Tasks: Test vTaskDelay", "[freertos]")
{
    portTEST_REF_CLOCK_INIT();

    #if ( configNUM_CORES > 1 )
        vTestOnAllCores(test_vTaskDelay, NULL, configTEST_DEFAULT_STACK_SIZE, configTEST_UNITY_TASK_PRIORITY - 1);
    #else
        /* Test vTaskDelay directly on the current core */
        test_vTaskDelay(NULL);
    #endif

    portTEST_REF_CLOCK_DEINIT();
}

#endif /* ( INCLUDE_vTaskDelay == 1 ) */

/* ------------------------------------------------------------------------------------------------------------------ */

/*
Test vTaskDelayUntil

Purpose:
    - Test that vTaskDelayUntil is accurate
Procedure:
    - The test contains TEST_VTASKDELAYUNTIL_ITERATIONS number of iterations. For each iteration...
        - vTaskDelay(1) to align to next tick boundary
        - Store current tick count and current time (using ref clock)
        - Call vTaskDelayUntil() for TEST_VTASKDELAYUNTIL_TICKS, using the stored tick count as the previous wake time
        - Get post delay tick count and ref clock time
    - For single core, run the test directly from the UnityTask
    - For SMP, run the test once on each core (using vTestOnAllCores())
Expected:
    - The elapsed ticks should be exactly TEST_VTASKDELAYUNTIL_TICKS since vTaskDelayUntil() is relative to the previous
      wake time
    - The elapsed time should be equivalent to TEST_VTASKDELAYUNTIL_TICKS tick periods, with 1 tick period of error
      allowed (in case ref clock functions last longer that a tick period).
*/

#if ( INCLUDE_xTaskDelayUntil == 1 )

#define TEST_VTASKDELAYUNTIL_TICKS              5   // Number of ticks to delay in test
#define TEST_VTASKDELAYUNTIL_ITERATIONS         5   // Number of iterations in test

static void test_vTaskDelayUntil(void *arg)
{
    /* Delay until the next tick boundary */
    vTaskDelay(1);

    for (int i = 0; i < TEST_VTASKDELAYUNTIL_ITERATIONS; i++) {
        TickType_t tick_start, tick_end, last_wake_tick;
        portTEST_REF_CLOCK_TYPE ref_clock_start, ref_clock_end;

        /* Get the current tick count and ref clock time */
        tick_start = xTaskGetTickCount();
        last_wake_tick = tick_start;
        ref_clock_start = portTEST_REF_CLOCK_GET_TIME();

        vTaskDelayUntil(&last_wake_tick, TEST_VTASKDELAYUNTIL_TICKS);

        /* Get the post delay tick count and ref clock time */
        tick_end = xTaskGetTickCount();
        ref_clock_end = portTEST_REF_CLOCK_GET_TIME();

        /* Check that the elapsed ticks is accurate. Elapsed ticks should be exact as vTaskDelayUntil() executes a
         * delay relative to last_wake_tick. */
        TEST_ASSERT_EQUAL(TEST_VTASKDELAYUNTIL_TICKS, tick_end - tick_start);
        TEST_ASSERT_EQUAL(tick_end, last_wake_tick);

        /* Check that the elapsed ref clock time is accurate. We allow 1 tick time worth of error to account for the
         * the execution time of the ref clock functions. */
        TEST_ASSERT_UINT32_WITHIN(portTEST_TICKS_TO_REF_CLOCK(1),
                                  portTEST_TICKS_TO_REF_CLOCK(TEST_VTASKDELAYUNTIL_TICKS),
                                  ref_clock_end - ref_clock_start);
    }
}

TEST_CASE("Tasks: Test vTaskDelayUntil", "[freertos]")
{
    portTEST_REF_CLOCK_INIT();

    #if ( configNUM_CORES > 1 )
        vTestOnAllCores(test_vTaskDelayUntil, NULL, configTEST_DEFAULT_STACK_SIZE, configTEST_UNITY_TASK_PRIORITY - 1);
    #else
        /* Test vTaskDelay directly on the current core */
        test_vTaskDelayUntil(NULL);
    #endif

    portTEST_REF_CLOCK_DEINIT();
}

#endif /* ( INCLUDE_xTaskDelayUntil == 1 ) */
