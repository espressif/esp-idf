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
#include "esp_private/rtc_ctrl.h"
#include "esp_private/spi_flash_os.h"

#include "esp_rom_sys.h"
#include "esp_cpu.h"

#include "soc/soc.h"

#include "soc/rtc_periph.h"
#include "esp_attr.h"
#include "bootloader_flash.h"
#include "esp_intr_alloc.h"

#include "hal/brownout_hal.h"

#include "sdkconfig.h"

#if defined(CONFIG_ESP_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif

#if CONFIG_ESP_SYSTEM_BROWNOUT_INTR
IRAM_ATTR static void rtc_brownout_isr_handler(void *arg)
{
    /* Normally RTC ISR clears the interrupt flag after the application-supplied
     * handler returns. Since restart is called here, the flag needs to be
     * cleared manually.
     */
    brownout_hal_intr_clear();

    // Stop the other core.
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    const uint32_t core_id = esp_cpu_get_core_id();
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_stall(other_core_id);
#endif

    esp_reset_reason_set_hint(ESP_RST_BROWNOUT);
#if CONFIG_SPI_FLASH_BROWNOUT_RESET
    if (spi_flash_brownout_need_reset()) {
        bootloader_flash_reset_chip();
    } else
#endif // CONFIG_SPI_FLASH_BROWNOUT_RESET
    {
        esp_rom_printf("\r\nBrownout detector was triggered\r\n\r\n");
    }

    esp_restart_noos();
}
#endif // CONFIG_ESP_SYSTEM_BROWNOUT_INTR

void esp_brownout_init(void)
{
#if CONFIG_ESP_SYSTEM_BROWNOUT_INTR
    brownout_hal_config_t cfg = {
        .threshold = BROWNOUT_DET_LVL,
        .enabled = true,
        .reset_enabled = false,
        .flash_power_down = true,
        .rf_power_down = true,
    };

    brownout_hal_config(&cfg);
    brownout_hal_intr_clear();
#if CONFIG_IDF_TARGET_ESP32C6 // TODO: IDF-5645
    rtc_isr_register(rtc_brownout_isr_handler, NULL, LP_ANALOG_PERI_LP_ANA_BOD_MODE0_LP_INT_ENA_M, RTC_INTR_FLAG_IRAM);
#else
    rtc_isr_register(rtc_brownout_isr_handler, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M, RTC_INTR_FLAG_IRAM);
#endif
    brownout_hal_intr_enable(true);

#else // brownout without interrupt

    brownout_hal_config_t cfg = {
        .threshold = BROWNOUT_DET_LVL,
        .enabled = true,
        .reset_enabled = true,
        .flash_power_down = true,
        .rf_power_down = true,
    };

    brownout_hal_config(&cfg);
#endif
}

void esp_brownout_disable(void)
{
    brownout_hal_config_t cfg = {
        .enabled = false,
    };

    brownout_hal_config(&cfg);
#if CONFIG_ESP_SYSTEM_BROWNOUT_INTR
    brownout_hal_intr_enable(false);
    rtc_isr_deregister(rtc_brownout_isr_handler, NULL);
#endif // CONFIG_ESP_SYSTEM_BROWNOUT_INTR
}
