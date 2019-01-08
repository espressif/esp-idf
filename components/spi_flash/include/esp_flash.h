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
#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#include "hal/spi_flash_host_drv.h"

struct spi_flash_chip_t;
typedef struct spi_flash_chip_t spi_flash_chip_t;

typedef struct esp_flash_t esp_flash_t;

/** @brief Structure for describing a region of flash */
typedef struct {
    uint32_t offset;
    uint32_t size;
} esp_flash_region_t;

/* OS-level integration hooks for accessing flash chips inside a running OS */
typedef struct {
    /**
     * Called before commencing any flash operation. Does not need to be
     * recursive (ie is called at most once for each call to 'end').
     */
    esp_err_t (*start)(void *arg);

    /** Called after completing any flash operation. */
    esp_err_t (*end)(void *arg);

    /** Delay for at least 'ms' milliseconds. Called in between 'start' and 'end'. */
    esp_err_t (*delay_ms)(void *arg, unsigned ms);
} esp_flash_os_functions_t;

/** @brief Structure to describe a SPI flash chip connected to the system.

    Structure must be passed to esp_flash_init() before use.
*/
struct esp_flash_t {
    const spi_flash_chip_t *chip_drv;   ///< Pointer to chip-model-specific "adpater" structure. If NULL, will be detected during initialisatiopn.
    spi_flash_host_driver_t *host;      ///< Pointer to hardware-specific "host_driver" structure.

    const esp_flash_os_functions_t *os_func;    ///< Pointer to os-specific hooker strcuture.
    void *os_func_data;                         ///< Pointer to argument for os-specific hooker.

    esp_flash_read_mode_t read_mode; ///< Configured SPI flash read mode. Set before initialisation.
    uint32_t size;                   ///< Size of SPI flash in bytes. If 0, size will be detected during initialisation.
};


/** @brief Initialise SPI flash chip interface.
 *
 * This function must be called before any other API functions are called for this chip.
 *
 * @note Only the spi, speed & read_mode fields of the chip structure need to be initialised. Other fields will be auto-detected
 * if left set to zero or NULL.
 *
 * @note If the chip->drv pointer is NULL, chip chip_drv will be autodetected based on its manufacturer & product IDs. See
 * esp_flash_registered_flash_drivers pointer for details of this process.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @return ESP_OK on success, or a flash error code if initialisation fails.
 */
esp_err_t esp_flash_init(esp_flash_t *chip);

/**
 * Check if appropriate chip driver is set.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 *
 * @return true if set, otherwise false.
 */
bool esp_flash_chip_driver_initialized(const esp_flash_t *chip);

