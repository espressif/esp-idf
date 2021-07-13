/*
 * SPDX-FileCopyrightText: 2017-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/efuse_periph.h"
#include "soc/rtc_cntl_reg.h"
#if CONFIG_IDF_TARGET_ESP32
#include "soc/dport_reg.h"
#endif
#include "esp_rom_sys.h"
#include "esp_rom_uart.h"

__attribute__((weak)) void bootloader_clock_configure(void)
{
    // ROM bootloader may have put a lot of text into UART0 FIFO.
    // Wait for it to be printed.
    // This is not needed on power on reset, when ROM bootloader is running at
    // 40 MHz. But in case of TG WDT reset, CPU may still be running at >80 MHZ,
    // and will be done with the bootloader much earlier than UART FIFO is empty.
    esp_rom_uart_tx_wait_idle(0);

    /* Set CPU to 80MHz. Keep other clocks unmodified. */
    int cpu_freq_mhz = 80;

#if CONFIG_IDF_TARGET_ESP32
    /* On ESP32 rev 0, switching to 80/160 MHz if clock was previously set to
     * 240 MHz may cause the chip to lock up (see section 3.5 of the errata
     * document). For rev. 0, switch to 240 instead if it has been enabled
     * previously.
     */
    uint32_t chip_ver_reg = REG_READ(EFUSE_BLK0_RDATA3_REG);
    if ((chip_ver_reg & EFUSE_RD_CHIP_VER_REV1_M) == 0 &&
            DPORT_REG_GET_FIELD(DPORT_CPU_PER_CONF_REG, DPORT_CPUPERIOD_SEL) == DPORT_CPUPERIOD_SEL_240) {
        cpu_freq_mhz = 240;
    }
#endif

    if (rtc_clk_apb_freq_get() < APB_CLK_FREQ || esp_rom_get_reset_reason(0) != RESET_REASON_CPU0_SW) {
        rtc_clk_config_t clk_cfg = RTC_CLK_CONFIG_DEFAULT();
#if CONFIG_IDF_TARGET_ESP32
        clk_cfg.xtal_freq = CONFIG_ESP32_XTAL_FREQ;
#endif
        /* ESP32-S2 doesn't have XTAL_FREQ choice, always 40MHz */
        clk_cfg.cpu_freq_mhz = cpu_freq_mhz;
        clk_cfg.slow_freq = rtc_clk_slow_freq_get();
        clk_cfg.fast_freq = rtc_clk_fast_freq_get();
        rtc_clk_init(clk_cfg);
    }

    /* As a slight optimization, if 32k XTAL was enabled in sdkconfig, we enable
     * it here. Usually it needs some time to start up, so we amortize at least
     * part of the start up time by enabling 32k XTAL early.
     * App startup code will wait until the oscillator has started up.
     */
#if CONFIG_ESP_SYSTEM_RTC_EXT_XTAL
    if (!rtc_clk_32k_enabled()) {
        rtc_clk_32k_bootstrap(CONFIG_ESP_SYSTEM_RTC_EXT_XTAL_BOOTSTRAP_CYCLES);
    }
#endif // CONFIG_ESP_SYSTEM_RTC_EXT_XTAL

    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
}
