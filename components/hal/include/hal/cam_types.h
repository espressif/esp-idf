/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/clk_tree_defs.h"
#include "hal/color_types.h"
#include "hal/cam_ctlr_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCDCAM_CAM_DATA_WIDTH_MAX
#define CAP_DVP_PERIPH_NUM      SOC_LCDCAM_CAM_PERIPH_NUM   /*!< DVP port number */
#define CAM_DVP_DATA_SIG_NUM    SOC_LCDCAM_CAM_DATA_WIDTH_MAX   /*!< DVP data bus width of CAM */
#else
#define CAP_DVP_PERIPH_NUM      0                           /*!< Default value */
#define CAM_DVP_DATA_SIG_NUM    0                           /*!< Default value */
#endif

#if SOC_LCDCAM_CAM_SUPPORTED
typedef soc_periph_cam_clk_src_t cam_clock_source_t;        /*!< Clock source type of CAM */
#else
typedef int cam_clock_source_t;                             /*!< Default type */
#endif

#ifdef __cplusplus
}
#endif
