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

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in soc/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SPI Flash (common part)

#pragma once

#include "hal/spi_flash_ll.h"
#include "hal/spi_types.h"
#include "hal/spi_flash_types.h"
#include "soc/soc_memory_layout.h"

/* Hardware host-specific constants */
#define SPI_FLASH_HAL_MAX_WRITE_BYTES 64
#define SPI_FLASH_HAL_MAX_READ_BYTES 64

/**
 * Generic driver context structure for all chips using the SPI peripheral.
 * Include this into the HEAD of the driver data for other driver
 * implementations that also use the SPI peripheral.
 */
typedef struct {
    spi_dev_t *spi;             ///< Pointer to SPI peripheral registers (SP1, SPI2 or SPI3). Set before initialisation.
    int cs_num;                 ///< Which cs pin is used, 0-2.
    int extra_dummy;
    spi_flash_ll_clock_reg_t clock_conf;
} spi_flash_memspi_data_t;

/// Configuration structure for the SPI driver.
typedef struct {
    spi_host_device_t host_id;            ///< SPI peripheral ID.
    int cs_num;             ///< Which cs pin is used, 0-2.
    bool iomux;             ///< Whether the IOMUX is used, used for timing compensation.
    int input_delay_ns;     ///< Input delay on the MISO pin after the launch clock， used for timing compensation.
    esp_flash_speed_t speed;///< SPI flash clock speed to work at.
} spi_flash_memspi_config_t;

/**
 * Configure SPI flash hal settings.
 *
 * @param data Buffer to hold configured data, the buffer should be in DRAM to be available when cache disabled
 * @param cfg Configurations to set
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_INVALID_ARG: the data buffer is not in the DRAM.
 */
esp_err_t spi_flash_hal_init(spi_flash_memspi_data_t *data_out, const spi_flash_memspi_config_t *cfg);

/**
 * Configure the device-related register before transactions.
 *
 * @param driver The driver context.
 *
 * @return always return ESP_OK.
 */
esp_err_t spi_flash_hal_device_config(spi_flash_host_driver_t *driver);

/**
 * Send an user-defined spi transaction to the device.
 *
 * @note This is usually used when the memspi interface doesn't support some
 *      particular commands. Since this function supports timing compensation, it is
 *      also used to receive some data when the frequency is high.
 *
 * @param driver The driver context.
 * @param trans The transaction to send, also holds the received data.
 *
 * @return always return ESP_OK.
 */
esp_err_t spi_flash_hal_common_command(spi_flash_host_driver_t *driver, spi_flash_trans_t *trans);

/**
 * Erase whole flash chip by using the erase chip (C7h) command.
 *
 * @param driver The driver context.
 */
void spi_flash_hal_erase_chip(spi_flash_host_driver_t *driver);

/**
 * Erase a specific sector by its start address through the sector erase (20h)
 * command.
 *
 * @param driver The driver context.
 * @param start_address Start address of the sector to erase.
 */
void spi_flash_hal_erase_sector(spi_flash_host_driver_t *driver, uint32_t start_address);

/**
 * Erase a specific 64KB block by its start address through the 64KB block
 * erase (D8h) command.
 *
 * @param driver The driver context.
 * @param start_address Start address of the block to erase.
 */
void spi_flash_hal_erase_block(spi_flash_host_driver_t *driver, uint32_t start_address);

/**
 * Program a page of the flash using the page program (02h) command.
 *
 * @param driver The driver context.
 * @param address Address of the page to program
 * @param buffer Data to program
 * @param length Size of the buffer in bytes, no larger than ``SPI_FLASH_HAL_MAX_WRITE_BYTES`` (64) bytes.
 */
void spi_flash_hal_program_page(spi_flash_host_driver_t *driver, const void *buffer, uint32_t address, uint32_t length);

/**
 * Read from the flash. Call ``spi_flash_hal_configure_host_read_mode`` to
 * configure the read command before calling this function.
 *
 * @param driver The driver context.
 * @param buffer Buffer to store the read data
 * @param address Address to read
 * @param length Length to read, no larger than ``SPI_FLASH_HAL_MAX_READ_BYTES`` (64) bytes.
 *
 * @return always return ESP_OK.
 */
esp_err_t spi_flash_hal_read(spi_flash_host_driver_t *driver, void *buffer, uint32_t address, uint32_t read_len);

/**
 * @brief Send the write enable (06h) or write disable (04h) command to the flash chip.
 *
 * @param driver The driver context.
 * @param wp true to enable the write protection, otherwise false.
 *
 * @return always return ESP_OK.
 */
esp_err_t spi_flash_hal_set_write_protect(spi_flash_host_driver_t *chip_drv, bool wp);

/**
 * Check whether the SPI host is idle and can perform other operations.
 *
 * @param driver The driver context.
 *
 * @return ture if idle, otherwise false.
 */
bool spi_flash_hal_host_idle(spi_flash_host_driver_t *driver);

/**
 *  Configure the SPI host hardware registers for the specified read mode.
 *
 *  Note that calling this configures SPI host registers, so if running any
 *  other commands as part of set_read_mode() then these must be run before
 *  calling this function.
 *
 * @param driver The driver context
 * @param read_mode The HW read mode to use
 * @param addr_bitlen Length of the address phase, in bits
 * @param dummy_cyclelen_base Base cycles of the dummy phase, some extra dummy cycles may be appended to compensate the timing.
 * @param read_command  Actual reading command to send to flash chip on the bus.
 *
 * @return always return ESP_OK.
 */
esp_err_t spi_flash_hal_configure_host_read_mode(spi_flash_host_driver_t *driver, esp_flash_read_mode_t read_mode,
        uint32_t addr_bitlen, uint32_t dummy_cyclelen_base,
        uint32_t read_command);

/**
 * Poll until the last operation is done.
 *
 * @param driver The driver context.
 */
void spi_flash_hal_poll_cmd_done(spi_flash_host_driver_t *driver);

/**
 * Check whether the given buffer can be used as the write buffer directly. If 'chip' is connected to the main SPI bus, we can only write directly from
 * regions that are accessible ith cache disabled. *
 *
 * @param driver The driver context
 * @param p The buffer holding data to send.
 *
 * @return True if the buffer can be used to send data, otherwise false.
 */
static inline bool spi_flash_hal_supports_direct_write(spi_flash_host_driver_t *driver, const void *p)
{
#ifdef ESP_PLATFORM
    bool direct_write = ( ((spi_flash_memspi_data_t *)driver->driver_data)->spi != &SPI1
                          || esp_ptr_in_dram(p) );
#else
    //If it is not on real chips, there is no limitation that the data has to be in DRAM.
    bool direct_write = true;
#endif
    return direct_write;
}

/**
 * Check whether the given buffer can be used as the read buffer directly. If 'chip' is connected to the main SPI bus, we can only read directly from
 * regions that are accessible ith cache disabled. *
 *
 * @param driver The driver context
 * @param p The buffer to hold the received data.
 *
 * @return True if the buffer can be used to receive data, otherwise false.
 */
static inline bool spi_flash_hal_supports_direct_read(spi_flash_host_driver_t *driver, const void *p)
{
#ifdef ESP_PLATFORM
//currently the driver doesn't support to read through DMA, no word-aligned requirements
    bool direct_read = ( ((spi_flash_memspi_data_t *)driver->driver_data)->spi != &SPI1
                          || esp_ptr_in_dram(p) );
#else
    //If it is not on real chips, there is no limitation that the data has to be in DRAM.
    bool direct_read = true;
#endif
    return direct_read;
}