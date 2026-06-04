/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
#include "soc/io_mux_reg.h"
#include "driver/rtc_io.h"
#include "driver/lp_io.h"
#include "hal/debug_probe_ll.h"
#include "esp_private/debug_probe.h"
#include "esp_private/gpio.h"
#include "esp_private/esp_gpio_reserve.h"
#include "esp_rom_gpio.h"

ESP_LOG_ATTR_TAG(TAG, "dbg_probe");

typedef struct debug_probe_unit_t debug_probe_unit_t;
typedef struct debug_probe_channel_t debug_probe_channel_t;

struct debug_probe_unit_t {
    debug_probe_unit_id_t unit_id; // unit id
    debug_probe_channel_t *channels[DEBUG_PROBE_LL_CHANNELS_PER_UNIT]; // channels installed in this unit
    uint64_t pin_bit_mask; // bit-mask of the GPIOs used by this unit
    uint32_t probe_out_inv_mask;   // bit i set = invert i-th probe signal
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

static esp_err_t connect_probe_out_to_pin(debug_probe_unit_id_t unit_id, int pin, int sig_idx, bool out_inv)
{
    uint32_t sig_index = debug_probe_periph_signals.units[unit_id].out_sig[sig_idx];
    if (unit_id == DEBUG_PROBE_UNIT_HP) {
        return gpio_matrix_output(pin, sig_index, out_inv, false);
    } else if (unit_id == DEBUG_PROBE_UNIT_LP) {
        ESP_RETURN_ON_ERROR(rtc_gpio_init(pin), TAG, "init RTC GPIO %d failed", pin);
        return lp_gpio_matrix_output(pin, sig_index, out_inv, false);
    } else {
        return ESP_ERR_NOT_SUPPORTED;
    }
}

static esp_err_t debug_probe_unit_destroy(debug_probe_unit_t *unit)
{
    debug_probe_unit_id_t unit_id = unit->unit_id;

    // remove the unit from the platform
    _lock_acquire(&s_platform.mutex);
    s_platform.units[unit_id] = NULL;
    _lock_release(&s_platform.mutex);

    // disable the probe output
    debug_probe_ll_enable_unit(unit_id, false);
    esp_gpio_revoke(unit->pin_bit_mask);
    if (unit_id == DEBUG_PROBE_UNIT_LP) {
        for (uint64_t m = unit->pin_bit_mask; m; m &= m - 1) {
            rtc_gpio_deinit((gpio_num_t)(__builtin_ffsll(m) - 1));
        }
    }
    // free the memory
    free(unit);
    return ESP_OK;
}

esp_err_t debug_probe_new_unit(const debug_probe_unit_config_t *config, debug_probe_unit_handle_t *out_handle)
{
    debug_probe_unit_t *unit = NULL;
    debug_probe_unit_id_t unit_id = config->unit_id;
    ESP_RETURN_ON_FALSE(config && out_handle && unit_id < SOC_DEBUG_PROBE_NUM_UNIT, ESP_ERR_INVALID_ARG, TAG, "invalid args");

    /* Check the requested unit slot is free (unit_id from config->unit_id) */
    _lock_acquire(&s_platform.mutex);
    if (s_platform.units[unit_id] != NULL) {
        _lock_release(&s_platform.mutex);
        ESP_RETURN_ON_FALSE(false, ESP_ERR_NOT_FOUND, TAG, "unit slot in use");
    }
    unit = calloc(1, sizeof(debug_probe_unit_t));
    if (unit) {
        s_platform.units[unit_id] = unit;
    }
    _lock_release(&s_platform.mutex);
    ESP_RETURN_ON_FALSE(unit, ESP_ERR_NO_MEM, TAG, "no mem for unit");
    unit->unit_id = unit_id;

    uint64_t pin_bit_mask = 0;
    for (int i = 0; i < SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH; i++) {
        if (config->probe_out_gpio_nums[i] >= 0) {
            pin_bit_mask |= BIT64(config->probe_out_gpio_nums[i]);
        }
    }
    // reserve the GPIO output path, because we don't expect another peripheral to signal to the same GPIO
    uint64_t old_gpio_rsv_mask = esp_gpio_reserve(pin_bit_mask);
    // check if the GPIO is already used by others
    if (old_gpio_rsv_mask & pin_bit_mask) {
        ESP_LOGW(TAG, "GPIO conflict with others");
    }

    // connect the probe output signals to the GPIOs
    esp_err_t ret = ESP_OK;
    for (int i = 0; i < SOC_DEBUG_PROBE_MAX_OUTPUT_WIDTH; i++) {
        int pin = config->probe_out_gpio_nums[i];
        if (pin >= 0) {
            bool out_inv = !!(config->probe_out_inv_mask & BIT(i));
            ret = connect_probe_out_to_pin(unit_id, pin, i, out_inv);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Probe out pin %d failed: %s", pin, esp_err_to_name(ret));
                break;
            }
        }
    }
    if (ret != ESP_OK) {
        unit->pin_bit_mask = pin_bit_mask;
        (void)debug_probe_unit_destroy(unit);
        return ret;
    }

