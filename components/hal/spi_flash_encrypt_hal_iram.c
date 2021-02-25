// Copyright 2021 Espressif Systems (Shanghai) PTE LTD
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

// This part is put in iram.

#include "hal/spi_flash_encrypted_ll.h"

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
