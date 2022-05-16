/*
 * SPDX-FileCopyrightText: 2019-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bootloader_common.h"
#include "bootloader_clock.h"
#include "soc/efuse_reg.h"
#include "soc/syscon_reg.h"

uint8_t bootloader_common_get_chip_revision(void)
{
    uint8_t eco_bit0, eco_bit1, eco_bit2;
    eco_bit0 = (REG_READ(EFUSE_BLK0_RDATA3_REG) & 0xF000) >> 15;
    eco_bit1 = (REG_READ(EFUSE_BLK0_RDATA5_REG) & 0x100000) >> 20;
    eco_bit2 = (REG_READ(SYSCON_DATE_REG) & 0x80000000) >> 31;
    uint32_t combine_value = (eco_bit2 << 2) | (eco_bit1 << 1) | eco_bit0;
    uint8_t chip_ver = 0;
    switch (combine_value) {
    case 0:
        chip_ver = 0;
        break;
    case 1:
        chip_ver = 1;
        break;
    case 3:
        chip_ver = 2;
        break;
#if CONFIG_IDF_ENV_FPGA
    case 4: /* Empty efuses, but SYSCON_DATE_REG bit is set */
        chip_ver = 3;
        break;
#endif
    case 7:
        chip_ver = 3;
        break;
    default:
        chip_ver = 0;
        break;
    }
    return chip_ver;
}

uint32_t bootloader_common_get_chip_ver_pkg(void)
{
    uint32_t pkg_version = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG);
    uint32_t pkg_version_4bit = REG_GET_FIELD(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_VER_PKG_4BIT);
    return (pkg_version_4bit << 3) | pkg_version;
}

int bootloader_clock_get_rated_freq_mhz()
{
    //Check if ESP32 is rated for a CPU frequency of 160MHz only
    if (REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_RATED) &&
        REG_GET_BIT(EFUSE_BLK0_RDATA3_REG, EFUSE_RD_CHIP_CPU_FREQ_LOW)) {
        return 160;
    }
    return 240;
}
