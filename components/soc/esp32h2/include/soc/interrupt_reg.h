/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/interrupt_matrix_reg.h"
#include "soc/plic_reg.h"
#include "soc/soc_caps.h"

#ifdef __has_include
#  if __has_include("sdkconfig.h")
#    include "sdkconfig.h"
#  endif
#endif

#if CONFIG_SECURE_ENABLE_TEE
#define INTERRUPT_PRIO_REG(n)                   (PLIC_UXINT0_PRI_REG + (n)*4)
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   PLIC_UXINT_THRESH_REG
#else
#define INTERRUPT_PRIO_REG(n)                   (PLIC_MXINT0_PRI_REG + (n)*4)
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   PLIC_MXINT_THRESH_REG
#endif

/**
 * ESP32H2 should use the PLIC controller as the interrupt controller instead of INTC (SOC_INT_PLIC_SUPPORTED = y)
 * Keep the following macros for backward compatibility reasons
 */
#if CONFIG_SECURE_ENABLE_TEE
#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG      PLIC_UXINT_ENABLE_REG
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      PLIC_UXINT_THRESH_REG
#define INTERRUPT_CORE0_CPU_INT_CLEAR_REG       PLIC_UXINT_CLEAR_REG
#define INTERRUPT_CORE0_CPU_INT_TYPE_REG        PLIC_UXINT_TYPE_REG
#define INTC_INT_PRIO_REG(n)                    (PLIC_UXINT0_PRI_REG + (n)*4)
#else
#define INTERRUPT_CORE0_CPU_INT_ENABLE_REG      PLIC_MXINT_ENABLE_REG
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      PLIC_MXINT_THRESH_REG
#define INTERRUPT_CORE0_CPU_INT_CLEAR_REG       PLIC_MXINT_CLEAR_REG
#define INTERRUPT_CORE0_CPU_INT_TYPE_REG        PLIC_MXINT_TYPE_REG
#define INTC_INT_PRIO_REG(n)                    (PLIC_MXINT0_PRI_REG + (n)*4)
#endif

#define DR_REG_INTERRUPT_BASE                   DR_REG_INTMTX_BASE
