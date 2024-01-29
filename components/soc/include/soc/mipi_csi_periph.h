/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif
