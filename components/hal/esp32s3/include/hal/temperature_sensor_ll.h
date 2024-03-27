/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
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
#include "hal/regi2c_ctrl.h"
#include "soc/regi2c_saradc.h"
#include "soc/apb_saradc_struct.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_struct.h"
#include "hal/temperature_sensor_types.h"
#include "hal/misc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEMPERATURE_SENSOR_LL_ADC_FACTOR     (0.4386)
#define TEMPERATURE_SENSOR_LL_DAC_FACTOR     (27.88)
#define TEMPERATURE_SENSOR_LL_OFFSET_FACTOR  (20.52)
#define TEMPERATURE_SENSOR_LL_MEASURE_MAX    (125)
#define TEMPERATURE_SENSOR_LL_MEASURE_MIN    (-40)

/**
 * @brief Enable the temperature sensor power.
 *
 * @param enable true: enable the power.
 */
static inline void temperature_sensor_ll_enable(bool enable)
{
    SENS.sar_tctrl.tsens_power_up_force = enable;
    SENS.sar_tctrl2.tsens_xpd_force = enable;
    SENS.sar_tctrl.tsens_power_up = enable;
}

/**
 * @brief Enable the clock
 */
static inline void temperature_sensor_ll_bus_clk_enable(bool enable)
{
    SENS.sar_peri_clk_gate_conf.tsens_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define temperature_sensor_ll_bus_clk_enable(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; temperature_sensor_ll_bus_clk_enable(__VA_ARGS__);} while(0)

/**
 * @brief Enable the clock
 */
static inline void temperature_sensor_ll_reset_module(void)
{
    SENS.sar_peri_reset_conf.tsens_reset = 1;
    SENS.sar_peri_reset_conf.tsens_reset = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define temperature_sensor_ll_reset_module(...) do {(void)__DECLARE_RCC_ATOMIC_ENV; temperature_sensor_ll_reset_module(__VA_ARGS__);} while(0)

/**
 * @brief Choose the clock. No need to choose the clock source on ESP32-S3. ESP32-S3
 *        can use RTC clock.
 */
static inline void temperature_sensor_ll_clk_sel(temperature_sensor_clk_src_t clk_src)
{
    // No need to select the temperature sensor clock on esp32s3.
}

/**
 * @brief Set the hardware range, you can refer to the table ``temperature_sensor_attributes``
 *
 * @param tsens_dac ``reg_val`` in table ``temperature_sensor_attributes``
 */
static inline void temperature_sensor_ll_set_range(uint32_t tsens_dac)
{
    REGI2C_WRITE_MASK(I2C_SAR_ADC, I2C_SARADC_TSENS_DAC, tsens_dac);
}

/**
 * @brief Get the raw value of temperature sensor.
 *
 * @return uint32_t raw_value
 */
static inline uint32_t temperature_sensor_ll_get_raw_value(void)
{
    SENS.sar_tctrl.tsens_dump_out = 1;
    while (!SENS.sar_tctrl.tsens_ready) {
    }
    SENS.sar_tctrl.tsens_dump_out = 0;
    return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_tctrl, tsens_out);
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
    return HAL_FORCE_READ_U32_REG_FIELD(SENS.sar_tctrl, tsens_clk_div);
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
    HAL_FORCE_MODIFY_U32_REG_FIELD(SENS.sar_tctrl, tsens_clk_div, clk_div);
}

#ifdef __cplusplus
}
#endif
