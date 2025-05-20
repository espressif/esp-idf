/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief This file is only applicable to the touch hardware version1
 *        Version 1 includes ESP32
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

static const char *TAG = "touch";

portMUX_TYPE g_touch_spinlock = portMUX_INITIALIZER_UNLOCKED;

/******************************************************************************
 *                       Scope: touch v1 impl private                         *
 ******************************************************************************/
#define TOUCH_DEFAULT_SW_FILTER_COEFF   TOUCH_SMOOTH_IIR_FILTER_2

static uint32_t s_touch_default_sw_iir_filter(touch_channel_handle_t chan_handle, const touch_sw_filter_data_t *filter_data, void *user_filter_ctx)
{
    /* Not used in the default filter algorithm */
    (void)chan_handle;
    (void)user_filter_ctx;
    /* If the previous data is 0, return current value directly */
    if (filter_data->prev_output == 0) {
        return filter_data->curr_input;
    }
    return filter_data->prev_output - (filter_data->prev_output >> TOUCH_DEFAULT_SW_FILTER_COEFF) +
           (filter_data->curr_input >> TOUCH_DEFAULT_SW_FILTER_COEFF);
}

/******************************************************************************
 *                       Scope: touch driver private                          *
 ******************************************************************************/
void touch_priv_execute_sw_filter(void* arg)
{
    touch_sensor_handle_t sens_handle = (touch_sensor_handle_t)arg;
    uint32_t chan_mask = sens_handle->chan_mask;
    /* Read raw data and do the filtering */
    FOR_EACH_TOUCH_CHANNEL(i) {
        touch_channel_handle_t chan_handle = sens_handle->ch[i];
        if (chan_handle) {
            touch_sw_filter_data_t fdata = {
                .prev_output = chan_handle->smooth_data,
            };
            /* Read raw data */
            while (!touch_ll_is_measure_done());
            touch_ll_read_chan_data(chan_handle->id, TOUCH_LL_READ_RAW, &fdata.curr_input);
            /* Do filtering */
            chan_handle->smooth_data = sens_handle->data_filter_fn(chan_handle, &fdata, sens_handle->user_filter_ctx);
        }
        /* Break if no more enabled channels */
        chan_mask >>= 1;
        if (!chan_mask) {
            break;
        }
    }
    chan_mask = sens_handle->chan_mask;
    /* Callback if the channel activated or de-activated */
    FOR_EACH_TOUCH_CHANNEL(i) {
        touch_channel_handle_t chan_handle = sens_handle->ch[i];
        if (chan_handle) {
            /* Prepare basic event data */
            touch_base_event_data_t base_edata = {
                .chan = chan_handle,
                .chan_id = chan_handle->id,
            };
            bool is_below = chan_handle->smooth_data < chan_handle->abs_thresh;
            /* Judge if the channel is active or inactive for now */
            bool is_active_now = sens_handle->is_below_trig ? is_below : !is_below;
            /* Inactive before but now activated */
            if (!chan_handle->is_active && is_active_now) {
                chan_handle->is_active = true;
                if (sens_handle->cbs.on_active) {
                    sens_handle->cbs.on_active(sens_handle, &base_edata, sens_handle->user_ctx);
                }
            }
            /* Active before but now de-activated */
            if (chan_handle->is_active && !is_active_now) {
                chan_handle->is_active = false;
                if (sens_handle->cbs.on_inactive) {
                    sens_handle->cbs.on_inactive(sens_handle, &base_edata, sens_handle->user_ctx);
                }
            }
        }
        /* Break if no more enabled channels */
        chan_mask >>= 1;
        if (!chan_mask) {
            break;
        }
    }
}

void touch_priv_enable_module(bool enable)
{
    // Nothing to do
    // No dedicate touch sensor clock gate or power domain need to operate
}

