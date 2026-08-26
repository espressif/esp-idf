/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "esp_attr.h"
#include "driver/sdm.h"

typedef struct {
    sdm_channel_handle_t chan;
    esp_err_t ret;
} sdm_iram_test_ctx_t;

static void IRAM_ATTR test_set_pulse_density_iram(void *args)
{
    sdm_iram_test_ctx_t *ctx = (sdm_iram_test_ctx_t *)args;
    // can't use TEST_ESP_OK here
    ctx->ret = sdm_channel_set_pulse_density(ctx->chan, 127);
}

TEST_CASE("sdm_works_with_cache_disabled", "[sdm]")
{
    sdm_config_t config = {
        .gpio_num = GPIO_NUM_0,
        .clk_src = SDM_CLK_SRC_DEFAULT,
        .sample_rate_hz = 1000000,
        .flags = {
            .invert_out = false,
            .allow_pd = false,
        },
    };
    sdm_channel_handle_t chan;
    TEST_ESP_OK(sdm_new_channel(&config, &chan));
    TEST_ESP_OK(sdm_channel_set_pulse_density(chan, 0));
    TEST_ESP_OK(sdm_channel_enable(chan));
    vTaskDelay(pdMS_TO_TICKS(500));

    printf("change pulse density in IRAM\r\n");
    sdm_iram_test_ctx_t ctx = {
        .chan = chan,
        .ret = ESP_FAIL,
    };
    unity_utils_run_cache_disable_stub(test_set_pulse_density_iram, &ctx);
    TEST_ESP_OK(ctx.ret);
    vTaskDelay(pdMS_TO_TICKS(500));

    TEST_ESP_OK(sdm_channel_disable(chan));
    TEST_ESP_OK(sdm_del_channel(chan));
}
