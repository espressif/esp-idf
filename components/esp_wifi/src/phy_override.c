/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include "esp_attr.h"
#include "driver/adc.h"

/*
 * This file is used to override the hooks provided by the PHY lib for some system features.
 * Call phy_override() so that this file will be linked.
 */

static bool s_wifi_adc_xpd_flag;

void include_esp_phy_override(void)
{
    /* When this empty function is called, all functions below will be linked. */
}

/* Coordinate ADC power with other modules. */
// It seems that it is only required on ESP32, but we still compile it for all chips, in case it is
// called by PHY unexpectedly.
void set_xpd_sar(bool en)
{
    if (s_wifi_adc_xpd_flag == en) {
        /* ignore repeated calls to set_xpd_sar when the state is already correct */
        return;
    }

    s_wifi_adc_xpd_flag = en;
    if (en) {
        adc_power_acquire();
    } else {
        adc_power_release();
    }
}

//add spinlock protection
extern void regi2c_enter_critical(void);
extern void regi2c_exit_critical(void);

IRAM_ATTR void phy_i2c_enter_critical(void)
{
    regi2c_enter_critical();
}

IRAM_ATTR void phy_i2c_exit_critical(void)
{
    regi2c_exit_critical();
}
