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
#define EXAMPLE_MIPI_SCCB_SCL_IO                (34)
#define EXAMPLE_MIPI_SCCB_SDA_IO                (31)
#define EXAMPLE_MIPI_CSI_DISP_HSIZE             800
#define EXAMPLE_MIPI_CSI_DISP_VSIZE_640P        640
#define EXAMPLE_MIPI_CSI_DISP_VSIZE_1280P       1280
#define EXAMPLE_MIPI_CSI_640P                   1
#define EXAMPLE_MIPI_IDI_CLOCK_RATE             (50000000)
#define EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS      200 //line_rate = pclk * 4

#define EXAMPLE_MIPI_DSI_IMAGE_HSIZE            800
#define EXAMPLE_MIPI_DSI_IMAGE_VSIZE            1280
#define EXAMPLE_MIPI_DSI_IMAGE_HSYNC            40
#define EXAMPLE_MIPI_DSI_IMAGE_HBP              140
#define EXAMPLE_MIPI_DSI_IMAGE_HFP              40
#define EXAMPLE_MIPI_DSI_IMAGE_VSYNC            4
#define EXAMPLE_MIPI_DSI_IMAGE_VBP              16
#define EXAMPLE_MIPI_DSI_IMAGE_VFP              16

#define EXAMPLE_OV5647_DEV_ADDR                 0x36
#ifdef __cplusplus
}
#endif
