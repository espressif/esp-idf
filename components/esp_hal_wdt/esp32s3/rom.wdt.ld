/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
PROVIDE( wdt_hal_init = 0x40000dbc );
PROVIDE( wdt_hal_deinit = 0x40000dc8 );
PROVIDE( wdt_hal_config_stage = 0x40000dd4 );
PROVIDE( wdt_hal_write_protect_disable = 0x40000de0 );
PROVIDE( wdt_hal_write_protect_enable = 0x40000dec );
PROVIDE( wdt_hal_enable = 0x40000df8 );
PROVIDE( wdt_hal_disable = 0x40000e04 );
PROVIDE( wdt_hal_handle_intr = 0x40000e10 );
PROVIDE( wdt_hal_feed = 0x40000e1c );
PROVIDE( wdt_hal_set_flashboot_en = 0x40000e28 );
PROVIDE( wdt_hal_is_enabled = 0x40000e34 );
