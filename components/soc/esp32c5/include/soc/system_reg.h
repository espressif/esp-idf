/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/hp_system_reg.h"

// TODO: IDF-5720
#include "soc/intpri_reg.h"
#define SYSTEM_CPU_INTR_FROM_CPU_0_REG INTPRI_CPU_INTR_FROM_CPU_0_REG
#define SYSTEM_CPU_INTR_FROM_CPU_0 INTPRI_CPU_INTR_FROM_CPU_0
