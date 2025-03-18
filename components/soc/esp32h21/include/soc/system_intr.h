/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/interrupts.h"

// Maps misc system interrupt to hardware interrupt names
#define SYS_CPU_INTR_FROM_CPU_0_SOURCE  ETS_CPU_INTR_FROM_CPU_0_SOURCE
#define SYS_CPU_INTR_FROM_CPU_1_SOURCE  ETS_CPU_INTR_FROM_CPU_1_SOURCE
#define SYS_CPU_INTR_FROM_CPU_2_SOURCE  ETS_CPU_INTR_FROM_CPU_2_SOURCE
#define SYS_CPU_INTR_FROM_CPU_3_SOURCE  ETS_CPU_INTR_FROM_CPU_3_SOURCE

#define SYS_TG0_WDT_INTR_SOURCE         ETS_TG0_WDT_INTR_SOURCE
#define SYS_TG1_WDT_INTR_SOURCE         ETS_TG1_WDT_INTR_SOURCE
