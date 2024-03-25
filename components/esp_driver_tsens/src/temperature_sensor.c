/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "sdkconfig.h"
#if CONFIG_TEMP_SENSOR_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif
#include "esp_log.h"
#include "sys/lock.h"
#include "soc/rtc.h"
#include "esp_check.h"
#include "esp_types.h"
#include "esp_heap_caps.h"
#include "clk_ctrl_os.h"
#include "freertos/FreeRTOS.h"
#include "driver/temperature_sensor.h"
#include "esp_efuse_rtc_calib.h"
#include "esp_private/periph_ctrl.h"
#include "temperature_sensor_private.h"
#include "hal/temperature_sensor_ll.h"
#include "soc/temperature_sensor_periph.h"
#include "esp_memory_utils.h"
#include "esp_private/sar_periph_ctrl.h"

static const char *TAG = "temperature_sensor";

static float s_deltaT = NAN; // unused number

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
static int8_t s_temperature_regval_2_celsius(temperature_sensor_handle_t tsens, uint8_t regval);
#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

static temperature_sensor_attribute_t *s_tsens_attribute_copy;

static int inline accuracy_compare(const void *p1, const void *p2)
{
    return ((*(temperature_sensor_attribute_t *)p1).error_max < (*(temperature_sensor_attribute_t *)p2).error_max) ? -1 : 1;
}

static esp_err_t temperature_sensor_attribute_table_sort(void)
{
    s_tsens_attribute_copy = (temperature_sensor_attribute_t *)heap_caps_malloc(sizeof(temperature_sensor_attributes), TEMPERATURE_SENSOR_MEM_ALLOC_CAPS);
    ESP_RETURN_ON_FALSE(s_tsens_attribute_copy != NULL, ESP_ERR_NO_MEM, TAG, "No space for s_tsens_attribute_copy");
    for (int i = 0 ; i < TEMPERATURE_SENSOR_ATTR_RANGE_NUM; i++) {
        s_tsens_attribute_copy[i] = temperature_sensor_attributes[i];
    }
    // Sort from small to large by error_max.
    qsort(s_tsens_attribute_copy, TEMPERATURE_SENSOR_ATTR_RANGE_NUM, sizeof(s_tsens_attribute_copy[0]), accuracy_compare);
    return ESP_OK;
}

static esp_err_t temperature_sensor_choose_best_range(temperature_sensor_handle_t tsens, const temperature_sensor_config_t *tsens_config)
{
    for (int i = 0 ; i < TEMPERATURE_SENSOR_ATTR_RANGE_NUM; i++) {
        if ((tsens_config->range_min >= s_tsens_attribute_copy[i].range_min) && (tsens_config->range_max <= s_tsens_attribute_copy[i].range_max)) {
            tsens->tsens_attribute = &s_tsens_attribute_copy[i];
            break;
        }
        temp_sensor_sync_tsens_idx(i);
    }
    ESP_RETURN_ON_FALSE(tsens->tsens_attribute != NULL, ESP_ERR_INVALID_ARG, TAG, "Out of testing range");
    return ESP_OK;
}

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
static void IRAM_ATTR temperature_sensor_isr(void *arg)
{
    temperature_sensor_ll_clear_intr();
    bool cbs_yield = false;
    temperature_sensor_handle_t tsens = (temperature_sensor_handle_t) arg;
    temperature_val_intr_condition_t intr_condition = (temperature_sensor_ll_get_wakeup_reason() == 1 ? TEMPERATURE_VAL_HIGHER_THAN_HIGH_THRESHOLD : TEMPERATURE_VAL_LOWER_THAN_LOW_THRESHOLD);
    temperature_sensor_threshold_event_data_t data = {
        .celsius_value = s_temperature_regval_2_celsius(tsens, temperature_sensor_ll_get_raw_value()),
        .intr_condition = intr_condition,
    };
    if (tsens->threshold_cbs) {
        if (tsens->threshold_cbs(tsens, &data, tsens->cb_user_arg)) {
            cbs_yield = true;
        }
    }

    if (cbs_yield) {
        portYIELD_FROM_ISR();
    }
}
#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

