/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The ll is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The Lowlevel layer for SPI Flash Encryption.
#pragma once

#include <stdbool.h>
#include <string.h>
#include "soc/hp_system_reg.h"
#include "soc/xts_aes_reg.h"
#include "soc/soc.h"
#include "soc/soc_caps.h"
#include "hal/assert.h"

#include "hal/efuse_hal.h"
#include "soc/chip_revision.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Choose type of chip you want to encrypt manually
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
    REG_SET_BIT(HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG,
                HP_SYSTEM_ENABLE_DOWNLOAD_MANUAL_ENCRYPT |
                HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT);
}

/*
 * Disable the flash encryption mode.
 */
static inline void spi_flash_encrypt_ll_disable(void)
{
    REG_CLR_BIT(HP_SYSTEM_EXTERNAL_DEVICE_ENCRYPT_DECRYPT_CONTROL_REG,
                HP_SYSTEM_ENABLE_SPI_MANUAL_ENCRYPT);
}

/**
 * Choose type of chip you want to encrypt manually
 *
 * @param type The type of chip to be encrypted
 *
 * @note The hardware currently support flash encryption.
 */
static inline void spi_flash_encrypt_ll_type(flash_encrypt_ll_type_t type)
{
    // Our hardware only support flash encryption
    HAL_ASSERT(type == FLASH_ENCRYPTION_MANU);
    REG_SET_FIELD(XTS_AES_DESTINATION_REG(0), XTS_AES_DESTINATION, type);
}

/**
 * Configure the data size of a single encryption.
 *
 * @param block_size Size of the desired block.
 */
static inline void spi_flash_encrypt_ll_buffer_length(uint32_t size)
{
    // Desired block should not be larger than the block size.
    REG_SET_FIELD(XTS_AES_LINESIZE_REG(0), XTS_AES_LINESIZE, size >> 5);
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
    uint32_t plaintext_offs = (address % SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX);
    HAL_ASSERT(plaintext_offs + size <= SOC_FLASH_ENCRYPTED_XTS_AES_BLOCK_MAX);
    memcpy((void *)(XTS_AES_PLAIN_MEM(0) + plaintext_offs), buffer, size);
}

/**
 * Copy the flash address to XTS_AES physical address
 *
 * @param flash_addr flash address to write.
 */
static inline void spi_flash_encrypt_ll_address_save(uint32_t flash_addr)
{
    REG_SET_FIELD(XTS_AES_PHYSICAL_ADDRESS_REG(0), XTS_AES_PHYSICAL_ADDRESS, flash_addr);
}

/**
 * Start flash encryption
 */
static inline void spi_flash_encrypt_ll_calculate_start(void)
{
    REG_SET_FIELD(XTS_AES_TRIGGER_REG(0), XTS_AES_TRIGGER, 1);
}

/**
 * Wait for flash encryption termination
 */
static inline void spi_flash_encrypt_ll_calculate_wait_idle(void)
{
    while(REG_GET_FIELD(XTS_AES_STATE_REG(0), XTS_AES_STATE) == 0x1) {
    }
}

/**
 * Finish the flash encryption and make encrypted result accessible to SPI.
 */
static inline void spi_flash_encrypt_ll_done(void)
{
    REG_SET_BIT(XTS_AES_RELEASE_REG(0), XTS_AES_RELEASE);
    while(REG_GET_FIELD(XTS_AES_STATE_REG(0), XTS_AES_STATE) != 0x3) {
    }
}

/**
 * Set to destroy encrypted result
 */
static inline void spi_flash_encrypt_ll_destroy(void)
{
    REG_SET_BIT(XTS_AES_DESTROY_REG(0), XTS_AES_DESTROY);
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

/**
 * @brief Enable the pseudo-round function during XTS-AES operations
 *
 * @param mode set the mode for pseudo rounds, zero to disable, with increasing security upto three.
 * @param base basic number of pseudo rounds, zero if disable
 * @param increment increment number of pseudo rounds, zero if disable
 * @param key_rng_cnt update frequency of the pseudo-key, zero if disable
 */
static inline void spi_flash_encrypt_ll_enable_pseudo_rounds(uint8_t mode, uint8_t base, uint8_t increment, uint8_t key_rng_cnt)
{
    REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_MODE_PSEUDO, mode);

    if (mode) {
        REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_PSEUDO_BASE, base);
        REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_PSEUDO_INC, increment);
        REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_PSEUDO_RNG_CNT, key_rng_cnt);
    } else {
        REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_PSEUDO_BASE, 0);
        REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_PSEUDO_INC, 0);
        REG_SET_FIELD(XTS_AES_PSEUDO_ROUND_CONF_REG(0), XTS_AES_PSEUDO_RNG_CNT, 0);
    }
}

/**
 * @brief Check if the pseudo round function is supported
 * The XTS-AES pseudo round function is only avliable in chip version
 * above 1.2 in ESP32-H2
 */
static inline bool spi_flash_encrypt_ll_is_pseudo_rounds_function_supported(void)
{
    return ESP_CHIP_REV_ABOVE(efuse_hal_chip_revision(), 102);
}

#ifdef __cplusplus
}
#endif
