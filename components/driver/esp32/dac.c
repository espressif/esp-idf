// Copyright 2019-2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "driver/rtc_io.h"
#include "driver/dac.h"
#include "soc/dac_periph.h"
#include "hal/dac_hal.h"

extern portMUX_TYPE rtc_spinlock; //TODO: Will be placed in the appropriate position after the rtc module is finished.
#define DAC_ENTER_CRITICAL()  portENTER_CRITICAL(&rtc_spinlock)
#define DAC_EXIT_CRITICAL()  portEXIT_CRITICAL(&rtc_spinlock)

/*---------------------------------------------------------------
                    Digital controller setting
---------------------------------------------------------------*/

esp_err_t dac_i2s_enable(void)
{
    DAC_ENTER_CRITICAL();
    dac_hal_digi_enable_dma(true);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}

esp_err_t dac_i2s_disable(void)
{
    DAC_ENTER_CRITICAL();
    dac_hal_digi_enable_dma(false);
    DAC_EXIT_CRITICAL();

    return ESP_OK;
}