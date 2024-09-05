/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/***************************************
 Group bluetooth
 ***************************************/

/* Functions */
r__os_msys_find_pool = 0x40000ac0;
r_ble_hci_ram_hs_acl_tx = 0x40000ac8;
r_ble_hci_ram_hs_cmd_tx = 0x40000acc;
r_ble_hci_ram_ll_acl_tx = 0x40000ad0;
r_ble_hci_ram_ll_evt_tx = 0x40000ad4;
r_ble_hci_trans_deinit = 0x40000af4;
r_ble_hci_trans_env_init = 0x40000af8;
r_ble_hci_uart_hs_acl_tx = 0x40000b10;
r_ble_hci_uart_hs_cmd_tx = 0x40000b14;
r_ble_hci_uart_ll_acl_tx = 0x40000b18;
r_ble_hci_uart_ll_evt_tx = 0x40000b1c;
r_ble_hw_driver_deinit = 0x40000b60;
r_ble_hw_driver_env_init = 0x40000b64;
r_ble_hw_resolv_list_get_cur_entry = 0x40000b84;
r_ble_hw_whitelist_clear = 0x40000ba4;
r_ble_hw_whitelist_rmv = 0x40000bb0;
r_ble_ll_adv_deinit = 0x40000c00;
r_ble_ll_adv_env_init = 0x40000c08;
r_ble_ll_adv_ext_set_enable = 0x40000c10;
r_ble_ll_adv_ext_set_param = 0x40000c14;
r_ble_ll_adv_legacy_pdu_make = 0x40000c40;
r_ble_ll_adv_periodic_enable = 0x40000c50;
r_ble_ll_adv_periodic_set_data = 0x40000c5c;
r_ble_ll_adv_remove = 0x40000c84;
r_ble_ll_adv_reset = 0x40000c88;
r_ble_ll_adv_rx_pkt_in = 0x40000c94;
r_ble_ll_adv_send_conn_comp_ev = 0x40000ca8;
r_ble_ll_adv_set_adv_data = 0x40000cac;
r_ble_ll_adv_set_scan_rsp_data = 0x40000cbc;
r_ble_ll_adv_set_sched = 0x40000cc0;
r_ble_ll_adv_sm_init = 0x40000cd4;
r_ble_ll_adv_sync_calculate = 0x40000cf4;
r_ble_ll_adv_sync_pdu_make = 0x40000cfc;
r_ble_ll_arr_pool_init = 0x40000d10;
r_ble_ll_check_scan_params = 0x40000d24;
r_ble_ll_conn_connect_ind_pdu_make = 0x40000d54;
r_ble_ll_conn_create = 0x40000d58;
r_ble_ll_conn_created = 0x40000d60;
r_ble_ll_conn_enqueue_pkt = 0x40000d7c;
r_ble_ll_conn_env_init = 0x40000d80;
r_ble_ll_conn_hci_le_set_phy = 0x40000db4;
r_ble_ll_conn_hci_set_chan_class = 0x40000dd8;
r_ble_ll_conn_is_dev_connected = 0x40000dec;
r_ble_ll_conn_module_deinit = 0x40000dfc;
r_ble_ll_conn_module_init = 0x40000e00;
r_ble_ll_conn_next_event = 0x40000e08;
r_ble_ll_conn_prepare_tx_pdu = 0x40000e10;
r_ble_ll_conn_rx_data_pdu = 0x40000e1c;
r_ble_ll_conn_slave_start = 0x40000e38;
r_ble_ll_conn_sm_new = 0x40000e40;
r_ble_ll_conn_sm_npl_deinit = 0x40000e44;
r_ble_ll_conn_sm_npl_init = 0x40000e48;
r_ble_ll_ctrl_chk_proc_start = 0x40000e58;
r_ble_ll_ctrl_conn_upd_make = 0x40000e68;
r_ble_ll_ctrl_enc_allowed_pdu_tx = 0x40000e78;
r_ble_ll_ctrl_proc_init = 0x40000ea8;
r_ble_ll_ctrl_proc_start = 0x40000eb0;
r_ble_ll_ctrl_proc_unk_rsp = 0x40000eb8;
r_ble_ll_ctrl_rx_feature_rsp = 0x40000ee4;
r_ble_ll_ctrl_rx_pdu = 0x40000ef0;
r_ble_ll_ctrl_tx_done = 0x40000f30;
r_ble_ll_deinit = 0x40000f40;
r_ble_ll_env_init = 0x40000f48;
r_ble_ll_ext_conn_create = 0x40000f60;
r_ble_ll_generic_data_init = 0x40000f78;
r_ble_ll_get_chan_to_scan = 0x40000f80;
r_ble_ll_hci_cb_host_buf_size = 0x40000f98;
r_ble_ll_hci_cmd_proc = 0x40000fac;
r_ble_ll_hci_ctlr_bb_cmd_proc = 0x40000fb4;
r_ble_ll_hci_deinit = 0x40000fb8;
r_ble_ll_hci_env_init = 0x40000fc0;
r_ble_ll_hci_event_send = 0x40001004;
r_ble_ll_hci_le_cmd_proc = 0x40001024;
r_ble_ll_hci_le_read_local_features = 0x40001040;
r_ble_ll_hci_send_legacy_ext_adv_report = 0x40001080;
r_ble_ll_init = 0x400010a8;
r_ble_ll_init_rx_pkt_in = 0x400010b4;
r_ble_ll_mbuf_init = 0x400010dc;
r_ble_ll_modify_sca_action = 0x400010e8;
r_ble_ll_rand = 0x400010fc;
r_ble_ll_rand_deinit = 0x40001104;
r_ble_ll_rand_env_init = 0x40001108;
r_ble_ll_read_tx_power = 0x4000112c;
r_ble_ll_reset = 0x40001130;
r_ble_ll_resolv_deinit = 0x4000113c;
r_ble_ll_resolv_env_init = 0x40001148;
r_ble_ll_resolv_gen_priv_addr = 0x4000114c;
r_ble_ll_resolv_init = 0x4000116c;
r_ble_ll_resolv_list_rmv = 0x4000118c;
r_ble_ll_scan_deinit = 0x400011e4;
r_ble_ll_scan_env_init = 0x40001208;
r_ble_ll_scan_init = 0x40001230;
r_ble_ll_scan_pre_process = 0x4000124c;
r_ble_ll_scan_rx_pkt_in_on_aux = 0x40001260;
r_ble_ll_scan_rx_pkt_in_on_legacy = 0x40001264;
r_ble_ll_scan_send_adv_report = 0x40001270;
r_ble_ll_scan_set_enable = 0x40001278;
r_ble_ll_scan_set_scan_params = 0x40001284;
r_ble_ll_set_ext_scan_params = 0x400012a4;
r_ble_ll_set_host_feat = 0x400012a8;
r_ble_ll_sync_create = 0x400012d4;
r_ble_ll_sync_deinit = 0x400012d8;
r_ble_ll_sync_env_init = 0x400012e0;
r_ble_ll_sync_est_event_failed = 0x400012e4;
r_ble_ll_sync_est_event_success = 0x400012e8;
r_ble_ll_sync_parse_ext_hdr = 0x40001334;
r_ble_ll_sync_put_syncinfo = 0x40001344;
r_ble_ll_sync_receive_enable = 0x40001348;
r_ble_ll_sync_rx_pkt_in = 0x40001358;
r_ble_ll_task = 0x4000137c;
r_ble_ll_trace_set_func = 0x40001380;
r_ble_ll_tx_mbuf_pducb = 0x40001394;
r_ble_ll_tx_pkt_in = 0x40001398;
r_ble_ll_whitelist_clear = 0x400013c8;
r_ble_ll_whitelist_rmv = 0x400013d0;
r_ble_ll_whitelist_search = 0x400013d4;
r_ble_lll_adv_aux_scannable_pdu_payload_len = 0x400013dc;
r_ble_lll_adv_drop_event = 0x40001420;
r_ble_lll_adv_ext_estimate_data_itvl = 0x4000142c;
r_ble_lll_adv_halt = 0x40001434;
r_ble_lll_adv_periodic_schedule_next = 0x4000144c;
r_ble_lll_adv_periodic_start = 0x40001450;
r_ble_lll_adv_periodic_stop = 0x40001454;
r_ble_lll_adv_rx_pkt_isr = 0x40001464;
r_ble_lll_adv_sec_done = 0x40001468;
r_ble_lll_conn_append_tx_buffer = 0x400014c8;
r_ble_lll_conn_cth_flow_free_credit = 0x400014f0;
r_ble_lll_conn_end = 0x400014f8;
r_ble_lll_conn_env_deinit = 0x400014fc;
r_ble_lll_conn_env_init = 0x40001500;
r_ble_lll_conn_event_end = 0x40001504;
r_ble_lll_conn_event_end_timer_cb = 0x40001508;
r_ble_lll_conn_event_halt = 0x4000150c;
r_ble_lll_conn_get_rx_mbuf = 0x40001528;
r_ble_lll_conn_halt = 0x4000152c;
r_ble_lll_conn_module_deinit = 0x40001538;
r_ble_lll_conn_module_init = 0x4000153c;
r_ble_lll_conn_no_mem_evt_pre_cb = 0x40001544;
r_ble_lll_conn_process_acked_pdu = 0x4000154c;
r_ble_lll_conn_process_in_isr = 0x40001550;
r_ble_lll_conn_rx_pkt_isr = 0x40001560;
r_ble_lll_conn_set_slave_flow_control = 0x4000156c;
r_ble_lll_conn_slave_new = 0x40001570;
r_ble_lll_conn_update_encryption = 0x40001590;
r_ble_lll_dtm_deinit = 0x400015a4;
r_ble_lll_dtm_init = 0x400015b4;
r_ble_lll_dtm_tx_create_ctx = 0x400015dc;
r_ble_lll_init_rx_pkt_isr = 0x40001624;
r_ble_lll_reset = 0x40001644;
r_ble_lll_rfmgmt_controller_sleep_en = 0x40001648;
r_ble_lll_rfmgmt_deinit = 0x4000164c;
r_ble_lll_rfmgmt_disable = 0x40001650;
r_ble_lll_rfmgmt_enable = 0x40001654;
r_ble_lll_rfmgmt_enable_now = 0x40001658;
r_ble_lll_rfmgmt_init = 0x4000165c;
r_ble_lll_rfmgmt_release_ev = 0x40001668;
r_ble_lll_rfmgmt_reset = 0x4000166c;
r_ble_lll_rfmgmt_timer_exp = 0x40001680;
r_ble_lll_rfmgmt_timer_reschedule = 0x40001684;
r_ble_lll_scan_aux_data_free = 0x40001698;
r_ble_lll_scan_halt = 0x400016c4;
r_ble_lll_scan_init = 0x400016cc;
r_ble_lll_scan_npl_init = 0x400016d0;
r_ble_lll_scan_process_rsp_in_isr = 0x400016e8;
r_ble_lll_scan_restart = 0x400016f0;
r_ble_lll_scan_rx_isr_on_aux = 0x400016f4;
r_ble_lll_scan_rx_isr_on_legacy = 0x400016f8;
r_ble_lll_scan_rx_pkt_isr = 0x400016fc;
r_ble_lll_scan_stop = 0x40001710;
r_ble_lll_scan_targeta_is_matched = 0x40001714;
r_ble_lll_sched_conn_reschedule = 0x40001730;
r_ble_lll_sched_deinit = 0x40001734;
r_ble_lll_sched_env_init = 0x4000173c;
r_ble_lll_sched_execute_check = 0x40001740;
r_ble_lll_sched_init = 0x40001748;
r_ble_lll_sched_sync_reschedule = 0x40001784;
r_ble_lll_sync_event_end = 0x400017a0;
r_ble_lll_sync_event_start_cb = 0x400017a8;
r_ble_lll_sync_halt = 0x400017b0;
r_ble_lll_sync_rx_pkt_isr = 0x400017c8;
r_ble_phy_config_access_addr = 0x400017f0;
r_ble_phy_env_init = 0x4000180c;
r_ble_phy_init = 0x40001828;
r_ble_phy_isr = 0x4000182c;
r_ble_phy_module_deinit = 0x40001840;
r_ble_phy_monitor_bb_sync = 0x40001848;
r_ble_phy_set_adv_mode = 0x40001888;
r_ble_phy_set_rxhdr = 0x400018a0;
r_ble_phy_set_scan_mode = 0x400018a4;
r_ble_phy_set_sequence_mode = 0x400018a8;
r_ble_phy_txpower_round = 0x400018d8;
r_ble_phy_update_conn_sequence = 0x400018e0;
r_ble_phy_update_encryption = 0x400018e4;
r_hal_rtc_intr_init = 0x40001960;
r_hal_rtc_irq_handler = 0x40001964;
r_hal_timer_deinit = 0x40001968;
r_hal_timer_env_init = 0x40001970;
r_hal_timer_init = 0x40001974;
r_hal_timer_set_exp_tick = 0x40001988;
r_hal_timer_start_at = 0x40001990;
r_hal_timer_stop = 0x40001994;
r_mem_split_frag = 0x400019bc;
r_os_mbuf_append = 0x400019e0;
r_os_memblock_put = 0x40001a48;
r_os_mempool_init_internal = 0x40001a64;
r_os_mempool_module_init = 0x40001a6c;
/* Data (.data, .bss, .rodata) */
g_ble_ll_ctrl_pkt_lengths_ro = 0x3ff4fd8c;


