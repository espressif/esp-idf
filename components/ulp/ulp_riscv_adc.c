/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp32s3/ulp_riscv_adc.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "hal/adc_hal.h"
#include "esp_private/esp_sleep_internal.h"


extern esp_err_t adc1_rtc_mode_acquire(void);
extern uint32_t get_calibration_offset(adc_unit_t adc_n, adc_channel_t chan);

static const char *TAG = "ulp_riscv_adc";

esp_err_t ulp_riscv_adc_init(const ulp_riscv_adc_cfg_t *cfg)
{
    esp_err_t ret = ESP_OK;

    ESP_GOTO_ON_FALSE(cfg, ESP_ERR_INVALID_ARG, err, TAG, "cfg == NULL");

    adc1_config_channel_atten(cfg->channel, cfg->atten);
    adc1_config_width(cfg->width);

    //Calibrate the ADC
    uint32_t cal_val = get_calibration_offset(ADC_UNIT_1, cfg->channel);
    adc_hal_set_calibration_param(ADC_NUM_1, cal_val);

    adc1_rtc_mode_acquire();

    esp_sleep_enable_adc_tsens_monitor(true);

err:
    return ret;
}
