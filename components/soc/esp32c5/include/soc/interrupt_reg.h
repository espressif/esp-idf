/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/interrupt_matrix_reg.h"
#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ESP32C5 uses the CLIC controller as the interrupt controller (SOC_INT_CLIC_SUPPORTED = y)
 */
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)

/* We only have a single core on the C5, CORE0 */
#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      INTERRUPT_CURRENT_CORE_INT_THRESH_REG

#define INTERRUPT_COREx_INTR_STATUS_REG_BASE(cpu)   INTERRUPT_CORE0_INT_STATUS_0_REG

#ifdef __cplusplus
}
#endif
