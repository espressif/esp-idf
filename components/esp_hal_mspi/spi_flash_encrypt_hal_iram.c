/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// This part is put in iram.

#include "hal/spi_flash_encrypted_ll.h"
#include "soc/soc_caps.h"

void spi_flash_encryption_hal_enable(void)
{
    spi_flash_encrypt_ll_enable();
#if SOC_IS(ESP32S2)
    spi_flash_encrypt_ll_aes_accelerator_enable();
#endif //SOC_IS(ESP32S2)
#if !SOC_IS(ESP32)
    spi_flash_encrypt_ll_type(FLASH_ENCRYPTION_MANU);
#endif // !SOC_IS(ESP32)
}

void spi_flash_encryption_hal_disable(void)
{
    spi_flash_encrypt_ll_disable();
}

void spi_flash_encryption_hal_prepare(uint32_t address, const uint32_t* buffer, uint32_t size)
{
#if !SOC_IS(ESP32)
    spi_flash_encrypt_ll_buffer_length(size);
#endif // !SOC_IS(ESP32)
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
void spi_flash_encryption_hal_enable_pseudo_rounds(esp_xts_aes_psuedo_rounds_state_t mode, uint8_t base, uint8_t increment, uint8_t key_rng_cnt)
{
    if (spi_flash_encrypt_ll_is_pseudo_rounds_function_supported()) {
        spi_flash_encrypt_ll_enable_pseudo_rounds(mode, base, increment, key_rng_cnt);
    }
}
#endif /* SOC_FLASH_ENCRYPTION_XTS_AES_SUPPORT_PSEUDO_ROUND */
