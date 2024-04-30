/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#if SOC_CPU_HAS_HWLOOP

/* CSR 0x7F1 lowest 2 bits describe the following states:
 * 00: OFF
 * 01: Initial
 * 10: Clean
 * 11: Dirty
 */
#define CSR_HWLP_STATE_REG      0x7F1

#define CSR_LOOP0_START_ADDR    0x7C6
#define CSR_LOOP0_END_ADDR      0x7C7
#define CSR_LOOP0_COUNT         0x7C8
#define CSR_LOOP1_START_ADDR    0x7C9
#define CSR_LOOP1_END_ADDR      0x7CA
#define CSR_LOOP1_COUNT         0x7CB

#endif /* SOC_CPU_HAS_HWLOOP */
