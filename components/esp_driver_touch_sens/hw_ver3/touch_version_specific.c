/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This file is only applicable to the touch hardware version3
 *        Version 3 includes ESP32-P4
 */

#include <inttypes.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "soc/touch_sensor_periph.h"
#include "soc/rtc.h"
#include "hal/hal_utils.h"
#include "driver/touch_sens.h"
#include "esp_private/rtc_ctrl.h"
#include "esp_private/periph_ctrl.h"
#include "esp_clk_tree.h"
#include "esp_sleep.h"
#include "../../common/touch_sens_private.h"
#if CONFIG_TOUCH_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_check.h"

static const char *TAG = "touch";

portMUX_TYPE g_touch_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************
 *                       Scope: touch driver private                          *
 ******************************************************************************/

void touch_priv_enable_module(bool enable)
{
    TOUCH_ENTER_CRITICAL(TOUCH_RTC_LOCK);
    touch_ll_enable_module_clock(enable);
    touch_ll_enable_out_gate(enable);
#if SOC_TOUCH_SENSOR_VERSION >= 2
    // Reset the benchmark after finished the scanning
    touch_ll_reset_chan_benchmark(TOUCH_LL_FULL_CHANNEL_MASK);
#endif
    TOUCH_EXIT_CRITICAL(TOUCH_RTC_LOCK);
}

