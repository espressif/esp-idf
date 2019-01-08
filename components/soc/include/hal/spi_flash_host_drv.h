// Copyright 2010-2019 Espressif Systems (Shanghai) PTE LTD
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

#include "hal/spi_flash_ll.h"
#include "hal/esp_flash_err.h"

/** Definition of a common transaction. Also holds the return value. */
typedef struct {
    uint8_t command;        ///< Command to send, always 8bits
    uint8_t mosi_len;       ///< Output data length, in bits
    uint8_t miso_len;       ///< Input data length, in bits
    uint32_t mosi_data;     ///< Output data to slave
    uint32_t miso_data[2];  ///< [out] Input data from slave, little endian
} spi_flash_trans_t;


struct spi_flash_host_driver_t;
typedef struct spi_flash_host_driver_t spi_flash_host_driver_t;

/** Host driver configuration and context structure. */
struct spi_flash_host_driver_t {
    /**
     * Configuration and static data used by the specific host driver. The type
     * is determined by the host driver.
     */
    void *driver_data;
    /**
     * Configure the device-related register before transactions. This saves
     * some time to re-configure those registers when we send continuously
     */
    esp_err_t (*dev_config)(spi_flash_host_driver_t *driver);
    /**
     * Send an user-defined spi transaction to the device.
     */
    esp_err_t (*common_command)(spi_flash_host_driver_t *driver, spi_flash_trans_t *t);
    /**
     * Read flash ID.
     */
    esp_err_t (*read_id)(spi_flash_host_driver_t *driver, uint32_t *id);
    /**
     * Erase whole flash chip.
     */
    void (*erase_chip)(spi_flash_host_driver_t *driver);
    /**
     * Erase a specific sector by its start address.
     */
    void (*erase_sector)(spi_flash_host_driver_t *driver, uint32_t start_address);
    /**
     * Erase a specific block by its start address.
     */
    void (*erase_block)(spi_flash_host_driver_t *driver, uint32_t start_address);
    /**
     * Read the status of the flash chip.
     */
    esp_err_t (*read_status)(spi_flash_host_driver_t *driver, uint8_t *out_sr);
    /**
     * Disable write protection.
     */
    esp_err_t (*set_write_protect)(spi_flash_host_driver_t *driver, bool wp);
    /**
     * Program a page of the flash. Check ``max_write_bytes`` for the maximum allowed writing length.
     */
    void (*program_page)(spi_flash_host_driver_t *driver, const void *buffer, uint32_t address, uint32_t length);
    /** Check whether need to allocate new buffer to write */
    bool (*supports_direct_write)(spi_flash_host_driver_t *driver, const void *p);
    /** Check whether need to allocate new buffer to read */
    bool (*supports_direct_read)(spi_flash_host_driver_t *driver, const void *p);
    /** maximum length of program_page */
    int max_write_bytes;
    /**
     * Read data from the flash. Check ``max_read_bytes`` for the maximum allowed reading length.
     */
    esp_err_t (*read)(spi_flash_host_driver_t *driver, void *buffer, uint32_t address, uint32_t read_len);
    /** maximum length of read */
    int max_read_bytes;
    /**
     * Check whether the host is idle to perform new operations.
     */
    bool (*host_idle)(spi_flash_host_driver_t *driver);
    /**
     * Configure the host to work at different read mode.
     */
    esp_err_t (*configure_host_read_mode)(spi_flash_host_driver_t *driver, esp_flash_read_mode_t read_mode, uint32_t addr_bitlen, uint32_t dummy_bitlen_base, uint32_t read_command);
    /**
     *  Internal use, poll the HW until the last operation is done.
     */
    void (*poll_cmd_done)(spi_flash_host_driver_t *driver);
    /**
     * For some host (SPI1), they are shared with a cache. When the data is
     * modified, the cache needs to be flushed. Left NULL if not supported.
     */
    esp_err_t (*flush_cache)(spi_flash_host_driver_t* driver, uint32_t addr, uint32_t size);
    /**
     * Check if the given region is protected (e.g. is the bootloader). Left
     * NULL if current host doesn't need protection.
     */
    bool (*region_protected)(spi_flash_host_driver_t* driver, uint32_t addr, uint32_t size);
};

