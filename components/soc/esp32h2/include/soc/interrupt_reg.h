/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "interrupt_matrix_reg.h"
#include "plic_reg.h"
#include "soc/soc_caps.h"

#define INTERRUPT_PRIO_REG(n)                   (PLIC_MXINT0_PRI_REG + (n)*4)
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   PLIC_MXINT_THRESH_REG

/**
 * ESP32H2 should use the PLIC controller as the interrupt controller instead of INTC (SOC_INT_PLIC_SUPPORTED = y)
 * Keep the following macros for backward compatibility reasons
 */
#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG      PLIC_MXINT_ENABLE_REG
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      PLIC_MXINT_THRESH_REG
#define INTERRUPT_CORE0_CPU_INT_CLEAR_REG       PLIC_MXINT_CLEAR_REG
#define INTERRUPT_CORE0_CPU_INT_TYPE_REG        PLIC_MXINT_TYPE_REG
#define INTC_INT_PRIO_REG(n)                    (PLIC_MXINT0_PRI_REG + (n)*4)
#define DR_REG_INTERRUPT_BASE                   DR_REG_INTERRUPT_MATRIX_BASE
