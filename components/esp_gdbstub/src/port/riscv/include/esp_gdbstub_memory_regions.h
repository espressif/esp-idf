/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_gdbstub_memory_regions_common.h"
#include "soc/soc.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline bool __is_valid_memory_region(intptr_t addr)
{
    return (addr >= SOC_DROM_LOW && addr < SOC_DROM_HIGH) ||
           (addr >= SOC_IROM_LOW && addr < SOC_IROM_HIGH) ||
           (addr >= SOC_IRAM_LOW && addr < SOC_IRAM_HIGH) ||
           (addr >= SOC_DRAM_LOW && addr < SOC_DRAM_HIGH) ||
           (addr >= SOC_IROM_MASK_LOW && addr < SOC_IROM_MASK_HIGH) ||
           (addr >= SOC_DROM_MASK_LOW && addr < SOC_DROM_MASK_HIGH) ||
#if defined(SOC_RTC_IRAM_LOW) && defined(SOC_RTC_IRAM_HIGH)
           (addr >= SOC_RTC_IRAM_LOW && addr < SOC_RTC_IRAM_HIGH) ||
           /* RTC DRAM and RTC DATA are identical with RTC IRAM, hence we skip them */
#endif
#if defined(SOC_DEBUG_LOW) && defined(SOC_DEBUG_HIGH)
           (addr >= SOC_DEBUG_LOW && addr < SOC_DEBUG_HIGH) ||
#endif
#if defined(SOC_CPU_SUBSYSTEM_LOW) && defined(SOC_CPU_SUBSYSTEM_HIGH)
           (addr >= SOC_CPU_SUBSYSTEM_LOW && addr < SOC_CPU_SUBSYSTEM_HIGH) ||
#endif
           (addr >= SOC_PERIPHERAL_LOW && addr < SOC_PERIPHERAL_HIGH);
}

static inline bool is_valid_memory_region(intptr_t addr)
{
    /* We shouldn't read transport registers since it will disturb the debugging.  */
    return (!is_transport_memory_region(addr)) && __is_valid_memory_region(addr);
}

#ifdef __cplusplus
}
#endif
