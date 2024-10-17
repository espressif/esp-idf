/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <string.h>
#include <stdatomic.h>
#include <sys/lock.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "soc/soc_caps.h"
#include "soc/debug_probe_periph.h"
#include "hal/debug_probe_ll.h"
#include "esp_private/debug_probe.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"

static const char *TAG = "dbg_probe";

typedef struct debug_probe_unit_t debug_probe_unit_t;
typedef struct debug_probe_channel_t debug_probe_channel_t;

struct debug_probe_unit_t {
    int unit_id; // unit id
    debug_probe_channel_t *channels[DEBUG_PROBE_LL_CHANNELS_PER_UNIT]; // channels installed in this unit
};

struct debug_probe_channel_t {
    debug_probe_unit_t *unit; // the unit this channel belongs to
    int chan_id;              // channel id
};

typedef struct debug_probe_platform_t {
    _lock_t mutex;  // platform level mutex lock
    debug_probe_unit_t *units[SOC_DEBUG_PROBE_NUM_UNIT]; // debug probe units
} debug_probe_platform_t;

static debug_probe_platform_t s_platform; // singleton platform

static esp_err_t debug_probe_unit_destroy(debug_probe_unit_t *unit)
{
    int unit_id = unit->unit_id;

    // remove the unit from the platform
    _lock_acquire(&s_platform.mutex);
    s_platform.units[unit_id] = NULL;
    _lock_release(&s_platform.mutex);

    // disable the probe output
    debug_probe_ll_enable_unit(unit_id, false);
    // free the memory
    free(unit);
    return ESP_OK;
}

esp_err_t debug_probe_new_unit(const debug_probe_unit_config_t *config, debug_probe_unit_handle_t *out_handle)
{
    esp_err_t ret = ESP_OK;
    debug_probe_unit_t *unit = NULL;
    int unit_id = -1;
    ESP_RETURN_ON_FALSE(config && out_handle, ESP_ERR_INVALID_ARG, TAG, "invalid args");

    // search for a free unit slot
    _lock_acquire(&s_platform.mutex);
    for (int i = 0; i < SOC_DEBUG_PROBE_NUM_UNIT; i++) {
        if (s_platform.units[i] == NULL) {
            unit_id = i;
            unit = calloc(1, sizeof(debug_probe_unit_t));
            s_platform.units[i] = unit;
            break;
        }
    }
    _lock_release(&s_platform.mutex);
    ESP_RETURN_ON_FALSE(unit_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free unit slot");
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_NO_MEM, TAG, "no mem for unit");
    unit->unit_id = unit_id;

    // configure the GPIOs
    gpio_config_t monitor_io_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 0,
    };
    for (int i = 0; i < SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH; i++) {
        // skip unused IOs
        if (config->probe_out_gpio_nums[i] < 0) {
            continue;
        }
        monitor_io_conf.pin_bit_mask |= (1ULL << config->probe_out_gpio_nums[i]);
    }
    if (monitor_io_conf.pin_bit_mask) {
        ESP_GOTO_ON_ERROR(gpio_config(&monitor_io_conf), err, TAG, "gpio_config failed");
    }

    // connect the probe output signals to the GPIOs
    for (int i = 0; i < SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH; i++) {
        if (config->probe_out_gpio_nums[i] < 0) {
            continue;
        }
        esp_rom_gpio_connect_out_signal(config->probe_out_gpio_nums[i],
                                        debug_probe_periph_signals.units[unit_id].out_sig[i],
                                        false, false);
    }

    // enable the probe unit
    debug_probe_ll_enable_unit(unit_id, true);

    *out_handle = unit;
    return ESP_OK;

err:
    if (unit) {
        debug_probe_unit_destroy(unit);
    }
    return ret;
}

esp_err_t debug_probe_del_unit(debug_probe_unit_handle_t unit)
{
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    // if there's any channel still being used, return error
    int channel_in_use = -1;
    _lock_acquire(&s_platform.mutex);
    for (int i = 0; i < DEBUG_PROBE_LL_CHANNELS_PER_UNIT; i++) {
        if (unit->channels[i]) {
            channel_in_use = i;
            break;
        }
    }
    _lock_release(&s_platform.mutex);
    ESP_RETURN_ON_FALSE(channel_in_use < 0, ESP_ERR_INVALID_STATE, TAG, "channel %d still in use", channel_in_use);
    return debug_probe_unit_destroy(unit);
}

