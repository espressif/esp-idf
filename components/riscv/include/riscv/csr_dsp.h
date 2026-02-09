/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "soc/soc_caps.h"

#if SOC_CPU_HAS_DSP

#define CSR_DSP_XACC_L         0x806
#define CSR_DSP_XACC_H         0x807
#define CSR_DSP_SAR            0x809
#define CSR_DSP_STATUS         0x80a

/**
 * CSR lowest 2 bits describe the following states:
 * 00: Off
 * 01: Initial
 * 10: Clean
 * 11: Dirty
 */
#define CSR_DSP_STATE_REG      0x7f3

#endif /* SOC_CPU_HAS_DSP */
