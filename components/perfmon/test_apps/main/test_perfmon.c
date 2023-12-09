/*
 * SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "perfmon.h"
#include "unity.h"

#include "xtensa-debug-module.h"
#include "eri.h"
#include "xtensa_perfmon_access.h"

static const char *TAG = "perfmon";

static void delay(void)
{
    for (int i = 0 ; i < 1000 ; i++) {
        __asm__ __volatile__("nop");
    }
}

TEST_CASE("Start/stop/reset sanity check", "[perfmon]")
{
    xtensa_perfmon_stop();
    xtensa_perfmon_init(0, 0, 0xffff, 0, 6);

    xtensa_perfmon_reset(0);
    delay();
    uint32_t count_0 = eri_read(ERI_PERFMON_PM0);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(0, count_0, "Counter should be 0 after reset");

    xtensa_perfmon_start();
    delay();
    uint32_t count_1 = eri_read(ERI_PERFMON_PM0);
    TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(0, count_1, "Counter should not be 0 after start");

    xtensa_perfmon_stop();
    uint32_t count_2 = eri_read(ERI_PERFMON_PM0);
    delay();
    uint32_t count_3 = eri_read(ERI_PERFMON_PM0);
    TEST_ASSERT_EQUAL_UINT32_MESSAGE(count_2, count_3, "Counter should not change after stop");

    xtensa_perfmon_reset(0);
    xtensa_perfmon_start();
    delay();
    delay();
    uint32_t count_4 = eri_read(ERI_PERFMON_PM0);
    TEST_ASSERT_GREATER_THAN_UINT32_MESSAGE(count_1, count_4, "Counter should be greater when delay is longer");

    xtensa_perfmon_stop();
}

static void test_call(void *params)
{
    delay();
}

static void test_callback(void *params, uint32_t select, uint32_t mask, uint32_t value)
{
    int *call_count = (int *)params;
    ESP_LOGI(TAG, "%s: select=%" PRIu32 ",  mask=%" PRIx32 ", value=%" PRIu32,
             __func__, select, mask, value);
    (*call_count)++;
}

TEST_CASE("xtensa_perfmon_exec custom callback", "[perfmon]")
{
    int num_counters = sizeof(xtensa_perfmon_select_mask_all) / sizeof(xtensa_perfmon_select_mask_all[0]) / 2;
    int callback_call_count = 0;
    xtensa_perfmon_config_t pm_config = {
        .counters_size = num_counters,
        .select_mask = xtensa_perfmon_select_mask_all,
        .repeat_count = 200,
        .max_deviation = 1,
        .call_function = test_call,
        .call_params = NULL,
        .callback = test_callback,
        .callback_params = &callback_call_count,
        .tracelevel = -1
    };
    TEST_ESP_OK(xtensa_perfmon_exec(&pm_config));

    TEST_ASSERT_NOT_EQUAL_MESSAGE(0, callback_call_count, "Callback should be called at least once");
    // Performance counters should not have overflow status set
    TEST_ESP_OK(xtensa_perfmon_overflow(0));
    TEST_ESP_OK(xtensa_perfmon_overflow(1));
    TEST_ASSERT_EQUAL_MESSAGE(num_counters, callback_call_count,
                              "Callback should be called once for every counter");
}

TEST_CASE("xtensa_perfmon_view_cb test", "[perfmon]")
{
    const uint32_t test_table[] = {
        XTPERF_CNT_CYCLES, XTPERF_MASK_CYCLES, // total cycles
        XTPERF_CNT_INSN, XTPERF_MASK_INSN_ALL, // total instructions
        XTPERF_CNT_D_LOAD_U1, XTPERF_MASK_D_LOAD_LOCAL_MEM, // Mem read
        XTPERF_CNT_D_STORE_U1, XTPERF_MASK_D_STORE_LOCAL_MEM, // Mem write
        XTPERF_CNT_BUBBLES, XTPERF_MASK_BUBBLES_ALL &(~XTPERF_MASK_BUBBLES_R_HOLD_REG_DEP),  // wait for other reasons
        XTPERF_CNT_BUBBLES, XTPERF_MASK_BUBBLES_R_HOLD_REG_DEP,           // Wait for register dependency
        XTPERF_CNT_OVERFLOW, XTPERF_MASK_OVERFLOW,               // Last test cycle
    };
    int num_counters = sizeof(test_table) / sizeof(test_table[0]) / 2;

    // We will collect the output of xtensa_perfmon_view_cb in a string
    // and check that the output matches the counters table above.
    char *out_str = NULL;
    size_t out_len = 0;
    FILE *out_stream = open_memstream(&out_str, &out_len);

    xtensa_perfmon_config_t pm_config = {
        .counters_size = num_counters,
        .select_mask = test_table,
        .repeat_count = 200,
        .max_deviation = 1,
        .call_function = test_call,
        .call_params = NULL,
        .callback = xtensa_perfmon_view_cb,
        .callback_params = out_stream,
        .tracelevel = -1,
    };

    TEST_ESP_OK(xtensa_perfmon_exec(&pm_config));
    fclose(out_stream);

    TEST_ASSERT_MESSAGE(strlen(out_str) > 0, "xtensa_perfmon_view_cb should print something");
    // Check that performance counters defined in test_table are present in the output:
    const char *p = out_str;

    // 1. XTPERF_CNT_CYCLES
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Counts cycles.");
    TEST_ASSERT_NOT_NULL(p);

    // 2. XTPERF_CNT_INSN
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Successfully Retired Instructions.");

    // 3. XTPERF_CNT_D_LOAD_U1
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Load Instruction (Data Memory).");

    // 4. XTPERF_CNT_D_STORE_U1
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Store Instruction (Data Memory).");
    TEST_ASSERT_NOT_NULL(p);

    // 5. XTPERF_CNT_BUBBLES
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Hold and Other Bubble cycles.");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "CTI bubble (e.g. branch delay slot)");

    // 6. XTPERF_CNT_BUBBLES (with a different mask)
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Hold and Other Bubble cycles.");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "R hold caused by register dependency");

    // 7. XTPERF_CNT_OVERFLOW
    p = strstr(p, "Value =");
    TEST_ASSERT_NOT_NULL(p);
    p = strstr(p, "Overflow counter");
    TEST_ASSERT_NOT_NULL(p);

    free(out_str);
}
