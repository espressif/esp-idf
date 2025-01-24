/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This part is put in iram.

#include "hal/spi_flash_encrypted_ll.h"
#include "soc/soc_caps.h"

void spi_flash_encryption_hal_enable(void)
{
    spi_flash_encrypt_ll_enable();
#if CONFIG_IDF_TARGET_ESP32S2
    spi_flash_encrypt_ll_aes_accelerator_enable();
#endif //CONFIG_IDF_TARGET_ESP32S2
#if !CONFIG_IDF_TARGET_ESP32
    spi_flash_encrypt_ll_type(FLASH_ENCRYPTION_MANU);
#endif // !CONFIG_IDF_TARGET_ESP32
}

void spi_flash_encryption_hal_disable(void)
{
    spi_flash_encrypt_ll_disable();
}

void spi_flash_encryption_hal_prepare(uint32_t address, const uint32_t* buffer, uint32_t size)
{
#if !CONFIG_IDF_TARGET_ESP32
    spi_flash_encrypt_ll_buffer_length(size);
#endif // !CONFIG_IDF_TARGET_ESP32
    spi_flash_encrypt_ll_address_save(address);
    spi_flash_encrypt_ll_plaintext_save(address, buffer, size);
    spi_flash_encrypt_ll_calculate_start();
}

void spi_flash_encryption_hal_done(void)
{
    spi_flash_encrypt_ll_calculate_wait_idle();
    spi_flash_encrypt_ll_done();
}

void spi_flash_encryption_hal_destroy(void)
{
    spi_flash_encrypt_ll_destroy();
}

bool spi_flash_encryption_hal_check(uint32_t address, uint32_t length)
{
    return spi_flash_encrypt_ll_check(address, length);
}

#ifdef SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND
void spi_flash_encryption_hal_enable_pseudo_rounds(uint8_t mode, uint8_t base, uint8_t increment, uint8_t key_rng_cnt)
{
    if (spi_flash_encrypt_ll_is_pseudo_rounds_function_supported()) {
        spi_flash_encrypt_ll_enable_pseudo_rounds(mode, base, increment, key_rng_cnt);
    }
}
#endif /* SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND */
