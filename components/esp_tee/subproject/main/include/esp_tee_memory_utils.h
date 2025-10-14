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
    return (((intptr_t)p >= SOC_NS_IDRAM_START && (intptr_t)p < SOC_NS_IDRAM_END) ||
            ((intptr_t)p >= (size_t)esp_tee_app_config.ns_drom_start && (intptr_t)p < SOC_S_MMU_MMAP_RESV_START_VADDR) ||
            ((intptr_t)p >= SOC_RTC_DATA_LOW && (intptr_t)p < SOC_RTC_DATA_HIGH));
}

#ifdef __cplusplus
}
#endif
