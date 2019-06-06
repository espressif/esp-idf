// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <sys/param.h>

#include "esp_spi_flash.h"
#include "esp_spi_flash_chip.h"
#include "cache_utils.h"
#include "soc/system_reg.h"
#include "soc/soc_memory_layout.h"
#include "esp32s2beta/rom/spi_flash.h"
#include "esp32s2beta/rom/cache.h"

esp_rom_spiflash_result_t IRAM_ATTR spi_flash_write_encrypted_chip(size_t dest_addr, const void *src, size_t size)
{
    const spi_flash_guard_funcs_t *ops = spi_flash_guard_get();
    esp_rom_spiflash_result_t rc;

    if (!esp_ptr_internal(src)) {
        uint8_t block[128]; // Need to buffer in RAM as we write
        while (size > 0) {
            size_t next_block = MIN(size, sizeof(block));
            memcpy(block, src, next_block);

            esp_rom_spiflash_result_t r = spi_flash_write_encrypted_chip(dest_addr, block, next_block);
            if (r != ESP_ROM_SPIFLASH_RESULT_OK) {
                return r;
            }

            size -= next_block;
            dest_addr += next_block;
            src = ((uint8_t *)src) + next_block;
        }
        bzero(block, sizeof(block));

        return ESP_ROM_SPIFLASH_RESULT_OK;
    }
    else { // Already in internal memory
        rc = spi_flash_unlock();
        if (rc != ESP_ROM_SPIFLASH_RESULT_OK) {
            return rc;
        }

        if (ops && ops->start) {
            ops->start();
        }
        rc = SPI_Encrypt_Write(dest_addr, src, size);
        if (ops && ops->end) {
            ops->end();
        }
        return rc;
    }
}
