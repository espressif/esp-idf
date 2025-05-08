/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp_macros.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "soc/soc.h"
#include "soc/rtc_periph.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "bootloader_flash.h"
#include "esp_intr_alloc.h"
#include "hal/brownout_hal.h"
#include "hal/brownout_ll.h"
#include "sdkconfig.h"
#include "esp_rom_uart.h"
#include "hal/uart_ll.h"
#include "soc/power_supply_periph.h"
#include "esp_brownout.h"
#include "esp_check.h"
#include "esp_memory_utils.h"

#if defined(CONFIG_ESP_BROWNOUT_DET_LVL)
#define BROWNOUT_DET_LVL CONFIG_ESP_BROWNOUT_DET_LVL
#else
#define BROWNOUT_DET_LVL 0
#endif

static __attribute__((unused)) DRAM_ATTR const char TAG[] = "BOD";

static brownout_callback_t s_brownout_callback = NULL;

#if CONFIG_ESP_BROWNOUT_USE_INTR
IRAM_ATTR static void rtc_brownout_isr_handler(void *arg)
{
    /* Normally RTC ISR clears the interrupt flag after the application-supplied
     * handler returns. Since restart is called here, the flag needs to be
     * cleared manually.
     */
    brownout_ll_intr_clear();

    if (s_brownout_callback) {
        s_brownout_callback();
    }

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
        ESP_DRAM_LOGI(TAG, "Brownout detector was triggered\r\n\r\n");
    }

    // Flush any data left in UART FIFOs
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (uart_ll_is_enabled(i)) {
            esp_rom_output_tx_wait_idle(i);
        }
    }

    esp_rom_software_reset_system();

    ESP_INFINITE_LOOP();
}
#endif // CONFIG_ESP_BROWNOUT_USE_INTR

void esp_brownout_init(void)
{
#if CONFIG_ESP_BROWNOUT_USE_INTR
    brownout_hal_config_t cfg = {
        .threshold = BROWNOUT_DET_LVL,
        .enabled = true,
        .reset_enabled = false,
        .flash_power_down = true,
        .rf_power_down = true,
    };

    brownout_hal_config(&cfg);
    brownout_ll_intr_clear();

#if CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2
    rtc_isr_register(rtc_brownout_isr_handler, NULL, RTC_CNTL_BROWN_OUT_INT_ENA_M, RTC_INTR_FLAG_IRAM);
#else
    intr_handle_t bod_intr;
    esp_intr_alloc_intrstatus(power_supply_periph_signal.irq, ESP_INTR_FLAG_IRAM | ESP_INTR_FLAG_SHARED, (uint32_t)brownout_ll_intr_get_status_reg(), BROWNOUT_DETECTOR_LL_INTERRUPT_MASK, &rtc_brownout_isr_handler, NULL, &bod_intr);
#endif

    brownout_ll_intr_enable(true);

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
#if CONFIG_ESP_BROWNOUT_USE_INTR
    brownout_ll_intr_enable(false);
    rtc_isr_deregister(rtc_brownout_isr_handler, NULL);
#endif // CONFIG_ESP_BROWNOUT_USE_INTR
}

esp_err_t esp_brownout_register_callback(brownout_callback_t callback)
{
    if (callback != NULL) {
        ESP_RETURN_ON_FALSE(esp_ptr_in_iram(callback), ESP_ERR_INVALID_ARG, TAG, "brownout callback is not in IRAM");
    }
    s_brownout_callback = callback;

    return ESP_OK;
}
