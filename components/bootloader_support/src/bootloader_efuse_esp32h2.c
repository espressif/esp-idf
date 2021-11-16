/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "soc/efuse_reg.h"

uint8_t bootloader_common_get_chip_revision(void)
{
#if IDF_TARGET_ESP32H2_BETA_VERSION_1 // TODO: IDF-4337
    // should return the same value as esp_efuse_get_chip_ver()
    return REG_GET_FIELD(EFUSE_RD_MAC_SPI_SYS_3_REG, EFUSE_WAFER_VERSION);
#else
    return 0;
#endif
}

uint32_t bootloader_common_get_chip_ver_pkg(void)
{
#if IDF_TARGET_ESP32H2_BETA_VERSION_1 // TODO: IDF-4337
    // should return the same value as esp_efuse_get_pkg_ver()
    return REG_GET_FIELD(EFUSE_RD_MAC_SPI_SYS_3_REG, EFUSE_PKG_VERSION);
#else
    return 0;
#endif
}
