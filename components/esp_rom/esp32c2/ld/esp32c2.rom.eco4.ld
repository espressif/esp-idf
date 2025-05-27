/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */


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
//lmacSetTxFrame = 0x40001b4c;
pm_check_state = 0x40001b6c;
pm_dream = 0x40001b78;
pm_on_beacon_rx = 0x40001ba0;
//pm_parse_beacon = 0x40001bac;
//pm_process_tim = 0x40001bb0;
pm_rx_beacon_process = 0x40001bb4;
pm_rx_data_process = 0x40001bb8;
pm_sleep = 0x40001bbc;
//pm_tbtt_process = 0x40001bc4;
ppAssembleAMPDU = 0x40001bcc;
ppCalTxAMPDULength = 0x40001bd8;
ppRxFragmentProc = 0x40001c28;
ppRxPkt = 0x40001c2c;
pp_coex_tx_request = 0x40001c54;
rcGetSched = 0x40001c88;
rcTxUpdatePer = 0x40001c94;
rcUpdateRate = 0x40001c9c;
wDev_AppendRxBlocks = 0x40001cdc;
//wDev_IndicateFrame = 0x40001cec;
wDev_ProcessFiq = 0x40001d08;
/*wDev_ProcessRxSucData = 0x40001d0c;*/
ppProcTxDone = 0x40001d1c;
/*pm_tx_data_done_process = 0x40001d20;*/
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
//pm_update_next_tbtt = 0x40001e50;
pm_set_sleep_type = 0x40001e54;
/*pm_tx_null_data_done_process = 0x40001eb0;*/
//pm_tx_data_process = 0x40001eb4;
/*pm_attach = 0x40001eb8;*/
/*pm_coex_schm_process = 0x40001ebc;*/
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
//ieee80211_send_action_vendor_spec = 0x40002034;
ieee80211_send_mgmt = 0x40002038;
//ieee80211_auth_construct = 0x4000203c;
ieee80211_deauth_construct = 0x40002040;
ieee80211_disassoc_construct = 0x40002044;
//ieee80211_add_xrates = 0x40002058;
//ieee80211_assoc_req_construct = 0x40002060;
ieee80211_assoc_resp_construct = 0x40002064;
//ieee80211_timer_process = 0x4000208c;
cnx_coexist_timeout = 0x40002090;
//sta_recv_mgmt = 0x40002094;
ieee80211_send_probereq = 0x4000209c;
//sta_auth_open = 0x400020a0;
//sta_auth_sae = 0x400020a8;
cnx_coexist_timeout_process = 0x400020ac;
ieee80211_add_extcap = 0x400020cc;
//ieee80211_alloc_proberesp = 0x400020d8;
esp_wifi_80211_tx = 0x400020f8;
ieee80211_crypto_aes_128_cmac_decrypt = 0x40002104;
ieee80211_output_do = 0x4000210c;
ieee80211_send_nulldata = 0x40002110;
//ieee80211_setup_robust_mgmtframe = 0x40002114;
//ieee80211_mgmt_output = 0x40002118;
//ieee80211_send_deauth_no_bss = 0x40002120;
//ieee80211_tx_mgt_cb = 0x4000212c;
//sta_rx_csa = 0x40002134;
sta_send_sa_query_req = 0x40002138;
sta_send_sa_query_resp = 0x4000213c;
sta_recv_sa_query_req = 0x40002140;
sta_recv_sa_query_resp = 0x40002144;
//ieee80211_parse_beacon = 0x40002148;
ieee80211_match_security = 0x40002154;
//ieee80211_parse_rsn = 0x4000215c;
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
