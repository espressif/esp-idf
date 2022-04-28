/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_spi_flash.h"
#include "esp32/rom/cache.h"
#include "esp_rom_spiflash.h"

static inline void IRAM_ATTR spi_flash_guard_start(void)
{
    const spi_flash_guard_funcs_t *ops = spi_flash_guard_get();
    if (ops && ops->start) {
        ops->start();
    }
}

static inline void IRAM_ATTR spi_flash_guard_end(void)
{
    const spi_flash_guard_funcs_t *ops = spi_flash_guard_get();
    if (ops && ops->end) {
        ops->end();
    }
}

extern void IRAM_ATTR flash_rom_init(void);
esp_rom_spiflash_result_t IRAM_ATTR spi_flash_write_encrypted_chip(size_t dest_addr, const void *src, size_t size)
{
    const uint8_t *ssrc = (const uint8_t *)src;
    esp_rom_spiflash_result_t rc = ESP_ROM_SPIFLASH_RESULT_OK;

    assert((dest_addr % 16) == 0);
    assert((size % 16) == 0);

    /* esp_rom_spiflash_write_encrypted encrypts data in RAM as it writes,
       so copy to a temporary buffer - 32 bytes at a time.

       Each call to esp_rom_spiflash_write_encrypted takes a 32 byte "row" of
       data to encrypt, and each row is two 16 byte AES blocks
       that share a key (as derived from flash address).
    */
    uint8_t encrypt_buf[32] __attribute__((aligned(4)));
    uint32_t row_size;
    for (size_t i = 0; i < size; i += row_size) {
        uint32_t row_addr = dest_addr + i;
        if (i == 0 && (row_addr % 32) != 0) {
            /* writing to second block of a 32 byte row */
            row_size = 16;
            row_addr -= 16;
            /* copy to second block in buffer */
            memcpy(encrypt_buf + 16, ssrc + i, 16);
            /* decrypt the first block from flash, will reencrypt to same bytes */
            spi_flash_read_encrypted(row_addr, encrypt_buf, 16);
        } else if (size - i == 16) {
            /* 16 bytes left, is first block of a 32 byte row */
            row_size = 16;
            /* copy to first block in buffer */
            memcpy(encrypt_buf, ssrc + i, 16);
            /* decrypt the second block from flash, will reencrypt to same bytes */
            spi_flash_read_encrypted(row_addr + 16, encrypt_buf + 16, 16);
        } else {
            /* Writing a full 32 byte row (2 blocks) */
            row_size = 32;
            memcpy(encrypt_buf, ssrc + i, 32);
        }

        spi_flash_guard_start();
        flash_rom_init();
        rc = esp_rom_spiflash_write_encrypted(row_addr, (uint32_t *)encrypt_buf, 32);
        spi_flash_guard_end();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            break;
        }
    }
    bzero(encrypt_buf, sizeof(encrypt_buf));

    return rc;
}
