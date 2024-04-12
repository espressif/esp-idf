/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
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
#include "esp_clk_internal.h"
#include "esp32h2/rom/ets_sys.h"
#include "esp32h2/rom/uart.h"
#include "soc/soc.h"
#include "soc/pcr_reg.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/i2s_reg.h"
#include "soc/lpperi_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/pcr_reg.h"
#include "hal/wdt_hal.h"
#include "hal/uart_ll.h"
#include "hal/i2c_ll.h"
#include "hal/rmt_ll.h"
#include "hal/ledc_ll.h"
#include "hal/lp_clkrst_ll.h"
#include "hal/timer_ll.h"
#include "hal/twai_ll.h"
#include "hal/i2s_ll.h"
#include "hal/pcnt_ll.h"
#include "hal/etm_ll.h"
#include "hal/mcpwm_ll.h"
#include "hal/parlio_ll.h"
#include "hal/gdma_ll.h"
#include "hal/pau_ll.h"
#include "hal/spi_ll.h"
#include "hal/clk_gate_ll.h"
#include "hal/temperature_sensor_ll.h"
#include "hal/usb_serial_jtag_ll.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_pmu.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"
#include "esp_sleep.h"

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

    assert(rtc_clk_xtal_freq_get() == SOC_XTAL_FREQ_32M);

    rtc_clk_8m_enable(true);
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_RC_FAST);
#endif

