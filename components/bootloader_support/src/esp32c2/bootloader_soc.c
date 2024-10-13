/*
 * SPDX-FileCopyrightText: 2019-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "soc/rtc_cntl_reg.h"

void bootloader_ana_super_wdt_reset_config(bool enable)
{
    REG_CLR_BIT(RTC_CNTL_FIB_SEL_REG, RTC_CNTL_FIB_SUPER_WDT_RST);

    if (enable) {
        REG_CLR_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_BYPASS_RST);
    } else {
        REG_SET_BIT(RTC_CNTL_SWD_CONF_REG, RTC_CNTL_SWD_BYPASS_RST);
    }
}

//Not supported but common bootloader calls the function. Do nothing
void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    (void)enable;
}
