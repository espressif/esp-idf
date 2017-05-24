#pragma once
#include "spi_flash_lowlevel.h"

/** @brief SPI flash driver definition structure.
 *
 * The driver structure contains chip-specific pointers to functions to perform SPI flash operations, and some
 * chip-specific numeric values.
 *
 * @note This is not a public API. Driver-specific functions are called from the public API (declared in
 * spi_flash_lowlevel.h). They assume the caller has already validated arguments and enabled relevant protections
 * (disabling flash cache, prevent concurrent SPI access, etc.)
 *
 * Do not call driver functions directly in other contexts.
 *
 * A generic driver and it's related operations are defined in spi_flash_lowlevel_generic.h which can be used as
 * building blocks for written new/specific SPI flash chip drivers.
 *
 * @note All of these functions may be called with SPI flash cache disabled, so must only ever access IRAM/DRAM/ROM.
 */
typedef struct esp_flash_driver {
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
     * @return FLASH_OK if probing was successful, an error otherwise. Driver may
     * assume that returning FLASH_OK means it has claimed this chip.
     */
    esp_flash_err_t (*probe)(esp_flash_chip_t *chip, uint32_t flash_id);

    /* Read SPI flash ID
     *
     * Sends RDID (or an equivalent command) to the device.
     */
    esp_flash_err_t (*read_id)(const esp_flash_chip_t *chip, uint32_t *id);

    /* Detect SPI flash size
     *
     * Interrogate the chip to detect it's size.
     */
    esp_flash_err_t (*detect_size)(const esp_flash_chip_t *chip, uint32_t *size);

    /* Erase the entire chip

       Caller has verified the chip is not write protected.
     */
    esp_flash_err_t (*erase_chip)(const esp_flash_chip_t *chip);

    /* Erase a sector of the chip. Sector size is specified in the 'sector_size' field.

       sector_address is an offset in bytes.

       Caller has verified that this sector should be non-write-protected.
     */
    esp_flash_err_t (*erase_sector)(const esp_flash_chip_t *chip, uint32_t sector_address);

    /* Erase a multi-sector block of the chip. Block size is specified in the 'block_erase_size' field.
       sector_address is an offset in bytes.

       Caller has verified that this block should be non-write-protected.
     */
    esp_flash_err_t (*erase_block)(const esp_flash_chip_t *chip, uint32_t block_address);

    uint32_t sector_size; /* Sector is minimum erase size */
    uint32_t block_erase_size; /* Optimal (fastest) block size for multi-sector erases on this chip */

    /* Read the write protect status of the entire chip. */
    esp_flash_err_t (*get_chip_write_protect)(const esp_flash_chip_t *chip, bool *write_protected);

    /* Set the write protect status of the entire chip. */
    esp_flash_err_t (*set_chip_write_protect)(const esp_flash_chip_t *chip, bool write_protect_chip);

    /* Number of individually write protectable regions on this chip. Range 0-63. */
    uint8_t num_protectable_regions;
    /* Pointer to an array describing each protectable region. Should have num_protectable_regions elements. */
    const esp_flash_region_t *protectable_regions;
    /* Get a bitmask describing all protectable regions on the chip. Each bit represents one entry in the
       protectable_regions array, ie bit (1<<N) is set then the region at array entry N is write protected. */
    esp_flash_err_t (*get_protected_regions)(const esp_flash_chip_t *chip, uint64_t *regions);

    /* Set protectable regions on the chip. Each bit represents on entry in the protectable regions array. */
    esp_flash_err_t (*set_protected_regions)(const esp_flash_chip_t *chip, uint64_t regions);

    /* Read data from the chip.
     *
     * Before calling this function, the caller will have called chip->drv->set_read_mode(chip) in order to configure the chip's read mode correctly.
     */
    esp_flash_err_t (*read)(const esp_flash_chip_t *chip, void *buffer, uint32_t address, uint32_t length);

    /* Write any amount of data to the chip.
     */
    esp_flash_err_t (*write)(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);


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
    esp_flash_err_t (*page_program)(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

    /* Page size as written by the page_program function. Usually 256 bytes. */
    uint32_t page_size;

    /* Perform an encrypted write to the chip, using internal flash encryption hardware. */
    esp_flash_err_t (*write_encrypted)(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

    /* Set the write enable flag. This function is called internally by other functions in this structure, before a destructive
       operation takes place. */
    esp_flash_err_t (*write_enable)(const esp_flash_chip_t *chip);

    /* Wait for the SPI flash chip to be idle (any write operation to be complete.) This function is both called from the higher-level API functions, and from other functions in this structure.

       timeout_ms should be a timeout (in milliseconds) before the function returns FLASH_ERR_NO_RESPONSE. This is useful to avoid hanging
       if the chip is otherwise unresponsive (ie returns all 0xFF or similar.)
    */
    esp_flash_err_t (*wait_idle)(const esp_flash_chip_t *chip, unsigned timeout_ms);

    /* Configure both the SPI host and the chip for the read mode specified in chip->read_mode.
     *
     * This function is called by the higher-level API before the 'read' function is called.
     *
     * Can return FLASH_ERR_UNSUPPORTED_HOST or FLASH_ERR_UNSUPPORTED_CHIP if the specified mode is unsupported.
     */
    esp_flash_err_t (*set_read_mode)(const esp_flash_chip_t *chip);
} esp_flash_driver_t;

/* Pointer to an array of pointers to all known drivers for flash chips. This array is used
   by esp_flash_init() to detect the flash chip driver, if none is supplied by the caller.

   Array is terminated with a NULL pointer.

   This pointer can be overwritten with a pointer to a new array, to update the list of known flash chips.
 */
extern const esp_flash_driver_t **esp_flash_registered_flash_drivers;

/* Provide OS-level integration hooks for accessing flash chips
   inside a running OS */
typedef struct
{
    /* Call this function before commencing any flash operation.

     Does not need to be recursive (ie is called at most once for each call to 'end'.
    */
    esp_flash_err_t (*start)(const esp_flash_chip_t *chip);

    /* Call this function after completing any flash operation. */
    esp_flash_err_t (*end)(const esp_flash_chip_t *chip);

    /* Delay for at least 'ms' milliseconds.

       This function will be called in between 'start' and 'end'.
     */
    esp_flash_err_t (*delay_ms)(unsigned ms);
} esp_flash_os_functions_t;

/* The default (ie initial boot) no-OS ROM esp_flash_os_functions_t */
const esp_flash_os_functions_t esp_flash_noos_functions;

/* Pointer to the current esp_flash_os_functions_t structure in use.
   Can be changed at runtime to reflect different running conditions.
 */
extern const esp_flash_os_functions_t *esp_flash_os_functions;
