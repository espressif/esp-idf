
/*
 * SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_macros.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_serial_output.h"
#include "soc/soc_caps.h"
#include "soc/dport_reg.h"
#include "soc/gpio_reg.h"
#include "soc/timer_group_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "soc/syscon_reg.h"
#include "hal/wdt_hal.h"
#include "hal/uart_ll.h"
#include "soc/soc_memory_layout.h"

#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/rtc.h"

#define ALIGN_DOWN(val, align)  ((val) & ~((align) - 1))

extern int _bss_end;

void esp_system_reset_modules_on_exit(void)
{
    // Flush any data left in UART FIFOs before reset the UART peripheral
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (uart_ll_is_enabled(i)) {
            esp_rom_output_tx_wait_idle(i);
        }
    }

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG,
                      SYSTEM_WIFIBB_RST | SYSTEM_FE_RST | SYSTEM_WIFIMAC_RST | SYSTEM_SDIO_RST |
                      SYSTEM_EMAC_RST | SYSTEM_MACPWR_RST | SYSTEM_BTBB_RST | SYSTEM_BTBB_REG_RST |
                      SYSTEM_RW_BTMAC_RST | SYSTEM_RW_BTLP_RST | SYSTEM_RW_BTMAC_REG_RST | SYSTEM_RW_BTLP_REG_RST);
    REG_WRITE(SYSTEM_CORE_RST_EN_REG, 0);

    // Reset timer, systimer, spi, uart, mcpwm
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN0_REG,
                      SYSTEM_TIMERS_RST | SYSTEM_SPI01_RST | SYSTEM_UART_RST | SYSTEM_SYSTIMER_RST |
                      SYSTEM_PWM0_RST | SYSTEM_PWM1_RST);
    REG_WRITE(SYSTEM_PERIP_RST_EN0_REG, 0);

    // Reset dma and crypto peripherals. This ensures a clean state for the crypto peripherals after a CPU restart
    // and hence avoiding any possibility with crypto failure in ROM security workflows.
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_DMA_RST | SYSTEM_CRYPTO_AES_RST | SYSTEM_CRYPTO_DS_RST |
                      SYSTEM_CRYPTO_HMAC_RST | SYSTEM_CRYPTO_RSA_RST | SYSTEM_CRYPTO_SHA_RST |
                      // The DMA inside SDMMC Host needs to be reset to avoid memory corruption after restart.
                      SYSTEM_SDIO_HOST_RST);
    REG_WRITE(SYSTEM_PERIP_RST_EN1_REG, 0);

    SET_PERI_REG_MASK(SYSTEM_EDMA_CTRL_REG, SYSTEM_EDMA_RESET);
    CLEAR_PERI_REG_MASK(SYSTEM_EDMA_CTRL_REG, SYSTEM_EDMA_RESET);
}

/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void esp_restart_noos(void)
{
    // Disable interrupts
    esp_cpu_intr_disable(0xFFFFFFFF);

    // Enable RTC watchdog for 1 second
    wdt_hal_context_t rtc_wdt_ctx;
    wdt_hal_init(&rtc_wdt_ctx, WDT_RWDT, 0, false);
    uint32_t stage_timeout_ticks = (uint32_t)(1000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_SYSTEM);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE1, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    //Enable flash boot mode so that flash booting after restart is protected by the RTC WDT.
    wdt_hal_set_flashboot_en(&rtc_wdt_ctx, true);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);

    // Disable TG0/TG1 watchdogs
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

#ifdef CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM
    if (esp_ptr_external_ram(esp_cpu_get_sp())) {
        // If stack_addr is from External Memory (CONFIG_FREERTOS_TASK_CREATE_ALLOW_EXT_MEM is used)
        // then need to switch SP to Internal Memory otherwise
        // we will get the "Cache disabled but cached memory region accessed" error after Cache_Read_Disable.
        uint32_t new_sp = ALIGN_DOWN((uint32_t)&_bss_end, 16);
        SET_STACK(new_sp);
    }
#endif

    // Reset and stall the other CPU.
    // CPU must be reset before stalling, in case it was running a s32c1i
    // instruction. This would cause memory pool to be locked by arbiter
    // to the stalled CPU, preventing current CPU from accessing this pool.
    const uint32_t core_id = esp_cpu_get_core_id();
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_rom_software_reset_cpu(other_core_id);
    esp_cpu_stall(other_core_id);
#endif

    // Disable cache
    Cache_Disable_ICache();
    Cache_Disable_DCache();

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC1_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC2_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC3_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC4_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC5_IN_SEL_CFG_REG, 0x30);

    // reset necessary peripheral modules
    esp_system_reset_modules_on_exit();

    // Set CPU back to XTAL source, same as hard reset, but keep BBPLL on so that USB Serial JTAG can log at 1st stage bootloader.
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_set_to_default_config();
#endif

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // Clear entry point for APP CPU
    REG_WRITE(SYSTEM_CORE_1_CONTROL_1_REG, 0);
#endif

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        esp_rom_software_reset_cpu(1);
#endif
        esp_rom_software_reset_cpu(0);
    }
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_rom_software_reset_cpu(0);
        esp_cpu_unstall(0);
        esp_rom_software_reset_cpu(1);
    }
#endif

    ESP_INFINITE_LOOP();
}
