/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/periph_defs.h"
#include "soc/clk_tree_defs.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LP_PERIPHERALS_SUPPORTED
/**
 * @brief Enable LP peripheral module by un-gating the clock and de-asserting the reset signal.
 *
 * @param[in] lp_periph LP peripheral module
 *
 * @note If @c lp_periph_module_enable() is called a number of times,
 *       @c lp_periph_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
void lp_periph_module_enable(lp_periph_module_t lp_periph);

/**
 * @brief Disable LP peripheral module by gating the clock and asserting the reset signal.
 *
 * @param[in] lp_periph LP peripheral module
 *
 * @note If @c lp_periph_module_enable() is called a number of times,
 *       @c lp_periph_module_disable() has to be called the same number of times,
 *       in order to put the peripheral into disabled state.
 */
void lp_periph_module_disable(lp_periph_module_t lp_periph);

/**
 * @brief Reset LP peripheral module by asserting and de-asserting the reset signal.
 *
 * @param[in] lp_periph LP peripheral module
 *
 * @note Calling this function does not enable or disable the clock for the module.
 */
void lp_periph_module_reset(lp_periph_module_t lp_periph);

/**
 * @brief Set LP peripheral module clock source.
 *
 * @param[in] lp_periph LP peripheral module
 * @param[in] clk_src Module clock source, check soc/clk_tree_defs.h for the available clock sources for each LP peripheral
 */
void lp_periph_set_clk_src(lp_periph_module_t lp_periph, soc_module_clk_t clk_src);
#endif

#ifdef __cplusplus
}
#endif
