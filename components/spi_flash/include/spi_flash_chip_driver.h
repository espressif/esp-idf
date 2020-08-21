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
#include "esp_flash.h"

struct esp_flash_t;
typedef struct esp_flash_t esp_flash_t;

typedef struct spi_flash_chip_t spi_flash_chip_t;

/** Timeout configurations for flash operations, all in us */
typedef struct {
    uint32_t chip_erase_timeout;    ///< Timeout for chip erase operation
    uint32_t block_erase_timeout;   ///< Timeout for block erase operation
    uint32_t sector_erase_timeout;  ///< Timeout for sector erase operation
    uint32_t idle_timeout;          ///< Default timeout for other commands to be sent by host and get done by flash
    uint32_t page_program_timeout;  ///< Timeout for page program operation
} flash_chip_op_timeout_t;

/** @brief SPI flash chip driver definition structure.
 *
 * The chip driver structure contains chip-specific pointers to functions to perform SPI flash operations, and some
 * chip-specific numeric values.
 *
 * @note This is not a public API. These functions are called from the public API (declared in
 * esp_flash.h). They assume the caller has already validated arguments and enabled relevant protections
 * (disabling flash cache, prevent concurrent SPI access, etc.)
 *
 * Do not call chip driver functions directly in other contexts.
 *
 * A generic driver for generic chips and its related operations are defined in
 * spi_flash_chip_generic.h which can be used as building blocks for written
 * new/specific SPI flash chip drivers.
 *
 * @note All of these functions may be called with SPI flash cache disabled, so must only ever access IRAM/DRAM/ROM.
 */
struct spi_flash_chip_t {
    const char *name; ///< Name of the chip driver
    const flash_chip_op_timeout_t *timeout; ///< Timeout configuration for this chip
    /* Probe to detect if a supported SPI flash chip is found.
     *
     * Attempts to configure 'chip' with these operations and probes for a matching SPI flash chip.
     *
     * Auto-detection of a SPI flash chip calls this function in turn on each registered driver (see esp_flash_registered_flash_drivers).
     *
     * ID - as read by spi_flash_generic_read_id() - is supplied so each probe
     * function doesn't need to unnecessarily read ID, but probe is permitted
     * to interrogate flash in any non-destructive way.
     *
     * It is permissible for the driver to modify the 'chip' structure if probing succeeds (specifically, to assign something to the
     * driver_data pointer if that is useful for the driver.)
     *
     * @return ESP_OK if probing was successful, an error otherwise. Driver may
     * assume that returning ESP_OK means it has claimed this chip.
     */
    esp_err_t (*probe)(esp_flash_t *chip, uint32_t flash_id);

    esp_err_t (*reset)(esp_flash_t *chip);


    /* Detect SPI flash size
     *
     * Interrogate the chip to detect its size.
     */
    esp_err_t (*detect_size)(esp_flash_t *chip, uint32_t *size);

    /* Erase the entire chip

       Caller has verified the chip is not write protected.
     */
    esp_err_t (*erase_chip)(esp_flash_t *chip);

    /* Erase a sector of the chip. Sector size is specified in the 'sector_size' field.

       sector_address is an offset in bytes.

       Caller has verified that this sector should be non-write-protected.
     */
    esp_err_t (*erase_sector)(esp_flash_t *chip, uint32_t sector_address);

    /* Erase a multi-sector block of the chip. Block size is specified in the 'block_erase_size' field.
       sector_address is an offset in bytes.

       Caller has verified that this block should be non-write-protected.
     */
    esp_err_t (*erase_block)(esp_flash_t *chip, uint32_t block_address);

    uint32_t sector_size; /* Sector is minimum erase size */
    uint32_t block_erase_size; /* Optimal (fastest) block size for multi-sector erases on this chip */

    /* Read the write protect status of the entire chip. */
    esp_err_t (*get_chip_write_protect)(esp_flash_t *chip, bool *out_write_protected);

    /* Set the write protect status of the entire chip. */
    esp_err_t (*set_chip_write_protect)(esp_flash_t *chip, bool chip_write_protect);

    /* Number of individually write protectable regions on this chip. Range 0-63. */
    uint8_t num_protectable_regions;
    /* Pointer to an array describing each protectable region. Should have num_protectable_regions elements. */
    const esp_flash_region_t *protectable_regions;
    /* Get a bitmask describing all protectable regions on the chip. Each bit represents one entry in the
       protectable_regions array, ie bit (1<<N) is set then the region at array entry N is write protected. */
    esp_err_t (*get_protected_regions)(esp_flash_t *chip, uint64_t *regions);

    /* Set protectable regions on the chip. Each bit represents on entry in the protectable regions array. */
    esp_err_t (*set_protected_regions)(esp_flash_t *chip, uint64_t regions);

    /* Read data from the chip.
     *
     * Before calling this function, the caller will have called chip->drv->set_read_mode(chip) in order to configure the chip's read mode correctly.
     */
    esp_err_t (*read)(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length);

    /* Write any amount of data to the chip.
     */
    esp_err_t (*write)(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);


    /* Use the page program command to write data to the chip.
     *
     * This function is expected to be called by chip->drv->write (if the
     * chip->drv->write implementation doesn't call it then it can be left as NULL.)
     *
     * - The length argument supplied to this function is at most 'page_size' bytes.
     *
     * - The region between 'address' and 'address + length' will not cross a page_size aligned boundary (the write
     *   implementation is expected to split such a write into two before calling page_program.)
     */
    esp_err_t (*program_page)(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);

    /* Page size as written by the page_program function. Usually 256 bytes. */
    uint32_t page_size;

    /* Perform an encrypted write to the chip, using internal flash encryption hardware. */
    esp_err_t (*write_encrypted)(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);


    /* Wait for the SPI flash chip to be idle (any write operation to be complete.) This function is both called from the higher-level API functions, and from other functions in this structure.

       timeout_ms should be a timeout (in milliseconds) before the function returns ESP_ERR_TIMEOUT. This is useful to avoid hanging
       if the chip is otherwise unresponsive (ie returns all 0xFF or similar.)
    */
    esp_err_t (*wait_idle)(esp_flash_t *chip, uint32_t timeout_us);

    /* Configure both the SPI host and the chip for the read mode specified in chip->read_mode.
     *
     * This function is called by the higher-level API before the 'read' function is called.
     *
     * Can return ESP_ERR_FLASH_UNSUPPORTED_HOST or ESP_ERR_FLASH_UNSUPPORTED_CHIP if the specified mode is unsupported.
     */
    esp_err_t (*set_io_mode)(esp_flash_t *chip);

    /*
     * Get whether the Quad Enable (QE) is set. (*out_io_mode)=SPI_FLASH_QOUT if
     * enabled, otherwise disabled
     */
    esp_err_t (*get_io_mode)(esp_flash_t *chip, esp_flash_io_mode_t* out_io_mode);
};

/* Pointer to an array of pointers to all known drivers for flash chips. This array is used
   by esp_flash_init() to detect the flash chip driver, if none is supplied by the caller.

   Array is terminated with a NULL pointer.

   This pointer can be overwritten with a pointer to a new array, to update the list of known flash chips.
 */
extern const spi_flash_chip_t **esp_flash_registered_chips;
