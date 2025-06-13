/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This file is only applicable to the touch hardware version2
 *        Version 2 includes ESP32-S2 and ESP32-S3
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
#include "esp_log.h"
#include "esp_check.h"

#define TOUCH_DENOISE_CHAN_ID       0           /*!< The touch channel that can be used as the denoise channel */

static const char *TAG = "touch";

portMUX_TYPE g_touch_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************
 *                       Scope: touch driver private                          *
 ******************************************************************************/

void touch_priv_enable_module(bool enable)
{
    TOUCH_ENTER_CRITICAL(TOUCH_RTC_LOCK);
    touch_ll_enable_clock_gate(enable);
    // Reset the benchmark after finished the scanning
    touch_ll_reset_chan_benchmark(TOUCH_LL_FULL_CHANNEL_MASK);
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
    touch_ll_interrupt_clear(status);
    touch_base_event_data_t data;
    touch_ll_get_active_channel_mask(&data.status_mask);
    uint32_t curr_chan = touch_ll_get_current_meas_channel();
    if (curr_chan == 0) {
        return;
    }
    /* It actually won't be out of range in the real environment, but limit the range to pass the coverity check */
    uint32_t curr_chan_offset = (curr_chan >= SOC_TOUCH_SENSOR_NUM ? SOC_TOUCH_SENSOR_NUM - 1 : curr_chan) - TOUCH_MIN_CHAN_ID;
    data.chan = g_touch->ch[curr_chan_offset];
    /* If the channel is not registered, return directly */
    if (!data.chan) {
        return;
    }
    data.chan_id = data.chan->id;

    if (status & TOUCH_LL_INTR_MASK_DONE) {
#if !SOC_TOUCH_PROXIMITY_MEAS_DONE_SUPPORTED
        /* For the target like ESP32-S2 that don't support proximity done interrupt,
           Simulate the interrupt by software by judge the scan times. */
        if (data.chan->prox_id > 0 &&
                touch_ll_proximity_get_total_scan_times() == touch_ll_proximity_get_curr_scan_cnt(data.chan_id)) {
            /* Set the proximity scan done flag to simulate a proximity done interrupt */
            status |= TOUCH_LL_INTR_MASK_PROX_DONE;
        }
#endif
        if (g_touch->cbs.on_measure_done) {
            need_yield |= g_touch->cbs.on_measure_done(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_SCAN_DONE) {
#if CONFIG_IDF_TARGET_ESP32S2
        /* Workaround for the fake scan done interrupt.
           (Only happens when both channel 13 and 14 are enabled)
           The scan done interrupt will be triggered twice for channel 13 and 14,
           but we only hope it be triggered after channel 14 measurement done. */
        bool fake_scan_done = data.chan_id == 13 && (g_touch->chan_mask >> 13 == 0x03);
        if (g_touch->cbs.on_scan_done && !fake_scan_done)
#else
        if (g_touch->cbs.on_scan_done)
#endif
        {
            need_yield |= g_touch->cbs.on_scan_done(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_PROX_DONE) {
        /* Accumulated proximity sensing data is stored in the benchmark data register.
           Read it out to latch the last proximity sensing data. */
        touch_ll_read_chan_data(data.chan_id, TOUCH_LL_READ_BENCHMARK, &data.chan->prox_val[0]);
        // TODO: support to judge by software if the proximity channel triggered
        if (g_touch->cbs.on_proximity_meas_done) {
            need_yield |= g_touch->cbs.on_proximity_meas_done(g_touch, &data, g_touch->user_ctx);
        }
    }
    if (status & TOUCH_LL_INTR_MASK_ACTIVE) {
        /* When the guard ring activated, disable the scanning of other channels to avoid fake touch */
        TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
        if (g_touch->waterproof_en && data.chan == g_touch->guard_chan) {
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
        if (g_touch->waterproof_en && data.chan == g_touch->guard_chan) {
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
        if ((g_touch->cbs.on_timeout)) {
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

    ESP_RETURN_ON_FALSE(sens_cfg->sample_cfg_num && sens_cfg->sample_cfg, ESP_ERR_INVALID_ARG, TAG,
                        "at least one sample configuration required");
    ESP_RETURN_ON_FALSE(sens_cfg->sample_cfg_num <= TOUCH_SAMPLE_CFG_NUM, ESP_ERR_INVALID_ARG, TAG,
                        "at most %d sample configurations supported", (int)(TOUCH_SAMPLE_CFG_NUM));

    /* Get the source clock frequency for the first time */
    if (!sens_handle->src_freq_hz) {
        /* Touch sensor actually uses dynamic fast clock LP_DYN_FAST_CLK, but it will only switch to the slow clock during sleep,
        * This driver only designed for wakeup case (sleep case should use ULP driver), so we only need to consider RTC_FAST here */
        esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_RTC_FAST, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sens_handle->src_freq_hz);
        ESP_RETURN_ON_FALSE(sens_handle->src_freq_hz, ESP_FAIL, TAG, "Failed to get RTC_FAST clock frequency");
        ESP_LOGD(TAG, "touch rtc clock source: RTC_FAST, frequency: %"PRIu32" Hz", sens_handle->src_freq_hz);
    }
    if (!sens_handle->interval_freq_hz) {
        ESP_RETURN_ON_ERROR(esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_RTC_SLOW, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sens_handle->interval_freq_hz),
                            TAG, "get interval clock frequency failed");
    }

    uint32_t src_freq_mhz = sens_handle->src_freq_hz / 1000000;
    hal_cfg->power_on_wait_ticks = (uint32_t)sens_cfg->power_on_wait_us * src_freq_mhz;
    hal_cfg->power_on_wait_ticks = hal_cfg->power_on_wait_ticks > TOUCH_LL_PAD_MEASURE_WAIT_MAX ?
                                   TOUCH_LL_PAD_MEASURE_WAIT_MAX : hal_cfg->power_on_wait_ticks;
    hal_cfg->meas_interval_ticks = (uint32_t)(sens_cfg->meas_interval_us * sens_handle->interval_freq_hz / 1000000);
    hal_cfg->timeout_ticks = (uint32_t)sens_cfg->max_meas_time_us * src_freq_mhz;
    ESP_RETURN_ON_FALSE(hal_cfg->timeout_ticks <= TOUCH_LL_TIMEOUT_MAX, ESP_ERR_INVALID_ARG, TAG,
                        "max_meas_time_ms should within %"PRIu32" ms", TOUCH_LL_TIMEOUT_MAX / (sens_handle->src_freq_hz / 1000));
    hal_cfg->sample_cfg_num = sens_cfg->sample_cfg_num;  // Only one sample cfg
    hal_cfg->sample_cfg = (touch_hal_sample_config_t *)sens_cfg->sample_cfg;
    return ESP_OK;
}

esp_err_t touch_priv_config_controller(touch_sensor_handle_t sens_handle, const touch_sensor_config_t *sens_cfg)
{
#if CONFIG_TOUCH_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    /* Check and convert the configuration to hal configurations */
    touch_hal_config_t hal_cfg = {};
    ESP_RETURN_ON_ERROR(s_touch_convert_to_hal_config(sens_handle, sens_cfg, &hal_cfg),
                        TAG, "parse the configuration failed due to the invalid configuration");
    sens_handle->sample_cfg_num = 1;  // Only have one set of sampling configuration

    /* Configure the hardware */
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_hal_config_controller(&hal_cfg);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    return ESP_OK;
}

esp_err_t touch_priv_config_channel(touch_channel_handle_t chan_handle, const touch_channel_config_t *chan_cfg)
{
    // Check the validation of the channel active threshold
    ESP_RETURN_ON_FALSE(chan_cfg->active_thresh[0] <= TOUCH_LL_ACTIVE_THRESH_MAX, ESP_ERR_INVALID_ARG,
                        TAG, "the active threshold out of range 0~%d", TOUCH_LL_ACTIVE_THRESH_MAX);
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_ll_set_chan_active_threshold(chan_handle->id, chan_cfg->active_thresh[0]);
    touch_ll_set_charge_speed(chan_handle->id, chan_cfg->charge_speed);
    touch_ll_set_init_charge_voltage(chan_handle->id, chan_cfg->init_charge_volt);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_chan_benchmark_config_t bm_cfg = {.do_reset = true};
    /* Reset the benchmark to overwrite the legacy benchmark during the deep sleep */
    touch_channel_config_benchmark(chan_handle, &bm_cfg);
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
    if (sens_handle->denoise_en) {
        touch_sensor_config_denoise_channel(sens_handle, NULL);
    }
    return ESP_OK;
}

esp_err_t touch_priv_channel_read_data(touch_channel_handle_t chan_handle, touch_chan_data_type_t type, uint32_t *data)
{
    ESP_RETURN_ON_FALSE_ISR(type >= TOUCH_CHAN_DATA_TYPE_RAW && type <= TOUCH_CHAN_DATA_TYPE_PROXIMITY,
                            ESP_ERR_INVALID_ARG, TAG, "The channel data type is invalid");
    ESP_RETURN_ON_FALSE_ISR(type != TOUCH_CHAN_DATA_TYPE_PROXIMITY || chan_handle->prox_id > 0, ESP_ERR_INVALID_ARG, TAG, "This is not a proximity sensing channel");
    TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
    switch (type) {
    default: // fall through
    case TOUCH_CHAN_DATA_TYPE_RAW:
        touch_ll_read_chan_data(chan_handle->id, TOUCH_LL_READ_RAW, data);
        break;
    case TOUCH_CHAN_DATA_TYPE_SMOOTH:
        touch_ll_read_chan_data(chan_handle->id, TOUCH_LL_READ_SMOOTH, data);
        break;
    case TOUCH_CHAN_DATA_TYPE_BENCHMARK:
        touch_ll_read_chan_data(chan_handle->id, TOUCH_LL_READ_BENCHMARK, data);
        break;
    case TOUCH_CHAN_DATA_TYPE_PROXIMITY:
        /* Get the proximity value from the stored data.
        * The proximity value are updated in the isr when proximity scanning is done */
        *data = chan_handle->prox_val[0];
        break;
    }
    TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
    return ESP_OK;
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
    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
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
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_channel_config_benchmark(touch_channel_handle_t chan_handle, const touch_chan_benchmark_config_t *benchmark_cfg)
{
    TOUCH_NULL_POINTER_CHECK_ISR(chan_handle);
    TOUCH_NULL_POINTER_CHECK_ISR(benchmark_cfg);
    if (benchmark_cfg->do_reset) {
        touch_ll_reset_chan_benchmark(BIT(chan_handle->id));
    }
    return ESP_OK;
}

esp_err_t touch_sensor_config_sleep_wakeup(touch_sensor_handle_t sens_handle, const touch_sleep_config_t *sleep_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    int dp_slp_chan_id = -1;
    touch_hal_config_t hal_cfg = {};
    touch_hal_config_t *hal_cfg_ptr = NULL;
    esp_sleep_pd_option_t slp_opt = ESP_PD_OPTION_AUTO;

    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
    TOUCH_GOTO_ON_FALSE_FSM(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    if (sleep_cfg) {
        ESP_GOTO_ON_FALSE(sleep_cfg->slp_wakeup_lvl == TOUCH_LIGHT_SLEEP_WAKEUP || sleep_cfg->slp_wakeup_lvl == TOUCH_DEEP_SLEEP_WAKEUP,
                          ESP_ERR_INVALID_ARG, err, TAG, "Invalid sleep level");
        /* Enabled touch sensor as wake-up source */
        ESP_GOTO_ON_ERROR(esp_sleep_enable_touchpad_wakeup(), err, TAG, "Failed to enable touch sensor wakeup");
        /* If set the deep sleep channel (i.e., enable deep sleep wake-up),
           configure the deep sleep related settings. */
        if (sleep_cfg->slp_wakeup_lvl == TOUCH_DEEP_SLEEP_WAKEUP) {
            if (sleep_cfg->deep_slp_allow_pd) {
                ESP_GOTO_ON_FALSE(sleep_cfg->deep_slp_chan, ESP_ERR_INVALID_ARG, err, TAG,
                                  "deep sleep waken channel can't be NULL when allow RTC power down");
#if CONFIG_IDF_TARGET_ESP32S2
                /* Workaround: In deep sleep, for ESP32S2, Power down the RTC_PERIPH will change the slope configuration of Touch sensor sleep pad.
                 * The configuration change will change the reading of the sleep pad, which will cause the touch wake-up sensor to trigger falsely. */
                slp_opt = ESP_PD_OPTION_ON;
                ESP_LOGW(TAG, "Keep the RTC_PERIPH power on in case the false trigger");
#endif
            } else {
                /* Keep the RTC_PERIPH power domain on in deep sleep */
                slp_opt = ESP_PD_OPTION_ON;
            }
            sens_handle->allow_pd = sleep_cfg->deep_slp_allow_pd;

            /* Check and convert the configuration to hal configurations */
            if (sleep_cfg->deep_slp_sens_cfg) {
                hal_cfg_ptr = &hal_cfg;
                ESP_GOTO_ON_ERROR(s_touch_convert_to_hal_config(sens_handle, sleep_cfg->deep_slp_sens_cfg, hal_cfg_ptr),
                                  err, TAG, "parse the configuration failed due to the invalid configuration");
            }
            sens_handle->deep_slp_chan = sleep_cfg->deep_slp_chan;
            if (sleep_cfg->deep_slp_chan) {
                dp_slp_chan_id = sleep_cfg->deep_slp_chan->id;
            }
            TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
            touch_ll_sleep_set_threshold(sleep_cfg->deep_slp_thresh[0]);
            TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
        } else {
            /* Keep the RTC_PERIPH power domain on in light sleep */
            sens_handle->allow_pd = false;
            slp_opt = ESP_PD_OPTION_ON;
        }
        sens_handle->sleep_en = true;
    } else {
        /* Disable the touch sensor as wake-up source */
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
        if (sens_handle->sleep_en && !sens_handle->allow_pd) {
            /* No longer hold the RTC_PERIPH power by touch sensor */
            slp_opt = ESP_PD_OPTION_OFF;
        }
        sens_handle->allow_pd = false;
        sens_handle->deep_slp_chan = NULL;
        sens_handle->sleep_en = false;
    }

#if SOC_PM_SUPPORT_RTC_PERIPH_PD
    esp_sleep_pd_domain_t pd_domain = ESP_PD_DOMAIN_RTC_PERIPH;
#else
#warning "RTC_PERIPH power domain is not supported"
    esp_sleep_pd_domain_t pd_domain = ESP_PD_DOMAIN_MAX;
#endif  // SOC_PM_SUPPORT_RTC_PERIPH_PD
    ESP_GOTO_ON_ERROR(esp_sleep_pd_config(pd_domain, slp_opt), err, TAG, "Failed to set RTC_PERIPH power domain");

    /* Save or update the sleep config */
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_hal_save_sleep_config(dp_slp_chan_id, hal_cfg_ptr, sens_handle->allow_pd);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

err:
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}

// Water proof can be enabled separately
esp_err_t touch_sensor_config_waterproof(touch_sensor_handle_t sens_handle, const touch_waterproof_config_t *wp_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
    TOUCH_GOTO_ON_FALSE_FSM(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    if (wp_cfg) {
        ESP_GOTO_ON_FALSE(wp_cfg->shield_chan && wp_cfg->shield_chan->id == 14, ESP_ERR_INVALID_ARG, err, TAG, "Shield channel must be channel 14");
        // Check the validation of the waterproof configuration
        TOUCH_NULL_POINTER_CHECK(wp_cfg->shield_chan);
        TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
        sens_handle->waterproof_en = true;
        sens_handle->guard_chan = wp_cfg->guard_chan;
        sens_handle->shield_chan = wp_cfg->shield_chan;
        touch_ll_waterproof_set_guard_chan(wp_cfg->guard_chan ? wp_cfg->guard_chan->id : TOUCH_LL_NULL_CHANNEL);
        // need to disable the scanning of the shield channel
        touch_ll_enable_scan_mask(BIT(wp_cfg->shield_chan->id), false);
        touch_ll_waterproof_set_shield_driver(wp_cfg->shield_drv);
        touch_ll_waterproof_enable(true);
        TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    } else {
        TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
        touch_ll_waterproof_enable(false);
        touch_ll_waterproof_set_guard_chan(TOUCH_LL_NULL_CHANNEL);
        touch_ll_enable_scan_mask(BIT(sens_handle->shield_chan->id), true);
        touch_ll_waterproof_set_shield_driver(0);
        sens_handle->guard_chan = NULL;
        sens_handle->shield_chan = NULL;
        sens_handle->waterproof_en = false;
        TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    }
err:
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_config_proximity_sensing(touch_sensor_handle_t sens_handle, const touch_proximity_config_t *prox_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
    TOUCH_GOTO_ON_FALSE_FSM(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);

    /* Reset proximity sensing part of all channels */
    FOR_EACH_TOUCH_CHANNEL(i) {
        if (sens_handle->ch[i]) {
            sens_handle->ch[i]->prox_id = 0;
            sens_handle->ch[i]->prox_cnt = 0;
            memset(sens_handle->ch[i]->prox_val, 0, sizeof(sens_handle->ch[i]->prox_val[0]) * TOUCH_SAMPLE_CFG_NUM);
        }
    }

    if (prox_cfg) {
        sens_handle->proximity_en = true;
        for (int i = 0; i < TOUCH_PROXIMITY_CHAN_NUM; i++) {
            if (prox_cfg->proximity_chan[i]) {
                prox_cfg->proximity_chan[i]->prox_id = i + 1;
                touch_ll_set_proximity_sensing_channel(i, prox_cfg->proximity_chan[i]->id);
            } else {
                touch_ll_set_proximity_sensing_channel(i, TOUCH_LL_NULL_CHANNEL);
            }
        }
        touch_ll_proximity_set_total_scan_times(prox_cfg->scan_times);
    } else {
        for (int i = 0; i < TOUCH_PROXIMITY_CHAN_NUM; i++) {
            touch_ll_set_proximity_sensing_channel(i, TOUCH_LL_NULL_CHANNEL);
        }
        sens_handle->proximity_en = false;
    }
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    TOUCH_FSM_ERR_TAG(err)
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_config_denoise_channel(touch_sensor_handle_t sens_handle, const touch_denoise_chan_config_t *denoise_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    esp_err_t ret = ESP_OK;
    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
    TOUCH_GOTO_ON_FALSE_FSM(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    if (denoise_cfg) {
        TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
        sens_handle->denoise_en = true;
        sens_handle->chan_mask |= BIT(TOUCH_DENOISE_CHAN_ID);
        touch_ll_set_charge_speed(TOUCH_DENOISE_CHAN_ID, denoise_cfg->charge_speed);
        touch_ll_set_init_charge_voltage(TOUCH_DENOISE_CHAN_ID, denoise_cfg->init_charge_volt);
        touch_ll_denoise_set_reference_cap(denoise_cfg->ref_cap);
        touch_ll_denoise_set_resolution(denoise_cfg->resolution);
        touch_ll_denoise_enable(true);
        TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    } else {
        TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
        sens_handle->denoise_en = false;
        sens_handle->chan_mask &= ~BIT(TOUCH_DENOISE_CHAN_ID);
        touch_ll_denoise_enable(false);
        TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    }
    TOUCH_FSM_ERR_TAG(err)
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}