void IRAM_ATTR touch_priv_default_intr_handler(void *arg)
{
    /* If the touch controller object has not been allocated, return directly */
    if (!g_touch) {
        return;
    }
    bool need_yield = false;
    uint32_t status = touch_ll_get_intr_status_mask();
    g_touch->is_meas_timeout = false;
    touch_ll_intr_clear(status);
    touch_base_event_data_t data;
    touch_ll_get_active_channel_mask(&data.status_mask);
    data.chan = g_touch->ch[touch_ll_get_current_meas_channel()];
    /* If the channel is not registered, return directly */
    if (!data.chan) {
        return;
    }
    data.chan_id = data.chan->id;

    if (status & TOUCH_LL_INTR_MASK_DONE) {
        if (g_touch->cbs.on_measure_done) {
            need_yield |= g_touch->cbs.on_measure_done(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_SCAN_DONE) {
        if (g_touch->cbs.on_scan_done) {
            need_yield |= g_touch->cbs.on_scan_done(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_PROX_DONE) {
        data.chan->prox_cnt++;
        /* The proximity sensing result only accurate when the scanning times equal to the sample_cfg_num */
        if (data.chan->prox_cnt == g_touch->sample_cfg_num) {
            data.chan->prox_cnt = 0;
            for (uint32_t i = 0; i < g_touch->sample_cfg_num; i++) {
                touch_ll_read_chan_data(data.chan_id, i, TOUCH_LL_READ_BENCHMARK, &data.chan->prox_val[i]);
            }
            if (g_touch->cbs.on_proximity_meas_done) {
                need_yield |= g_touch->cbs.on_proximity_meas_done(g_touch, &data, g_touch->user_ctx);
            }
        }
    }
    if (status & TOUCH_LL_INTR_MASK_ACTIVE) {
        /* When the guard ring activated, disable the scanning of other channels to avoid fake touch */
        TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        if (g_touch->immersion_proof && data.chan == g_touch->guard_chan) {
            touch_ll_enable_scan_mask(~BIT(data.chan->id), false);
        }
        TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        if (g_touch->cbs.on_active) {
            need_yield |= g_touch->cbs.on_active(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_INACTIVE) {
        /* When the guard ring inactivated, enable the scanning of other channels again */
        TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        if (g_touch->immersion_proof && data.chan == g_touch->guard_chan) {
            touch_ll_enable_scan_mask(g_touch->chan_mask & (~BIT(g_touch->shield_chan->id)), true);
        }
        TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        if (g_touch->cbs.on_inactive) {
            need_yield |= g_touch->cbs.on_inactive(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_TIMEOUT) {
        g_touch->is_meas_timeout = true;
        touch_ll_force_done_curr_measurement();
        if (g_touch->cbs.on_timeout) {
            need_yield |= g_touch->cbs.on_timeout(g_touch, &data, g_touch->user_ctx);
        }
    }

    if (need_yield) {
        portYIELD_FROM_ISR();
    }
}

static esp_err_t s_touch_convert_to_hal_config(touch_sensor_handle_t sens_handle, const touch_sensor_config_t *sens_cfg, touch_hal_config_t *hal_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_cfg);
    TOUCH_NULL_POINTER_CHECK(hal_cfg);
    TOUCH_NULL_POINTER_CHECK(hal_cfg->sample_cfg);
    ESP_RETURN_ON_FALSE(sens_cfg->sample_cfg_num && sens_cfg->sample_cfg, ESP_ERR_INVALID_ARG, TAG,
                        "at least one sample configuration required");
    ESP_RETURN_ON_FALSE(sens_cfg->sample_cfg_num <= TOUCH_SAMPLE_CFG_NUM, ESP_ERR_INVALID_ARG, TAG,
                        "at most %d sample configurations supported", (int)(TOUCH_SAMPLE_CFG_NUM));

    /* Get the source clock frequency for the first time */
    if (!sens_handle->src_freq_hz) {
        /* Touch sensor actually uses dynamic fast clock LP_DYN_FAST_CLK, but it will only switch to the slow clock during sleep,
        * This driver only designed for wakeup case (sleep case should use ULP driver), so we only need to consider RTC_FAST here */
        ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_RTC_FAST, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sens_handle->src_freq_hz),
                            TAG, "get clock frequency failed");
        ESP_LOGD(TAG, "touch rtc clock source: RTC_FAST, frequency: %"PRIu32" Hz", sens_handle->src_freq_hz);
    }

    uint32_t src_freq_hz = sens_handle->src_freq_hz;
    uint32_t src_freq_mhz = src_freq_hz / 1000000;
    hal_cfg->power_on_wait_ticks = (uint32_t)sens_cfg->power_on_wait_us * src_freq_mhz;
    hal_cfg->meas_interval_ticks = (uint32_t)(sens_cfg->meas_interval_us * src_freq_mhz);
    hal_cfg->timeout_ticks = (uint32_t)sens_cfg->max_meas_time_us * src_freq_mhz;
    ESP_RETURN_ON_FALSE(hal_cfg->timeout_ticks <= TOUCH_LL_TIMEOUT_MAX, ESP_ERR_INVALID_ARG, TAG,
                        "max_meas_time_ms should within %"PRIu32, TOUCH_LL_TIMEOUT_MAX / src_freq_mhz);
    hal_cfg->sample_cfg_num = sens_cfg->sample_cfg_num;
    hal_cfg->output_mode = sens_cfg->output_mode;

    for (uint32_t smp_cfg_id = 0; smp_cfg_id < sens_cfg->sample_cfg_num; smp_cfg_id++) {
        const touch_sensor_sample_config_t *sample_cfg = &(sens_cfg->sample_cfg[smp_cfg_id]);
        ESP_RETURN_ON_FALSE(sample_cfg->div_num > 0, ESP_ERR_INVALID_ARG, TAG,
                            "div_num can't be 0");
        /* Assign the hal configurations */
        hal_cfg->sample_cfg[smp_cfg_id].div_num = sample_cfg->div_num;
        hal_cfg->sample_cfg[smp_cfg_id].charge_times = sample_cfg->charge_times;
        hal_cfg->sample_cfg[smp_cfg_id].rc_filter_res = sample_cfg->rc_filter_res;
        hal_cfg->sample_cfg[smp_cfg_id].rc_filter_cap = sample_cfg->rc_filter_cap;
        hal_cfg->sample_cfg[smp_cfg_id].low_drv = sample_cfg->low_drv;
        hal_cfg->sample_cfg[smp_cfg_id].high_drv = sample_cfg->high_drv;
        hal_cfg->sample_cfg[smp_cfg_id].bias_volt = sample_cfg->bias_volt;
    }
    return ESP_OK;
}

esp_err_t touch_priv_config_controller(touch_sensor_handle_t sens_handle, const touch_sensor_config_t *sens_cfg)
{
#if CONFIG_TOUCH_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    /* Check and convert the configuration to hal configurations */
    touch_hal_sample_config_t sample_cfg[TOUCH_SAMPLE_CFG_NUM] = {};
    touch_hal_config_t hal_cfg = {
        .sample_cfg = sample_cfg,
    };
    ESP_RETURN_ON_ERROR(s_touch_convert_to_hal_config(sens_handle, sens_cfg, &hal_cfg),
                        TAG, "parse the configuration failed due to the invalid configuration");
    sens_handle->sample_cfg_num = sens_cfg->sample_cfg_num;

    /* Configure the hardware */
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_hal_config_controller(&hal_cfg);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    return ESP_OK;
}

esp_err_t touch_priv_config_channel(touch_channel_handle_t chan_handle, const touch_channel_config_t *chan_cfg)
{
    uint8_t sample_cfg_num = chan_handle->base->sample_cfg_num;
    // Check the validation of the channel active threshold
    for (uint8_t smp_cfg_id = 0; smp_cfg_id < sample_cfg_num; smp_cfg_id++) {
        ESP_RETURN_ON_FALSE(chan_cfg->active_thresh[smp_cfg_id] <= TOUCH_LL_ACTIVE_THRESH_MAX, ESP_ERR_INVALID_ARG,
                            TAG, "the active threshold out of range 0~%d", TOUCH_LL_ACTIVE_THRESH_MAX);
    }
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    for (uint8_t smp_cfg_id = 0; smp_cfg_id < sample_cfg_num; smp_cfg_id++) {
        touch_ll_set_chan_active_threshold(chan_handle->id, smp_cfg_id, chan_cfg->active_thresh[smp_cfg_id]);
    }
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    return ESP_OK;
}

esp_err_t touch_priv_deinit_controller(touch_sensor_handle_t sens_handle)
{
    /* Disable the additional functions */
    if (sens_handle->proximity_en) {
        touch_sensor_config_proximity_sensing(sens_handle, NULL);
    }
    if (sens_handle->sleep_en) {
        touch_sensor_config_sleep_wakeup(sens_handle, NULL);
    }
    if (sens_handle->waterproof_en) {
        touch_sensor_config_waterproof(sens_handle, NULL);
    }
    return ESP_OK;
}

esp_err_t touch_priv_channel_read_data(touch_channel_handle_t chan_handle, touch_chan_data_type_t type, uint32_t *data)
{
    ESP_RETURN_ON_FALSE_ISR(type >= TOUCH_CHAN_DATA_TYPE_SMOOTH && type <= TOUCH_CHAN_DATA_TYPE_PROXIMITY,
                            ESP_ERR_INVALID_ARG, TAG, "The channel data type is invalid");
#if CONFIG_TOUCH_CTRL_FUNC_IN_IRAM
    ESP_RETURN_ON_FALSE_ISR(esp_ptr_internal(data), ESP_ERR_INVALID_ARG, TAG, "user context not in internal RAM");
#endif
    uint8_t sample_cfg_num = chan_handle->base->sample_cfg_num;
    if (type < TOUCH_CHAN_DATA_TYPE_PROXIMITY) {
        uint32_t internal_type = 0;
        switch (type) {
        default: // fall through
        case TOUCH_CHAN_DATA_TYPE_SMOOTH:
            internal_type = TOUCH_LL_READ_SMOOTH;
            break;
        case TOUCH_CHAN_DATA_TYPE_BENCHMARK:
            internal_type = TOUCH_LL_READ_BENCHMARK;
            break;
        }
        if (type <= TOUCH_CHAN_DATA_TYPE_BENCHMARK) {
            TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
            for (int i = 0; i < sample_cfg_num; i++) {
                touch_ll_read_chan_data(chan_handle->id, i, internal_type, &data[i]);
            }
            TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        }
    } else {
        if (!chan_handle->is_prox_chan) {
            ESP_EARLY_LOGW(TAG, "This is not a proximity sensing channel");
        }
        TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        /* Get the proximity value from the stored data.
         * The proximity value are updated in the isr when proximity scanning is done */
        for (int i = 0; i < sample_cfg_num; i++) {
            data[i] = chan_handle->prox_val[i];
        }
        TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
    }
    return ESP_OK;
}

void touch_priv_config_benchmark(touch_channel_handle_t chan_handle, const touch_chan_benchmark_config_t *benchmark_cfg)
{
    if (benchmark_cfg->do_reset) {
        touch_ll_reset_chan_benchmark(BIT(chan_handle->id));
    }
}

/******************************************************************************
 *                              Scope: public APIs                            *
 ******************************************************************************/

esp_err_t touch_sensor_config_filter(touch_sensor_handle_t sens_handle, const touch_sensor_filter_config_t *filter_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    if (filter_cfg) {
        ESP_RETURN_ON_FALSE(filter_cfg->benchmark.denoise_lvl >= 0 && filter_cfg->benchmark.denoise_lvl <= 4,
                            ESP_ERR_INVALID_ARG, TAG, "denoise_lvl is out of range");
    }

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);

    if (filter_cfg) {
        touch_ll_filter_enable(true);
        /* Configure the benchmark filter and update strategy */
        touch_ll_filter_set_filter_mode(filter_cfg->benchmark.filter_mode);
        if (filter_cfg->benchmark.filter_mode == TOUCH_BM_JITTER_FILTER) {
            touch_ll_filter_set_jitter_step(filter_cfg->benchmark.jitter_step);
        }
        touch_ll_filter_set_denoise_level(filter_cfg->benchmark.denoise_lvl);
        /* Configure the touch data filter */
        touch_ll_filter_set_smooth_mode(filter_cfg->data.smooth_filter);
        touch_ll_filter_set_active_hysteresis(filter_cfg->data.active_hysteresis);
        touch_ll_filter_set_debounce(filter_cfg->data.debounce_cnt);
    } else {
        touch_ll_filter_enable(false);
    }

    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_config_sleep_wakeup(touch_sensor_handle_t sens_handle, const touch_sleep_config_t *sleep_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    int dp_slp_chan_id = -1;
    touch_hal_sample_config_t sample_cfg[TOUCH_SAMPLE_CFG_NUM] = {};
    touch_hal_config_t hal_cfg = {
        .sample_cfg = sample_cfg,
    };
    touch_hal_config_t *hal_cfg_ptr = NULL;

    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);
    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    if (sleep_cfg) {
        ESP_GOTO_ON_FALSE(sleep_cfg->slp_wakeup_lvl == TOUCH_LIGHT_SLEEP_WAKEUP || sleep_cfg->slp_wakeup_lvl == TOUCH_DEEP_SLEEP_WAKEUP,
                          ESP_ERR_INVALID_ARG, err, TAG, "Invalid sleep level");
        /* Enabled touch sensor as wake-up source */
        ESP_GOTO_ON_ERROR(esp_sleep_enable_touchpad_wakeup(), err, TAG, "Failed to enable touch sensor wakeup");
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
        // Keep ESP_PD_DOMAIN_RTC_PERIPH power domain on during the light/deep sleep, so that to keep the touch sensor working
        ESP_GOTO_ON_ERROR(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON), err, TAG, "Failed to keep touch sensor module clock during the sleep");
#endif

        /* If set the deep sleep channel (i.e., enable deep sleep wake-up),
           configure the deep sleep related settings. */
        if (sleep_cfg->slp_wakeup_lvl == TOUCH_DEEP_SLEEP_WAKEUP) {
            ESP_GOTO_ON_FALSE(sleep_cfg->deep_slp_chan, ESP_ERR_INVALID_ARG, err, TAG, "deep sleep waken channel can't be NULL");
            dp_slp_chan_id = sleep_cfg->deep_slp_chan->id;

            /* Check and convert the configuration to hal configurations */
            if (sleep_cfg->deep_slp_sens_cfg) {
                hal_cfg_ptr = &hal_cfg;
                ESP_GOTO_ON_ERROR(s_touch_convert_to_hal_config(sens_handle, (const touch_sensor_config_t *)sleep_cfg->deep_slp_sens_cfg, hal_cfg_ptr),
                                  err, TAG, "parse the configuration failed due to the invalid configuration");
            }
            sens_handle->sleep_en = true;
            sens_handle->deep_slp_chan = sleep_cfg->deep_slp_chan;
            TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
            /* Set sleep threshold */
            for (uint8_t smp_cfg_id = 0; smp_cfg_id < TOUCH_SAMPLE_CFG_NUM; smp_cfg_id++) {
                touch_ll_sleep_set_threshold(smp_cfg_id, sleep_cfg->deep_slp_thresh[smp_cfg_id]);
            }
            TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
        }

    } else {
        /* Disable the touch sensor as wake-up source */
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);
#endif
        sens_handle->deep_slp_chan = NULL;
        sens_handle->sleep_en = false;
    }

    /* Save or update the sleep config */
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_hal_save_sleep_config(dp_slp_chan_id, hal_cfg_ptr);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

// Water proof can be enabled separately
esp_err_t touch_sensor_config_waterproof(touch_sensor_handle_t sens_handle, const touch_waterproof_config_t *wp_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);

    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    if (wp_cfg) {
        // Check the validation of the waterproof configuration
        TOUCH_NULL_POINTER_CHECK(wp_cfg->shield_chan);
        TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
        sens_handle->waterproof_en = true;
        sens_handle->immersion_proof = wp_cfg->flags.immersion_proof;
        sens_handle->guard_chan = wp_cfg->guard_chan;
        sens_handle->shield_chan = wp_cfg->shield_chan;
        touch_ll_waterproof_set_guard_chan(wp_cfg->guard_chan ? wp_cfg->guard_chan->id : TOUCH_LL_NULL_CHANNEL);
        touch_ll_waterproof_set_shield_chan_mask(BIT(wp_cfg->shield_chan->id));
        // need to disable the scanning of the shield channel
        touch_ll_enable_scan_mask(BIT(wp_cfg->shield_chan->id), false);
        touch_ll_waterproof_set_shield_driver(wp_cfg->shield_drv);
        touch_ll_waterproof_enable(true);
        TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    } else {
        TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
        touch_ll_waterproof_enable(false);
        touch_ll_waterproof_set_guard_chan(TOUCH_LL_NULL_CHANNEL);
        touch_ll_waterproof_set_shield_chan_mask(0);
        if (sens_handle->shield_chan) {
            touch_ll_enable_scan_mask(BIT(sens_handle->shield_chan->id), true);
        }
        touch_ll_waterproof_set_shield_driver(0);
        sens_handle->guard_chan = NULL;
        sens_handle->shield_chan = NULL;
        sens_handle->immersion_proof = false;
        sens_handle->waterproof_en = false;
        TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    }
err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_config_proximity_sensing(touch_sensor_handle_t sens_handle, const touch_proximity_config_t *prox_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    xSemaphoreTake(sens_handle->mutex, portMAX_DELAY);

    ESP_GOTO_ON_FALSE(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);

    /* Reset proximity sensing part of all channels */
    FOR_EACH_TOUCH_CHANNEL(i) {
        if (sens_handle->ch[i] && sens_handle->ch[i]->is_prox_chan) {
            sens_handle->ch[i]->is_prox_chan = false;
            sens_handle->ch[i]->prox_cnt = 0;
            for (int i = 0; i < TOUCH_SAMPLE_CFG_NUM; i++) {
                sens_handle->ch[i]->prox_val[i] = 0;
            }
        }
    }

    if (prox_cfg) {
        sens_handle->proximity_en = true;
        uint8_t sample_cfg_num = sens_handle->sample_cfg_num;
        for (int i = 0; i < TOUCH_PROXIMITY_CHAN_NUM; i++) {
            if (prox_cfg->proximity_chan[i]) {
                prox_cfg->proximity_chan[i]->is_prox_chan = true;
                touch_ll_set_proximity_sensing_channel(i, prox_cfg->proximity_chan[i]->id);
            } else {
                touch_ll_set_proximity_sensing_channel(i, TOUCH_LL_NULL_CHANNEL);
            }
        }
        touch_ll_proximity_set_total_scan_times(prox_cfg->scan_times * sample_cfg_num);
        for (uint8_t smp_cfg_id = 0; smp_cfg_id < sample_cfg_num; smp_cfg_id++) {
            touch_ll_proximity_set_charge_times(smp_cfg_id, prox_cfg->charge_times[smp_cfg_id]);
        }
    } else {
        for (int i = 0; i < TOUCH_PROXIMITY_CHAN_NUM; i++) {
            touch_ll_set_proximity_sensing_channel(i, TOUCH_LL_NULL_CHANNEL);
        }
        sens_handle->proximity_en = false;
    }
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

err:
    xSemaphoreGive(sens_handle->mutex);
    return ret;
}
