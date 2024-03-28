/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "hal/assert.h"
#include "hal/misc.h"
#include "soc/mipi_dsi_host_struct.h"
#include "hal/mipi_dsi_types.h"

#define MIPI_DSI_LL_MIN_PHY_MBPS 80
#define MIPI_DSI_LL_MAX_PHY_MBPS 1500

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the PHY clock lane
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_phy_ll_enable_clock_lane(dsi_host_dev_t *dev, bool enable)
{
    dev->phy_rstz.phy_enableclk = enable;
}

/**
 * @brief Reset the digital section of the PHY
 *
 * @param dev Pointer to the DSI Host controller register base address
 */
static inline void mipi_dsi_phy_ll_reset(dsi_host_dev_t *dev)
{
    dev->phy_rstz.phy_rstz = 0;
    dev->phy_rstz.phy_rstz = 1;
}

/**
 * @brief Shutdown the PHY
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param on_off True to power up, false to shut down
 */
static inline void mipi_dsi_phy_ll_power_on_off(dsi_host_dev_t *dev, bool on_off)
{
    dev->phy_rstz.phy_shutdownz = on_off;
}

/**
 * @brief Force the PHY to stay on while in ULPS
 *
 * @note To follow the programming model, use wakeup_pll function
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param enable True to enable, False to disable
 */
static inline void mipi_dsi_phy_ll_force_pll(dsi_host_dev_t *dev, bool force)
{
    dev->phy_rstz.phy_forcepll = force;
}

/**
 * @brief Check if the PHY PLL is locked
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if the PLL is locked, False otherwise
 */
static inline bool mipi_dsi_phy_ll_is_pll_locked(dsi_host_dev_t *dev)
{
    return dev->phy_status.phy_lock;
}

/**
 * @brief Check if the all active lanes are in the stop state
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @return True if the lanes are all in stop state, False otherwise
 */
static inline bool mipi_dsi_phy_ll_are_lanes_stopped(dsi_host_dev_t *dev)
{
    uint32_t status = dev->phy_status.val;
    const uint32_t mask = 1 << 2 | 1 << 4 | 1 << 7;
    return (status & mask) == mask;
}

/**
 * @brief Set the number of active data lanes
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param lane_num Number of lanes used in the DSI link
 */
static inline void mipi_dsi_phy_ll_set_data_lane_number(dsi_host_dev_t *dev, uint32_t lane_num)
{
    dev->phy_if_cfg.n_lanes = lane_num - 1;
}

/**
 * @brief Set the minimum time PHY needs to stay in Stop state before requesting an HS transmission
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param wait_time Time in lane byte clock cycles
 */
static inline void mipi_dsi_phy_ll_set_stop_wait_time(dsi_host_dev_t *dev, uint32_t wait_time)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->phy_if_cfg, phy_stop_wait_time, wait_time);
}

/**
 * @brief Set the maximum time required to perform a read command in lane byte clock cycles
 *
 * @note This can only be modified when no read command is in progress
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param max_time Maximum time required to perform a read command in lane byte clock cycles
 */
static inline void mipi_dsi_phy_ll_set_max_read_time(dsi_host_dev_t *dev, uint32_t max_time)
{
    dev->phy_tmr_rd_cfg.max_rd_time = max_time;
}

/**
 * @brief ULPS mode request on all active data lanes
 *
 * @param dev Pointer to the DSI Host controller register base address
 */
static inline void mipi_dsi_phy_ll_data_enter_ulps(dsi_host_dev_t *dev)
{
    dev->phy_ulps_ctrl.phy_txrequlpslan = 1;
}

/**
 * @brief ULPS mode exit on all active data lanes
 *
 * @param dev Pointer to the DSI Host controller register base address
 */
static inline void mipi_dsi_phy_ll_data_exit_ulps(dsi_host_dev_t *dev)
{
    dev->phy_ulps_ctrl.phy_txexitulpslan = 1;
}

/**
 * @brief ULPS mode request on clock lane
 *
 * @param dev Pointer to the DSI Host controller register base address
 */
static inline void mipi_dsi_phy_ll_clk_enter_ulps(dsi_host_dev_t *dev)
{
    dev->phy_ulps_ctrl.phy_txrequlpsclk = 1;
}

/**
 * @brief ULPS mode exit on clock lane
 *
 * @param dev Pointer to the DSI Host controller register base address
 */
static inline void mipi_dsi_phy_ll_clk_exit_ulps(dsi_host_dev_t *dev)
{
    dev->phy_ulps_ctrl.phy_txexitulpsclk = 1;
}

/**
 * @brief Control the internal interface (clock and pins) between the DSI Host and the D-PHY
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param clock_level Level of the clock
 * @param clear Whether to clear the pins of the PHY
 */
static inline void mipi_dsi_phy_ll_write_clock(dsi_host_dev_t *dev, uint32_t clock_level, bool clear)
{
    dev->phy_tst_ctrl0.val = clock_level << 1 | clear;
}

/**
 * @brief Write the PHY register via internal interface (so-called the test interface)
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param reg_addr Address of the PHY register
 */
static inline void mipi_dsi_phy_ll_write_reg_addr(dsi_host_dev_t *dev, uint8_t reg_addr)
{
    dev->phy_tst_ctrl1.val = (1 << 16) | (reg_addr & 0xFF);
}

/**
 * @brief Write the PHY register value via internal interface (so-called the test interface)
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param reg_val Value to write to the PHY register
 */
static inline void mipi_dsi_phy_ll_write_reg_val(dsi_host_dev_t *dev, uint8_t reg_val)
{
    dev->phy_tst_ctrl1.val = reg_val & 0xFF;
}

/**
 * @brief Send trigger transmission
 *
 * @note Only one bit of the trigger_request is asserted at a time
 * @note Only call this function when the PHY is not in LPDT or ULPS modes
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param trigger_request Trigger request
 */
static inline void mipi_dsi_phy_ll_escape_trigger(dsi_host_dev_t *dev, uint8_t trigger_request)
{
    dev->phy_tx_triggers.phy_tx_triggers = trigger_request;
    while (dev->phy_status.phy_stopstate0lane == 0);
    dev->phy_tx_triggers.phy_tx_triggers = 0;
}

/**
 * @brief Set the time to switch between HS and LP
 *
 * @param dev Pointer to the DSI Host controller register base address
 * @param data_hs2lp Time to switch data lane from HS to LP, in lane byte clock cycles
 * @param data_lp2hs Time to switch data lane from LP to HS, in lane byte clock cycles
 * @param clk_hs2lp Time to switch clock lane from HS to LP, in lane byte clock cycles
 * @param clk_lp2hs Time to switch clock lane from LP to HS, in lane byte clock cycles
 */
static inline void mipi_dsi_phy_ll_set_switch_time(dsi_host_dev_t *dev, uint32_t data_hs2lp, uint32_t data_lp2hs, uint32_t clk_hs2lp, uint32_t clk_lp2hs)
{
    dev->phy_tmr_cfg.phy_hs2lp_time = data_hs2lp;
    dev->phy_tmr_cfg.phy_lp2hs_time = data_lp2hs;
    dev->phy_tmr_lpclk_cfg.phy_clkhs2lp_time = clk_hs2lp;
    dev->phy_tmr_lpclk_cfg.phy_clklp2hs_time = clk_lp2hs;
}

#ifdef __cplusplus

}
#endif
