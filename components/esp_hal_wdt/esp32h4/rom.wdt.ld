/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h4.rom.wdt.ld for esp32h4
 *
 *
 * Generated from ../../rom/target/esp32h4/interface-esp32h4.yml md5sum bb9ee317148d3c5b10610936a3675a65
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group hal_wdt
 ***************************************/

/* Functions */
wdt_hal_init = 0x40000340;
wdt_hal_deinit = 0x40000344;
wdt_hal_config_stage = 0x40000348;
wdt_hal_write_protect_disable = 0x4000034c;
wdt_hal_write_protect_enable = 0x40000350;
wdt_hal_enable = 0x40000354;
wdt_hal_disable = 0x40000358;
wdt_hal_handle_intr = 0x4000035c;
wdt_hal_feed = 0x40000360;
wdt_hal_set_flashboot_en = 0x40000364;
wdt_hal_is_enabled = 0x40000368;
