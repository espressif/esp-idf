/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/soc.h"
#include "soc/efuse_reg.h"

#if CONFIG_IDF_TARGET_ESP32S3
/**
 * Since rom of esp32s3 does not export function ets_efuse_get_opiconfig,
 * patch this function here.
 */
uint32_t esp_rom_efuse_get_opiconfig(void)
{
    uint64_t spiconfig1 = REG_GET_FIELD(EFUSE_RD_MAC_SPI_SYS_2_REG, EFUSE_SPI_PAD_CONF_1);
    uint64_t spiconfig2 = REG_GET_FIELD(EFUSE_RD_MAC_SPI_SYS_3_REG, EFUSE_SPI_PAD_CONF_2);
    uint64_t opiconfig = (spiconfig2 << 12) | (spiconfig1 >> 20);
    if (opiconfig == 0 || opiconfig == 0x3fffffffllu) {
        return 0;
    }
    // （MSB）EFUSE_SPI_PAD_CONF_2(18bit) + EFUSE_SPI_PAD_CONF_1(32bit) + EFUSE_SPI_PAD_CONF_0(16bit) (LSB)
    // [36:41] -- DQS
    // [42:47] -- D4
    // [48:53] -- D5
    // [54:59] -- D6
    // [60:65] -- D7
    return opiconfig & 0x3fffffff;
}
#endif