/** @brief Read flash ID via the common "RDID" SPI flash command.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] Pointer to receive ID value.
 *
 * ID is a 24-bit value. Lower 16 bits of 'id' are the chip ID, upper 8 bits are the manufacturer ID.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_read_id(esp_flash_t *chip, uint32_t *id);

/** @brief Detect flash size based on flash ID.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] Detected size in bytes.
 *
 * @note Most flash chips use a common format for flash ID, where the lower 4 bits specify the size as a power of 2. If
 * the manufacturer doesn't follow this convention, the size may be incorrectly detected.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_get_size(esp_flash_t *chip, uint32_t *size);

/** @brief Erase flash chip contents
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 *
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_erase_chip(esp_flash_t *chip);

/** @brief Erase a region of the flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param start Address to start erasing flash. Must be sector aligned.
 * @param len Length of region to erase. Must also be sector aligned.
 *
 * Sector size is specifyed in chip->drv->sector_size field (typically 4096 bytes.) ESP_ERR_INVALID_ARG will be
 * returned if the start & length are not a multiple of this size.
 *
 * Erase is performed using block (multi-sector) erases where possible (block size is specified in
 * chip->drv->block_erase_size field, typically 65536 bytes). Remaining sectors are erased using individual sector erase
 * commands.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_erase_region(esp_flash_t *chip, uint32_t start, uint32_t len);

/** @brief Read if the entire chip is write protected
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] write_protected Pointer to boolean, set to the value of the write protect flag.
 *
 * @note A correct result for this flag depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_get_chip_write_protect(esp_flash_t *chip, bool *write_protected);

/** @brief Set write protection for the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param write_protected Boolean value for the write protect flag
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * If write protection is enabled, destructive operations will fail with ESP_ERR_FLASH_PROTECTED.
 *
 * Some SPI flash chips may require a power cycle before write protect status can be cleared. Otherwise,
 * write protection can be removed via a follow-up call to this function.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_set_chip_write_protect(esp_flash_t *chip, bool write_protect_chip);


/** @brief Read the list of individually protectable regions of this SPI flash chip.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param regions[out] Pointer to receive a pointer to the array of protectable regions of the chip.
 * @param[out] Pointer to an integer receiving the count of protectable regions in the array returned in 'regions'.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t
esp_flash_get_protectable_regions(const esp_flash_t *chip, const esp_flash_region_t **regions, uint32_t *num_regions);


/** @brief Detect if a region of the SPI flash chip is protected
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param region Pointer to a struct describing a protected region. This must match one of the regions returned from esp_flash_get_protectable_regions(...).
 * @param[out] protected Pointer to a flag which is set based on the protected status for this region.
 *
 * @note It is possible for this result to be false and write operations to still fail, if protection is enabled for the entire chip.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_get_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool *protected);

/** @brief Update the protected status for a region of the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param region Pointer to a struct describing a protected region. This must match one of the regions returned from esp_flash_get_protectable_regions(...).
 * @param protected Write protection flag to set.
 *
 * @note It is possible for the region protection flag to be cleared and write operations to still fail, if protection is enabled for the entire chip.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_set_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool protected);

/** @brief Read data from the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param buffer Pointer to a buffer where the data will be read. To get better performance, this should be in the DRAM and word aligned.
 * @param address Address on flash to read from. Must be less than chip->size field.
 * @param length Length (in bytes) of data to read.
 *
 * There are no alignment constraints on buffer, address or length.
 *
 * @note If on-chip flash encryption is used, this function returns raw (ie encrypted) data. Use the flash cache
 * to transparently decrypt data.
 *
 * @return
 *      - ESP_OK: success
 *      - ESP_ERR_NO_MEM: the buffer is not valid, however failed to malloc on
 *        the heap.
 *      - or a flash error code if operation failed.
 */
esp_err_t esp_flash_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length);

/** @brief Write data to the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param address Address on flash to write to. Must be previously erased (SPI NOR flash can only write bits 1->0).
 * @param buffer Pointer to a buffer with the data to write. To get better performance, this should be in the DRAM and word aligned.
 * @param length Length (in bytes) of data to write.
 *
 * There are no alignment constraints on buffer, address or length.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);

/** @brief Encrypted and write data to the SPI flash chip using on-chip hardware flash encryption
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param address Address on flash to write to. 16 byte aligned. Must be previously erased (SPI NOR flash can only write bits 1->0).
 * @param buffer Pointer to a buffer with the data to write.
 * @param length Length (in bytes) of data to write. 16 byte aligned.
 *
 * @note Both address & length must be 16 byte aligned, as this is the encryption block size
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_write_encrypted(esp_flash_t *chip, uint32_t address, const void *buffer, uint32_t length);


/** @brief Pointer to the "default" SPI flash chip, ie the main chip attached to the MCU.

   This chip is used if the 'chip' argument pass to esp_flash_xxx API functions is ever NULL.
*/
extern esp_flash_t *esp_flash_default_chip;

/** @brief Initialise the default SPI flash chip
 *
 * Called by OS startup code. You do not need to call this in your own applications.
 */
esp_err_t esp_flash_init_default_chip();

/**
 *  Enable OS-level SPI flash protections in IDF
 *
 * @return ESP_OK if success, otherwise failed. See return value of ``esp_flash_init_os_functions``.
 */
esp_err_t esp_flash_app_init(); /* ROM TODO move this to IDF */

/**
 *  Enable OS-level SPI flash for a specific chip.
 *
 * @param chip The chip to init os functions.
 * @param host_id Which SPI host to use, 0 for SPI1, 1 for HSPI2 and 2 for VSPI.
 *
 * @return ESP_OK if success, otherwise failed. See return value of ``esp_flash_init_os_functions``.
 */
esp_err_t esp_flash_init_os_functions(esp_flash_t *chip, int host_id);

/* The default (ie initial boot) no-OS ROM esp_flash_os_functions_t */
extern const esp_flash_os_functions_t spi1_default_os_functions; //todo: put into non-ROM headers

/* Pointer to the current esp_flash_os_functions_t structure in use.
   Can be changed at runtime to reflect different running conditions.
 */
//extern const esp_flash_os_functions_t *os_func;
