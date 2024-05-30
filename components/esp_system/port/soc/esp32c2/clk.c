/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_cpu.h"
#include "esp_private/esp_clk.h"
#include "esp_clk_internal.h"
#include "esp32c2/rom/ets_sys.h"
#include "esp32c2/rom/uart.h"
#include "esp32c2/rom/rtc.h"
#include "soc/system_reg.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "hal/wdt_hal.h"
#include "esp_private/periph_ctrl.h"
#include "bootloader_clock.h"
#include "soc/syscon_reg.h"
#include "esp_rom_uart.h"

/* Number of cycles to wait from the 32k XTAL oscillator to consider it running.
 * Larger values increase startup delay. Smaller values may cause false positive
 * detection (i.e. oscillator runs for a few cycles and then stops).
 */
#define SLOW_CLK_CAL_CYCLES     CONFIG_RTC_CLK_CAL_CYCLES

#define MHZ (1000000)

/* Indicates that this 32k oscillator gets input from external oscillator, rather
 * than a crystal.
 */
#define EXT_OSC_FLAG    BIT(3)

/* This is almost the same as soc_rtc_slow_clk_src_t, except that we define
 * an extra enum member for the external 32k oscillator.
 * For convenience, lower 2 bits should correspond to soc_rtc_slow_clk_src_t values.
 */
typedef enum {
    SLOW_CLK_RTC = SOC_RTC_SLOW_CLK_SRC_RC_SLOW,                       //!< Internal 150 kHz RC oscillator
    SLOW_CLK_8MD256 = SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256,               //!< Internal 8 MHz RC oscillator, divided by 256
    SLOW_CLK_32K_EXT_OSC = SOC_RTC_SLOW_CLK_SRC_OSC_SLOW | EXT_OSC_FLAG //!< External 32k oscillator connected to pin0
} slow_clk_sel_t;

static void select_rtc_slow_clk(slow_clk_sel_t slow_clk);
static __attribute__((unused)) void recalib_bbpll(void);

static const char *TAG = "clk";

void esp_rtc_init(void)
{
#if CONFIG_ESP_SYSTEM_BBPLL_RECALIB
    // In earlier version of ESP-IDF, the PLL provided by bootloader is not stable enough.
    // Do calibration again here so that we can use better clock for the timing tuning.
    recalib_bbpll();
#endif

#if !CONFIG_IDF_ENV_FPGA
    rtc_config_t cfg = RTC_CONFIG_DEFAULT();
    soc_reset_reason_t rst_reas;
    rst_reas = esp_rom_get_reset_reason(0);
    if (rst_reas == RESET_REASON_CHIP_POWER_ON) {
        cfg.cali_ocode = 1;
    }
    rtc_init(cfg);
#endif
}

__attribute__((weak)) void esp_clk_init(void)
{
#if !CONFIG_IDF_ENV_FPGA
#ifndef CONFIG_XTAL_FREQ_AUTO
    assert(rtc_clk_xtal_freq_get() == CONFIG_XTAL_FREQ);
#endif

    bool rc_fast_d256_is_enabled = rtc_clk_8md256_enabled();
    rtc_clk_8m_enable(true, rc_fast_d256_is_enabled);
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_RC_FAST);
#endif

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    // WDT uses a SLOW_CLK clock source. After a function select_rtc_slow_clk a frequency of this source can changed.
    // If the frequency changes from 150kHz to 32kHz, then the timeout set for the WDT will increase 4.6 times.
    // Therefore, for the time of frequency change, set a new lower timeout value (1.6 sec on 40MHz XTAL and 2.5 sec on 26MHz XTAL).
    // This prevents excessive delay before resetting in case the supply voltage is drawdown.
    // (If frequency is changed from 150kHz to 32kHz then WDT timeout will increased to 1.6sec * 150/32 = 7.5 sec 40MHz XTAL,
    //  or 11.72 sec on 26MHz XTAL).
    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &RTCCNTL};
