/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "soc/hp_sys_clkrst_struct.h"
#include "hal/misc.h"
#include "hal/mipi_dsi_host_ll.h"
#include "hal/mipi_dsi_brg_ll.h"
#include "hal/mipi_dsi_phy_ll.h"

#define MIPI_DSI_LL_NUM_BUS        1 // support only 1 MIPI DSI bus
#define MIPI_DSI_LL_MAX_DATA_LANES 2 // support up to 2 data lanes

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the bus clock for MIPI DSI module
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void _mipi_dsi_ll_enable_bus_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_dsi_sys_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_enable_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _mipi_dsi_ll_enable_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the MIPI DSI module
 *
 * @param group_id Group ID
 */
static inline void mipi_dsi_ll_reset_register(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dsi_brg = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_dsi_brg = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_reset_register(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_reset_register(__VA_ARGS__)

/**
 * @brief Enable MIPI DSI DPI clock
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void mipi_dsi_ll_enable_dpi_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_dsi_dpiclk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_enable_dpi_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_enable_dpi_clock(__VA_ARGS__)

/**
 * @brief Set the clock source for the DSI DPI interface
 *
 * @param group_id Group ID
 * @param source Clock source
 */
static inline void mipi_dsi_ll_set_dpi_clock_source(int group_id, mipi_dsi_dpi_clock_source_t source)
{
    (void)group_id;
    switch (source) {
    case MIPI_DSI_DPI_CLK_SRC_XTAL:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_dsi_dpiclk_src_sel = 0;
        break;
    case MIPI_DSI_DPI_CLK_SRC_PLL_F160M:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_dsi_dpiclk_src_sel = 2;
        break;
    case MIPI_DSI_DPI_CLK_SRC_PLL_F240M:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_dsi_dpiclk_src_sel = 1;
        break;
    default:
        abort();
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_set_dpi_clock_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_set_dpi_clock_source(__VA_ARGS__)

/**
 * @brief Set the clock division factor for the DPI clock source
 *
 * @param group_id Group ID
 * @param div Division factor
 */
static inline void mipi_dsi_ll_set_dpi_clock_div(int group_id, uint32_t div)
{
    (void)group_id;
    HAL_FORCE_MODIFY_U32_REG_FIELD(HP_SYS_CLKRST.peri_clk_ctrl03, reg_mipi_dsi_dpiclk_div_num, div - 1);
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_set_dpi_clock_div(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_set_dpi_clock_div(__VA_ARGS__)

/**
 * @brief Enable MIPI DSI PHY configuration clock
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void mipi_dsi_ll_enable_phy_config_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_dsi_dphy_cfg_clk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_enable_phy_config_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_enable_phy_config_clock(__VA_ARGS__)

/**
 * @brief Enable MIPI DSI PHY PLL reference clock
 *
 * @param group_id Group ID
 * @param enable true to enable, false to disable
 */
static inline void mipi_dsi_ll_enable_phy_reference_clock(int group_id, bool enable)
{
    (void)group_id;
    HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_dsi_dphy_pll_refclk_en = enable;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_enable_phy_reference_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_enable_phy_reference_clock(__VA_ARGS__)

/**
 * @brief Set the clock source for the DSI PHY interface
 *
 * @param group_id Group ID
 * @param source Clock source
 */
static inline void mipi_dsi_ll_set_phy_clock_source(int group_id, mipi_dsi_phy_clock_source_t source)
{
    (void)group_id;
    switch (source) {
    case MIPI_DSI_PHY_CLK_SRC_PLL_F20M:
        HP_SYS_CLKRST.peri_clk_ctrl02.reg_mipi_dsi_dphy_clk_src_sel = 0;
        break;
    case MIPI_DSI_PHY_CLK_SRC_RC_FAST:
        HP_SYS_CLKRST.peri_clk_ctrl02.reg_mipi_dsi_dphy_clk_src_sel = 1;
        break;
    case MIPI_DSI_PHY_CLK_SRC_PLL_F25M:
        HP_SYS_CLKRST.peri_clk_ctrl02.reg_mipi_dsi_dphy_clk_src_sel = 2;
        break;
    default:
        abort();
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_dsi_ll_set_phy_clock_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_dsi_ll_set_phy_clock_source(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
