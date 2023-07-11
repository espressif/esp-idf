/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
#include "soc/hp_system_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/soc.h"
#include "hal/assert.h"

#ifdef __cplusplus
extern "C" {
#endif

//TODO: IDF-7545

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
    // REG_SET_BIT(HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG,
    //             HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT |
    //             HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT);
    abort();
}

/*
 * Disable the flash encryption mode.
 */
static inline void spi_flash_encrypt_ll_disable(void)
{
    // REG_CLR_BIT(HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG,
    //             HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT);
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
    // Our hardware only support flash encryption
    HAL_ASSERT(type == FLASH_ENCRYPTION_MANU);
    REG_SET_FIELD(SPI_MEM_XTS_DESTINATION_REG(0), SPI_MEM_SPI_XTS_DESTINATION, type);
}

/**
 * Configure the data size of a single encryption.
 *
 * @param block_size Size of the desired block.
 */
static inline void spi_flash_encrypt_ll_buffer_length(uint32_t size)
{
    // Desired block should not be larger than the block size.
    REG_SET_FIELD(SPI_MEM_XTS_LINESIZE_REG(0), SPI_MEM_SPI_XTS_LINESIZE, size >> 5);
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
    uint32_t plaintext_offs = (address % 64);
    memcpy((void *)(SPI_MEM_XTS_PLAIN_BASE_REG(0) + plaintext_offs), buffer, size);
}

/**
 * Copy the flash address to XTS_AES physical address
 *
 * @param flash_addr flash address to write.
 */
static inline void spi_flash_encrypt_ll_address_save(uint32_t flash_addr)
{
    REG_SET_FIELD(SPI_MEM_XTS_PHYSICAL_ADDRESS_REG(0), SPI_MEM_SPI_XTS_PHYSICAL_ADDRESS, flash_addr);
}

/**
 * Start flash encryption
 */
static inline void spi_flash_encrypt_ll_calculate_start(void)
{
    REG_SET_FIELD(SPI_MEM_XTS_TRIGGER_REG(0), SPI_MEM_SPI_XTS_TRIGGER, 1);
}

/**
 * Wait for flash encryption termination
 */
static inline void spi_flash_encrypt_ll_calculate_wait_idle(void)
{
    while(REG_GET_FIELD(SPI_MEM_XTS_STATE_REG(0), SPI_MEM_SPI_XTS_STATE) == 0x1) {
    }
}

/**
 * Finish the flash encryption and make encrypted result accessible to SPI.
 */
static inline void spi_flash_encrypt_ll_done(void)
{
    REG_SET_BIT(SPI_MEM_XTS_RELEASE_REG(0), SPI_MEM_SPI_XTS_RELEASE);
    while(REG_GET_FIELD(SPI_MEM_XTS_STATE_REG(0), SPI_MEM_SPI_XTS_STATE) != 0x3) {
    }
}

/**
 * Set to destroy encrypted result
 */
static inline void spi_flash_encrypt_ll_destroy(void)
{
    REG_SET_BIT(SPI_MEM_XTS_DESTROY_REG(0), SPI_MEM_SPI_XTS_DESTROY);
}

/**
 * Check if is qualified to encrypt the buffer
 *
 * @param address the address of written flash partition.
 * @param length Buffer size.
 */
static inline bool spi_flash_encrypt_ll_check(uint32_t address, uint32_t length)
{
    return ((address % length) == 0) ? true : false;
}

#ifdef __cplusplus
}
#endif
