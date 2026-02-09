/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "hal/misc.h"
#include "hal/assert.h"
#include "soc/mipi_csi_host_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Control the internal interface (clock and pins) between the CSI Host and the d-PHY
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param clock_level Level of the clock
 * @param clear Whether to clear the pins of the PHY
 */
static inline void mipi_csi_phy_ll_write_clock(csi_host_dev_t *dev, uint32_t clock_level, bool clear)
{
    dev->phy_test_ctrl0.val = clock_level << 1 | clear;
}

/**
 * @brief Write the PHY register via test interface
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param reg_addr Address of the PHY register
 */
static inline void mipi_csi_phy_ll_write_reg_addr(csi_host_dev_t *dev, uint8_t reg_addr)
{
    dev->phy_test_ctrl1.val = (1 << 16) | (reg_addr & 0xFF);
}

/**
 * @brief Write the PHY register value via test interface
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param reg_val Value to write to the PHY register
 */
static inline void mipi_csi_phy_ll_write_reg_val(csi_host_dev_t *dev, uint8_t reg_val)
{
    dev->phy_test_ctrl1.val = reg_val & 0xFF;
}

/**
 * @brief Enable dphy shutdown input
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_csi_phy_ll_enable_shutdown_input(csi_host_dev_t *dev, bool en)
{
    dev->phy_shutdownz.phy_shutdownz = !en; // shutdown input, active low
}

/**
 * @brief Enable dphy reset output
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_csi_phy_ll_enable_reset_output(csi_host_dev_t *dev, bool en)
{
    dev->dphy_rstz.dphy_rstz = !en; // phy reset output, active low
}

#ifdef __cplusplus
}
#endif
