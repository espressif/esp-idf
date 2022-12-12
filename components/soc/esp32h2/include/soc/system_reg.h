/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/hp_system_reg.h"
#include "intpri_reg.h"

// ESP32H2-TODO : TODO: IDF-5720
#define SYSTEM_CPU_INTR_FROM_CPU_0_REG INTPRI_CPU_INTR_FROM_CPU_0_REG
#define SYSTEM_CPU_INTR_FROM_CPU_0 INTPRI_CPU_INTR_FROM_CPU_0
