/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

uint8_t bootloader_common_get_chip_revision(void)
{
    // should return the same value as esp_efuse_get_chip_ver()
    /* No other revisions for ESP32-S3 */
    return 0;
}

uint32_t bootloader_common_get_chip_ver_pkg(void)
{
    // should return the same value as esp_efuse_get_pkg_ver()
    return 0;
}
