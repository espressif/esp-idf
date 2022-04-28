// Copyright 2021 Espressif Systems (Shanghai) CO LTD
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

#include "esp_rom_efuse.h"

static uint8_t esp_crc8(uint8_t const * p, uint32_t len)
{
    unsigned char i = 0;
    unsigned char crc = 0;

    while (len--) {
        crc = crc ^ (*p++);

        for (i = 0; i < 8; i++) {
            if ((crc) & 0x01) {
                crc = (crc >> 1) ^ (0x8c) ;
            } else {
                crc = crc >> 1;
            }
        }
    }

    return (crc);
}

uint8_t esp_rom_efuse_mac_address_crc8(const uint8_t *data, uint32_t len)
{
    return esp_crc8(data, len);
}

uint32_t esp_rom_efuse_get_flash_gpio_info(void)
{
    return 0;
}

uint32_t esp_rom_efuse_get_flash_wp_gpio(void)
{
    return 0;
}

bool esp_rom_efuse_is_secure_boot_enabled(void)
{
    return false;
}
