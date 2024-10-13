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
#include "soc/mipi_csi_bridge_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI_BRG_LL_BRG_NUMS    1
#define MIPI_CSI_BRG_LL_GET_HW(id)  (((id) == 0) ? &MIPI_CSI_BRIDGE : NULL)

/**
 * @brief Enable the clock for MIPI CSI bridge phy
 *
 * @param dev  Pointer to the CSI bridge controller register base address
 * @param en true to enable, false to disable
 */
static inline void mipi_csi_brg_ll_enable_clock(csi_brg_dev_t *dev, bool en)
{
    dev->host_ctrl.csi_enableclk = en;
}

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

/**
 * @brief Set the buffer almost full threshold for the MIPI CSI bridge
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param afull_thrd full threshold
 */
static inline void mipi_csi_brg_ll_set_flow_ctl_buf_afull_thrd(csi_brg_dev_t *dev, size_t afull_thrd)
{
    dev->buf_flow_ctl.csi_buf_afull_thrd = afull_thrd;
}

/**
 * @brief Set the frame data whether contain hsync
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param en 0: frame data doesn't contain hsync. 1: frame data contains hsync.
 */
static inline void mipi_csi_brg_ll_enable_has_hsync(csi_brg_dev_t *dev, bool en)
{
    dev->frame_cfg.has_hsync_e = en;
}

/**
 * @brief Set the min value of data type used for pixel filter.
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param type_min The min data type allowed by bridge's pixel filter.
 * The data type specifie the format and the content of the payload data.
 */
static inline void mipi_csi_brg_ll_set_data_type_min(csi_brg_dev_t *dev, uint16_t type_min)
{
    dev->data_type_cfg.data_type_min = type_min;
}

/**
 * @brief Set the the max value of data type used for pixel filter.
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param type_max The max data type allowed by bridge's pixel filter.
 * The data type specifie the format and the content of the payload data.
 */
static inline void mipi_csi_brg_ll_set_data_type_max(csi_brg_dev_t *dev, uint16_t type_max)
{
    dev->data_type_cfg.data_type_max = type_max;
}

/**
 * @brief Set the DMA interval configuration.
 *
 * @param dev Pointer to the CSI bridge controller register base address
 * @param interval 16'b1: 1 cycle. 16'b11: 2 cycle. ... ... 16'hFFFF: 16 cycle.
 */
static inline void mipi_csi_brg_ll_set_dma_req_interval(csi_brg_dev_t *dev, uint16_t interval)
{
    HAL_FORCE_MODIFY_U32_REG_FIELD(dev->dma_req_interval, dma_req_interval, interval);
}

/**
 * @brief Set the data endianness order in bytes
 *
 * @param dev           Pointer to the CSI bridge controller register base address
 * @param byte_swap_en  byte swap enable or not
 */
static inline void mipi_csi_brg_ll_set_byte_endian(csi_brg_dev_t *dev, bool byte_swap_en)
{
    dev->endian_mode.byte_endian_order = byte_swap_en;
}

#ifdef __cplusplus
}
#endif
