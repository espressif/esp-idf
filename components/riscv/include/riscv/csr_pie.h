/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#if SOC_CPU_HAS_PIE

/* CSR lowest 2 bits describe the following states:
 * 00: OFF
 * 01: Initial
 * 10: Clean
 * 11: Dirty
 */
#define CSR_PIE_STATE_REG      0x7F2

#endif /* SOC_CPU_HAS_PIE */
