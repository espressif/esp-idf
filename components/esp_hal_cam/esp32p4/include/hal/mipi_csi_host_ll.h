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
#include "hal/mipi_csi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI_HOST_LL_GET_HW(num)   (((num) == 0) ? (&MIPI_CSI_HOST) : NULL)

#define MIPI_CSI_HOST_LL_LANE_NUM_MAX    2

/**
 * @brief Enable CSI host reset output
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_csi_host_ll_enable_reset_output(csi_host_dev_t *dev, bool en)
{
    dev->csi2_resetn.csi2_resetn = !en; // host reset output, active low
}

/**
 * @brief Set the MIPI CSI lane num
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param lanes_num Number of MIPI CSI lanes
 */
static inline void mipi_csi_host_ll_set_active_lanes_num(csi_host_dev_t *dev, int lanes_num)
{
    dev->n_lanes.n_lanes = lanes_num - 1;
}

/**
 * @brief Enable the CSI virtual channel extension
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_csi_host_ll_enable_virtual_channel_extension(csi_host_dev_t *dev, bool en)
{
    dev->vc_extension.vcx = !en; // 0 is enable
}

/**
 * @brief Enable the data de-scrambling on the controller side
 *
 * @param dev Pointer to the CSI Host controller register base address
 * @param en True to enable, False to disable
 */
static inline void mipi_csi_host_ll_enable_scrambling(csi_host_dev_t *dev, bool en)
{
    dev->scrambling.scramble_enable = en;
}

#ifdef __cplusplus
}
#endif
