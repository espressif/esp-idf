/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/*
ESP32C3 ECO3 ROM address table
Version 3 API's imported from the ROM
*/


/***************************************
   Group eco3_bluetooth
 ***************************************/

/* Functions */
r_lld_legacy_adv_dynamic_pti_get = 0x40001b38;
r_lld_legacy_adv_dynamic_pti_process = 0x40001b3c;
r_lld_ext_adv_dynamic_pti_get = 0x40001b40;
r_lld_ext_adv_dynamic_aux_pti_process = 0x40001b44;
r_lld_ext_adv_dynamic_pti_process = 0x40001b48;
/*
r_lld_adv_ext_pkt_prepare_set = 0x40001b4c;
*/
r_lld_adv_ext_chain_connectable_construct = 0x40001b54;
r_lld_adv_pkt_rx_connect_post = 0x40001b5c;
r_lld_adv_start_init_evt_param = 0x40001b60;
r_lld_adv_start_set_cs = 0x40001b64;
/* r_lld_adv_start_update_filter_policy = 0x40001b68; */
r_lld_adv_start_schedule_asap = 0x40001b6c;
r_lld_con_tx_prog_new_packet_coex = 0x40001b70;
r_lld_per_adv_dynamic_pti_get = 0x40001b78;
r_lld_per_adv_evt_start_chm_upd = 0x40001b7c;
r_lld_ext_scan_dynamic_pti_get = 0x40001b80;
r_lld_sync_insert = 0x40001b88;
/*
r_sch_prog_ble_push = 0x40001b8c;
*/
r_sch_prog_bt_push = 0x40001b90;
r_lld_init_evt_end_type_set = 0x40001b94;
r_lld_init_evt_end_type_get = 0x40001b98;
r_lld_adv_direct_adv_use_rpa_addr_state_set = 0x40001b9c;
r_lld_adv_direct_adv_use_rpa_addr_state_get = 0x40001ba0;
r_lld_init_evt_end_type_check_state_set = 0x40001ba4;
r_lld_init_evt_end_type_check_state_get = 0x40001ba8;
