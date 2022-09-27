/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>
#include "soc/soc.h"
#include "soc/lp_analog_peri_reg.h"

void bootloader_ana_bod_reset_config(bool enable)
{
    REG_CLR_BIT(LP_ANALOG_PERI_LP_ANA_FIB_ENABLE_REG, LP_ANALOG_PERI_LP_ANA_FIB_BOR_RST);
    if (enable) {
        REG_SET_BIT(LP_ANALOG_PERI_LP_ANA_BOD_MODE1_CNTL_REG, LP_ANALOG_PERI_LP_ANA_BOD_MODE1_RESET_ENA);
    } else {
        REG_CLR_BIT(LP_ANALOG_PERI_LP_ANA_BOD_MODE1_CNTL_REG, LP_ANALOG_PERI_LP_ANA_BOD_MODE1_RESET_ENA);
    }
}

void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    REG_CLR_BIT(LP_ANALOG_PERI_LP_ANA_FIB_ENABLE_REG, LP_ANALOG_PERI_LP_ANA_FIB_GLITCH_RST);
    if (enable) {
        REG_SET_BIT(LP_ANALOG_PERI_LP_ANA_CK_GLITCH_CNTL_REG, LP_ANALOG_PERI_LP_ANA_CK_GLITCH_RESET_ENA);
    } else {
        REG_CLR_BIT(LP_ANALOG_PERI_LP_ANA_CK_GLITCH_CNTL_REG, LP_ANALOG_PERI_LP_ANA_CK_GLITCH_RESET_ENA);
    }
}
