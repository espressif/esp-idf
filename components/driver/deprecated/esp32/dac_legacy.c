/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "freertos/FreeRTOS.h"
#include "hal/dac_ll.h"
#include "esp_err.h"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define DAC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

esp_err_t dac_i2s_enable(void)
{
    DAC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(true);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_i2s_disable(void)
{
    DAC_ENTER_CRITICAL();
    dac_ll_digi_enable_dma(false);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}
