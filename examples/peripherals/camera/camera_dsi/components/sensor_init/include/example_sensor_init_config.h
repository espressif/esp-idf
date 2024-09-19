/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_CAM_SCCB_FREQ                   (100000)
#define EXAMPLE_CAM_SCCB_SCL_IO                 (8)
#define EXAMPLE_CAM_SCCB_SDA_IO                 (7)

#if CONFIG_EXAMPLE_MIPI_CSI_HRES_800
#if CONFIG_EXAMPLE_MIPI_CSI_VRES_640
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_800x640_50fps"
#else
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_800x1280_50fps"
#endif
#endif

#if CONFIG_EXAMPLE_MIPI_CSI_HRES_1024
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_1024x600_30fps"
#endif

#ifdef __cplusplus
}
#endif