void IRAM_ATTR touch_priv_default_intr_handler(void *arg)
{
    /* If the touch controller object has not been allocated, return directly */
    if (!g_touch) {
        return;
    }
    bool need_yield = false;
    touch_hw_active_event_data_t data;
    // Only one `on_active` interrupt source, clear directly
    touch_ll_interrupt_clear(TOUCH_LL_INTR_MASK_ALL);
    touch_ll_get_active_channel_mask(&(data.active_mask));
    touch_ll_clear_active_channel_status();
    // Get the activated channels
    if (g_touch->cbs.on_hw_active) {
        need_yield |= g_touch->cbs.on_hw_active(g_touch, &data, g_touch->user_ctx);
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
        esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_RTC_FAST, ESP_CLK_TREE_SRC_FREQ_PRECISION_CACHED, &sens_handle->src_freq_hz);
        ESP_LOGD(TAG, "touch rtc clock source: RTC_FAST, frequency: %"PRIu32" Hz", sens_handle->src_freq_hz);
    }

    uint32_t src_freq_mhz = sens_handle->src_freq_hz / 1000000;
    hal_cfg->power_on_wait_ticks = (uint32_t)sens_cfg->power_on_wait_us * src_freq_mhz;
    hal_cfg->power_on_wait_ticks = hal_cfg->power_on_wait_ticks > TOUCH_LL_PAD_MEASURE_WAIT_MAX ?
                                   TOUCH_LL_PAD_MEASURE_WAIT_MAX : hal_cfg->power_on_wait_ticks;
    hal_cfg->meas_interval_ticks = (uint32_t)(sens_cfg->meas_interval_us * src_freq_mhz);

    sens_handle->is_below_trig = sens_cfg->intr_trig_mode == TOUCH_INTR_TRIG_ON_BELOW_THRESH;
    hal_cfg->intr_trig_mode = sens_cfg->intr_trig_mode;
    hal_cfg->intr_trig_group = sens_cfg->intr_trig_group;
    hal_cfg->sample_cfg_num = sens_cfg->sample_cfg_num;  // Only one sample cfg

    uint32_t charge_duration_ticks = (uint32_t)(sens_cfg->sample_cfg->charge_duration_ms * src_freq_mhz * 1000);
    charge_duration_ticks = charge_duration_ticks > TOUCH_LL_CHARGE_DURATION_MAX ?
                            TOUCH_LL_CHARGE_DURATION_MAX : charge_duration_ticks;
    hal_cfg->sample_cfg->charge_duration_ticks = charge_duration_ticks;
    hal_cfg->sample_cfg->charge_volt_lim_h = sens_cfg->sample_cfg->charge_volt_lim_h;
    hal_cfg->sample_cfg->charge_volt_lim_l = sens_cfg->sample_cfg->charge_volt_lim_l;
    return ESP_OK;
}

esp_err_t touch_priv_config_controller(touch_sensor_handle_t sens_handle, const touch_sensor_config_t *sens_cfg)
{
#if CONFIG_TOUCH_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    /* Check and convert the configuration to hal configurations */
    touch_hal_sample_config_t hal_sample_cfg = {};
    touch_hal_config_t hal_cfg = {
        .sample_cfg = &hal_sample_cfg,
    };
    ESP_RETURN_ON_ERROR(s_touch_convert_to_hal_config(sens_handle, sens_cfg, &hal_cfg),
                        TAG, "parse the configuration failed due to the invalid configuration");
    sens_handle->sample_cfg_num = 1;  // Only have one set of sampling configuration

    /* Configure the hardware */
    assert(hal_cfg.sample_cfg_num == 1);
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_hal_config_controller(&hal_cfg);
    touch_ll_reset_trigger_groups();
    // Reset all channels' activate threshold according to the trigger mode in case of the mis-triggering even the channel is not enabled.
    uint32_t rst_thresh = sens_cfg->intr_trig_mode == TOUCH_INTR_TRIG_ON_BELOW_THRESH ? 0 : TOUCH_LL_ACTIVE_THRESH_MAX;
    FOR_EACH_TOUCH_CHANNEL(i) {
        touch_ll_set_chan_active_threshold(i, rst_thresh);
    }
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

    return ESP_OK;
}