/***************************************
 Group rom_pp
 ***************************************/

/* Functions */
esf_buf_alloc = 0x40001ae0;
esf_buf_alloc_dynamic = 0x40001ae4;
esf_buf_recycle = 0x40001ae8;
hal_mac_tx_set_ppdu = 0x40001af8;
ic_mac_deinit = 0x40001b00;
lmacAdjustTimestamp = 0x40001b10;
lmacDiscardMSDU = 0x40001b18;
lmacEndFrameExchangeSequence = 0x40001b1c;
lmacMSDUAged = 0x40001b28;
lmacSetTxFrame = 0x40001b4c;
pm_check_state = 0x40001b6c;
pm_dream = 0x40001b78;
pm_on_beacon_rx = 0x40001ba0;
pm_parse_beacon = 0x40001bac;
pm_process_tim = 0x40001bb0;
pm_rx_beacon_process = 0x40001bb4;
pm_rx_data_process = 0x40001bb8;
pm_sleep = 0x40001bbc;
pm_tbtt_process = 0x40001bc4;
ppAssembleAMPDU = 0x40001bcc;
ppCalTxAMPDULength = 0x40001bd8;
ppRxFragmentProc = 0x40001c28;
ppRxPkt = 0x40001c2c;
pp_coex_tx_request = 0x40001c54;
rcGetSched = 0x40001c88;
rcTxUpdatePer = 0x40001c94;
rcUpdateRate = 0x40001c9c;
wDev_AppendRxBlocks = 0x40001cdc;
wDev_IndicateFrame = 0x40001cec;
wDev_ProcessFiq = 0x40001d08;
wDev_ProcessRxSucData = 0x40001d0c;
ppProcTxDone = 0x40001d1c;
pm_tx_data_done_process = 0x40001d20;
ppMapWaitTxq = 0x40001d28;
lmacDiscardFrameExchangeSequence = 0x40001d70;
lmacDisableTransmit = 0x40001d74;
lmacProcessTxTimeout = 0x40001d78;
lmacProcessTxSuccess = 0x40001d7c;
lmacProcessTxComplete = 0x40001d8c;
lmacProcessTxopQComplete = 0x40001dc0;
lmacInit = 0x40001dc8;
hal_init = 0x40001dd0;
mac_txrx_init = 0x40001de0;
lmac_stop_hw_txq = 0x40001df8;
ppTxFragmentProc = 0x40001e08;
esf_buf_setup = 0x40001e0c;
hal_crypto_set_key_entry = 0x40001e18;
pm_start = 0x40001e34;
pm_stop = 0x40001e38;
hal_set_sta_tbtt = 0x40001e4c;
pm_update_next_tbtt = 0x40001e50;
pm_set_sleep_type = 0x40001e54;
pm_tx_null_data_done_process = 0x40001eb0;
pm_tx_data_process = 0x40001eb4;
pm_attach = 0x40001eb8;
pm_coex_schm_process = 0x40001ebc;
pm_on_probe_resp_rx = 0x40001ecc;
pm_send_probe_stop = 0x40001edc;
hal_sniffer_rx_set_promis = 0x40001ef4;
hal_sniffer_set_promis_misc_pkt = 0x40001efc;
tsf_hal_set_tbtt_start_time = 0x40001f38;


