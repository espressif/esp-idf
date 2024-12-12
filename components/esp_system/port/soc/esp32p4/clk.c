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
#include "esp_sleep.h"
#include "esp_clk_internal.h"
#include "esp32p4/rom/ets_sys.h"
#include "esp32p4/rom/uart.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_periph.h"
#include "soc/i2s_reg.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "esp_cpu.h"
#include "mspi_timing_tuning_configs.h"

#include "soc/hp_sys_clkrst_reg.h"
#include "soc/lp_clkrst_reg.h"
#include "soc/lp_system_reg.h"
#include "soc/sdmmc_reg.h"
#include "soc/spi_mem_c_reg.h"
#include "soc/spi_mem_s_reg.h"
#include "soc/usb_serial_jtag_reg.h"
#include "soc/trace_struct.h"

#include "hal/aes_ll.h"
#include "hal/assist_debug_ll.h"
#include "hal/ds_ll.h"
#include "hal/ecc_ll.h"
#include "hal/etm_ll.h"
#include "hal/gdma_ll.h"
#include "hal/hmac_ll.h"
#include "hal/mipi_csi_ll.h"
#include "hal/mipi_dsi_brg_ll.h"
#include "hal/mpi_ll.h"
#include "hal/pau_ll.h"
#include "hal/parlio_ll.h"
#include "hal/psram_ctrlr_ll.h"
#include "hal/rtc_io_ll.h"
#include "hal/sha_ll.h"
#include "hal/spi_ll.h"
#include "hal/spimem_flash_ll.h"
#include "hal/timer_ll.h"
#include "hal/uart_ll.h"
#include "hal/usb_serial_jtag_ll.h"
#include "hal/usb_utmi_ll.h"
#include "hal/wdt_hal.h"

#include "esp_private/esp_modem_clock.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/periph_ctrl.h"
#include "esp_private/esp_clk.h"
#include "esp_private/esp_pmu.h"
#include "esp_rom_uart.h"
#include "esp_rom_sys.h"

/* Number of cycles to wait from the 32k XTAL oscillator to consider it running.
 * Larger values increase startup delay. Smaller values may cause false positive
 * detection (i.e. oscillator runs for a few cycles and then stops).
 */
#define SLOW_CLK_CAL_CYCLES     CONFIG_RTC_CLK_CAL_CYCLES

#define MHZ (1000000)

static void select_rtc_slow_clk(soc_rtc_slow_clk_src_t rtc_slow_clk_src);

static const char *TAG = "clk";

void IRAM_ATTR esp_rtc_init(void)
{
#if SOC_PMU_SUPPORTED
    pmu_init();
#endif  //SOC_PMU_SUPPORTED
}

__attribute__((weak)) void esp_clk_init(void)
{
    assert(rtc_clk_xtal_freq_get() == SOC_XTAL_FREQ_40M);

    rtc_clk_8m_enable(true);
#if CONFIG_RTC_FAST_CLK_SRC_RC_FAST
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_RC_FAST);
#elif CONFIG_RTC_FAST_CLK_SRC_XTAL
    rtc_clk_fast_src_set(SOC_RTC_FAST_CLK_SRC_XTAL);
    esp_sleep_sub_mode_config(ESP_SLEEP_RTC_FAST_USE_XTAL_MODE, true);
