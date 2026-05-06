/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>
#include "stdatomic.h"
#include "freertos/FreeRTOS.h"
#include "hal/dac_periph.h"
#include "hal/dac_types.h"
#include "hal/dac_ll.h"
#include "esp_private/gpio.h"
#include "esp_check.h"
#include "dac_priv_common.h"

typedef enum {
    DAC_CHAN_FSM_IDLE,
    DAC_CHAN_FSM_REGISTERED,
    DAC_CHAN_FSM_ENABLED,
    DAC_CHAN_FSM_WAIT,        // transition state
} dac_channel_fsm_t;

static _Atomic dac_channel_fsm_t s_dac_chan_fsm[SOC_DAC_CHAN_NUM] = {
    [0 ... SOC_DAC_CHAN_NUM - 1] = DAC_CHAN_FSM_IDLE,
};
/* Global dac spin lock for the whole DAC driver */
portMUX_TYPE dac_spinlock = portMUX_INITIALIZER_UNLOCKED;

static const char *TAG = "dac_common";

esp_err_t dac_priv_register_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_IDLE;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_REGISTERED)) {
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d has been registered", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t dac_priv_deregister_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_REGISTERED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_IDLE)) {
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d is still enabled or not registered", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t dac_priv_enable_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_REGISTERED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_WAIT)) {
        gpio_num_t gpio_num = (gpio_num_t)dac_periph_signal.dac_channel_io_num[chan_id];
        gpio_config_as_analog(gpio_num);
        DAC_RTC_ENTER_CRITICAL();
        dac_ll_power_on(chan_id);
        dac_ll_rtc_sync_by_adc(false);
        DAC_RTC_EXIT_CRITICAL();
        atomic_store(&s_dac_chan_fsm[chan_id], DAC_CHAN_FSM_ENABLED);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d is already enabled or not registered", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}

esp_err_t dac_priv_disable_channel(dac_channel_t chan_id)
{
    ESP_RETURN_ON_FALSE(IS_VALID_DAC_CHANNEL(chan_id), ESP_ERR_INVALID_ARG, TAG, "channel id is invalid");

    dac_channel_fsm_t expected_fsm = DAC_CHAN_FSM_ENABLED;
    if (atomic_compare_exchange_strong(&s_dac_chan_fsm[chan_id], &expected_fsm, DAC_CHAN_FSM_WAIT)) {
        DAC_RTC_ENTER_CRITICAL();
        dac_ll_power_down(chan_id);
        DAC_RTC_EXIT_CRITICAL();
        atomic_store(&s_dac_chan_fsm[chan_id], DAC_CHAN_FSM_REGISTERED);
        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "dac channel %d is not enabled", chan_id);
        return ESP_ERR_INVALID_STATE;
    }
}
