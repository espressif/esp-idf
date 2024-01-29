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

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_MIPI_CSI_SUPPORTED
/**
 * @brief MIPI CSI PHY clock source
 */
typedef soc_periph_mipi_csi_phy_clk_src_t mipi_csi_phy_clock_source_t;
#else
typedef int mipi_csi_phy_clock_source_t;
#endif // SOC_MIPI_CSI_SUPPORTED

/**
 * @brief MIPI CSI Color Type
 */
typedef enum {
    MIPI_CSI_COLOR_RAW8   = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW8),   ///< RAW8
    MIPI_CSI_COLOR_RAW10  = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW10),  ///< RAW10
    MIPI_CSI_COLOR_RAW12  = COLOR_TYPE_ID(COLOR_SPACE_RAW, COLOR_PIXEL_RAW12),  ///< RAW12
    MIPI_CSI_COLOR_RGB565 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB565), ///< RGB565
    MIPI_CSI_COLOR_RGB666 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB666), ///< RGB666
    MIPI_CSI_COLOR_RGB888 = COLOR_TYPE_ID(COLOR_SPACE_RGB, COLOR_PIXEL_RGB888), ///< RGB888
    MIPI_CSI_COLOR_YUV420 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV420), ///< YUV420
    MIPI_CSI_COLOR_YUV422 = COLOR_TYPE_ID(COLOR_SPACE_YUV, COLOR_PIXEL_YUV422), ///< YUV422
} mipi_csi_color_t;

#ifdef __cplusplus
}
#endif
