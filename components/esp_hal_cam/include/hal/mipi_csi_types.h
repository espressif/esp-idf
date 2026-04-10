/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_assert.h"
#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/color_types.h"
#include "hal/cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI_CUSTOM_DATA_TYPE_MIN      0x30
#define MIPI_CSI_CUSTOM_DATA_TYPE_MAX      0x37

#if SOC_MIPI_CSI_SUPPORTED
/**
 * @brief MIPI CSI PHY clock source
 */
typedef soc_periph_mipi_csi_phy_clk_src_t mipi_csi_phy_clock_source_t;
#else
typedef int mipi_csi_phy_clock_source_t;
#endif // SOC_MIPI_CSI_SUPPORTED

#ifdef __cplusplus
}
#endif
