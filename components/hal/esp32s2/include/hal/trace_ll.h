/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "esp_bit_defs.h"
#include "soc/dport_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void trace_ll_set_mem_block(int block)
{
    DPORT_WRITE_PERI_REG(DPORT_PMS_OCCUPY_3_REG, BIT(block-4));
}

#ifdef __cplusplus
}
#endif
