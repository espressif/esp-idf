/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "sdkconfig.h"
#include "esp_types.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_private/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"
#include "esp_log.h"
#include "esp_efuse_rtc_calib.h"
#include "hal/temperature_sensor_ll.h"
#include "driver/temp_sensor_types_legacy.h"
#include "esp_private/periph_ctrl.h"

static const char *TAG = "tsens";

#define TSENS_ADC_FACTOR  (0.4386)
#define TSENS_DAC_FACTOR  (27.88)
#define TSENS_SYS_OFFSET  (20.52)

typedef struct {
    int index;
    int offset;
    int reg_val;
    int range_min;
    int range_max;
    int error_max;
} tsens_dac_offset_t;

static const tsens_dac_offset_t dac_offset[TSENS_DAC_MAX] = {
    /*     DAC     Offset reg_val  min  max  error */
    {TSENS_DAC_L0,   -2,     5,    50,  125,   3},
    {TSENS_DAC_L1,   -1,     7,    20,  100,   2},
    {TSENS_DAC_L2,    0,    15,   -10,   80,   1},
    {TSENS_DAC_L3,    1,    11,   -30,   50,   2},
    {TSENS_DAC_L4,    2,    10,   -40,   20,   3},
};

typedef enum {
    TSENS_HW_STATE_UNCONFIGURED,
    TSENS_HW_STATE_CONFIGURED,
    TSENS_HW_STATE_STARTED,
} tsens_hw_state_t;

static tsens_hw_state_t tsens_hw_state = TSENS_HW_STATE_UNCONFIGURED;

static float s_deltaT = NAN; // Unused number

esp_err_t temp_sensor_set_config(temp_sensor_config_t tsens)
{
    esp_err_t err = ESP_OK;
    if (tsens_hw_state == TSENS_HW_STATE_STARTED) {
        ESP_LOGE(TAG, "Do not configure the temp sensor when it's running!");
        err = ESP_ERR_INVALID_STATE;
    }
    temperature_sensor_ll_set_clk_div(tsens.clk_div);
    temperature_sensor_ll_set_range(dac_offset[tsens.dac_offset].reg_val);
    temperature_sensor_ll_enable(true);
    ESP_LOGI(TAG, "Config range [%d°C ~ %d°C], error < %d°C",
             dac_offset[tsens.dac_offset].range_min,
             dac_offset[tsens.dac_offset].range_max,
             dac_offset[tsens.dac_offset].error_max);
    tsens_hw_state = TSENS_HW_STATE_CONFIGURED;
    return err;
}

esp_err_t temp_sensor_get_config(temp_sensor_config_t *tsens)
{
    ESP_RETURN_ON_FALSE(tsens != NULL, ESP_ERR_INVALID_ARG, TAG, "no tsens specified");
    tsens->dac_offset = temperature_sensor_ll_get_offset();
    for (int i = TSENS_DAC_L0; i < TSENS_DAC_MAX; i++) {
        if ((int)tsens->dac_offset == dac_offset[i].reg_val) {
            tsens->dac_offset = dac_offset[i].index;
            break;
        }
    }
    tsens->clk_div = temperature_sensor_ll_get_clk_div();
    return ESP_OK;
}

esp_err_t temp_sensor_start(void)
{
    esp_err_t err = ESP_OK;
    if (tsens_hw_state != TSENS_HW_STATE_CONFIGURED) {
        ESP_LOGE(TAG, "Is already running or not be configured");
        err = ESP_ERR_INVALID_STATE;
    }
    regi2c_saradc_enable();
    periph_module_enable(PERIPH_TEMPSENSOR_MODULE);
    temperature_sensor_ll_enable(true);
    temperature_sensor_ll_clk_enable(true);
    temperature_sensor_ll_clk_sel(TEMPERATURE_SENSOR_CLK_SRC_DEFAULT);
    tsens_hw_state = TSENS_HW_STATE_STARTED;
    return err;
}

esp_err_t temp_sensor_stop(void)
{
    regi2c_saradc_disable();
    temperature_sensor_ll_enable(false);
    tsens_hw_state = TSENS_HW_STATE_CONFIGURED;
    return ESP_OK;
}

esp_err_t temp_sensor_read_raw(uint32_t *tsens_out)
{
    ESP_RETURN_ON_FALSE(tsens_out != NULL, ESP_ERR_INVALID_ARG, TAG, "no tsens_out specified");
    *tsens_out = temperature_sensor_ll_get_raw_value();
    return ESP_OK;
}

static esp_err_t read_delta_t_from_efuse(void)
{
    ESP_RETURN_ON_ERROR(esp_efuse_rtc_calib_get_tsens_val(&s_deltaT), TAG, "Calibration error");
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

esp_err_t temp_sensor_read_celsius(float *celsius)
{
    ESP_RETURN_ON_FALSE(celsius != NULL, ESP_ERR_INVALID_ARG, TAG, "celsius points to nothing");
    if (tsens_hw_state != TSENS_HW_STATE_STARTED) {
        ESP_LOGE(TAG, "Has not been started");
        return ESP_ERR_INVALID_STATE;
    }
    temp_sensor_config_t tsens;
    uint32_t tsens_out = 0;
    temp_sensor_get_config(&tsens);
    temp_sensor_read_raw(&tsens_out);
    ESP_LOGV(TAG, "tsens_out %"PRIu32, tsens_out);
    const tsens_dac_offset_t *dac = &dac_offset[tsens.dac_offset];
    *celsius = parse_temp_sensor_raw_value(tsens_out, dac->offset);
    if (*celsius < dac->range_min || *celsius > dac->range_max) {
        ESP_LOGW(TAG, "Exceeding the temperature range!");
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

/**
 * @brief This function will be called during start up, to check that this legacy temp sensor driver is not running along with the new driver
 */
__attribute__((constructor))
static void check_legacy_temp_sensor_driver_conflict(void)
{
    // This function was declared as weak here. temperature_sensor driver has one implementation.
    // So if temperature_sensor driver is not linked in, then `temperature_sensor_install()` should be NULL at runtime.
    extern __attribute__((weak)) esp_err_t temperature_sensor_install(const void *tsens_config, void **ret_tsens);
    if ((void *)temperature_sensor_install != NULL) {
        ESP_EARLY_LOGE(TAG, "CONFLICT! driver_ng is not allowed to be used with the legacy driver");
        abort();
    }
    ESP_EARLY_LOGW(TAG, "legacy driver is deprecated, please migrate to `driver/temperature_sensor.h`");
}
