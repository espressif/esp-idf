/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "esp_private/system_internal.h"
#include "driver/rtc_cntl.h"

#include "esp_rom_sys.h"

#include "soc/soc.h"
#include "esp_cpu.h"
#include "soc/rtc_periph.h"
#include "hal/cpu_hal.h"

#include "hal/brownout_hal.h"

#include "sdkconfig.h"

#if defined(CONFIG_ESP_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif

#if SOC_BROWNOUT_RESET_SUPPORTED
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
     * at the same time as the following esp_rom_printf.
     */
    esp_cpu_stall(!cpu_hal_get_core_id());
    esp_reset_reason_set_hint(ESP_RST_BROWNOUT);
    esp_rom_printf("\r\nBrownout detector was triggered\r\n\r\n");
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
    rtc_isr_register(rtc_brownout_isr_handler, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M);

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
