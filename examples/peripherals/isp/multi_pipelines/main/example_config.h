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
<<<<<<< HEAD:examples/peripherals/isp/auto_focus/main/example_config.h
#define EXAMPLE_MIPI_SCCB_SCL_IO                (8)
#define EXAMPLE_MIPI_SCCB_SDA_IO                (7)
#define EXAMPLE_MIPI_IDI_CLOCK_RATE             (50000000)
#define EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS      200 //line_rate = pclk * 4

#define EXAMPLE_OV5647_DEV_ADDR                 0x36
=======
#define EXAMPLE_MIPI_CSI_LANE_BITRATE_MBPS      200 //line_rate = pclk * 4

>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade:examples/peripherals/isp/multi_pipelines/main/example_config.h
#define EXAMPLE_DW9714_DEV_ADDR                 0xC
#ifdef __cplusplus
}
#endif
