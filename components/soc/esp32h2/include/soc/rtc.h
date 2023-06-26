/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "soc/soc.h"
#include "soc/clk_tree_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file rtc.h
 * @brief Low-level RTC power, clock functions.
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
 */

#define MHZ (1000000)

#define RTC_SLOW_CLK_150K_CAL_TIMEOUT_THRES(cycles)  (cycles << 10)
#define RTC_SLOW_CLK_32K_CAL_TIMEOUT_THRES(cycles)   (cycles << 12)
#define RTC_FAST_CLK_8M_CAL_TIMEOUT_THRES(cycles)    (TIMG_RTC_CALI_TIMEOUT_THRES_V) // Just use the max timeout thres value

#define OTHER_BLOCKS_POWERUP        1
#define OTHER_BLOCKS_WAIT           1

// TODO: IDF-6254
/* Approximate mapping of voltages to RTC_CNTL_DBIAS_WAK, RTC_CNTL_DBIAS_SLP,
 * RTC_CNTL_DIG_DBIAS_WAK, RTC_CNTL_DIG_DBIAS_SLP values.
 */
#define RTC_CNTL_DBIAS_SLP  5 //sleep dig_dbias & rtc_dbias
#define RTC_CNTL_DBIAS_0V90 13 //digital voltage
#define RTC_CNTL_DBIAS_0V95 16
#define RTC_CNTL_DBIAS_1V00 18
#define RTC_CNTL_DBIAS_1V05 20
#define RTC_CNTL_DBIAS_1V10 23
#define RTC_CNTL_DBIAS_1V15 25
#define RTC_CNTL_DBIAS_1V20 28
#define RTC_CNTL_DBIAS_1V25 30
#define RTC_CNTL_DBIAS_1V30 31 //voltage is about 1.34v in fact

/* Delays for various clock sources to be enabled/switched.
 * All values are in microseconds.
 */
#define SOC_DELAY_RTC_FAST_CLK_SWITCH       3
#define SOC_DELAY_RTC_SLOW_CLK_SWITCH       300
#define SOC_DELAY_RC_FAST_ENABLE            50
#define SOC_DELAY_RC_FAST_DIGI_SWITCH       5
#define SOC_DELAY_RC32K_ENABLE              300
#define SOC_DELAY_LP_PLL_SWITCH             3
#define SOC_DELAY_LP_PLL_ENABLE             50

/* Core voltage: //TODO: IDF-6254
 * Currently, ESP32H2 never adjust its wake voltage in runtime
 * Only sets dig/rtc voltage dbias at startup time
 */
#define DIG_DBIAS_80M       RTC_CNTL_DBIAS_1V20
#define DIG_DBIAS_160M      RTC_CNTL_DBIAS_1V20
#define DIG_DBIAS_XTAL      RTC_CNTL_DBIAS_1V10
#define DIG_DBIAS_2M        RTC_CNTL_DBIAS_1V00

#define RTC_CNTL_PLL_BUF_WAIT_DEFAULT  20
#define RTC_CNTL_XTL_BUF_WAIT_DEFAULT  100
#define RTC_CNTL_CK8M_WAIT_DEFAULT  20
#define RTC_CK8M_ENABLE_WAIT_DEFAULT 5

#define RTC_CNTL_CK8M_DFREQ_DEFAULT  860
#define RTC_CNTL_SCK_DCAP_DEFAULT    85
#define RTC_CNTL_RC32K_DFREQ_DEFAULT 700

/* Various delays to be programmed into power control state machines */
#define RTC_CNTL_XTL_BUF_WAIT_SLP_US            (250)
#define RTC_CNTL_PLL_BUF_WAIT_SLP_CYCLES        (1)
#define RTC_CNTL_CK8M_WAIT_SLP_CYCLES           (4)
#define RTC_CNTL_WAKEUP_DELAY_CYCLES            (5)
#define RTC_CNTL_OTHER_BLOCKS_POWERUP_CYCLES    (1)
#define RTC_CNTL_OTHER_BLOCKS_WAIT_CYCLES       (1)
#define RTC_CNTL_MIN_SLP_VAL_MIN                (2)

