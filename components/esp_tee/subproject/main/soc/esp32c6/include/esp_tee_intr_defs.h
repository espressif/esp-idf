/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/interrupt_matrix_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

// LP_APM_M0_INTR, LP_APM_M1_INTR
#define TEE_SECURE_INT_APM_MASK_0  (0x00300000)
// HP_APM_M0_INTR, HP_APM_M1_INTR, HP_APM_M2_INTR, HP_APM_M3_INTR, LP_APM0_INTR
#define TEE_SECURE_INT_APM_MASK_1  (0x000000F8)

// LP_RTC_TIMER_INTR
#define TEE_SECURE_INT_MASK_0      (TEE_SECURE_INT_APM_MASK_0 | 0x00008000)
#if !CONFIG_SECURE_TEE_TEST_MODE
#define TEE_SECURE_INT_MASK_1      (TEE_SECURE_INT_APM_MASK_1)
#else
// + TG0_T0_INTR (only for test mode)
#define TEE_SECURE_INT_MASK_1      (TEE_SECURE_INT_APM_MASK_1 |  0x00080000)
#endif
// AES_INTR, SHA_INTR, ECC_INTR
#define TEE_SECURE_INT_MASK_2      (0x00001600)

#define INTMTX_STATUS_REG_0        (INTMTX_CORE0_INT_STATUS_REG_0_REG)
#define INTMTX_STATUS_REG_1        (INTMTX_CORE0_INT_STATUS_REG_1_REG)
#define INTMTX_STATUS_REG_2        (INTMTX_CORE0_INT_STATUS_REG_2_REG)

#ifdef __cplusplus
}
#endif
