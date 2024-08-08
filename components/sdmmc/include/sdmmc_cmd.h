/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdio.h>
#include "esp_err.h"
#include "sd_protocol_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Call `sdmmc_io_read_bytes`, `sdmmc_io_write_bytes`, `sdmmc_io_read_blocks` or `sdmmc_io_write_bocks` APIs with
 * address ORed by this flag to send CMD53 with OP Code clear (fixed address) */
#define SDMMC_IO_FIXED_ADDR BIT(31)

/**
 * Probe and initialize SD/MMC card using given host
 *
 * @note Only SD cards (SDSC and SDHC/SDXC) are supported now.
 *       Support for MMC/eMMC cards will be added later.
 *
 * @param host  pointer to structure defining host controller
 * @param out_card  pointer to structure which will receive information
 *                  about the card when the function completes
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_card_init(const sdmmc_host_t* host,
        sdmmc_card_t* out_card);

/**
 * @brief Print information about the card to a stream
 * @param stream  stream obtained using fopen or fdopen
 * @param card  card information structure initialized using sdmmc_card_init
 */
void sdmmc_card_print_info(FILE* stream, const sdmmc_card_t* card);

/**
 * Get status of SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_get_status(sdmmc_card_t* card);

/**
 * Write given number of sectors to SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param src   pointer to data buffer to read data from; data size must be
 *              equal to sector_count * card->csd.sector_size
 * @param start_sector  sector where to start writing
 * @param sector_count  number of sectors to write
 * @return
 *      - ESP_OK on success or sector_count equal to 0
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_write_sectors(sdmmc_card_t* card, const void* src,
        size_t start_sector, size_t sector_count);

/**
 * Read given number of sectors from the SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param dst   pointer to data buffer to write into; buffer size must be
 *              at least sector_count * card->csd.sector_size
 * @param start_sector  sector where to start reading
 * @param sector_count  number of sectors to read
 * @return
 *      - ESP_OK on success or sector_count equal to 0
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_read_sectors(sdmmc_card_t* card, void* dst,
        size_t start_sector, size_t sector_count);

/**
 * Erase given number of sectors from the SD/MMC card
 *
 * @note When sdmmc_erase_sectors used with cards in SDSPI mode, it was
 * observed that card requires re-init after erase operation.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param start_sector  sector where to start erase
 * @param sector_count  number of sectors to erase
 * @param arg  erase command (CMD38) argument
 * @return
 *      - ESP_OK on success or sector_count equal to 0
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_erase_sectors(sdmmc_card_t* card, size_t start_sector,
        size_t sector_count, sdmmc_erase_arg_t arg);

/**
 * Check if SD/MMC card supports discard
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - ESP_OK if supported by the card/device
 *      - ESP_FAIL if not supported by the card/device
 */
esp_err_t sdmmc_can_discard(sdmmc_card_t* card);

/**
 * Check if SD/MMC card supports trim
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - ESP_OK if supported by the card/device
 *      - ESP_FAIL if not supported by the card/device
 */
esp_err_t sdmmc_can_trim(sdmmc_card_t* card);

/**
 * Check if SD/MMC card supports sanitize
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - ESP_OK if supported by the card/device
 *      - ESP_FAIL if not supported by the card/device
 */
esp_err_t sdmmc_mmc_can_sanitize(sdmmc_card_t* card);

/**
 * Sanitize the data that was unmapped by a Discard command
 *
 * @note  Discard command has to precede sanitize operation. To discard, use
 *        MMC_DICARD_ARG with sdmmc_erase_sectors argument
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param timeout_ms timeout value in milliseconds required to sanitize the
 *                   selected range of sectors.
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_mmc_sanitize(sdmmc_card_t* card, uint32_t timeout_ms);

/**
 * Erase complete SD/MMC card
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_full_erase(sdmmc_card_t* card);

/**
 * Read one byte from an SDIO card using IO_RW_DIRECT (CMD52)
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param reg  byte address within IO function
 * @param[out] out_byte  output, receives the value read from the card
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_io_read_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t reg, uint8_t *out_byte);

/**
 * Write one byte to an SDIO card using IO_RW_DIRECT (CMD52)
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param reg  byte address within IO function
 * @param in_byte  value to be written
 * @param[out] out_byte  if not NULL, receives new byte value read
 *                       from the card (read-after-write).
 * @return
 *      - ESP_OK on success
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_io_write_byte(sdmmc_card_t* card, uint32_t function,
        uint32_t reg, uint8_t in_byte, uint8_t* out_byte);

/**
 * Read multiple bytes from an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs read operation using CMD53 in byte mode.
 * For block mode, see sdmmc_io_read_blocks.
 *
 * By default OP Code is set (incrementing address). To send CMD53 without this bit, OR the argument `addr` with
 * `SDMMC_IO_FIXED_ADDR`.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where reading starts
 * @param dst  buffer which receives the data read from card. Aligned to 4 byte boundary unless
 *             `SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF` flag is set when calling `sdmmc_card_init`. The flag is mandatory
 *             when the buffer is behind the cache.
 * @param size  number of bytes to read, 1 to 512.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_SIZE if size exceeds 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_io_read_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size);

/**
 * Write multiple bytes to an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs write operation using CMD53 in byte mode.
 * For block mode, see sdmmc_io_write_blocks.
 *
 * By default OP Code is set (incrementing address). To send CMD53 without this bit, OR the argument `addr` with
 * `SDMMC_IO_FIXED_ADDR`.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where writing starts
 * @param src  data to be written. Aligned to 4 byte boundary unless `SDMMC_HOST_FLAG_ALLOC_ALIGNED_BUF` flag is set
 *             when calling `sdmmc_card_init`. The flag is mandatory when the buffer is behind the cache.
 * @param size  number of bytes to write, 1 to 512.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_SIZE if size exceeds 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_io_write_bytes(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size);

/**
 * Read blocks of data from an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs read operation using CMD53 in block mode.
 * For byte mode, see sdmmc_io_read_bytes.
 *
 * By default OP Code is set (incrementing address). To send CMD53 without this bit, OR the argument `addr` with
 * `SDMMC_IO_FIXED_ADDR`.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where writing starts
 * @param dst  buffer which receives the data read from card. Aligned to 4 byte boundary, and also cache line size if
 *             the buffer is behind the cache.
 * @param size  number of bytes to read, must be divisible by the card block
 *              size.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_SIZE if size is not divisible by 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_io_read_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, void* dst, size_t size);

/**
 * Write blocks of data to an SDIO card using IO_RW_EXTENDED (CMD53)
 *
 * This function performs write operation using CMD53 in block mode.
 * For byte mode, see sdmmc_io_write_bytes.
 *
 * By default OP Code is set (incrementing address). To send CMD53 without this bit, OR the argument `addr` with
 * `SDMMC_IO_FIXED_ADDR`.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param function  IO function number
 * @param addr  byte address within IO function where writing starts
 * @param src  data to be written. Aligned to 4 byte boundary, and also cache line size if the buffer is behind the
 *             cache.
 * @param size  number of bytes to write, must be divisible by the card block
 *              size.
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_SIZE if size is not divisible by 512 bytes
 *      - One of the error codes from SDMMC host controller
 */
