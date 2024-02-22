/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esf_buf_alloc = 0x400015bc;
esf_buf_alloc_dynamic = 0x400015c0;
esf_buf_recycle = 0x400015c4;
hal_mac_tx_set_ppdu = 0x400015d4;
ic_mac_deinit = 0x400015dc;
lmacDiscardMSDU = 0x400015f4;
/*lmacSetTxFrame = 0x40001628;*/
lmacTxDone = 0x4000162c;
lmacTxFrame = 0x40001630;
mac_tx_set_htsig = 0x40001638;
mac_tx_set_plcp1 = 0x40001640;
pm_check_state = 0x40001648;
pm_on_beacon_rx = 0x4000167c;
/*pm_parse_beacon = 0x40001688;*/
pm_process_tim = 0x4000168c;
pm_rx_beacon_process = 0x40001690;
pm_rx_data_process = 0x40001694;
/* pm_sleep = 0x40001698;*/
/* pm_tbtt_process = 0x400016a0;*/
ppMapTxQueue = 0x400016d8;
ppProcTxSecFrame = 0x400016dc;
/*ppRxFragmentProc = 0x40001704;*/
/* rcGetSched = 0x40001764;*/
rcTxUpdatePer = 0x40001770;
rcUpdateTxDone = 0x4000177c;
wDevCheckBlockError = 0x400017b4;
/* wDev_IndicateFrame = 0x400017c8;*/
wDev_ProcessFiq = 0x400017f0;
wDev_ProcessRxSucData = 0x400017f4;
/*ppProcTxDone = 0x40001804;*/
pm_tx_data_done_process = 0x40001808;
ppMapWaitTxq = 0x40001810;
/*ieee80211_encap_esfbuf = 0x4000185c;*/
/*sta_input = 0x40001870;*/
ieee80211_crypto_decap = 0x4000189c;
ieee80211_decap = 0x400018a0;
coex_core_timer_idx_get = 0x400018d0;
rom1_chip_i2c_readReg = 0x40001924;
rom1_chip_i2c_writeReg = 0x40001928;
rom_index_to_txbbgain = 0x40001964;
rom_pbus_xpd_tx_on = 0x400019b0;
rom1_set_noise_floor = 0x400019e8;
rom_set_tx_dig_gain = 0x400019f0;
rom_set_txcap_reg = 0x400019f4;
rom_txbbgain_to_index = 0x40001a0c;
rom1_disable_wifi_agc = 0x40001a1c;
rom1_enable_wifi_agc = 0x40001a20;
rom1_tx_paon_set = 0x40001a44;
rom_agc_reg_init = 0x40001a54;
rom_bb_reg_init = 0x40001a58;
rom1_set_pbus_reg = 0x40001a70;
rom_phy_xpd_rf = 0x40001a78;
rom_write_txrate_power_offset = 0x40001a8c;
rom1_get_rate_fcc_index = 0x40001a90;
rom1_read_sar2_code = 0x40001aa4;
rom2_temp_to_power1 = 0x40001ab4;
rom1_get_i2c_hostid = 0x40001ac8;
rom_open_i2c_xpd = 0x40001af8;
rom2_tsens_read_init1 = 0x40001b00;
rom_tsens_code_read = 0x40001b04;
rom_tsens_dac_cal = 0x40001b10;
rom1_phy_en_hw_set_freq = 0x40001b20;
rom1_phy_dis_hw_set_freq = 0x40001b24;
rom_pll_vol_cal = 0x40001b28;

rom1_bt_get_tx_gain = 0x40001bb8;
rom1_get_chan_target_power = 0x40001bbc;
rom2_get_tx_gain_value1 = 0x40001bc0;
rom1_wifi_tx_dig_gain = 0x40001bc4;
rom1_wifi_get_tx_gain = 0x40001bc8;
rom1_fe_i2c_reg_renew = 0x40001bcc;
rom1_i2c_master_reset = 0x40001bd4;
rom1_phy_wakeup_init = 0x40001bf0;
rom1_phy_i2c_init1 = 0x40001bf4;
rom1_tsens_temp_read = 0x40001bf8;
rom1_bt_track_pll_cap = 0x40001bfc;
rom1_wifi_set_tx_gain = 0x40001c04;
rom1_txpwr_cal_track = 0x40001c08;
rom1_bt_set_tx_gain = 0x40001c10;
rom1_phy_close_rf = 0x40001c18;


