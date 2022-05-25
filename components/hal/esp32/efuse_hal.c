/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/efuse_ll.h"
#include "hal/assert.h"
#include "hal/efuse_hal.h"
#include "soc/syscon_reg.h"

uint32_t efuse_hal_get_major_chip_version(void)
{
    uint8_t eco_bit0 = efuse_ll_get_chip_ver_rev1();
    uint8_t eco_bit1 = efuse_ll_get_chip_ver_rev2();
    uint8_t eco_bit2 = (REG_READ(SYSCON_DATE_REG) & 0x80000000) >> 31;
    uint32_t combine_value = (eco_bit2 << 2) | (eco_bit1 << 1) | eco_bit0;
    uint32_t chip_ver = 0;
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
#endif // CONFIG_IDF_ENV_FPGA
    case 7:
        chip_ver = 3;
        break;
    default:
        chip_ver = 0;
        break;
    }
    return chip_ver;
}

uint32_t efuse_hal_get_minor_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_minor();
}

uint32_t efuse_hal_get_rated_freq_mhz(void)
{
    //Check if ESP32 is rated for a CPU frequency of 160MHz only
    if (efuse_ll_get_chip_cpu_freq_rated() && efuse_ll_get_chip_cpu_freq_low()) {
        return 160;
    }
    return 240;
}
