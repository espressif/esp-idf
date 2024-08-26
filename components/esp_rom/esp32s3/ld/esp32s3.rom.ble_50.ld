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

/* extend adv */
f_hci_le_set_ext_adv_param_cmd_handler = 0x40000000;
f_hci_le_set_adv_set_rand_addr_cmd_handler = 0x40000000;
f_hci_le_set_ext_adv_data_cmd_handler = 0x40000000;
f_hci_le_set_ext_scan_rsp_data_cmd_handler = 0x40000000;
f_hci_le_set_ext_adv_en_cmd_handler = 0x40000000;
f_hci_le_rd_max_adv_data_len_cmd_handler = 0x40000000;
f_hci_le_rd_nb_supp_adv_sets_cmd_handler = 0x40000000;
f_hci_le_rmv_adv_set_cmd_handler = 0x40000000;
f_hci_le_clear_adv_sets_cmd_handler = 0x40000000;
r_lld_adv_sync_info_set = 0x40000000;

r_lld_ext_adv_dynamic_pti_process = 0x40000000;
r_lld_adv_ext_chain_construct = 0x40000000;
r_lld_adv_aux_evt_canceled_cbk = 0x40000000;
r_lld_adv_aux_evt_start_cbk = 0x40000000;
r_lld_adv_aux_ch_idx_set = 0x40000000;

/* periodic adv */
f_hci_le_set_per_adv_param_cmd_handler = 0x40000000;
f_hci_le_set_per_adv_data_cmd_handler = 0x40000000;
f_hci_le_set_per_adv_en_cmd_handler = 0x40000000;
r_lld_per_adv_ch_map_update = 0x40000000;
r_lld_per_adv_init = 0x40000000;

/* PA list */
f_hci_le_add_dev_to_per_adv_list_cmd_handler = 0x40000000;
f_hci_le_rmv_dev_from_per_adv_list_cmd_handler = 0x40000000;
f_hci_le_clear_per_adv_list_cmd_handler = 0x40000000;
f_hci_le_rd_per_adv_list_size_cmd_handler = 0x40000000;

/* extend scan */
f_hci_le_set_ext_scan_param_cmd_handler = 0x40000000;
f_hci_le_set_ext_scan_en_cmd_handler = 0x40000000;
r_lld_scan_process_pkt_rx_ext_adv = 0x40000000;
r_lld_scan_trunc_ind = 0x40000000;

/* extend con */
f_hci_le_ext_create_con_cmd_handler = 0x40000000;
r_lld_init_process_pkt_rx_adv_ext_ind = 0x40000000;
r_lld_init_process_pkt_rx_aux_connect_rsp = 0x40000000;

/* PA sync */
f_hci_le_per_adv_create_sync_cmd_handler = 0x40000000;
f_hci_le_per_adv_create_sync_cancel_cmd_handler = 0x40000000;
f_hci_le_per_adv_term_sync_cmd_handler = 0x40000000;
f_lld_per_adv_rx_end_ind_handler_hack = 0x40000000;
f_lld_sync_start_req_handler = 0x40000000;
f_lld_per_adv_rep_ind_handler = 0x40000000;
r_lld_sync_init = 0x40000000;

/* phy update*/
r_phy_upd_proc_start = 0x40000000;
f_llc_op_phy_upd_ind_handler = 0x40000000;
f_ll_phy_req_handler = 0x40000000;
f_ll_phy_rsp_handler = 0x40000000;
f_ll_phy_update_ind_handler = 0x40000000;
f_lld_phy_upd_cfm_handler = 0x40000000;
f_hci_le_set_phy_cmd_handler = 0x40000000;
llc_llcp_phy_update_ind_ack = 0x40000000;
