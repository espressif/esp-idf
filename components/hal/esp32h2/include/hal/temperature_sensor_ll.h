/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
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
#include "hal/temperature_sensor_types.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMPERATURE_SENSOR_LL_ADC_FACTOR     (0.4386)
#define TEMPERATURE_SENSOR_LL_DAC_FACTOR     (27.88)
#define TEMPERATURE_SENSOR_LL_OFFSET_FACTOR  (20.52)

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
static inline void temperature_sensor_ll_clk_enable(bool enable)
{
    // No need to enable the temperature clock on esp32h2
}

/**
 * @brief Select the clock source for temperature sensor. On ESP32-H2, temperature sensor
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
    return APB_SARADC.apb_tsens_ctrl.tsens_out;
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
    return APB_SARADC.apb_tsens_ctrl.tsens_clk_div;
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
    APB_SARADC.apb_tsens_ctrl.tsens_clk_div = clk_div;
}

#ifdef __cplusplus
}
#endif
