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

#include "soc/dport_reg.h"
#include "soc/flash_encryption_reg.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enable encryption in flash
 */
static inline void spi_flash_encrypt_ll_enable(void)
{
    DPORT_REG_SET_BIT(DPORT_SLAVE_SPI_CONFIG_REG, DPORT_SLAVE_SPI_MASK_PRO | DPORT_SPI_ENCRYPT_ENABLE);
}

/**
 * Disable encryption in flash
 */
static inline void spi_flash_encrypt_ll_disable(void)
{
    DPORT_REG_CLR_BIT(DPORT_SLAVE_SPI_CONFIG_REG, DPORT_SLAVE_SPI_MASK_PRO | DPORT_SPI_ENCRYPT_ENABLE);
}

/**
 * Copy the flash address to physical address
 *
 * @param flash_addr The flash address.
 *
 * @note the address must be 8-byte aligned
 */
static inline void spi_flash_encrypt_ll_address_save(uint32_t flash_addr)
{
    REG_WRITE(FLASH_ENCRYPTION_ADDRESS_REG, flash_addr);
}

/**
 * Wait for flash encryption operation completeness.
 */
static inline void spi_flash_encrypt_ll_calculate_wait_idle(void)
{
    while(!(REG_READ(FLASH_ENCRYPTION_DONE_REG) & BIT(0))) {
    }
}

/**
 * Start encryption on data buffer.
 */
static inline void spi_flash_encrypt_ll_calculate_start(void)
{
    REG_WRITE(FLASH_ENCRYPTION_START_REG, BIT(0));
}

/**
 * Save the plaintext for encryption
 *
 * @param address address of the partition to be written.
 * @param buffer Buffer to store the input data.
 * @param size Buffer size.
 */
static inline void spi_flash_encrypt_ll_plaintext_save(uint32_t address, const uint32_t* buffer, uint32_t size)
{
    for (int i = 0; i < 8; i++) {
        REG_WRITE(FLASH_ENCRYPTION_BUFFER_REG + (i << 2), buffer[i]);
    }
}

/**
 * Finish the flash encryption and make encrypted result accessible to SPI.
 */
static inline void spi_flash_encrypt_ll_done(void)
{
    // Do nothing on ESP32
}

/**
 * Set to destroy encrypted result
 */
static inline void spi_flash_encrypt_ll_destroy(void)
{
    // Do nothing on ESP32
}

/**
 * Check if is qualified to encrypt the buffer
 *
 * @param address the address of written flash partition.
 * @param length Buffer size.
 */
static inline bool spi_flash_encrypt_ll_check(uint32_t address, uint32_t length)
{
    return ((address % 16) == 0) ? true : false;
}

#ifdef __cplusplus
}
#endif