/*
set sleep_init default param
*/
#define RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_DEFAULT  5
#define RTC_CNTL_DBG_ATTEN_LIGHTSLEEP_NODROP  0
#define RTC_CNTL_DBG_ATTEN_DEEPSLEEP_DEFAULT  15
#define RTC_CNTL_DBG_ATTEN_MONITOR_DEFAULT  0
#define RTC_CNTL_BIASSLP_MONITOR_DEFAULT  0
#define RTC_CNTL_BIASSLP_SLEEP_ON  0
#define RTC_CNTL_BIASSLP_SLEEP_DEFAULT  1
#define RTC_CNTL_PD_CUR_MONITOR_DEFAULT  0
#define RTC_CNTL_PD_CUR_SLEEP_ON  0
#define RTC_CNTL_PD_CUR_SLEEP_DEFAULT  1
#define RTC_CNTL_DG_VDD_DRV_B_SLP_DEFAULT 254

/*
The follow value is used to get a reasonable rtc voltage dbias value according to digital dbias & some other value
storing in efuse (based on ATE 5k ECO3 chips)
*/
#define K_RTC_MID_MUL10000 215
#define K_DIG_MID_MUL10000 213
#define V_RTC_MID_MUL10000  10800
#define V_DIG_MID_MUL10000  10860

/**
 * @brief Possible main XTAL frequency values.
 *
 * Enum values should be equal to frequency in MHz.
 */
typedef enum {
    RTC_XTAL_FREQ_32M = 32,
} rtc_xtal_freq_t;

/**
 * @brief CPU clock configuration structure
 */
typedef struct rtc_cpu_freq_config_s {
    soc_cpu_clk_src_t source;       //!< The clock from which CPU clock is derived
    uint32_t source_freq_mhz;       //!< Source clock frequency
    uint32_t div;                   //!< Divider, freq_mhz = source_freq_mhz / div
    uint32_t freq_mhz;              //!< CPU clock frequency
} rtc_cpu_freq_config_t;

#define RTC_CLK_CAL_FRACT  19  //!< Number of fractional bits in values returned by rtc_clk_cal

#define RTC_VDDSDIO_TIEH_1_8V 0 //!< TIEH field value for 1.8V VDDSDIO
#define RTC_VDDSDIO_TIEH_3_3V 1 //!< TIEH field value for 3.3V VDDSDIO

/**
 * @brief Clock source to be calibrated using rtc_clk_cal function
 *
 * @note On previous targets, the enum values somehow reflects the register field values of TIMG_RTC_CALI_CLK_SEL
 *       However, this is not true on ESP32H2. The conversion to register field values is explicitly done in
 *       rtc_clk_cal_internal
 */
typedef enum {
    RTC_CAL_RTC_MUX = -1,                                  //!< Currently selected RTC_SLOW_CLK
    RTC_CAL_RC_SLOW = SOC_RTC_SLOW_CLK_SRC_RC_SLOW,        //!< Internal 150kHz RC oscillator
    RTC_CAL_RC32K = SOC_RTC_SLOW_CLK_SRC_RC32K,            //!< Internal 32kHz RC oscillator, as one type of 32k clock
    RTC_CAL_32K_XTAL = SOC_RTC_SLOW_CLK_SRC_XTAL32K,       //!< External 32kHz XTAL, as one type of 32k clock
    RTC_CAL_32K_OSC_SLOW = SOC_RTC_SLOW_CLK_SRC_OSC_SLOW,  //!< External slow clock signal input by lp_pad_gpiox, as one type of 32k clock
    RTC_CAL_RC_FAST                                        //!< Internal 8MHz RC oscillator
} rtc_cal_sel_t;

/**
 * Initialization parameters for rtc_clk_init
 */
typedef struct {
    rtc_xtal_freq_t xtal_freq : 8;             //!< Main XTAL frequency
    uint32_t cpu_freq_mhz : 10;                //!< CPU frequency to set, in MHz
    soc_rtc_fast_clk_src_t fast_clk_src : 2;   //!< RTC_FAST_CLK clock source to choose
    soc_rtc_slow_clk_src_t slow_clk_src : 3;   //!< RTC_SLOW_CLK clock source to choose
    uint32_t clk_rtc_clk_div : 8;
    uint32_t clk_8m_clk_div : 3;               //!< RC_FAST clock divider (division is by clk_8m_div+1, i.e. 0 means ~8MHz frequency)
    uint32_t slow_clk_dcap : 8;                //!< RC_SLOW clock adjustment parameter (higher value leads to lower frequency)
    uint32_t clk_8m_dfreq : 10;                //!< RC_FAST clock adjustment parameter (higher value leads to higher frequency)
    uint32_t rc32k_dfreq : 10;                 //!< Internal RC32K clock adjustment parameter (higher value leads to higher frequency)
} rtc_clk_config_t;

