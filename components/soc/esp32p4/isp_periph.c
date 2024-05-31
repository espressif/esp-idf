/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/periph_defs.h"
#include "soc/isp_periph.h"

#ifdef __cplusplus
extern "C" {
#endif

const isp_info_t isp_hw_info = {
    .instances = {
        [0] = {
            .irq = ETS_ISP_INTR_SOURCE,
        }
    },
    .dvp_ctlr = {
        [0] = {
            .dvp_pclk_sig = CAM_PCLK_PAD_IN_IDX,
            .dvp_hsync_sig = CAM_H_SYNC_PAD_IN_IDX,
            .dvp_vsync_sig = CAM_V_SYNC_PAD_IN_IDX,
            .dvp_de_sig = CAM_H_ENABLE_PAD_IN_IDX,
            .dvp_data_sig = {
                CAM_DATA_IN_PAD_IN0_IDX,
                CAM_DATA_IN_PAD_IN1_IDX,
                CAM_DATA_IN_PAD_IN2_IDX,
                CAM_DATA_IN_PAD_IN3_IDX,
                CAM_DATA_IN_PAD_IN4_IDX,
                CAM_DATA_IN_PAD_IN5_IDX,
                CAM_DATA_IN_PAD_IN6_IDX,
                CAM_DATA_IN_PAD_IN7_IDX,
                CAM_DATA_IN_PAD_IN8_IDX,
                CAM_DATA_IN_PAD_IN9_IDX,
                CAM_DATA_IN_PAD_IN10_IDX,
                CAM_DATA_IN_PAD_IN11_IDX,
                CAM_DATA_IN_PAD_IN12_IDX,
                CAM_DATA_IN_PAD_IN13_IDX,
                CAM_DATA_IN_PAD_IN14_IDX,
                CAM_DATA_IN_PAD_IN15_IDX
            },
        },
    }
};

#ifdef __cplusplus
}
#endif
