/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include <sys/param.h>
#include "soc/soc_caps.h"
#include "hal/efuse_hal.h"
#include "hal/efuse_ll.h"

uint32_t efuse_hal_get_major_chip_version(void)
{
    uint32_t ret = efuse_ll_get_chip_wafer_version_major();
    //Workaround: The major version field was allocated to other purposes when block version is v1.1.
    //Luckily only chip v0.0 have this kind of block version and efuse usage.
    if (efuse_ll_get_chip_wafer_version_minor() == 0 &&
        efuse_ll_get_blk_version_major() == 1 &&
        efuse_ll_get_blk_version_minor() == 1) {
        ret = 0;
    }
    return ret;
}

uint32_t efuse_hal_get_minor_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_minor();
}
