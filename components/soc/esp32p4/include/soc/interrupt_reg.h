/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ESP32P4 uses the CLIC controller as the interrupt controller (SOC_INT_CLIC_SUPPORTED = y)
 *
 * The memory map for interrupt registers is on a per-core basis, CLIC_INT_THRESH_REG points to
 * the current core interrupt register, whereas CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF points
 * to the other core registers, regardless of the core we are currently running on.
 */
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)
#define INTERRUPT_OTHER_CORE_INT_THRESH_REG     (CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF)

#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      (rv_utils_get_core_id() == 0 ? INTERRUPT_CURRENT_CORE_INT_THRESH_REG : INTERRUPT_OTHER_CORE_INT_THRESH_REG)
#define INTERRUPT_CORE1_CPU_INT_THRESH_REG      (rv_utils_get_core_id() == 1 ? INTERRUPT_CURRENT_CORE_INT_THRESH_REG : INTERRUPT_OTHER_CORE_INT_THRESH_REG)


#ifdef __cplusplus
}
#endif
