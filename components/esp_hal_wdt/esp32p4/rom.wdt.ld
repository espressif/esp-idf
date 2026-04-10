/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4.rom.wdt.ld for esp32p4
 *
 *
 * Generated from ./target/esp32p4/interface-esp32p4.yml md5sum 56d78222be1daa0502090a078288f4d5
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group hal_wdt
 ***************************************/

/* Functions */
wdt_hal_init = 0x4fc001fc;
wdt_hal_deinit = 0x4fc00200;
rom_wdt_hal_config_stage = 0x4fc00204;
wdt_hal_write_protect_disable = 0x4fc00208;
wdt_hal_write_protect_enable = 0x4fc0020c;
wdt_hal_enable = 0x4fc00210;
wdt_hal_disable = 0x4fc00214;
wdt_hal_handle_intr = 0x4fc00218;
wdt_hal_feed = 0x4fc0021c;
wdt_hal_set_flashboot_en = 0x4fc00220;
wdt_hal_is_enabled = 0x4fc00224;
