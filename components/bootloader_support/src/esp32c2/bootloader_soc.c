/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdbool.h>

void bootloader_ana_super_wdt_reset_config(bool enable)
{
    (void)enable; // ESP32-C2 has none of these features.
}

void bootloader_ana_bod_reset_config(bool enable)
{
    (void)enable; // ESP32-C2 has none of these features.
}

void bootloader_ana_clock_glitch_reset_config(bool enable)
{
    (void)enable; // ESP32-C2 has none of these features.
}