esp_err_t touch_priv_config_channel(touch_channel_handle_t chan_handle, const touch_channel_config_t *chan_cfg)
{
    // Check the validation of the channel active threshold
    ESP_RETURN_ON_FALSE(chan_cfg->abs_active_thresh[0] <= TOUCH_LL_ACTIVE_THRESH_MAX, ESP_ERR_INVALID_ARG,
                        TAG, "the active threshold out of range 0~%d", TOUCH_LL_ACTIVE_THRESH_MAX);
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    chan_handle->abs_thresh = chan_cfg->abs_active_thresh[0];
    touch_ll_set_chan_active_threshold(chan_handle->id, chan_cfg->abs_active_thresh[0]);
    touch_ll_set_charge_speed(chan_handle->id, chan_cfg->charge_speed);
    touch_ll_set_init_charge_voltage(chan_handle->id, chan_cfg->init_charge_volt);
    if (chan_cfg->group & TOUCH_CHAN_TRIG_GROUP_1) {
        touch_ll_config_trigger_group1(chan_handle->id, true);
    }
    if (chan_cfg->group & TOUCH_CHAN_TRIG_GROUP_2) {
        touch_ll_config_trigger_group2(chan_handle->id, true);
    }
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);
    return ESP_OK;
}

esp_err_t touch_priv_deinit_controller(touch_sensor_handle_t sens_handle)
{
    touch_ll_reset_trigger_groups();
    /* Disable the additional functions */
    if (sens_handle->sleep_en) {
        touch_sensor_config_sleep_wakeup(sens_handle, NULL);
    }
    return ESP_OK;
}

esp_err_t touch_priv_channel_read_data(touch_channel_handle_t chan_handle, touch_chan_data_type_t type, uint32_t *data)
{
    ESP_RETURN_ON_FALSE_ISR(type >= TOUCH_CHAN_DATA_TYPE_RAW && type <= TOUCH_CHAN_DATA_TYPE_SMOOTH,
                            ESP_ERR_INVALID_ARG, TAG, "The channel data type is invalid");
    ESP_RETURN_ON_FALSE_ISR(type == TOUCH_CHAN_DATA_TYPE_SMOOTH && chan_handle->base->data_filter_fn != NULL,
                            ESP_ERR_INVALID_STATE, TAG, "The software filter has not configured");
    TOUCH_ENTER_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
    switch (type) {
    default: // fall through
    case TOUCH_CHAN_DATA_TYPE_RAW:
        // Have to wait the measure done, otherwise it might read the un-finished data
        while (!touch_ll_is_measure_done());
        touch_ll_read_chan_data(chan_handle->id, TOUCH_LL_READ_RAW, data);
        break;
    case TOUCH_CHAN_DATA_TYPE_SMOOTH:
        *data = chan_handle->smooth_data;
        break;
    }
    TOUCH_EXIT_CRITICAL_SAFE(TOUCH_PERIPH_LOCK);
    return ESP_OK;
}

/******************************************************************************
 *                              Scope: public APIs                            *
 ******************************************************************************/