#ifdef CONFIG_BOOTLOADER_WDT_ENABLE
    // WDT uses a SLOW_CLK clock source. After a function select_rtc_slow_clk a frequency of this source can changed.
    // If the frequency changes from 150kHz to 32kHz, then the timeout set for the WDT will increase 4.6 times.
    // Therefore, for the time of frequency change, set a new lower timeout value (2 sec).
    // This prevents excessive delay before resetting in case the supply voltage is drawdown.
    // (If frequency is changed from 150kHz to 32kHz then WDT timeout will increased to 2 sec * 150/32 = 9.375 sec).

    wdt_hal_context_t rtc_wdt_ctx = {.inst = WDT_RWDT, .rwdt_dev = &LP_WDT};

    uint32_t stage_timeout_ticks = (uint32_t)(2000ULL * rtc_clk_slow_freq_get_hz() / 1000ULL);
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
    if (CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM != -1) {
        esp_rom_output_tx_wait_idle(CONFIG_ESP_CONSOLE_ROM_SERIAL_PORT_NUM);
    }

    if (res)  {
        rtc_clk_cpu_freq_set_config(&new_config);
    }

    // Re calculate the ccount to make time calculation correct.
    esp_cpu_set_cycle_count((uint64_t)esp_cpu_get_cycle_count() * new_freq_mhz / old_freq_mhz);

    // Set crypto clock (`clk_sec`) to use 96M PLL clock
    REG_SET_FIELD(PCR_SEC_CONF_REG, PCR_SEC_CLK_SEL, 0x3);
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
            cal_val = (uint32_t)(cal_dividend / rtc_clk_slow_freq_get_hz());
        }
    } while (cal_val == 0);
    ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %" PRIu32, cal_val);
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
    soc_rtc_slow_clk_src_t rtc_slow_clk_src = rtc_clk_slow_src_get();
    esp_sleep_pd_domain_t pu_domain = (esp_sleep_pd_domain_t)(\
                                                              (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) ? ESP_PD_DOMAIN_XTAL32K \
                                                              : (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) ? ESP_PD_DOMAIN_RC32K \
                                                              : ESP_PD_DOMAIN_MAX);
    esp_sleep_pd_config(pu_domain, ESP_PD_OPTION_ON);

    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    if ((rst_reason != RESET_REASON_CPU0_MWDT0) && (rst_reason != RESET_REASON_CPU0_MWDT1)      \
            && (rst_reason != RESET_REASON_CPU0_SW) && (rst_reason != RESET_REASON_CPU0_RTC_WDT)    \
            && (rst_reason != RESET_REASON_CPU0_JTAG)) {
#if CONFIG_ESP_CONSOLE_UART_NUM != 0
        uart_ll_enable_bus_clock(UART_NUM_0, false);
#elif CONFIG_ESP_CONSOLE_UART_NUM != 1
        uart_ll_enable_bus_clock(UART_NUM_1, false);
#endif
        i2c_ll_enable_bus_clock(0, false);
        i2c_ll_enable_bus_clock(1, false);
        i2c_ll_enable_controller_clock(&I2C0, false);
        i2c_ll_enable_controller_clock(&I2C1, false);
        rmt_ll_enable_bus_clock(0, false);
        rmt_ll_enable_group_clock(0, false);
        ledc_ll_enable_clock(&LEDC, false);
        ledc_ll_enable_bus_clock(false);
        timer_ll_enable_clock(&TIMERG0, 0, false);
        timer_ll_enable_clock(&TIMERG1, 0, false);
        _timer_ll_enable_bus_clock(0, false);
        _timer_ll_enable_bus_clock(1, false);
        twai_ll_enable_clock(0, false);
        twai_ll_enable_bus_clock(0, false);
        i2s_ll_enable_bus_clock(0, false);
        i2s_ll_tx_disable_clock(&I2S0);
        i2s_ll_rx_disable_clock(&I2S0);
        pcnt_ll_enable_bus_clock(0, false);
        etm_ll_enable_bus_clock(0, false);
        mcpwm_ll_enable_bus_clock(0, false);
        mcpwm_ll_group_enable_clock(0, false);
        parlio_ll_rx_enable_clock(&PARL_IO, false);
        parlio_ll_tx_enable_clock(&PARL_IO, false);
        parlio_ll_enable_bus_clock(0, false);
        gdma_ll_force_enable_reg_clock(&GDMA, false);
        gdma_ll_enable_bus_clock(0, false);
#if CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
        spi_ll_enable_bus_clock(SPI1_HOST, false);
#endif
        spi_ll_enable_bus_clock(SPI2_HOST, false);
        temperature_sensor_ll_bus_clk_enable(false);
        pau_ll_enable_bus_clock(false);

        periph_ll_disable_clk_set_rst(PERIPH_UHCI0_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_SARADC_MODULE);
#if !CONFIG_ESP_SYSTEM_HW_PC_RECORD
        /* Disable ASSIST Debug module clock if PC recoreding function is not used,
         * if stack guard function needs it, it will be re-enabled at esp_hw_stack_guard_init */
        periph_ll_disable_clk_set_rst(PERIPH_ASSIST_DEBUG_MODULE);
#endif
        periph_ll_disable_clk_set_rst(PERIPH_RSA_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_AES_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_SHA_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_ECC_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_HMAC_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_DS_MODULE);
        periph_ll_disable_clk_set_rst(PERIPH_ECDSA_MODULE);

        // TODO: Replace with hal implementation
        REG_CLR_BIT(PCR_CTRL_TICK_CONF_REG, PCR_TICK_ENABLE);
        REG_CLR_BIT(PCR_TRACE_CONF_REG, PCR_TRACE_CLK_EN);
        REG_CLR_BIT(PCR_MEM_MONITOR_CONF_REG, PCR_MEM_MONITOR_CLK_EN);
        REG_CLR_BIT(PCR_PVT_MONITOR_CONF_REG, PCR_PVT_MONITOR_CLK_EN);
        REG_CLR_BIT(PCR_PVT_MONITOR_FUNC_CLK_CONF_REG, PCR_PVT_MONITOR_FUNC_CLK_EN);
        WRITE_PERI_REG(PCR_CTRL_CLK_OUT_EN_REG, 0);

#if !CONFIG_USJ_ENABLE_USB_SERIAL_JTAG && !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
        // Disable USB-Serial-JTAG clock and it's pad if not used
        usb_serial_jtag_ll_phy_enable_pad(false);
        usb_serial_jtag_ll_enable_bus_clock(false);
#endif
    }

    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_CHIP_BROWN_OUT) \
            || (rst_reason == RESET_REASON_SYS_RTC_WDT) || (rst_reason == RESET_REASON_SYS_SUPER_WDT)) {
        _lp_clkrst_ll_enable_rng_clock(false);
        CLEAR_PERI_REG_MASK(LPPERI_CLK_EN_REG, LPPERI_OTP_DBG_CK_EN);
        CLEAR_PERI_REG_MASK(LPPERI_CLK_EN_REG, LPPERI_LP_ANA_I2C_CK_EN);
        CLEAR_PERI_REG_MASK(LPPERI_CLK_EN_REG, LPPERI_LP_IO_CK_EN);
        WRITE_PERI_REG(LP_CLKRST_LP_CLK_PO_EN_REG, 0);
    }
}
