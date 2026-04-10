/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"
#include "soc/gpio_sig_map.h"
#if SOC_HAS(ISP)
#include "hal/isp_ll.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ISP_SUPPORTED
typedef struct {
    struct {
        const uint32_t irq;
    } instances[ISP_LL_PERIPH_NUMS];
    struct {
        const uint32_t dvp_pclk_sig;
        const uint32_t dvp_hsync_sig;
        const uint32_t dvp_vsync_sig;
        const uint32_t dvp_de_sig;
        const uint32_t dvp_data_sig[ISP_LL_DVP_DATA_WIDTH_MAX];
    } dvp_ctlr[ISP_LL_DVP_CTLR_NUMS];
} isp_info_t;

extern const isp_info_t isp_hw_info;
#endif

#ifdef __cplusplus
}
#endif
