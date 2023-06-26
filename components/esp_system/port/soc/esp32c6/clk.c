/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "esp_clk_internal.h"
#include "esp32c6/rom/ets_sys.h"
#include "esp32c6/rom/uart.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/i2s_reg.h"
#include "esp_cpu.h"
#include "hal/wdt_hal.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_pmu.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "ocode_init.h"

/* Number of cycles to wait from the 32k XTAL oscillator to consider it running.
 * Larger values increase startup delay. Smaller values may cause false positive
 * detection (i.e. oscillator runs for a few cycles and then stops).
 */
#define SLOW_CLK_CAL_CYCLES     CONFIG_RTC_CLK_CAL_CYCLES

#define MHZ (1000000)

static void select_rtc_slow_clk(soc_rtc_slow_clk_src_t rtc_slow_clk_src);

static const char *TAG = "clk";


 __attribute__((weak)) void esp_clk_init(void)
{
#if !CONFIG_IDF_ENV_FPGA
    pmu_init();
    if (esp_rom_get_reset_reason(0) == RESET_REASON_CHIP_POWER_ON) {
        esp_ocode_calib_init();
    }

    assert(rtc_clk_xtal_freq_get() == RTC_XTAL_FREQ_40M);

    rtc_clk_8m_enable(true);
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_RC_FAST);
#endif

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    // WDT uses a SLOW_CLK clock source. After a function select_rtc_slow_clk a frequency of this source can changed.
    // If the frequency changes from 150kHz to 32kHz, then the timeout set for the WDT will increase 4.6 times.
    // Therefore, for the time of frequency change, set a new lower timeout value (1.6 sec).
    // This prevents excessive delay before resetting in case the supply voltage is drawdown.
    // (If frequency is changed from 150kHz to 32kHz then WDT timeout will increased to 1.6sec * 150/32 = 7.5 sec).
    wdt_hal_context_t rtc_wdt_ctx = RWDT_HAL_CONTEXT_DEFAULT();
    uint32_t stage_timeout_ticks = (uint32_t)(1600ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
    wdt_hal_write_protect_disable(&rtc_wdt_ctx);
    wdt_hal_feed(&rtc_wdt_ctx);
    //Bootloader has enabled RTC WDT until now. We're only modifying timeout, so keep the stage and timeout action the same
    wdt_hal_config_stage(&rtc_wdt_ctx, WDT_STAGE0, stage_timeout_ticks, WDT_STAGE_ACTION_RESET_RTC);
    wdt_hal_write_protect_enable(&rtc_wdt_ctx);
#endif

#if defined(CONFIG_RTC_CLK_SRC_EXT_CRYS)
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_XTAL32K);
#elif defined(CONFIG_RTC_CLK_SRC_EXT_OSC)
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_OSC_SLOW);
#elif defined(CONFIG_RTC_CLK_SRC_INT_RC32K)
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_RC32K);
#else
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_RC_SLOW);
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
    esp_rom_uart_tx_wait_idle(CONFIG_ESP_CONSOLE_UART_NUM);

    if (res)  {
        rtc_clk_cpu_freq_set_config(&new_config);
    }

    // Re calculate the ccount to make time calculation correct.
    esp_cpu_set_cycle_count( (uint64_t)esp_cpu_get_cycle_count() * new_freq_mhz / old_freq_mhz );
}

