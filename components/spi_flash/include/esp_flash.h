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

#include "hal/spi_flash_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct spi_flash_chip_t;
typedef struct spi_flash_chip_t spi_flash_chip_t;

typedef struct esp_flash_t esp_flash_t;

/** @brief Structure for describing a region of flash */
typedef struct {
    uint32_t offset;    ///< Start address of this region
    uint32_t size;      ///< Size of the region
} esp_flash_region_t;

/** @brief OS-level integration hooks for accessing flash chips inside a running OS
 *
 * It's in the public header because some instances should be allocated statically in the startup
 * code. May be updated according to hardware version and new flash chip feature requirements,
 * shouldn't be treated as public API.
 *
 *  For advanced developers, you may replace some of them with your implementations at your own
 *  risk.
*/
typedef struct {
    /**
     * Called before commencing any flash operation. Does not need to be
     * recursive (ie is called at most once for each call to 'end').
     */
    esp_err_t (*start)(void *arg);

    /** Called after completing any flash operation. */
    esp_err_t (*end)(void *arg);

    /** Called before any erase/write operations to check whether the region is limited by the OS */
    esp_err_t (*region_protected)(void* arg, size_t start_addr, size_t size);

    /** Delay for at least 'us' microseconds. Called in between 'start' and 'end'. */
    esp_err_t (*delay_us)(void *arg, uint32_t us);

    /** Called for get temp buffer when buffer from application cannot be directly read into/write from. */
    void *(*get_temp_buffer)(void* arg, size_t reqest_size, size_t* out_size);

    /** Called for release temp buffer. */
    void (*release_temp_buffer)(void* arg, void *temp_buf);

    #define SPI_FLASH_YIELD_REQ_YIELD   BIT(0)
    #define SPI_FLASH_YIELD_REQ_SUSPEND BIT(1)

    /** Yield to other tasks. Called during erase operations.
     * @return ESP_OK means yield needs to be called (got an event to handle), while ESP_ERR_TIMEOUT means skip yield.*/
    esp_err_t (*check_yield)(void *arg, uint32_t chip_status, uint32_t* out_request);

    #define SPI_FLASH_YIELD_STA_RESUME  BIT(2)

    /** Yield to other tasks. Called during erase operations. */
    esp_err_t (*yield)(void *arg, uint32_t* out_status);

    /** Called for get system time. */
    int64_t (*get_system_time)(void *arg);

} esp_flash_os_functions_t;

/** @brief Structure to describe a SPI flash chip connected to the system.

    Structure must be initialized before use (passed to esp_flash_init()). It's in the public
    header because some instances should be allocated statically in the startup code. May be
    updated according to hardware version and new flash chip feature requirements, shouldn't be
    treated as public API.

    For advanced developers, you may replace some of them with your implementations at your own
    risk.
*/
struct esp_flash_t {
    spi_flash_host_inst_t* host;   ///< Pointer to hardware-specific "host_driver" structure. Must be initialized before used.
    const spi_flash_chip_t *chip_drv;   ///< Pointer to chip-model-specific "adapter" structure. If NULL, will be detected during initialisation.

    const esp_flash_os_functions_t *os_func;    ///< Pointer to os-specific hook structure. Call ``esp_flash_init_os_functions()`` to setup this field, after the host is properly initialized.
    void *os_func_data;                         ///< Pointer to argument for os-specific hooks. Left NULL and will be initialized with ``os_func``.

    esp_flash_io_mode_t read_mode; ///< Configured SPI flash read mode. Set before ``esp_flash_init`` is called.
    uint32_t size;                   ///< Size of SPI flash in bytes. If 0, size will be detected during initialisation.
    uint32_t chip_id;               ///< Detected chip id.
    uint32_t busy             :1;   ///< This flag is used to verify chip's status.
    uint32_t reserved_flags   :31;  ///< reserved.
};


/** @brief Initialise SPI flash chip interface.
 *
 * This function must be called before any other API functions are called for this chip.
 *
 * @note Only the ``host`` and ``read_mode`` fields of the chip structure must
 *       be initialised before this function is called. Other fields may be
 *       auto-detected if left set to zero or NULL.
 *
 * @note If the chip->drv pointer is NULL, chip chip_drv will be auto-detected
 *       based on its manufacturer & product IDs. See
 *       ``esp_flash_registered_flash_drivers`` pointer for details of this process.
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
 * @param[out] out_id Pointer to receive ID value.
 *
 * ID is a 24-bit value. Lower 16 bits of 'id' are the chip ID, upper 8 bits are the manufacturer ID.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_read_id(esp_flash_t *chip, uint32_t *out_id);

/** @brief Detect flash size based on flash ID.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] out_size Detected size in bytes.
 *
 * @note Most flash chips use a common format for flash ID, where the lower 4 bits specify the size as a power of 2. If
 * the manufacturer doesn't follow this convention, the size may be incorrectly detected.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_get_size(esp_flash_t *chip, uint32_t *out_size);

/** @brief Read flash unique ID via the common "RDUID" SPI flash command.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init().
 * @param[out] out_id Pointer to receive unique ID value.
 *
 * ID is a 64-bit value.
 *
 * @return
 *      - ESP_OK on success, or a flash error code if operation failed.
 *      - ESP_ERR_NOT_SUPPORTED if the chip doesn't support read id.
 */
