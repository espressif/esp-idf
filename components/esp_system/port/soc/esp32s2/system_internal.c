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
#include "esp_efuse.h"
#include "esp_log.h"
#include "esp32s2/rom/cache.h"
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

#include "esp32s2/rom/rtc.h"

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

    // Reset and stall the other CPU.
    // CPU must be reset before stalling, in case it was running a s32c1i
    // instruction. This would cause memory pool to be locked by arbiter
    // to the stalled CPU, preventing current CPU from accessing this pool.
    const uint32_t core_id = esp_cpu_get_core_id();

    //Todo: Refactor to use Interrupt or Task Watchdog API, and a system level WDT context
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

    // 2nd stage bootloader reconfigures SPI flash signals.
    // Reset them to the defaults expected by ROM.
    WRITE_PERI_REG(GPIO_FUNC0_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC1_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC2_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC3_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC4_IN_SEL_CFG_REG, 0x30);
    WRITE_PERI_REG(GPIO_FUNC5_IN_SEL_CFG_REG, 0x30);

    // Reset wifi/bluetooth/ethernet/sdio (bb/mac)
    DPORT_SET_PERI_REG_MASK(DPORT_CORE_RST_EN_REG, DPORT_WIFIBB_RST   | \
                                                   DPORT_FE_RST       | \
                                                   DPORT_WIFIMAC_RST  | \
                                                   DPORT_BTBB_RST     | \
                                                   DPORT_BTMAC_RST    | \
                                                   DPORT_SDIO_RST     | \
                                                   DPORT_EMAC_RST     | \
                                                   DPORT_MACPWR_RST   | \
                                                   DPORT_RW_BTMAC_RST | \
                                                   DPORT_RW_BTLP_RST);
    DPORT_REG_WRITE(DPORT_CORE_RST_EN_REG, 0);

    // Reset timer/spi/uart
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG,
                            DPORT_TIMERS_RST | DPORT_SPI01_RST | DPORT_SPI2_RST | DPORT_SPI3_RST | DPORT_SPI2_DMA_RST | DPORT_SPI3_DMA_RST | DPORT_UART_RST);
    DPORT_REG_WRITE(DPORT_PERIP_RST_EN_REG, 0);

    // Set CPU back to XTAL source, no PLL, same as hard reset
    rtc_clk_cpu_freq_set_xtal();

    // Reset CPUs
    if (core_id == 0) {
        esp_cpu_reset(0);
    }
    while (true) {
        ;
    }
}
