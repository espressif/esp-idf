/*
 * SPDX-FileCopyrightText: 2020-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/rtc.h"

#ifdef BOOTLOADER_BUILD

int esp_clk_apb_freq(void)
{
    return rtc_clk_apb_freq_get();
}

#endif // BOOTLOADER_BUILD
