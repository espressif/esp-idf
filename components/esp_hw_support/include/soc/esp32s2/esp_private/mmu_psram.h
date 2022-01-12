/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <sys/param.h>
#include "esp_err.h"
#include "sdkconfig.h"


#ifdef __cplusplus
extern "C" {
#endif

esp_err_t mmu_map_psram(uint32_t start_paddr, uint32_t map_length, uint32_t *out_start_vaddr);

#if CONFIG_SPIRAM_FETCH_INSTRUCTIONS
extern int _instruction_reserved_start;
extern int _instruction_reserved_end;

esp_err_t mmu_config_psram_text_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page);

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
#endif

#if CONFIG_SPIRAM_RODATA
extern int _rodata_reserved_start;
extern int _rodata_reserved_end;

esp_err_t mmu_config_psram_rodata_segment(uint32_t start_page, uint32_t psram_size, uint32_t *out_page);

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
#endif


#ifdef __cplusplus
}
#endif
