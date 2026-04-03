/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define TEST_INTR_NUM_PASS_IN_SEC  (31)

#include "soc/interrupt_core0_reg.h"

#define INTMTX_SEC_STATUS_REG             INTERRUPT_CORE0_INTR_SEC_STATUS_REG
#define INTMTX_SIG_IDX_ASSERT_IN_SEC_REG  INTERRUPT_CORE0_INTR_SIG_IDX_ASSERT_IN_SEC_REG

#ifndef __ASSEMBLER__
#include "soc/interrupts.h"
#include "soc/hp_system_reg.h"
#include "esp_bit_defs.h"

#define TG0_T0_INTR_SRC             (ETS_TG0_T0_INTR_SOURCE)
#define CPU_FROM_CPU_N_INTR_SRC(n)  (ETS_FROM_CPU_INTR0_SOURCE + n)
#define CPU_INTR_FROM_CPU_N_REG(n)  (HP_SYSTEM_CPU_INT_FROM_CPU_0_REG + 4 * (n))

#define INTMTX_INT_SRC_PASS_IN_SEC_BIT    BIT(6)

#endif
