/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "soc/soc_caps_full.h"

// helper macros to access module attributes
#define SOC_MIPI_DSI_ATTR(_attr) SOC_MODULE_ATTR(MIPI_DSI, _attr)

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_HAS(MIPI_DSI)

/**
 * @brief MIPI DSI PHY PLL frequency range
 */
typedef struct {
    uint32_t start_mbps;       /*!< Start frequency of the range (included) */
    uint32_t end_mbps;         /*!< End frequency of the range (included) */
    uint8_t hs_freq_range_sel; /*!< HS operating frequency range selection */
} soc_mipi_dsi_phy_pll_freq_range_t;

extern const soc_mipi_dsi_phy_pll_freq_range_t soc_mipi_dsi_phy_pll_ranges[];
extern const size_t num_of_soc_mipi_dsi_phy_pll_ranges;

typedef struct {
    const int brg_irq_id;    // interrupt source ID for MIPI DSI Bridge
} soc_mipi_dsi_signal_desc_t;

extern const soc_mipi_dsi_signal_desc_t soc_mipi_dsi_signals[SOC_MIPI_DSI_ATTR(INST_NUM)];

#endif // SOC_HAS(MIPI_DSI)

#ifdef __cplusplus
}
#endif
