/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "soc/lp_analog_peri_reg.h"

void bootloader_ana_super_wdt_reset_config(bool enable)
{
    //TODO: IDF-14656
}

void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    (void)enable;
}
