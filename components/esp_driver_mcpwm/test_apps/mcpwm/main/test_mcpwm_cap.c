/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "driver/mcpwm_cap.h"
#include "driver/mcpwm_sync.h"
#include "driver/gpio.h"
#include "test_mcpwm_utils.h"

TEST_CASE("mcpwm_capture_install_uninstall", "[mcpwm]")
{
    printf("install mcpwm capture timers\r\n");
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    int total_cap_timers = SOC_MCPWM_GROUPS * SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP;
    mcpwm_cap_timer_handle_t cap_timers[total_cap_timers];
    int k = 0;
    for (int i = 0; i < SOC_MCPWM_GROUPS; i++) {
        cap_timer_config.group_id = i;
        for (int j = 0; j < SOC_MCPWM_CAPTURE_TIMERS_PER_GROUP; j++) {
            TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timers[k++]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_capture_timer(&cap_timer_config, &cap_timers[0]));
    }

    printf("install mcpwm capture channels\r\n");
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = TEST_CAP_GPIO,
        .prescale = 2,
        .flags.pos_edge = true,
    };
    mcpwm_cap_channel_handle_t cap_channels[total_cap_timers][SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER];
    for (int i = 0; i < total_cap_timers; i++) {
        for (int j = 0; j < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; j++) {
            TEST_ESP_OK(mcpwm_new_capture_channel(cap_timers[i], &cap_chan_config, &cap_channels[i][j]));
        }
        TEST_ESP_ERR(ESP_ERR_NOT_FOUND, mcpwm_new_capture_channel(cap_timers[i], &cap_chan_config, &cap_channels[i][0]));
    }

    printf("uninstall mcpwm capture channels and timers\r\n");
    for (int i = 0; i < total_cap_timers; i++) {
        for (int j = 0; j < SOC_MCPWM_CAPTURE_CHANNELS_PER_TIMER; j++) {
            TEST_ESP_OK(mcpwm_del_capture_channel(cap_channels[i][j]));
        }
        TEST_ESP_OK(mcpwm_del_capture_timer(cap_timers[i]));
    }
}

TEST_MCPWM_CALLBACK_ATTR
static bool test_capture_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *edata, void *user_data)
{
    uint32_t *cap_value = (uint32_t *)user_data;
    if (edata->cap_edge == MCPWM_CAP_EDGE_NEG) {
        cap_value[1] = edata->cap_value;
    } else {
        cap_value[0] = edata->cap_value;
    }
    return false;
}

TEST_CASE("mcpwm_capture_ext_gpio", "[mcpwm]")
{
    printf("init a gpio to simulate the external capture signal\r\n");
    const int cap_gpio = TEST_CAP_GPIO;
    gpio_config_t ext_gpio_conf = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = BIT(cap_gpio),
    };
    TEST_ESP_OK(gpio_config(&ext_gpio_conf));

    printf("install mcpwm capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
        .resolution_hz = 8 * 1000 * 1000,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    // put the GPIO into a preset state
    gpio_set_level(cap_gpio, 0);

    printf("install mcpwm capture channel\r\n");
    mcpwm_cap_channel_handle_t pps_channel;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = cap_gpio,
        .prescale = 1,
        .flags.pos_edge = true,
        .flags.neg_edge = true,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &pps_channel));

    printf("install callback for capture channel\r\n");
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_callback,
    };
    uint32_t cap_value[2] = {0};
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(pps_channel, &cbs, cap_value));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(pps_channel));

    printf("check input function before starting capture\r\n");
    gpio_set_level(cap_gpio, 1);
    TEST_ASSERT_EQUAL(1, gpio_get_level(cap_gpio));
    gpio_set_level(cap_gpio, 0);
    TEST_ASSERT_EQUAL(0, gpio_get_level(cap_gpio));

    printf("enable and start capture timer\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    printf("simulate GPIO capture signal\r\n");
    gpio_set_level(cap_gpio, 1);
    esp_rom_delay_us(10 * 1000);
    gpio_set_level(cap_gpio, 0);
    esp_rom_delay_us(10 * 1000);
    printf("capture value: Pos=%"PRIu32", Neg=%"PRIu32"\r\n", cap_value[0], cap_value[1]);
    uint32_t clk_src_res;
    TEST_ESP_OK(mcpwm_capture_timer_get_resolution(cap_timer, &clk_src_res));
    clk_src_res /= 1000; // convert to kHz
    printf("timer resolution:%"PRIu32"KHz\r\n", clk_src_res);
    TEST_ASSERT_UINT_WITHIN(1000, 10000, (cap_value[1] - cap_value[0]) * 1000 / clk_src_res);

    printf("uninstall capture channel and timer\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_disable(pps_channel));
    TEST_ESP_OK(mcpwm_del_capture_channel(pps_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));

    printf("check input function after removing capture\r\n");
    gpio_set_level(cap_gpio, 1);
    TEST_ASSERT_EQUAL(1, gpio_get_level(cap_gpio));
    gpio_set_level(cap_gpio, 0);
    TEST_ASSERT_EQUAL(0, gpio_get_level(cap_gpio));

    TEST_ESP_OK(gpio_reset_pin(cap_gpio));
}

typedef struct {
    uint32_t cap_data[2];
    int cap_data_index;
} test_soft_catch_user_data_t;

TEST_MCPWM_CALLBACK_ATTR
static bool soft_cap_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *data, void *user_data)
{
    test_soft_catch_user_data_t *cbdata = (test_soft_catch_user_data_t *)user_data;
    cbdata->cap_data[cbdata->cap_data_index++] = data->cap_value;
    return false;
}

