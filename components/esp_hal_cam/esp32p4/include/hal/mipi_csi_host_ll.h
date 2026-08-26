/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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
 * @brief MIPI CSI host interrupt categories, bit layout aligned with the `int_st_main` register
 */
typedef enum {
    MIPI_CSI_HOST_LL_INTR_PHY_FATAL         = (1 << 0),  /*!< PHY fatal error, e.g. data lane Start-of-Transmission sync error (ErrSotSyncHS) */
    MIPI_CSI_HOST_LL_INTR_PKT_FATAL         = (1 << 1),  /*!< Packet fatal error, e.g. header double-bit ECC error or shorter payload */
    MIPI_CSI_HOST_LL_INTR_BNDRY_FRAME_FATAL = (1 << 2),  /*!< Frame boundary error, e.g. unpaired Frame Start / Frame End */
    MIPI_CSI_HOST_LL_INTR_SEQ_FRAME_FATAL   = (1 << 3),  /*!< Frame sequence error */
    MIPI_CSI_HOST_LL_INTR_CRC_FRAME_FATAL   = (1 << 4),  /*!< A frame containing at least one CRC error */
    MIPI_CSI_HOST_LL_INTR_PLD_CRC_FATAL     = (1 << 5),  /*!< Payload CRC error */
    MIPI_CSI_HOST_LL_INTR_DATA_ID           = (1 << 6),  /*!< Unrecognized or unsupported data type detected */
    MIPI_CSI_HOST_LL_INTR_ECC_CORRECTED     = (1 << 7),  /*!< Header single-bit ECC error (corrected by hardware) */
    MIPI_CSI_HOST_LL_INTR_PHY               = (1 << 16), /*!< PHY error, e.g. Start-of-Transmission error (ErrSotHS) or escape entry error (ErrEsc) */
} mipi_csi_host_ll_intr_t;

/// Mask covering all MIPI CSI host error interrupts
#define MIPI_CSI_HOST_LL_INTR_ERR_ALL  (MIPI_CSI_HOST_LL_INTR_PHY_FATAL | \
                                        MIPI_CSI_HOST_LL_INTR_PKT_FATAL | \
                                        MIPI_CSI_HOST_LL_INTR_BNDRY_FRAME_FATAL | \
                                        MIPI_CSI_HOST_LL_INTR_SEQ_FRAME_FATAL | \
                                        MIPI_CSI_HOST_LL_INTR_CRC_FRAME_FATAL | \
                                        MIPI_CSI_HOST_LL_INTR_PLD_CRC_FATAL | \
                                        MIPI_CSI_HOST_LL_INTR_DATA_ID | \
                                        MIPI_CSI_HOST_LL_INTR_ECC_CORRECTED | \
                                        MIPI_CSI_HOST_LL_INTR_PHY)

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

/**
 * @brief Enable or disable the MIPI CSI host error interrupts
 *
 * @param dev   Pointer to the CSI Host controller register base address
 * @param mask  Interrupt categories to operate on, see `mipi_csi_host_ll_intr_t`
 * @param en    True to enable (unmask), false to disable (mask)
 */
static inline void mipi_csi_host_ll_enable_intr(csi_host_dev_t *dev, uint32_t mask, bool en)
{
    if (mask & MIPI_CSI_HOST_LL_INTR_PHY_FATAL) {
        dev->int_msk_phy_fatal.val = en ? 0x3 : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_PKT_FATAL) {
        dev->int_msk_pkt_fatal.val = en ? 0x3 : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_BNDRY_FRAME_FATAL) {
        dev->int_msk_bndry_frame_fatal.val = en ? 0xffff : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_SEQ_FRAME_FATAL) {
        dev->int_msk_seq_frame_fatal.val = en ? 0xffff : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_CRC_FRAME_FATAL) {
        dev->int_msk_crc_frame_fatal.val = en ? 0xffff : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_PLD_CRC_FATAL) {
        dev->int_msk_pld_crc_fatal.val = en ? 0xffff : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_DATA_ID) {
        dev->int_msk_data_id.val = en ? 0xffff : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_ECC_CORRECTED) {
        dev->int_msk_ecc_corrected.val = en ? 0xffff : 0x0;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_PHY) {
        dev->int_msk_phy.val = en ? 0x30003 : 0x0;
    }
}

/**
 * @brief Get the MIPI CSI host top-level interrupt status
 *
 * @param dev Pointer to the CSI Host controller register base address
 *
 * @return The top-level interrupt status, a combination of `mipi_csi_host_ll_intr_t`
 */
static inline uint32_t mipi_csi_host_ll_get_intr_status(csi_host_dev_t *dev)
{
    return dev->int_st_main.val;
}

/**
 * @brief Clear the MIPI CSI host interrupt status
 *
 * @note The per-category status registers are clear-on-read, reading them also clears the
 *       corresponding bit in the `int_st_main` register.
 *
 * @param dev   Pointer to the CSI Host controller register base address
 * @param mask  Interrupt categories to clear, see `mipi_csi_host_ll_intr_t`
 */
static inline void mipi_csi_host_ll_clear_intr_status(csi_host_dev_t *dev, uint32_t mask)
{
    if (mask & MIPI_CSI_HOST_LL_INTR_PHY_FATAL) {
        (void)dev->int_st_phy_fatal.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_PKT_FATAL) {
        (void)dev->int_st_pkt_fatal.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_BNDRY_FRAME_FATAL) {
        (void)dev->int_st_bndry_frame_fatal.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_SEQ_FRAME_FATAL) {
        (void)dev->int_st_seq_frame_fatal.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_CRC_FRAME_FATAL) {
        (void)dev->int_st_crc_frame_fatal.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_PLD_CRC_FATAL) {
        (void)dev->int_st_pld_crc_fatal.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_DATA_ID) {
        (void)dev->int_st_data_id.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_ECC_CORRECTED) {
        (void)dev->int_st_ecc_corrected.val;
    }
    if (mask & MIPI_CSI_HOST_LL_INTR_PHY) {
        (void)dev->int_st_phy.val;
    }
}

#ifdef __cplusplus
}
#endif
