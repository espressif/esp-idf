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

uint32_t efuse_hal_get_major_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_major();
}

uint32_t efuse_hal_get_minor_chip_version(void)
{
    return efuse_ll_get_chip_wafer_version_minor();
}
