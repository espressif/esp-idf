/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <string.h>
#include "hal/twai_types.h"
#include "soc/hp_sys_clkrst_struct.h"
#include "soc/hp_system_struct.h"

#define TWAI_LL_GET(_attr) TWAI_LL_ ## _attr
#define TWAI_LL_SUPPORT(_feat) TWAI_LL_SUPPORT_ ## _feat

#define TWAI_LL_SUPPORT_TIMESTAMP   1
#define TWAI_LL_SUPPORT_RX_STATUS   1
#define TWAI_LL_SUPPORT_MEM_LP      1   // support memory low power mode

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TWAI_LL_MEM_LP_MODE_DEEP_SLEEP,    // memory will enter deep sleep during low power stage, keep memory data
    TWAI_LL_MEM_LP_MODE_LIGHT_SLEEP,   // memory will enter light sleep during low power stage, keep memory data
    TWAI_LL_MEM_LP_MODE_SHUT_DOWN,     // memory will be powered down during low power stage
    TWAI_LL_MEM_LP_MODE_DISABLE,       // disable the low power stage
} twai_ll_mem_lp_mode_sel_t;

/**
 * @brief Enable the bus clock and module clock for TWAI module
 *
 * @param twai_id Hardware ID
 * @param enable true to enable, false to disable
 */
static inline void twai_ll_enable_bus_clock(uint8_t twai_id, bool enable)
{
    switch (twai_id) {
    case 0:
        HP_SYS_CLKRST.twai0_ctrl0.reg_twai0_apb_clk_en = enable;
        break;
    case 1:
        HP_SYS_CLKRST.twai1_ctrl0.reg_twai1_apb_clk_en = enable;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Select the power control mode for the TWAI memory block
 *
 * @param twai_id Hardware ID
 */
static inline void twai_ll_mem_power_by_pmu(uint8_t twai_id)
{
    switch (twai_id) {
    case 0:
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_force_ctrl = 0;
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_lp_en = 0;
        break;
    case 1:
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_force_ctrl = 0;
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_lp_en = 0;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Select the low power mode for the TWAI memory block
 *
 * @param twai_id Hardware ID
 * @param mode Low power mode
 */
static inline void twai_ll_mem_lp_mode_sel(uint8_t twai_id, twai_ll_mem_lp_mode_sel_t mode)
{
    switch (twai_id) {
    case 0:
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_lp_mode = mode;
        break;
    case 1:
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_lp_mode = mode;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Force the TWAI memory block to power on
 *
 * @param twai_id Hardware ID
 */
static inline void twai_ll_mem_force_power_on(uint8_t twai_id)
{
    switch (twai_id) {
    case 0:
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_force_ctrl = 1;
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_lp_en = 0;
        break;
    case 1:
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_force_ctrl = 1;
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_lp_en = 0;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Force the TWAI memory block to enter low power mode
 *
 * @param twai_id Hardware ID
 */
static inline void twai_ll_mem_force_low_power(uint8_t twai_id)
{
    switch (twai_id) {
    case 0:
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_force_ctrl = 1;
        HP_SYSTEM.sys_can0_mem_lp_ctrl.sys_can0_mem_lp_en = 1;
        break;
    case 1:
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_force_ctrl = 1;
        HP_SYSTEM.sys_can1_mem_lp_ctrl.sys_can1_mem_lp_en = 1;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Reset the TWAI module
 *
 * @param twai_id Hardware ID
 */
static inline void twai_ll_reset_register(uint8_t twai_id)
{
    switch (twai_id) {
    case 0:
        HP_SYS_CLKRST.twai0_ctrl0.reg_twai0_rst_en = 1;
        HP_SYS_CLKRST.twai0_ctrl0.reg_twai0_rst_en = 0;
        break;
    case 1:
        HP_SYS_CLKRST.twai1_ctrl0.reg_twai1_rst_en = 1;
        HP_SYS_CLKRST.twai1_ctrl0.reg_twai1_rst_en = 0;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Set clock source for TWAI module
 *
 * @param twai_id Hardware ID
 * @param clk_src Clock source
 */
static inline void twai_ll_set_clock_source(uint8_t twai_id, twai_clock_source_t clk_src)
{
    uint32_t clk_sel = 0;

    switch (clk_src) {
    case TWAI_CLK_SRC_XTAL:
        clk_sel = 0;
        break;
    case TWAI_CLK_SRC_PLL_F80M:
        clk_sel = 2;
        break;
    default:
        HAL_ASSERT(false);
    }

    switch (twai_id) {
    case 0:
        HP_SYS_CLKRST.twai0_ctrl0.reg_twai0_clk_src_sel = clk_sel;
        break;
    case 1:
        HP_SYS_CLKRST.twai1_ctrl0.reg_twai1_clk_src_sel = clk_sel;
        break;
    default:
        HAL_ASSERT(false);
    }
}

/**
 * @brief Enable TWAI module clock source
 *
 * @param twai_id Hardware ID
 * @param enable true to enable, false to disable
 */
static inline void twai_ll_enable_clock(uint8_t twai_id, bool enable)
{
    switch (twai_id) {
    case 0:
        HP_SYS_CLKRST.twai0_ctrl0.reg_twai0_clk_en = enable;
        break;
    case 1:
        HP_SYS_CLKRST.twai1_ctrl0.reg_twai1_clk_en = enable;
        break;
    default:
        HAL_ASSERT(false);
    }
}

#ifdef __cplusplus
}
#endif
