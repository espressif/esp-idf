/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "sdkconfig.h"
#include "esp_macros.h"
#include "esp_system.h"
#include "esp_private/system_internal.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "riscv/rv_utils.h"
#include "esp_rom_serial_output.h"
#include "soc/gpio_reg.h"
#include "esp_cpu.h"
#include "soc/rtc.h"
#include "esp_private/rtc_clk.h"
#include "soc/uart_reg.h"
#include "hal/wdt_hal.h"
#include "esp_private/cache_err_int.h"

#include "esp32p4/rom/cache.h"
#include "esp32p4/rom/rtc.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/hp_system_reg.h"
#include "hal/uart_ll.h"
#include "hal/gdma_ll.h"
#include "hal/axi_dma_ll.h"
#include "hal/dw_gdma_ll.h"
#include "hal/dma2d_ll.h"
#include "hal/efuse_hal.h"

void esp_system_reset_modules_on_exit(void)
{
    // Flush any data left in UART FIFOs
    for (int i = 0; i < SOC_UART_HP_NUM; ++i) {
        if (uart_ll_is_enabled(i)) {
            esp_rom_output_tx_wait_idle(i);
        }
    }

    // Note: AXI bus doesn't allow an undergoing transaction to be interrupted in the middle
    // If you want to reset a AXI master, you should make sure that the master is in IDLE first
    if (gdma_ll_is_bus_clock_enabled(1)) {
        for (int i = 0; i < GDMA_LL_AXI_PAIRS_PER_GROUP; i++) {
            axi_dma_ll_tx_abort(AXI_DMA_LL_GET_HW(0), i, true);
            axi_dma_ll_rx_abort(AXI_DMA_LL_GET_HW(0), i, true);
            while (!axi_dma_ll_tx_is_reset_avail(AXI_DMA_LL_GET_HW(0), i));
            while (!axi_dma_ll_rx_is_reset_avail(AXI_DMA_LL_GET_HW(0), i));
        }
    }
    if (dma2d_ll_is_bus_clock_enabled(0)) {
        for (int i = 0; i < DMA2D_LL_GET(RX_CHANS_PER_INST); i++) {
            dma2d_ll_rx_abort(DMA2D_LL_GET_HW(0), i, true);
            while (!dma2d_ll_rx_is_reset_avail(DMA2D_LL_GET_HW(0), i));
        }
        for (int i = 0; i < DMA2D_LL_GET(TX_CHANS_PER_INST); i++) {
            dma2d_ll_tx_abort(DMA2D_LL_GET_HW(0), i, true);
            while (!dma2d_ll_tx_is_reset_avail(DMA2D_LL_GET_HW(0), i));
        }
    }
    if (dw_gdma_ll_is_bus_clock_enabled(0)) {
        for (int i = 0; i < DW_GDMA_LL_CHANNELS_PER_GROUP; i++) {
            dw_gdma_ll_channel_abort(DW_GDMA_LL_GET_HW(0), i);
        }
    }

    // Set Peripheral clk rst
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_DUAL_MSPI_AXI);
    // DMA needs to be reset to avoid memory corruption after restart. Now only AHB supports this.
    // For other AXI DMAs, we have already stop them above.
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_AHB_PDMA);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_TIMERGRP1);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_STIMER);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART0_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART1_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART2_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART3_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART4_CORE);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_ADC);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_H264);

    // Clear Peripheral clk rst
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_MSPI_AXI);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN0_REG, HP_SYS_CLKRST_REG_RST_EN_DUAL_MSPI_AXI);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_AHB_PDMA);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_TIMERGRP1);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_STIMER);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART0_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART1_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART2_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART3_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN1_REG, HP_SYS_CLKRST_REG_RST_EN_UART4_CORE);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_ADC);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_H264);

    // The DMA inside SDMMC Host needs to be reset to avoid memory corruption after restart.
    SET_PERI_REG_MASK(LP_CLKRST_HP_SDMMC_EMAC_RST_CTRL_REG, LP_CLKRST_RST_EN_SDMMC);
    CLEAR_PERI_REG_MASK(LP_CLKRST_HP_SDMMC_EMAC_RST_CTRL_REG, LP_CLKRST_RST_EN_SDMMC);

    // Reset crypto peripherals. This ensures a clean state for the crypto peripherals after a CPU restart
    // and hence avoiding any possibility with crypto failure in ROM security workflows.
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_CRYPTO);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_AES);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_DS);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_ECC);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_ECDSA);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_HMAC);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_KM);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_RSA);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_SHA);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_CRYPTO);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_AES);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_DS);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_ECC);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_ECDSA);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_HMAC);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_KM);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_RSA);
    CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_SHA);

