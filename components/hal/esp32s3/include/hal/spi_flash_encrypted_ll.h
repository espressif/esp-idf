// Copyright 2015-2021 Espressif Systems (Shanghai) PTE LTD
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
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash Encryption.

#include "soc/system_reg.h"
#include "soc/hwcrypto_reg.h"
#include "soc/soc.h"
#include "string.h"
#include "assert.h"
#include <stdbool.h>

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
    REG_SET_BIT(SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG,
                SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT |
                SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT);
}

/*
 * Disable the flash encryption mode.
 */
static inline void spi_flash_encrypt_ll_disable(void)
{
    REG_CLR_BIT(SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG,
                SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT);
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
    assert(type == FLASH_ENCRYPTION_MANU);
    REG_WRITE(AES_XTS_DESTINATION_REG, type);
}

/**
 * Configure the data size of a single encryption.
 *
 * @param block_size Size of the desired block.
 */
static inline void spi_flash_encrypt_ll_buffer_length(uint32_t size)
{
    // Desired block should not be larger than the block size.
    REG_WRITE(AES_XTS_SIZE_REG, size >> 5);
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
    memcpy((void *)(AES_XTS_PLAIN_BASE + plaintext_offs), buffer, size);
}

/**
 * Copy the flash address to XTS_AES physical address
 *
 * @param flash_addr flash address to write.
 */
static inline void spi_flash_encrypt_ll_address_save(uint32_t flash_addr)
{
    REG_WRITE(AES_XTS_PHYSICAL_ADDR_REG, flash_addr);
}

/**
 * Start flash encryption
 */
static inline void spi_flash_encrypt_ll_calculate_start(void)
{
    REG_WRITE(AES_XTS_TRIGGER_REG, 1);
}

/**
 * Wait for flash encryption termination
 */
static inline void spi_flash_encrypt_ll_calculate_wait_idle(void)
{
    while(REG_READ(AES_XTS_STATE_REG) == 0x1) {
    }
}

/**
 * Finish the flash encryption and make encrypted result accessible to SPI.
 */
static inline void spi_flash_encrypt_ll_done(void)
{
    REG_WRITE(AES_XTS_RELEASE_REG, 1);
    while(REG_READ(AES_XTS_STATE_REG) != 0x3) {
    }
}

/**
 * Set to destroy encrypted result
 */
static inline void spi_flash_encrypt_ll_destroy(void)
{
    REG_WRITE(AES_XTS_DESTROY_REG, 1);
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
