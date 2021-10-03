/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>

#include "hal/cpu_hal.h"
#include "hal/mpu_hal.h"
#include "hal/mpu_types.h"
#include "soc/soc_caps.h"
#include "bootloader_mem.h"
#include "soc/cpu.h"

void bootloader_init_mem(void)
{
    cpu_hal_init_hwloop();

    // protect memory region
    esp_cpu_configure_region_protection();
}