esp_err_t sdmmc_io_write_blocks(sdmmc_card_t* card, uint32_t function,
        uint32_t addr, const void* src, size_t size);

/**
 * Enable SDIO interrupt in the SDMMC host
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_SUPPORTED if the host controller does not support
 *        IO interrupts
 */
esp_err_t sdmmc_io_enable_int(sdmmc_card_t* card);

/**
 * Block until an SDIO interrupt is received
 *
 * Slave uses D1 line to signal interrupt condition to the host.
 * This function can be used to wait for the interrupt.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param timeout_ticks  time to wait for the interrupt, in RTOS ticks
 * @return
 *      - ESP_OK if the interrupt is received
 *      - ESP_ERR_NOT_SUPPORTED if the host controller does not support
 *        IO interrupts
 *      - ESP_ERR_TIMEOUT if the interrupt does not happen in timeout_ticks
 */
esp_err_t sdmmc_io_wait_int(sdmmc_card_t* card, TickType_t timeout_ticks);

/**
 * Get the data of CIS region of an SDIO card.
 *
 * You may provide a buffer not sufficient to store all the CIS data. In this
 * case, this function stores as much data into your buffer as possible. Also,
 * this function will try to get and return the size required for you.
 *
 * @param card  pointer to card information structure previously initialized
 *              using sdmmc_card_init
 * @param out_buffer Output buffer of the CIS data
 * @param buffer_size Size of the buffer.
 * @param inout_cis_size Mandatory, pointer to a size, input and output.
 *              - input: Limitation of maximum searching range, should be 0 or larger than
 *                      buffer_size. The function searches for CIS_CODE_END until this range. Set to
 *                      0 to search infinitely.
 *              - output: The size required to store all the CIS data, if CIS_CODE_END is found.
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_INVALID_RESPONSE: if the card does not (correctly) support CIS.
 *      - ESP_ERR_INVALID_SIZE: CIS_CODE_END found, but buffer_size is less than
 *              required size, which is stored in the inout_cis_size then.
 *      - ESP_ERR_NOT_FOUND: if the CIS_CODE_END not found. Increase input value of
 *              inout_cis_size or set it to 0, if you still want to search for the end;
 *              output value of inout_cis_size is invalid in this case.
 *      - and other error code return from sdmmc_io_read_bytes
 */
esp_err_t sdmmc_io_get_cis_data(sdmmc_card_t* card, uint8_t* out_buffer, size_t buffer_size, size_t* inout_cis_size);

/**
 * Parse and print the CIS information of an SDIO card.
 *
 * @note Not all the CIS codes and all kinds of tuples are supported. If you
 * see some unresolved code, you can add the parsing of these code in
 * sdmmc_io.c and contribute to the IDF through the Github repository.
 *
 *              using sdmmc_card_init
 * @param buffer Buffer to parse
 * @param buffer_size Size of the buffer.
 * @param fp File pointer to print to, set to NULL to print to stdout.
 *
 * @return
 *      - ESP_OK: on success
 *      - ESP_ERR_NOT_SUPPORTED: if the value from the card is not supported to be parsed.
 *      - ESP_ERR_INVALID_SIZE: if the CIS size fields are not correct.
 */
esp_err_t sdmmc_io_print_cis_info(uint8_t* buffer, size_t buffer_size, FILE* fp);


#ifdef __cplusplus
}
#endif