esp_err_t touch_sensor_config_sleep_wakeup(touch_sensor_handle_t sens_handle, const touch_sleep_config_t *sleep_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);

    esp_err_t ret = ESP_OK;
    int dp_slp_chan_id = -1;
    touch_hal_sample_config_t hal_sample_cfg = {};
    touch_hal_config_t hal_cfg = {
        .sample_cfg = &hal_sample_cfg,
    };
    touch_hal_config_t *hal_cfg_ptr = NULL;

    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
    TOUCH_GOTO_ON_FALSE_FSM(!sens_handle->is_enabled, ESP_ERR_INVALID_STATE, err, TAG, "Please disable the touch sensor first");

    if (sleep_cfg) {
        ESP_GOTO_ON_FALSE(sleep_cfg->slp_wakeup_lvl == TOUCH_LIGHT_SLEEP_WAKEUP || sleep_cfg->slp_wakeup_lvl == TOUCH_DEEP_SLEEP_WAKEUP,
                          ESP_ERR_INVALID_ARG, err, TAG, "Invalid sleep level");
        /* Enabled touch sensor as wake-up source */
        ESP_GOTO_ON_ERROR(esp_sleep_enable_touchpad_wakeup(), err, TAG, "Failed to enable touch sensor wakeup");
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
        // Keep ESP_PD_DOMAIN_RTC_PERIPH power domain on during the light/deep sleep, so that to keep the touch sensor working
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif  // SOC_PM_SUPPORT_RC_FAST_PD

        /* If set the deep sleep channel (i.e., enable deep sleep wake-up),
           configure the deep sleep related settings. */
        if (sleep_cfg->slp_wakeup_lvl == TOUCH_DEEP_SLEEP_WAKEUP) {
            /* Check and convert the configuration to hal configurations */
            if (sleep_cfg->deep_slp_sens_cfg) {
                hal_cfg_ptr = &hal_cfg;
                ESP_GOTO_ON_ERROR(s_touch_convert_to_hal_config(sens_handle, sleep_cfg->deep_slp_sens_cfg, hal_cfg_ptr),
                                  err, TAG, "parse the configuration failed due to the invalid configuration");
            }
            sens_handle->sleep_en = true;
        }
    } else {
        /* Disable the touch sensor as wake-up source */
        esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TOUCHPAD);
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO);
#endif  // SOC_PM_SUPPORT_RC_FAST_PD
        sens_handle->deep_slp_chan = NULL;
        sens_handle->sleep_en = false;
    }

    /* Save or update the sleep config */
    TOUCH_ENTER_CRITICAL(TOUCH_PERIPH_LOCK);
    touch_hal_save_sleep_config(dp_slp_chan_id, hal_cfg_ptr, false);
    TOUCH_EXIT_CRITICAL(TOUCH_PERIPH_LOCK);

err:
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}

esp_err_t touch_sensor_config_filter(touch_sensor_handle_t sens_handle, const touch_sensor_filter_config_t *filter_cfg)
{
    TOUCH_NULL_POINTER_CHECK(sens_handle);
    esp_err_t ret = ESP_OK;

    xSemaphoreTakeRecursive(sens_handle->mutex, portMAX_DELAY);
    if (filter_cfg) {
        ESP_GOTO_ON_FALSE(filter_cfg->interval_ms, ESP_ERR_INVALID_ARG, err, TAG, "Invalid timer interval");
        if (sens_handle->sw_filter_timer == NULL) {
            esp_timer_create_args_t timer_cfg = {
                .callback = touch_priv_execute_sw_filter,
                .arg = sens_handle,
                /* Choose TASK dispatch because the filter might involve float calculation,
                   and also reading the raw data might need to wait the measurement done */
                .dispatch_method = ESP_TIMER_TASK,
                .name = "touch_v1_sw_filter_timer",
                .skip_unhandled_events = true,
            };
            ESP_GOTO_ON_ERROR(esp_timer_create(&timer_cfg, &(sens_handle->sw_filter_timer)),
                              err, TAG, "Failed to create the sw filter timer");
        }
        sens_handle->timer_interval_ms = filter_cfg->interval_ms;
        /* If user does not specify a filter, use the default filter instead */
        sens_handle->data_filter_fn = filter_cfg->data_filter_fn ?
                                      filter_cfg->data_filter_fn :
                                      s_touch_default_sw_iir_filter;
        sens_handle->user_filter_ctx = filter_cfg->user_filter_ctx;
    } else {
        if (sens_handle->sw_filter_timer) {
            ESP_GOTO_ON_ERROR(esp_timer_delete(sens_handle->sw_filter_timer), err, TAG, "Failed to delete the sw filter timer");
        }
        sens_handle->data_filter_fn = NULL;
        sens_handle->user_filter_ctx = NULL;
        sens_handle->timer_interval_ms = 0;
        FOR_EACH_TOUCH_CHANNEL(i) {
            if (sens_handle->ch[i]) {
                sens_handle->ch[i]->smooth_data = 0;
            }
        }
    }
err:
    xSemaphoreGiveRecursive(sens_handle->mutex);
    return ret;
}
