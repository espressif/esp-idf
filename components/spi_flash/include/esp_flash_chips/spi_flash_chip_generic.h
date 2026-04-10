/*
 * SPDX-FileCopyrightText: 2015-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include "esp_flash.h"
#include "esp_flash_chips/spi_flash_chip_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The 'chip_generic' SPI flash operations are a lowest common subset of SPI
 * flash commands, that work across most chips.
 *
 * These can be used as-is via the esp_flash_common_chip_driver chip_drv, or
 * they can be used as "base chip_drv" functions when creating a new
 * spi_flash_host_driver_t chip_drv structure.
 *
 * All of the functions in this header are internal functions, not part of a
 * public API. See esp_flash.h for the public API.
 */

/**
 * @brief Generic probe function
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param flash_id expected manufacture id.
 *
 * @return ESP_OK if the id read from chip->drv_read_id matches (always).
 */
esp_err_t spi_flash_chip_generic_probe(esp_flash_t *chip, uint32_t flash_id);

/**
 * @brief Generic reset function
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 *
 * @return ESP_OK if sending success, or error code passed from ``common_command`` or ``wait_idle`` functions of host driver.
 */
esp_err_t spi_flash_chip_generic_reset(esp_flash_t *chip);

/**
 * @brief Generic size detection function
 *
 * Tries to detect the size of chip by using the lower 4 bits of the chip->drv->read_id result = N, and assuming size is 2 ^ N.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param size Output of the detected size
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_FLASH_UNSUPPORTED_CHIP if the manufacturer id is not correct, which may means an error in the reading
 *      - or other error passed from the ``read_id`` function of host driver
 */
esp_err_t spi_flash_chip_generic_detect_size(esp_flash_t *chip, uint32_t *size);

/**
 * @brief Erase chip by using the generic erase chip command.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - or other error passed from the ``set_write_protect``, ``wait_idle`` or ``erase_chip`` function of host driver
 */
esp_err_t spi_flash_chip_generic_erase_chip(esp_flash_t *chip);

/**
 * @brief Erase sector by using the generic sector erase command.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param start_address Start address of the sector to erase
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - or other error passed from the ``set_write_protect``, ``wait_idle`` or ``erase_sector`` function of host driver
 */
esp_err_t spi_flash_chip_generic_erase_sector(esp_flash_t *chip, uint32_t start_address);

/**
 * @brief Erase block by the generic 64KB block erase command
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param start_address Start address of the block to erase
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - or other error passed from the ``set_write_protect``, ``wait_idle`` or ``erase_block`` function of host driver
 */
esp_err_t spi_flash_chip_generic_erase_block(esp_flash_t *chip, uint32_t start_address);

/**
 * @brief Read from flash by using a read command that matches the programmed
 * read mode.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param buffer Buffer to hold the data read from flash
 * @param address Start address of the data on the flash
 * @param length Length to read
 *
 * @return always ESP_OK currently
 */
esp_err_t spi_flash_chip_generic_read(esp_flash_t *chip, void *buffer, uint32_t address, uint32_t length);

/**
 * @brief Perform a page program using the page program command.
 *
 * @note Length of each call should not excced the limitation in
 * ``chip->host->max_write_bytes``. This function is called in
 * ``spi_flash_chip_generic_write`` recursively until the whole page is
 * programmed. Strongly suggest to call ``spi_flash_chip_generic_write``
 * instead.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param buffer Buffer holding the data to program
 * @param address Start address to write to flash
 * @param length Length to write, no longer than ``chip->host->max_write_bytes``.
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_NOT_SUPPORTED if the chip is not able to perform the operation. This is indicated by WREN = 1 after the command is sent.
 *      - or other error passed from the ``wait_idle`` or ``program_page`` function of host driver
 */
esp_err_t
spi_flash_chip_generic_page_program(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);

/**
 * @brief Perform a generic write. Split the write buffer into page program
 * operations, and call chip->chip_drv->page-program() for each.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param buffer Buffer holding the data to program
 * @param address Start address to write to flash
 * @param length Length to write
 *
 * @return
 *      - ESP_OK if success
 *      - or other error passed from the ``wait_idle`` or ``program_page`` function of host driver
  */
esp_err_t spi_flash_chip_generic_write(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);

/**
 * @brief Perform a write using on-chip flash encryption. Not implemented yet.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param buffer Buffer holding the data to program
 * @param address Start address to write to flash
 * @param length Length to write
 *
 * @return always ESP_ERR_FLASH_UNSUPPORTED_HOST.
 */
