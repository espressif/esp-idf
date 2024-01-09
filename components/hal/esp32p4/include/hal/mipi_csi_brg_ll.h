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
#include "soc/mipi_csi_bridge_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI_BRG_LL_BRG_NUMS    1
#define MIPI_CSI_BRG_LL_GET_HW(id)  (((id) == 0) ? &MIPI_CSI_BRIDGE : NULL)

/**
 * @brief Enable the CSI bridge
 *
 * @param dev  Pointer to the CSI bridge controller register base address
 * @param en   True to enable, false to disable
 */
static inline void mipi_csi_brg_ll_enable(csi_brg_dev_t *dev, bool en)
{
    dev->csi_en.csi_brg_en = en;
}

/**
 * @brief Set the number of 64-bit words in one dma burst transfer
 *
 * @note valid only when csi_bridge is the flow controller
 *
 * @param dev        Pointer to the CSI bridge controller register base address
 * @param burst_len  Number of 64-bit words in one dma burst transfer
 */
static inline void mipi_csi_brg_ll_set_burst_len(csi_brg_dev_t *dev, uint32_t burst_len)
{
    dev->dma_req_cfg.dma_burst_len = burst_len;
}

/**
 * @brief Set horizontal pixel number
 *
 * @param[in] dev        Pointer to the CSI bridge controller register base address
 * @param[in] pixel_num  number of pixels
 */
static inline void mipi_csi_brg_ll_set_intput_data_h_pixel_num(csi_brg_dev_t *dev, uint32_t pixel_num)
{
    dev->frame_cfg.hadr_num = pixel_num;
}

/**
 * @brief Set vertical row number
 *
 * @param[in] dev      Pointer to the CSI bridge controller register base address
 * @param[in] row_num  number of rows
 */
static inline void mipi_csi_brg_ll_set_intput_data_v_row_num(csi_brg_dev_t *dev, uint32_t row_num)
{
    dev->frame_cfg.vadr_num = row_num;
}

#ifdef __cplusplus
}
#endif