    unit->pin_bit_mask = pin_bit_mask;
    unit->probe_out_inv_mask = config->probe_out_inv_mask;

    // enable the probe unit
    debug_probe_ll_enable_unit(unit_id, true);

    *out_handle = unit;
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
    debug_probe_unit_id_t unit_id = unit->unit_id;
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
    debug_probe_unit_id_t unit_id = unit->unit_id;

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
    uint8_t target_module = (unit_id == DEBUG_PROBE_UNIT_HP)
                            ? (uint8_t)config->target_module.hp_target
                            : (uint8_t)config->target_module.lp_target;
    debug_probe_ll_channel_set_target_module(unit_id, chan_id, target_module);
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
    debug_probe_unit_t *unit = chan->unit;
    ESP_RETURN_ON_FALSE(byte_idx < 4, ESP_ERR_INVALID_ARG, TAG, "byte_idx out of range");
    ESP_RETURN_ON_FALSE(sig_group < 16, ESP_ERR_INVALID_ARG, TAG, "sig_group out of range");
    debug_probe_ll_channel_add_signal_group(unit->unit_id, chan->chan_id, byte_idx, sig_group);
    return ESP_OK;
}

esp_err_t debug_probe_unit_merge16(debug_probe_unit_handle_t unit, debug_probe_channel_handle_t chan0, debug_probe_split_u16_t split_of_chan0,
                                   debug_probe_channel_handle_t chan1, debug_probe_split_u16_t split_of_chan1)
{
    ESP_RETURN_ON_FALSE(unit && chan0, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    ESP_RETURN_ON_FALSE(chan0->unit == unit, ESP_ERR_INVALID_ARG, TAG, "chan0 not belong to unit");
    if (chan1 != NULL) {
        ESP_RETURN_ON_FALSE(chan1->unit == unit, ESP_ERR_INVALID_ARG, TAG, "chan1 not belong to unit");
    }
    debug_probe_unit_id_t unit_id = unit->unit_id;

    debug_probe_ll_set_lower16_output(unit_id, chan0->chan_id, split_of_chan0);
    if (chan1 != NULL) {
        debug_probe_ll_set_upper16_output(unit_id, chan1->chan_id, split_of_chan1);
    }
    return ESP_OK;
}

esp_err_t debug_probe_unit_read(debug_probe_unit_handle_t unit, uint32_t *value)
{
    ESP_RETURN_ON_FALSE(unit && value, ESP_ERR_INVALID_ARG, TAG, "invalid args");
    debug_probe_unit_id_t unit_id = unit->unit_id;
    uint32_t raw = debug_probe_ll_read_output(unit_id);
    *value = raw ^ unit->probe_out_inv_mask;
    return ESP_OK;
}
