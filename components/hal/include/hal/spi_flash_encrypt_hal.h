/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*******************************************************************************
 * NOTICE
 * The HAL is not public api, don't use in application code.
 * See readme.md in hal/include/hal/readme.md
 ******************************************************************************/

// The HAL layer for SPI Flash Encryption

#include "hal/spi_flash_encrypted_ll.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable the flash encryption
 */
void spi_flash_encryption_hal_enable(void);

/**
 * @brief Disable the flash encryption
 */
void spi_flash_encryption_hal_disable(void);

/**
 * Prepare flash encryption before operation.
 *
 * @param address The destination address in flash for the write operation.
 * @param buffer Data for programming
 * @param size Size to program.
 *
 * @note address and buffer must be 8-word aligned.
 */
void spi_flash_encryption_hal_prepare(uint32_t address, const uint32_t* buffer, uint32_t size);

/**
 * @brief flash data encryption operation is done.
 */
void spi_flash_encryption_hal_done(void);

/**
 * Destroy encrypted result
 */
void spi_flash_encryption_hal_destroy(void);

/**
 * Check if is qualified to encrypt the buffer
 *
 * @param address the address of written flash partition.
 * @param length Buffer size.
 */
bool spi_flash_encryption_hal_check(uint32_t address, uint32_t length);

#ifdef __cplusplus
}
#endif
