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
        .max_write_bytes = SPI_FLASH_HAL_MAX_WRITE_BYTES, \
        .read = spi_flash_hal_read, \
        .max_read_bytes = SPI_FLASH_HAL_MAX_READ_BYTES, \
        .host_idle = spi_flash_hal_host_idle, \
        .configure_host_read_mode = spi_flash_hal_configure_host_read_mode, \
        .poll_cmd_done = spi_flash_hal_poll_cmd_done, \
        .flush_cache = memspi_host_flush_cache, \
}

/// configuration for the memspi host
typedef spi_flash_memspi_config_t memspi_host_config_t;
/// context for the memspi host
typedef spi_flash_memspi_data_t memspi_host_data_t;

/**
 * Initialize the memory SPI host.
 *
 * @param host Pointer to the host structure.
 * @param data Pointer to allocated space to hold the context of host driver.
 * @param cfg Pointer to configuration structure
 *
 * @return always return ESP_OK
 */
esp_err_t memspi_host_init_pointers(spi_flash_host_driver_t *host, memspi_host_data_t *data, const memspi_host_config_t *cfg);

/*******************************************************************************
 * NOTICE
 * Rest part of this file are part of the HAL layer
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

/**
 * @brief Read the Status Register read from RDSR (05h).
 *
 * High speed implementation of RDID through memspi interface relying on the
 * ``common_command``.
 *
 * @param driver The driver context.
 * @param id Output of the read ID from the slave.
 *
 * @return
 *  - ESP_OK: if success
 *  - ESP_ERR_FLASH_NO_RESPONSE: if no response from chip
 *  - or other cases from ``spi_hal_common_command``
 */
esp_err_t memspi_host_read_id_hs(spi_flash_host_driver_t *driver, uint32_t *id);

/**
 * High speed implementation of RDSR through memspi interface relying on the
 * ``common_command``.
 *
 * @param driver The driver context.
 * @param id Output of the read ID from the slave.
 *
 * @return
 *  - ESP_OK: if success
 *  - or other cases from ``spi_hal_common_command``
 */
esp_err_t memspi_host_read_status_hs(spi_flash_host_driver_t *driver, uint8_t *out_sr);

/**
 * Flush the cache (if needed) after the contents are modified.
 *
 * @param driver The driver context.
 * @param addr Start address of the modified region
 * @param size Size of the region modified.
 *
 * @return always ESP_OK.
 */
esp_err_t memspi_host_flush_cache(spi_flash_host_driver_t* driver, uint32_t addr, uint32_t size);
