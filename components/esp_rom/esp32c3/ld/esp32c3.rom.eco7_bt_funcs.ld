/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/***************************************
 Group eco7_bluetooth
 ***************************************/

/* Functions */
r_lld_con_count_get = 0x40001c48;
r_lld_update_con_offset = 0x40001c4c;
r_lld_con_update_last_clock = 0x40001c50;
r_lld_con_llcp_ind_info_clear = 0x40001c54;
r_lld_con_update_terminte_info_init = 0x40001c58;
r_lld_con_terminate_max_evt_update = 0x40001c5c;
r_llc_pref_param_compute_eco = 0x40001ce8;
r_llc_hci_con_upd_info_send_eco = 0x40001cec;
r_llc_rem_encrypt_proc_continue_eco = 0x40001cf0;
r_lld_ext_adv_dynamic_aux_pti_process_eco = 0x40001cfc;
r_lld_adv_start_eco = 0x40001d04;
r_lld_con_evt_canceled_cbk_eco = 0x40001d08;
r_lld_con_evt_time_update_eco = 0x40001d0c;
r_lld_con_start_eco = 0x40001d10;
r_lld_con_frm_isr_eco = 0x40001d14;
r_lld_con_tx_eco = 0x40001d18;
r_lld_ext_scan_dynamic_pti_process_eco = 0x40001d28;
r_lld_scan_frm_eof_isr_eco = 0x40001d2c;
r_lld_sync_start_eco = 0x40001d30;
r_lld_sync_insert_eco = 0x40001d34;
r_llm_adv_rep_flow_control_update_eco = 0x40001d38;
r_llm_env_adv_dup_filt_init_eco = 0x40001d3c;
r_llm_env_adv_dup_filt_deinit_eco = 0x40001d40;
r_llm_adv_rep_flow_control_check_eco = 0x40001d44;
r_llm_scan_start_eco = 0x40001d48;
r_llm_update_duplicate_scan_count = 0x40001d4c;
r_llc_hci_command_handler_pre = 0x40001d50;
r_llc_hci_command_handler_get = 0x40001d54;
r_llc_hci_command_handler_search = 0x40001d58;
r_llc_llcp_pdu_handler_pre = 0x40001d60;
r_llc_llcp_pdu_handler_end = 0x40001d64;
r_llc_con_conflict_check = 0x40001d6c;
r_sch_prog_hw_reset_try = 0x40001d70;
r_sch_prog_et_state_reset = 0x40001d74;
r_sch_prog_end_isr_handler = 0x40001d78;
r_sch_plan_conflict_check = 0x40001d7c;
r_rwble_isr_hw_fixed = 0x40001d80;
r_bt_bb_recorrect_is_dead = 0x40001d84;
r_bt_bb_restart_hw_recorrect = 0x40001d88;
r_ke_task_handler_pre = 0x40001da0;
r_ke_task_handler_end = 0x40001da4;
r_lld_scan_frm_skip_isr_eco = 0x40001db0;
r_lld_ext_scan_dynamic_pti_reset = 0x40001db4;
r_llc_rem_phy_upd_proc_continue_eco = 0x40001db8;
r_llm_get_preferred_phys = 0x40001dbc;
r_lld_hw_cca_isr_eco = 0x40001dc0;
r_lld_sw_cca_isr_eco = 0x40001dc4;
r_lld_cca_chan_prn_e = 0x40001dc8;
r_lld_cca_chan_prn_s = 0x40001dcc;
r_lld_cca_chan_sel_remap = 0x40001dd0;
r_lld_cca_chan_sel_1 = 0x40001dd4;
r_lld_cca_chan_sel_2 = 0x40001dd8;
r_lld_cca_set_thresh = 0x40001ddc;
r_lld_cca_con_start = 0x40001de0;
r_lld_cca_con_end = 0x40001de4;
r_lld_cca_chm_restore = 0x40001de8;
r_lld_cca_chan_unused_check = 0x40001dec;
r_lld_cca_chm_update_check = 0x40001df0;
r_lld_cca_busy_mode_handle = 0x40001df4;
r_lld_cca_lbt_handle = 0x40001df8;
r_lld_cca_scst_timeout_check = 0x40001dfc;
r_lld_cca_chan_avl_timeout_check = 0x40001e00;

r_lld_con_start_hook = 0x40001ca8;

/* ble Functions eco */
r_bt_bb_isr_hack = 0x40000b9c;
r_bt_rf_coex_conn_phy_coded_data_time_limit_en_get_hack = 0x40000ba8;
r_bt_rtp_get_txpwr_idx_by_act_hack = 0x40000c00;
r_btdm_task_post_hack = 0x40000c14;
r_btdm_task_post_from_isr_hack = 0x40000c18;
r_btdm_task_recycle_hack = 0x40000c1c;
r_ke_task_schedule_hack = 0x40000e80;
r_llc_hci_command_handler_hack = 0x40000ef0;
r_llc_loc_ch_map_proc_continue_hack = 0x40000f5c;
r_llc_loc_con_upd_proc_continue_hack = 0x40000f60;
r_llc_loc_phy_upd_proc_continue_hack = 0x40000f78;
r_llc_rem_con_upd_proc_continue_hack = 0x40000fb4;
r_lld_con_sched_hack = 0x40001118;
r_lld_con_stop_hack = 0x40001124;
r_lld_llcp_rx_ind_handler_hack = 0x400011b0;
r_lld_per_adv_sched_hack = 0x400011f8;
r_rf_txpwr_cs_get_hack = 0x40001428;
r_rf_txpwr_dbm_get_hack = 0x4000142c;
r_sch_arb_event_start_isr_hack = 0x400014f8;
r_sch_plan_set_hack = 0x40001534;
r_sch_prog_end_isr_hack = 0x40001538;
r_lld_adv_ext_chain_scannable_construct_hack = 0x40001b58;

r_lld_scan_process_pkt_rx_hack = 0x40001280;
r_llm_le_features_get_hack = 0x400013b0;

/* ble functions rename */
r_lld_init_start_hack = 0x400011a4;

/* ble functions disable */
/*
r_lld_adv_frm_isr_eco = 0x40001d00;
r_lld_res_list_clear = 0x40004638;
r_lld_res_list_rem = 0x40004680;
r_lld_adv_start_hook = 0x40001c80;
r_lld_con_evt_start_cbk_eco = 0x40001d1c;
r_lld_con_tx_prog_new_packet = 0x40001b74;
r_lld_adv_ext_chain_none_construct = 0x40001b50;
r_llc_llcp_send_eco = 0x40001cf4;
r_llc_llcp_channel_map_ind_ack = 0x40001d68;
r_rwble_isr = 0x40001464;
r_lld_scan_start_eco = 0x40001d24;
r_lld_scan_try_sched_eco = 0x40001dac;
r_lld_scan_start_hook = 0x40001c74;
r_lld_init_start_hook = 0x40001cb8;
r_lld_scan_evt_start_cbk_eco = 0x40001d20;
r_ke_task_handler_get_overwrite = 0x40001da8;
r_hci_register_vendor_desc_tab = 0x40000d9c;
r_lld_scan_process_pkt_rx_adv_rep = 0x40001284;
r_register_esp_vendor_cmd_handler = 0x40001400;
r_llc_llcp_pdu_handler_get_overwrite = 0x40001d5c;
r_llc_start_eco = 0x40001cf8;
*/
