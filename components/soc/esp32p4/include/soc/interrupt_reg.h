/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

// ESP32P4 uses the CLIC controller as the interrupt controller (SOC_INT_CLIC_SUPPORTED = y)
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG CLIC_INT_THRESH_REG
#define INTERRUPT_CORE1_CPU_INT_THRESH_REG CLIC_INT_THRESH_REG