esp_err_t esp_flash_read_unique_chip_id(esp_flash_t *chip, uint64_t *out_id);

/** @brief Erase flash chip contents
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 *
 *
 * @return
 *      - ESP_OK on success,
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - Other flash error code if operation failed.
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
 * @return
 *      - ESP_OK on success,
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - Other flash error code if operation failed.
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
 * @param write_protect Boolean value for the write protect flag
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * Some SPI flash chips may require a power cycle before write protect status can be cleared. Otherwise,
 * write protection can be removed via a follow-up call to this function.
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_set_chip_write_protect(esp_flash_t *chip, bool write_protect);


/** @brief Read the list of individually protectable regions of this SPI flash chip.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] out_regions Pointer to receive a pointer to the array of protectable regions of the chip.
 * @param[out] out_num_regions Pointer to an integer receiving the count of protectable regions in the array returned in 'regions'.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_get_protectable_regions(const esp_flash_t *chip, const esp_flash_region_t **out_regions, uint32_t *out_num_regions);


/** @brief Detect if a region of the SPI flash chip is protected
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param region Pointer to a struct describing a protected region. This must match one of the regions returned from esp_flash_get_protectable_regions(...).
 * @param[out] out_protected Pointer to a flag which is set based on the protected status for this region.
 *
 * @note It is possible for this result to be false and write operations to still fail, if protection is enabled for the entire chip.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_get_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool *out_protected);

/** @brief Update the protected status for a region of the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param region Pointer to a struct describing a protected region. This must match one of the regions returned from esp_flash_get_protectable_regions(...).
 * @param protect Write protection flag to set.
 *
 * @note It is possible for the region protection flag to be cleared and write operations to still fail, if protection is enabled for the entire chip.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and chip_drv in use (via the 'chip->drv'
 * field).
 *
 * @return ESP_OK on success, or a flash error code if operation failed.
 */
esp_err_t esp_flash_set_protected_region(esp_flash_t *chip, const esp_flash_region_t *region, bool protect);

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
 *      - ESP_ERR_NO_MEM: Buffer is in external PSRAM which cannot be concurrently accessed, and a temporary internal buffer could not be allocated.
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
 * @return
 *      - ESP_OK on success,
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - Other flash error code if operation failed.
 */
esp_err_t esp_flash_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);

/** @brief Encrypted and write data to the SPI flash chip using on-chip hardware flash encryption
 *
 * @param chip Pointer to identify flash chip. Must be NULL (the main flash chip). For other chips, encrypted write is not supported.
 * @param address Address on flash to write to. 16 byte aligned. Must be previously erased (SPI NOR flash can only write bits 1->0).
 * @param buffer Pointer to a buffer with the data to write.
 * @param length Length (in bytes) of data to write. 16 byte aligned.
 *
 * @note Both address & length must be 16 byte aligned, as this is the encryption block size
 *
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_NOT_SUPPORTED: encrypted write not supported for this chip.
 *  - ESP_ERR_INVALID_ARG: Either the address, buffer or length is invalid.
 *  - or other flash error code from spi_flash_write_encrypted().
 */
esp_err_t esp_flash_write_encrypted(esp_flash_t *chip, uint32_t address, const void *buffer, uint32_t length);

/** @brief Read and decrypt data from the SPI flash chip using on-chip hardware flash encryption
 *
 * @param chip Pointer to identify flash chip. Must be NULL (the main flash chip). For other chips, encrypted read is not supported.
 * @param address Address on flash to read from.
 * @param out_buffer Pointer to a buffer for the data to read to.
 * @param length Length (in bytes) of data to read.
 *
 * @return
 *  - ESP_OK: on success
 *  - ESP_ERR_NOT_SUPPORTED: encrypted read not supported for this chip.
 *  - or other flash error code from spi_flash_read_encrypted().
 */
esp_err_t esp_flash_read_encrypted(esp_flash_t *chip, uint32_t address, void *out_buffer, uint32_t length);

/** @brief Pointer to the "default" SPI flash chip, ie the main chip attached to the MCU.

   This chip is used if the 'chip' argument pass to esp_flash_xxx API functions is ever NULL.
*/
extern esp_flash_t *esp_flash_default_chip;


/*******************************************************************************
 * Utility Functions
 ******************************************************************************/

/**
 * @brief Returns true if chip is configured for Quad I/O or Quad Fast Read.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 *
 * @return true if flash works in quad mode, otherwise false
 */
static inline bool esp_flash_is_quad_mode(const esp_flash_t *chip)
{
    return (chip->read_mode == SPI_FLASH_QIO) || (chip->read_mode == SPI_FLASH_QOUT);
}

#ifdef __cplusplus
}
#endif