#else
#error "No RTC fast clock source configured"
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

    do {
        if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
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

        // Disable unused clock sources after clock source switching is complete.
        // Regardless of the clock source selection, the internal 136K clock source will always keep on.
        if (rtc_slow_clk_src != SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
            rtc_clk_32k_enable(false);
        }
        if (rtc_slow_clk_src != SOC_RTC_SLOW_CLK_SRC_RC32K) {
            rtc_clk_rc32k_enable(false);
        }

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

    if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC_SLOW) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL32K, ESP_PD_OPTION_AUTO);
        REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_XTAL_32K_CLK_EN);
        REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_RC_32K_CLK_EN);
        esp_sleep_pd_config(ESP_PD_DOMAIN_RC32K, ESP_PD_OPTION_AUTO);
    } else if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_XTAL32K) {
        // RC slow (150K) always ON
        esp_sleep_pd_config(ESP_PD_DOMAIN_RC32K, ESP_PD_OPTION_AUTO);
        REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_RC_32K_CLK_EN);
        esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL32K, ESP_PD_OPTION_ON);
    } else if (rtc_slow_clk_src == SOC_RTC_SLOW_CLK_SRC_RC32K) {
        // RC slow (150K) always ON
        esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL32K, ESP_PD_OPTION_AUTO);
        REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_XTAL_32K_CLK_EN);
        esp_sleep_pd_config(ESP_PD_DOMAIN_RC32K, ESP_PD_OPTION_ON);
    }

    soc_reset_reason_t rst_reason = esp_rom_get_reset_reason(0);
    // HP modules related clock control
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_CORE_PMU_PWR_DOWN)
            || (rst_reason == RESET_REASON_SYS_BROWN_OUT) || (rst_reason == RESET_REASON_SYS_RWDT) || (rst_reason == RESET_REASON_SYS_SUPER_WDT)
            || (rst_reason == RESET_REASON_CORE_SW) || (rst_reason == RESET_REASON_CORE_MWDT) || (rst_reason == RESET_REASON_CORE_RWDT) || (rst_reason == RESET_REASON_CORE_PWR_GLITCH) || (rst_reason == RESET_REASON_CORE_EFUSE_CRC) || (rst_reason == RESET_REASON_CORE_USB_JTAG) || (rst_reason == RESET_REASON_CORE_USB_UART)
       ) {
        // hp_sys_clkrst register gets reset only if chip reset or pmu powers down hp
        // but at core reset and above, we will also disable HP modules' clock gating to save power consumption
        _gdma_ll_enable_bus_clock(0, false);
        _gdma_ll_enable_bus_clock(1, false);
        _pau_ll_enable_bus_clock(false);
        _parlio_ll_enable_bus_clock(0, false);
        _etm_ll_enable_bus_clock(0, false);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_BITSRAMBLER_SYS_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_BITSRAMBLER_RX_SYS_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_BITSRAMBLER_TX_SYS_CLK_EN);

// Non-Console UART
#if CONFIG_ESP_CONSOLE_UART_NUM != 0
        _uart_ll_enable_bus_clock(UART_NUM_0, false);
        _uart_ll_sclk_disable(&UART0);
#elif CONFIG_ESP_CONSOLE_UART_NUM != 1
        _uart_ll_enable_bus_clock(UART_NUM_1, false);
        _uart_ll_sclk_disable(&UART1);
#endif
        _uart_ll_enable_bus_clock(UART_NUM_2, false);
        _uart_ll_sclk_disable(&UART2);
        _uart_ll_enable_bus_clock(UART_NUM_3, false);
        _uart_ll_sclk_disable(&UART3);
        _uart_ll_enable_bus_clock(UART_NUM_4, false);
        _uart_ll_sclk_disable(&UART4);

        _timer_ll_enable_bus_clock(0, false);
        _timer_ll_enable_clock(&TIMERG0, 0, false);
        _timer_ll_enable_clock(&TIMERG0, 1, false);

        _timer_ll_enable_bus_clock(1, false);
        _timer_ll_enable_clock(&TIMERG1, 0, false);
        _timer_ll_enable_clock(&TIMERG1, 1, false);

        mipi_dsi_brg_ll_enable_ref_clock(&MIPI_DSI_BRIDGE, false);
        _mipi_csi_ll_enable_host_bus_clock(0, false);

        REG_CLR_BIT(SDHOST_CLK_EDGE_SEL_REG, SDHOST_CCLK_EN);

#if CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
        _spimem_ctrlr_ll_unset_clock(0);
#endif

#if !MSPI_TIMING_FLASH_NEEDS_TUNING
        REG_CLR_BIT(SPI_MEM_C_TIMING_CALI_REG, SPI_MEM_C_TIMING_CLK_ENA);
        REG_CLR_BIT(SPI_MEM_C_SMEM_TIMING_CALI_REG, SPI_MEM_C_SMEM_TIMING_CLK_ENA);
#endif

#if !MSPI_TIMING_PSRAM_NEEDS_TUNING
        REG_CLR_BIT(SPI_MEM_S_TIMING_CALI_REG, SPI_MEM_S_TIMING_CLK_ENA);
        REG_CLR_BIT(SPI_MEM_S_SMEM_TIMING_CALI_REG, SPI_MEM_S_SMEM_TIMING_CLK_ENA);
#endif

#if !CONFIG_SPIRAM
        _psram_ctrlr_ll_enable_core_clock(PSRAM_CTRLR_LL_MSPI_ID_2, false);
        _psram_ctrlr_ll_enable_module_clock(PSRAM_CTRLR_LL_MSPI_ID_2, false);
#endif

        _spi_ll_enable_bus_clock(SPI2_HOST, false);
        _spi_ll_enable_bus_clock(SPI3_HOST, false);
        _spi_ll_enable_clock(SPI2_HOST, false);
        _spi_ll_enable_clock(SPI3_HOST, false);

#if !CONFIG_ESP_SYSTEM_HW_PC_RECORD
        /* Disable ASSIST Debug module clock if PC recoreding function is not used,
         * if stack guard function needs it, it will be re-enabled at esp_hw_stack_guard_init */
        _assist_debug_ll_enable_bus_clock(false);
