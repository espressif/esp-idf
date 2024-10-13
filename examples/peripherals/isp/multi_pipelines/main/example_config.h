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

#define EXAMPLE_DW9714_DEV_ADDR                 0xC
#ifdef __cplusplus
}
#endif
