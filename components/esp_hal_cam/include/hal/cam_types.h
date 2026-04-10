/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
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

#if SOC_LCDCAM_CAM_SUPPORTED
typedef soc_periph_cam_clk_src_t cam_clock_source_t;        /*!< Clock source type of CAM */
#else
typedef int cam_clock_source_t;                             /*!< Default type */
#endif

#ifdef __cplusplus
}
#endif
