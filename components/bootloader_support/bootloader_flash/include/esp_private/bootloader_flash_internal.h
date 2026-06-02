/*
 * SPDX-FileCopyrightText: 2020-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "esp_err.h"
#include "sdkconfig.h"
#include "bootloader_flash.h"
#include "bootloader_flash_override.h"
#include "soc/soc_caps.h"
#include "soc/ext_mem_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------- Utilities -------------------------------- //
/**
 * This file provides a Flash API for bootloader_support code,
 * that can be used from bootloader or app code.
 * This header is available to source code in the ESP-IDF components only.
 */

#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_BLOCK_SIZE 	0x10000

#define MMAP_ALIGNED_MASK 	(CONFIG_MMU_PAGE_SIZE - 1)
#define MMU_FLASH_MASK    (~(CONFIG_MMU_PAGE_SIZE - 1))
#define MMU_FLASH_MASK_FROM_VAL(PAGE_SZ) (~((PAGE_SZ) - 1))
#define MMU_DROM_END_ENTRY_VADDR_FROM_VAL(PAGE_SZ) (SOC_DRAM_FLASH_ADDRESS_HIGH - (PAGE_SZ))

/**
 * MMU mapping must always be in the unit of a CONFIG_MMU_PAGE_SIZE
 * This macro is a helper for you to get needed page nums to be mapped. e.g.:
 * Let's say CONFIG_MMU_PAGE_SIZE is 64KB.
 * - v_start = 0x4200_0004
 * - size = 4 * 64KB
 *
 * You should map from 0x4200_0000, then map 5 pages.
 */
#define GET_REQUIRED_MMU_PAGES(size, v_start)    ((size + (v_start - (v_start & MMU_FLASH_MASK)) + CONFIG_MMU_PAGE_SIZE - 1) / CONFIG_MMU_PAGE_SIZE)

/**
 * @brief Get number of free pages
 *
 * @return Number of free pages
 */
uint32_t bootloader_mmap_get_free_pages(void);

/**
 * @brief Map a region of flash to data memory
 *
 * @important In bootloader code, only one region can be bootloader_mmaped at once. The previous region must be bootloader_munmapped before another region is mapped.
 *
 * @important In app code, these functions are not thread safe.
 *
 * Call bootloader_munmap once for each successful call to bootloader_mmap.
 *
 * In esp-idf app, this function maps directly to spi_flash_mmap with the @ref
 * spi_flash_mmap_flag_t::SPI_FLASH_MMAP_FLAG_BLOCKS_WRITE flag set. When XIP on PSRAM (`CONFIG_SPIRAM_XIP_FROM_PSRAM`)  enabled, flash erasing/writing
 * will be blocked until unmap.
 *
 * @param offset - Starting flash offset to map to memory.
 * @param length - Length of data to map.
 *
 * @return Pointer to mapped data memory (at src_addr), or NULL
 * if an allocation error occurred.
 */
const void *bootloader_mmap(uint32_t src_addr, uint32_t size);


/**
 * @brief Unmap a previously mapped region of flash
 *
 * Call bootloader_munmap once for each successful call to bootloader_mmap.
 */
void bootloader_munmap(const void *mapping);

/**
 * @brief  Read data from Flash.
 *
 *
 * @note All of src, dest and size have to be 4-byte aligned.
 *
 * @param  src   source address of the data in Flash.
 * @param  dest  pointer to the destination buffer
 * @param  size  length of data
 * @param  allow_decrypt If true and flash encryption is enabled, data on flash
 *         will be decrypted transparently as part of the read.
 *
 * @return ESP_OK on success, ESP_ERR_FLASH_OP_FAIL on SPI failure,
 * ESP_ERR_FLASH_OP_TIMEOUT on SPI timeout.
 */
esp_err_t bootloader_flash_read(size_t src_addr, void *dest, size_t size, bool allow_decrypt);


/**
 * @brief  Write data to Flash.
 *
 * @note All of dest_addr, src and size have to be 4-byte aligned. If write_encrypted is set, dest_addr and size must be 32-byte aligned.
 *
 * @note In bootloader, when write_encrypted == true, the src buffer is encrypted in place.
 *
 * @note [ESP-TEE] Using this API from the TEE will return an error if the dest_addr lies
 *       within the active TEE partition range.
 *
 * @param  dest_addr Destination address to write in Flash.
 * @param  src Pointer to the data to write to flash
 * @param  size Length of data in bytes.
 * @param  write_encrypted If true, data will be written encrypted on flash.
 *
 * @return ESP_OK on success, ESP_ERR_FLASH_OP_FAIL on SPI failure,
 * ESP_ERR_FLASH_OP_TIMEOUT on SPI timeout.
 */
esp_err_t bootloader_flash_write(size_t dest_addr, void *src, size_t size, bool write_encrypted);

/**
 * @brief  Erase the Flash sector.
 *
 * @param  sector  Sector number, the count starts at sector 0, 4KB per sector.
 *
 * @return esp_err_t
 */
esp_err_t bootloader_flash_erase_sector(size_t sector);

/**
 * @brief  Erase the Flash range.
 *
 * @note   [ESP-TEE] Using this API from the TEE will return an error if the start_addr lies
 *         within the active TEE partition range.
 *
 * @param  start_addr start address of flash offset
 * @param  size       sector aligned size to be erased
 *
 * @return esp_err_t
 */
esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size);

/**
 * @brief Enable the flash write protect (WEL bit).
 */
void bootloader_enable_wp(void);

/**
 * @brief Once this function is called,
 * any on-going internal operations will be terminated and the device will return to its default power-on
 * state and lose all the current volatile settings, such as Volatile Status Register bits, Write Enable Latch
 * (WEL) status, Program/Erase Suspend status, etc.
 */
void bootloader_spi_flash_reset(void);

//-------------------------------- Init APIs -------------------------------- //
//For SDK usage only
/**
 * @brief Initialize spi_flash in bootloader and print flash info
 *
 * @return ESP_OK on success, otherwise see esp_err_t
 */
esp_err_t bootloader_init_spi_flash(void);

#if CONFIG_APP_BUILD_TYPE_RAM && !CONFIG_APP_BUILD_TYPE_PURE_RAM_APP
/**
 * @brief Config all flash related stuff according to the header. The consistency of all flash configs is ensured.
 *
 * @return None
 */
void bootloader_flash_hardware_init(void);
#endif

/**
 * @brief Initialise mspi core clock
 */
void bootloader_init_mspi_clock(void);


#ifdef __cplusplus
}
#endif
