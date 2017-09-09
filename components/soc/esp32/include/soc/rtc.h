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
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file rtc.h
 * @brief Low-level RTC power, clock, and sleep functions.
 *
 * Functions in this file facilitate configuration of ESP32's RTC_CNTL peripheral.
 * RTC_CNTL peripheral handles many functions:
 * - enables/disables clocks and power to various parts of the chip; this is
 *   done using direct register access (forcing power up or power down) or by
 *   allowing state machines to control power and clocks automatically
 * - handles sleep and wakeup functions
 * - maintains a 48-bit counter which can be used for timekeeping
 *
 * These functions are not thread safe, and should not be viewed as high level
 * APIs. For example, while this file provides a function which can switch
 * CPU frequency, this function is on its own is not sufficient to implement
 * frequency switching in ESP-IDF context: some coordination with RTOS,
 * peripheral drivers, and WiFi/BT stacks is also required.
 *
 * These functions will normally not be used in applications directly.
 * ESP-IDF provides, or will provide, drivers and other facilities to use
 * RTC subsystem functionality.
 *
 * The functions are loosely split into the following groups:
 * - rtc_clk: clock switching, calibration
 * - rtc_time: reading RTC counter, conversion between counter values and time
 * - rtc_sleep: entry into sleep modes
 * - rtc_init: initialization
 */


/**
 * @brief Possible main XTAL frequency values.
 *
 * Enum values should be equal to frequency in MHz.
 */
typedef enum {
    RTC_XTAL_FREQ_AUTO = 0,     //!< Automatic XTAL frequency detection
    RTC_XTAL_FREQ_40M = 40,     //!< 40 MHz XTAL
    RTC_XTAL_FREQ_26M = 26,     //!< 26 MHz XTAL
    RTC_XTAL_FREQ_24M = 24,     //!< 24 MHz XTAL
} rtc_xtal_freq_t;

/**
 * @brief CPU frequency values
 */
typedef enum {
    RTC_CPU_FREQ_XTAL = 0,      //!< Main XTAL frequency
    RTC_CPU_FREQ_80M = 1,       //!< 80 MHz
    RTC_CPU_FREQ_160M = 2,      //!< 160 MHz
    RTC_CPU_FREQ_240M = 3,      //!< 240 MHz
    RTC_CPU_FREQ_2M = 4,        //!< 2 MHz
} rtc_cpu_freq_t;

/**
 * @brief RTC SLOW_CLK frequency values
 */
typedef enum {
    RTC_SLOW_FREQ_RTC = 0,      //!< Internal 150 kHz RC oscillator
    RTC_SLOW_FREQ_32K_XTAL = 1, //!< External 32 kHz XTAL
    RTC_SLOW_FREQ_8MD256 = 2,   //!< Internal 8 MHz RC oscillator, divided by 256
} rtc_slow_freq_t;

/**
 * @brief RTC FAST_CLK frequency values
 */
typedef enum {
    RTC_FAST_FREQ_XTALD4 = 0,   //!< Main XTAL, divided by 4
    RTC_FAST_FREQ_8M = 1,       //!< Internal 8 MHz RC oscillator
} rtc_fast_freq_t;

/* With the default value of CK8M_DFREQ, 8M clock frequency is 8.5 MHz +/- 7% */
#define RTC_FAST_CLK_FREQ_APPROX 8500000

/**
 * @brief Clock source to be calibrated using rtc_clk_cal function
 */
typedef enum {
    RTC_CAL_RTC_MUX = 0,       //!< Currently selected RTC SLOW_CLK
    RTC_CAL_8MD256 = 1,        //!< Internal 8 MHz RC oscillator, divided by 256
    RTC_CAL_32K_XTAL = 2       //!< External 32 kHz XTAL
} rtc_cal_sel_t;

/**
 * Initialization parameters for rtc_clk_init
 */
