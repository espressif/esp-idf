/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include "esp_attr.h"
#include "soc/soc.h"
#include "soc/ext_mem_defs.h"
#include "esp_tee.h"

#ifdef __cplusplus
extern "C" {
#endif

FORCE_INLINE_ATTR bool esp_tee_ptr_in_ree(const void *p)
{
    uintptr_t addr = (uintptr_t)p;
    return (
               (addr >= SOC_NS_IDRAM_START && addr < SOC_NS_IDRAM_END) ||
               (addr >= (uintptr_t)esp_tee_app_config.ns_drom_start &&
                addr < SOC_S_MMU_MMAP_RESV_START_VADDR)
#if SOC_RTC_MEM_SUPPORTED
               || (addr >= SOC_RTC_DATA_LOW && addr < SOC_RTC_DATA_HIGH)
#endif
           );
}

FORCE_INLINE_ATTR bool esp_tee_buf_in_ree(const void *p, size_t len)
{
    uintptr_t start = (uintptr_t)p;

    /* Reject zero-length and overflow */
    if (len == 0 || len > (SIZE_MAX - start)) {
        return false;
    }

    return esp_tee_ptr_in_ree(p) &&
           esp_tee_ptr_in_ree((const char *)p + len - 1);
}

#ifdef __cplusplus
}
#endif
