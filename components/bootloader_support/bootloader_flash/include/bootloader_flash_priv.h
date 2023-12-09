/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef __BOOTLOADER_FLASH_H
#define __BOOTLOADER_FLASH_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>
#include <spi_flash_mmap.h> /* including in bootloader for error values */
#include "sdkconfig.h"
#include "bootloader_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_SECTOR_SIZE 0x1000
#define FLASH_BLOCK_SIZE 	0x10000
#define MMAP_ALIGNED_MASK 	(SPI_FLASH_MMU_PAGE_SIZE - 1)
#define MMU_FLASH_MASK    (~(SPI_FLASH_MMU_PAGE_SIZE - 1))

/**
 * MMU mapping must always be in the unit of a SPI_FLASH_MMU_PAGE_SIZE
 * This macro is a helper for you to get needed page nums to be mapped. e.g.:
 * Let's say SPI_FLASH_MMU_PAGE_SIZE is 64KB.
 * - v_start = 0x4200_0004
 * - size = 4 * 64KB
 *
 * You should map from 0x4200_0000, then map 5 pages.
 */
#define GET_REQUIRED_MMU_PAGES(size, v_start)    ((size + (v_start - (v_start & MMU_FLASH_MASK)) + SPI_FLASH_MMU_PAGE_SIZE - 1) / SPI_FLASH_MMU_PAGE_SIZE)

/* SPI commands (actual on-wire commands not SPI controller bitmasks)
   Suitable for use with the bootloader_execute_flash_command static function.
*/
#define CMD_RDID       0x9F
#define CMD_WRSR       0x01
#define CMD_WRSR2      0x31 /* Not all SPI flash uses this command */
#define CMD_WRSR3      0x11 /* Not all SPI flash uses this command */
#define CMD_WREN       0x06
#define CMD_WRENVSR    0x50 /* Flash write enable for volatile SR bits */
#define CMD_WRDI       0x04
#define CMD_RDSR       0x05
#define CMD_RDSR2      0x35 /* Not all SPI flash uses this command */
#define CMD_RDSR3      0x15 /* Not all SPI flash uses this command */
#define CMD_OTPEN      0x3A /* Enable OTP mode, not all SPI flash uses this command */
#define CMD_RDSFDP     0x5A /* Read the SFDP of the flash */
#define CMD_RESUME     0x7A /* Resume command to clear flash suspend bit */
#define CMD_RESETEN    0x66
#define CMD_RESET      0x99
#define CMD_FASTRD_QIO_4B   0xEC
#define CMD_FASTRD_QUAD_4B  0x6C
#define CMD_FASTRD_DIO_4B   0xBC
#define CMD_FASTRD_DUAL_4B  0x3C
#define CMD_FASTRD_4B       0x0C
#define CMD_SLOWRD_4B       0x13


/* Provide a Flash API for bootloader_support code,
   that can be used from bootloader or app code.

   This header is available to source code in the bootloader &
   bootloader_support components only.
*/

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

/**
 * @brief  Erase the Flash range.
 *
 * @param  start_addr start address of flash offset
 * @param  size       sector aligned size to be erased
 *
 * @return esp_err_t
 */
esp_err_t bootloader_flash_erase_range(uint32_t start_addr, uint32_t size);

/**
 * @brief Execute a user command on the flash
 *
 * @param command The command value to execute.
 * @param mosi_data MOSI data to send
 * @param mosi_len Length of MOSI data, in bits
 * @param miso_len Length of MISO data to receive, in bits
 * @return Received MISO data
 */
uint32_t bootloader_execute_flash_command(uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len);

/**
 * @brief Read the SFDP of the flash
 *
 * @param sfdp_addr Address of the parameter to read
 * @param miso_byte_num Bytes to read
 * @return The read SFDP, little endian, 4 bytes at most
 */
uint32_t bootloader_flash_read_sfdp(uint32_t sfdp_addr, unsigned int miso_byte_num);

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

#ifdef __cplusplus
}
#endif

#endif
