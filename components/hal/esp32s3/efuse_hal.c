/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/assert.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"

#define ESP_EFUSE_BLOCK_ERROR_BITS(error_reg, block) ((error_reg) & (0x0F << (4 * (block))))


//The wafer_major and MSB of wafer_minor fields was allocated to other purposes when block version is v1.1.
//Luckily only chip v0.0 have this kind of block version and efuse usage.
//This workaround fixes the issue.
static inline bool is_eco0(uint32_t minor_raw)
{
    return ((minor_raw & 0x7) == 0 &&
            efuse_ll_get_blk_version_major() == 1 && efuse_ll_get_blk_version_minor() == 1);
}

uint32_t efuse_hal_get_major_chip_version(void)
{
    uint32_t minor_raw = efuse_ll_get_chip_wafer_version_minor();

    if (is_eco0(minor_raw)) {
        return 0;
    }
    return efuse_ll_get_chip_wafer_version_major();
}

uint32_t efuse_hal_get_minor_chip_version(void)
{
    uint32_t minor_raw = efuse_ll_get_chip_wafer_version_minor();

    if (is_eco0(minor_raw)) {
        return 0;
    }
    return minor_raw;
}
