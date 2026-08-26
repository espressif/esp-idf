/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s3.rom.ld for esp32s3
 *
 *
 * Generated from ./interface-esp32s3.yml md5sum 39c4ce259b11323b9404c192b01b712b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */


/* BLE scan */

f_hci_le_set_scan_param_cmd_handler = 0x40000000;
f_hci_le_set_scan_en_cmd_handler = 0x40000000;
f_llm_scan_period_to_handler_hack = 0x40000000;
f_lld_adv_rep_ind_handler_hack = 0x40000000;
r_lld_scan_init = 0x40000000;
r_lld_scan_restart = 0x40000000;
f_lld_scan_end_ind_handler_hack = 0x40000000;
r_llm_env_adv_dup_filt_deinit_eco = 0x40000000;
llm_exception_list_init = 0x40000000;
llm_duplicate_list_init = 0x40000000;
f_hci_vendor_ble_update_duplicate_exceptional_list_cmd_handler = 0x40000000;
f_hci_vendor_ble_init_adv_flow_control_cmd_handler = 0x40000000;
f_hci_vendor_ble_update_adv_report_flow_control_cmd_handler = 0x40000000;
coex_schm_ble_scan_stop = 0x40000000;

f_hci_le_set_ext_scan_param_cmd_handler = 0x40000000;
f_hci_le_set_ext_scan_en_cmd_handler = 0x40000000;
r_lld_scan_process_pkt_rx_ext_adv = 0x40000000;
r_lld_scan_trunc_ind = 0x40000000;