/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
ieee80211_ampdu_reorder = 0x40001fb0;
ieee80211_encap_esfbuf = 0x40001fb8;
ieee80211_output_process = 0x40001fc4;
sta_input = 0x40001fcc;
ieee80211_classify = 0x40001fe0;
ieee80211_crypto_decap = 0x40001ff8;
ieee80211_ccmp_decrypt = 0x4000200c;
ieee80211_ccmp_encrypt = 0x40002010;
ieee80211_send_action_vendor_spec = 0x40002034;
ieee80211_send_mgmt = 0x40002038;
ieee80211_auth_construct = 0x4000203c;
ieee80211_deauth_construct = 0x40002040;
ieee80211_disassoc_construct = 0x40002044;
ieee80211_add_xrates = 0x40002058;
ieee80211_assoc_req_construct = 0x40002060;
ieee80211_assoc_resp_construct = 0x40002064;
ieee80211_timer_process = 0x4000208c;
cnx_coexist_timeout = 0x40002090;
sta_recv_mgmt = 0x40002094;
ieee80211_send_probereq = 0x4000209c;
sta_auth_open = 0x400020a0;
sta_auth_sae = 0x400020a8;
cnx_coexist_timeout_process = 0x400020ac;
ieee80211_add_extcap = 0x400020cc;
ieee80211_alloc_proberesp = 0x400020d8;
esp_wifi_80211_tx = 0x400020f8;
ieee80211_crypto_aes_128_cmac_decrypt = 0x40002104;
ieee80211_output_do = 0x4000210c;
ieee80211_send_nulldata = 0x40002110;
ieee80211_setup_robust_mgmtframe = 0x40002114;
ieee80211_mgmt_output = 0x40002118;
ieee80211_send_deauth = 0x40002120;
ieee80211_tx_mgt_cb = 0x4000212c;
sta_rx_csa = 0x40002134;
sta_send_sa_query_req = 0x40002138;
sta_send_sa_query_resp = 0x4000213c;
sta_recv_sa_query_req = 0x40002140;
sta_recv_sa_query_resp = 0x40002144;
ieee80211_parse_beacon = 0x40002148;
ieee80211_match_security = 0x40002154;
ieee80211_parse_rsn = 0x4000215c;
/* Data (.data, .bss, .rodata) */
s_sa_query_retries = 0x3fcdfad8;
s_sa_query_success = 0x3fcdfad5;
s_trans_id = 0x3fcdfac4;


