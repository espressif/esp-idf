/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "hal/ldo_ll.h"
#include "esp_ldo_regulator.h"

static const char *TAG = "ldo";

typedef struct ldo_regulator_channel_t {
    int chan_id;
    int voltage_mv;
    int ref_cnt;
    struct {
        uint32_t adjustable : 1;
    } flags;
} ldo_regulator_channel_t;

static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;

static const uint32_t s_ldo_channel_adjustable_mask = LDO_LL_ADJUSTABLE_CHAN_MASK; // each bit represents if the LDO channel is adjustable in hardware

static ldo_regulator_channel_t s_ldo_channels[LDO_LL_UNIT_NUM] = {
    [0 ... LDO_LL_UNIT_NUM - 1] = {
        .chan_id = -1,
        .voltage_mv = 0,
        .ref_cnt = 0,
        .flags.adjustable = 1,
    },
};

esp_err_t esp_ldo_acquire_channel(const esp_ldo_channel_config_t *config, esp_ldo_channel_handle_t *out_handle)
{
    ESP_RETURN_ON_FALSE(config, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(ldo_ll_is_valid_ldo_id(config->chan_id), ESP_ERR_INVALID_ARG, TAG, "invalid ldo channel ID");
    ESP_RETURN_ON_FALSE(config->voltage_mv <= LDO_LL_MAX_VOLTAGE_MV,
                        ESP_ERR_INVALID_ARG, TAG, "invalid voltage value: %d", config->voltage_mv);
    int unit_id = LDO_ID2UNIT(config->chan_id);
    ldo_regulator_channel_t *channel = &s_ldo_channels[unit_id];

    bool check_adjustable_constraint_valid = true;
    bool check_voltage_constraint_valid = true;
    portENTER_CRITICAL(&s_spinlock);
    if (config->flags.adjustable) {
        // the user wants to adjust it
        // but the channel is marked as not adjustable
        if (channel->flags.adjustable == 0) {
            check_adjustable_constraint_valid = false;
        } else if (channel->ref_cnt != 0) {
            // or the channel is already in use by others
            // but we don't allow different users to adjust the same LDO, in case they set to different voltages
            // that's to say, if the LDO channel is adjustable, it can only have one reference
            check_adjustable_constraint_valid = false;
        }
    } else {
        // the user doesn't want to adjust the voltage
        // but the channel is already in use by others
        if (channel->ref_cnt != 0) {
            if (channel->flags.adjustable) {
                // we don't allow to have another non-adjustable user
                check_adjustable_constraint_valid = false;
            } else if (channel->voltage_mv != config->voltage_mv) {
                // the voltage is different from us
                check_voltage_constraint_valid = false;
            }
        }
    }
    if (check_voltage_constraint_valid && check_adjustable_constraint_valid) {
        if (channel->ref_cnt == 0) {
            // if the channel is not in use, we need to set the voltage and enable it
            ldo_ll_set_output_voltage_mv(unit_id, config->voltage_mv);
            ldo_ll_enable(unit_id, true);
        }
        // update the channel attributes
        channel->ref_cnt++;
        channel->voltage_mv = config->voltage_mv;
        channel->flags.adjustable = config->flags.adjustable;
        channel->chan_id = config->chan_id;
    }
    portEXIT_CRITICAL(&s_spinlock);

    ESP_RETURN_ON_FALSE(check_voltage_constraint_valid, ESP_ERR_INVALID_ARG, TAG,
                        "can't change the voltage for a non-adjustable channel, expect:%dmV, current:%dmV",
                        config->voltage_mv, channel->voltage_mv);
    ESP_RETURN_ON_FALSE(check_adjustable_constraint_valid, ESP_ERR_INVALID_ARG, TAG,
                        "can't acquire the channel, already in use by others or not adjustable");

    if (out_handle) {
        *out_handle = channel;
    }
    return ESP_OK;
}

esp_err_t esp_ldo_release_channel(esp_ldo_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    int unit_id = LDO_ID2UNIT(chan->chan_id);

    bool is_valid_state = true;
    portENTER_CRITICAL(&s_spinlock);
    if (chan->ref_cnt <= 0) {
        is_valid_state = false;
    } else {
        chan->ref_cnt--;
        if (chan->ref_cnt == 0) {
            // if the channel is not in use, we need to disable it
            ldo_ll_enable(unit_id, false);
            // and reset the ldo voltage
            chan->voltage_mv = 0;
            chan->flags.adjustable = (s_ldo_channel_adjustable_mask & (1 << unit_id)) != 0;
            chan->chan_id = -1;
        }
    }
    portEXIT_CRITICAL(&s_spinlock);

    ESP_RETURN_ON_FALSE(is_valid_state, ESP_ERR_INVALID_STATE, TAG, "LDO channel released too many times");

    return ESP_OK;
}

esp_err_t esp_ldo_channel_adjust_voltage(esp_ldo_channel_handle_t chan, int voltage_mv)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(chan->flags.adjustable, ESP_ERR_NOT_SUPPORTED, TAG, "LDO is not adjustable");
    // check if the voltage is within the valid range
    ESP_RETURN_ON_FALSE(voltage_mv >= LDO_LL_MIN_VOLTAGE_MV && voltage_mv <= LDO_LL_MAX_VOLTAGE_MV,
                        ESP_ERR_INVALID_ARG, TAG, "invalid voltage value: %d", voltage_mv);

    // About Thread Safety:
    // because there won't be more than 1 consumer for the same adjustable LDO channel (guaranteed by esp_ldo_acquire_channel)
    // this function should be thread safe as long as the LDO channel handle is thread safe,
    // i.e., the handle is not shared between threads without mutex protection
    chan->voltage_mv = voltage_mv;
    int unit_id = LDO_ID2UNIT(chan->chan_id);
    ldo_ll_set_output_voltage_mv(unit_id, voltage_mv);

    return ESP_OK;
}

esp_err_t esp_ldo_dump(FILE *stream)
{
    char line[100];
    fprintf(stream, "ESP LDO Channel State:\n");
    fprintf(stream, "%-5s %-5s %-10s %-12s %-5s\n", "Index", "ID", "ref_cnt", "voltage_mv", "adjustable");
    for (int i = 0; i < LDO_LL_UNIT_NUM; i++) {
        char *buf = line;
        size_t len = sizeof(line);
        memset(line, 0x0, len);
        snprintf(buf, len, "%-5d %-5d %-10d %-12d %-5s\n",
                 i,
                 s_ldo_channels[i].chan_id,
                 s_ldo_channels[i].ref_cnt,
                 s_ldo_channels[i].voltage_mv,
                 s_ldo_channels[i].flags.adjustable ? "yes" : "no");
        fputs(line, stream);
    }
    return ESP_OK;
}
