/*
 * SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_INTERRUPT_REG_H_
#define _SOC_INTERRUPT_REG_H_

#include "soc/interrupt_core0_reg.h"
#include "soc/interrupt_core1_reg.h"
#include "soc.h"

#if (!CONFIG_FREERTOS_UNICORE)
#define INTERRUPT_COREx_INTR_STATUS_REG_BASE(cpu)   (cpu == 0? INTERRUPT_CORE0_INTR_STATUS_0_REG : INTERRUPT_CORE1_INTR_STATUS_0_REG)
#else
#define INTERRUPT_COREx_INTR_STATUS_REG_BASE(cpu)   INTERRUPT_CORE0_INTR_STATUS_0_REG
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /*_SOC_INTERRUPT_REG_H_ */
