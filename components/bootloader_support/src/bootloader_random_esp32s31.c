/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "sdkconfig.h"
#include "bootloader_random.h"
#include "hal/trng_ll.h"

void bootloader_random_enable(void)
{
    trng_ll_enable();
}

void bootloader_random_disable(void)
{
    trng_ll_disable();
}