esp_err_t temperature_sensor_install(const temperature_sensor_config_t *tsens_config, temperature_sensor_handle_t *ret_tsens)
{
#if CONFIG_TEMP_SENSOR_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((tsens_config && ret_tsens), ESP_ERR_INVALID_ARG, TAG, "Invalid argument");
    ESP_RETURN_ON_FALSE((s_tsens_attribute_copy == NULL), ESP_ERR_INVALID_STATE, TAG, "Already installed");
    temperature_sensor_handle_t tsens = NULL;
    tsens = (temperature_sensor_obj_t *) heap_caps_calloc(1, sizeof(temperature_sensor_obj_t), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_NO_MEM, TAG, "no mem for temp sensor");
    if (tsens->clk_src == 0) {
        tsens->clk_src = TEMPERATURE_SENSOR_CLK_SRC_DEFAULT;
    } else {
        tsens->clk_src = tsens_config->clk_src;
    }

    temperature_sensor_power_acquire();
    temperature_sensor_ll_clk_sel(tsens->clk_src);

    ESP_GOTO_ON_ERROR(temperature_sensor_attribute_table_sort(), err, TAG, "Table sort failed");
    ESP_GOTO_ON_ERROR(temperature_sensor_choose_best_range(tsens, tsens_config), err, TAG, "Cannot select the correct range");

    ESP_LOGI(TAG, "Range [%d°C ~ %d°C], error < %d°C",
             tsens->tsens_attribute->range_min,
             tsens->tsens_attribute->range_max,
             tsens->tsens_attribute->error_max);

    temperature_sensor_ll_set_range(tsens->tsens_attribute->reg_val);

    tsens->fsm = TEMP_SENSOR_FSM_INIT;
    *ret_tsens = tsens;
    return ESP_OK;
err:
    temperature_sensor_uninstall(tsens);
    return ret;
}

esp_err_t temperature_sensor_uninstall(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "tsens not in init state");

    if (s_tsens_attribute_copy) {
        free(s_tsens_attribute_copy);
    }
    s_tsens_attribute_copy = NULL;

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
    temperature_sensor_ll_enable_intr(false);
    if (tsens->temp_sensor_isr_handle) {
        ESP_RETURN_ON_ERROR(esp_intr_free(tsens->temp_sensor_isr_handle), TAG, "uninstall interrupt service failed");
    }
#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
    temperature_sensor_power_release();

    free(tsens);
    return ESP_OK;
}

static esp_err_t s_update_tsens_attribute(temperature_sensor_handle_t tsens)
{
    uint32_t dac;
    ESP_RETURN_ON_FALSE(tsens != NULL, ESP_ERR_INVALID_ARG, TAG, "no tsens specified");
    dac = temperature_sensor_ll_get_offset();
    for (int i = 0 ; i < TEMPERATURE_SENSOR_ATTR_RANGE_NUM; i++) {
        if (dac == s_tsens_attribute_copy[i].reg_val) {
            tsens->tsens_attribute = &s_tsens_attribute_copy[i];
            break;
        }
    }
    return ESP_OK;
}

esp_err_t temperature_sensor_enable(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "tsens not in init state");

#if SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC
    if (tsens->clk_src == TEMPERATURE_SENSOR_CLK_SRC_RC_FAST) {
        periph_rtc_dig_clk8m_enable();
    }
#endif

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
    temperature_sensor_ll_wakeup_enable(true);
    temperature_sensor_ll_sample_enable(true);
#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

    // After enabling/resetting the temperature sensor,
    // the output value gradually approaches the true temperature
    // value as the measurement time increases. 300us is recommended.
    esp_rom_delay_us(300);
    tsens->fsm = TEMP_SENSOR_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t temperature_sensor_disable(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE(tsens, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "tsens not enabled yet");

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
    temperature_sensor_ll_wakeup_enable(false);
    temperature_sensor_ll_sample_enable(false);
#endif

#if SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC
    if (tsens->clk_src == TEMPERATURE_SENSOR_CLK_SRC_RC_FAST) {
        periph_rtc_dig_clk8m_disable();
    }
#endif

    tsens->fsm = TEMP_SENSOR_FSM_INIT;
    return ESP_OK;
}

static esp_err_t read_delta_t_from_efuse(void)
{
    if (esp_efuse_rtc_calib_get_tsens_val(&s_deltaT) != ESP_OK) {
        ESP_LOGW(TAG, "Calibration failed");
    }
    ESP_LOGD(TAG, "s_deltaT = %f", s_deltaT);
    return ESP_OK;
}

static float parse_temp_sensor_raw_value(int16_t tsens_raw)
{
    if (isnan(s_deltaT)) { //suggests that the value is not initialized
        read_delta_t_from_efuse();
    }
    float result = tsens_raw - s_deltaT / 10.0;
    return result;
}

esp_err_t temperature_sensor_get_celsius(temperature_sensor_handle_t tsens, float *out_celsius)
{
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Has not been installed");
    ESP_RETURN_ON_FALSE(out_celsius != NULL, ESP_ERR_INVALID_ARG, TAG, "Celsius points to nothing");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "tsens not enabled yet");
    bool range_changed;
    int16_t tsens_out = temp_sensor_get_raw_value(&range_changed);
    *out_celsius = parse_temp_sensor_raw_value(tsens_out);

    if (*out_celsius < TEMPERATURE_SENSOR_LL_MEASURE_MIN || *out_celsius > TEMPERATURE_SENSOR_LL_MEASURE_MAX) {
        ESP_LOGE(TAG, "Exceeding temperature measure range.");
        return ESP_ERR_INVALID_STATE;
    }
    if (range_changed) {
        s_update_tsens_attribute(tsens);
    }
    return ESP_OK;
}