#if CONFIG_ESP32P4_REV_MIN_FULL < 101
    if (efuse_hal_chip_revision() < 101) {
        // enable soc clk and reset parent crypto
        SET_PERI_REG_MASK(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_CRYPTO_SYS_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_CRYPTO);
        CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_CRYPTO);

        // enable soc clk for key manager
        SET_PERI_REG_MASK(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_KEY_MANAGER_SYS_CLK_EN);

        // enable key manager peripheral clock and reset
        SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_CRYPTO_KM_CLK_EN);
        SET_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_KM);
        CLEAR_PERI_REG_MASK(HP_SYS_CLKRST_HP_RST_EN2_REG, HP_SYS_CLKRST_REG_RST_EN_KM);
    }
#endif

#if CONFIG_ESP32P4_REV_MIN_FULL == 0
    // enable MPI, SHA and ECDSA peripheral clocks
    SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_CRYPTO_RSA_CLK_EN);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_CRYPTO_SHA_CLK_EN);
    SET_PERI_REG_MASK(HP_SYS_CLKRST_PERI_CLK_CTRL25_REG, HP_SYS_CLKRST_REG_CRYPTO_ECDSA_CLK_EN);
#endif
}

/* "inner" restart function for after RTOS, interrupts & anything else on this
 * core are already stopped. Stalls other core, resets hardware,
 * triggers restart.
*/
void esp_restart_noos(void)
{
    // Disable interrupts
    rv_utils_intr_global_disable();
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

    const uint32_t core_id = esp_cpu_get_core_id();
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    const uint32_t other_core_id = (core_id == 0) ? 1 : 0;
    esp_cpu_reset(other_core_id);
    esp_cpu_stall(other_core_id);
#endif

    // Disable TG0/TG1 watchdogs
    wdt_hal_context_t wdt0_context = {.inst = WDT_MWDT0, .mwdt_dev = &TIMERG0};
    wdt_hal_write_protect_disable(&wdt0_context);
    wdt_hal_disable(&wdt0_context);
    wdt_hal_write_protect_enable(&wdt0_context);

    wdt_hal_context_t wdt1_context = {.inst = WDT_MWDT1, .mwdt_dev = &TIMERG1};
    wdt_hal_write_protect_disable(&wdt1_context);
    wdt_hal_disable(&wdt1_context);
    wdt_hal_write_protect_enable(&wdt1_context);

    // Disable cache
#if CONFIG_SPIRAM
    Cache_WriteBack_All(CACHE_MAP_L1_DCACHE);
#endif
    Cache_Disable_L2_Cache();

    esp_system_reset_modules_on_exit();

    // Set CPU back to XTAL source (and MEM_CLK, APB_CLK back to power-on reset frequencies), same as hard reset, keep CPLL on.
#if !CONFIG_IDF_ENV_FPGA
    rtc_clk_cpu_set_to_default_config();
#endif

#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    // clear entry point for APP CPU
    ets_set_appcpu_boot_addr(0);
#endif

#if CONFIG_SPIRAM_INSTRUCTIONS_RODATA
    // disable remap if enabled in menuconfig
    REG_CLR_BIT(HP_SYS_HP_PSRAM_FLASH_ADDR_INTERCHANGE_REG, HP_SYS_HP_PSRAM_FLASH_ADDR_INTERCHANGE_DMA | HP_SYS_HP_PSRAM_FLASH_ADDR_INTERCHANGE_CPU);
#endif

    // Reset CPUs
    if (core_id == 0) {
        // Running on PRO CPU: APP CPU is stalled. Can reset both CPUs.
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
        esp_cpu_reset(1);
#endif
        esp_cpu_reset(0);
    }
#if !CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    else {
        // Running on APP CPU: need to reset PRO CPU and unstall it,
        // then reset APP CPU
        esp_cpu_reset(0);
        esp_cpu_unstall(0);
        esp_cpu_reset(1);
    }
#endif

    ESP_INFINITE_LOOP();
}
