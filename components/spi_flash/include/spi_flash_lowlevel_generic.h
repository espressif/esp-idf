#pragma once
#include "spi_flash_lowlevel_driver.h"
/* The 'generic' SPI flash operations are a lowest common subset of SPI flash commands, that work across most chips.
 *
 * These can be used as-is vai the esp_flash_common_chip_driver driver, or they can be used as "base driver" functions when
 * creating a new esp_flash_driver_t driver structure.
 *
 *
 * All of the functions in this header are internal functions, not part of a public API. See spi_flash_lowlevel.h for
 * the public API.
 */

/* SPI commands (actual on-wire commands not SPI controller bitmasks)
   Suitable for use with spi_flash_common_command static function.
*/
#define CMD_RDID       0x9F
#define CMD_WRSR       0x01
#define CMD_WRSR2      0x31 /* Not all SPI flash uses this command */
#define CMD_WREN       0x06
#define CMD_WRDI       0x04
#define CMD_RDSR       0x05
#define CMD_RDSR2      0x35 /* Not all SPI flash uses this command */

#define CMD_FASTRD_QIO  0xEB
#define CMD_FASTRD_QUAD 0x6B
#define CMD_FASTRD_DIO  0xBB
#define CMD_FASTRD_DUAL 0x3B
#define CMD_FASTRD      0x0B
#define CMD_READ        0x03 /* Speed limited */

#define CMD_CHIP_ERASE 0xC7
#define CMD_SECTOR_ERASE 0x20
#define CMD_LARGE_BLOCK_ERASE 0xD8 /* 64KB block erase command */

#define SR_WIP  (1<<0) /* Status register write-in-progress bit */
#define SR_WREN (1<<1) /* Status register write enable bit */


/** @brief Execute a simple SPI flash command against the chip.
 *
 * @param chip Pointer to the chip to use.
 * @param command Command to execute (an on-wire hex command.)
 * @param mosi_data Up to 32 bits of MOSI data to write after the command.
 * @param mosi_len Length of MOSI data (in bits.)
 * @param miso_len Length of MISO data (in bits.)
 * @return MISO value read back, if any (depending on miso_len value.)
 */
uint32_t spi_flash_common_command(const esp_flash_chip_t *chip, uint8_t command, uint32_t mosi_data, uint8_t mosi_len, uint8_t miso_len);

/** @brief Returns true if the pin configuration for this chip uses the GPIO matrix for any signals. */
bool spi_flash_uses_gpio_matrix(const esp_flash_chip_t *chip);

/** @brief Generic probe function
 *
 * If chip->drv_read_id succeeds, the probe succeeds.
 */
esp_flash_err_t spi_flash_generic_probe(esp_flash_chip_t *chip, uint32_t flash_id);

/** @brief Generic implementation of esp_flash_driver_t->read_id
 *
 * Uses the RDID command (9Fh) supported by most SPI flash chips.
 *
 * Results of all-zeroes or all-ones are considered failures (probably no chip attached.)
 */
esp_flash_err_t spi_flash_generic_read_id(const esp_flash_chip_t *chip, uint32_t *id);

/** @brief Generic size detection function
 *
 * Tries to detect the size of chip by using the lower 4 bits of the chip->drv->read_id result = N, and assuming size is 2 ^ N.
 */
esp_flash_err_t spi_flash_generic_detect_size(const esp_flash_chip_t *chip, uint32_t *size);

/** @brief Erase chip by using the generic erase chip (C7h) command. */
esp_flash_err_t spi_flash_generic_erase_chip(const esp_flash_chip_t *chip);

/** @brief Erase sector by using the generic sector erase (20h) command. */
esp_flash_err_t spi_flash_generic_erase_sector(const esp_flash_chip_t *chip, uint32_t start_address);

/** @brief Erase block by using the generic 64KB block erase (D8h) command */
esp_flash_err_t spi_flash_generic_erase_block(const esp_flash_chip_t *chip, uint32_t start_address);

/** @brief Read from flash by using a read command that matches the programmed read mode. */
esp_flash_err_t spi_flash_generic_read(const esp_flash_chip_t *chip, void *buffer, uint32_t address, uint32_t length);

/** @brief Perform a page program using the page program (02h) command. */
esp_flash_err_t spi_flash_generic_page_program(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

/** @brief Perform a generic write. Split the write buffer into
    one page operations, and call chip->drv->page-program() for each.
*/
esp_flash_err_t spi_flash_generic_write(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

/** @brief Perform a write using on-chip flash encryption */
esp_flash_err_t spi_flash_generic_write_encrypted(const esp_flash_chip_t *chip, uint32_t address, const void *buffer, uint32_t length);

/** @brief Send the write enable (06h) command and verify the expected bit (1) in the status register is set. */
esp_flash_err_t spi_flash_generic_write_enable(const esp_flash_chip_t *chip);

/** @brief Wait for the SPI host hardware state machine to be idle.

    This isn't a flash driver operation, but it's called by spi_flash_generic_wait_idle() and may be useful when implementing alternative drivers.

    timeout_ms will be decremented if the function needs to wait until the host hardware is idle.
*/
esp_flash_err_t spi_flash_generic_wait_host_idle(const esp_flash_chip_t *chip, uint32_t *timeout_ms);

/** @brief Read flash status via the RDSR command (05h) and wait for bit 0 (write in progress bit) to be cleared. */
esp_flash_err_t spi_flash_generic_wait_idle(const esp_flash_chip_t *chip, uint32_t timeout_ms);

/** @brief Utility function to configure the SPI host hardware registers for the specified read mode.

    Called by spi_flash_generic_set_read_mode() but may also be useful
    when implementing other SPI flash drivers.

    Note that calling this configures SPI host registers, so if running any other commands as part of set_read_mode() then these must be run before calling this function.
 */
esp_flash_err_t spi_flash_common_configure_host_read_mode(const esp_flash_chip_t *chip);

/** @brief Utility function for set_read_mode driver function
 *
 * Most setting of read mode follows a common pattern, except for how to enable Quad I/O modes (QIO/QOUT).
 * These use different commands to read/write the status register, and a different bit is set/cleared.
 *
 * This is a generic utility function to implement set_read_mode() for this pattern. Also configures host
 * registers via spi_flash_common_configure_host_read_mode().
 *
 * @param qe_rdsr_command SPI flash command to read status register
 * @param qe_wrsr_command SPI flash command to write status register
 * @param qe_sr_bitwidth Width of the status register these commands operate on, in bits.
 * @param qe_sr_bit Bit mask for enabling Quad Enable functions on this chio.
 */
esp_flash_err_t spi_flash_common_set_read_mode(const esp_flash_chip_t *chip, uint8_t qe_rdsr_command, uint8_t qe_wrsr_command, uint8_t qe_sr_bitwidth, unsigned qe_sr_bit);

/** @brief Set the specified SPI read mode.
 *
 * Includes setting SPI host hardware registers, but also setting quad enable status register bit if needed.
 */
esp_flash_err_t spi_flash_generic_set_read_mode(const esp_flash_chip_t *chip);

/** @brief Returns true if chip is configured for Quad I/O or
    Quad Fast Read */
inline static bool spi_flash_is_quad_mode(const esp_flash_chip_t *chip)
{
    return chip->read_mode == ESP_FLASH_QIO || chip->read_mode == ESP_FLASH_QOUT;
}

/* Generic SPI flash driver, uses all the above functions for its operations.  In default autodetection, this is used as
   a catchall if a more specific driver is not found.
*/
extern const esp_flash_driver_t esp_flash_generic_chip_driver;

