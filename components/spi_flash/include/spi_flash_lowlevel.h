#pragma once
#include <stdint.h>
#include <stdbool.h>

#include "soc/spi_struct.h"

struct esp_flash_driver;

/** @brief Mode used for reading from SPI flash */
typedef enum {
    ESP_FLASH_QIO,    ///< Both address & data transferred using quad I/O
    ESP_FLASH_QOUT,   ///< Data read using quad I/O
    ESP_FLASH_DIO,    ///< Both address & data transferred using dual I/O
    ESP_FLASH_DOUT,   ///< Data read using dual I/O
    ESP_FLASH_FASTRD, ///< Data read using single I/O, no limit on speed
    ESP_FLASH_SLOWRD, ///< Data read using single I/O, some limits on speed

    ESP_FLASH_READ_MODE_MAX,
} esp_flash_read_mode_t;

/** @brief Configured SPI flash clock speed */
typedef enum {
    ESP_FLASH_80MHZ,
    ESP_FLASH_40MHZ,
    ESP_FLASH_26MHZ,
    ESP_FLASH_20MHZ,
    ESP_FLASH_SPEED_MAX,
} esp_flash_speed_t;

/** @brief Structure for describing a region of flash */
typedef struct {
    uint32_t offset;
    uint32_t size;
} esp_flash_region_t;

// TODO this is copied from SPI driver, should be unified somehow
typedef struct {
    int mosi_io_num;                ///< GPIO pin for Master Out Slave In (=spi_d) signal
    int miso_io_num;                ///< GPIO pin for Master In Slave Out (=spi_q) signal
    int sclk_io_num;                ///< GPIO pin for Spi CLocK signal
    int quadwp_io_num;              ///< GPIO pin for WP (Write Protect) signal which is used as D2 in 4-bit communication modes, or -1 if not used.
    int quadhd_io_num;              ///< GPIO pin for HD (HolD) signal which is used as D3 in 4-bit communication modes, or -1 if not used.
} esp_flash_pin_cfg_t;

/** @brief Structure to describe a SPI flash chip connected to the system.

    Structure must be passed to esp_flash_init() before use.
*/
typedef struct {
    spi_dev_t *spi;                  ///< Pointer to hardware SPI bus registers used for connection (SP1, SPI2 or SPI3). Set before initialisation.
    esp_flash_speed_t speed;         ///< Configured SPI flash clock speed. Set before initialisation.
    esp_flash_read_mode_t read_mode; ///< Configured SPI flash read mode. Set before initialisation.
    uint32_t size;                   ///< Size of SPI flash in bytes. If 0, size will be detected during initialisation.
    const struct esp_flash_driver *drv; ///< Pointer to chip-model-specific "driver" structure. If NULL, will be detected during initialisatiopn.
    const esp_flash_pin_cfg_t *pins;       ///< Pin configuration for the chip

    void *driver_data; ///< Currently unused, allows drivers to store driver-implementation-specific data on initialisation
} esp_flash_chip_t;

/** @brief Possible errors returned from SPI flash low-level API */
typedef enum {
    FLASH_OK = 0,                ///< Success
    FLASH_ERR_NOT_INITIALISED,   ///< esp_flash_chip_t structure not correctly initialised by esp_flash_init().
    FLASH_ERR_INVALID_ARG,       ///< A supplied argument was invalid.
    FLASH_ERR_NOT_FOUND,         ///< A requested value is not found.
    FLASH_ERR_NO_RESPONSE,       ///< Chip did not respond to the command, or timed out.
    FLASH_ERR_UNSUPPORTED_HOST,  ///< Requested operation isn't supported via this host SPI bus (chip->spi field).
    FLASH_ERR_UNSUPPORTED_CHIP,  ///< Requested operation isn't supported by this model of SPI flash chip.
    FLASH_ERR_PROTECTED,         ///< Write operation failed due to chip's write protection being enabled.
} esp_flash_err_t;

/** @brief Initialise SPI flash chip interface.
 * 
 * This function must be called before any other API functions are called for this chip.
 *
 * @note Only the spi, speed & read_mode fields of the chip structure need to be initialised. Other fields will be auto-detected
 * if left set to zero or NULL.
 *
 * @note If the chip->drv pointer is NULL, chip driver will be autodetected based on its manufacturer & product IDs. See
 * esp_flash_registered_flash_drivers pointer for details of this process.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @return FLASH_OK on success, or a flash error code if initialisation fails.
 */
esp_flash_err_t esp_flash_init(esp_flash_chip_t *chip);

