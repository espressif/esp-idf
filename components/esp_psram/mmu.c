/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * This file will be redesigned into MMU driver, to maintain all the external
 * memory contexts including:
 * - Flash
 * - PSRAM
 * - DDR
 *
 * Now only MMU-PSRAM related private APIs
 */

#include <stdint.h>
#include <sys/param.h>
#include "sdkconfig.h"
#include "esp_attr.h"
#include "esp_log.h"
#include "soc/ext_mem_defs.h"
#include "esp_private/mmu.h"

#if CONFIG_IDF_TARGET_ESP32S2
#include "soc/extmem_reg.h"
#include "esp32s2/rom/cache.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "soc/extmem_reg.h"
#include "esp32s3/rom/cache.h"
#endif


#define ALIGN_UP_BY(num, align) (((num) + ((align) - 1)) & ~((align) - 1))
__attribute__((unused)) static const char *TAG = "mmu";
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;


intptr_t mmu_get_psram_vaddr_start(void)
{
#if CONFIG_IDF_TARGET_ESP32S3

    intptr_t rodata_end_aligned = ALIGN_UP_BY((intptr_t)&_rodata_reserved_end, MMU_PAGE_SIZE);
    ESP_EARLY_LOGV(TAG, "rodata_end_aligned is 0x%x bytes", rodata_end_aligned);
    return rodata_end_aligned;

#elif CONFIG_IDF_TARGET_ESP32S2
    return DPORT_CACHE_ADDRESS_LOW;
#else   //CONFIG_IDF_TARGET_ESP32
    return DRAM1_CACHE_ADDRESS_LOW;
#endif
}

intptr_t mmu_get_psram_vaddr_end(void)
{
#if CONFIG_IDF_TARGET_ESP32S3
    return DRAM0_CACHE_ADDRESS_HIGH;
#elif CONFIG_IDF_TARGET_ESP32S2
    return DRAM0_CACHE_ADDRESS_HIGH;
#else   //CONFIG_IDF_TARGET_ESP32
    return DRAM1_CACHE_ADDRESS_HIGH;
#endif
}

//------------------------------------Copy Flash .text to PSRAM-------------------------------------//
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
static uint32_t instruction_in_spiram;
static uint32_t instr_start_page;
static uint32_t instr_end_page;
static int instr_flash2spiram_offs;

/**
 * - These logics are abstracted from the PSRAM driver
 * - These functions are only required by `flash_mmap.c` for converting paddr to vaddr, and vice versa
 * - The `flash_mmpa.c` will be rewritten into MMU driver
 *
 * Therefore, keep the APIs here for now
 */
void instruction_flash_page_info_init(uint32_t psram_start_physical_page)
{
#if CONFIG_IDF_TARGET_ESP32S2
    uint32_t instr_page_cnt = ((uint32_t)&_instruction_reserved_end - (uint32_t)&_instruction_reserved_start + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    uint32_t instr_mmu_offset = ((uint32_t)&_instruction_reserved_start & MMU_VADDR_MASK) / MMU_PAGE_SIZE;
    instr_start_page = ((volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS0_MMU_START))[instr_mmu_offset];
#elif CONFIG_IDF_TARGET_ESP32S3
    uint32_t instr_page_cnt = ((uint32_t)&_instruction_reserved_end - SOC_IROM_LOW + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    instr_start_page = *((volatile uint32_t *)(DR_REG_MMU_TABLE + CACHE_IROM_MMU_START));
#endif
    instr_start_page &= MMU_VALID_VAL_MASK;
    instr_end_page = instr_start_page + instr_page_cnt - 1;
    instr_flash2spiram_offs = instr_start_page - psram_start_physical_page;
    instruction_in_spiram = 1;
    ESP_DRAM_LOGV("mmu_psram", "Instructions from flash page%d copy to SPIRAM page%d, Offset: %d", instr_start_page, psram_start_physical_page, instr_flash2spiram_offs);
}

uint32_t esp_spiram_instruction_access_enabled(void)
{
    return instruction_in_spiram;
}

int instruction_flash2spiram_offset(void)
{
    return instr_flash2spiram_offs;
}

uint32_t instruction_flash_start_page_get(void)
{
    return instr_start_page;
}

uint32_t instruction_flash_end_page_get(void)
{
    return instr_end_page;
}
#endif  //CONFIG_SPIRAM_FETCH_INSTRUCTIONS


#if CONFIG_SPIRAM_RODATA
//------------------------------------Copy Flash .rodata to PSRAM-------------------------------------//
static uint32_t rodata_in_spiram;
static int rodata_flash2spiram_offs;
static uint32_t rodata_start_page;
static uint32_t rodata_end_page;

/**
 * - These logics are abstracted from the PSRAM driver
 * - These functions are only required by `flash_mmap.c` for converting paddr to vaddr, and vice versa
 * - The `flash_mmpa.c` will be rewritten into MMU driver
 *
 * Therefore, keep the APIs here for now
 */
void rodata_flash_page_info_init(uint32_t psram_start_physical_page)
{
#if CONFIG_IDF_TARGET_ESP32S2
    uint32_t rodata_page_cnt = ((uint32_t)&_rodata_reserved_end - (uint32_t)&_rodata_reserved_start + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    uint32_t rodata_mmu_offset = ((uint32_t)&_rodata_reserved_start & MMU_VADDR_MASK) / MMU_PAGE_SIZE;
    rodata_start_page = ((volatile uint32_t *)(DR_REG_MMU_TABLE + PRO_CACHE_IBUS2_MMU_START))[rodata_mmu_offset];
#elif CONFIG_IDF_TARGET_ESP32S3
    uint32_t rodata_page_cnt = ((uint32_t)&_rodata_reserved_end - ((uint32_t)&_rodata_reserved_start & ~ (MMU_PAGE_SIZE - 1)) + MMU_PAGE_SIZE - 1) / MMU_PAGE_SIZE;
    rodata_start_page = *(volatile uint32_t *)(DR_REG_MMU_TABLE + CACHE_DROM_MMU_START);
#endif
    rodata_start_page &= MMU_VALID_VAL_MASK;
    rodata_end_page = rodata_start_page + rodata_page_cnt - 1;
    rodata_flash2spiram_offs = rodata_start_page - psram_start_physical_page;
    rodata_in_spiram = 1;
    ESP_DRAM_LOGV("mmu_psram", "Rodata from flash page%d copy to SPIRAM page%d, Offset: %d", rodata_start_page, psram_start_physical_page, rodata_flash2spiram_offs);
}

uint32_t esp_spiram_rodata_access_enabled(void)
{
    return rodata_in_spiram;
}

int rodata_flash2spiram_offset(void)
{
    return rodata_flash2spiram_offs;
}

uint32_t rodata_flash_start_page_get(void)
{
    return rodata_start_page;
}

uint32_t rodata_flash_end_page_get(void)
{
    return rodata_end_page;
}
#endif  //#if CONFIG_SPIRAM_RODATA
