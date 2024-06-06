/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "soc/soc_caps.h"
#include "hal/gpio_types.h"
#include "hal/rtc_io_ll.h"

#define RTCIO_OUTPUT_NORMAL _Pragma ("GCC warning \"'RTCIO_OUTPUT_NORMAL' macro is deprecated\"") RTCIO_LL_OUTPUT_NORMAL
#define RTCIO_OUTPUT_OD     _Pragma ("GCC warning \"'RTCIO_OUTPUT_OD' macro is deprecated\"")     RTCIO_LL_OUTPUT_OD

typedef enum {
    LP_IO_NUM_0 = 0,     /*!< GPIO0, input and output */
    LP_IO_NUM_1 = 1,     /*!< GPIO1, input and output */
    LP_IO_NUM_2 = 2,     /*!< GPIO2, input and output */
    LP_IO_NUM_3 = 3,     /*!< GPIO3, input and output */
    LP_IO_NUM_4 = 4,     /*!< GPIO4, input and output */
    LP_IO_NUM_5 = 5,     /*!< GPIO5, input and output */
    LP_IO_NUM_6 = 6,     /*!< GPIO6, input and output */
    LP_IO_NUM_7 = 7,     /*!< GPIO7, input and output */
#if SOC_RTCIO_PIN_COUNT > 8
    LP_IO_NUM_8 = 8,     /*!< GPIO8, input and output */
    LP_IO_NUM_9 = 9,     /*!< GPIO9, input and output */
    LP_IO_NUM_10 = 10,   /*!< GPIO10, input and output */
    LP_IO_NUM_11 = 11,   /*!< GPIO11, input and output */
    LP_IO_NUM_12 = 12,   /*!< GPIO12, input and output */
    LP_IO_NUM_13 = 13,   /*!< GPIO13, input and output */
    LP_IO_NUM_14 = 14,   /*!< GPIO14, input and output */
    LP_IO_NUM_15 = 15,   /*!< GPIO15, input and output */
#endif
} lp_io_num_t;

typedef enum {
    LP_IO_INTR_DISABLE = 0,     /*!< Disable GPIO interrupt                             */
    LP_IO_INTR_POSEDGE = 1,     /*!< GPIO interrupt type : rising edge                  */
    LP_IO_INTR_NEGEDGE = 2,     /*!< GPIO interrupt type : falling edge                 */
    LP_IO_INTR_ANYEDGE = 3,     /*!< GPIO interrupt type : both rising and falling edge */
    LP_IO_INTR_LOW_LEVEL = 4,   /*!< GPIO interrupt type : input low level trigger      */
    LP_IO_INTR_HIGH_LEVEL = 5,  /*!< GPIO interrupt type : input high level trigger     */
} lp_io_intr_type_t;

/**
 * @brief Initialize a rtcio pin
 * @note If IO is used in LP application, `rtc_gpio_init` must be called at least once
 *       for the using IO before loading LP core firmware in HP Code.
 *
 * @param lp_io_num The rtc io pin to initialize
 */
static inline void ulp_lp_core_gpio_init(lp_io_num_t lp_io_num)
{
#if SOC_LP_IO_CLOCK_IS_INDEPENDENT
    _rtcio_ll_enable_io_clock(true);
#endif
    rtcio_ll_function_select(lp_io_num, RTCIO_LL_FUNC_RTC);
}

/**
 * @brief Enable output
 *
 * @param lp_io_num The rtc io pin to enable output for
 */
static inline void ulp_lp_core_gpio_output_enable(lp_io_num_t lp_io_num)
{
    rtcio_ll_output_enable(lp_io_num);
}

/**
 * @brief Disable output
 *
 * @param lp_io_num The rtc io pin to disable output for
 */
static inline void ulp_lp_core_gpio_output_disable(lp_io_num_t lp_io_num)
{
    rtcio_ll_output_disable(lp_io_num);
}

/**
 * @brief Enable input
 *
 * @param lp_io_num The rtc io pin to enable input for
 */
static inline void ulp_lp_core_gpio_input_enable(lp_io_num_t lp_io_num)
{
    rtcio_ll_input_enable(lp_io_num);
}

/**
 * @brief Disable input
 *
 * @param lp_io_num The rtc io pin to disable input for
 */
static inline void ulp_lp_core_gpio_input_disable(lp_io_num_t lp_io_num)
{
    rtcio_ll_input_disable(lp_io_num);
}

/**
 * @brief Set rtcio output level
 *
 * @param lp_io_num The rtc io pin to set the output level for
 * @param level 0: output low; 1: output high.
 */
static inline void ulp_lp_core_gpio_set_level(lp_io_num_t lp_io_num, uint8_t level)
{
    rtcio_ll_set_level(lp_io_num, level);
}

/**
 * @brief Get rtcio output level
 *
 * @param lp_io_num The rtc io pin to get the output level for
 */
static inline uint32_t ulp_lp_core_gpio_get_level(lp_io_num_t lp_io_num)
{
    return rtcio_ll_get_level(lp_io_num);
}

/**
 * @brief Set rtcio output mode
 *
 * @param lp_io_num The rtc io pin to set the output mode for
 * @param mode RTCIO_LL_OUTPUT_NORMAL: normal, RTCIO_LL_OUTPUT_OD: open drain
 */
static inline void ulp_lp_core_gpio_set_output_mode(lp_io_num_t lp_io_num, rtcio_ll_out_mode_t mode)
{
    rtcio_ll_output_mode_set(lp_io_num, mode);
}

/**
 * @brief Enable internal pull-up resistor
 *
 * @param lp_io_num The rtc io pin to enable pull-up for
 */
static inline void ulp_lp_core_gpio_pullup_enable(lp_io_num_t lp_io_num)
{
    /* Enable internal weak pull-up */
    rtcio_ll_pullup_enable(lp_io_num);
}

/**
 * @brief Disable internal pull-up resistor
 *
 * @param lp_io_num The rtc io pin to disable pull-up for
 */
static inline void ulp_lp_core_gpio_pullup_disable(lp_io_num_t lp_io_num)
{
    /* Disable internal weak pull-up */
    rtcio_ll_pullup_disable(lp_io_num);
}

/**
 * @brief Enable internal pull-down resistor
 *
 * @param lp_io_num The rtc io pin to enable pull-down for
 */
static inline void ulp_lp_core_gpio_pulldown_enable(lp_io_num_t lp_io_num)
{
    /* Enable internal weak pull-down */
    rtcio_ll_pulldown_enable(lp_io_num);
}

/**
 * @brief Disable internal pull-down resistor
 *
 * @param lp_io_num The rtc io pin to disable pull-down for
 */
static inline void ulp_lp_core_gpio_pulldown_disable(lp_io_num_t lp_io_num)
{
    /* Enable internal weak pull-down */
    rtcio_ll_pulldown_disable(lp_io_num);
}

/**
 * @brief Enable interrupt for lp io pin
 *
 * @param lp_io_num The lp io pin to enable interrupt for
 * @param intr_type The interrupt type to enable
 */
static inline void ulp_lp_core_gpio_intr_enable(lp_io_num_t lp_io_num, lp_io_intr_type_t intr_type)
{
    rtcio_ll_intr_enable(lp_io_num, intr_type);
}

/**
 * @brief Clear interrupt status for all lp io
 *
 */
static inline void ulp_lp_core_gpio_clear_intr_status(void)
{
    rtcio_ll_clear_interrupt_status();
}

#ifdef __cplusplus
}
#endif
