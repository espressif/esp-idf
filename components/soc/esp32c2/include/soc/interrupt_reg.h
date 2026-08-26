/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include "soc/interrupt_core0_reg.h"

#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   INTERRUPT_CORE0_CPU_INT_THRESH_REG
#define INTERRUPT_PRIO_REG(n)                   (INTERRUPT_CORE0_CPU_INT_PRI_0_REG + (n)*4)