typedef struct {
    rtc_xtal_freq_t xtal_freq : 8;  //!< Main XTAL frequency
    rtc_cpu_freq_t cpu_freq : 3;    //!< CPU frequency to set
    rtc_fast_freq_t fast_freq : 1;  //!< RTC_FAST_CLK frequency to set
    rtc_slow_freq_t slow_freq : 2;  //!< RTC_SLOW_CLK frequency to set
    uint32_t clk_8m_div : 3;        //!< RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
    uint32_t slow_clk_dcap : 8;     //!< RTC 150k clock adjustment parameter (higher value leads to lower frequency)
    uint32_t clk_8m_dfreq : 8;      //!< RTC 8m clock adjustment parameter (higher value leads to higher frequency)
} rtc_clk_config_t;

/**
 * Default initializer for rtc_clk_config_t
 */
#define RTC_CLK_CONFIG_DEFAULT() { \
    .xtal_freq = RTC_XTAL_FREQ_AUTO, \
    .cpu_freq = RTC_CPU_FREQ_80M, \
    .fast_freq = RTC_FAST_FREQ_8M, \
    .slow_freq = RTC_SLOW_FREQ_RTC, \
    .clk_8m_div = 0, \
    .slow_clk_dcap = RTC_CNTL_SCK_DCAP_DEFAULT, \
    .clk_8m_dfreq = RTC_CNTL_CK8M_DFREQ_DEFAULT, \
}

/**
 * Initialize clocks and set CPU frequency
 *
 * If cfg.xtal_freq is set to RTC_XTAL_FREQ_AUTO, this function will attempt
 * to auto detect XTAL frequency. Auto detection is performed by comparing
 * XTAL frequency with the frequency of internal 8MHz oscillator. Note that at
 * high temperatures the frequency of the internal 8MHz oscillator may drift
 * enough for auto detection to be unreliable.
 * Auto detection code will attempt to distinguish between 26MHz and 40MHz
 * crystals. 24 MHz crystals are not supported by auto detection code.
 * If XTAL frequency can not be auto detected, this 26MHz frequency will be used.
 *
 * @param cfg clock configuration as rtc_clk_config_t
 */
void rtc_clk_init(rtc_clk_config_t cfg);

/**
 * @brief Get main XTAL frequency
 *
 * This is the value passed to rtc_clk_init function, or if the value was
 * RTC_XTAL_FREQ_AUTO, the detected XTAL frequency.
 *
 * @return XTAL frequency, one of rtc_xtal_freq_t
 */
rtc_xtal_freq_t rtc_clk_xtal_freq_get();

/**
 * @brief Enable or disable 32 kHz XTAL oscillator
 * @param en  true to enable, false to disable
 */
void rtc_clk_32k_enable(bool en);

/**
 * @brief Get the state of 32k XTAL oscillator
 * @return true if 32k XTAL oscillator has been enabled
 */
bool rtc_clk_32k_enabled();

/**
 * @brief Enable 32k oscillator, configuring it for fast startup time.
 * Note: to achieve higher frequency stability, rtc_clk_32k_enable function
 * must be called one the 32k XTAL oscillator has started up. This function
 * will initially disable the 32k XTAL oscillator, so it should not be called
 * when the system is using 32k XTAL as RTC_SLOW_CLK.
 */
void rtc_clk_32k_bootstrap();

/**
 * @brief Enable or disable 8 MHz internal oscillator
 *
 * Output from 8 MHz internal oscillator is passed into a configurable
 * divider, which by default divides the input clock frequency by 256.
 * Output of the divider may be used as RTC_SLOW_CLK source.
 * Output of the divider is referred to in register descriptions and code as
 * 8md256 or simply d256. Divider values other than 256 may be configured, but
 * this facility is not currently needed, so is not exposed in the code.
 *
 * When 8MHz/256 divided output is not needed, the divider should be disabled
 * to reduce power consumption.
 *
 * @param clk_8m_en true to enable 8MHz generator
 * @param d256_en true to enable /256 divider
 */
void rtc_clk_8m_enable(bool clk_8m_en, bool d256_en);

/**
 * @brief Get the state of 8 MHz internal oscillator
 * @return true if the oscillator is enabled
 */
bool rtc_clk_8m_enabled();

/**
 * @brief Get the state of /256 divider which is applied to 8MHz clock
 * @return true if the divided output is enabled
 */
bool rtc_clk_8md256_enabled();

