/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_system.h"
#include "esp_log.h"
#include "spi_flash_mmap.h"
#include "esp_flash_internal.h"
#include "esp_newlib.h"
#include "esp_xt_wdt.h"
#include "esp_cpu.h"
#include "esp_private/startup_internal.h"
#include "soc/soc_caps.h"
#include "hal/wdt_hal.h"
#include "hal/uart_types.h"
#include "hal/uart_ll.h"

#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
#include "private/esp_coexist_internal.h"
#endif

#if CONFIG_PM_ENABLE
#include "esp_pm.h"
#include "esp_private/pm_impl.h"
#endif

#include "esp_private/esp_clk.h"
#include "esp_private/spi_flash_os.h"
#include "esp_private/brownout.h"

#include "esp_rom_caps.h"
#include "esp_rom_sys.h"

#if SOC_BOD_SUPPORTED
#include "hal/brownout_ll.h"
#endif

// Using the same tag as in startup.c to keep the logs unchanged
static const char* TAG = "cpu_start";

// Hook to force the linker to include this file
void esp_system_include_startup_funcs(void)
{
}

// [refactor-todo] Most of these init functions should be moved to the respective components.

ESP_SYSTEM_INIT_FN(init_show_cpu_freq, CORE, BIT(0), 10)
{
    ESP_EARLY_LOGI(TAG, "Pro cpu start user code");
    int cpu_freq = esp_clk_cpu_freq();
    ESP_EARLY_LOGI(TAG, "cpu freq: %d Hz", cpu_freq);

    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(init_brownout, CORE, BIT(0), 104)
{
    // [refactor-todo] leads to call chain rtc_is_register (driver) -> esp_intr_alloc (esp32/esp32s2) ->
    // malloc (newlib) -> heap_caps_malloc (heap), so heap must be at least initialized
#if CONFIG_ESP_BROWNOUT_DET
    esp_brownout_init();
#else
#if SOC_CAPS_NO_RESET_BY_ANA_BOD
    brownout_ll_ana_reset_enable(false);
#endif // SOC_CAPS_NO_RESET_BY_ANA_BOD
#endif // CONFIG_ESP_BROWNOUT_DET
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(init_newlib_time, CORE, BIT(0), 105)
{
    esp_newlib_time_init();
    return ESP_OK;
}

#if !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
ESP_SYSTEM_INIT_FN(init_flash, CORE, BIT(0), 130)
{
#if CONFIG_SPI_FLASH_ROM_IMPL
    spi_flash_rom_impl_init();
#endif

    esp_flash_app_init();
    esp_err_t flash_ret = esp_flash_init_default_chip();
    assert(flash_ret == ESP_OK);
    (void)flash_ret;
#if CONFIG_SPI_FLASH_BROWNOUT_RESET
    spi_flash_needs_reset_check();
#endif // CONFIG_SPI_FLASH_BROWNOUT_RESET
    return ESP_OK;
}
#endif // !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP

#if CONFIG_ESP_XT_WDT
ESP_SYSTEM_INIT_FN(init_xt_wdt, CORE, BIT(0), 170)
{
    esp_xt_wdt_config_t cfg = {
        .timeout                = CONFIG_ESP_XT_WDT_TIMEOUT,
        .auto_backup_clk_enable = CONFIG_ESP_XT_WDT_BACKUP_CLK_ENABLE,
    };
    return esp_xt_wdt_init(&cfg);
}
#endif // CONFIG_ESP_XT_WDT

#if CONFIG_PM_ENABLE
ESP_SYSTEM_INIT_FN(init_pm, SECONDARY, BIT(0), 201)
{
    esp_pm_impl_init();
    return ESP_OK;
}
#endif // CONFIG_PM_ENABLE

#if SOC_APB_BACKUP_DMA
ESP_SYSTEM_INIT_FN(init_apb_dma, SECONDARY, BIT(0), 203)
{
    extern void esp_apb_backup_dma_lock_init(void);
    esp_apb_backup_dma_lock_init();
    return ESP_OK;
}
#endif

#if CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE
ESP_SYSTEM_INIT_FN(init_coexist, SECONDARY, BIT(0), 204)
{
    esp_coex_adapter_register(&g_coex_adapter_funcs);
    coex_pre_init();
    return ESP_OK;
}
#endif // CONFIG_SW_COEXIST_ENABLE || CONFIG_EXTERNAL_COEX_ENABLE

#ifndef CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE
ESP_SYSTEM_INIT_FN(init_disable_rtc_wdt, SECONDARY, BIT(0), 999)
{
    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_disable(&rtc_wdt_ctx);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
    return ESP_OK;
}
#endif // CONFIG_BOOTLOADER_WDT_DISABLE_IN_USER_CODE
