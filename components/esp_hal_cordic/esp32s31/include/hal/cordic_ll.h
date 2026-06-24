/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/cordic_struct.h"
#include "hal/cordic_types.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CORDIC_LL_GET_HW()            (&CORDIC)
#define CORDIC_LL_PRECISION_MAX          (0xF)

/**
 * @brief CORDIC operation mode
 */
typedef enum {
    CORDIC_LL_MODE_REG = 0,                  ///< Register mode: data is transferred via registers
    CORDIC_LL_MODE_DMA,                      ///< DMA mode: data is transferred via DMA
} cordic_ll_mode_t;

/**
 * @brief Enable the hardware clock for CORDIC module
 *
 * @param enable True to enable; false to disable
 */
static inline void cordic_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_apb_clk_en = enable;
}

/**
 * @brief Enable the CORDIC module clock
 *
 * @param enable True to enable; false to disable
 */
static inline void cordic_ll_enable_clock(bool enable)
{
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_clk_en = enable;
}

/**
 * @brief Reset the CORDIC module
 */
static inline void cordic_ll_reset_module_register(void)
{
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_apb_rst_en = 1;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_apb_rst_en = 0;
}

/**
 * @brief Set the clock source for CORDIC module
 *
 * @param source Clock source to use (XTAL, RC_FAST, or PLL_F160M)
 */
static inline void cordic_ll_set_clock_source(cordic_clock_source_t source)
{
    switch (source) {
    case CORDIC_CLK_SRC_XTAL:
        HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_clk_src_sel = 0;
        break;
    case CORDIC_CLK_SRC_RC_FAST:
        HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_clk_src_sel = 1;
        break;
    case CORDIC_CLK_SRC_PLL_F160M:
        HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_clk_src_sel = 2;
        break;
    default:
        HAL_ASSERT(false && "unsupported cordic clock source");
        break;
    }
}

/**
 * @brief Set the calculation function type (e.g., cosine, sine, arctan)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param function Function type to calculate
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_function(cordic_dev_t *hw, cordic_func_t function)
{
    switch (function) {
    case ESP_CORDIC_FUNC_COS:
        hw->csr_cfg.func = 0;
        break;
    case ESP_CORDIC_FUNC_SIN:
        hw->csr_cfg.func = 1;
        break;
    case ESP_CORDIC_FUNC_PHASE:
        hw->csr_cfg.func = 2;
        break;
    case ESP_CORDIC_FUNC_MODULUS:
        hw->csr_cfg.func = 3;
        break;
    case ESP_CORDIC_FUNC_ARCTAN:
        hw->csr_cfg.func = 4;
        break;
    case ESP_CORDIC_FUNC_COSH:
        hw->csr_cfg.func = 5;
        break;
    case ESP_CORDIC_FUNC_SINH:
        hw->csr_cfg.func = 6;
        break;
    case ESP_CORDIC_FUNC_ARCHTANH:
        hw->csr_cfg.func = 7;
        break;
    case ESP_CORDIC_FUNC_LOGE:
        hw->csr_cfg.func = 8;
        break;
    case ESP_CORDIC_FUNC_SQUARE_ROOT:
        hw->csr_cfg.func = 9;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set the calculation mode (register mode or DMA mode)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param mode Operation mode (REG_MODE or DMA_MODE)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_mode(cordic_dev_t *hw, cordic_ll_mode_t mode)
{
    if (mode == CORDIC_LL_MODE_REG) {
        hw->csr_cfg.work_mode = 0;
    } else if (mode == CORDIC_LL_MODE_DMA) {
        hw->csr_cfg.work_mode = 1;
    } else {
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set the calculation precision (iteration count)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param precision Precision value (number of iterations)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_precision(cordic_dev_t *hw, uint16_t precision)
{
    hw->csr_cfg.press = precision;
}

/**
 * @brief Set the input data scale factor
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param scale Scale factor for input data (range depends on function type)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_scale(cordic_dev_t *hw, uint16_t scale)
{
    hw->csr_cfg.scale = scale;
}

/**
 * @brief Set the number of result outputs (one or two)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param number Number of result outputs (ONE_NUM or TWO_NUM)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_result_number(cordic_dev_t *hw, uint8_t number)
{
    hw->csr_cfg.res_num = number - 1;
}

/**
 * @brief Set the number of input arguments (one or two)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param number Number of input arguments (ONE_NUM or TWO_NUM)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_argument_number(cordic_dev_t *hw, uint8_t number)
{
    hw->csr_cfg.arg_num = number - 1;
}

/**
 * @brief Set the result data format size (Q15 or Q31)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param size Result data format (Q15_SIZE or Q31_SIZE)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_result_format(cordic_dev_t *hw, cordic_iq_format_t format)
{
    if (format == ESP_CORDIC_FORMAT_Q15) {
        hw->csr_cfg.res_size = 0;
    } else {
        hw->csr_cfg.res_size = 1;
    }
}

/**
 * @brief Set the argument data format size (Q15 or Q31)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param size Argument data format (Q15_SIZE or Q31_SIZE)
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_argument_format(cordic_dev_t *hw, cordic_iq_format_t format)
{
    if (format == ESP_CORDIC_FORMAT_Q15) {
        hw->csr_cfg.arg_size = 0;
    } else {
        hw->csr_cfg.arg_size = 1;
    }
}

/**
 * @brief Set the first input argument value
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param arg First argument value in fixed-point format
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_argument_1(cordic_dev_t *hw, uint32_t arg)
{
    hw->arg1.arg1_data = arg;
}

/**
 * @brief Set the second input argument value (for two-argument functions)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @param arg Second argument value in fixed-point format
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_argument_2(cordic_dev_t *hw, uint32_t arg)
{
    hw->arg2.arg2_data = arg;
}

/**
 * @brief Start the CORDIC calculation
 *
 * @param hw Pointer to the CORDIC hardware register structure
 */
__attribute__((always_inline))
static inline void cordic_ll_start_calculate(cordic_dev_t *hw)
{
    hw->csr_cfg.update_flag = 1;
}

/**
 * @brief Get the first calculation result
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @return First result value in fixed-point format
 */
__attribute__((always_inline))
static inline uint32_t cordic_ll_get_calculate_result_1(cordic_dev_t *hw)
{
    return hw->res1.res1_data;
}

/**
 * @brief Get the second calculation result (for functions with two outputs)
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @return Second result value in fixed-point format
 */
__attribute__((always_inline))
static inline uint32_t cordic_ll_get_calculate_result_2(cordic_dev_t *hw)
{
    return hw->res2.res2_data;
}

/**
 * @brief Check if the calculation result is ready
 *
 * @param hw Pointer to the CORDIC hardware register structure
 * @return True if result is ready, false otherwise
 */
__attribute__((always_inline))
static inline bool cordic_ll_is_calculate_result_ready(cordic_dev_t *hw)
{
    return hw->csr_cfg.res_rdy_flag;
}

#ifdef __cplusplus
}
#endif
