// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

/**
 * @file rtc.h
 * @brief Declarations of APIs provided by librtc.a
 *
 * This file is not in the include directory of esp32 component, so it is not
 * part of the public API. As the source code of librtc.a is gradually moved
 * into the ESP-IDF, some of these APIs will be exposed to applications.
 *
 * For now, only esp_deep_sleep function declared in esp_deepsleep.h
 * is part of public API.
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum{
    XTAL_40M = 40,
    XTAL_26M = 26,
    XTAL_24M = 24,
    XTAL_AUTO = 0
} xtal_freq_t;

typedef enum{
    CPU_XTAL = 0,
    CPU_80M = 1,
    CPU_160M = 2,
    CPU_240M = 3,
    CPU_2M = 4
} cpu_freq_t;

typedef enum {
    CALI_RTC_MUX = 0,
    CALI_8MD256 = 1,
    CALI_32K_XTAL = 2
} cali_clk_t;

/**
 * This function must be called to initialize RTC library
 * @param xtal_freq  Frequency of main crystal
 */
void rtc_init_lite(xtal_freq_t xtal_freq);

/**
 * Switch CPU frequency
 * @param cpu_freq  new CPU frequency
 */
void rtc_set_cpu_freq(cpu_freq_t cpu_freq);

/**
 * @brief Return RTC slow clock's period
 * @param cali_clk  clock to calibrate
 * @param slow_clk_cycles  number of slow clock cycles to average
 * @param xtal_freq  chip's main XTAL freq
 * @return average slow clock period in microseconds, Q13.19 fixed point format
 */
uint32_t rtc_slowck_cali(cali_clk_t cali_clk, uint32_t slow_clk_cycles);

/**
 * @brief Convert from microseconds to slow clock cycles
 * @param time_in_us_h Time in microseconds, higher 32 bit part
 * @param time_in_us_l Time in microseconds, lower 32 bit part
 * @param slow_clk_period  Period of slow clock in microseconds, Q13.19 fixed point format (as returned by rtc_slowck_cali).
 * @param[out] cylces_h  output, higher 32 bit part of number of slow clock cycles
 * @param[out] cycles_l  output, lower 32 bit part of number of slow clock cycles
 */
void rtc_usec2rtc(uint32_t time_in_us_h, uint32_t time_in_us_l, uint32_t slow_clk_period, uint32_t *cylces_h, uint32_t *cycles_l);


#define DEEP_SLEEP_PD_NORMAL         BIT(0)   /*!< Base deep sleep mode */
#define DEEP_SLEEP_PD_RTC_PERIPH     BIT(1)   /*!< Power down RTC peripherals */
#define DEEP_SLEEP_PD_RTC_SLOW_MEM   BIT(2)   /*!< Power down RTC SLOW memory */
#define DEEP_SLEEP_PD_RTC_FAST_MEM   BIT(3)   /*!< Power down RTC FAST memory */

/**
 * @brief Prepare for entering sleep mode
 * @param deep_slp   DEEP_SLEEP_PD_ flags combined with OR (DEEP_SLEEP_PD_NORMAL must be included)
 * @param cpu_lp_mode  for deep sleep, should be 0
 */
void rtc_slp_prep_lite(uint32_t deep_slp, uint32_t cpu_lp_mode);


#define RTC_EXT_EVENT0_TRIG     BIT(0)
#define RTC_EXT_EVENT1_TRIG     BIT(1)
#define RTC_GPIO_TRIG           BIT(2)
#define RTC_TIMER_EXPIRE        BIT(3)
#define RTC_SDIO_TRIG           BIT(4)
#define RTC_MAC_TRIG            BIT(5)
#define RTC_UART0_TRIG          BIT(6)
#define RTC_UART1_TRIG          BIT(7)
#define RTC_TOUCH_TRIG          BIT(8)
#define RTC_SAR_TRIG            BIT(9)
#define RTC_BT_TRIG             BIT(10)


#define RTC_EXT_EVENT0_TRIG_EN  RTC_EXT_EVENT0_TRIG
#define RTC_EXT_EVENT1_TRIG_EN  RTC_EXT_EVENT1_TRIG
#define RTC_GPIO_TRIG_EN        RTC_GPIO_TRIG
#define RTC_TIMER_EXPIRE_EN     RTC_TIMER_EXPIRE
#define RTC_SDIO_TRIG_EN        RTC_SDIO_TRIG
#define RTC_MAC_TRIG_EN         RTC_MAC_TRIG
#define RTC_UART0_TRIG_EN       RTC_UART0_TRIG
#define RTC_UART1_TRIG_EN       RTC_UART1_TRIG
#define RTC_TOUCH_TRIG_EN       RTC_TOUCH_TRIG
#define RTC_SAR_TRIG_EN         RTC_SAR_TRIG
#define RTC_BT_TRIG_EN          RTC_BT_TRIG

/**
 * @brief Enter sleep mode for given number of cycles
 * @param cycles_h  higher 32 bit part of number of slow clock cycles
 * @param cycles_l  lower 32 bit part of number of slow clock cycles
 * @param wakeup_opt  wake up reason to enable (RTC_xxx_EN flags combined with OR)
 * @param reject_opt  reserved, should be 0
 * @return TBD
 */
uint32_t rtc_sleep(uint32_t cycles_h, uint32_t cycles_l, uint32_t wakeup_opt, uint32_t reject_opt);

/**
 * @brief Shutdown PHY and RF. TODO: convert this function to another one.
 */
void pm_close_rf(void);

#ifdef __cplusplus
}
#endif

