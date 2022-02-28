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


void efuse_hal_get_mac(uint8_t *mac)
{
    *((uint32_t*)&mac[0]) = efuse_ll_get_mac0();
    *((uint16_t*)&mac[4]) = (uint16_t) efuse_ll_get_mac1();
}