static void select_rtc_slow_clk(soc_rtc_slow_clk_src_t rtc_slow_clk_src)
{
    uint32_t cal_val = 0;
    /* number of times to repeat 32k XTAL calibration
     * before giving up and switching to the internal RC
     */
    int retry_32k_xtal = 3;

    do {
        if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K || rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
            /* 32k XTAL oscillator needs to be enabled and running before it can
             * be used. Hardware doesn't have a direct way of checking if the
             * oscillator is running. Here we use rtc_clk_cal function to count
             * the number of main XTAL cycles in the given number of 32k XTAL
             * oscillator cycles. If the 32k XTAL has not started up, calibration
             * will time out, returning 0.
             */
            ESP_EARLY_LOGD(TAG, "waiting for 32k oscillator to start up");
            rtc_cal_sel_t cal_sel = 0;
            if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
                rtc_clk_32k_enable(true);
                cal_sel = RTC_CAL_32K_XTAL;
            } else if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
                rtc_clk_32k_enable_external();
                cal_sel = RTC_CAL_32K_OSC_SLOW;
            }
            // When SLOW_CLK_CAL_CYCLES is set to 0, clock calibration will not be performed at startup.
            if (SLOW_CLK_CAL_CYCLES > 0) {
                cal_val = rtc_clk_cal(cal_sel, SLOW_CLK_CAL_CYCLES);
                if (cal_val == 0) {
                    if (retry_32k_xtal-- > 0) {
                        continue;
                    }
                    ESP_EARLY_LOGW(TAG, "32 kHz clock not found, switching to internal 150 kHz oscillator");
                    rtc_slow_clk_src = SOC_RTC_SLOW_CLK_SRC_RC_SLOW;
                }
            }
        } else if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) {
            rtc_clk_rc32k_enable(true);
        }
        rtc_clk_slow_src_set(rtc_slow_clk_src);

        if (SLOW_CLK_CAL_CYCLES > 0) {
            /* TODO: 32k XTAL oscillator has some frequency drift at startup.
             * Improve calibration routine to wait until the frequency is stable.
             */
            cal_val = rtc_clk_cal(RTC_CAL_RTC_MUX, SLOW_CLK_CAL_CYCLES);
        } else {
            const uint64_t cal_dividend = (1ULL << RTC_CLK_CAL_FRACT) * 1000000ULL;
            cal_val = (uint32_t) (cal_dividend / rtc_clk_slow_freq_get_hz());
        }
    } while (cal_val == 0);
    ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %d", cal_val);
    esp_clk_slowclk_cal_set(cal_val);
}

void rtc_clk_select_rtc_slow_clk(void)
{
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_XTAL32K);
}

/* This function is not exposed as an API at this point.
 * All peripheral clocks are default enabled after chip is powered on.
 * This function disables some peripheral clocks when cpu starts.
 * These peripheral clocks are enabled when the peripherals are initialized
 * and disabled when they are de-initialized.
 */
__attribute__((weak)) void esp_perip_clk_init(void)
{
    modem_clock_domain_pmu_state_icg_map_init();

    /* During system initialization, the low-power clock source of the modem
     * (WiFi, BLE or Coexist) follows the configuration of the slow clock source
     * of the system. If the WiFi, BLE or Coexist module needs a higher
     * precision sleep clock (for example, the BLE needs to use the main XTAL
     * oscillator (40 MHz) to provide the clock during the sleep process in some
     * scenarios), the module needs to switch to the required clock source by
     * itself. */ //TODO - WIFI-5233
    soc_rtc_slow_clk_src_t rtc_slow_clk_src = rtc_clk_slow_src_get();
    modem_clock_lpclk_src_t modem_lpclk_src = (modem_clock_lpclk_src_t) ( \
                  (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_SLOW)  ? MODEM_CLOCK_LPCLK_SRC_RC_SLOW \
                : (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K)  ? MODEM_CLOCK_LPCLK_SRC_XTAL32K  \
                : (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K)    ? MODEM_CLOCK_LPCLK_SRC_RC32K    \
                : (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) ? MODEM_CLOCK_LPCLK_SRC_EXT32K   \
                : SOC_RTC_SLOW_CLK_SRC_RC_SLOW);
    modem_clock_select_lp_clock_source(PERIPH_WIFI_MODULE, modem_lpclk_src, 0);

    ESP_EARLY_LOGW(TAG, "esp_perip_clk_init() has not been implemented yet");
#if 0 // TODO: IDF-5658
    uint32_t common_perip_clk, hwcrypto_perip_clk, wifi_bt_sdio_clk = 0;
    uint32_t common_perip_clk1 = 0;

    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);

    /* For reason that only reset CPU, do not disable the clocks
     * that have been enabled before reset.
     */
    if (rst_reason == RESET_REASON_CPU0_MWDT0 || rst_reason == RESET_REASON_CPU0_SW ||
            rst_reason == RESET_REASON_CPU0_RTC_WDT || rst_reason == RESET_REASON_CPU0_MWDT1) {
        common_perip_clk = ~READ_PERI_REG(SYSTEM_PERIP_CLK_EN0_REG);
        hwcrypto_perip_clk = ~READ_PERI_REG(SYSTEM_PERIP_CLK_EN1_REG);
        wifi_bt_sdio_clk = ~READ_PERI_REG(SYSTEM_WIFI_CLK_EN_REG);
    } else {
        common_perip_clk = SYSTEM_WDG_CLK_EN |
                           SYSTEM_I2S0_CLK_EN |
#if CONFIG_ESP_CONSOLE_UART_NUM != 0
                           SYSTEM_UART_CLK_EN |
#endif
#if CONFIG_ESP_CONSOLE_UART_NUM != 1
                           SYSTEM_UART1_CLK_EN |
#endif
                           SYSTEM_SPI2_CLK_EN |
                           SYSTEM_I2C_EXT0_CLK_EN |
                           SYSTEM_UHCI0_CLK_EN |
                           SYSTEM_RMT_CLK_EN |
                           SYSTEM_LEDC_CLK_EN |
                           SYSTEM_TIMERGROUP1_CLK_EN |
                           SYSTEM_SPI3_CLK_EN |
                           SYSTEM_SPI4_CLK_EN |
                           SYSTEM_TWAI_CLK_EN |
                           SYSTEM_I2S1_CLK_EN |
                           SYSTEM_SPI2_DMA_CLK_EN |
                           SYSTEM_SPI3_DMA_CLK_EN;

        common_perip_clk1 = 0;
        hwcrypto_perip_clk = SYSTEM_CRYPTO_AES_CLK_EN |
                             SYSTEM_CRYPTO_SHA_CLK_EN |
                             SYSTEM_CRYPTO_RSA_CLK_EN;
        wifi_bt_sdio_clk = SYSTEM_WIFI_CLK_WIFI_EN |
                           SYSTEM_WIFI_CLK_BT_EN_M |
                           SYSTEM_WIFI_CLK_UNUSED_BIT5 |
                           SYSTEM_WIFI_CLK_UNUSED_BIT12;
    }

    //Reset the communication peripherals like I2C, SPI, UART, I2S and bring them to known state.
    common_perip_clk |= SYSTEM_I2S0_CLK_EN |
