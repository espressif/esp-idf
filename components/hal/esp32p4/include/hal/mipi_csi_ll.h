/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "hal/hal_utils.h"
#include "hal/mipi_csi_types.h"
#include "hal/mipi_csi_brg_ll.h"
#include "hal/mipi_csi_phy_ll.h"
#include "hal/mipi_csi_host_ll.h"
#include "soc/hp_sys_clkrst_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI_LL_HOST_CTLR_NUMS    1

/*---------------------------------------------------------------
                      CSI Bridge
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for CSI Bridge module
 *
 * @param csi_bridge_id    CSI Bridge ID
 * @param en               enable / disable
 */
static inline void mipi_csi_ll_enable_brg_module_clock(int csi_bridge_id, bool en)
{
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_csi_brg_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_brg_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_enable_brg_module_clock(__VA_ARGS__)

/**
 * @brief Reset the CSI Bridge module
 *
 * @param csi_bridge_id    CSI Bridge ID
 */
static inline void mipi_csi_ll_reset_brg_module_clock(int csi_bridge_id)
{
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_brg = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_brg = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_reset_brg_module_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_reset_brg_module_clock(__VA_ARGS__)

/*---------------------------------------------------------------
                      CSI PHY
---------------------------------------------------------------*/
/**
 * @brief Set the clock source for the MIPI CSI D-PHY
 *
 * @param group_id Group ID
 * @param source Clock source
 */
static inline void mipi_csi_ll_set_phy_clock_source(int group_id, mipi_csi_phy_clock_source_t source)
{
    (void)group_id;
    switch (source) {
    case MIPI_CSI_PHY_CLK_SRC_PLL_F20M:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_clk_src_sel = 0;
        break;
    case MIPI_CSI_PHY_CLK_SRC_RC_FAST:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_clk_src_sel = 1;
        break;
    case MIPI_CSI_PHY_CLK_SRC_PLL_F25M:
        HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_clk_src_sel = 2;
        break;
    default:
        abort();
    }
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_set_phy_clock_source(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_set_phy_clock_source(__VA_ARGS__)

/**
 * @brief Enable MIPI CSI PHY configuration clock
 *
 * @param group_id Group ID
 * @param en true to enable, false to disable
 */
static inline void mipi_csi_ll_enable_phy_config_clock(int group_id, bool en)
{
    (void)group_id;
    HP_SYS_CLKRST.peri_clk_ctrl03.reg_mipi_csi_dphy_cfg_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_phy_config_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_enable_phy_config_clock(__VA_ARGS__)

/*---------------------------------------------------------------
                      CSI Host
---------------------------------------------------------------*/
/**
 * @brief Enable the bus clock for MIPI CSI host
 *
 * @param group_id Group ID
 * @param en true to enable, false to disable
 */
static inline void _mipi_csi_ll_enable_host_bus_clock(int group_id, bool en)
{
    (void)group_id;
    HP_SYS_CLKRST.soc_clk_ctrl1.reg_csi_host_sys_clk_en = en;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_enable_host_bus_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; _mipi_csi_ll_enable_host_bus_clock(__VA_ARGS__)

/**
 * @brief Reset the MIPI CSI host CLK
 *
 * @param group_id Group ID
 */
static inline void mipi_csi_ll_reset_host_clock(int group_id)
{
    (void)group_id;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_host = 1;
    HP_SYS_CLKRST.hp_rst_en0.reg_rst_en_csi_host = 0;
}

/// use a macro to wrap the function, force the caller to use it in a critical section
/// the critical section needs to declare the __DECLARE_RCC_ATOMIC_ENV variable in advance
#define mipi_csi_ll_reset_host_clock(...) (void)__DECLARE_RCC_ATOMIC_ENV; mipi_csi_ll_reset_host_clock(__VA_ARGS__)

#ifdef __cplusplus
}
#endif
