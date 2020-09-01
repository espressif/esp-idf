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

#include <esp_types.h>
#include "esp_flash_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Definition of a common transaction. Also holds the return value. */
typedef struct {
    uint8_t command;            ///< Command to send, always 8bits
    uint8_t mosi_len;           ///< Output data length, in bytes
    uint8_t miso_len;           ///< Input data length, in bytes
    uint8_t address_bitlen;     ///< Length of address in bits, set to 0 if command does not need an address
    uint32_t address;           ///< Address to perform operation on
    const uint8_t *mosi_data;   ///< Output data to salve
    uint8_t *miso_data;         ///< [out] Input data from slave, little endian
} spi_flash_trans_t;

/**
 * @brief SPI flash clock speed values, always refer to them by the enum rather
 * than the actual value (more speed may be appended into the list).
 *
 * A strategy to select the maximum allowed speed is to enumerate from the
 * ``ESP_FLSH_SPEED_MAX-1`` or highest frequency supported by your flash, and
 * decrease the speed until the probing success.
 */
typedef enum {
    ESP_FLASH_5MHZ = 0, ///< The flash runs under 5MHz
    ESP_FLASH_10MHZ,    ///< The flash runs under 10MHz
    ESP_FLASH_20MHZ,    ///< The flash runs under 20MHz
    ESP_FLASH_26MHZ,    ///< The flash runs under 26MHz
    ESP_FLASH_40MHZ,    ///< The flash runs under 40MHz
    ESP_FLASH_80MHZ,    ///< The flash runs under 80MHz
    ESP_FLASH_SPEED_MAX, ///< The maximum frequency supported by the host is ``ESP_FLASH_SPEED_MAX-1``.
} esp_flash_speed_t;

///Lowest speed supported by the driver, currently 5 MHz
#define ESP_FLASH_SPEED_MIN     ESP_FLASH_5MHZ

/** @brief Mode used for reading from SPI flash */
typedef enum {
    SPI_FLASH_SLOWRD = 0, ///< Data read using single I/O, some limits on speed
    SPI_FLASH_FASTRD, ///< Data read using single I/O, no limit on speed
    SPI_FLASH_DOUT,   ///< Data read using dual I/O
    SPI_FLASH_DIO,    ///< Both address & data transferred using dual I/O
    SPI_FLASH_QOUT,   ///< Data read using quad I/O
    SPI_FLASH_QIO,    ///< Both address & data transferred using quad I/O

    SPI_FLASH_READ_MODE_MAX,    ///< The fastest io mode supported by the host is ``ESP_FLASH_READ_MODE_MAX-1``.
} esp_flash_io_mode_t;

///Slowest io mode supported by ESP32, currently SlowRd
#define SPI_FLASH_READ_MODE_MIN SPI_FLASH_SLOWRD

struct spi_flash_host_driver_s;
typedef struct spi_flash_host_driver_s spi_flash_host_driver_t;

/** SPI Flash Host driver instance */
typedef struct {
    const struct spi_flash_host_driver_s* driver;  ///< Pointer to the implementation function table
    // Implementations can wrap this structure into their own ones, and append other data here
} spi_flash_host_inst_t ;


/** Host driver configuration and context structure. */
struct spi_flash_host_driver_s {
    /**
     * Configure the device-related register before transactions. This saves
     * some time to re-configure those registers when we send continuously
     */
    esp_err_t (*dev_config)(spi_flash_host_inst_t *host);
    /**
     * Send an user-defined spi transaction to the device.
     */
    esp_err_t (*common_command)(spi_flash_host_inst_t *host, spi_flash_trans_t *t);
    /**
     * Read flash ID.
     */
    esp_err_t (*read_id)(spi_flash_host_inst_t *host, uint32_t *id);
    /**
     * Erase whole flash chip.
     */
    void (*erase_chip)(spi_flash_host_inst_t *host);
    /**
     * Erase a specific sector by its start address.
     */
    void (*erase_sector)(spi_flash_host_inst_t *host, uint32_t start_address);
    /**
     * Erase a specific block by its start address.
     */
    void (*erase_block)(spi_flash_host_inst_t *host, uint32_t start_address);
    /**
     * Read the status of the flash chip.
     */
    esp_err_t (*read_status)(spi_flash_host_inst_t *host, uint8_t *out_sr);
    /**
     * Disable write protection.
     */
    esp_err_t (*set_write_protect)(spi_flash_host_inst_t *host, bool wp);
    /**
     * Program a page of the flash. Check ``max_write_bytes`` for the maximum allowed writing length.
     */
    void (*program_page)(spi_flash_host_inst_t *host, const void *buffer, uint32_t address, uint32_t length);
    /** Check whether given buffer can be directly used to write */
    bool (*supports_direct_write)(spi_flash_host_inst_t *host, const void *p);
    /**
     * Slicer for write data. The `program_page` should be called iteratively with the return value
     * of this function.
     *
     * @param address Beginning flash address to write
     * @param len Length request to write
     * @param align_addr Output of the aligned address to write to
     * @param page_size Physical page size of the flash chip
     * @return Length that can be actually written in one `program_page` call
     */
    int (*write_data_slicer)(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_addr,
                             uint32_t page_size);
    /**
     * Read data from the flash. Check ``max_read_bytes`` for the maximum allowed reading length.
     */
    esp_err_t (*read)(spi_flash_host_inst_t *host, void *buffer, uint32_t address, uint32_t read_len);
    /** Check whether given buffer can be directly used to read */
    bool (*supports_direct_read)(spi_flash_host_inst_t *host, const void *p);
    /**
     * Slicer for read data. The `read` should be called iteratively with the return value
     * of this function.
     *
     * @param address Beginning flash address to read
     * @param len Length request to read
     * @param align_addr Output of the aligned address to read
     * @param page_size Physical page size of the flash chip
     * @return Length that can be actually read in one `read` call
     */
    int (*read_data_slicer)(spi_flash_host_inst_t *host, uint32_t address, uint32_t len, uint32_t *align_addr, uint32_t page_size);
    /**
     * Check whether the host is idle to perform new operations.
     */
    bool (*host_idle)(spi_flash_host_inst_t *host);
    /**
     * Configure the host to work at different read mode. Responsible to compensate the timing and set IO mode.
     */
    esp_err_t (*configure_host_io_mode)(spi_flash_host_inst_t *host, uint32_t command,
                                        uint32_t addr_bitlen, int dummy_bitlen_base,
                                        esp_flash_io_mode_t io_mode);
    /**
     *  Internal use, poll the HW until the last operation is done.
     */
    void (*poll_cmd_done)(spi_flash_host_inst_t *host);
    /**
     * For some host (SPI1), they are shared with a cache. When the data is
     * modified, the cache needs to be flushed. Left NULL if not supported.
     */
    esp_err_t (*flush_cache)(spi_flash_host_inst_t* host, uint32_t addr, uint32_t size);
};

#ifdef __cplusplus
}
#endif
