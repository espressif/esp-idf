/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ISP_SUPPORTED
typedef struct {
    struct {
        const uint32_t irq;
    } instances[SOC_ISP_NUMS];
    struct {
        const uint32_t dvp_pclk_sig;
        const uint32_t dvp_hsync_sig;
        const uint32_t dvp_vsync_sig;
        const uint32_t dvp_de_sig;
        const uint32_t dvp_data_sig[SOC_ISP_DVP_DATA_WIDTH_MAX];
    } dvp_ctlr[SOC_ISP_DVP_CTLR_NUMS];
} isp_info_t;

extern const isp_info_t isp_hw_info;
#endif

#ifdef __cplusplus
}
#endif
