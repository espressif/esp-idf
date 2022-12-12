/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "hal/cache_types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    intptr_t start;
    intptr_t end;
    size_t size;
    cache_bus_mask_t bus_id;
    uint32_t caps;
} mmu_mem_region_t;

//These regions is referring to linear address
extern const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM];

#ifdef __cplusplus
}
#endif
