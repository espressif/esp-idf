/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
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
#include "esp32c5/rom/ets_sys.h"
#include "esp32c5/rom/uart.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/i2s_reg.h"
#include "soc/chip_revision.h"
#include "esp_cpu.h"
#include "hal/efuse_hal.h"
#include "hal/wdt_hal.h"
#include "hal/clk_tree_ll.h"
#if SOC_MODEM_CLOCK_SUPPORTED
#include "hal/modem_lpcon_ll.h"
#endif
#include "hal/apm_ll.h"
#include "hal/clk_gate_ll.h"
#include "hal/clk_tree_ll.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_modem_clock.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_pmu.h"
#include "esp_rom_serial_output.h"
#include "esp_rom_sys.h"

/* Number of cycles to wait from the 32k XTAL oscillator to consider it running.
 * Larger values increase startup delay. Smaller values may cause false positive
 * detection (i.e. oscillator runs for a few cycles and then stops).
 */
#define SLOW_CLK_CAL_CYCLES     CONFIG_RTC_CLK_CAL_CYCLES

#define MHZ (1000000)

static void select_rtc_slow_clk(soc_rtc_slow_clk_src_t rtc_slow_clk_src);

ESP_LOG_ATTR_TAG(TAG, "clk");

void esp_rtc_init(void)
{
#if !CONFIG_IDF_ENV_FPGA
    pmu_init();
#endif
}

__attribute__((weak)) void esp_clk_init(void)
{
#if !CONFIG_IDF_ENV_FPGA
    assert((rtc_clk_xtal_freq_get() == SOC_XTAL_FREQ_48M) || (rtc_clk_xtal_freq_get() == SOC_XTAL_FREQ_40M));

    rtc_clk_8m_enable(true);
#if CONFIG_RTC_FAST_CLK_SRC_RC_FAST
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_RC_FAST);
#elif CONFIG_RTC_FAST_CLK_SRC_XTAL
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_XTAL);
    if (esp_sleep_sub_mode_dump_config(NULL)[ESP_SLEEP_RTC_FAST_USE_XTAL_MODE] == 0) {
        esp_sleep_sub_mode_config(ESP_SLEEP_RTC_FAST_USE_XTAL_MODE, true);
    }
#else
#error "No RTC fast clock source configured"
#endif
#endif //!CONFIG_IDF_ENV_FPGA

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

    modem_clock_deselect_all_module_lp_clock_source();
#if defined(CONFIG_RTC_CLK_SRC_EXT_CRYS)
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_XTAL32K);
#elif defined(CONFIG_RTC_CLK_SRC_EXT_OSC)
    select_rtc_slow_clk(SOC_RTC_SLOW_CLK_SRC_OSC_SLOW);
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
}

