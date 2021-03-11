// Copyright 2015-2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#include "hal/spi_flash_hal.h"

/** Default configuration for the memspi (high speed version) */
#define ESP_FLASH_DEFAULT_HOST_DRIVER()  (spi_flash_host_driver_t) { \
        .dev_config = spi_flash_hal_device_config, \
        .common_command = spi_flash_hal_common_command, \
        .read_id = memspi_host_read_id_hs, \
        .erase_chip = spi_flash_hal_erase_chip, \
        .erase_sector = spi_flash_hal_erase_sector, \
        .erase_block = spi_flash_hal_erase_block, \
        .read_status = memspi_host_read_status_hs, \
        .set_write_protect = spi_flash_hal_set_write_protect, \
        .supports_direct_write = spi_flash_hal_supports_direct_write, \
        .supports_direct_read = spi_flash_hal_supports_direct_read, \
        .program_page = spi_flash_hal_program_page, \
        .write_data_slicer = memspi_host_write_data_slicer, \
        .read = spi_flash_hal_read, \
        .read_data_slicer = memspi_host_read_data_slicer, \
        .host_status = spi_flash_hal_check_status, \
        .configure_host_io_mode = spi_flash_hal_configure_host_io_mode, \
        .poll_cmd_done = spi_flash_hal_poll_cmd_done, \
        .flush_cache = memspi_host_flush_cache, \
        .check_suspend = NULL, \
        .resume = spi_flash_hal_resume, \
        .suspend = spi_flash_hal_suspend,\
        .sus_setup = spi_flash_hal_setup_read_suspend,\
}

/// configuration for the memspi host
typedef spi_flash_hal_config_t memspi_host_config_t;
/// context for the memspi host
typedef spi_flash_hal_context_t memspi_host_inst_t;

/**
 * Initialize the memory SPI host.
 *
 * @param host Pointer to the host structure.
 * @param cfg Pointer to configuration structure
 *
 * @return always return ESP_OK
 */
esp_err_t memspi_host_init_pointers(memspi_host_inst_t *host, const memspi_host_config_t *cfg);

/*******************************************************************************
 * NOTICE
 * Rest part of this file are part of the HAL layer
 * The HAL is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

/**
 * @brief Read the Status Register read from RDSR (05h).
 *
 * High speed implementation of RDID through memspi interface relying on the
 * ``common_command``.
 *
 * @param host The driver context.
 * @param id Output of the read ID from the slave.
 *
 * @return
 *  - ESP_OK: if success
 *  - ESP_ERR_FLASH_NO_RESPONSE: if no response from chip
 *  - or other cases from ``spi_hal_common_command``
 */
esp_err_t memspi_host_read_id_hs(spi_flash_host_inst_t *host, uint32_t *id);

/**
 * High speed implementation of RDSR through memspi interface relying on the
 * ``common_command``.
 *
 * @param host The driver context.
 * @param id Output of the read ID from the slave.
 *
 * @return
 *  - ESP_OK: if success
 *  - or other cases from ``spi_hal_common_command``
 */
esp_err_t memspi_host_read_status_hs(spi_flash_host_inst_t *host, uint8_t *out_sr);

/**
 * Flush the cache (if needed) after the contents are modified.
 *
 * @param host The driver context.
 * @param addr Start address of the modified region
 * @param size Size of the region modified.
 *
 * @return always ESP_OK.
 */
esp_err_t memspi_host_flush_cache(spi_flash_host_inst_t *host, uint32_t addr, uint32_t size);

/**
 *  Erase contents of entire chip.
 *
 * @param host The driver context.
 */
void memspi_host_erase_chip(spi_flash_host_inst_t *host);

/**
 *  Erase a sector starting from a given address. For 24bit address only.
 *
 * @param host The driver context.
 * @param start_address Starting address of the sector.
 */
void memspi_host_erase_sector(spi_flash_host_inst_t *host, uint32_t start_address);

/**
 *  Erase a block starting from a given address. For 24bit address only.
 *
 * @param host The driver context.
 * @param start_address Starting address of the block.
 */
void memspi_host_erase_block(spi_flash_host_inst_t *host, uint32_t start_address);

/**
 * Program a page with contents of a buffer. For 24bit address only.
 *
 * @param host The driver context.
 * @param buffer Buffer which contains the data to be flashed.
 * @param address Starting address of where to flash the data.
 * @param length The number of bytes to flash.
 */
void memspi_host_program_page(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length);

/**
 * Set ability to write to chip.
 *
 * @param host The driver context.
 * @param wp Enable or disable write protect (true - enable, false - disable).
 */
esp_err_t memspi_host_set_write_protect(spi_flash_host_inst_t *host, bool wp);

/**
 * Read data to buffer.
 *
 * @param host The driver context.
 * @param buffer Buffer which contains the data to be read.
 * @param address Starting address of where to read the data.
 * @param length The number of bytes to read.
 */
esp_err_t memspi_host_read(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len);

/**
 * @brief Slicer for read data used in non-encrypted regions. This slicer does nothing but
 *        limit the length to the maximum size the host supports.
 *
 * @param address Flash address to read
 * @param len Length to read
 * @param align_address Output of the address to read, should be equal to the input `address`
 * @param page_size Physical SPI flash page size
 *
 * @return Length that can actually be read in one `read` call in `spi_flash_host_driver_t`.
 */
int memspi_host_read_data_slicer(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_address, uint32_t page_size);

/**
 * @brief Slicer for write data used in non-encrypted regions. This slicer limit the length to the
 *        maximum size the host supports, and truncate if the write data lie accross the page boundary
 *        (256 bytes)
 *
 * @param address Flash address to write
 * @param len Length to write
 * @param align_address Output of the address to write, should be equal to the input `address`
 * @param page_size Physical SPI flash page size
 *
 * @return Length that can actually be written in one `program_page` call in `spi_flash_host_driver_t`.
 */
int memspi_host_write_data_slicer(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_address, uint32_t page_size);
