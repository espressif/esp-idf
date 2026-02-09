/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.wdt.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group hal_wdt
 ***************************************/

/* Functions */
wdt_hal_init = 0x2f800368;
wdt_hal_deinit = 0x2f80036c;
wdt_hal_config_stage = 0x2f800370;
wdt_hal_write_protect_disable = 0x2f800374;
wdt_hal_write_protect_enable = 0x2f800378;
wdt_hal_enable = 0x2f80037c;
wdt_hal_disable = 0x2f800380;
wdt_hal_handle_intr = 0x2f800384;
wdt_hal_feed = 0x2f800388;
wdt_hal_set_flashboot_en = 0x2f80038c;
wdt_hal_is_enabled = 0x2f800390;