/**
 * Default initializer for rtc_clk_config_t
 */
#define RTC_CLK_CONFIG_DEFAULT() { \
    .xtal_freq = RTC_XTAL_FREQ_32M, \
    .cpu_freq_mhz = 96, \
    .fast_clk_src = SOC_RTC_FAST_CLK_SRC_RC_FAST, \
    .slow_clk_src = SOC_RTC_SLOW_CLK_SRC_RC_SLOW, \
    .clk_rtc_clk_div = 0, \
    .clk_8m_clk_div = 0, \
    .slow_clk_dcap = RTC_CNTL_SCK_DCAP_DEFAULT, \
    .clk_8m_dfreq = RTC_CNTL_CK8M_DFREQ_DEFAULT, \
    .rc32k_dfreq = RTC_CNTL_RC32K_DFREQ_DEFAULT, \
}

/**
 * Initialize clocks and set CPU frequency
 *
 * @param cfg clock configuration as rtc_clk_config_t
 */
void rtc_clk_init(rtc_clk_config_t cfg);

/**
 * @brief Get main XTAL frequency
 *
 * This is the value stored in RTC register RTC_XTAL_FREQ_REG by the bootloader. As passed to
 * rtc_clk_init function
 *
 * @return XTAL frequency, one of rtc_xtal_freq_t
 */
rtc_xtal_freq_t rtc_clk_xtal_freq_get(void);

/**
 * @brief Update XTAL frequency
 *
 * Updates the XTAL value stored in RTC_XTAL_FREQ_REG. Usually this value is ignored
 * after startup.
 *
 * @param xtal_freq New frequency value
 */
void rtc_clk_xtal_freq_update(rtc_xtal_freq_t xtal_freq);

/**
 * @brief Enable or disable 32 kHz XTAL oscillator
 * @param en  true to enable, false to disable
 */
void rtc_clk_32k_enable(bool en);

/**
 * @brief Configure 32 kHz XTAL oscillator to accept external clock signal
 */
void rtc_clk_32k_enable_external(void);

/**
 * @brief Get the state of 32k XTAL oscillator
 * @return true if 32k XTAL oscillator has been enabled
 */
bool rtc_clk_32k_enabled(void);

/**
 * @brief Enable 32k oscillator, configuring it for fast startup time.
 * Note: to achieve higher frequency stability, rtc_clk_32k_enable function
 * must be called one the 32k XTAL oscillator has started up. This function
 * will initially disable the 32k XTAL oscillator, so it should not be called
 * when the system is using 32k XTAL as RTC_SLOW_CLK.
 *
 * @param cycle Number of 32kHz cycles to bootstrap external crystal.
 *              If 0, no square wave will be used to bootstrap crystal oscillation.
 */
void rtc_clk_32k_bootstrap(uint32_t cycle);

/**
 * @brief Enable or disable 32 kHz internal rc oscillator
 * @param en  true to enable, false to disable
 */
void rtc_clk_rc32k_enable(bool enable);

/**
 * @brief Enable or disable 8 MHz internal oscillator
 *
 * @param clk_8m_en true to enable 8MHz generator
 */
void rtc_clk_8m_enable(bool clk_8m_en);

/**
 * @brief Get the state of 8 MHz internal oscillator
 * @return true if the oscillator is enabled
 */
bool rtc_clk_8m_enabled(void);

/**
 * @brief Enable or disable LP_PLL_CLK
 * @param enable true to enable, false to disable
 */
void rtc_clk_lp_pll_enable(bool enable);

/**
 * @brief Select clock source for LP_PLL_CLK
 * @param clk_src clock source (one of soc_lp_pll_clk_src_t values)
 */
void rtc_clk_lp_pll_src_set(soc_lp_pll_clk_src_t clk_src);

/**
 * @brief Select source for RTC_SLOW_CLK
 * @param clk_src clock source (one of soc_rtc_slow_clk_src_t values)
 */
