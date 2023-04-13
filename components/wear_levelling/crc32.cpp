/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "crc32.h"
#include "esp_rom_crc.h"

unsigned int crc32::crc32_le(unsigned int crc, unsigned char const *buf, unsigned int len)
{
    return ::esp_rom_crc32_le(crc, buf, len);
}
