/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
PROVIDE( wdt_hal_init = 0x4000027c );
PROVIDE( wdt_hal_deinit = 0x40000280 );
PROVIDE( wdt_hal_config_stage = 0x40000284 );
PROVIDE( wdt_hal_write_protect_disable = 0x40000288 );
PROVIDE( wdt_hal_write_protect_enable = 0x4000028c );
PROVIDE( wdt_hal_enable = 0x40000290 );
PROVIDE( wdt_hal_disable = 0x40000294 );
PROVIDE( wdt_hal_handle_intr = 0x40000298 );
PROVIDE( wdt_hal_feed = 0x4000029c );
PROVIDE( wdt_hal_set_flashboot_en = 0x400002a0 );
PROVIDE( wdt_hal_is_enabled = 0x400002a4 );