void rtc_clk_slow_src_set(soc_rtc_slow_clk_src_t clk_src);

/**
 * @brief Get the RTC_SLOW_CLK source
 * @return currently selected clock source (one of soc_rtc_slow_clk_src_t values)
 */
soc_rtc_slow_clk_src_t rtc_clk_slow_src_get(void);

/**
 * @brief Get the approximate frequency of RTC_SLOW_CLK, in Hz
 *
 * - if SOC_RTC_SLOW_CLK_SRC_RC_SLOW is selected, returns 136000
 * - if SOC_RTC_SLOW_CLK_SRC_XTAL32K is selected, returns 32768
 * - if SOC_RTC_SLOW_CLK_SRC_RC32K is selected, returns 32768
 * - if SOC_RTC_SLOW_CLK_SRC_OSC_SLOW is selected, returns 32768
 *
 * rtc_clk_cal function can be used to get more precise value by comparing
 * RTC_SLOW_CLK frequency to the frequency of main XTAL.
 *
 * @return RTC_SLOW_CLK frequency, in Hz
 */
uint32_t rtc_clk_slow_freq_get_hz(void);

/**
 * @brief Select source for RTC_FAST_CLK
 * @param clk_src clock source (one of soc_rtc_fast_clk_src_t values)
 */
void rtc_clk_fast_src_set(soc_rtc_fast_clk_src_t clk_src);

/**
 * @brief Get the RTC_FAST_CLK source
 * @return currently selected clock source (one of soc_rtc_fast_clk_src_t values)
 */
soc_rtc_fast_clk_src_t rtc_clk_fast_src_get(void);

/**
 * @brief Get CPU frequency config for a given frequency
 * @param freq_mhz  Frequency in MHz
 * @param[out] out_config Output, CPU frequency configuration structure
 * @return true if frequency can be obtained, false otherwise
 */
bool rtc_clk_cpu_freq_mhz_to_config(uint32_t freq_mhz, rtc_cpu_freq_config_t *out_config);

/**
 * @brief Switch CPU frequency
 *
 * This function sets CPU frequency according to the given configuration
 * structure. It enables PLLs, if necessary.
 *
 * @note This function in not intended to be called by applications in FreeRTOS
 * environment. This is because it does not adjust various timers based on the
 * new CPU frequency.
 *
 * @param config  CPU frequency configuration structure
 */
void rtc_clk_cpu_freq_set_config(const rtc_cpu_freq_config_t *config);

/**
 * @brief Switch CPU frequency (optimized for speed)
 *
 * This function is a faster equivalent of rtc_clk_cpu_freq_set_config.
 * It works faster because it does not disable PLLs when switching from PLL to
 * XTAL and does not enabled them when switching back. If PLL is not already
 * enabled when this function is called to switch from XTAL to PLL frequency,
 * or the PLL which is enabled is the wrong one, this function will fall back
 * to calling rtc_clk_cpu_freq_set_config.
 *
 * Unlike rtc_clk_cpu_freq_set_config, this function relies on static data,
 * so it is less safe to use it e.g. from a panic handler (when memory might
 * be corrupted).
 *
 * @note This function in not intended to be called by applications in FreeRTOS
 * environment. This is because it does not adjust various timers based on the
 * new CPU frequency.
 *
 * @param config  CPU frequency configuration structure
 */
void rtc_clk_cpu_freq_set_config_fast(const rtc_cpu_freq_config_t *config);

/**
 * @brief Get the currently used CPU frequency configuration
 * @param[out] out_config  Output, CPU frequency configuration structure
 */
void rtc_clk_cpu_freq_get_config(rtc_cpu_freq_config_t *out_config);

/**
 * @brief Switch CPU clock source to XTAL
 *
 * Short form for filling in rtc_cpu_freq_config_t structure and calling
 * rtc_clk_cpu_freq_set_config when a switch to XTAL is needed.
 * Assumes that XTAL frequency has been determined — don't call in startup code.
 *
 * @note On ESP32H2, this function will check whether BBPLL can be disabled. If there is no consumer, then BBPLL will be
 * turned off. The behaviour is the same as using rtc_clk_cpu_freq_set_config to switch cpu clock source to XTAL.
 */