#if CONFIG_ESP_CONSOLE_UART_NUM != 0
                        SYSTEM_UART_CLK_EN |
#endif
#if CONFIG_ESP_CONSOLE_UART_NUM != 1
                        SYSTEM_UART1_CLK_EN |
#endif
                        SYSTEM_SPI2_CLK_EN |
                        SYSTEM_I2C_EXT0_CLK_EN |
                        SYSTEM_UHCI0_CLK_EN |
                        SYSTEM_RMT_CLK_EN |
                        SYSTEM_UHCI1_CLK_EN |
                        SYSTEM_SPI3_CLK_EN |
                        SYSTEM_SPI4_CLK_EN |
                        SYSTEM_I2C_EXT1_CLK_EN |
                        SYSTEM_I2S1_CLK_EN |
                        SYSTEM_SPI2_DMA_CLK_EN |
                        SYSTEM_SPI3_DMA_CLK_EN;
    common_perip_clk1 = 0;

    /* Change I2S clock to audio PLL first. Because if I2S uses 160MHz clock,
     * the current is not reduced when disable I2S clock.
     */
    // TOCK(check replacement)
    // REG_SET_FIELD(I2S_CLKM_CONF_REG(0), I2S_CLK_SEL, I2S_CLK_AUDIO_PLL);
    // REG_SET_FIELD(I2S_CLKM_CONF_REG(1), I2S_CLK_SEL, I2S_CLK_AUDIO_PLL);

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
    CLEAR_PERI_REG_MASK(SYSTEM_BT_LPCK_DIV_FRAC_REG, SYSTEM_LPCLK_SEL_8M);
    SET_PERI_REG_MASK(SYSTEM_BT_LPCK_DIV_FRAC_REG, SYSTEM_LPCLK_SEL_RTC_SLOW);

    /* Enable RNG clock. */
    periph_module_enable(PERIPH_RNG_MODULE);
#endif

    /* Enable TimerGroup 0 clock to ensure its reference counter will never
     * be decremented to 0 during normal operation and preventing it from
     * being disabled.
     * If the TimerGroup 0 clock is disabled and then reenabled, the watchdog
     * registers (Flashboot protection included) will be reenabled, and some
     * seconds later, will trigger an unintended reset.
     */
    periph_module_enable(PERIPH_TIMG0_MODULE);
}