esp_err_t
spi_flash_chip_generic_write_encrypted(esp_flash_t *chip, const void *buffer, uint32_t address, uint32_t length);

/**
 * @brief Send the write enable or write disable command and verify the expected bit (1) in
 * the status register is set.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param write_protect true to enable write protection, false to send write enable.
 *
 * @return
 *      - ESP_OK if success
 *      - or other error passed from the ``wait_idle``, ``read_status`` or
 *        ``set_write_protect`` function of host driver
 */
esp_err_t spi_flash_chip_generic_set_write_protect(esp_flash_t *chip, bool write_protect);

/**
 * @brief Check whether WEL (write enable latch) bit is set in the Status Register read from RDSR.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param out_write_protect Output of whether the write protect is set.
 *
 * @return
 *      - ESP_OK if success
 *      - or other error passed from the ``read_status`` function of host driver
 */
esp_err_t spi_flash_chip_generic_get_write_protect(esp_flash_t *chip, bool *out_write_protect);

#define ESP_FLASH_CHIP_GENERIC_NO_TIMEOUT -1
/**
 * @brief Send commands to read one of the reg of the chip
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param reg_id     Type of the register to read
 * @param out_reg    Output of the register value
 * @return esp_err_t Error code passed from the ``read_status`` function of host driver.
 */
esp_err_t spi_flash_chip_generic_read_reg(esp_flash_t* chip, spi_flash_register_t reg_id, uint32_t* out_reg);

/**
 * @brief Read flash status via the RDSR command and wait for bit 0 (write in
 * progress bit) to be cleared.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param timeout_us Time to wait before timeout, in us.
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_TIMEOUT if not idle before timeout
 *      - or other error passed from the ``wait_idle`` or ``read_status`` function of host driver
 */
esp_err_t spi_flash_chip_generic_wait_idle(esp_flash_t *chip, uint32_t timeout_us);

/**
 * @brief Set the specified SPI read mode according to the data in the chip
 *        context. Set quad enable status register bit if needed.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 *
 * @return
 *      - ESP_OK if success
*      - ESP_ERR_TIMEOUT if not idle before timeout
 *      - or other error passed from the ``set_write_protect`` or ``common_command`` function of host driver
 */
esp_err_t spi_flash_chip_generic_set_io_mode(esp_flash_t *chip);

/**
  * Get whether the Quad Enable (QE) is set.
  *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param out_quad_mode Pointer to store the output mode.
 *          - SPI_FLASH_QOUT: QE is enabled
 *          - otherwise: QE is disabled
 *
 * @return
 *      - ESP_OK if success
 *      - or other error passed from the ``common_command`` function of host driver
  */
esp_err_t spi_flash_chip_generic_get_io_mode(esp_flash_t *chip, esp_flash_io_mode_t* out_quad_mode);

/**
 * @brief Read the chip unique ID.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param flash_unique_id Pointer to store output unique id.
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_NOT_SUPPORTED if the chip doesn't support read id.
 */
esp_err_t spi_flash_chip_generic_read_unique_id(esp_flash_t *chip, uint64_t* flash_unique_id);

/**
 * Generic SPI flash chip_drv, uses all the above functions for its operations.
 * In default autodetection, this is used as a catchall if a more specific
 * chip_drv is not found.
 */
extern const spi_flash_chip_t esp_flash_chip_generic;

/*******************************************************************************
 *  Utilities
*******************************************************************************/

/// Function pointer type for reading status register with QE bit.
typedef esp_err_t (*esp_flash_rdsr_func_t)(esp_flash_t* chip, uint32_t* out_sr);

/**
 * Use RDSR2 (35H) to read bit 15-8 of the SR, and RDSR (05H) to read bit 7-0.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param out_sr Pointer to buffer to hold the status register, 16 bits.
 *
 * @return ESP_OK if success, otherwise error code passed from the
 *         `common_command` function of the host driver.
 */
esp_err_t spi_flash_common_read_status_16b_rdsr_rdsr2(esp_flash_t* chip, uint32_t* out_sr);

/**
 * Use RDSR2 (35H) to read bit 15-8 of the SR.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param out_sr Pointer to buffer to hold the status register, 8 bits.
 *
 * @return ESP_OK if success, otherwise error code passed from the
 *         `common_command` function of the host driver.
 */
esp_err_t spi_flash_common_read_status_8b_rdsr2(esp_flash_t* chip, uint32_t* out_sr);

/**
 * Use RDSR (05H) to read bit 7-0 of the SR.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param out_sr Pointer to buffer to hold the status register, 8 bits.
 *
 * @return ESP_OK if success, otherwise error code passed from the
 *         `common_command` function of the host driver.
 */
