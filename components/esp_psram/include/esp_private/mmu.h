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

#pragma once

#include <sys/param.h>
#include "esp_err.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if CONFIG_IDF_TARGET_ESP32
#define MMU_PAGE_SIZE                   0x8000
#else
#define MMU_PAGE_SIZE                   0x10000
#define MMU_PAGE_TO_BYTES(page_id)      ((page_id) * MMU_PAGE_SIZE)
#define BYTES_TO_MMU_PAGE(bytes)        ((bytes) / MMU_PAGE_SIZE)
#endif

/**
 * @brief Get the vaddr start for PSRAM
 *
 * @return PSRAM vaddr start address
 */
intptr_t mmu_get_psram_vaddr_start(void);

/**
 * @brief Get the vaddr end for PSRAM
 *
 * @return PSRAM vaddr end address
 */
intptr_t mmu_get_psram_vaddr_end(void);


#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
/**
 * @brief Copy Flash texts to PSRAM
 *
 * @param[in]  start_page    PSRAM physical start page
 * @param[in]  psram_size    PSRAM available size
 * @param[out] out_page      Used pages
 */
esp_err_t mmu_config_psram_text_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page);

/**
 * @brief Init other file requested MMU variables
 *
 * - These logics are abstracted from the PSRAM driver
 * - These functions are only required by `flash_mmap.c` for converting paddr to vaddr, and vice versa
 * - The `flash_mmpa.c` will be rewritten into MMU driver
 *
 * Therefore, keep the APIs here for now
 */
void instruction_flash_page_info_init(uint32_t psram_start_physical_page);

/**
 * @brief Get the start page number of the instruction in SPI flash
 *
 * @return start page number
 */
uint32_t instruction_flash_start_page_get(void);

/**
 * @brief Get the end page number of the instruction in SPI flash
 *
 * @return end page number
 */
uint32_t instruction_flash_end_page_get(void);

/**
 * @brief Get the offset of instruction from SPI flash to SPI RAM
 *
 * @return instruction offset
 */
int instruction_flash2spiram_offset(void);
#endif  // #if CONFIG_SPIRAM_FETCH_INSTRUCTIONS


#if CONFIG_SPIRAM_RODATA
/**
 * @brief Copy Flash rodata to PSRAM
 *
 * @param[in]  start_page    PSRAM physical start page
 * @param[in]  psram_size    PSRAM available size
 * @param[out] out_page      Used pages
 */
esp_err_t mmu_config_psram_rodata_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page);

/**
 * @brief Init other file requested MMU variables
 *
 * - These logics are abstracted from the PSRAM driver
 * - These functions are only required by `flash_mmap.c` for converting paddr to vaddr, and vice versa
 * - The `flash_mmpa.c` will be rewritten into MMU driver
 *
 * Therefore, keep the APIs here for now
 */
void rodata_flash_page_info_init(uint32_t psram_start_physical_page);

/**
 * @brief Get the start page number of the rodata in SPI flash
 *
 * @return start page number
 */
uint32_t rodata_flash_start_page_get(void);

/**
 * @brief Get the end page number of the rodata in SPI flash
 *
 * @return end page number
 */
uint32_t rodata_flash_end_page_get(void);

/**
 * @brief Get the offset number of rodata from SPI flash to SPI RAM
 *
 * @return rodata offset
 */
int rodata_flash2spiram_offset(void);
#endif  // #if CONFIG_SPIRAM_RODATA


#ifdef __cplusplus
}
#endif
