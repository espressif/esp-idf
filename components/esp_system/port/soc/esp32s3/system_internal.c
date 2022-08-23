
/*
 * SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_uart.h"
#include "soc/dport_reg.h"
#include "soc/gpio_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/timer_group_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "soc/syscon_reg.h"
#include "soc/rtc_periph.h"
#include "hal/wdt_hal.h"
#include "freertos/xtensa_api.h"
#include "soc/soc_memory_layout.h"

#include "esp32s3/rom/cache.h"
#include "esp32s3/rom/rtc.h"

#define ALIGN_DOWN(val, align)  ((val) & ~((align) - 1))

extern int _bss_end;

/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void IRAM_ATTR esp_restart_noos(void)
{
    // Disable interrupts
    xt_ints_off(0xFFFFFFFF);

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

    // Flush any data left in UART FIFOs
    esp_rom_uart_tx_wait_idle(0);
    esp_rom_uart_tx_wait_idle(1);

#ifdef CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY
    if (esp_ptr_external_ram(esp_cpu_get_sp())) {
        // If stack_addr is from External Memory (CONFIG_SPIRAM_ALLOW_STACK_EXTERNAL_MEMORY is used)
        // then need to switch SP to Internal Memory otherwise
        // we will get the "Cache disabled but cached memory region accessed" error after Cache_Read_Disable.
        uint32_t new_sp = ALIGN_DOWN(_bss_end, 16);
        SET_STACK(new_sp);
    }
#endif

    // Disable cache
    Cache_Disable_ICache();
    Cache_Disable_DCache();

    // Reset and stall the other CPU.
    // CPU must be reset before stalling, in case it was running a s32c1i
    // instruction. This would cause memory pool to be locked by arbiter
    // to the stalled CPU, preventing current CPU from accessing this pool.
    const uint32_t core_id = esp_cpu_get_core_id();
#if !CONFIG_FREERTOS_UNICORE
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);
#endif

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC1_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC2_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC3_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC4_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC5_IN_SEL_CFG_REG, 0x30);

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    SET_PERI_REG_MASK(SYSTEM_CORE_RST_EN_REG,
                      SYSTEM_WIFIBB_RST | SYSTEM_FE_RST | SYSTEM_WIFIMAC_RST |
                      SYSTEM_SDIO_RST | SYSTEM_EMAC_RST | SYSTEM_MACPWR_RST |
                      SYSTEM_BTBB_RST | SYSTEM_BTBB_REG_RST |
                      SYSTEM_RW_BTMAC_RST | SYSTEM_RW_BTLP_RST | SYSTEM_RW_BTMAC_REG_RST | SYSTEM_RW_BTLP_REG_RST);
    REG_WRITE(SYSTEM_CORE_RST_EN_REG, 0);

    // Reset timer/spi/uart
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN0_REG,
                      SYSTEM_TIMERS_RST | SYSTEM_SPI01_RST | SYSTEM_UART_RST | SYSTEM_SYSTIMER_RST);
    REG_WRITE(SYSTEM_PERIP_RST_EN0_REG, 0);

    // Reset dma
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN1_REG, SYSTEM_DMA_RST);
    REG_WRITE(SYSTEM_PERIP_RST_EN1_REG, 0);

    SET_PERI_REG_MASK(SYSTEM_EDMA_CTRL_REG, SYSTEM_EDMA_RESET);
    CLEAR_PERI_REG_MASK(SYSTEM_EDMA_CTRL_REG, SYSTEM_EDMA_RESET);

    // Set CPU back to XTAL source, no PLL, same as hard reset
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_freq_set_xtal();
#endif

#if !CONFIG_FREERTOS_UNICORE
    // Clear entry point for APP CPU
    REG_WRITE(SYSTEM_CORE_1_CONTROL_1_REG, 0);
#endif

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
#if !CONFIG_FREERTOS_UNICORE
        esp_cpu_reset(1);
#endif
        esp_cpu_reset(0);
    }
#if !CONFIG_FREERTOS_UNICORE
    else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
#endif
    while (true) {
        ;
    }
}
