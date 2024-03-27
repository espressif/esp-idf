/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The hal is not public api, don't use in application code.
 * See readme.md in component/hal/readme.md
 ******************************************************************************/

// The LL for temperature sensor

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"
#include "soc/tsens_struct.h"
#include "soc/lpperi_struct.h"
#include "soc/tsens_reg.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/interrupts.h"
#include "soc/soc_etm_source.h"
#include "hal/temperature_sensor_types.h"
#include "hal/assert.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMPERATURE_SENSOR_LL_ADC_FACTOR     (0.4386)
#define TEMPERATURE_SENSOR_LL_DAC_FACTOR     (27.88)
#define TEMPERATURE_SENSOR_LL_OFFSET_FACTOR  (20.52)
#define TEMPERATURE_SENSOR_LL_MEASURE_MAX    (125)
#define TEMPERATURE_SENSOR_LL_MEASURE_MIN    (-40)

#define TEMPERATURE_SENSOR_LL_INTR_MASK      TSENS_COCPU_TSENS_WAKE_INT_ST

#define TEMPERATURE_SENSOR_LL_ETM_EVENT_TABLE(event)                     \
    (uint32_t [TEMPERATURE_SENSOR_EVENT_MAX]){                           \
        [TEMPERATURE_SENSOR_EVENT_OVER_LIMIT] = TMPSNSR_EVT_OVER_LIMIT,  \
    }[event]

#define TEMPERATURE_SENSOR_LL_ETM_TASK_TABLE(task)                     \
    (uint32_t [TEMPERATURE_SENSOR_TASK_MAX]){                           \
        [TEMPERATURE_SENSOR_TASK_START] = TMPSNSR_TASK_START_SAMPLE,    \
        [TEMPERATURE_SENSOR_TASK_STOP] = TMPSNSR_TASK_STOP_SAMPLE,      \
    }[task]

typedef enum {
    TEMPERATURE_SENSOR_LL_WAKE_ABSOLUTE = 0,
    TEMPERATURE_SENSOR_LL_WAKE_DELTA = 1,
} temperature_sensor_ll_wakeup_mode_t;

/**
 * @brief Enable the temperature sensor power.
 *
 * @param enable true: enable the power.
 */
static inline void temperature_sensor_ll_enable(bool enable)
{
    LP_TSENS.ctrl.power_up = enable;
}

/**
 * @brief Enable the clock
 */
static inline void temperature_sensor_ll_bus_clk_enable(bool enable)
{
    LPPERI.clk_en.ck_en_lp_tsens = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define temperature_sensor_ll_bus_clk_enable(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; temperature_sensor_ll_bus_clk_enable(__VA_ARGS__);} while(0)

/**
 * @brief Reset the Temperature sensor module
 */
static inline void temperature_sensor_ll_reset_module(void)
{
    LPPERI.reset_en.rst_en_lp_tsens = 1;
    LPPERI.reset_en.rst_en_lp_tsens = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define temperature_sensor_ll_reset_module(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; temperature_sensor_ll_reset_module(__VA_ARGS__);} while(0)

/**
 * @brief Select the clock source for temperature sensor.
 *
 * @param clk_src refer to ``temperature_sensor_clk_src_t``
 */
static inline void temperature_sensor_ll_clk_sel(temperature_sensor_clk_src_t clk_src)
{
    // Only LP_PLL can be used as clock source on P4.
}

/**
 * @brief Set the hardware range, you can refer to the table ``temperature_sensor_attributes``
 *
 * @param tsens_dac ``reg_val`` in table ``temperature_sensor_attributes``
 */
static inline void temperature_sensor_ll_set_range(uint32_t range)
{
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC_TSENS_DAC, range);
}

/**
 * @brief Get the raw value of temperature sensor.
 *
 * @return uint32_t raw_value
 */
__attribute__((always_inline))
static inline uint32_t temperature_sensor_ll_get_raw_value(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_TSENS.ctrl, out);
}

