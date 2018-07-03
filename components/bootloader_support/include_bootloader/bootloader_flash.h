// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef __BOOTLOADER_FLASH_H
#define __BOOTLOADER_FLASH_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>
#include "esp_spi_flash.h"

#define FLASH_SECTOR_SIZE 0x1000

/* Provide a Flash API for bootloader_support code,
   that can be used from bootloader or app code.

   This header is available to source code in the bootloader &
   bootloader_support components only.
*/

/**
 * @brief Map a region of flash to data memory
 *
 * @important In bootloader code, only one region can be bootloader_mmaped at once. The previous region must be bootloader_munmapped before another region is mapped.
 *
 * @important In app code, these functions are not thread safe.
 *
 * Call bootloader_munmap once for each successful call to bootloader_mmap.
 *
 * In esp-idf app, this function maps directly to spi_flash_mmap.
 *
 * @param offset - Starting flash offset to map to memory.
 * @param length - Length of data to map.
 *
 * @return Pointer to mapped data memory (at src_addr), or NULL
 * if an allocation error occured.
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
 * Note: In bootloader, when write_encrypted == true, the src buffer is encrypted in place.
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

#endif
