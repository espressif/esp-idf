/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/efuse_reg.h"

uint8_t bootloader_common_get_chip_revision(void)
{
    // should return the same value as esp_efuse_get_chip_ver()
    return REG_GET_FIELD(EFUSE_RD_MAC_SPI_SYS_3_REG, EFUSE_WAFER_VERSION);
}

uint32_t bootloader_common_get_chip_ver_pkg(void)
{
    // should return the same value as esp_efuse_get_pkg_ver()
    return REG_GET_FIELD(EFUSE_RD_MAC_SPI_SYS_3_REG, EFUSE_PKG_VERSION);
}
