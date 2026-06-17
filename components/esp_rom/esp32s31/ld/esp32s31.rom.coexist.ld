/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.coexist.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
esp_coex_rom_version_get = 0x2f800bf0;
coex_bt_release = 0x2f800bf4;
coex_bt_request = 0x2f800bf8;
coex_core_ble_conn_dyn_prio_get = 0x2f800bfc;
coex_core_event_duration_get = 0x2f800c00;
coex_core_pti_get = 0x2f800c04;
coex_core_release = 0x2f800c08;
coex_core_request = 0x2f800c0c;
coex_core_status_get = 0x2f800c10;
coex_core_timer_idx_get = 0x2f800c14;
coex_event_duration_get = 0x2f800c18;
coex_hw_timer_disable = 0x2f800c1c;
coex_hw_timer_enable = 0x2f800c20;
coex_hw_timer_set = 0x2f800c24;
coex_schm_interval_set = 0x2f800c28;
coex_schm_lock = 0x2f800c2c;
coex_schm_unlock = 0x2f800c30;
coex_status_get = 0x2f800c34;
coex_wifi_release = 0x2f800c38;
esp_coex_ble_conn_dynamic_prio_get = 0x2f800c3c;
/* Data (.data, .bss, .rodata) */
coex_env_ptr = 0x2f07ffa8;
coex_pti_tab_ptr = 0x2f07ffa4;
coex_schm_env_ptr = 0x2f07ffa0;
coexist_funcs = 0x2f07ff9c;
g_coa_funcs_p = 0x2f07ff98;
g_coex_param_ptr = 0x2f07ff94;