/***************************************
 Group rom_coexist
 ***************************************/

/* Functions */
coex_core_event_duration_get = 0x40002178;
coex_core_timer_idx_get = 0x4000218c;
coex_hw_timer_tick_get = 0x400021b8;


/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
phy_xpd_tsens = 0x400021e8;
bt_track_pll_cap = 0x40002534;
tx_pwctrl_background = 0x40002548;


/***************************************
 Group eco4_systimer_hal
 ***************************************/

/* Functions */
systimer_hal_init = 0x400002a8;
systimer_hal_get_time = 0x400002b0;
systimer_hal_set_alarm_target = 0x400002b4;
systimer_hal_set_alarm_period = 0x400002b8;
systimer_hal_counter_value_advance = 0x400002c8;
systimer_hal_deinit = 0x40002ea8;
systimer_hal_set_tick_rate_ops = 0x40002eac;


/***************************************
 Group eco4_bluetooth
 ***************************************/

/* Functions */
r_ble_ll_adv_ext_pdu_tx_len = 0x40002eb0;
r_ble_ll_adv_ext_estimate_data_itvl = 0x40002eb4;
r_ble_ll_adv_ext_check_data_itvl = 0x40002eb8;
r_ble_ll_ctrl_channel_status_report_timer_cb = 0x40002ebc;
r_ble_ll_ctrl_channel_class_enable_make = 0x40002ec0;
r_ble_ll_ctrl_channel_class_reporting_make = 0x40002ec4;
r_ble_ll_ctrl_rx_channel_reporting_ind = 0x40002ec8;
r_ble_ll_ctrl_rx_channel_status_ind = 0x40002ecc;
r_ble_ll_ctrl_channel_class_info_update = 0x40002ed0;
r_ble_ll_adv_set_data_related_addr_change = 0x40002ed4;
r_ble_ll_misc_additional_options_set = 0x40002ed8;
r_ble_phy_get_txdbm_by_level = 0x40002edc;
r_hal_timer_disable_intr = 0x40002ee0;
r_hal_timer_enable_intr = 0x40002ee4;
r_hal_timer_task_stop = 0x40002ee8;
r_ble_lll_rfmgmt_env_init = 0x40002eec;
r_ble_ll_scan_set_aux_ll_flag = 0x40002ef0;
r_ble_ll_rf_temp_calibration = 0x40002ef4;
r_ble_ll_adv_env_deinit = 0x40002ef8;
r_ble_ll_conn_env_deinit = 0x40002efc;
r_ble_ll_hci_env_deinit = 0x40002f00;
r_ble_ll_rand_env_deinit = 0x40002f04;
r_ble_ll_resolv_env_deinit = 0x40002f08;
r_ble_ll_scan_env_deinit = 0x40002f0c;
r_ble_ll_sync_env_deinit = 0x40002f10;
r_ble_lll_rfmgmt_env_deinit = 0x40002f14;
r_hal_timer_env_deinit = 0x40002f18;
r_ble_ll_env_deinit = 0x40002f1c;
r_ble_ll_generic_data_deinit = 0x40002f20;
r_ble_hci_trans_env_deinit = 0x40002f24;
r_ble_ll_conn_callout_env_init = 0x40002f28;
r_ble_ll_conn_callout_env_deinit = 0x40002f2c;
r_ble_ll_scan_callout_env_init = 0x40002f30;
r_ble_ll_scan_callout_env_deinit = 0x40002f34;
r_ble_ll_callout_env_init = 0x40002f38;
r_ble_ll_callout_env_deinit = 0x40002f3c;
r_ble_ll_resolv_callout_env_init = 0x40002f40;
r_ble_ll_resolv_callout_env_deinit = 0x40002f44;
r_ble_ll_get_npl_element_info = 0x40002f48;
r_ble_rtc_wake_up_cpu_clr = 0x40002f4c;
r_ble_ll_scan_hci_set_adv_report_flow_ctrl = 0x40002f50;
r_ble_ll_scan_hci_update_adv_report_flow_ctrl = 0x40002f54;
r_ble_ll_scan_send_adv_lost_report = 0x40002f58;
r_ble_ll_scan_adv_report_cth_flow_is_enabled = 0x40002f5c;
r_ble_ll_scan_adv_report_cth_flow_alloc_credit = 0x40002f60;
r_ble_ll_scan_adv_report_cth_flow_free_credit = 0x40002f64;
r_ble_ll_scan_adv_report_cth_flow_have_credit = 0x40002f68;
r_ble_ll_scan_adv_report_lost_cnt_threshold_arrived = 0x40002f6c;
r_ble_ll_scan_adv_report_lost_cnt_clear = 0x40002f70;
r_ble_ll_scan_adv_report_lost_cnt_add = 0x40002f74;
r_ble_ll_trace_hex = 0x40002f78;
r_ble_ll_conn_calc_closest_event_cntr = 0x40002f7c;
r_ble_ll_trace_buffer_select = 0x40002f80;
r_ble_ll_adv_vendor_hci_legacy_adv_clear = 0x40002f84;
r_ble_ll_conn_is_lru_compare_with_sync = 0x40002f88;
r_ble_ll_conn_rollback_last_unmapped_chan = 0x40002f8c;
r_ble_ll_hci_vs_csa_set = 0x40002f90;
r_ble_ll_hci_reset = 0x40002f94;
r_ble_ll_adv_status_check = 0x40002f98;
r_ble_ll_conn_status_check = 0x40002f9c;
r_ble_ll_scan_status_check = 0x40002fa0;
r_ble_ll_sync_status_check = 0x40002fa4;
r_ble_ll_resolv_status_check = 0x40002fa8;
r_ble_ll_whitelist_status_check = 0x40002fac;
r_ble_ll_adv_delay_get = 0x40002fb0;
r_ble_ll_scan_continue_status_get = 0x40002fb4;
r_ble_ll_default_privacy_mode_get = 0x40002fb8;
r_ble_ll_adv_periodic_status_check = 0x40002fbc;
r_ble_ll_sync_list_status_check = 0x40002fc0;
r_ble_lll_rfmgmt_wake_up_ev = 0x40002fc4;
r_ble_lll_sched_env_deinit = 0x40002fc8;
r_ble_phy_env_deinit = 0x40002fcc;
r_ble_hw_driver_env_deinit = 0x40002fd0;
r_ble_lll_dtm_env_init = 0x40002fd4;
r_ble_lll_dtm_env_deinit = 0x40002fd8;
r_ble_lll_scan_callout_env_init = 0x40002fdc;
r_ble_lll_scan_callout_env_deinit = 0x40002fe0;
r_ble_lll_scan_env_init = 0x40002fe4;
r_ble_lll_scan_env_deinit = 0x40002fe8;
r_ble_lll_get_npl_element_info = 0x40002fec;
r_ble_lll_conn_rxpdu_alloc = 0x40002ff0;
r_ble_lll_scan_filter_out_useless_adv = 0x40002ff4;
r_ble_hw_whitelist_check_in_wl = 0x40002ff8;
r_ble_phy_stats_reset = 0x40002ffc;
r_ble_phy_ramup_time_set = 0x40003000;
r_ble_lll_rfmgmt_should_skip_light_sleep_check = 0x40003004;
r_ble_phy_rx_err_record = 0x40003008;
r_ble_lll_rfmgmt_wake_up_overhead_set = 0x4000300c;
r_ble_lll_conn_event_delete_and_reschedule = 0x40003010;
r_ble_lll_conn_should_reschedule = 0x40003014;
r_ble_lll_adv_ext_event_rmvd_from_sched = 0x40003018;
r_ble_lll_conn_process_rx_data_after_halt = 0x4000301c;
r_ble_phy_global_rxbuf_get = 0x40003020;
/* Data (.data, .bss, .rodata) */
priv_config_additional_opts_ptr = 0x3fcdfa70;
g_ble_ll_ctrl_pkt_lengths_eco4_ro = 0x3ff4fbac;