#if SOC_TEMPERATURE_SENSOR_INTR_SUPPORT

static uint8_t s_temperature_celsius_2_regval(temperature_sensor_handle_t tsens, int8_t celsius)
{
    return (uint8_t)((celsius + TEMPERATURE_SENSOR_LL_OFFSET_FACTOR + TEMPERATURE_SENSOR_LL_DAC_FACTOR * tsens->tsens_attribute->offset) / TEMPERATURE_SENSOR_LL_ADC_FACTOR);
}

IRAM_ATTR static int8_t s_temperature_regval_2_celsius(temperature_sensor_handle_t tsens, uint8_t regval)
{
    return TEMPERATURE_SENSOR_LL_ADC_FACTOR * regval - TEMPERATURE_SENSOR_LL_DAC_FACTOR * tsens->tsens_attribute->offset - TEMPERATURE_SENSOR_LL_OFFSET_FACTOR;
}

esp_err_t temperature_sensor_set_absolute_threshold(temperature_sensor_handle_t tsens, const temperature_sensor_abs_threshold_config_t *abs_cfg)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Temperature sensor has not been installed");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "temperature sensor is not in init state");
    ESP_RETURN_ON_FALSE(abs_cfg, ESP_ERR_INVALID_ARG, TAG, "Invalid callback configuration");

    temperature_sensor_ll_set_sample_rate(0xffff);
    temperature_sensor_ll_wakeup_mode(TEMPERATURE_SENSOR_LL_WAKE_ABSOLUTE);
    temperature_sensor_ll_set_th_high_val(s_temperature_celsius_2_regval(tsens, abs_cfg->high_threshold));
    temperature_sensor_ll_set_th_low_val(s_temperature_celsius_2_regval(tsens, abs_cfg->low_threshold));

    return ret;
}

esp_err_t temperature_sensor_set_delta_threshold(temperature_sensor_handle_t tsens, const temperature_sensor_delta_threshold_config_t *delta_cfg)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Temperature sensor has not been installed");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "temperature sensor is not in init state");
    ESP_RETURN_ON_FALSE(delta_cfg, ESP_ERR_INVALID_ARG, TAG, "Invalid callback configuration");

    temperature_sensor_ll_set_sample_rate(0xffff);
    temperature_sensor_ll_wakeup_mode(TEMPERATURE_SENSOR_LL_WAKE_DELTA);
    temperature_sensor_ll_set_th_high_val((uint8_t)(delta_cfg->increase_delta / TEMPERATURE_SENSOR_LL_ADC_FACTOR));
    temperature_sensor_ll_set_th_low_val((uint8_t)(delta_cfg->decrease_delta / TEMPERATURE_SENSOR_LL_ADC_FACTOR));

    return ret;
}

esp_err_t temperature_sensor_register_callbacks(temperature_sensor_handle_t tsens, const temperature_sensor_event_callbacks_t *cbs, void *user_arg)
{
    esp_err_t ret = ESP_OK;
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Temperature sensor has not been installed");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_INIT, ESP_ERR_INVALID_STATE, TAG, "temperature sensor is not in init state");
    ESP_RETURN_ON_FALSE(cbs, ESP_ERR_INVALID_ARG, TAG, "callback group pointer is invalid");

#if CONFIG_TEMP_SENSOR_ISR_IRAM_SAFE
    if (cbs->on_threshold) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(cbs->on_threshold), ESP_ERR_INVALID_ARG, TAG, "threshold callback not in IRAM");
    }
    if (user_arg) {
        ESP_RETURN_ON_FALSE(esp_ptr_internal(user_arg), ESP_ERR_INVALID_ARG, TAG, "user argument not in internal RAM");
    }
#endif

    int isr_flags = TEMPERATURE_SENSOR_INTR_ALLOC_FLAGS;
#if SOC_ADC_TEMPERATURE_SHARE_INTR
    isr_flags |= ESP_INTR_FLAG_SHARED;
#endif

    // lazy install interrupt service.
    if (!tsens->temp_sensor_isr_handle) {
        ret = esp_intr_alloc_intrstatus(ETS_TEMPERATURE_SENSOR_INTR_SOURCE, isr_flags,
                                        (uint32_t)temperature_sensor_ll_get_intr_status(),
                                        TEMPERATURE_SENSOR_LL_INTR_MASK, temperature_sensor_isr, tsens, &tsens->temp_sensor_isr_handle);
    }

    if (cbs->on_threshold != NULL) {
        temperature_sensor_ll_enable_intr(true);
        temperature_sensor_ll_clear_intr();
        tsens->threshold_cbs = cbs->on_threshold;
        tsens->cb_user_arg = user_arg;
    } else {
        temperature_sensor_ll_enable_intr(false);
    }
    return ret;
}

#endif // SOC_TEMPERATURE_SENSOR_INTR_SUPPORT
