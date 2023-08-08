/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/chip_revision.h"
#include "hal/efuse_hal.h"

#if !CONFIG_IDF_TARGET_ESP32C6 && !CONFIG_IDF_TARGET_ESP32H2 && !CONFIG_IDF_TARGET_ESP32P4// TODO: IDF-5645
#include "soc/rtc_cntl_reg.h"
#else
#include "soc/lp_wdt_reg.h"
#include "soc/lp_timer_reg.h"
#include "soc/lp_analog_peri_reg.h"
#include "soc/pmu_reg.h"
#endif

#if CONFIG_IDF_TARGET_ESP32
#include "hal/clk_tree_ll.h"
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

    /* Set CPU to a higher certain frequency. Keep other clocks unmodified. */
    int cpu_freq_mhz = CPU_CLK_FREQ_MHZ_BTLD;

#if CONFIG_IDF_TARGET_ESP32
    /* On ESP32 rev 0, switching to 80/160 MHz if clock was previously set to
     * 240 MHz may cause the chip to lock up (see section 3.5 of the errata
     * document). For rev. 0, switch to 240 instead if it has been enabled
     * previously.
     */
    if (!ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 100) &&
            clk_ll_cpu_get_freq_mhz_from_pll() == CLK_LL_PLL_240M_FREQ_MHZ) {
        cpu_freq_mhz = 240;
    }
#endif

    if (esp_rom_get_reset_reason(0) != RESET_REASON_CPU0_SW || rtc_clk_apb_freq_get() < APB_CLK_FREQ) {
        rtc_clk_config_t clk_cfg = RTC_CLK_CONFIG_DEFAULT();

        clk_cfg.cpu_freq_mhz = cpu_freq_mhz;

        // Use RTC_SLOW clock source sel register field's default value, RC_SLOW, for 2nd stage bootloader
        // RTC_SLOW clock source will be switched according to Kconfig selection at application startup
        clk_cfg.slow_clk_src = rtc_clk_slow_src_get();
        if (clk_cfg.slow_clk_src == SOC_RTC_SLOW_CLK_SRC_INVALID) {
            clk_cfg.slow_clk_src = SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
        }

#if CONFIG_IDF_TARGET_ESP32C6
        // TODO: IDF-5781 Some of esp32c6 SOC_RTC_FAST_CLK_SRC_XTAL_D2 rtc_fast clock has timing issue
        // Force to use SOC_RTC_FAST_CLK_SRC_RC_FAST since 2nd stage bootloader
        clk_cfg.fast_clk_src = SOC_RTC_FAST_CLK_SRC_RC_FAST;
#else
        // Use RTC_FAST clock source sel register field's default value, XTAL_DIV, for 2nd stage bootloader
        // RTC_FAST clock source will be switched to RC_FAST at application startup
        clk_cfg.fast_clk_src = rtc_clk_fast_src_get();
        if (clk_cfg.fast_clk_src == SOC_RTC_FAST_CLK_SRC_INVALID) {
            clk_cfg.fast_clk_src = SOC_RTC_FAST_CLK_SRC_XTAL_DIV;
        }
#endif
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

#if CONFIG_IDF_TARGET_ESP32C6
    // CLR ENA
    CLEAR_PERI_REG_MASK(LP_WDT_INT_ENA_REG, LP_WDT_SUPER_WDT_INT_ENA);                                      /* SWD */
    CLEAR_PERI_REG_MASK(LP_TIMER_LP_INT_ENA_REG, LP_TIMER_MAIN_TIMER_LP_INT_ENA);                           /* MAIN_TIMER */
    CLEAR_PERI_REG_MASK(LP_ANALOG_PERI_LP_ANA_LP_INT_ENA_REG, LP_ANALOG_PERI_LP_ANA_BOD_MODE0_LP_INT_ENA);  /* BROWN_OUT */
    CLEAR_PERI_REG_MASK(LP_WDT_INT_ENA_REG, LP_WDT_LP_WDT_INT_ENA);                                         /* WDT */
    CLEAR_PERI_REG_MASK(PMU_HP_INT_ENA_REG, PMU_SOC_WAKEUP_INT_ENA);                                        /* SLP_REJECT */
    CLEAR_PERI_REG_MASK(PMU_HP_INT_ENA_REG, PMU_SOC_SLEEP_REJECT_INT_ENA);                                  /* SLP_WAKEUP */
    // SET CLR
    SET_PERI_REG_MASK(LP_WDT_INT_CLR_REG, LP_WDT_SUPER_WDT_INT_CLR);                                        /* SWD */
    SET_PERI_REG_MASK(LP_TIMER_LP_INT_CLR_REG, LP_TIMER_MAIN_TIMER_LP_INT_CLR);                             /* MAIN_TIMER */
    SET_PERI_REG_MASK(LP_ANALOG_PERI_LP_ANA_LP_INT_CLR_REG, LP_ANALOG_PERI_LP_ANA_BOD_MODE0_LP_INT_CLR);    /* BROWN_OUT */
    SET_PERI_REG_MASK(LP_WDT_INT_CLR_REG, LP_WDT_LP_WDT_INT_CLR);                                           /* WDT */
#elif CONFIG_IDF_TARGET_ESP32H2
    // CLR ENA
    CLEAR_PERI_REG_MASK(LP_WDT_INT_ENA_REG, LP_WDT_SUPER_WDT_INT_ENA);                                      /* SWD */
    CLEAR_PERI_REG_MASK(LP_ANALOG_PERI_LP_ANA_LP_INT_ENA_REG, LP_ANALOG_PERI_LP_ANA_BOD_MODE0_LP_INT_ENA);  /* BROWN_OUT */
    CLEAR_PERI_REG_MASK(LP_WDT_INT_ENA_REG, LP_WDT_LP_WDT_INT_ENA);                                         /* WDT */
    CLEAR_PERI_REG_MASK(PMU_HP_INT_ENA_REG, PMU_SOC_WAKEUP_INT_ENA);                                        /* SLP_REJECT */
    CLEAR_PERI_REG_MASK(PMU_HP_INT_ENA_REG, PMU_SOC_SLEEP_REJECT_INT_ENA);                                  /* SLP_WAKEUP */
    // SET CLR
    SET_PERI_REG_MASK(LP_WDT_INT_CLR_REG, LP_WDT_SUPER_WDT_INT_CLR);                                        /* SWD */
    SET_PERI_REG_MASK(LP_ANALOG_PERI_LP_ANA_LP_INT_CLR_REG, LP_ANALOG_PERI_LP_ANA_BOD_MODE0_LP_INT_CLR);    /* BROWN_OUT */
    SET_PERI_REG_MASK(LP_WDT_INT_CLR_REG, LP_WDT_LP_WDT_INT_CLR);                                           /* WDT */
    SET_PERI_REG_MASK(PMU_HP_INT_CLR_REG, PMU_SOC_WAKEUP_INT_CLR);                                          /* SLP_REJECT */
    SET_PERI_REG_MASK(PMU_HP_INT_CLR_REG, PMU_SOC_SLEEP_REJECT_INT_CLR);                                    /* SLP_WAKEUP */
#elif CONFIG_IDF_TARGET_ESP32P4
// TODO: IDF-8008
#else
    REG_WRITE(RTC_CNTL_INT_ENA_REG, 0);
    REG_WRITE(RTC_CNTL_INT_CLR_REG, UINT32_MAX);
#endif
}
