/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "ulp_adc.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_hal_common.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/adc_share_hw_ctrl.h"

static const char *TAG = "ulp_adc";
static adc_oneshot_unit_handle_t s_adc1_handle = NULL;

esp_err_t ulp_adc_init(const ulp_adc_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;

    ESP_RETURN_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, TAG, "cfg == NULL");
    ESP_RETURN_ON_FALSE(cfg->adc_n == ADC_UNIT_1, ESP_ERR_INVALID_ARG, TAG, "Only ADC_UNIT_1 is supported for now");

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = cfg->adc_n,
        .ulp_mode = cfg->ulp_mode,
    };

    if (init_config1.ulp_mode == ADC_ULP_MODE_DISABLE) {
        /* Default to RISCV for backward compatibility */
        ESP_LOGI(TAG, "No ulp mode specified in cfg struct, default to riscv");
        init_config1.ulp_mode = ADC_ULP_MODE_RISCV;
    }

    ret = adc_oneshot_new_unit(&init_config1, &s_adc1_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = cfg->width,
        .atten = cfg->atten,
    };
    ret = adc_oneshot_config_channel(s_adc1_handle, cfg->channel, &config);
    if (ret != ESP_OK) {
        return ret;
    }

    //Calibrate the ADC
#if SOC_ADC_CALIBRATION_V1_SUPPORTED
    adc_set_hw_calibration_code(cfg->adc_n, cfg->atten);
#endif

    return ret;
}

esp_err_t ulp_adc_deinit(void)
{
    // No need to check for null-pointer and stuff, oneshot driver already does that
    return adc_oneshot_del_unit(s_adc1_handle);
}