/**
 * @brief Enable or disable APLL
 *
 * Output frequency is given by the formula:
 * apll_freq = xtal_freq * (4 + sdm2 + sdm1/256 + sdm0/65536)/((o_div + 2) * 2)
 *
 * The dividend in this expression should be in the range of 240 - 600 MHz.
 *
 * In rev. 0 of ESP32, sdm0 and sdm1 are unused and always set to 0.
 *
 * @param enable  true to enable, false to disable
 * @param sdm0  frequency adjustment parameter, 0..255
 * @param sdm1  frequency adjustment parameter, 0..255
 * @param sdm2  frequency adjustment parameter, 0..63
 * @param o_div  frequency divider, 0..31
 */
void rtc_clk_apll_enable(bool enable, uint32_t sdm0, uint32_t sdm1,
        uint32_t sdm2, uint32_t o_div);

/**
 * @brief Select source for RTC_SLOW_CLK
 * @param slow_freq clock source (one of rtc_slow_freq_t values)
 */
void rtc_clk_slow_freq_set(rtc_slow_freq_t slow_freq);

/**
 * @brief Get the RTC_SLOW_CLK source
 * @return currently selected clock source (one of rtc_slow_freq_t values)
 */
rtc_slow_freq_t rtc_clk_slow_freq_get();

/**
 * @brief Get the approximate frequency of RTC_SLOW_CLK, in Hz
 *
 * - if RTC_SLOW_FREQ_RTC is selected, returns ~150000
 * - if RTC_SLOW_FREQ_32K_XTAL is selected, returns 32768
 * - if RTC_SLOW_FREQ_8MD256 is selected, returns ~33000
 *
 * rtc_clk_cal function can be used to get more precise value by comparing
 * RTC_SLOW_CLK frequency to the frequency of main XTAL.
 *
 * @return RTC_SLOW_CLK frequency, in Hz
 */
uint32_t rtc_clk_slow_freq_get_hz();

/**
 * @brief Select source for RTC_FAST_CLK
 * @param fast_freq clock source (one of rtc_fast_freq_t values)
 */
void rtc_clk_fast_freq_set(rtc_fast_freq_t fast_freq);

/**
 * @brief Get the RTC_FAST_CLK source
 * @return currently selected clock source (one of rtc_fast_freq_t values)
 */
rtc_fast_freq_t rtc_clk_fast_freq_get();

/**
 * @brief Switch CPU frequency
 *
 * If a PLL-derived frequency is requested (80, 160, 240 MHz), this function
 * will enable the PLL. Otherwise, PLL will be disabled.
 * Note: this function is not optimized for switching speed. It may take several
 * hundred microseconds to perform frequency switch.
 *
 * @param cpu_freq  new CPU frequency
 */
void rtc_clk_cpu_freq_set(rtc_cpu_freq_t cpu_freq);

/**
 * @brief Get the currently selected CPU frequency
 *
 * Although CPU can be clocked by APLL and RTC 8M sources, such support is not
 * exposed through this library. As such, this function will not return
 * meaningful values when these clock sources are configured (e.g. using direct
 * access to clock selection registers). In debug builds, it will assert; in
 * release builds, it will return RTC_CPU_FREQ_XTAL.
 *
 * @return CPU frequency (one of rtc_cpu_freq_t values)
 */
rtc_cpu_freq_t rtc_clk_cpu_freq_get();

/**
 * @brief Get corresponding frequency value for rtc_cpu_freq_t enum value
 * @param cpu_freq  CPU frequency, on of rtc_cpu_freq_t values
 * @return CPU frequency, in HZ
 */
uint32_t rtc_clk_cpu_freq_value(rtc_cpu_freq_t cpu_freq);

/**
 * @brief Store new APB frequency value into RTC_APB_FREQ_REG
 *
 * This function doesn't change any hardware clocks.
 *
 * Functions which perform frequency switching and change APB frequency call
 * this function to update the value of APB frequency stored in RTC_APB_FREQ_REG
 * (one of RTC general purpose retention registers). This should not normally
 * be called from application code.
 *
 * @param apb_freq  new APB frequency, in Hz
 */
