/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdlib.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "soc/ext_mem_defs.h"
#include "ext_mem_layout.h"
#include "mmu.h"


#if CONFIG_IDF_TARGET_ESP32
/**
 * These regions is referring to linear address
 * The start addresses in this list should always be sorted from low to high, as MMU driver will need to
 * coalesce adjacent regions
 */
const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {

    /*linear start                     linear end                         bus size                        bus ID,          bus capabilities */

    //Can be used for text
    {SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW, SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_IRAM0_LINEAR), CACHE_BUS_IBUS0, MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT},
    //Can be used for text
    {SOC_MMU_IRAM1_LINEAR_ADDRESS_LOW, SOC_MMU_IRAM1_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_IRAM1_LINEAR), CACHE_BUS_IBUS1, MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT},
    //Can be used for text
    {SOC_MMU_IROM0_LINEAR_ADDRESS_LOW, SOC_MMU_IROM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_IROM0_LINEAR), CACHE_BUS_IBUS2, MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT},
    //Can be used for rodata
    {SOC_MMU_DROM0_LINEAR_ADDRESS_LOW, SOC_MMU_DROM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_DROM0_LINEAR), CACHE_BUS_DBUS0, MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT},
    //Can be used for PSRAM
    {SOC_MMU_DRAM1_LINEAR_ADDRESS_LOW, SOC_MMU_DRAM1_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_DRAM1_LINEAR), CACHE_BUS_DBUS1, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT},
};


#elif CONFIG_IDF_TARGET_ESP32S2
/**
 * These regions is referring to linear address
 * The start addresses in this list should always be sorted from low to high, as MMU driver will need to
 * coalesce adjacent regions
 */
const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {

    /*linear start                     linear end                         bus size                        bus ID,          bus capabilities */

    //Can be used for text
    {SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW, SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_IRAM0_LINEAR), CACHE_BUS_IBUS0, MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT},
    //Can be used for text
    {SOC_MMU_IRAM1_LINEAR_ADDRESS_LOW, SOC_MMU_IRAM1_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_IRAM1_LINEAR), CACHE_BUS_IBUS1, MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT},
    //Can be used for Flash rodata, connected by IBUS
    {SOC_MMU_DROM0_LINEAR_ADDRESS_LOW, SOC_MMU_DROM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_DROM0_LINEAR), CACHE_BUS_IBUS2, MMU_MEM_CAP_READ | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT},
    //Can be used for PSRAM
    {SOC_MMU_DPORT_LINEAR_ADDRESS_LOW, SOC_MMU_DPORT_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_DPORT_LINEAR), CACHE_BUS_DBUS2, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT},
    //Can be used for PSRAM
    {SOC_MMU_DRAM1_LINEAR_ADDRESS_LOW, SOC_MMU_DRAM1_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_DRAM1_LINEAR), CACHE_BUS_DBUS1, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT},
    //Can be used for PSRAM
    {SOC_MMU_DRAM0_LINEAR_ADDRESS_LOW, SOC_MMU_DRAM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_DRAM0_LINEAR), CACHE_BUS_DBUS0, MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT},
};


#elif CONFIG_IDF_TARGET_ESP32S3
/**
 * The start addresses in this list should always be sorted from low to high, as MMU driver will need to
 * coalesce adjacent regions
 */
const mmu_mem_region_t g_mmu_mem_regions[SOC_MMU_LINEAR_ADDRESS_REGION_NUM] = {

    /*linear start                     linear end                         bus size                        bus ID,                            bus capabilities */

    /**
     * Can be used for Flash text, rodata, and PSRAM
     * IRAM0 linear address should be always the same as DRAM0 linear address
     */
    {SOC_MMU_IRAM0_LINEAR_ADDRESS_LOW, SOC_MMU_IRAM0_LINEAR_ADDRESS_HIGH, BUS_SIZE(SOC_MMU_IRAM0_LINEAR), CACHE_BUS_IBUS0 | CACHE_BUS_DBUS0, MMU_MEM_CAP_EXEC | MMU_MEM_CAP_READ | MMU_MEM_CAP_WRITE | MMU_MEM_CAP_32BIT | MMU_MEM_CAP_8BIT},
};
#endif
