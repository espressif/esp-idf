/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file sleep_retention.h
 *
 * This file contains declarations of memory retention related functions in light sleeo mode.
 */

#if SOC_PM_SUPPORT_CPU_PD

/**
 * @brief Whether to allow the cpu power domain to be powered off.
 *
 * In light sleep mode, only when the system can provide enough memory
 * for cpu retention, the cpu power domain can be powered off.
 */
bool cpu_domain_pd_allowed(void);

#endif

#if SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD

/**
 * @brief Enable memory retention of some modules.
 *
 * In light sleep mode, before the system goes to sleep, enable the memory
 * retention of modules such as CPU and I/D-cache tag memory.
 */
void sleep_enable_memory_retention(void);

/**
 * @brief Disable memory retention of some modules.
 *
 * In light sleep mode, after the system exits sleep, disable the memory
 * retention of moudles such as CPU and I/D-cache tag memory.
 */
void sleep_disable_memory_retention(void);

#endif // SOC_PM_SUPPORT_CPU_PD || SOC_PM_SUPPORT_TAGMEM_PD

#ifdef __cplusplus
}
#endif
