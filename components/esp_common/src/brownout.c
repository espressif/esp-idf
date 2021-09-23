// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "soc/soc.h"
#include "soc/cpu.h"
#include "soc/rtc_periph.h"
#include "hal/brownout_hal.h"
#include "esp_private/system_internal.h"
#include "driver/rtc_cntl.h"
#include "freertos/FreeRTOS.h"

#if defined(CONFIG_ESP32_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP32_BROWNOUT_DET_LVL
#elif defined(CONFIG_ESP32S2_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP32S2_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif

#ifdef SOC_BROWNOUT_RESET_SUPPORTED
#define BROWNOUT_RESET_EN true
#else
#define BROWNOUT_RESET_EN false
#endif // SOC_BROWNOUT_RESET_SUPPORTED

#ifndef SOC_BROWNOUT_RESET_SUPPORTED
static void rtc_brownout_isr_handler(void *arg)
{
    /* Normally RTC ISR clears the interrupt flag after the application-supplied
     * handler returns. Since restart is called here, the flag needs to be
     * cleared manually.
     */
    brownout_hal_intr_clear();
    /* Stall the other CPU to make sure the code running there doesn't use UART
     * at the same time as the following ets_printf.
     */
    esp_cpu_stall(!xPortGetCoreID());
    esp_reset_reason_set_hint(ESP_RST_BROWNOUT);
    ets_printf("\r\nBrownout detector was triggered\r\n\r\n");
    esp_restart_noos();
}
#endif // not SOC_BROWNOUT_RESET_SUPPORTED

void esp_brownout_init(void)
{
    brownout_hal_config_t cfg = {
        .threshold = BROWNOUT_DET_LVL,
        .enabled = true,
        .reset_enabled = BROWNOUT_RESET_EN,
        .flash_power_down = true,
        .rf_power_down = true,
    };

    brownout_hal_config(&cfg);

#ifndef SOC_BROWNOUT_RESET_SUPPORTED
    ESP_ERROR_CHECK( rtc_isr_register(rtc_brownout_isr_handler, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M) );

    brownout_hal_intr_enable(true);
#endif // not SOC_BROWNOUT_RESET_SUPPORTED
}

void esp_brownout_disable(void)
{
    brownout_hal_config_t cfg = {
        .enabled = false,
    };

    brownout_hal_config(&cfg);

#ifndef SOC_BROWNOUT_RESET_SUPPORTED
    brownout_hal_intr_enable(false);

    rtc_isr_deregister(rtc_brownout_isr_handler, NULL);
#endif // not SOC_BROWNOUT_RESET_SUPPORTED
}
