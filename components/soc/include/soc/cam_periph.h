/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_LCDCAM_CAM_SUPPORTED
typedef struct {
    struct {
        const periph_module_t module;
        const int irq_id;
        const int data_sigs[SOC_LCDCAM_CAM_DATA_WIDTH_MAX];
        const int hsync_sig;
        const int vsync_sig;
        const int pclk_sig;
        const int de_sig;
        const int clk_sig;
    } buses[SOC_LCDCAM_CAM_PERIPH_NUM];
} cam_signal_conn_t;

extern const cam_signal_conn_t cam_periph_signals;
#endif // SOC_LCDCAM_CAM_SUPPORTED

#ifdef __cplusplus
}
#endif
