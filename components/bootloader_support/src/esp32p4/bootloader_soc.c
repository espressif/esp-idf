/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "soc/lp_analog_peri_reg.h"
<<<<<<< HEAD
#include "soc/soc.h"
#include "hal/brownout_ll.h"
=======
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade

void bootloader_ana_super_wdt_reset_config(bool enable)
{
    //TODO: IDF-7514
}

<<<<<<< HEAD
void bootloader_ana_bod_reset_config(bool enable)
{
    REG_CLR_BIT(LP_ANALOG_PERI_FIB_ENABLE_REG, LP_ANALOG_PERI_LP_ANA_FIB_BOD_RST);

    brownout_ll_ana_reset_enable(enable);
}

=======
>>>>>>> a97a7b0962da148669bb333ff1f30bf272946ade
void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    //TODO: IDF-7514
}
