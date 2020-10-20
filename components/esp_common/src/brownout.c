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
#include "esp_sleep.h"
#include "driver/rtc_cntl.h"
#include "freertos/FreeRTOS.h"
#include "esp_rom_sys.h"

#if defined(CONFIG_ESP32_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP32_BROWNOUT_DET_LVL
#elif defined(CONFIG_ESP32S2_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP32S2_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif

#if SOC_BROWNOUT_RESET_SUPPORTED
#define BROWNOUT_RESET_EN true
#else
#define BROWNOUT_RESET_EN false
#endif // SOC_BROWNOUT_RESET_SUPPORTED


static void brownout_warn(const char *s)
{
    /* Stall the other CPU to make sure the code running there doesn't use UART
     * at the same time as the following ets_printf.
     */
    esp_cpu_stall(!xPortGetCoreID());
    esp_reset_reason_set_hint(ESP_RST_BROWNOUT);
    ets_printf(s);
}

static void rtc_brownout_isr_handler(void *arg)
{
    /* Normally RTC ISR clears the interrupt flag after the application-supplied
     * handler returns. Since restart is called here, the flag needs to be
     * cleared manually.
     */
    brownout_hal_intr_clear();
    brownout_warn("\r\nBrownout detector was triggered\r\n\r\n");
    esp_restart_noos();
}

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

#if CONFIG_ESP32_BROWNOUT_DET_SHUTDOWN_IF_EARLY || CONFIG_ESP32S2_BROWNOUT_DET_SHUTDOWN_IF_EARLY || CONFIG_ESP32S3_BROWNOUT_DET_SHUTDOWN_IF_EARLY
    /* We're still booting: if a brownout is already happening, shut down
     * rather than trying to recover. */
    if (brownout_hal_intr_raw())
    {
      brownout_warn("\r\nBrownout detector was triggered during boot: shutting down.\r\n\r\n");
      /* Disable all wakeup sources in order to fully shut down. */
      esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
      esp_deep_sleep_start();
    }
#endif

    /* Now that we've checked no brownout had occured until now in the boot
     * sequence, install the handler that resets the system upon brownout. */
    rtc_isr_register(rtc_brownout_isr_handler, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M);

    brownout_hal_intr_enable(true);
}
