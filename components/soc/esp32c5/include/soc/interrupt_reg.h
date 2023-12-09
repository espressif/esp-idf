/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/reg_base.h"
#include "soc/interrupt_matrix_reg.h"
#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

/**
 * ESP32C5 uses the CLIC controller as the interrupt controller (SOC_INT_CLIC_SUPPORTED = y)
 *
 * The memory map for interrupt registers is on a per-core basis, CLIC_INT_THRESH_REG points to
 * the current core interrupt register, whereas CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF points
 * to the other core registers, regardless of the core we are currently running on.
 */
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)
#define INTERRUPT_OTHER_CORE_INT_THRESH_REG     (CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF)

#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      INTERRUPT_CURRENT_CORE_INT_THRESH_REG
