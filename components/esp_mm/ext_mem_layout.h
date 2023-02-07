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
#include "hal/mmu_types.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    uint32_t start;           //laddr start
    uint32_t end;             //laddr end
    size_t size;              //region size
    cache_bus_mask_t bus_id;  //bus_id mask, for accessible cache buses
    mmu_target_t targets;     //region supported physical targets
    uint32_t caps;            //vaddr capabilities
} mmu_mem_region_t;

//These regions is referring to linear address
extern const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM];

#ifdef __cplusplus
}
#endif
