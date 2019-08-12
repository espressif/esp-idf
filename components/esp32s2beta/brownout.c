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
#include "soc/soc.h"
#include "soc/cpu.h"
#include "soc/rtc_cntl_reg.h"
#include "esp32s2beta/rom/ets_sys.h"
#include "esp_private/system_internal.h"
#include "driver/rtc_cntl.h"
#include "freertos/FreeRTOS.h"

#ifdef CONFIG_ESP32S2_BROWNOUT_DET_LVL
#define BROWNOUT_DET_LVL CONFIG_ESP32S2_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif //CONFIG_ESP32S2_BROWNOUT_DET_LVL

static void rtc_brownout_isr_handler(void *arg)
{
    /* Normally RTC ISR clears the interrupt flag after the application-supplied
     * handler returns. Since restart is called here, the flag needs to be
     * cleared manually.
     */
    REG_WRITE(RTC_CNTL_INT_CLR_REG, RTC_CNTL_BROWN_OUT_INT_CLR);
    /* Stall the other CPU to make sure the code running there doesn't use UART
     * at the same time as the following ets_printf.
     */
    esp_cpu_stall(!xPortGetCoreID());
    ets_printf("\r\nBrownout detector was triggered\r\n\r\n");
    esp_restart_noos();
}

void esp_brownout_init(void)
{
// TODO: implement brownout threshold configuration for esp32s2beta - IDF-751

    ESP_ERROR_CHECK( rtc_isr_register(rtc_brownout_isr_handler, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M) );

    REG_SET_BIT(RTC_CNTL_INT_ENA_REG, RTC_CNTL_BROWN_OUT_INT_ENA_M);
}