/***************************************
 Group eco4_rom_net80211
 ***************************************/

/* Functions */
sta_reset_beacon_timeout = 0x40003024;
ieee80211_post_hmac_tx = 0x40003028;
sta_rx_eapol = 0x4000302c;
/* Data (.data, .bss, .rodata) */
len_dh_ie_ptr = 0x3fcdfa6c;
g_authmode_threshold_failure_ptr = 0x3fcdfa68;


/***************************************
 Group eco4_rom_pp
 ***************************************/

/* Functions */
pm_enable_dream_timer = 0x40003030;
pm_handle_tbtt_interval = 0x40003034;
hal_set_sta_light_sleep_wake_ahead_time = 0x40003038;
hal_get_time_to_sta_next_tbtt = 0x4000303c;
pm_scale_listen_interval = 0x40003040;
pm_beacon_monitor_tbtt_start = 0x40003044;
pm_beacon_monitor_tbtt_stop = 0x40003048;
pm_beacon_monitor_timeout_process = 0x4000304c;
pm_beacon_monitor_tbtt_timeout_process = 0x40003050;
pm_on_tsf_timer = 0x40003054;
pp_coex_tx_release = 0x40003058;
ppRegressAmpdu = 0x4000305c;
trc_onPPTxDone = 0x40003060;
is_fragmented_pkt = 0x40003064;
pm_tx_null_data_done_quick_wake_process = 0x40003068;
pm_enable_beacon_monitor_timer = 0x4000306c;
pm_disable_beacon_monitor_timer = 0x40003070;
pm_beacon_offset_get_average = 0x40003074;
pm_beacon_offset_get_expect = 0x40003078;
pm_beacon_offset_get_params = 0x4000307c;
pm_beacon_offset_sample_start = 0x40003080;
pm_beacon_offset_sample = 0x40003084;
pm_beacon_offset_reset = 0x40003088;
pm_beacon_offset_is_sampling = 0x4000308c;
pm_beacon_offset_is_enabled = 0x40003090;
pm_beacon_offset_configure = 0x40003094;
pm_beacon_offset_resample_timeout_process = 0x40003098;
pm_beacon_offset_set_rx_beacon_standard = 0x4000309c;
pm_beacon_offset_get_rx_beacon_success_rate = 0x400030a0;
pm_beacon_offset_add_total_counter = 0x400030a4;
pm_beacon_offset_add_loss_counter = 0x400030a8;
pm_beacon_offset_check = 0x400030ac;
pm_beacon_offset_init = 0x400030b0;
pm_beacon_offset_deinit = 0x400030b4;
pm_get_tbtt_count = 0x400030b8;
pm_coex_schm_overall_period_get = 0x400030bc;
pm_coex_pwr_update = 0x400030c0;
/* Data (.data, .bss, .rodata) */
s_pm_beacon_offset_ptr = 0x3fcdfa64;
s_pm_beacon_offset_config_ptr = 0x3fcdfa60;
s_tbttstart_ptr = 0x3fcdfa5c;
