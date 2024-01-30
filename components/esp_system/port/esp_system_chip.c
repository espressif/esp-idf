/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_cpu.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "esp_private/rtc_clk.h"
#include "esp_private/panic_internal.h"
#include "esp_private/system_internal.h"
#include "esp_private/mspi_timing_tuning.h"
#include "esp_heap_caps.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "sdkconfig.h"

// used only by ESP32 panic handler
#ifdef CONFIG_IDF_TARGET_ESP32
void IRAM_ATTR esp_restart_noos_dig(void)
{
    // In case any of the calls below results in re-enabling of interrupts
    // (for example, by entering a critical section), disable all the
    // interrupts (e.g. from watchdogs) here.
#ifdef CONFIG_IDF_TARGET_ARCH_RISCV
    rv_utils_intr_global_disable();
#else
    xt_ints_off(0xFFFFFFFF);
#endif

    if (CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM != -1) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
    }

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    /**
     * Turn down MSPI speed
     *
     * We set MSPI clock to a high speed one before, ROM doesn't have such high speed clock source option.
     * This function will change clock source to a ROM supported one when system restarts.
     */
    mspi_timing_change_speed_mode_cache_safe(true);
#endif  //#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

    // switch to XTAL (otherwise we will keep running from the PLL)
    rtc_clk_cpu_set_to_default_config();

    // esp_restart_noos_dig() will generates a core reset, which does not reset the
    // registers of the RTC domain, so the CPU's stall state remains after the reset,
    // we need to release them here
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // unstall all other cores
    int core_id = esp_cpu_get_core_id();
    for (uint32_t i = 0; i < SOC_CPU_CORES_NUM; i++) {
        if (i != core_id) {
            esp_cpu_unstall(i);
        }
    }
#endif
    // generate core reset
    esp_rom_software_reset_system();
    while (true) {
        ;
    }
}
#endif

uint32_t esp_get_free_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
}

uint32_t esp_get_free_internal_heap_size(void)
{
    return heap_caps_get_free_size(MALLOC_CAP_8BIT | MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
}

uint32_t esp_get_minimum_free_heap_size(void)
{
    return heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
}

const char *esp_get_idf_version(void)
{
    return IDF_VER;
}

void __attribute__((noreturn)) esp_system_abort(const char *details)
{
    panic_abort(details);
}
