/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_caps.h"

typedef enum {
#if SOC_LEDC_SUPPORT_HS_MODE
    LEDC_HIGH_SPEED_MODE = 0, /*!< LEDC high speed speed_mode */
#endif
    LEDC_LOW_SPEED_MODE,      /*!< LEDC low speed speed_mode */
    LEDC_SPEED_MODE_MAX,      /*!< LEDC speed limit */
} ledc_mode_t;

typedef enum {
    LEDC_INTR_DISABLE = 0,    /*!< Disable LEDC interrupt */
    LEDC_INTR_FADE_END,       /*!< Enable LEDC interrupt */
    LEDC_INTR_MAX,
} ledc_intr_type_t;

typedef enum {
    LEDC_DUTY_DIR_DECREASE = 0,    /*!< LEDC duty decrease direction */
    LEDC_DUTY_DIR_INCREASE = 1,    /*!< LEDC duty increase direction */
    LEDC_DUTY_DIR_MAX,
} ledc_duty_direction_t;

typedef enum {
    LEDC_SLOW_CLK_RTC8M = 0,  /*!< LEDC low speed timer clock source is 8MHz RTC clock*/
    LEDC_SLOW_CLK_APB,     /*!< LEDC low speed timer clock source is 80MHz APB clock*/
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
    LEDC_SLOW_CLK_XTAL,    /*!< LEDC low speed timer clock source XTAL clock*/
#endif
} ledc_slow_clk_sel_t;

/**
 * In theory, the following enumeration shall be placed in LEDC driver's header.
 * However, as the next enumeration, `ledc_clk_src_t`, makes the use of some of
 * these values and to avoid mutual inclusion of the headers, we must define it
 * here.
 */
typedef enum {
    LEDC_AUTO_CLK = 0,    /*!< The driver will automatically select the source clock(REF_TICK or APB) based on the giving resolution and duty parameter when init the timer*/
    LEDC_USE_APB_CLK,     /*!< LEDC timer select APB clock as source clock*/
    LEDC_USE_RTC8M_CLK,   /*!< LEDC timer select RTC8M_CLK as source clock. Only for low speed channels and this parameter must be the same for all low speed channels*/
#if SOC_LEDC_SUPPORT_REF_TICK
    LEDC_USE_REF_TICK,    /*!< LEDC timer select REF_TICK clock as source clock*/
#endif
#if SOC_LEDC_SUPPORT_XTAL_CLOCK
    LEDC_USE_XTAL_CLK,    /*!< LEDC timer select XTAL clock as source clock*/
#endif
} ledc_clk_cfg_t;

/* Note: Setting numeric values to match ledc_clk_cfg_t values are a hack to avoid collision with
   LEDC_AUTO_CLK in the driver, as these enums have very similar names and user may pass
   one of these by mistake. */
typedef enum  {
#if SOC_LEDC_SUPPORT_REF_TICK
    LEDC_REF_TICK = LEDC_USE_REF_TICK, /*!< LEDC timer clock divided from reference tick (1Mhz) */
#endif
    LEDC_APB_CLK = LEDC_USE_APB_CLK,  /*!< LEDC timer clock divided from APB clock (80Mhz) */
    LEDC_SCLK = LEDC_USE_APB_CLK      /*!< Selecting this value for LEDC_TICK_SEL_TIMER let the hardware take its source clock from LEDC_APB_CLK_SEL */
} ledc_clk_src_t;

typedef enum {
    LEDC_TIMER_0 = 0, /*!< LEDC timer 0 */
    LEDC_TIMER_1,     /*!< LEDC timer 1 */
    LEDC_TIMER_2,     /*!< LEDC timer 2 */
    LEDC_TIMER_3,     /*!< LEDC timer 3 */
    LEDC_TIMER_MAX,
} ledc_timer_t;

typedef enum {
    LEDC_CHANNEL_0 = 0, /*!< LEDC channel 0 */
    LEDC_CHANNEL_1,     /*!< LEDC channel 1 */
    LEDC_CHANNEL_2,     /*!< LEDC channel 2 */
    LEDC_CHANNEL_3,     /*!< LEDC channel 3 */
    LEDC_CHANNEL_4,     /*!< LEDC channel 4 */
    LEDC_CHANNEL_5,     /*!< LEDC channel 5 */
#if SOC_LEDC_CHANNEL_NUM > 6
    LEDC_CHANNEL_6,     /*!< LEDC channel 6 */
    LEDC_CHANNEL_7,     /*!< LEDC channel 7 */
#endif
    LEDC_CHANNEL_MAX,
} ledc_channel_t;

typedef enum {
    LEDC_TIMER_1_BIT = 1,   /*!< LEDC PWM duty resolution of  1 bits */
    LEDC_TIMER_2_BIT,       /*!< LEDC PWM duty resolution of  2 bits */
    LEDC_TIMER_3_BIT,       /*!< LEDC PWM duty resolution of  3 bits */
    LEDC_TIMER_4_BIT,       /*!< LEDC PWM duty resolution of  4 bits */
    LEDC_TIMER_5_BIT,       /*!< LEDC PWM duty resolution of  5 bits */
    LEDC_TIMER_6_BIT,       /*!< LEDC PWM duty resolution of  6 bits */
    LEDC_TIMER_7_BIT,       /*!< LEDC PWM duty resolution of  7 bits */
    LEDC_TIMER_8_BIT,       /*!< LEDC PWM duty resolution of  8 bits */
    LEDC_TIMER_9_BIT,       /*!< LEDC PWM duty resolution of  9 bits */
    LEDC_TIMER_10_BIT,      /*!< LEDC PWM duty resolution of 10 bits */
    LEDC_TIMER_11_BIT,      /*!< LEDC PWM duty resolution of 11 bits */
    LEDC_TIMER_12_BIT,      /*!< LEDC PWM duty resolution of 12 bits */
    LEDC_TIMER_13_BIT,      /*!< LEDC PWM duty resolution of 13 bits */
    LEDC_TIMER_14_BIT,      /*!< LEDC PWM duty resolution of 14 bits */
#if SOC_LEDC_TIMER_BIT_WIDE_NUM > 14
    LEDC_TIMER_15_BIT,      /*!< LEDC PWM duty resolution of 15 bits */
    LEDC_TIMER_16_BIT,      /*!< LEDC PWM duty resolution of 16 bits */
    LEDC_TIMER_17_BIT,      /*!< LEDC PWM duty resolution of 17 bits */
    LEDC_TIMER_18_BIT,      /*!< LEDC PWM duty resolution of 18 bits */
    LEDC_TIMER_19_BIT,      /*!< LEDC PWM duty resolution of 19 bits */
    LEDC_TIMER_20_BIT,      /*!< LEDC PWM duty resolution of 20 bits */
#endif
    LEDC_TIMER_BIT_MAX,
} ledc_timer_bit_t;

typedef enum {
    LEDC_FADE_NO_WAIT = 0,  /*!< LEDC fade function will return immediately */
    LEDC_FADE_WAIT_DONE,    /*!< LEDC fade function will block until fading to the target duty */
    LEDC_FADE_MAX,
} ledc_fade_mode_t;

#ifdef __cplusplus
}
#endif
