/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "test_utils.h"
#include "unity_config.h"
#include "driver/gptimer.h"

static gptimer_handle_t ts_gptimer;

void ref_clock_init(void)
{
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_XTAL,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1000000, // Resolution is configured to 1MHz
    };
    TEST_ESP_OK(gptimer_new_timer(&timer_config, &ts_gptimer));
    TEST_ESP_OK(gptimer_enable(ts_gptimer));
    TEST_ESP_OK(gptimer_start(ts_gptimer));
}

void ref_clock_deinit(void)
{
    TEST_ESP_OK(gptimer_stop(ts_gptimer));
    TEST_ESP_OK(gptimer_disable(ts_gptimer));
    TEST_ESP_OK(gptimer_del_timer(ts_gptimer));
    ts_gptimer = NULL;
}

uint64_t ref_clock_get(void)
{
    uint64_t ts;
    gptimer_get_raw_count(ts_gptimer, &ts);
    return ts;
}
