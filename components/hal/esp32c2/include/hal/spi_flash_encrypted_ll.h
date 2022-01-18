/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash Encryption.

#include <stdbool.h>
#include <string.h>
#include "soc/system_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/soc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Choose type of chip you want to encrypt manully
typedef enum
{
    FLASH_ENCRYPTION_MANU = 0, ///!< Manually encrypt the flash chip.
    PSRAM_ENCRYPTION_MANU = 1  ///!< Manually encrypt the psram chip.
} flash_encrypt_ll_type_t;

/**
 * Enable the flash encryption function under spi boot mode and download boot mode.
 */
static inline void spi_flash_encrypt_ll_enable(void)
{
    abort();
}

/*
 * Disable the flash encryption mode.
 */
static inline void spi_flash_encrypt_ll_disable(void)
{
    abort();
}

/**
 * Choose type of chip you want to encrypt manully
 *
 * @param type The type of chip to be encrypted
 *
 * @note The hardware currently support flash encryption.
 */
static inline void spi_flash_encrypt_ll_type(flash_encrypt_ll_type_t type)
{
    abort();
}

/**
 * Configure the data size of a single encryption.
 *
 * @param block_size Size of the desired block.
 */
static inline void spi_flash_encrypt_ll_buffer_length(uint32_t size)
{
    abort();
}

/**
 * Save 32-bit piece of plaintext.
 *
 * @param address the address of written flash partition.
 * @param buffer Buffer to store the input data.
 * @param size Buffer size.
 *
 */
static inline void spi_flash_encrypt_ll_plaintext_save(uint32_t address, const uint32_t* buffer, uint32_t size)
{
    abort();
}

/**
 * Copy the flash address to XTS_AES physical address
 *
 * @param flash_addr flash address to write.
 */
static inline void spi_flash_encrypt_ll_address_save(uint32_t flash_addr)
{
    abort();
}

/**
 * Start flash encryption
 */
static inline void spi_flash_encrypt_ll_calculate_start(void)
{
    abort();
}

/**
 * Wait for flash encryption termination
 */
static inline void spi_flash_encrypt_ll_calculate_wait_idle(void)
{
    abort();
}

/**
 * Finish the flash encryption and make encrypted result accessible to SPI.
 */
static inline void spi_flash_encrypt_ll_done(void)
{
    abort();
}

/**
 * Set to destroy encrypted result
 */
static inline void spi_flash_encrypt_ll_destroy(void)
{
    abort();
}

/**
 * Check if is qualified to encrypt the buffer
 *
 * @param address the address of written flash partition.
 * @param length Buffer size.
 */
static inline bool spi_flash_encrypt_ll_check(uint32_t address, uint32_t length)
{
    abort();
}

#ifdef __cplusplus
}
#endif