void rtc_clk_cpu_freq_set_xtal(void);

/**
 * @brief Get the current stored APB frequency.
 * @return The APB frequency value as last set via rtc_clk_apb_freq_update(), in Hz.
 */
uint32_t rtc_clk_apb_freq_get(void);

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
uint64_t rtc_time_get(void);

/**
 * @brief Busy loop until next RTC_SLOW_CLK cycle
 *
 * This function returns not earlier than the next RTC_SLOW_CLK clock cycle.
 * In some cases (e.g. when RTC_SLOW_CLK cycle is very close), it may return
 * one RTC_SLOW_CLK cycle later.
 */
void rtc_clk_wait_for_slow_cycle(void);

/**
 * @brief Enable the rtc digital 8M clock
 *
 * This function is used to enable the digital rtc 8M clock to support peripherals.
 * For enabling the analog 8M clock, using `rtc_clk_8M_enable` function above.
 */
void rtc_dig_clk8m_enable(void);

/**
 * @brief Disable the rtc digital 8M clock
 *
 * This function is used to disable the digital rtc 8M clock, which is only used to support peripherals.
 */
void rtc_dig_clk8m_disable(void);

/**
 * @brief Get whether the rtc digital 8M clock is enabled
 */
bool rtc_dig_8m_enabled(void);

/**
 * @brief Calculate the real clock value after the clock calibration
 *
 * @param cal_val Average slow clock period in microseconds, fixed point value as returned from `rtc_clk_cal`
 * @return Frequency of the clock in Hz
 */
uint32_t rtc_clk_freq_cal(uint32_t cal_val);

// -------------------------- CLOCK TREE DEFS ALIAS ----------------------------
// **WARNING**: The following are only for backwards compatibility.
// Please use the declarations in soc/clk_tree_defs.h instead.
/**
 * @brief CPU clock source
 */
typedef soc_cpu_clk_src_t rtc_cpu_freq_src_t;
#define RTC_CPU_FREQ_SRC_XTAL SOC_CPU_CLK_SRC_XTAL  //!< XTAL
#define RTC_CPU_FREQ_SRC_PLL SOC_CPU_CLK_SRC_PLL    //!< PLL (96M)
#define RTC_CPU_FREQ_SRC_8M SOC_CPU_CLK_SRC_RC_FAST //!< Internal 8M RTC oscillator

/**
 * @brief RTC SLOW_CLK frequency values
 */
typedef soc_rtc_slow_clk_src_t rtc_slow_freq_t;
#define RTC_SLOW_FREQ_RTC SOC_RTC_SLOW_CLK_SRC_RC_SLOW         //!< Internal 150 kHz RC oscillator
#define RTC_SLOW_FREQ_32K_XTAL SOC_RTC_SLOW_CLK_SRC_XTAL32K    //!< External 32 kHz XTAL

/**
 * @brief RTC FAST_CLK frequency values
 */
typedef soc_rtc_fast_clk_src_t rtc_fast_freq_t;
#define RTC_FAST_FREQ_XTALD4 SOC_RTC_FAST_CLK_SRC_XTAL_DIV  //!< Main XTAL, divided by 2
#define RTC_FAST_FREQ_8M SOC_RTC_FAST_CLK_SRC_RC_FAST       //!< Internal 8 MHz RC oscillator

/* Alias of frequency related macros */
#define RTC_FAST_CLK_FREQ_APPROX    SOC_CLK_RC_FAST_FREQ_APPROX
#define RTC_FAST_CLK_FREQ_8M        SOC_CLK_RC_FAST_FREQ_APPROX
#define RTC_SLOW_CLK_FREQ_150K      SOC_CLK_RC_SLOW_FREQ_APPROX
#define RTC_SLOW_CLK_FREQ_32K       SOC_CLK_XTAL32K_FREQ_APPROX

/* Alias of deprecated function names */
#define rtc_clk_slow_freq_set(slow_freq) rtc_clk_slow_src_set(slow_freq)
#define rtc_clk_slow_freq_get() rtc_clk_slow_src_get()
#define rtc_clk_fast_freq_set(fast_freq) rtc_clk_fast_src_set(fast_freq)
#define rtc_clk_fast_freq_get() rtc_clk_fast_src_get()

#ifdef __cplusplus
}
#endif