void rtc_clk_apb_freq_update(uint32_t apb_freq);

/**
 * @brief Get the current stored APB frequency.
 * @return The APB frequency value as last set via rtc_clk_apb_freq_update(), in Hz.
 */
uint32_t rtc_clk_apb_freq_get();

#define RTC_CLK_CAL_FRACT  19  //!< Number of fractional bits in values returned by rtc_clk_cal

/**
 * @brief Measure RTC slow clock's period, based on main XTAL frequency
 *
 * This function will time out and return 0 if the time for the given number
 * of cycles to be counted exceeds the expected time twice. This may happen if
 * 32k XTAL is being calibrated, but the oscillator has not started up (due to
 * incorrect loading capacitance, board design issue, or lack of 32 XTAL on board).
 *
 * @param cal_clk  clock to be measured
 * @param slow_clk_cycles  number of slow clock cycles to average
 * @return average slow clock period in microseconds, Q13.19 fixed point format,
 *         or 0 if calibration has timed out
 */
uint32_t rtc_clk_cal(rtc_cal_sel_t cal_clk, uint32_t slow_clk_cycles);

/**
 * @brief Measure ratio between XTAL frequency and RTC slow clock frequency
 * @param cal_clk slow clock to be measured
 * @param slow_clk_cycles number of slow clock cycles to average
 * @return average ratio between XTAL frequency and slow clock frequency,
 *         Q13.19 fixed point format, or 0 if calibration has timed out.
 */
uint32_t rtc_clk_cal_ratio(rtc_cal_sel_t cal_clk, uint32_t slow_clk_cycles);

/**
 * @brief Convert time interval from microseconds to RTC_SLOW_CLK cycles
 * @param time_in_us Time interval in microseconds
 * @param slow_clk_period  Period of slow clock in microseconds, Q13.19
 *                         fixed point format (as returned by rtc_slowck_cali).
 * @return number of slow clock cycles
 */
uint64_t rtc_time_us_to_slowclk(uint64_t time_in_us, uint32_t period);

/**
 * @brief Convert time interval from RTC_SLOW_CLK to microseconds
 * @param time_in_us Time interval in RTC_SLOW_CLK cycles
 * @param slow_clk_period  Period of slow clock in microseconds, Q13.19
 *                         fixed point format (as returned by rtc_slowck_cali).
 * @return time interval in microseconds
 */
uint64_t rtc_time_slowclk_to_us(uint64_t rtc_cycles, uint32_t period);

/**
 * @brief Get current value of RTC counter
 *
 * RTC has a 48-bit counter which is incremented by 2 every 2 RTC_SLOW_CLK
 * cycles. Counter value is not writable by software. The value is not adjusted
 * when switching to a different RTC_SLOW_CLK source.
 *
 * Note: this function may take up to 1 RTC_SLOW_CLK cycle to execute
 *
 * @return current value of RTC counter
 */
uint64_t rtc_time_get();

/**
 * @brief sleep configuration for rtc_sleep_init function
 */
typedef struct {
    uint32_t soc_clk_sel : 2;           //!< SoC clock select, see RTC_CNTL_SOC_CLK_SEL
    uint32_t lslp_mem_inf_fpu : 1;      //!< force normal voltage in sleep mode (digital domain memory)
    uint32_t rtc_mem_inf_fpu : 1;       //!< force normal voltage in sleep mode (RTC memory)
    uint32_t rtc_mem_inf_follow_cpu : 1;//!< keep low voltage in sleep mode (even if ULP/touch is used)
    uint32_t rtc_fastmem_pd_en : 1;     //!< power down RTC fast memory
    uint32_t rtc_slowmem_pd_en : 1;     //!< power down RTC slow memory
    uint32_t rtc_peri_pd_en : 1;        //!< power down RTC peripherals
    uint32_t wifi_pd_en : 1;            //!< power down WiFi
    uint32_t rom_mem_pd_en : 1;         //!< power down main RAM and ROM
    uint32_t deep_slp : 1;              //!< power down digital domain
    uint32_t wdt_flashboot_mod_en : 1;  //!< enable WDT flashboot mode
    uint32_t dig_dbias_wak : 3;         //!< set bias for digital domain, in active mode
    uint32_t dig_dbias_slp : 3;         //!< set bias for digital domain, in sleep mode
    uint32_t rtc_dbias_wak : 3;         //!< set bias for RTC domain, in active mode
    uint32_t rtc_dbias_slp : 3;         //!< set bias for RTC domain, in sleep mode
    uint32_t lslp_meminf_pd : 1;        //!< remove all peripheral force power up flags
    uint32_t vddsdio_pd_en : 1;         //!< power down VDDSDIO regulator
} rtc_sleep_config_t;

