/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#define TEST_INTR_NUM_PASS_IN_SEC  (31)

#ifndef __ASSEMBLER__
#include "soc/interrupts.h"

#define TG0_T0_INTR_SRC             (ETS_TG0_T0_LEVEL_INTR_SOURCE)
#define CPU_FROM_CPU_N_INTR_SRC(n)  (ETS_FROM_CPU_INTR0_SOURCE + n)
#endif
