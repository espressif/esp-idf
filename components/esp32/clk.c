// Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "esp_clk.h"
#include "esp_clk_internal.h"
#include "rom/ets_sys.h"
#include "rom/uart.h"
#include "rom/rtc.h"
#include "soc/soc.h"
#include "soc/rtc.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/i2s_reg.h"
#include "driver/periph_ctrl.h"
#include "xtensa/core-macros.h"
#include "bootloader_clock.h"

/* Number of cycles to wait from the 32k XTAL oscillator to consider it running.
 * Larger values increase startup delay. Smaller values may cause false positive
 * detection (i.e. oscillator runs for a few cycles and then stops).
 */
#define XTAL_32K_DETECT_CYCLES  32
#define SLOW_CLK_CAL_CYCLES     CONFIG_ESP32_RTC_CLK_CAL_CYCLES

#define MHZ (1000000)

static void select_rtc_slow_clk(rtc_slow_freq_t slow_clk);

// g_ticks_us defined in ROMs for PRO and APP CPU
extern uint32_t g_ticks_per_us_pro;
extern uint32_t g_ticks_per_us_app;

static const char* TAG = "clk";


void esp_clk_init(void)
{
    rtc_config_t cfg = RTC_CONFIG_DEFAULT();
    rtc_init(cfg);

#ifdef CONFIG_COMPATIBLE_PRE_V2_1_BOOTLOADERS
    /* Check the bootloader set the XTAL frequency.

       Bootloaders pre-v2.1 don't do this.
    */
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();
    if (xtal_freq == RTC_XTAL_FREQ_AUTO) {
        ESP_EARLY_LOGW(TAG, "RTC domain not initialised by bootloader");
        bootloader_clock_configure();
    }
#else
    /* If this assertion fails, either upgrade the bootloader or enable CONFIG_COMPATIBLE_PRE_V2_1_BOOTLOADERS */
    assert(rtc_clk_xtal_freq_get() != RTC_XTAL_FREQ_AUTO);
#endif

    rtc_clk_fast_freq_set(RTC_FAST_FREQ_8M);

#ifdef CONFIG_ESP32_RTC_CLOCK_SOURCE_EXTERNAL_CRYSTAL
    select_rtc_slow_clk(RTC_SLOW_FREQ_32K_XTAL);
#else
    select_rtc_slow_clk(RTC_SLOW_FREQ_RTC);
#endif

    uint32_t freq_mhz = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ;
    rtc_cpu_freq_t freq = RTC_CPU_FREQ_80M;
    switch(freq_mhz) {
        case 240:
            freq = RTC_CPU_FREQ_240M;
            break;
        case 160:
            freq = RTC_CPU_FREQ_160M;
            break;
        default:
            freq_mhz = 80;
            /* no break */
        case 80:
            freq = RTC_CPU_FREQ_80M;
            break;
    }

    // Wait for UART TX to finish, otherwise some UART output will be lost
    // when switching APB frequency
    uart_tx_wait_idle(CONFIG_CONSOLE_UART_NUM);
    
    uint32_t freq_before = rtc_clk_cpu_freq_value(rtc_clk_cpu_freq_get()) / MHZ ;
    
    rtc_clk_cpu_freq_set(freq);

    // Re calculate the ccount to make time calculation correct. 
    uint32_t freq_after = CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ;
    XTHAL_SET_CCOUNT( XTHAL_GET_CCOUNT() * freq_after / freq_before );
}

int IRAM_ATTR esp_clk_cpu_freq(void)
{
    return g_ticks_per_us_pro * 1000000;
}

int IRAM_ATTR esp_clk_apb_freq(void)
{
    return MIN(g_ticks_per_us_pro, 80) * 1000000;
}

void IRAM_ATTR ets_update_cpu_frequency(uint32_t ticks_per_us)
{
    /* Update scale factors used by ets_delay_us */
    g_ticks_per_us_pro = ticks_per_us;
    g_ticks_per_us_app = ticks_per_us;
}

static void select_rtc_slow_clk(rtc_slow_freq_t slow_clk)
{
    if (slow_clk == RTC_SLOW_FREQ_32K_XTAL) {
        /* 32k XTAL oscillator needs to be enabled and running before it can
         * be used. Hardware doesn't have a direct way of checking if the
         * oscillator is running. Here we use rtc_clk_cal function to count
         * the number of main XTAL cycles in the given number of 32k XTAL
         * oscillator cycles. If the 32k XTAL has not started up, calibration
         * will time out, returning 0.
         */
        rtc_clk_32k_enable(true);
        uint32_t cal_val = 0;
        uint32_t wait = 0;
        // increment of 'wait' counter equivalent to 3 seconds
        const uint32_t warning_timeout = 3 /* sec */ * 32768 /* Hz */ / (2 * XTAL_32K_DETECT_CYCLES);
        ESP_EARLY_LOGD(TAG, "waiting for 32k oscillator to start up")
        do {
            ++wait;
            cal_val = rtc_clk_cal(RTC_CAL_32K_XTAL, XTAL_32K_DETECT_CYCLES);
            if (wait % warning_timeout == 0) {
                ESP_EARLY_LOGW(TAG, "still waiting for 32k oscillator to start up");
            }
        } while (cal_val == 0);
        ESP_EARLY_LOGD(TAG, "32k oscillator ready, wait=%d", wait);
    }
    rtc_clk_slow_freq_set(slow_clk);
    uint32_t cal_val;
    if (SLOW_CLK_CAL_CYCLES > 0) {
        /* TODO: 32k XTAL oscillator has some frequency drift at startup.
         * Improve calibration routine to wait until the frequency is stable.
         */
        cal_val = rtc_clk_cal(RTC_CAL_RTC_MUX, SLOW_CLK_CAL_CYCLES);
    } else {
        const uint64_t cal_dividend = (1ULL << RTC_CLK_CAL_FRACT) * 1000000ULL;
        cal_val = (uint32_t) (cal_dividend / rtc_clk_slow_freq_get_hz());
    }
    ESP_EARLY_LOGD(TAG, "RTC_SLOW_CLK calibration value: %d", cal_val);
    esp_clk_slowclk_cal_set(cal_val);
}