/**
 * Default initializer for rtc_sleep_config_t
 *
 * This initializer sets all fields to "reasonable" values (e.g. suggested for
 * production use) based on a combination of RTC_SLEEP_PD_x flags.
 *
 * @param RTC_SLEEP_PD_x flags combined using bitwise OR
 */
#define RTC_SLEEP_CONFIG_DEFAULT(sleep_flags) { \
    .soc_clk_sel = RTC_CNTL_SOC_CLK_SEL_XTL, \
    .lslp_mem_inf_fpu = 0, \
    .rtc_mem_inf_fpu = 0, \
    .rtc_mem_inf_follow_cpu = ((sleep_flags) & RTC_SLEEP_PD_RTC_MEM_FOLLOW_CPU) ? 1 : 0, \
    .rtc_fastmem_pd_en = ((sleep_flags) & RTC_SLEEP_PD_RTC_FAST_MEM) ? 1 : 0, \
    .rtc_slowmem_pd_en = ((sleep_flags) & RTC_SLEEP_PD_RTC_SLOW_MEM) ? 1 : 0, \
    .rtc_peri_pd_en = ((sleep_flags) & RTC_SLEEP_PD_RTC_PERIPH) ? 1 : 0, \
    .wifi_pd_en = 0, \
    .rom_mem_pd_en = 0, \
    .deep_slp = ((sleep_flags) & RTC_SLEEP_PD_DIG) ? 1 : 0, \
    .wdt_flashboot_mod_en = 0, \
    .dig_dbias_wak = RTC_CNTL_DBIAS_1V10, \
    .dig_dbias_slp = RTC_CNTL_DBIAS_0V90, \
    .rtc_dbias_wak = RTC_CNTL_DBIAS_0V90, \
    .rtc_dbias_slp = RTC_CNTL_DBIAS_0V90, \
    .lslp_meminf_pd = 1, \
    .vddsdio_pd_en = ((sleep_flags) & RTC_SLEEP_PD_VDDSDIO) ? 1 : 0, \
};

#define RTC_SLEEP_PD_DIG                BIT(0)  //!< Deep sleep (power down digital domain)
#define RTC_SLEEP_PD_RTC_PERIPH         BIT(1)  //!< Power down RTC peripherals
#define RTC_SLEEP_PD_RTC_SLOW_MEM       BIT(2)  //!< Power down RTC SLOW memory
#define RTC_SLEEP_PD_RTC_FAST_MEM       BIT(3)  //!< Power down RTC FAST memory
#define RTC_SLEEP_PD_RTC_MEM_FOLLOW_CPU BIT(4)  //!< RTC FAST and SLOW memories are automatically powered up and down along with the CPU
#define RTC_SLEEP_PD_VDDSDIO            BIT(5)  //!< Power down VDDSDIO regulator

/**
 * @brief Prepare the chip to enter sleep mode
 *
 * This function configures various power control state machines to handle
 * entry into light sleep or deep sleep mode, switches APB and CPU clock source
 * (usually to XTAL), and sets bias voltages for digital and RTC power domains.
 *
 * This function does not actually enter sleep mode; this is done using
 * rtc_sleep_start function. Software may do some other actions between
 * rtc_sleep_init and rtc_sleep_start, such as set wakeup timer and configure
 * wakeup sources.
 * @param cfg sleep mode configuration
 */
void rtc_sleep_init(rtc_sleep_config_t cfg);


/**
 * @brief Set target value of RTC counter for RTC_TIMER_TRIG_EN wakeup source
 * @param t value of RTC counter at which wakeup from sleep will happen;
 *          only the lower 48 bits are used
 */
