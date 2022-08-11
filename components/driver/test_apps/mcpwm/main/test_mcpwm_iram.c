/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "soc/soc_caps.h"
#include "esp_private/esp_clk.h"
#include "esp_private/spi_flash_os.h"
#include "driver/mcpwm_cap.h"
#include "driver/mcpwm_sync.h"
#include "driver/gpio.h"
#include "test_mcpwm_utils.h"

static bool IRAM_ATTR test_capture_callback_iram_safe(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    uint32_t *cap_value = (uint32_t *)user_data;
    if (edata->cap_edge == MCPWM_CAP_EDGE_NEG) {
        cap_value[1] = edata->cap_value;
    } else {
        cap_value[0] = edata->cap_value;
    }
    return false;
}

static void IRAM_ATTR test_mcpwm_capture_gpio_simulate(int gpio_sig)
{
    // disable flash cache
    spi_flash_guard_get()->start();
    gpio_set_level(gpio_sig, 1);
    esp_rom_delay_us(1000);
    gpio_set_level(gpio_sig, 0);
    // enable flash cache
    spi_flash_guard_get()->end();
}

TEST_CASE("mcpwm_capture_iram_safe", "[mcpwm]")
{
    printf("install mcpwm capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_APB,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    const int cap_gpio = 0;
    // put the GPIO into a preset state
    gpio_set_level(cap_gpio, 0);

    printf("install mcpwm capture channel\r\n");
    mcpwm_cap_channel_handle_t pps_channel;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = cap_gpio,
        .prescale = 1,
        .flags.pos_edge = true,
        .flags.neg_edge = true,
        .flags.io_loop_back = true, // so we can use GPIO functions to simulate the external capture signal
        .flags.pull_up = true,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &pps_channel));

    printf("install callback for capture channel\r\n");
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_callback_iram_safe,
    };
    uint32_t cap_value[2] = {0};
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(pps_channel, &cbs, cap_value));

    printf("enable and start capture timer\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    printf("disable cache, simulate GPIO capture signal\r\n");
    test_mcpwm_capture_gpio_simulate(cap_gpio);

    printf("capture value: Pos=%"PRIu32", Neg=%"PRIu32"\r\n", cap_value[0], cap_value[1]);
    // Capture timer is clocked from APB by default
    uint32_t clk_src_res = esp_clk_apb_freq();
    TEST_ASSERT_UINT_WITHIN(2000, clk_src_res / 1000, cap_value[1] - cap_value[0]);

    printf("uninstall capture channel and timer\r\n");
    TEST_ESP_OK(mcpwm_del_capture_channel(pps_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
}
