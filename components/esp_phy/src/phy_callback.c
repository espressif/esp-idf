/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

typedef enum {
    OK = 0,
    FAIL,
    PENDING,
    BUSY,
    CANCEL,
} STATUS;

extern int phy_printf(const char* format, ...);

static uint8_t g_rf_cmdstop = 3;

void esp_phy_test_start_stop(uint8_t value)
{
    g_rf_cmdstop = value;
}

int esp_phy_cmdstop_callback(void)
{
    return g_rf_cmdstop;
}

STATUS esp_phy_getstopcmd(void)
{
    uint8_t value = esp_phy_cmdstop_callback();
    if (value == 0) {
        return OK;
    } else if (value == 1) {
        return BUSY;
    } else if (value == 2) {
        phy_printf("Please run cmdstop to exit current cmd!\n");
        return FAIL;
    } else {
        return FAIL;
    }
}