static esp_err_t debug_probe_channel_destroy(debug_probe_channel_t *chan)
{
    debug_probe_unit_t *unit = chan->unit;
    int unit_id = unit->unit_id;
    int chan_id = chan->chan_id;

    // remove the channel from the unit
    _lock_acquire(&s_platform.mutex);
    unit->channels[chan_id] = NULL;
    _lock_release(&s_platform.mutex);

    // disable the probe channel
    debug_probe_ll_enable_channel(unit_id, chan_id, false);
    // free the memory
    free(chan);
    return ESP_OK;
}

esp_err_t debug_probe_new_channel(debug_probe_unit_handle_t unit, const debug_probe_channel_config_t *config, debug_probe_channel_handle_t *out_handle)
{
    debug_probe_channel_t *chan = NULL;
    int chan_id = -1;
    ESP_RETURN_ON_FALSE(unit && config && out_handle, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    int unit_id = unit->unit_id;

    // search for a free channel slot
    _lock_acquire(&s_platform.mutex);
    for (int i = 0; i < DEBUG_PROBE_LL_CHANNELS_PER_UNIT; i++) {
        if (unit->channels[i] == NULL) {
            chan_id = i;
            chan = calloc(1, sizeof(debug_probe_channel_t));
            unit->channels[i] = chan;
            break;
        }
    }
    _lock_release(&s_platform.mutex);
    ESP_RETURN_ON_FALSE(chan_id >= 0, ESP_ERR_NOT_FOUND, TAG, "no free chan slot");
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_NO_MEM, TAG, "no mem for chan");
    chan->chan_id = chan_id;
    chan->unit = unit;

    // one channel can only monitor one target module
    debug_probe_ll_channel_set_target_module(unit_id, chan_id, config->target_module);
    debug_probe_ll_enable_channel(unit_id, chan_id, true);

    *out_handle = chan;
    return ESP_OK;
}

esp_err_t debug_probe_del_channel(debug_probe_channel_handle_t chan)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    return debug_probe_channel_destroy(chan);
}

esp_err_t debug_probe_chan_add_signal_by_byte(debug_probe_channel_handle_t chan, uint8_t byte_idx, uint8_t sig_group)
{
    ESP_RETURN_ON_FALSE(chan, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    ESP_RETURN_ON_FALSE(byte_idx < 4, ESP_ERR_INVALID_ARG, TAG, "byte_idx out of range");
    ESP_RETURN_ON_FALSE(sig_group < 16, ESP_ERR_INVALID_ARG, TAG, "sig_group out of range");
    debug_probe_unit_t *unit = chan->unit;
    debug_probe_ll_channel_add_signal_group(unit->unit_id, chan->chan_id, byte_idx, sig_group);
    return ESP_OK;
}

esp_err_t debug_probe_unit_merge16(debug_probe_unit_handle_t unit, debug_probe_channel_handle_t chan0, debug_probe_split_u16_t split_of_chan0,
                                   debug_probe_channel_handle_t chan1, debug_probe_split_u16_t split_of_chan1)
{
    ESP_RETURN_ON_FALSE(unit && chan0 && chan1, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    ESP_RETURN_ON_FALSE(chan0->unit == unit && chan1->unit == unit, ESP_ERR_INVALID_ARG, TAG, "chan not belong to unit");
    int unit_id = unit->unit_id;
    debug_probe_ll_set_lower16_output(unit_id, chan0->chan_id, split_of_chan0);
    debug_probe_ll_set_upper16_output(unit_id, chan1->chan_id, split_of_chan1);
    return ESP_OK;
}

esp_err_t debug_probe_unit_read(debug_probe_unit_handle_t unit, uint32_t *value)
{
    ESP_RETURN_ON_FALSE(unit && value, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    int unit_id = unit->unit_id;
    *value = debug_probe_ll_read_output(unit_id);
    return ESP_OK;
}
