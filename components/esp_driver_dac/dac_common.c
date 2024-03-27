/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "soc/soc_caps.h"
#include "soc/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#include "driver/rtc_io.h"
#include "esp_check.h"
#include "dac_priv_common.h"

typedef struct {
    bool          in_use;
    bool          is_enabled;
    const char    *mode;
} dac_channel_info_t;

static dac_channel_info_t s_dac_chan[SOC_DAC_CHAN_NUM] = {
    [0 ... SOC_DAC_CHAN_NUM - 1] = {
        .in_use = false,
        .is_enabled = false,
        .mode = NULL,
    }
};
/* Global dac spin lock for the whole DAC driver */
portMUX_TYPE dac_spinlock = portMUX_INITIALIZER_UNLOCKED;

static const char *TAG = "dac_common";

esp_err_t dac_priv_register_channel(dac_channel_t chan_id, const char *mode_name)
{
    ESP_RETURN_ON_FALSE(chan_id < SOC_DAC_CHAN_NUM, ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");
    DAC_NULL_POINTER_CHECK(mode_name);
    esp_err_t ret = ESP_OK;
    if (!s_dac_chan[chan_id].in_use) {
        s_dac_chan[chan_id].in_use = true;
        s_dac_chan[chan_id].mode = mode_name;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "dac channel %d has been registered by %s", chan_id, s_dac_chan[chan_id].mode);
    }
    return ret;
}

esp_err_t dac_priv_deregister_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(chan_id < SOC_DAC_CHAN_NUM, ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");
    ESP_RETURN_ON_FALSE(!s_dac_chan[chan_id].is_enabled, ESP_ERR_INVALID_STATE, TAG, "the channel is still enabled");
    esp_err_t ret = ESP_OK;
    if (s_dac_chan[chan_id].in_use) {
        s_dac_chan[chan_id].in_use = false;
        s_dac_chan[chan_id].mode = NULL;
    } else {
        ret = ESP_ERR_INVALID_STATE;
    }
    return ret;
}

esp_err_t dac_priv_enable_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(chan_id < SOC_DAC_CHAN_NUM, ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");
    ESP_RETURN_ON_FALSE(s_dac_chan[chan_id].in_use, ESP_ERR_INVALID_STATE, TAG, "the channel is not registered");

    gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[chan_id];
    rtc_gpio_init(gpio_num);
    rtc_gpio_set_direction(gpio_num, RTC_GPIO_MODE_DISABLED);
    rtc_gpio_pullup_dis(gpio_num);
    rtc_gpio_pulldown_dis(gpio_num);
    DAC_RTC_ENTER_CRITICAL();
    dac_ll_power_on(chan_id);
    dac_ll_rtc_sync_by_adc(false);
    DAC_RTC_EXIT_CRITICAL();
    s_dac_chan[chan_id].is_enabled = true;
    return ESP_OK;
}

esp_err_t dac_priv_disable_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(chan_id < SOC_DAC_CHAN_NUM, ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");
    ESP_RETURN_ON_FALSE(s_dac_chan[chan_id].in_use, ESP_ERR_INVALID_STATE, TAG, "the channel is not registered");

    gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[chan_id];
    rtc_gpio_deinit(gpio_num);
    DAC_RTC_ENTER_CRITICAL();
    dac_ll_power_down(chan_id);
    DAC_RTC_EXIT_CRITICAL();
    s_dac_chan[chan_id].is_enabled = false;

    return ESP_OK;
}