#ifdef CONFIG_XTAL_FREQ_26
    uint32_t stage_timeout_ticks = (uint32_t)(2500ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
#else
    uint32_t stage_timeout_ticks = (uint32_t)(1600ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
#endif
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_feed(&rtc_wdt_ctx);
    //Bootloader has enabled RTC WDT until now. We're only modifying timeout, so keep the stage and timeout action the same
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif

#if defined(CONFIG_RTC_CLK_SRC_EXT_OSC)
    select_rtc_slow_clk(SLOW_CLK_32K_EXT_OSC);
#elif defined(CONFIG_RTC_CLK_SRC_INT_8MD256)
    select_rtc_slow_clk(SLOW_CLK_8MD256);
#else
    select_rtc_slow_clk(SLOW_CLK_RTC);
#endif

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    // After changing a frequency WDT timeout needs to be set for new frequency.
    stage_timeout_ticks = (uint32_t)((uint64_t)CONFIG_BOOTLOADER_WDT_TIME_MS * rtc_clk_slow_freq_get_hz() / 1000);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_feed(&rtc_wdt_ctx);
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif

    rtc_cpu_freq_config_t old_config, new_config;
    rtc_clk_cpu_freq_get_config(&old_config);
    const uint32_t old_freq_mhz = old_config.freq_mhz;
    const uint32_t new_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ;

    bool res = rtc_clk_cpu_freq_mhz_to_config(new_freq_mhz, &new_config);
    assert(res);

    // Wait for UART TX to finish, otherwise some UART output will be lost
    // when switching APB frequency
    if (CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM != -1) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
    }

    if (res)  {
        rtc_clk_cpu_freq_set_config(&new_config);
    }

    // Re calculate the ccount to make time calculation correct.
    esp_cpu_set_cycle_count((uint64_t)esp_cpu_get_cycle_count() * new_freq_mhz / old_freq_mhz);
}

static void select_rtc_slow_clk(slow_clk_sel_t slow_clk)
{
    soc_rtc_slow_clk_src_t rtc_slow_clk_src = slow_clk & RTC_CNTL_ANA_CLK_RTC_SEL_V;
    uint32_t cal_val = 0;
    /* number of times to repeat external clock calibration
     * before giving up and switching to the internal RC
     */
    int retry_ext_clk = 3;

    do {
        if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
            /* external clock needs to be connected to PIN0 before it can
             * be used. Here we use rtc_clk_cal function to count
             * the number of ext clk cycles in the given number of ext clk
             * cycles. If the ext clk has not started up, calibration
             * will time out, returning 0.
             */
            ESP_EARLY_LOGD(TAG, "waiting for external clock by pin0 to start up");
            rtc_clk_32k_enable_external();

            // When SLOW_CLK_CAL_CYCLES is set to 0, clock calibration will not be performed at startup.
            if (SLOW_CLK_CAL_CYCLES > 0) {
                cal_val = rtc_clk_cal(RTC_CAL_32K_OSC_SLOW, SLOW_CLK_CAL_CYCLES);
                if (cal_val == 0) {
                    if (retry_ext_clk-- > 0) {
                        continue;
                    }
                    ESP_EARLY_LOGW(TAG, "external clock connected to pin0 not found, switching to internal 150 kHz oscillator");
                    rtc_slow_clk_src = SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
                }
            }
        } else if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_FAST_D256) {
            rtc_clk_8m_enable(true, true);
        }
        rtc_clk_slow_src_set(rtc_slow_clk_src);

        if (SLOW_CLK_CAL_CYCLES > 0) {
            /* TODO: 32k XTAL oscillator has some frequency drift at startup.
             * Improve calibration routine to wait until the frequency is stable.
             */
            cal_val = rtc_clk_cal(RTC_CAL_RTC_MUX, SLOW_CLK_CAL_CYCLES);
        } else {
            const uint64_t cal_dividend = (1ULL << RTC_CLK_CAL_FRACT) * 1000000ULL;
            cal_val = (uint32_t)(cal_dividend / rtc_clk_slow_freq_get_hz());
        }
    } while (cal_val == 0);
    ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %" PRIu32, cal_val);
    esp_clk_slowclk_cal_set(cal_val);
}

/* This function is not exposed as an API at this point.
 * All peripheral clocks are default enabled after chip is powered on.
 * This function disables some peripheral clocks when cpu starts.
 * These peripheral clocks are enabled when the peripherals are initialized
 * and disabled when they are de-initialized.
 */
__attribute__((weak)) void esp_perip_clk_init(void)
{
    uint32_t common_perip_clk, hwcrypto_perip_clk, wifi_bt_sdio_clk = 0;
    uint32_t common_perip_clk1 = 0;

    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);

    /* For reason that only reset CPU, do not disable the clocks
     * that have been enabled before reset.
     */
    if (rst_reason == RESET_REASON_CPU0_MWDT0 || rst_reason == RESET_REASON_CPU0_SW ||
            rst_reason == RESET_REASON_CPU0_RTC_WDT || rst_reason == RESET_REASON_CPU0_JTAG) {
        common_perip_clk = ~READ_PERI_REG(SYSTEM_PERIP_CLK_EN0_REG);
        hwcrypto_perip_clk = ~READ_PERI_REG(SYSTEM_PERIP_CLK_EN1_REG);
        wifi_bt_sdio_clk = ~READ_PERI_REG(SYSTEM_WIFI_CLK_EN_REG);
    } else {
        common_perip_clk = SYSTEM_SPI2_CLK_EN |
#if CONFIG_ESP_CONSOLE_UART_NUM != 0
                           SYSTEM_UART_CLK_EN |
#endif
#if CONFIG_ESP_CONSOLE_UART_NUM != 1
                           SYSTEM_UART1_CLK_EN |
#endif
                           SYSTEM_LEDC_CLK_EN |
                           SYSTEM_I2C_EXT0_CLK_EN |
                           SYSTEM_LEDC_CLK_EN;

        common_perip_clk1 = 0;
        hwcrypto_perip_clk = SYSTEM_CRYPTO_SHA_CLK_EN;
        wifi_bt_sdio_clk = SYSTEM_WIFI_CLK_WIFI_EN |
                           SYSTEM_WIFI_CLK_BT_EN_M |
                           SYSTEM_WIFI_CLK_UNUSED_BIT5 |
                           SYSTEM_WIFI_CLK_UNUSED_BIT12;
    }

    //Reset the communication peripherals like I2C, SPI, UART and bring them to known state.
    common_perip_clk |= SYSTEM_SPI2_CLK_EN |