TEST_CASE("mcpwm_capture_software_catch", "[mcpwm]")
{
    printf("install mcpwm capture timer\r\n");
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    mcpwm_capture_timer_config_t cap_timer_config = {
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
        .group_id = 0,
    };
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    printf("install mcpwm capture channel\r\n");
    mcpwm_cap_channel_handle_t cap_channel = NULL;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = -1, // don't need any GPIO, we use software to trigger a catch
        .prescale = 2,
    };
    test_soft_catch_user_data_t test_callback_data = {};
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel));

    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, mcpwm_capture_channel_trigger_soft_catch(cap_channel));

    printf("register event callback for capture channel\r\n");
    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = soft_cap_callback,
    };
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(cap_channel, &cbs, &test_callback_data));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(cap_channel));

    printf("enable and start capture timer\r\n");
    TEST_ESP_OK(mcpwm_capture_timer_enable(cap_timer));
    TEST_ESP_OK(mcpwm_capture_timer_start(cap_timer));

    printf("trigger software catch\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    esp_rom_delay_us(10 * 1000);
    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    esp_rom_delay_us(10 * 1000);
    // check user data
    TEST_ASSERT_EQUAL(2, test_callback_data.cap_data_index);
    uint32_t delta = test_callback_data.cap_data[1] - test_callback_data.cap_data[0];
    esp_rom_printf("duration=%" PRIu32 " ticks\r\n", delta);
    uint32_t clk_src_res;
    TEST_ESP_OK(mcpwm_capture_timer_get_resolution(cap_timer, &clk_src_res));
    clk_src_res /= 1000; // convert to kHz
    TEST_ASSERT_UINT_WITHIN(1000, 10000, delta * 1000 / clk_src_res);

    printf("uninstall capture channel and timer\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_disable(cap_channel));
    TEST_ESP_OK(mcpwm_capture_timer_disable(cap_timer));
    TEST_ESP_OK(mcpwm_del_capture_channel(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
}

TEST_MCPWM_CALLBACK_ATTR
static bool test_capture_after_sync_callback(mcpwm_cap_channel_handle_t cap_channel, const mcpwm_capture_event_data_t *data, void *user_data)
{
    uint32_t *cap_data = (uint32_t *)user_data;
    *cap_data = data->cap_value;
    return false;
}

TEST_CASE("mcpwm_capture_timer_sync_phase_lock", "[mcpwm]")
{
    mcpwm_capture_timer_config_t cap_timer_config = {
        .group_id = 0,
        .clk_src = MCPWM_CAPTURE_CLK_SRC_DEFAULT,
    };
    mcpwm_cap_timer_handle_t cap_timer = NULL;
    TEST_ESP_OK(mcpwm_new_capture_timer(&cap_timer_config, &cap_timer));

    mcpwm_sync_handle_t soft_sync = NULL;
    mcpwm_soft_sync_config_t soft_sync_config = {};
    TEST_ESP_OK(mcpwm_new_soft_sync_src(&soft_sync_config, &soft_sync));

    mcpwm_capture_timer_sync_phase_config_t sync_config = {
        .count_value = 1000,
        .direction = MCPWM_TIMER_DIRECTION_UP,
        .sync_src = soft_sync,
    };
    TEST_ESP_OK(mcpwm_capture_timer_set_phase_on_sync(cap_timer, &sync_config));

    mcpwm_cap_channel_handle_t cap_channel = NULL;
    mcpwm_capture_channel_config_t cap_chan_config = {
        .gpio_num = -1, // don't need any GPIO, we use software to trigger a catch
        .prescale = 1,
    };
    TEST_ESP_OK(mcpwm_new_capture_channel(cap_timer, &cap_chan_config, &cap_channel));

    mcpwm_capture_event_callbacks_t cbs = {
        .on_cap = test_capture_after_sync_callback,
    };
    uint32_t cap_data;
    TEST_ESP_OK(mcpwm_capture_channel_register_event_callbacks(cap_channel, &cbs, &cap_data));

    printf("enable capture channel\r\n");
    TEST_ESP_OK(mcpwm_capture_channel_enable(cap_channel));

    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    vTaskDelay(pdMS_TO_TICKS(10));
    printf("capture data before sync: %"PRIu32"\r\n", cap_data);

    TEST_ESP_OK(mcpwm_soft_sync_activate(soft_sync));
    TEST_ESP_OK(mcpwm_capture_channel_trigger_soft_catch(cap_channel));
    vTaskDelay(pdMS_TO_TICKS(10));
    printf("capture data after sync: %"PRIu32"\r\n", cap_data);
    TEST_ASSERT_EQUAL(1000, cap_data);
    TEST_ESP_OK(mcpwm_capture_channel_disable(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_channel(cap_channel));
    TEST_ESP_OK(mcpwm_del_capture_timer(cap_timer));
    TEST_ESP_OK(mcpwm_del_sync_src(soft_sync));
}
