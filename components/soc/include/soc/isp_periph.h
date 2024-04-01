/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "soc/periph_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_ISP_SUPPORTED
typedef struct {
    struct {
        const uint32_t irq;
    } instances[SOC_ISP_NUMS];
} isp_info_t;

extern const isp_info_t isp_hw_info;
#endif

#ifdef __cplusplus
}
#endif