void rtc_sleep_set_wakeup_time(uint64_t t);


#define RTC_EXT0_TRIG_EN    BIT(0)  //!< EXT0 GPIO wakeup
#define RTC_EXT1_TRIG_EN    BIT(1)  //!< EXT1 GPIO wakeup
#define RTC_GPIO_TRIG_EN    BIT(2)  //!< GPIO wakeup (light sleep only)
#define RTC_TIMER_TRIG_EN   BIT(3)  //!< Timer wakeup
#define RTC_SDIO_TRIG_EN    BIT(4)  //!< SDIO wakeup (light sleep only)
#define RTC_MAC_TRIG_EN     BIT(5)  //!< MAC wakeup (light sleep only)
#define RTC_UART0_TRIG_EN   BIT(6)  //!< UART0 wakeup (light sleep only)
#define RTC_UART1_TRIG_EN   BIT(7)  //!< UART1 wakeup (light sleep only)
#define RTC_TOUCH_TRIG_EN   BIT(8)  //!< Touch wakeup
#define RTC_ULP_TRIG_EN     BIT(9)  //!< ULP wakeup
#define RTC_BT_TRIG_EN      BIT(10) //!< BT wakeup (light sleep only)

/**
 * @brief Enter deep or light sleep mode
 *
 * This function enters the sleep mode previously configured using rtc_sleep_init
 * function. Before entering sleep, software should configure wake up sources
 * appropriately (set up GPIO wakeup registers, timer wakeup registers,
 * and so on).
 *
 * If deep sleep mode was configured using rtc_sleep_init, and sleep is not
 * rejected by hardware (based on reject_opt flags), this function never returns.
 * When the chip wakes up from deep sleep, CPU is reset and execution starts
 * from ROM bootloader.
 *
 * If light sleep mode was configured using rtc_sleep_init, this function
 * returns on wakeup, or if sleep is rejected by hardware.
 *
 * @param wakeup_opt  bit mask wake up reasons to enable (RTC_xxx_TRIG_EN flags
 *                    combined with OR)
 * @param reject_opt  bit mask of sleep reject reasons:
 *                      - RTC_CNTL_GPIO_REJECT_EN
 *                      - RTC_CNTL_SDIO_REJECT_EN
 *                    These flags are used to prevent entering sleep when e.g.
 *                    an external host is communicating via SDIO slave
 * @return non-zero if sleep was rejected by hardware
 */
uint32_t rtc_sleep_start(uint32_t wakeup_opt, uint32_t reject_opt);

/**
 * RTC power and clock control initialization settings
 */
typedef struct {
    uint32_t ck8m_wait : 8;         //!< Number of rtc_fast_clk cycles to wait for 8M clock to be ready
    uint32_t xtal_wait : 8;         //!< Number of rtc_fast_clk cycles to wait for XTAL clock to be ready
    uint32_t pll_wait : 8;          //!< Number of rtc_fast_clk cycles to wait for PLL to be ready
    uint32_t clkctl_init : 1;       //!< Perform clock control related initialization
    uint32_t pwrctl_init : 1;       //!< Perform power control related initialization
    uint32_t rtc_dboost_fpd : 1;    //!< Force power down RTC_DBOOST
} rtc_config_t;

/**
 * Default initializer of rtc_config_t.
 *
 * This initializer sets all fields to "reasonable" values (e.g. suggested for
 * production use).
 */
#define RTC_CONFIG_DEFAULT() {\
    .ck8m_wait = RTC_CNTL_CK8M_WAIT_DEFAULT, \
    .xtal_wait = RTC_CNTL_XTL_BUF_WAIT_DEFAULT, \
    .pll_wait  = RTC_CNTL_PLL_BUF_WAIT_DEFAULT, \
    .clkctl_init = 1, \
    .pwrctl_init = 1, \
    .rtc_dboost_fpd = 1 \
}

/**
 * Initialize RTC clock and power control related functions
 * @param cfg configuration options as rtc_config_t
 */
void rtc_init(rtc_config_t cfg);


#ifdef __cplusplus
}
#endif

