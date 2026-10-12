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

#define CORDIC_LL_GET_HW()            (&CORDIC)
#define CORDIC_LL_PRECISION_MAX       0xF
#define CORDIC_LL_INST_NUM            1U   /*!< Number of CORDIC hardware instances on this chip */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CORDIC operation mode
 */
typedef enum {
    CORDIC_LL_MODE_REG = 0,                  ///< Register mode: data is transferred via registers
    CORDIC_LL_MODE_DMA,                      ///< DMA mode: data is transferred via DMA
} cordic_ll_mode_t;

/**
 * @brief Enable the APB bus clock for CORDIC register access
 *
 * @param enable True to enable; false to disable
 */
static inline void cordic_ll_enable_bus_clock(bool enable)
{
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_apb_clk_en = enable;
}

/**
 * @brief Enable the CORDIC system and functional clocks
 *
 * @param enable True to enable; false to disable
 */
static inline void cordic_ll_enable_clock(bool enable)
{
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_sys_clk_en = enable;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_clk_en = enable;
}

/**
 * @brief Reset the CORDIC module (APB, system clock domain, and computation core)
 */
__attribute__((always_inline))
static inline void cordic_ll_reset_module(void)
{
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_apb_rst_en = 1;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_apb_rst_en = 0;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_sys_rst_en = 1;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_sys_rst_en = 0;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_core_rst_en = 1;
    HP_SYS_CLKRST.cordic_ctrl0.reg_cordic_core_rst_en = 0;
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
 * @brief Set the CORDIC functional clock divider
 *
 * The output clock is: src / (integer + numerator / denominator).
 * integer is programmed as (integer - 1). numerator = 0 and denominator = 0
 * means no fractional part.
 *
 * @param integer     Integral divider, range [1, CORDIC_LL_CLK_DIV_INTEGER_MAX]
 * @param numerator   Fractional numerator, range [0, CORDIC_LL_CLK_DIV_FRACT_MAX]
 * @param denominator Fractional denominator, range [0, CORDIC_LL_CLK_DIV_FRACT_MAX]
 */
static inline void cordic_ll_set_clock_div(uint32_t integer, uint32_t numerator, uint32_t denominator)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.cordic_ctrl1, reg_cordic_clk_div_num, integer - 1);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.cordic_ctrl1, reg_cordic_clk_div_numerator, numerator);
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.cordic_ctrl1, reg_cordic_clk_div_denonimator, denominator);
}

/**
 * @brief Program the calculation configuration fields of csr_cfg with one RMW
 *
 * @param hw        Pointer to the CORDIC hardware register structure
 * @param function  Function type to calculate
 * @param mode      Data transfer mode (register or DMA)
 * @param precision Iteration count programmed into press (already decremented by the caller)
 * @param scale     Input scale exponent
 * @param arg_num     Number of input arguments (1 or 2)
 * @param res_num     Number of result outputs (1 or 2)
 * @param arg_format  IQ format of the input arguments
 * @param res_format  IQ format of the results
 */
__attribute__((always_inline))
static inline void cordic_ll_set_calculate_config(cordic_dev_t *hw,
                                                  cordic_func_t function,
                                                  cordic_ll_mode_t mode,
                                                  uint16_t precision,
                                                  uint16_t scale,
                                                  uint8_t arg_num,
                                                  uint8_t res_num,
                                                  cordic_iq_format_t arg_format,
                                                  cordic_iq_format_t res_format)
{
    cordic_csr_cfg_reg_t csr;
    csr.val = hw->csr_cfg.val;

    switch (function) {
    case ESP_CORDIC_FUNC_COS:
        csr.func = 0;
        break;
    case ESP_CORDIC_FUNC_SIN:
        csr.func = 1;
        break;
    case ESP_CORDIC_FUNC_PHASE:
        csr.func = 2;
        break;
    case ESP_CORDIC_FUNC_MODULUS:
        csr.func = 3;
        break;
    case ESP_CORDIC_FUNC_ARCTAN:
        csr.func = 4;
        break;
    case ESP_CORDIC_FUNC_COSH:
        csr.func = 5;
        break;
    case ESP_CORDIC_FUNC_SINH:
        csr.func = 6;
        break;
    case ESP_CORDIC_FUNC_ARCHTANH:
        csr.func = 7;
        break;
    case ESP_CORDIC_FUNC_LOGE:
        csr.func = 8;
        break;
    case ESP_CORDIC_FUNC_SQUARE_ROOT:
        csr.func = 9;
        break;
    default:
        HAL_ASSERT(false);
    }

    csr.work_mode = mode;
    csr.press = precision;
    csr.scale = scale;
    csr.arg_num = arg_num - 1;
    csr.res_num = res_num - 1;
    csr.res_size = (res_format == ESP_CORDIC_FORMAT_Q15) ? 0 : 1;
    csr.arg_size = (arg_format == ESP_CORDIC_FORMAT_Q15) ? 0 : 1;
    csr.update_flag = 0;
    hw->csr_cfg.val = csr.val;
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
