/*
 * SPDX-FileCopyrightText: 2016-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#if CONFIG_TEMP_SENSOR_ENABLE_DEBUG_LOG
// The local log level must be defined before including esp_log.h
// Set the maximum log level for this source file
#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#endif

#include <stdlib.h>
#include <math.h>
#include <string.h>
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
#include "hal/temperature_sensor_types.h"
#include "hal/temperature_sensor_ll.h"

static const char *TAG = "temperature_sensor";

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define TEMPERATURE_SENSOR_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define TEMPERATURE_SENSOR_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

typedef enum {
    TSENS_HW_STATE_UNCONFIGURED,
    TSENS_HW_STATE_CONFIGURED,
    TSENS_HW_STATE_STARTED,
} temp_sensor_state_t;

static float s_deltaT = NAN; // unused number

typedef struct temperature_sensor_obj_t temperature_sensor_obj_t;

struct temperature_sensor_obj_t {
    const temp_sensor_ll_attribute_t *tsens_attribute;
    temp_sensor_state_t  tsens_hw_state;
    temperature_sensor_clk_src_t clk_src;
};

static temp_sensor_ll_attribute_t *s_tsens_attribute_copy;

static int inline accuracy_compare(const void *p1, const void *p2)
{
    return ((*(temp_sensor_ll_attribute_t *)p1).error_max < (*(temp_sensor_ll_attribute_t *)p2).error_max) ? -1 : 1;
}

static esp_err_t temperature_sensor_attribute_table_sort(void)
{
    s_tsens_attribute_copy = (temp_sensor_ll_attribute_t *)heap_caps_malloc(sizeof(temp_sensor_ll_attributes), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(s_tsens_attribute_copy != NULL, ESP_ERR_NO_MEM, TAG, "No space for s_tsens_attribute_copy");
    for (int i = 0 ; i < TEMPERATURE_SENSOR_LL_RANGE_NUM; i++) {
        s_tsens_attribute_copy[i] = temp_sensor_ll_attributes[i];
    }
    // Sort from small to large by error_max.
    qsort(s_tsens_attribute_copy, TEMPERATURE_SENSOR_LL_RANGE_NUM, sizeof(s_tsens_attribute_copy[0]), accuracy_compare);
    return ESP_OK;
}

static esp_err_t temperature_sensor_choose_best_range(temperature_sensor_handle_t tsens, const temperature_sensor_config_t *tsens_config)
{
    for (int i = 0 ; i < TEMPERATURE_SENSOR_LL_RANGE_NUM; i++) {
        if ((tsens_config->range_min >= s_tsens_attribute_copy[i].range_min) && (tsens_config->range_max <= s_tsens_attribute_copy[i].range_max)) {
            tsens->tsens_attribute = &s_tsens_attribute_copy[i];
            break;
        }
    }
    ESP_RETURN_ON_FALSE(tsens->tsens_attribute != NULL, ESP_ERR_INVALID_ARG, TAG, "Out of testing range");
    return ESP_OK;
}

esp_err_t temperature_sensor_install(const temperature_sensor_config_t *tsens_config, temperature_sensor_handle_t *ret_tsens)
{
    esp_err_t ret = ESP_OK;
#if CONFIG_TEMP_SENSOR_ENABLE_DEBUG_LOG
    esp_log_level_set(TAG, ESP_LOG_DEBUG);
#endif
    ESP_RETURN_ON_FALSE((tsens_config && ret_tsens), ESP_ERR_INVALID_ARG, TAG, "Invalid argument");
    ESP_RETURN_ON_FALSE((s_tsens_attribute_copy == NULL), ESP_ERR_INVALID_STATE, TAG, "Already installed");
    temperature_sensor_handle_t tsens;
    tsens = (temperature_sensor_obj_t *) heap_caps_calloc(1, sizeof(temperature_sensor_obj_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(tsens != NULL, ESP_ERR_NO_MEM, err, TAG, "install fail...");
    tsens->clk_src = tsens_config->clk_src;

    periph_module_enable(PERIPH_TEMPSENSOR_MODULE);
    periph_module_reset(PERIPH_TEMPSENSOR_MODULE);
    ESP_GOTO_ON_ERROR(temperature_sensor_attribute_table_sort(), err, TAG, "Table sort failed");
    ESP_GOTO_ON_ERROR(temperature_sensor_choose_best_range(tsens, tsens_config), err, TAG, "Cannot select the correct range");

    ESP_LOGI(TAG, "Range [%d°C ~ %d°C], error < %d°C",
             tsens->tsens_attribute->range_min,
             tsens->tsens_attribute->range_max,
             tsens->tsens_attribute->error_max);
    TEMPERATURE_SENSOR_ENTER_CRITICAL();
    temperature_sensor_ll_set_range(tsens->tsens_attribute->reg_val);
    TEMPERATURE_SENSOR_EXIT_CRITICAL();
    tsens->tsens_hw_state = TSENS_HW_STATE_CONFIGURED;
    *ret_tsens = tsens;
    return ESP_OK;
err:
    temperature_sensor_uninstall(tsens);
    return ret;
}

esp_err_t temperature_sensor_uninstall(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Has already been uninstalled");
    ESP_RETURN_ON_FALSE(tsens->tsens_hw_state != TSENS_HW_STATE_STARTED, ESP_ERR_INVALID_STATE, TAG, "Has not been stopped");
    if (s_tsens_attribute_copy) {
        free(s_tsens_attribute_copy);
    }
    s_tsens_attribute_copy = NULL;
    tsens->tsens_hw_state = TSENS_HW_STATE_UNCONFIGURED;
    heap_caps_free(tsens);
    tsens = NULL;
    return ESP_OK;
}

esp_err_t temperature_sensor_start(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Has not been installed");
    ESP_RETURN_ON_FALSE(tsens->tsens_hw_state == TSENS_HW_STATE_CONFIGURED, ESP_ERR_INVALID_STATE, TAG, "Is already running or has not been configured");
#if SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC
    if (tsens->clk_src == TEMPERATURE_SENSOR_CLK_SRC_FAST_RC) {
        periph_rtc_dig_clk8m_enable();
    }
#endif
    temperature_sensor_ll_clk_enable(true);
    temperature_sensor_ll_clk_sel(tsens->clk_src);
    temperature_sensor_ll_enable(true);
    tsens->tsens_hw_state = TSENS_HW_STATE_STARTED;
    return ESP_OK;
}

esp_err_t temperature_sensor_stop(temperature_sensor_handle_t tsens)
{
    ESP_RETURN_ON_FALSE(tsens->tsens_hw_state == TSENS_HW_STATE_STARTED, ESP_ERR_INVALID_STATE, TAG, "Has not been started");
    temperature_sensor_ll_enable(false);
#if SOC_TEMPERATURE_SENSOR_SUPPORT_FAST_RC
    if (tsens->clk_src == TEMPERATURE_SENSOR_CLK_SRC_FAST_RC) {
        periph_rtc_dig_clk8m_disable();
    }
#endif
    periph_module_disable(PERIPH_TEMPSENSOR_MODULE);
    tsens->tsens_hw_state = TSENS_HW_STATE_CONFIGURED;
    return ESP_OK;
}

static esp_err_t temp_sensor_read_raw(uint32_t *tsens_out)
{
    ESP_RETURN_ON_FALSE(tsens_out != NULL, ESP_ERR_INVALID_ARG, TAG, "No tsens_out specified");
    *tsens_out = temperature_sensor_ll_get_raw_value();
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

static float parse_temp_sensor_raw_value(uint32_t tsens_raw, const int dac_offset)
{
    if (isnan(s_deltaT)) { //suggests that the value is not initialized
        read_delta_t_from_efuse();
    }
    float result = (TEMPERATURE_SENSOR_LL_ADC_FACTOR * (float)tsens_raw - TEMPERATURE_SENSOR_LL_DAC_FACTOR * dac_offset - TEMPERATURE_SENSOR_LL_OFFSET_FACTOR) - s_deltaT / 10.0;
    return result;
}

esp_err_t temperature_sensor_get_celsius(temperature_sensor_handle_t tsens, float *out_celsius)
{
    ESP_RETURN_ON_FALSE((tsens != NULL), ESP_ERR_INVALID_ARG, TAG, "Has not been installed");
    ESP_RETURN_ON_FALSE(out_celsius != NULL, ESP_ERR_INVALID_ARG, TAG, "Celsius points to nothing");
    ESP_RETURN_ON_FALSE(tsens->tsens_hw_state == TSENS_HW_STATE_STARTED, ESP_ERR_INVALID_ARG, TAG, "Has not been started");
    uint32_t tsens_out = 0;
    temp_sensor_read_raw(&tsens_out);
    ESP_LOGV(TAG, "tsens_out %d", tsens_out);
    *out_celsius = parse_temp_sensor_raw_value(tsens_out, tsens->tsens_attribute->offset);
    if (*out_celsius < tsens->tsens_attribute->range_min || *out_celsius > tsens->tsens_attribute->range_max) {
        ESP_LOGW(TAG, "Temperature range exceeded!");
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check the new temperature driver is not running along with the legacy temp sensor driver
 */
__attribute__((constructor))
static void check_temperature_driver_conflict(void)
{
    extern int temp_sensor_driver_init_count;
    temp_sensor_driver_init_count++;
    if (temp_sensor_driver_init_count > 1) {
        ESP_EARLY_LOGE(TAG, "CONFLICT! The temperature driver can't work along with the legacy temp sensor driver");
        abort();
    }
}