#if CONFIG_ESP_CONSOLE_UART_NUM != 0
                        SYSTEM_UART_CLK_EN |
#endif
#if CONFIG_ESP_CONSOLE_UART_NUM != 1
                        SYSTEM_UART1_CLK_EN |
#endif
                        SYSTEM_I2C_EXT0_CLK_EN;
    common_perip_clk1 = 0;

#if !CONFIG_ESP_SYSTEM_HW_PC_RECORD
    /* Disable ASSIST Debug module clock if PC recoreding function is not used,
     * if stack guard function needs it, it will be re-enabled at esp_hw_stack_guard_init */
    CLEAR_PERI_REG_MASK(SYSTEM_CPU_PERI_CLK_EN_REG, SYSTEM_CLK_EN_ASSIST_DEBUG);
    SET_PERI_REG_MASK(SYSTEM_CPU_PERI_RST_EN_REG, SYSTEM_RST_EN_ASSIST_DEBUG);
#endif

    /* Disable some peripheral clocks. */
    CLEAR_PERI_REG_MASK(SYSTEM_PERIP_CLK_EN0_REG, common_perip_clk);
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN0_REG, common_perip_clk);

    CLEAR_PERI_REG_MASK(SYSTEM_PERIP_CLK_EN1_REG, common_perip_clk1);
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN1_REG, common_perip_clk1);

    /* Disable hardware crypto clocks. */
    CLEAR_PERI_REG_MASK(SYSTEM_PERIP_CLK_EN1_REG, hwcrypto_perip_clk);
    SET_PERI_REG_MASK(SYSTEM_PERIP_RST_EN1_REG, hwcrypto_perip_clk);

    /* Disable WiFi/BT/SDIO clocks. */
    CLEAR_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, wifi_bt_sdio_clk);
    SET_PERI_REG_MASK(SYSTEM_WIFI_CLK_EN_REG, SYSTEM_WIFI_CLK_EN);

    /* Set WiFi light sleep clock source to RTC slow clock */
    REG_SET_FIELD(SYSTEM_BT_LPCK_DIV_INT_REG, SYSTEM_BT_LPCK_DIV_NUM, 0);
    CLEAR_PERI_REG_MASK(SYSTEM_BT_LPCK_DIV_FRAC_REG, SYSTEM_LPCLK_SEL_XTAL32K | SYSTEM_LPCLK_SEL_XTAL | SYSTEM_LPCLK_SEL_8M | SYSTEM_LPCLK_SEL_RTC_SLOW);
    SET_PERI_REG_MASK(SYSTEM_BT_LPCK_DIV_FRAC_REG, SYSTEM_LPCLK_SEL_RTC_SLOW);

    /* Enable RNG clock. */
    periph_module_enable(PERIPH_RNG_MODULE);
}

// Workaround for bootloader not calibrated well issue.
// Placed in IRAM because disabling BBPLL may influence the cache
static void IRAM_ATTR NOINLINE_ATTR recalib_bbpll(void)
{
    rtc_cpu_freq_config_t old_config;
    rtc_clk_cpu_freq_get_config(&old_config);

    // There are two paths we arrive here: 1. CPU reset. 2. Other reset reasons.
    // - For other reasons, the bootloader will set CPU source to BBPLL and enable it. But there are calibration issues.
    //   Turn off the BBPLL and do calibration again to fix the issue.
    // - For CPU reset, the CPU source will be set to XTAL, while the BBPLL is kept to meet USB Serial JTAG's
    //   requirements. In this case, we don't touch BBPLL to avoid USJ disconnection.
    if (old_config.source == SOC_CPU_CLK_SRC_PLL) {
        rtc_clk_cpu_freq_set_xtal();
        rtc_clk_cpu_freq_set_config(&old_config);
    }
}
