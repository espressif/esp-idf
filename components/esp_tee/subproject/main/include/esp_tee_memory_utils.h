/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/ext_mem_defs.h"
#include "esp_tee.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR bool esp_tee_ptr_in_ree(const void *p)
{
    intptr_t addr = (intptr_t)p;
    return (
               (addr >= SOC_NS_IDRAM_START && addr < SOC_NS_IDRAM_END) ||
               (addr >= (intptr_t)esp_tee_app_config.ns_drom_start &&
                addr < SOC_S_MMU_MMAP_RESV_START_VADDR)
#if SOC_RTC_MEM_SUPPORTED
               || (addr >= SOC_RTC_DATA_LOW && addr < SOC_RTC_DATA_HIGH)
#endif
           );
}

#ifdef __cplusplus
}
#endif
