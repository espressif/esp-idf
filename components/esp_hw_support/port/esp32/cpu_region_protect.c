/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "hal/mpu_hal.h"

void esp_cpu_configure_region_protection(void)
{
    /* Note: currently this is configured the same on all Xtensa targets
     *
     * Both chips have the address space divided into 8 regions, 512MB each.
     */
    const int illegal_regions[] = {0, 4, 5, 6, 7}; // 0x00000000, 0x80000000, 0xa0000000, 0xc0000000, 0xe0000000
    for (size_t i = 0; i < sizeof(illegal_regions) / sizeof(illegal_regions[0]); ++i) {
        mpu_hal_set_region_access(illegal_regions[i], MPU_REGION_ILLEGAL);
    }

    mpu_hal_set_region_access(1, MPU_REGION_RW); // 0x20000000
}
