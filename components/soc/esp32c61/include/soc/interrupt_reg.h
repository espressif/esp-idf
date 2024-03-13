/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc_caps.h"
#include "soc/clic_reg.h"

/**
 * ESP32C61 uses the CLIC controller as the interrupt controller (SOC_INT_CLIC_SUPPORTED = y)
 */

#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)

#define INTERRUPT_CORE0_CPU_INT_THRESH_REG INTERRUPT_CURRENT_CORE_INT_THRESH_REG

#define DR_REG_INTERRUPT_CORE0_BASE DR_REG_INTERRUPT_MATRIX_BASE
