/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/***************************************
 Group hal_wdt
 ***************************************/

/* Functions */
wdt_hal_init = 0x40000370;
wdt_hal_deinit = 0x40000374;
wdt_hal_config_stage = 0x40000378;
wdt_hal_write_protect_disable = 0x4000037c;
wdt_hal_write_protect_enable = 0x40000380;
wdt_hal_enable = 0x40000384;
wdt_hal_disable = 0x40000388;
wdt_hal_handle_intr = 0x4000038c;
wdt_hal_feed = 0x40000390;
wdt_hal_set_flashboot_en = 0x40000394;
wdt_hal_is_enabled = 0x40000398;