esp_err_t spi_flash_common_read_status_8b_rdsr(esp_flash_t* chip, uint32_t* out_sr);

/// Function pointer type for writing status register with QE bit.
typedef esp_err_t (*esp_flash_wrsr_func_t)(esp_flash_t* chip, uint32_t sr);

/**
 * Use WRSR (01H) to write bit 7-0 of the SR.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param sr Value of the status register to write, 8 bits.
 *
 * @return ESP_OK if success, otherwise error code passed from the
 *         `common_command` function of the host driver.
 */
esp_err_t spi_flash_common_write_status_8b_wrsr(esp_flash_t* chip, uint32_t sr);

/**
 * Use WRSR (01H) to write bit 15-0 of the SR.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param sr Value of the status register to write, 16 bits.
 *
 * @return ESP_OK if success, otherwise error code passed from the
 *         `common_command` function of the host driver.
 */
esp_err_t spi_flash_common_write_status_16b_wrsr(esp_flash_t* chip, uint32_t sr);

/**
 * Use WRSR2 (31H) to write bit 15-8 of the SR.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param sr Value of the status register to write, 8 bits.
 *
 * @return ESP_OK if success, otherwise error code passed from the
 *         `common_command` function of the host driver.
 */
esp_err_t spi_flash_common_write_status_8b_wrsr2(esp_flash_t* chip, uint32_t sr);

/**
 * @brief Utility function for set_read_mode chip_drv function. If required,
 * set and check the QE bit in the flash chip to enable the QIO/QOUT mode.
 *
 * Most chip QE enable follows a common pattern, though commands to read/write
 * the status register may be different, as well as the position of QE bit.
 *
 * Registers to actually do Quad transitions and command to be sent in reading
 * should also be configured via
 * spi_flash_chip_generic_config_host_io_mode().
 *
 * Note that the bit length and qe position of wrsr_func, rdsr_func and
 * qe_sr_bit should be consistent.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param wrsr_func Function pointer for writing the status register
 * @param rdsr_func Function pointer for reading the status register
 * @param qe_sr_bit status with the qe bit only.
 *
 * @return always ESP_OK (currently).
 */
esp_err_t spi_flash_common_set_io_mode(esp_flash_t *chip, esp_flash_wrsr_func_t wrsr_func, esp_flash_rdsr_func_t rdsr_func, uint32_t qe_sr_bit);

/**
 * @brief Configure the host registers to use the specified read mode set in
 *        the ``chip->read_mode``.
 *
 * Usually called in chip_drv read() functions before actual reading
 * transactions. Also prepare the command to be sent in read functions.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param flags Special rules to configure io mode, (i.e. Whether 32 bit commands will be used (Currently only W25Q256 and GD25Q256 are supported))
 *
 * @return
 *      - ESP_OK if success
 *      - ESP_ERR_FLASH_NOT_INITIALISED if chip not initialized properly
 *      - or other error passed from the ``configure_host_mode`` function of host driver
 */
esp_err_t spi_flash_chip_generic_config_host_io_mode(esp_flash_t *chip, uint32_t flags);
#define SPI_FLASH_CONFIG_IO_MODE_32B_ADDR BIT(0)

/**
 * @brief Handle explicit yield requests
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @param wip  Write (erase) in progress, `true` if this function is called during waiting idle of a erase/write command; else `false`.
 * @return ESP_OK if success, otherwise failed.
 */
esp_err_t spi_flash_chip_generic_yield(esp_flash_t* chip, uint32_t wip);

/**
 * @brief Setup for flash suspend command configuration.
 *
 * @param chip Pointer to SPI flash chip to use. If NULL, esp_flash_default_chip is substituted.
 * @return ESP_OK
 */
esp_err_t spi_flash_chip_generic_suspend_cmd_conf(esp_flash_t *chip);

/**
 *
 * @brief Read the chip unique ID unsupported function.
 *
 * @param chip Pointer to SPI flash chip to use.
 * @param flash_unique_id Pointer to store output unique id (Although this function is an unsupported function, but the parameter should be kept for the consistence of the function pointer).
 * @return Always ESP_ERR_NOT_SUPPORTED.
 */
esp_err_t spi_flash_chip_generic_read_unique_id_none(esp_flash_t *chip, uint64_t* flash_unique_id);

/// Default timeout configuration used by most chips
extern const flash_chip_op_timeout_t spi_flash_chip_generic_timeout;

#ifdef __cplusplus
}
#endif
