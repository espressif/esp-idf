/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_rom_crc.h"
#include "ble_ots_common.h"

uint32_t ble_ots_checksum_calculate(const uint8_t *data, uint32_t offset, uint32_t length)
{
    /* esp_rom_crc32_le() inverts the CRC register on entry and on exit, so the
     * CRC-32 flavour OTS requires (init 0xFFFFFFFF, refin/refout enabled,
     * xorout 0xFFFFFFFF) is obtained by seeding with ~0xFFFFFFFF == 0 and
     * returning the value unchanged. A zero-length range yields 0, as before. */
    return esp_rom_crc32_le(0, data + offset, length);
}
