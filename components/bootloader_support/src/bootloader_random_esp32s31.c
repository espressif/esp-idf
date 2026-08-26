/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "hal/rng_ll.h"

void bootloader_random_enable(void)
{
    rng_ll_enable();
}

void bootloader_random_disable(void)
{
    rng_ll_disable();
}
