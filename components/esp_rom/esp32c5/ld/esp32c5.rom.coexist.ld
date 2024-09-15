/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.coexist.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum f5c146321f24f88ad1f27234da5aed11
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
esp_coex_rom_version_get = 0x40000ac8;
coex_bt_release = 0x40000acc;
coex_bt_request = 0x40000ad0;
coex_core_ble_conn_dyn_prio_get = 0x40000ad4;
coex_core_event_duration_get = 0x40000ad8;
coex_core_pti_get = 0x40000adc;
coex_core_release = 0x40000ae0;
coex_core_request = 0x40000ae4;
coex_core_status_get = 0x40000ae8;
coex_core_timer_idx_get = 0x40000aec;
coex_event_duration_get = 0x40000af0;
coex_hw_timer_disable = 0x40000af4;
coex_hw_timer_enable = 0x40000af8;
coex_hw_timer_set = 0x40000afc;
coex_schm_interval_set = 0x40000b00;
coex_schm_lock = 0x40000b04;
coex_schm_unlock = 0x40000b08;
/*coex_status_get = 0x40000b0c;*/
coex_wifi_release = 0x40000b10;
esp_coex_ble_conn_dynamic_prio_get = 0x40000b14;
/* Data (.data, .bss, .rodata) */
coex_env_ptr = 0x4085ffc4;
coex_pti_tab_ptr = 0x4085ffc0;
coex_schm_env_ptr = 0x4085ffbc;
coexist_funcs = 0x4085ffb8;
g_coa_funcs_p = 0x4085ffb4;
g_coex_param_ptr = 0x4085ffb0;
