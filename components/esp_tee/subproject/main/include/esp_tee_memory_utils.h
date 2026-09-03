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

/* TODO: Revisit these bounds for high-performance RISC-V SoCs (e.g. ESP32-P4,
 * ESP32-S31) with different memory maps than current ESP-TEE targets.
 */
FORCE_INLINE_ATTR bool esp_tee_buf_in_ree(const void *p, size_t len)
{
    uintptr_t start = (uintptr_t)p;

    /* Reject zero-length and overflow */
    if (len == 0 || len > (SIZE_MAX - start)) {
        return false;
    }

    uintptr_t end = start + len;
    return ((start >= SOC_NS_IDRAM_START && end <= SOC_NS_IDRAM_END) ||
            (start >= SOC_S_DROM_HIGH && end <= SOC_S_MMU_MMAP_RESV_START_VADDR)
#if SOC_RTC_MEM_SUPPORTED
            || (start >= SOC_RTC_DATA_LOW && end <= SOC_RTC_DATA_HIGH)
#endif
           );
}

FORCE_INLINE_ATTR bool esp_tee_ptr_in_ree(const void *p)
{
    return esp_tee_buf_in_ree(p, 4);
}

#ifdef __cplusplus
}
#endif