/** @brief Read flash ID via the common "RDID" SPI flash command.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] Pointer to receive ID value.
 *
 * ID is a 24-bit value. Lower 16 bits of 'id' are the chip ID, upper 8 bits are the manufacturer ID.
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_read_id(const esp_flash_chip_t *chip, uint32_t *id);

/** @brief Detect flash size based on flash ID.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] Detected size in bytes.
 *
 * @note Most flash chips use a common format for flash ID, where the lower 4 bits specify the size as a power of 2. If
 * the manufacturer doesn't follow this convention, the size may be incorrectly detected.
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_detect_size(const esp_flash_chip_t *chip, uint32_t *size);

/** @brief Erase flash chip contents
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 *
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_erase_chip(const esp_flash_chip_t *chip);

/** @brief Erase a region of the flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param start Address to start erasing flash. Must be sector aligned.
 * @param len Length of region to erase. Must also be sector aligned.
 *
 * Sector size is specifyed in chip->drv->sector_size field (typically 4096 bytes.) FLASH_ERR_INVALID_ARG will be
 * returned if the start & length are not a multiple of this size.
 *
 * Erase is performed using block (multi-sector) erases where possible (block size is specified in
 * chip->drv->block_erase_size field, typically 65536 bytes). Remaining sectors are erased using individual sector erase
 * commands.
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_erase_region(const esp_flash_chip_t *chip, uint32_t start, uint32_t len);

/** @brief Read if the entire chip is write protected
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param[out] write_protected Pointer to boolean, set to the value of the write protect flag.
 *
 * @note A correct result for this flag depends on the SPI flash chip model and driver in use (via the 'chip->drv'
 * field).
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_get_chip_write_protect(const esp_flash_chip_t *chip, bool *write_protected);

/** @brief Set write protection for the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param write_protected Boolean value for the write protect flag
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and driver in use (via the 'chip->drv'
 * field).
 *
 * If write protection is enabled, destructive operations will fail with FLASH_ERR_PROTECTED.
 *
 * Some SPI flash chips may require a power cycle before write protect status can be cleared. Otherwise,
 * write protection can be removed via a follow-up call to this function.
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_set_chip_write_protect(const esp_flash_chip_t *chip, bool write_protect_chip);


/** @brief Read the list of individually protectable regions of this SPI flash chip.
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param regions[out] Pointer to receive a pointer to the array of protectable regions of the chip.
 * @param[out] Pointer to an integer receiving the count of protectable regions in the array returned in 'regions'.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and driver in use (via the 'chip->drv'
 * field).
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_get_protectable_regions(const esp_flash_chip_t *chip, const esp_flash_region_t **regions, uint32_t *num_regions);


/** @brief Detect if a region of the SPI flash chip is protected
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param region Pointer to a struct describing a protected region. This must match one of the regions returned from esp_flash_get_protectable_regions(...).
 * @param[out] protected Pointer to a flag which is set based on the protected status for this region.
 *
 * @note It is possible for this result to be false and write operations to still fail, if protection is enabled for the entire chip.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and driver in use (via the 'chip->drv'
 * field).
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_get_protected_region(const esp_flash_chip_t *chip, const esp_flash_region_t *region, bool *protected);

/** @brief Update the protected status for a region of the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param region Pointer to a struct describing a protected region. This must match one of the regions returned from esp_flash_get_protectable_regions(...).
 * @param protected Write protection flag to set.
 *
 * @note It is possible for the region protection flag to be cleared and write operations to still fail, if protection is enabled for the entire chip.
 *
 * @note Correct behaviour of this function depends on the SPI flash chip model and driver in use (via the 'chip->drv'
 * field).
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_set_protected_region(const esp_flash_chip_t *chip, const esp_flash_region_t *region, bool protected);

/** @brief Read data from the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param buffer Pointer to a buffer where the data will be read.
 * @param address Address on flash to read from. Must be less than chip->size field.
 * @param length Length (in bytes) of data to read.
 *
 * There are no alignment constraints on buffer, address or length.
 *
 * @note If on-chip flash encryption is used, this function returns raw (ie encrypted) data. Use the flash cache
 * to transparently decrypt data.
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_read(const esp_flash_chip_t *chip, void *buffer, uint32_t address, uint32_t length);

/** @brief Write data to the SPI flash chip
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param address Address on flash to write to. Must be previously erased (SPI NOR flash can only write bits 1->0).
 * @param buffer Pointer to a buffer with the data to write.
 * @param length Length (in bytes) of data to write.
 *
 * There are no alignment constraints on buffer, address or length.
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_write(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

/** @brief Encrypted and write data to the SPI flash chip using on-chip hardware flash encryption
 *
 * @param chip Pointer to identify flash chip. Must have been successfully initialised via esp_flash_init()
 * @param address Address on flash to write to. 16 byte aligned. Must be previously erased (SPI NOR flash can only write bits 1->0).
 * @param buffer Pointer to a buffer with the data to write.
 * @param length Length (in bytes) of data to write. 16 byte aligned.
 *
 * @note Both address & length must be 16 byte aligned, as this is the encryption block size
 *
 * @return FLASH_OK on success, or a flash error code if operation failed.
 */
esp_flash_err_t esp_flash_write_encrypted(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

/** @brief Pointer to the "default" SPI flash chip, ie the main chip attached to the MCU.

   This chip is used if the 'chip' argument pass to esp_flash_xxx API functions is ever NULL.
*/
extern const esp_flash_chip_t *esp_flash_default_chip;

/** @brief Initialise the default SPI flash chip
 *
 * Called by OS startup code. You do not need to call this in your own applications.
 */
esp_flash_err_t esp_flash_init_default_chip();

/** Enable OS-level SPI flash protections in IDF */
void esp_flash_low_level_app_init(); /* ROM TODO move this to IDF */
