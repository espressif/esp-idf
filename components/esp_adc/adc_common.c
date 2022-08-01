/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <esp_types.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "freertos/FreeRTOS.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/adc_private.h"
#include "esp_private/adc_share_hw_ctrl.h"
#include "driver/gpio.h"
#include "hal/adc_hal.h"
#include "hal/adc_hal_common.h"
#include "hal/adc_hal_conf.h"
#include "soc/adc_periph.h"


static const char *TAG = "adc_common";
static portMUX_TYPE s_spinlock = portMUX_INITIALIZER_UNLOCKED;
extern portMUX_TYPE rtc_spinlock;

/*------------------------------------------------------------------------------
* For those who use APB_SARADC periph
*----------------------------------------------------------------------------*/
static int s_adc_digi_ctrlr_cnt;

void adc_apb_periph_claim(void)
{
    portENTER_CRITICAL(&s_spinlock);
    s_adc_digi_ctrlr_cnt++;
    if (s_adc_digi_ctrlr_cnt == 1) {
        //enable ADC digital part
        periph_module_enable(PERIPH_SARADC_MODULE);
        //reset ADC digital part
        periph_module_reset(PERIPH_SARADC_MODULE);
    }

    portEXIT_CRITICAL(&s_spinlock);
}

void adc_apb_periph_free(void)
{
    portENTER_CRITICAL(&s_spinlock);
    s_adc_digi_ctrlr_cnt--;
    if (s_adc_digi_ctrlr_cnt == 0) {
        periph_module_disable(PERIPH_SARADC_MODULE);
    } else if (s_adc_digi_ctrlr_cnt < 0) {
        portEXIT_CRITICAL(&s_spinlock);
        ESP_LOGE(TAG, "%s called, but `s_adc_digi_ctrlr_cnt == 0`", __func__);
        abort();
    }

    portEXIT_CRITICAL(&s_spinlock);
}

/*---------------------------------------------------------------
            ADC IOs
---------------------------------------------------------------*/
esp_err_t adc_io_to_channel(int io_num, adc_unit_t *unit_id, adc_channel_t *channel)
{
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), ESP_ERR_INVALID_ARG, TAG, "invalid gpio number");
    ESP_RETURN_ON_FALSE(unit_id && channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool found = false;
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        for (int j = 0; j < SOC_ADC_MAX_CHANNEL_NUM; j++) {
            if (adc_channel_io_map[i][j] == io_num) {
                *channel = j;
                *unit_id = i;
                found = true;
            }
        }
    }
    return (found) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t adc_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int *io_num)
{
    ESP_RETURN_ON_FALSE(unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid unit");
    ESP_RETURN_ON_FALSE(channel < SOC_ADC_CHANNEL_NUM(unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid channel");
    ESP_RETURN_ON_FALSE(io_num, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    *io_num = adc_channel_io_map[unit_id][channel];
    return ESP_OK;
}

#if SOC_ADC_CALIBRATION_V1_SUPPORTED
/*---------------------------------------------------------------
            ADC Hardware Calibration
---------------------------------------------------------------*/
static __attribute__((constructor)) void adc_hw_calibration(void)
{
    //Calculate all ICode
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        adc_hal_calibration_init(i);
        for (int j = 0; j < SOC_ADC_ATTEN_NUM; j++) {
            /**
             * This may get wrong when attenuations are NOT consecutive on some chips,
             * update this when bringing up the calibration on that chip
             */
            adc_calc_hw_calibration_code(i, j);
        }
    }
}
#endif //#if SOC_ADC_CALIBRATION_V1_SUPPORTED
