/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6.rom.coexist.ld for esp32c6
 *
 *
 * Generated from ./target/esp32c6/interface-esp32c6.yml md5sum 06c13e133e0743d09b87aba30d3e213b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
esp_coex_rom_version_get = 0x40000afc;
coex_bt_release = 0x40000b00;
coex_bt_request = 0x40000b04;
coex_core_ble_conn_dyn_prio_get = 0x40000b08;
/*coex_core_event_duration_get = 0x40000b0c;*/
coex_core_pti_get = 0x40000b10;
coex_core_release = 0x40000b14;
coex_core_request = 0x40000b18;
coex_core_status_get = 0x40000b1c;
/*coex_core_timer_idx_get = 0x40000b20;*/
coex_event_duration_get = 0x40000b24;
coex_hw_timer_disable = 0x40000b28;
coex_hw_timer_enable = 0x40000b2c;
coex_hw_timer_set = 0x40000b30;
coex_schm_interval_set = 0x40000b34;
coex_schm_lock = 0x40000b38;
coex_schm_unlock = 0x40000b3c;
coex_status_get = 0x40000b40;
coex_wifi_release = 0x40000b44;
esp_coex_ble_conn_dynamic_prio_get = 0x40000b48;
/* Data (.data, .bss, .rodata) */
coex_env_ptr = 0x4087ffc4;
coex_pti_tab_ptr = 0x4087ffc0;
coex_schm_env_ptr = 0x4087ffbc;
coexist_funcs = 0x4087ffb8;
g_coa_funcs_p = 0x4087ffb4;
g_coex_param_ptr = 0x4087ffb0;
