/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/temperature_sensor_ll.h"
#include "soc/temperature_sensor_periph.h"
#include "esp_private/sar_periph_ctrl.h"

static const char *TAG = "temperature_sensor";

typedef enum {
    TEMP_SENSOR_FSM_INIT,
    TEMP_SENSOR_FSM_ENABLE,
} temp_sensor_fsm_t;

static float s_deltaT = NAN; // unused number

typedef struct temperature_sensor_obj_t temperature_sensor_obj_t;

struct temperature_sensor_obj_t {
    const temperature_sensor_attribute_t *tsens_attribute;
    temp_sensor_fsm_t  fsm;
    temperature_sensor_clk_src_t clk_src;
};

static temperature_sensor_attribute_t *s_tsens_attribute_copy;

static int inline accuracy_compare(const void *p1, const void *p2)
{
    return ((*(temperature_sensor_attribute_t *)p1).error_max < (*(temperature_sensor_attribute_t *)p2).error_max) ? -1 : 1;
}

static esp_err_t temperature_sensor_attribute_table_sort(void)
{
    s_tsens_attribute_copy = (temperature_sensor_attribute_t *)heap_caps_malloc(sizeof(temperature_sensor_attributes), MALLOC_CAP_DEFAULT);
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
    ESP_GOTO_ON_FALSE(tsens != NULL, ESP_ERR_NO_MEM, err, TAG, "no mem for temp sensor");
    tsens->clk_src = tsens_config->clk_src;

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

    temperature_sensor_ll_clk_sel(tsens->clk_src);
    temperature_sensor_power_acquire();
    tsens->fsm = TEMP_SENSOR_FSM_ENABLE;
    return ESP_OK;
}

esp_err_t temperature_sensor_disable(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE(tsens, ESP_ERR_INVALID_ARG, TAG, "invalid argument");
    ESP_RETURN_ON_FALSE(tsens->fsm == TEMP_SENSOR_FSM_ENABLE, ESP_ERR_INVALID_STATE, TAG, "tsens not enabled yet");

    temperature_sensor_power_release();

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
