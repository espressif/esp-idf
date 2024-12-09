/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define EXAMPLE_RGB565_BITS_PER_PIXEL           16
#define EXAMPLE_MIPI_SCCB_FREQ                  (100000)
#define EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS      200 //line_rate = pclk * 4

#define EXAMPLE_MIPI_CSI_CAM_SCCB_SCL_IO        (8)
#define EXAMPLE_MIPI_CSI_CAM_SCCB_SDA_IO        (7)

#if CONFIG_EXAMPLE_MIPI_CSI_HRES_800

#if CONFIG_EXAMPLE_MIPI_CSI_VRES_640
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_800x640_50fps"
#elif CONFIG_EXAMPLE_MIPI_CSI_VRES_800
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_800x800_50fps"
#elif CONFIG_EXAMPLE_MIPI_CSI_VRES_1280
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_800x1280_50fps"
#endif

#elif CONFIG_EXAMPLE_MIPI_CSI_HRES_1024

#if CONFIG_EXAMPLE_MIPI_CSI_VRES_600
#define EXAMPLE_CAM_FORMAT                     "MIPI_2lane_24Minput_RAW8_1024x600_30fps"
#endif

#endif
#define EXAMPLE_DW9714_DEV_ADDR                 0xC
#ifdef __cplusplus
}
#endif
