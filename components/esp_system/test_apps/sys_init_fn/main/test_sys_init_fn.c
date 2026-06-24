/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Test cases for the ESP_SYSTEM_INIT_FN mechanism.
 *
 * The init functions are defined in test_init_fn_defs.c and are executed
 * automatically during startup (before app_main).  These tests inspect the
 * side-effects left by those functions to verify:
 *
 *   1. Init functions actually executed.
 *   2. CORE-stage functions executed before SECONDARY-stage functions.
 *   3. Within the same stage, lower priority values execute first.
 *   4. All expected functions ran (no silent drops).
 */

#include <stdio.h>
#include "unity.h"
#include "unity_test_runner.h"
#include "test_init_fn_defs.h"

/* ---------- Test cases ---------- */

TEST_CASE("CORE init functions executed", "[sys_init_fn]")
{
    TEST_ASSERT_TRUE_MESSAGE(core_prio_200_executed,
                             "CORE priority-200 init function did not execute");
    TEST_ASSERT_TRUE_MESSAGE(core_prio_250_executed,
                             "CORE priority-250 init function did not execute");
}

TEST_CASE("SECONDARY init functions executed", "[sys_init_fn]")
{
    TEST_ASSERT_TRUE_MESSAGE(secondary_prio_200_executed,
                             "SECONDARY priority-200 init function did not execute");
    TEST_ASSERT_TRUE_MESSAGE(secondary_prio_250_executed,
                             "SECONDARY priority-250 init function did not execute");
}

TEST_CASE("all four init functions traced", "[sys_init_fn]")
{
    /* We registered 4 init functions total (2 CORE + 2 SECONDARY) */
    TEST_ASSERT_GREATER_OR_EQUAL_INT_MESSAGE(4, trace_count,
                                             "expected at least 4 init function traces");
}

TEST_CASE("CORE stage runs before SECONDARY stage", "[sys_init_fn]")
{
    /*
     * In the trace log, CORE entries have tags < 1000,
     * SECONDARY entries have tags >= 1000.
     * All CORE entries must appear before any SECONDARY entry.
     */
    int first_secondary = -1;
    int last_core = -1;

    for (int i = 0; i < trace_count; i++) {
        if (trace_log[i] < 1000) {
            last_core = i;
        } else if (first_secondary < 0) {
            first_secondary = i;
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(last_core >= 0,   "no CORE trace entries found");
    TEST_ASSERT_TRUE_MESSAGE(first_secondary >= 0, "no SECONDARY trace entries found");
    TEST_ASSERT_LESS_THAN_INT_MESSAGE(first_secondary, last_core,
                                      "a CORE init function ran after a SECONDARY one");
}

TEST_CASE("priority ordering within CORE stage", "[sys_init_fn]")
{
    /*
     * Within the CORE stage, priority 200 (tag 200) must appear
     * before priority 250 (tag 250) in the trace log.
     */
    int pos_200 = -1;
    int pos_250 = -1;

    for (int i = 0; i < trace_count; i++) {
        if (trace_log[i] == 200 && pos_200 < 0) {
            pos_200 = i;
        }
        if (trace_log[i] == 250 && pos_250 < 0) {
            pos_250 = i;
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(pos_200 >= 0, "CORE prio-200 not in trace");
    TEST_ASSERT_TRUE_MESSAGE(pos_250 >= 0, "CORE prio-250 not in trace");
    TEST_ASSERT_LESS_THAN_INT_MESSAGE(pos_250, pos_200,
                                      "CORE prio-200 did not run before prio-250");
}

TEST_CASE("priority ordering within SECONDARY stage", "[sys_init_fn]")
{
    int pos_1200 = -1;
    int pos_1250 = -1;

    for (int i = 0; i < trace_count; i++) {
        if (trace_log[i] == 1200 && pos_1200 < 0) {
            pos_1200 = i;
        }
        if (trace_log[i] == 1250 && pos_1250 < 0) {
            pos_1250 = i;
        }
    }

    TEST_ASSERT_TRUE_MESSAGE(pos_1200 >= 0, "SECONDARY prio-200 not in trace");
    TEST_ASSERT_TRUE_MESSAGE(pos_1250 >= 0, "SECONDARY prio-250 not in trace");
    TEST_ASSERT_LESS_THAN_INT_MESSAGE(pos_1250, pos_1200,
                                      "SECONDARY prio-200 did not run before prio-250");
}

/* ---------- Entry point ---------- */

void app_main(void)
{
    printf("Running sys_init_fn host test app\n");
    unity_run_menu();
}
