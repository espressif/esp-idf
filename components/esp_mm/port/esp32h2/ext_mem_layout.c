/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/ext_mem_defs.h"
#include "../ext_mem_layout.h"
#include "hal/mmu_types.h"

/* NOTE: With ESP-TEE enabled:
 * - The start address is moved by the size of TEE IDROM segments since these
 *   segments are placed at the start of the linear address space
 * - TEE IROM and DROM segments are both 64KB (CONFIG_SECURE_TEE_IROM_SIZE,
 *   CONFIG_SECURE_TEE_DROM_SIZE) for now. Thus, the number of reserved entries
 *   from the start would be (64KB + 64KB)/MMU_PAGE_SIZE
 * - The last few MMU entries are reserved for TEE flash operations. The number
 *   of reserved entries matches the size of TEE IDROM segments (IROM + DROM)
 *   plus one additional entry, i.e. (64KB + 64KB)/MMU_PAGE_SIZE + 1
 */
#if CONFIG_SECURE_ENABLE_TEE
#define TEE_MMU_MEM_REG_START_OFFS   (CONFIG_SECURE_TEE_IROM_SIZE + CONFIG_SECURE_TEE_DROM_SIZE)
#define TEE_MMU_RESV_PAGES           ((CONFIG_SECURE_TEE_IROM_SIZE + CONFIG_SECURE_TEE_DROM_SIZE) / CONFIG_MMU_PAGE_SIZE)
#define TEE_MMU_MEM_REG_END_OFFS     ((TEE_MMU_RESV_PAGES + 1) * CONFIG_MMU_PAGE_SIZE)

#define MMU_MEM_REG_START_ADDR_W_TEE (SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW + TEE_MMU_MEM_REG_START_OFFS)
#define MMU_MEM_REG_END_ADDR_W_TEE   (SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH - TEE_MMU_MEM_REG_END_OFFS)

#define MMU_IRAM0_LINEAR_ADDRESS_LOW    MMU_MEM_REG_START_ADDR_W_TEE
#define MMU_IRAM0_LINEAR_ADDRESS_HIGH   MMU_MEM_REG_END_ADDR_W_TEE
#else
#define MMU_IRAM0_LINEAR_ADDRESS_LOW    SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW
#define MMU_IRAM0_LINEAR_ADDRESS_HIGH   SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH
#endif

/**
 * The start addresses in this list should always be sorted from low to high, as MMU driver will need to
 * coalesce adjacent regions
 */
const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {
    [0] = {
        .start = MMU_IRAM0_LINEAR_ADDRESS_LOW,
        .end = MMU_IRAM0_LINEAR_ADDRESS_HIGH,
        .size = MMU_IRAM0_LINEAR_ADDRESS_HIGH - MMU_IRAM0_LINEAR_ADDRESS_LOW,
        .bus_id = CACHE_BUS_IBUS0 | CACHE_BUS_DBUS0,
        .targets = MMU_TARGET_FLASH0,
        .caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT,
    },
};