static void select_rtc_slow_clk(soc_rtc_slow_clk_src_t rtc_slow_clk_src)
{
    uint32_t cal_val = 0;
    /* number of times to repeat 32k XTAL calibration
     * before giving up and switching to the internal RC
     */
    int retry_32k_xtal = 3;

    soc_rtc_slow_clk_src_t old_rtc_slow_clk_src = rtc_clk_slow_src_get();
    do {
        bool revoke_32k_enable = false;
        if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K || rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
            /* 32k XTAL oscillator needs to be enabled and running before it can
             * be used. Hardware doesn't have a direct way of checking if the
             * oscillator is running. Here we use rtc_clk_cal function to count
             * the number of main XTAL cycles in the given number of 32k XTAL
             * oscillator cycles. If the 32k XTAL has not started up, calibration
             * will time out, returning 0.
             */
            ESP_EARLY_LOGD(TAG, "waiting for 32k oscillator to start up");
            soc_clk_freq_calculation_src_t cal_sel = -1;
            if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
                rtc_clk_32k_enable(true);
                cal_sel = CLK_CAL_32K_XTAL;
            } else if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) {
                rtc_clk_32k_enable_external();
                cal_sel = CLK_CAL_32K_OSC_SLOW;
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
                    revoke_32k_enable = true;
                }
            }
        }
        rtc_clk_slow_src_set(rtc_slow_clk_src);
        // Disable unused clock sources after clock source switching is complete.
        // Regardless of the clock source selection, the internal 136K clock source will always keep on.
        if (revoke_32k_enable || \
                (((old_rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) || (old_rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW)) && \
                 ((rtc_slow_clk_src != SOC_RTC_SLOW_CLK_SRC_XTAL32K) && (rtc_slow_clk_src != SOC_RTC_SLOW_CLK_SRC_OSC_SLOW)))) {
            rtc_clk_32k_enable(false);
            rtc_clk_32k_disable_external();
        }
        // We have enabled all LP clock power in pmu_init, re-initialize the LP clock power based on the slow clock source after selection.
        pmu_lp_power_t lp_clk_power = {
            .xpd_xtal32k = (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) || (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW),
            .xpd_rc32k = 0,
            .xpd_fosc = 1,
            .pd_osc = 0
        };
        pmu_ll_lp_set_clk_power(&PMU, PMU_MODE_LP_ACTIVE, lp_clk_power.val);

        if (SLOW_CLK_CAL_CYCLES > 0) {
            /* TODO: 32k XTAL oscillator has some frequency drift at startup.
             * Improve calibration routine to wait until the frequency is stable.
             */
            cal_val = rtc_clk_cal(CLK_CAL_RTC_SLOW, SLOW_CLK_CAL_CYCLES);
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
    /* During system initialization, the low-power clock source of the modem
     * (WiFi, BLE or Coexist) follows the configuration of the slow clock source
     * of the system. If the WiFi, BLE or Coexist module needs a higher
     * precision sleep clock (for example, the BLE needs to use the main XTAL
     * oscillator (40 MHz) to provide the clock during the sleep process in some
     * scenarios), the module needs to switch to the required clock source by
     * itself. */
#if CONFIG_ESP_WIFI_ENABLED
    soc_rtc_slow_clk_src_t rtc_slow_clk_src = rtc_clk_slow_src_get();
    modem_clock_lpclk_src_t modem_lpclk_src = (modem_clock_lpclk_src_t)(
                                                  (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K)  ? MODEM_CLOCK_LPCLK_SRC_XTAL32K
                                                  : (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_OSC_SLOW) ? MODEM_CLOCK_LPCLK_SRC_EXT32K
                                                  : MODEM_CLOCK_LPCLK_SRC_RC_SLOW);
    modem_clock_select_lp_clock_source(PERIPH_WIFI_MODULE, modem_lpclk_src, 0);
#endif

    /* On ESP32-C5 ECO1, clearing BIT(31) of PCR_FPGA_DEBUG_REG is used to fix
     * the issue where the modem module fails to transmit and receive packets
     * due to the loss of the modem root clock caused by automatic clock gating
     * during soc root clock source switching. For detailed information, refer
     * to IDF-11064. */
    clk_ll_soc_root_clk_auto_gating_bypass(true);

    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    periph_ll_clk_gate_config_t clk_gate_config = {0};

#if CONFIG_ESP_CONSOLE_UART_NUM != 0
    clk_gate_config.disable_uart0_clk = true;
#endif
#if CONFIG_ESP_CONSOLE_UART_NUM != 1
    clk_gate_config.disable_uart1_clk = true;
#endif
#if CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
    clk_gate_config.disable_mspi_flash_clk = true;
#endif
#if !CONFIG_SECURE_ENABLE_TEE
    clk_gate_config.disable_crypto_periph_clk = true;
#endif
#if !CONFIG_USJ_ENABLE_USB_SERIAL_JTAG && !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
    clk_gate_config.disable_usb_serial_jtag = true;
#endif
#if !CONFIG_ESP_ENABLE_PVT
    clk_gate_config.disable_pvt_clk = true;
#endif

    periph_ll_clk_gate_set_default(rst_reason, &clk_gate_config);
}
