/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "soc/soc_caps.h"
#include "hal/mspi_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    struct {
        const uint32_t irq;    //irq source for interrupt mux
    } instances[MSPI_LL_PERIPH_NUM];
} mspi_info_t;

extern const mspi_info_t mspi_hw_info;

#ifdef __cplusplus
}
#endif
