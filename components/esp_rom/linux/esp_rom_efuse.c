/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

#if SOC_SPI_MEM_SUPPORT_OPI_MODE
uint32_t esp_rom_efuse_get_opiconfig(void)
{
    return 0;
}
#endif // SOC_SPI_MEM_SUPPORT_OPI_MODE

bool esp_rom_efuse_is_secure_boot_enabled(void)
{
    return false;
}
