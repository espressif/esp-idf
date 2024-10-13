/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include <assert.h>
#include "soc/soc.h"
#include "soc/lp_analog_peri_reg.h"

//TODO: [ESP32C61] IDF-9260, commented in verify code, check

void bootloader_ana_super_wdt_reset_config(bool enable)
{
    //C61 doesn't support bypass super WDT reset
    assert(enable);
    // lp_analog_peri_reg.h updated, now following registers
    // REG_CLR_BIT(LP_ANALOG_PERI_LP_ANA_FIB_ENABLE_REG, LP_ANALOG_PERI_LP_ANA_FIB_SUPER_WDT_RST);
}

//Not supported but common bootloader calls the function. Do nothing
void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    (void)enable;
}
