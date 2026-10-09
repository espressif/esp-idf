/*
 * SPDX-FileCopyrightText: 2019-2026 Espressif Systems (Shanghai) CO LTD
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
#include "esp_private/regi2c_ctrl.h"
#include "esp_private/sar_periph_ctrl.h"
#include "esp_clk_tree.h"
#include "esp_private/esp_clk_tree_common.h"
#include "hal/adc_periph.h"

static const char *TAG = "adc_common";

/*---------------------------------------------------------------
            ADC IOs
---------------------------------------------------------------*/
esp_err_t adc_io_to_channel(int io_num, adc_unit_t * const unit_id, adc_channel_t * const channel)
{
    ESP_RETURN_ON_FALSE(GPIO_IS_VALID_GPIO(io_num), ESP_ERR_INVALID_ARG, TAG, "invalid gpio number");
    ESP_RETURN_ON_FALSE(unit_id && channel, ESP_ERR_INVALID_ARG, TAG, "invalid argument: null pointer");

    bool found = false;
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        for (int j = 0; j < ADC_LL_MAX_CHANNEL_NUM; j++) {
            if (adc_channel_io_map[i][j] == io_num) {
                *channel = j;
                *unit_id = i;
                found = true;
            }
        }
    }
    return (found) ? ESP_OK : ESP_ERR_NOT_FOUND;
}

esp_err_t adc_channel_to_io(adc_unit_t unit_id, adc_channel_t channel, int * const io_num)
{
    ESP_RETURN_ON_FALSE(unit_id < SOC_ADC_PERIPH_NUM, ESP_ERR_INVALID_ARG, TAG, "invalid unit");
    ESP_RETURN_ON_FALSE(channel < ADC_LL_CHANNEL_NUM(unit_id), ESP_ERR_INVALID_ARG, TAG, "invalid channel");
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
    ANALOG_CLOCK_ENABLE();
    //Calculate all ICode
    for (int i = 0; i < SOC_ADC_PERIPH_NUM; i++) {
        if (ADC_LL_NEED_APB_PERIPH_CLAIM(i)) {
            adc_apb_periph_claim();
        }

        adc_hal_calibration_init(i);
        for (int j = 0; j < SOC_ADC_ATTEN_NUM; j++) {
            /**
             * This may get wrong when attenuations are NOT consecutive on some chips,
             * update this when bringing up the calibration on that chip
             */
            adc_calc_hw_calibration_code(i, j);
#if SOC_ADC_CALIB_CHAN_COMPENS_SUPPORTED
            /* Load the channel compensation from efuse */
            for (int k = 0; k < ADC_LL_CHANNEL_NUM(i); k++) {
                adc_load_hw_calibration_chan_compens(i, k, j);
            }
#endif
        }
        if (ADC_LL_NEED_APB_PERIPH_CLAIM(i)) {
            adc_apb_periph_free();
        }
    }
    ANALOG_CLOCK_DISABLE();
}
#endif //#if SOC_ADC_CALIBRATION_V1_SUPPORTED

#if ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
void adc_cal_sw_weight(adc_unit_t adc_n, adc_oneshot_clk_src_t clk_src)
{
    if (ADC_LL_NEED_APB_PERIPH_CLAIM(adc_n)) {
        adc_apb_periph_claim();
    }
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    ESP_ERROR_CHECK(esp_clk_tree_acquire_src((soc_module_clk_t)clk_src));
#endif
    ANALOG_CLOCK_ENABLE();
    sar_periph_ctrl_adc_oneshot_power_acquire();

    adc_hal_sw_weight_calibrate(adc_n, clk_src);

    sar_periph_ctrl_adc_oneshot_power_release();
    ANALOG_CLOCK_DISABLE();
#if SOC_ADC_DIG_CTRL_SUPPORTED && !SOC_ADC_RTC_CTRL_SUPPORTED
    ESP_ERROR_CHECK(esp_clk_tree_release_src((soc_module_clk_t)clk_src));
#endif
    if (ADC_LL_NEED_APB_PERIPH_CLAIM(adc_n)) {
        adc_apb_periph_free();
    }
}
#endif // ADC_LL_SW_WEIGHT_CALIBRATION_SUPPORTED
