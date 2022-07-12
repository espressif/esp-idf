/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "esp_private/panic_internal.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#if CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/memprot.h"
#elif CONFIG_IDF_TARGET_ESP32C2
#include "esp32c2/memprot.h"
#else
#include "esp_memprot.h"
#endif
#endif

#define SHUTDOWN_HANDLERS_NO 5

static shutdown_handler_t shutdown_handlers[SHUTDOWN_HANDLERS_NO];

void IRAM_ATTR esp_restart_noos_dig(void)
{
    // make sure all the panic handler output is sent from UART FIFO
    if (CONFIG_ESP_CONSOLE_UART_NUM >= 0) {
        esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);
    }

    // switch to XTAL (otherwise we will keep running from the PLL)
    rtc_clk_cpu_freq_set_xtal();

#if CONFIG_IDF_TARGET_ESP32
    esp_cpu_unstall(PRO_CPU_NUM);
#endif
    // reset the digital part
    esp_rom_software_reset_system();
    while (true) {
        ;
    }
}

esp_err_t esp_register_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            return ESP_ERR_INVALID_STATE;
        } else if (shutdown_handlers[i] == NULL) {
            shutdown_handlers[i] = handler;
            return ESP_OK;
        }
    }
    return ESP_ERR_NO_MEM;
}

esp_err_t esp_unregister_shutdown_handler(shutdown_handler_t handler)
{
    for (int i = 0; i < SHUTDOWN_HANDLERS_NO; i++) {
        if (shutdown_handlers[i] == handler) {
            shutdown_handlers[i] = NULL;
            return ESP_OK;
        }
    }
    return ESP_ERR_INVALID_STATE;
}


void IRAM_ATTR esp_restart(void)
{
    for (int i = SHUTDOWN_HANDLERS_NO - 1; i >= 0; i--) {
        if (shutdown_handlers[i]) {
            shutdown_handlers[i]();
        }
    }

#ifdef CONFIG_FREERTOS_SMP
    //Note: Scheduler suspension behavior changed in FreeRTOS SMP
    vTaskPreemptionDisable(NULL);
#else
    // Disable scheduler on this core.
    vTaskSuspendAll();
#endif // CONFIG_FREERTOS_SMP

    bool digital_reset_needed = false;
#if CONFIG_ESP_SYSTEM_MEMPROT_FEATURE
#if CONFIG_IDF_TARGET_ESP32S2
    if (esp_memprot_is_intr_ena_any() || esp_memprot_is_locked_any()) {
        digital_reset_needed = true;
    }
#else
    bool is_on = false;
    if (esp_mprot_is_intr_ena_any(&is_on) != ESP_OK || is_on) {
        digital_reset_needed = true;
    } else if (esp_mprot_is_conf_locked_any(&is_on) != ESP_OK || is_on) {
        digital_reset_needed = true;
    }
#endif
#endif
    if (digital_reset_needed) {
        esp_restart_noos_dig();
    }
    esp_restart_noos();
}

uint32_t esp_get_free_heap_size( void )
{
    return heap_caps_get_free_size( MALLOC_CAP_DEFAULT );
}

uint32_t esp_get_free_internal_heap_size( void )
{
    return heap_caps_get_free_size( MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL );
}

uint32_t esp_get_minimum_free_heap_size( void )
{
    return heap_caps_get_minimum_free_size( MALLOC_CAP_DEFAULT );
}

const char *esp_get_idf_version(void)
{
    return IDF_VER;
}

void __attribute__((noreturn)) esp_system_abort(const char *details)
{
    panic_abort(details);
}