/***************************************
 Group eco7_uart
 ***************************************/

/* Functions */
uart_tx_switch = 0x40001c44;


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
r_llc_start_eco = 0x40001cf8;
r_lld_ext_adv_dynamic_aux_pti_process_eco = 0x40001cfc;
r_lld_adv_start_eco = 0x40001d04;
r_lld_con_evt_canceled_cbk_eco = 0x40001d08;
r_lld_con_evt_time_update_eco = 0x40001d0c;
r_lld_con_start_eco = 0x40001d10;
r_lld_con_frm_isr_eco = 0x40001d14;
r_lld_con_tx_eco = 0x40001d18;
r_lld_scan_evt_start_cbk_eco = 0x40001d20;
r_lld_scan_start_eco = 0x40001d24;
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
r_llc_llcp_pdu_handler_get_overwrite = 0x40001d5c;
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
r_btdm_task_post_impl = 0x40001d8c;
r_btdm_task_post_from_isr_impl = 0x40001d90;
r_btdm_vnd_offload_post_from_isr = 0x40001d94;
r_btdm_vnd_offload_post = 0x40001d98;
r_btdm_vnd_offload_process = 0x40001d9c;
r_ke_task_handler_pre = 0x40001da0;
r_ke_task_handler_end = 0x40001da4;
r_ke_task_handler_get_overwrite = 0x40001da8;
r_lld_scan_try_sched_eco = 0x40001dac;
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

r_lld_scan_start_hook = 0x40001c74;
r_lld_con_start_hook = 0x40001ca8;
r_lld_init_start_hook = 0x40001cb8;

/* ble Functions eco */
r_bt_bb_isr = 0x40000b9c;
r_bt_rf_coex_conn_phy_coded_data_time_limit_en_get = 0x40000ba8;
r_bt_rtp_get_txpwr_idx_by_act = 0x40000c00;
r_btdm_task_post = 0x40000c14;
r_btdm_task_post_from_isr = 0x40000c18;
r_btdm_task_recycle = 0x40000c1c;
r_hci_register_vendor_desc_tab = 0x40000d9c;
r_ke_task_schedule = 0x40000e80;
r_llc_hci_command_handler = 0x40000ef0;
r_llc_loc_con_upd_proc_continue = 0x40000f60;
r_llc_loc_phy_upd_proc_continue = 0x40000f78;
r_llc_rem_con_upd_proc_continue = 0x40000fb4;
r_lld_con_sched = 0x40001118;
r_lld_con_stop = 0x40001124;
r_lld_llcp_rx_ind_handler = 0x400011b0;
r_lld_per_adv_sched = 0x400011f8;
r_lld_scan_process_pkt_rx_adv_rep = 0x40001284;
r_register_esp_vendor_cmd_handler = 0x40001400;
r_rf_txpwr_cs_get = 0x40001428;
r_rf_txpwr_dbm_get = 0x4000142c;
r_sch_arb_event_start_isr = 0x400014f8;
r_sch_plan_set = 0x40001534;
r_sch_prog_end_isr = 0x40001538;
r_lld_adv_ext_chain_scannable_construct = 0x40001b58;

r_lld_scan_process_pkt_rx = 0x40001280;
r_llm_le_features_get = 0x400013b0;

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
*/


/***************************************
 Group eco7_phy
 ***************************************/

/* Functions */
rom2_pll_cap_mem_update = 0x40001e04;
rom2_phy_i2c_enter_critical = 0x40001e08;
rom2_phy_i2c_exit_critical = 0x40001e0c;
rom2_rfpll_cap_correct = 0x40001e10;
rom2_write_pll_cap = 0x40001e14;
rom2_read_pll_cap = 0x40001e18;
rom2_tester_wifi_cali = 0x40001e1c;
rom2_wait_hw_freq_busy = 0x40001e20;
rom2_rfpll_cap_track = 0x40001e24;
rom2_ulp_code_track = 0x40001e28;
rom2_ulp_ext_code_set = 0x40001e2c;
rom2_phy_set_tsens_power = 0x40001e30;
rom2_phy_get_tsens_value = 0x40001e34;
rom_mac_tx_chan_offset = 0x40001e38;
rom_rx_gain_force = 0x40001e3c;
