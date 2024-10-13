/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/***************************************
 Group hal_wdt
 ***************************************/

/* Functions */
wdt_hal_init = 0x400003a4;
wdt_hal_deinit = 0x400003a8;
wdt_hal_config_stage = 0x400003ac;
wdt_hal_write_protect_disable = 0x400003b0;
wdt_hal_write_protect_enable = 0x400003b4;
wdt_hal_enable = 0x400003b8;
wdt_hal_disable = 0x400003bc;
wdt_hal_handle_intr = 0x400003c0;
wdt_hal_feed = 0x400003c4;
wdt_hal_set_flashboot_en = 0x400003c8;
wdt_hal_is_enabled = 0x400003cc;
