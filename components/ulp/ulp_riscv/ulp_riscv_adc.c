/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ulp_riscv_adc.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "hal/adc_hal_common.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/adc_share_hw_ctrl.h"

static const char *TAG = "ulp_riscv_adc";

esp_err_t ulp_riscv_adc_init(const ulp_riscv_adc_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, err, TAG, "cfg == NULL");
    ESP_GOTO_ON_FALSE(cfg->adc_n == ADC_UNIT_1, ESP_ERR_INVALID_ARG, err, TAG, "Only ADC_UNIT_1 is supported for now");

    //-------------ADC1 Init---------------//
    adc_oneshot_unit_handle_t adc1_handle;
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = cfg->adc_n,
        .ulp_mode = ADC_ULP_MODE_RISCV,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = cfg->width,
        .atten = cfg->atten,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, cfg->channel, &config));

    //Calibrate the ADC
    adc_set_hw_calibration_code(cfg->adc_n, cfg->atten);
    esp_sleep_enable_adc_tsens_monitor(true);

err:
    return ret;
}