/* This function is not exposed as an API at this point.
 * All peripheral clocks are default enabled after chip is powered on.
 * This function disables some peripheral clocks when cpu starts.
 * These peripheral clocks are enabled when the peripherals are initialized
 * and disabled when they are de-initialized.
 */
void esp_perip_clk_init(void)
{
    uint32_t common_perip_clk, hwcrypto_perip_clk, wifi_bt_sdio_clk = 0;

#if CONFIG_FREERTOS_UNICORE
    RESET_REASON rst_reas[1];
#else
    RESET_REASON rst_reas[2];
#endif

    rst_reas[0] = rtc_get_reset_reason(0);

#if !CONFIG_FREERTOS_UNICORE
    rst_reas[1] = rtc_get_reset_reason(1);
#endif

    /* For reason that only reset CPU, do not disable the clocks
     * that have been enabled before reset.
     */
    if ((rst_reas[0] >= TGWDT_CPU_RESET && rst_reas[0] <= RTCWDT_CPU_RESET)
#if !CONFIG_FREERTOS_UNICORE
        || (rst_reas[1] >= TGWDT_CPU_RESET && rst_reas[1] <= RTCWDT_CPU_RESET)
#endif
    ) {
        common_perip_clk = ~DPORT_READ_PERI_REG(DPORT_PERIP_CLK_EN_REG);
        hwcrypto_perip_clk = ~DPORT_READ_PERI_REG(DPORT_PERI_CLK_EN_REG);
        wifi_bt_sdio_clk = ~DPORT_READ_PERI_REG(DPORT_WIFI_CLK_EN_REG);
    }
    else {
        common_perip_clk = DPORT_WDG_CLK_EN |
                              DPORT_I2S0_CLK_EN |
#if CONFIG_CONSOLE_UART_NUM != 0
                              DPORT_UART_CLK_EN |
#endif
#if CONFIG_CONSOLE_UART_NUM != 1
                              DPORT_UART1_CLK_EN |
#endif
#if CONFIG_CONSOLE_UART_NUM != 2
                              DPORT_UART2_CLK_EN |
#endif
                              DPORT_SPI_CLK_EN |
                              DPORT_I2C_EXT0_CLK_EN |
                              DPORT_UHCI0_CLK_EN |
                              DPORT_RMT_CLK_EN |
                              DPORT_PCNT_CLK_EN |
                              DPORT_LEDC_CLK_EN |
                              DPORT_UHCI1_CLK_EN |
                              DPORT_TIMERGROUP1_CLK_EN |
//80MHz SPIRAM uses SPI2 as well; it's initialized before this is called. Do not disable the clock for that if this is enabled.
#if !CONFIG_SPIRAM_SPEED_80M
                              DPORT_SPI_CLK_EN_2 |
#endif
                              DPORT_PWM0_CLK_EN |
                              DPORT_I2C_EXT1_CLK_EN |
                              DPORT_CAN_CLK_EN |
                              DPORT_PWM1_CLK_EN |
                              DPORT_I2S1_CLK_EN |
                              DPORT_SPI_DMA_CLK_EN |
                              DPORT_PWM2_CLK_EN |
                              DPORT_PWM3_CLK_EN;
        hwcrypto_perip_clk = DPORT_PERI_EN_AES |
                                DPORT_PERI_EN_SHA |
                                DPORT_PERI_EN_RSA |
                                DPORT_PERI_EN_SECUREBOOT;
        wifi_bt_sdio_clk = DPORT_WIFI_CLK_WIFI_EN |
                              DPORT_WIFI_CLK_BT_EN_M |
                              DPORT_WIFI_CLK_UNUSED_BIT5 |
                              DPORT_WIFI_CLK_UNUSED_BIT12 |
                              DPORT_WIFI_CLK_SDIOSLAVE_EN |
                              DPORT_WIFI_CLK_SDIO_HOST_EN |
                              DPORT_WIFI_CLK_EMAC_EN;
    }

    /* Change I2S clock to audio PLL first. Because if I2S uses 160MHz clock,
     * the current is not reduced when disable I2S clock.
     */
    DPORT_SET_PERI_REG_MASK(I2S_CLKM_CONF_REG(0), I2S_CLKA_ENA);
    DPORT_SET_PERI_REG_MASK(I2S_CLKM_CONF_REG(1), I2S_CLKA_ENA);

    /* Disable some peripheral clocks. */
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, common_perip_clk);
    DPORT_SET_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, common_perip_clk);

    /* Disable hardware crypto clocks. */
    DPORT_CLEAR_PERI_REG_MASK(DPORT_PERI_CLK_EN_REG, hwcrypto_perip_clk);
    DPORT_SET_PERI_REG_MASK(DPORT_PERI_RST_EN_REG, hwcrypto_perip_clk);

    /* Disable WiFi/BT/SDIO clocks. */
    DPORT_CLEAR_PERI_REG_MASK(DPORT_WIFI_CLK_EN_REG, wifi_bt_sdio_clk);

    /* Enable RNG clock. */
    periph_module_enable(PERIPH_RNG_MODULE);
}