/**
 * @brief Get the offset value of temperature sensor.
 *
 * @note This function is only used in legacy driver
 *
 * @return uint32_t offset value
 */
static inline uint32_t temperature_sensor_ll_get_offset(void)
{
    return REGI2C_READ_MASK(I2C_SAR_ADC, I2C_SARADC_TSENS_DAC);
}

/**
 * @brief Get the clock division factor value.
 *
 * @note This function is only used in legacy driver
 *
 * @return uint32_t clock division factor
 */
static inline uint32_t temperature_sensor_ll_get_clk_div(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(LP_TSENS.ctrl, clk_div);
}

/**
 * @brief Set the clock division factor value, actually this has no impact on temperature sensor.
 *        Suggest just keep it as default value 6.
 *
 * @note This function is only used in legacy driver
 *
 * @param clk_div clock division factor, range from 1-10
 */
static inline void temperature_sensor_ll_set_clk_div(uint8_t clk_div)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_TSENS.ctrl, clk_div, clk_div);
}

/**
 * @brief Choose the wake-up mode for temperature sensor
 *
 * @param mode 0: Absolute value mode. 1: Difference mode.
 */
static inline void temperature_sensor_ll_wakeup_mode(temperature_sensor_ll_wakeup_mode_t mode)
{
    LP_TSENS.wakeup_ctrl.wakeup_mode = mode;
}

/**
 * @brief Get temperature sensor interrupt/wakeup in which reason
 *
 * @return uint8_t 0: temperature value lower than low threshold 1: otherwise, higher than high threshold.
 */
__attribute__((always_inline))
static inline uint8_t temperature_sensor_ll_get_wakeup_reason(void)
{
    return LP_TSENS.wakeup_ctrl.wakeup_over_upper_th;
}

/**
 * @brief Configure whether to enable temperature sensor wake up
 *
 * @param en true: enable, false: disable.
 */
static inline void temperature_sensor_ll_wakeup_enable(bool en)
{
    LP_TSENS.wakeup_ctrl.wakeup_en = en;
}

/**
 * @brief Configures the low threshold for temperature sensor to wakeup
 *
 * @param th_low low threshold value.
 */
static inline void temperature_sensor_ll_set_th_low_val(uint8_t th_low)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_TSENS.wakeup_ctrl, wakeup_th_low, th_low);
}

/**
 * @brief Configures the high threshold for temperature sensor to wakeup
 *
 * @param th_high high threshold value.
 */
static inline void temperature_sensor_ll_set_th_high_val(uint8_t th_high)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_TSENS.wakeup_ctrl, wakeup_th_high, th_high);
}

/**
 * @brief Enable temperature sensor interrupt
 *
 * @param enable true: enable. false: disable
 */
static inline void temperature_sensor_ll_enable_intr(bool enable)
{
    LP_TSENS.int_ena.cocpu_tsens_wake_int_ena = enable;
}

/**
 * @brief Clear temperature sensor interrupt
 */
__attribute__((always_inline))
static inline void temperature_sensor_ll_clear_intr(void)
{
    LP_TSENS.int_clr.cocpu_tsens_wake_int_clr = 1;
}

/**
 * @brief Get temperature sensor interrupt status.
 */
static inline volatile void *temperature_sensor_ll_get_intr_status(void)
{
    return &LP_TSENS.int_st;
}

/**
 * @brief Configure whether to enable hardware sampling
 *
 * @param en true: enable, false: disable
 */
static inline void temperature_sensor_ll_sample_enable(bool en)
{
    LP_TSENS.ctrl.sample_en = en;
}

/**
 * @brief Configures the hardware sampling rate
 *
 * @param rate sampling rate
 */
static inline void temperature_sensor_ll_set_sample_rate(uint16_t rate)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(LP_TSENS.sample_rate, sample_rate, rate);
}

#ifdef __cplusplus
}
#endif
