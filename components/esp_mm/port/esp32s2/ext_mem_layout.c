/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/ext_mem_defs.h"
#include "../ext_mem_layout.h"

/**
 * These regions is referring to linear address
 * The start addresses in this list should always be sorted from low to high, as MMU driver will need to
 * coalesce adjacent regions
 */
const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {
    [0] = {
        .start = SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW,
        .end = SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH,
        .size = BUS_SIZE(SOC_MMU_IRAM0_LINEAR),
        .bus_id = CACHE_BUS_IBUS0,
        .targets = MMU_TARGET_FLASH0 | MMU_TARGET_PSRAM0,
        .caps = MMU_MEM_CAP_EXEC | MMU_MEM_CAP_32BIT,
    },
    [1] = {
        .start = SOC_MMU_DROM0_LINEAR_ADDRESS_LOW,
        .end = SOC_MMU_DROM0_LINEAR_ADDRESS_HIGH,
        .size = BUS_SIZE(SOC_MMU_DROM0_LINEAR),
        .bus_id = CACHE_BUS_IBUS2,
        .targets = MMU_TARGET_FLASH0 | MMU_TARGET_PSRAM0,
        .caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT,
    },
    [2] = {
        .start = SOC_MMU_DPORT_LINEAR_ADDRESS_LOW,
        .end = SOC_MMU_DPORT_LINEAR_ADDRESS_HIGH,
        .size = BUS_SIZE(SOC_MMU_DPORT_LINEAR),
        .bus_id = CACHE_BUS_DBUS2,
        .targets = MMU_TARGET_FLASH0 | MMU_TARGET_PSRAM0,
        .caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT,
    },
    [3] = {
        .start = SOC_MMU_DRAM1_LINEAR_ADDRESS_LOW,
        .end = SOC_MMU_DRAM1_LINEAR_ADDRESS_HIGH,
        .size = BUS_SIZE(SOC_MMU_DRAM1_LINEAR),
        .bus_id = CACHE_BUS_DBUS1,
        .targets = MMU_TARGET_FLASH0 | MMU_TARGET_PSRAM0,
        .caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT,
    },
    [4] = {
        .start = SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW,
        .end = SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH,
        .size = BUS_SIZE(SOC_MMU_DRAM0_LINEAR),
        .bus_id = CACHE_BUS_DBUS0,
        .targets = MMU_TARGET_FLASH0 | MMU_TARGET_PSRAM0,
        .caps = MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT,
    },
};
