/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "sdkconfig.h"
#include "hal/cam_ctlr_types.h"
#include "esp_heap_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_DVP_CAM_FRAME_BITS_PER_PIXEL   16
#define TEST_DVP_CAM_EXPECTED_NEW_TRANS_MIN 6
#define TEST_DVP_CAM_EXPECTED_FINISHED_MIN  5
#define TEST_DVP_CAM_XCLK_FREQ_HZ           20000000
#define TEST_DVP_CAM_DATA_WIDTH             8

#if CONFIG_IDF_TARGET_ESP32S3
#define TEST_DVP_CAM_FRAME_WIDTH            640
#define TEST_DVP_CAM_FRAME_HEIGHT           480
#define TEST_DVP_CAM_FORMAT_NAME            "DVP_8bit_20Minput_YUV422_YUYV_640x480_6fps"
#define TEST_DVP_CAM_INPUT_COLOR_TYPE       CAM_CTLR_COLOR_YUV422_YUYV
#define TEST_DVP_CAM_OUTPUT_COLOR_TYPE      CAM_CTLR_COLOR_YUV422_YUYV

#define TEST_DVP_CAM_PWDN_IO            (-1)
#define TEST_DVP_CAM_RESET_IO           (-1)
#define TEST_DVP_CAM_SCCB_SCL_IO        5
#define TEST_DVP_CAM_SCCB_SDA_IO        4

#define TEST_DVP_CAM_D0_IO              11
#define TEST_DVP_CAM_D1_IO              9
#define TEST_DVP_CAM_D2_IO              8
#define TEST_DVP_CAM_D3_IO              10
#define TEST_DVP_CAM_D4_IO              12
#define TEST_DVP_CAM_D5_IO              18
#define TEST_DVP_CAM_D6_IO              17
#define TEST_DVP_CAM_D7_IO              16

#define TEST_DVP_CAM_XCLK_IO            15
#define TEST_DVP_CAM_PCLK_IO            13
#define TEST_DVP_CAM_DE_IO              7
#define TEST_DVP_CAM_VSYNC_IO           6

#elif CONFIG_IDF_TARGET_ESP32P4
#define TEST_DVP_CAM_FRAME_WIDTH            640
#define TEST_DVP_CAM_FRAME_HEIGHT           480
#define TEST_DVP_CAM_FORMAT_NAME            "DVP_8bit_20Minput_YUV422_UYVY_640x480_6fps"
#define TEST_DVP_CAM_INPUT_COLOR_TYPE       CAM_CTLR_COLOR_YUV422_UYVY
#define TEST_DVP_CAM_OUTPUT_COLOR_TYPE      CAM_CTLR_COLOR_YUV422_UYVY

#define TEST_DVP_CAM_PWDN_IO            (-1)
#define TEST_DVP_CAM_RESET_IO           (-1)

#if !CONFIG_ESP32P4_SELECTS_REV_LESS_V3
#define TEST_DVP_CAM_SCCB_SCL_IO        8
#define TEST_DVP_CAM_SCCB_SDA_IO        7

#define TEST_DVP_CAM_D0_IO              2
#define TEST_DVP_CAM_D1_IO              32
#define TEST_DVP_CAM_D2_IO              33
#define TEST_DVP_CAM_D3_IO              23
#define TEST_DVP_CAM_D4_IO              3
#define TEST_DVP_CAM_D5_IO              6
#define TEST_DVP_CAM_D6_IO              5
#define TEST_DVP_CAM_D7_IO              21

#define TEST_DVP_CAM_XCLK_IO            20
#define TEST_DVP_CAM_PCLK_IO            4
#define TEST_DVP_CAM_DE_IO              22
#define TEST_DVP_CAM_VSYNC_IO           37
#else
#define TEST_DVP_CAM_SCCB_SCL_IO        33
#define TEST_DVP_CAM_SCCB_SDA_IO        32

#define TEST_DVP_CAM_D0_IO              53
#define TEST_DVP_CAM_D1_IO              54
#define TEST_DVP_CAM_D2_IO              26
#define TEST_DVP_CAM_D3_IO              1
#define TEST_DVP_CAM_D4_IO              0
#define TEST_DVP_CAM_D5_IO              45
#define TEST_DVP_CAM_D6_IO              46
#define TEST_DVP_CAM_D7_IO              47

#define TEST_DVP_CAM_XCLK_IO            20
#define TEST_DVP_CAM_PCLK_IO            21
#define TEST_DVP_CAM_DE_IO              22
#define TEST_DVP_CAM_VSYNC_IO           23
#endif

#elif CONFIG_IDF_TARGET_ESP32S31

#define TEST_DVP_CAM_FRAME_WIDTH            640
#define TEST_DVP_CAM_FRAME_HEIGHT           480
#define TEST_DVP_CAM_FORMAT_NAME            "DVP_8bit_20Minput_YUV422_YUYV_640x480_6fps"
#define TEST_DVP_CAM_INPUT_COLOR_TYPE       CAM_CTLR_COLOR_YUV422_YUYV
#define TEST_DVP_CAM_OUTPUT_COLOR_TYPE      CAM_CTLR_COLOR_YUV422_YUYV

#define TEST_DVP_CAM_PWDN_IO            61
#define TEST_DVP_CAM_RESET_IO           60
#define TEST_DVP_CAM_SCCB_SCL_IO        1
#define TEST_DVP_CAM_SCCB_SDA_IO        0

#define TEST_DVP_CAM_D0_IO              46
#define TEST_DVP_CAM_D1_IO              47
#define TEST_DVP_CAM_D2_IO              48
#define TEST_DVP_CAM_D3_IO              49
#define TEST_DVP_CAM_D4_IO              50
#define TEST_DVP_CAM_D5_IO              51
#define TEST_DVP_CAM_D6_IO              52
#define TEST_DVP_CAM_D7_IO              53

#define TEST_DVP_CAM_XCLK_IO            55
#define TEST_DVP_CAM_PCLK_IO            54
#define TEST_DVP_CAM_DE_IO              57
#define TEST_DVP_CAM_VSYNC_IO           56

#endif

#define TEST_DVP_CAM_FRAME_SIZE_BYTES \
    (TEST_DVP_CAM_FRAME_WIDTH * TEST_DVP_CAM_FRAME_HEIGHT * TEST_DVP_CAM_FRAME_BITS_PER_PIXEL / 8)

#if CONFIG_SPIRAM
#define TEST_DVP_CAM_BUF_ALLOC_CAPS     (MALLOC_CAP_SPIRAM | MALLOC_CAP_DMA)
#else
#define TEST_DVP_CAM_BUF_ALLOC_CAPS     (MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)
#endif

#ifdef __cplusplus
}
#endif
