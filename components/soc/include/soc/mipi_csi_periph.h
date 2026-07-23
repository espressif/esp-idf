/*
 * SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MIPI_CSI_SUPPORTED

/**
 * @brief MIPI CSI PHY PLL frequency range
 */
typedef struct {
    uint32_t start_mbps;       /*!< Start frequency of the range (included) */
    uint32_t end_mbps;         /*!< End frequency of the range (included) */
    uint8_t hs_freq_range_sel; /*!< HS operating frequency range selection */
} soc_mipi_csi_phy_pll_freq_range_t;

extern const soc_mipi_csi_phy_pll_freq_range_t soc_mipi_csi_phy_pll_ranges[];
extern const size_t num_of_soc_mipi_csi_phy_pll_ranges;

typedef struct {
    const int host_irq_id;    // interrupt source ID for MIPI CSI Host
} soc_mipi_csi_signal_desc_t;

extern const soc_mipi_csi_signal_desc_t soc_mipi_csi_signals[1]; // only one MIPI CSI peripheral

#endif // SOC_MIPI_CSI_SUPPORTED

#ifdef __cplusplus
}
#endif
