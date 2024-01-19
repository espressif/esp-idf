/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include "soc/clic_reg.h"
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * ESP32C5 uses the CLIC controller as the interrupt controller (SOC_INT_CLIC_SUPPORTED = y)
 */
#define INTERRUPT_CURRENT_CORE_INT_THRESH_REG   (CLIC_INT_THRESH_REG)
#define INTERRUPT_OTHER_CORE_INT_THRESH_REG     (CLIC_INT_THRESH_REG + DUALCORE_CLIC_CTRL_OFF)

#define INTERRUPT_CORE0_CPU_INT_THRESH_REG      INTERRUPT_CURRENT_CORE_INT_THRESH_REG

#ifdef __cplusplus
}
#endif