#endif
        // Trace & Bus Monitor (0)
        TRACE0.clock_gate.clk_en = 0;
        TRACE1.clock_gate.clk_en = 0;
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_L2MEMMON_MEM_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_L2MEMMON_SYS_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL0_REG, HP_SYS_CLKRST_REG_TCMMON_SYS_CLK_EN);

        // Crypto Modules
        _aes_ll_enable_bus_clock(false);
        _ds_ll_enable_bus_clock(false);
        _ecc_ll_enable_bus_clock(false);
        _hmac_ll_enable_bus_clock(false);
        _mpi_ll_enable_bus_clock(false);
        _sha_ll_enable_bus_clock(false);

        // USB1.1
        REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL0_REG, LP_CLKRST_USB_OTG11_BK_SYS_CLK_EN);
        REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL0_REG, LP_CLKRST_USB_OTG11_48M_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_USB_OTG11_SYS_CLK_EN);
        // USB2.0
        _usb_utmi_ll_enable_bus_clock(false);
        REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL0_REG, LP_CLKRST_USB_OTG20_BK_SYS_CLK_EN);
        REG_CLR_BIT(LP_CLKRST_HP_USB_CLKRST_CTRL1_REG, LP_CLKRST_USB_OTG20_ULPI_CLK_EN);
        // UHCI
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_UHCI_APB_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_SOC_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_UHCI_SYS_CLK_EN);

#if !CONFIG_USJ_ENABLE_USB_SERIAL_JTAG && !CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG_ENABLED
        // Disable USB-Serial-JTAG clock and it's pad if not used
        usb_serial_jtag_ll_phy_enable_pad(false);
        _usb_serial_jtag_ll_enable_bus_clock(false);
        REG_SET_BIT(USB_SERIAL_JTAG_MEM_CONF_REG, USB_SERIAL_JTAG_USB_MEM_PD);
        REG_CLR_BIT(USB_SERIAL_JTAG_MEM_CONF_REG, USB_SERIAL_JTAG_USB_MEM_CLK_EN);
#endif
    }

    // HP modules' clock source gating control
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_CORE_PMU_PWR_DOWN)) {
        // Only safe to disable these clock source gatings if all HP modules clock configurations has been reset
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_REF_50M_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL1_REG, HP_SYS_CLKRST_REG_REF_25M_CLK_EN);
        // 240M CLK is for Key Management use, should not be gated
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_160M_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_120M_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_80M_CLK_EN);
        REG_CLR_BIT(HP_SYS_CLKRST_REF_CLK_CTRL2_REG, HP_SYS_CLKRST_REG_REF_20M_CLK_EN);
    }

    // LP related clock control
    if ((rst_reason == RESET_REASON_CHIP_POWER_ON) || (rst_reason == RESET_REASON_SYS_SUPER_WDT) \
            || (rst_reason == RESET_REASON_SYS_RWDT) || (rst_reason == RESET_REASON_SYS_BROWN_OUT)) {
        // lpperi,lp peripheral registers get reset for reset level equal or higher than system reset
        lp_uart_ll_sclk_disable(0);
        _lp_uart_ll_enable_bus_clock(0, false);
        _rtcio_ll_enable_io_clock(false);

        if (rst_reason == RESET_REASON_CHIP_POWER_ON) {
            // lp_aon_clkrst, lp_system registers get reset only if chip reset
            _uart_ll_enable_pad_sleep_clock(&UART0, false);
            _uart_ll_enable_pad_sleep_clock(&UART1, false);
            _uart_ll_enable_pad_sleep_clock(&UART2, false);
            _uart_ll_enable_pad_sleep_clock(&UART3, false);
            _uart_ll_enable_pad_sleep_clock(&UART4, false);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_PARLIO_TX_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_PARLIO_RX_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_I2S2_MCLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_I2S1_MCLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_I2S0_MCLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_EMAC_TX_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_EMAC_RX_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PAD_EMAC_TXRX_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_PLL_8M_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_AUDIO_PLL_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_SDIO_PLL2_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_SDIO_PLL1_CLK_EN);
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_SDIO_PLL0_CLK_EN);
#if !CONFIG_SPIRAM_BOOT_INIT
            REG_CLR_BIT(LP_CLKRST_HP_CLK_CTRL_REG, LP_CLKRST_HP_MPLL_500M_CLK_EN);
#endif
            REG_CLR_BIT(LP_SYSTEM_REG_HP_ROOT_CLK_CTRL_REG, LP_SYSTEM_REG_CPU_CLK_EN);
        }
    }
}
