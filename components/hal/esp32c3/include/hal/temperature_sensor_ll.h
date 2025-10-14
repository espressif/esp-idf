/*
 * SPDX-FileCopyrightText: 2020-2025 Espressif Systems (Shanghai) CO LTD
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
#include "soc/apb_saradc_struct.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "soc/system_struct.h"
#include "hal/temperature_sensor_types.h"
#include "hal/assert.h"
#include "hal/misc.h"
#include "hal/efuse_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMPERATURE_SENSOR_LL_ADC_FACTOR     (0.4386)
#define TEMPERATURE_SENSOR_LL_DAC_FACTOR     (27.88)
#define TEMPERATURE_SENSOR_LL_OFFSET_FACTOR  (20.52)
#define TEMPERATURE_SENSOR_LL_ADC_FACTOR_INT     (4386)
#define TEMPERATURE_SENSOR_LL_DAC_FACTOR_INT     (278800)
#define TEMPERATURE_SENSOR_LL_OFFSET_FACTOR_INT  (205200)
#define TEMPERATURE_SENSOR_LL_DENOMINATOR    (10000)
#define TEMPERATURE_SENSOR_LL_MEASURE_MAX    (125)
#define TEMPERATURE_SENSOR_LL_MEASURE_MIN    (-40)

/**
 * @brief Enable the temperature sensor power.
 *
 * @param enable true: enable the power.
 */
static inline void temperature_sensor_ll_enable(bool enable)
{
    APB_SARADC.apb_tsens_ctrl.tsens_pu = enable;
}

/**
 * @brief Enable the clock
 */
static inline void temperature_sensor_ll_bus_clk_enable(bool enable)
{
    SYSTEM.perip_clk_en1.reg_tsens_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define temperature_sensor_ll_bus_clk_enable(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; temperature_sensor_ll_bus_clk_enable(__VA_ARGS__);} while(0)

/**
 * @brief Reset the Temperature sensor module
 */
static inline void temperature_sensor_ll_reset_module(void)
{
    SYSTEM.perip_rst_en1.reg_tsens_rst = 1;
    SYSTEM.perip_rst_en1.reg_tsens_rst = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define temperature_sensor_ll_reset_module(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; temperature_sensor_ll_reset_module(__VA_ARGS__);} while(0)

/**
 * @brief Select the clock source for temperature sensor. On ESP32-C3, temperautre sensor
 *        can use XTAL or FOSC. To make it convenience, suggest using XTAL all the time.
 *
 * @param clk_src refer to ``temperature_sensor_clk_src_t``
 */
static inline void temperature_sensor_ll_clk_sel(temperature_sensor_clk_src_t clk_src)
{
    uint8_t clk_sel = 0;
    switch (clk_src) {
        case TEMPERATURE_SENSOR_CLK_SRC_XTAL:
            clk_sel = 1;
            break;
        case TEMPERATURE_SENSOR_CLK_SRC_RC_FAST:
            clk_sel = 0;
            break;
        default:
            HAL_ASSERT(false);
            break;
    }
    APB_SARADC.apb_tsens_ctrl2.tsens_clk_sel = clk_sel;
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
static inline uint32_t temperature_sensor_ll_get_raw_value(void)
{
    return HAL_FORCE_READ_U32_REG_FIELD(APB_SARADC.apb_tsens_ctrl, tsens_out);
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
    return HAL_FORCE_READ_U32_REG_FIELD(APB_SARADC.apb_tsens_ctrl, tsens_clk_div);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(APB_SARADC.apb_tsens_ctrl, tsens_clk_div, clk_div);
}

/**
 * @brief Retrieve and calculate the temperature sensor calibration value.
 *
 * @return Temperature calibration value.
 */
static inline int temperature_sensor_ll_load_calib_param(void)
{
    if (efuse_ll_get_blk_version_major() == 0) {
        return 0;
    }
    uint32_t cal_temp = EFUSE.rd_sys_part1_data4.temp_calib;
    // BIT(8) stands for sign: 1: negative, 0: positive
    int tsens_cal = ((cal_temp & BIT(8)) != 0)? -(uint8_t)cal_temp: (uint8_t)cal_temp;
    return tsens_cal;
}

/**
 * @brief Structure for temperature sensor related register values
 */
typedef struct {
    uint32_t tsens_ctrl;      // Temperature sensor control register (APB_SARADC_APB_TSENS_CTRL_REG)
    uint32_t tsens_ctrl2;     // Temperature sensor control register 2 (APB_SARADC_THRES1_HIGH_INT_ST_M)
} tsens_ll_reg_values_t;

/**
 * @brief Read temperature sensor related ADC register values for backup
 *
 * @param reg_values Output parameter, pointer to structure for storing register values
 */
static inline void tsens_ll_backup_registers(tsens_ll_reg_values_t *reg_values)
{
    reg_values->tsens_ctrl = APB_SARADC.apb_tsens_ctrl.val;
    reg_values->tsens_ctrl2 = APB_SARADC.apb_tsens_ctrl2.val;
}

/**
 * @brief Restore temperature sensor related ADC register values from backup
 *
 * @param reg_values Input parameter, pointer to structure containing register values to restore
 */
static inline void tsens_ll_restore_registers(const tsens_ll_reg_values_t *reg_values)
{
    APB_SARADC.apb_tsens_ctrl.val = reg_values->tsens_ctrl;
    APB_SARADC.apb_tsens_ctrl2.val = reg_values->tsens_ctrl2;
}

#ifdef __cplusplus
}
#endif
