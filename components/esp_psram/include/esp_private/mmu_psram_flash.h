/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @Backgrounds
 *
 * This file contains 2 parts:
 * 1. Feature: Copy Flash content to PSRAM. Related APIs are private:
 *    - mmu_config_psram_text_segment()
 *    - mmu_config_psram_rodata_segment()
 *
 * 2. Private APIs used by `flash_mmap.c` and `cache_utils.c`
 *    APIs in 2 are due to lack of MMU driver. There will be an MMU driver to maintain vaddr range.
 *    APIs in 2 will be refactored when MMU driver is ready
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
#define MMU_PAGE_SIZE                   CONFIG_MMU_PAGE_SIZE
#define MMU_PAGE_TO_BYTES(page_id)      ((page_id) * MMU_PAGE_SIZE)
#define BYTES_TO_MMU_PAGE(bytes)        ((bytes) / MMU_PAGE_SIZE)
#endif

/*----------------------------------------------------------------------------
                    Part 1 APIs (See @Backgrounds on top of this file)
-------------------------------------------------------------------------------*/
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
/**
 * @brief Copy Flash texts to PSRAM
 *
 * @param[in]  start_page    PSRAM physical start page
 * @param[in]  psram_size    PSRAM available size
 * @param[out] out_page      Used pages
 */
esp_err_t mmu_config_psram_text_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page);
#endif  //#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS

#if CONFIG_SPIRAM_RODATA
/**
 * @brief Copy Flash rodata to PSRAM
 *
 * @param[in]  start_page    PSRAM physical start page
 * @param[in]  psram_size    PSRAM available size
 * @param[out] out_page      Used pages
 */
esp_err_t mmu_config_psram_rodata_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page);
#endif  //#if CONFIG_SPIRAM_RODATA

/*----------------------------------------------------------------------------
                    Part 2 APIs (See @Backgrounds on top of this file)
-------------------------------------------------------------------------------*/
#if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM
/**
 * TODO: IDF-9049
 * @brief Vaddr to paddr, when XIP on PSRAM
 * @note This API only works for the original flash.text and flash.rodata, others vaddrs will return UINT32_MAX
 *
 * @param[in] ptr  Pointer
 *
 * @return Pointer corresponding physical addr
 */
size_t mmu_xip_psram_flash_vaddr_to_paddr(const void *ptr);
#else
#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
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
#endif  // #if CONFIG_SPIRAM_FLASH_LOAD_TO_PSRAM

#ifdef __cplusplus
}
#endif
