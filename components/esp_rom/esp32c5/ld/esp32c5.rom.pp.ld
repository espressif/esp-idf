/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.pp.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 4dfe6ccc311ca3575012c23e3753353c
 *
 * Compatible with ROM where ECO version equal or greater to 2.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_pp
 ***************************************/

/* Functions */
esp_pp_rom_version_get = 0x40000be8;
RC_GetBlockAckTime = 0x40000bec;
ebuf_list_remove = 0x40000bf0;
esf_buf_alloc = 0x40000bf4;
esf_buf_alloc_dynamic = 0x40000bf8;
esf_buf_recycle = 0x40000bfc;
GetAccess = 0x40000c00;
hal_mac_is_low_rate_enabled = 0x40000c04;
hal_mac_tx_get_blockack = 0x40000c08;
/*hal_mac_tx_set_ppdu = 0x40000c0c;*/
hal_mac_tx_clr_mplen = 0x40000c10;
hal_mac_get_txq_state = 0x40000c14;
hal_mac_clr_txq_state = 0x40000c18;
hal_mac_get_txq_complete = 0x40000c1c;
hal_mac_deinit_twt_tx = 0x40000c20;
hal_mac_is_dma_enable = 0x40000c24;
hal_he_get_bss_color = 0x40000c28;
hal_he_set_ersu = 0x40000c2c;
hal_he_get_mplen_addr_start = 0x40000c30;
hal_he_get_mplen_addr_end = 0x40000c34;
hal_he_set_bf_report_rate = 0x40000c38;
hal_he_set_ersu_ack_rate = 0x40000c3c;
hal_he_get_aid = 0x40000c40;
hal_random = 0x40000c44;
hal_enable_tx_statistics = 0x40000c48;
hal_enable_rx_statistics = 0x40000c4c;
hal_enable_tsf_timer = 0x40000c50;
hal_wdev_timer_target_enable = 0x40000c54;
hal_wdev_timer_set_target = 0x40000c58;
hal_tsf_timer_set_target = 0x40000c5c;
hal_tsf_clear_soc_wakeup_request = 0x40000c60;
hal_get_tsf_time = 0x40000c64;
hal_get_sta_tsf = 0x40000c68;
ic_get_trc = 0x40000c74;
/* ic_mac_deinit = 0x40000c78; */
/* ic_mac_init = 0x40000c7c; */
ic_interface_enabled = 0x40000c80;
is_lmac_idle = 0x40000c84;
ic_get_he_rts_threshold_bytes = 0x40000c88;
lmacAdjustTimestamp = 0x40000c8c;
lmacDiscardAgedMSDU = 0x40000c90;
/*lmacDiscardMSDU = 0x40000c94;*/
lmacEndFrameExchangeSequence = 0x40000c98;
lmacIsIdle = 0x40000c9c;
lmacIsLongFrame = 0x40000ca0;
lmacMSDUAged = 0x40000ca4;
lmacPostTxComplete = 0x40000ca8;
lmacProcessAllTxTimeout = 0x40000cac;
lmacProcessCollisions = 0x40000cb0;
lmacProcessRxSucData = 0x40000cb4;
lmacReachLongLimit = 0x40000cb8;
lmacReachShortLimit = 0x40000cbc;
lmacRecycleMPDU = 0x40000cc0;
lmacRxDone = 0x40000cc4;
lmacSetTxFrame = 0x40000cc8;
lmacTxDone = 0x40000ccc;
/*lmacTxFrame = 0x40000cd0;*/
/* lmacDisableTransmit = 0x40000cd4; */
lmacDiscardFrameExchangeSequence = 0x40000cd8;
lmacProcessCollision = 0x40000cdc;
lmacProcessAckTimeout = 0x40000ce0;
lmacProcessShortRetryFail = 0x40000ce4;
lmacProcessCollisions_task = 0x40000ce8;
lmacProcessTxRtsError = 0x40000cec;
lmacProcessTxError = 0x40000cf0;
lmacProcessCtsTimeout = 0x40000cf4;
lmacProcessLongRetryFail = 0x40000cf8;
lmacRetryTxFrame = 0x40000cfc;
lmacEndRetryAMPDUFail = 0x40000d00;
lmacProcessTxSuccess = 0x40000d04;
lmacProcessTxComplete = 0x40000d08;
lmacProcessLongFrameSuccess = 0x40000d0c;
lmacProcessShortFrameSuccess = 0x40000d10;
lmacProcessModemStateRxBeacon = 0x40000d14;
mac_tx_set_duration = 0x40000d18;
mac_tx_set_plcp0 = 0x40000d1c;
mac_tx_set_plcp1 = 0x40000d20;
mac_tx_set_plcp2 = 0x40000d24;
mac_tx_set_len = 0x40000d28;
mac_tx_set_htsig = 0x40000d2c;
mac_tx_set_vhtsig = 0x40000d30;
mac_tx_set_hesig = 0x40000d34;
mac_tx_set_tb = 0x40000d38;
mac_tx_set_mplen = 0x40000d3c;
mac_tx_set_txop_q = 0x40000d40;
mac_tx_set_pti = 0x40000d44;
/*pm_check_state = 0x40000d48;*/
pm_disable_dream_timer = 0x40000d4c;
pm_disable_sleep_delay_timer = 0x40000d50;
pm_dream = 0x40000d54;
pm_mac_wakeup = 0x40000d58;
pm_mac_sleep = 0x40000d5c;
pm_enable_active_timer = 0x40000d60;
pm_enable_sleep_delay_timer = 0x40000d64;
pm_local_tsf_process = 0x40000d68;
//pm_set_beacon_filter = 0x40000d6c;
/*pm_is_in_wifi_slice_threshold = 0x40000d70;*/
pm_is_waked = 0x40000d74;
/*pm_keep_alive = 0x40000d78;*/
//pm_on_beacon_rx = 0x40000d7c;
pm_on_data_rx = 0x40000d80;
pm_on_data_tx = 0x40000d84;
pm_on_tbtt = 0x40000d88;
pm_on_isr_sleep = 0x40000d8c;
pm_on_isr = 0x40000d90;
pm_on_isr_set_twt_target = 0x40000d94;
pm_on_isr_twt_wake = 0x40000d98;
pm_on_tsf_timer = 0x40000d9c;
pm_on_twt_force_tx = 0x40000da0;
//pm_parse_beacon = 0x40000da4;
/*pm_process_tim = 0x40000da8;*/
//pm_rx_beacon_process = 0x40000dac;
pm_rx_data_process = 0x40000db0;
pm_sleep = 0x40000db4;
pm_sleep_for = 0x40000db8;
//pm_tbtt_process = 0x40000dbc;
pm_tx_data_done_process = 0x40000dc0;
pm_allow_tx = 0x40000dc4;
pm_extend_tbtt_adaptive_servo = 0x40000dc8;
pm_scale_listen_interval = 0x40000dcc;
pm_parse_mbssid_element = 0x40000dd0;
pm_disconnected_wake = 0x40000dd4;
/*pm_tx_data_process = 0x40000dd8;*/
pm_is_twt_awake = 0x40000ddc;
pm_enable_twt_keep_alive = 0x40000de0;
pm_twt_on_tsf_timer = 0x40000de4;
/*pm_twt_process = 0x40000de8;*/
pm_is_twt_start = 0x40000dec;
pm_twt_set_target_wdev_time = 0x40000df0;
pm_twt_set_target_tsf = 0x40000df4;
pm_enable_twt_keep_alive_timer = 0x40000df8;
/*pm_mac_try_enable_modem_state = 0x40000dfc;*/
pm_beacon_monitor_tbtt_timeout_process = 0x40000e00;
//pm_update_next_tbtt = 0x40000e04;
pm_twt_disallow_tx = 0x40000e08;
//pm_clear_wakeup_signal = 0x40000e0c;
//pm_mac_disable_tsf_tbtt_soc_wakeup = 0x40000e10;
//pm_mac_disable_tsf_tbtt_modem_wakeup = 0x40000e14;
//pm_mac_enable_tsf_tbtt_soc_wakeup = 0x40000e18;
//pm_mac_enable_tsf_tbtt_modem_wakeup = 0x40000e1c;
//pm_mac_modem_params_rt_update = 0x40000e20;
pm_update_at_next_beacon = 0x40000e24;
tbtt_adaptive_setup = 0x40000e28;
tbtt_adaptive_servo = 0x40000e2c;
ppAMPDU2Normal = 0x40000e30;
ppHEAMPDU2Normal = 0x40000e34;
ppAssembleAMPDU = 0x40000e38;
ppCalFrameTimes = 0x40000e3c;
ppCalSubFrameLength = 0x40000e40;
ppCalTxAMPDULength = 0x40000e44;
ppCheckTxAMPDUlength = 0x40000e48;
ppDequeueRxq_Locked = 0x40000e4c;
ppDequeueTxQ = 0x40000e50;
ppEmptyDelimiterLength = 0x40000e54;
ppEnqueueRxq = 0x40000e58;
ppEnqueueTxDone = 0x40000e5c;
ppGetTxframe = 0x40000e60;
/*ppMapTxQueue = 0x40000e64;*/
//ppProcTxSecFrame = 0x40000e68;
ppProcessRxPktHdr = 0x40000e6c;
/*ppProcessTxQ = 0x40000e70;*/
ppRecordBarRRC = 0x40000e74;
ppRecycleAmpdu = 0x40000e78;
ppRecycleRxPkt = 0x40000e7c;
ppResortTxAMPDU = 0x40000e80;
ppResumeTxAMPDU = 0x40000e84;
/*ppRxFragmentProc = 0x40000e88;*/
/* ppRxPkt = 0x40000e8c; */
/* ppRxProtoProc = 0x40000e90; */
ppSearchTxQueue = 0x40000e94;
ppSearchTxframe = 0x40000e98;
ppSelectNextQueue = 0x40000e9c;
ppSubFromAMPDU = 0x40000ea0;
ppTask = 0x40000ea4;
ppTxPkt = 0x40000ea8;
ppTxProtoProc = 0x40000eac;
ppTxqUpdateBitmap = 0x40000eb0;
pp_coex_tx_request = 0x40000eb4;
pp_hdrsize = 0x40000eb8;
pp_post = 0x40000ebc;
pp_process_hmac_waiting_txq = 0x40000ec0;
ppDequeueTxDone_Locked = 0x40000ec4;
ppProcTxDone = 0x40000ec8;
ppMapWaitTxq = 0x40000ecc;
ppProcessWaitingQueue = 0x40000ed0;
ppDisableQueue = 0x40000ed4;
//ppCalVHTDeliNum = 0x40000ed8;
ppCalTxVHTSMPDULength = 0x40000edc;
ppCheckTxRTS = 0x40000ee0;
/*ppProcessLifeTime = 0x40000ee4;*/
ppProcTxCallback = 0x40000ee8;
ppCalPreFecPaddingFactor = 0x40000eec;
ppCalDeliNum = 0x40000ef0;
ppRemoveHTC = 0x40000ef4;
ppCheckTxHEAMPDUlength = 0x40000ef8;
ppCertSetRate = 0x40000efc;
ppSelectTxFormat = 0x40000f00;
ppCalTxHEAMPDULength = 0x40000f04;
pp_coex_tx_release = 0x40000f08;
ppAdd2AMPDUTail = 0x40000f0c;
ppDirectRecycleAmpdu = 0x40000f10;
ppRegressAmpdu = 0x40000f14;
pp_timer_do_process = 0x40000f18;
rcGetAmpduSched = 0x40000f1c;
rcUpdateRxDone = 0x40000f20;
rc_get_trc = 0x40000f24;
rc_get_trc_by_index = 0x40000f28;
rcAmpduLowerRate = 0x40000f2c;
rcampduuprate = 0x40000f30;
rcClearCurAMPDUSched = 0x40000f34;
rcClearCurSched = 0x40000f38;
rcClearCurStat = 0x40000f3c;
/*rcGetSched = 0x40000f40;*/
rcLowerSched = 0x40000f44;
rcSetTxAmpduLimit = 0x40000f48;
rcTxUpdatePer = 0x40000f4c;
rcUpdateAckSnr = 0x40000f50;
rcUpdateRate = 0x40000f54;
rcUpdateTxDone = 0x40000f58;
rcUpdateTxDoneAmpdu2 = 0x40000f5c;
rcUpSched = 0x40000f60;
rcReachRetryLimit = 0x40000f64;
rcGetDCMMaxRate = 0x40000f68;
rcGetSMPDURate = 0x40000f6c;
rcGetRate = 0x40000f70;
rx11AXRate2AMPDULimit = 0x40000f74;
rx11NRate2AMPDULimit = 0x40000f78;
rssi_margin = 0x40000f7c;
TRC_AMPDU_PER_DOWN_THRESHOLD = 0x40000f80;
TRC_AMPDU_PER_UP_THRESHOLD = 0x40000f84;
TRC_PER_IS_GOOD = 0x40000f88;
trc_calc_duration = 0x40000f8c;
trc_isTxAmpduOperational = 0x40000f90;
trc_onAmpduOp = 0x40000f94;
trc_SetTxAmpduState = 0x40000f98;
trc_tid_isTxAmpduOperational = 0x40000f9c;
trcAmpduSetState = 0x40000fa0;
trc_set_bf_report_rate = 0x40000fa4;
trc_onPPTxDone = 0x40000fa8;
wDevCheckBlockError = 0x40000fac;
wDev_AppendRxBlocks = 0x40000fb0;
wDev_DiscardFrame = 0x40000fb4;
wDev_GetNoiseFloor = 0x40000fb8;
wDev_IndicateAmpdu = 0x40000fbc;
wDev_IndicateFrame = 0x40000fc0;
wdev_mac_reg_load = 0x40000fc4;
wdev_mac_reg_store = 0x40000fc8;
wdev_mac_special_reg_load = 0x40000fcc;
wdev_mac_special_reg_store = 0x40000fd0;
wdev_mac_wakeup = 0x40000fd4;
wdev_mac_sleep = 0x40000fd8;
wDev_ProcessFiq = 0x40000fdc;
/*wDev_ProcessRxSucData = 0x40000fe0;*/
wdevProcessRxSucDataAll = 0x40000fe4;
wdev_csi_len_align = 0x40000fe8;
wDev_IndicateBeaconMemoryFrame = 0x40000fec;
wDev_ProcessBeaconMemoryRxSuc = 0x40000ff0;
wdevProcessModemStateRxBeacon = 0x40000ff4;
wdev_is_data_in_rxlist = 0x40000ff8;
wdev_process_tbtt = 0x40000ffc;
wdev_process_tsf_timer = 0x40001000;
wdev_process_beacon_filter = 0x40001004;
wdev_process_mac_modem_beacon_miss = 0x40001008;
config_is_cache_tx_buf_enabled = 0x4000100c;
esp_test_disable_tx_statistics = 0x40001018;
esp_test_enable_tx_statistics = 0x4000101c;
esp_test_clr_tx_statistics = 0x40001020;
esp_test_get_tx_statistics = 0x40001024;
esp_test_clr_tx_tb_statistics = 0x40001028;
esp_test_get_tx_tb_statistics = 0x4000102c;
/*esp_test_tx_process_complete = 0x40001030;*/
esp_test_tx_process_txq_state = 0x40001034;
esp_test_tx_enab_statistics = 0x40001038;
esp_test_tx_tb_complete = 0x4000103c;
esp_test_tx_count_retry = 0x40001040;
esp_test_tx_count_collision = 0x40001044;
esp_test_tx_count_timeout = 0x40001048;
esp_test_rx_process_complete = 0x4000104c;
esp_test_clr_rx_statistics = 0x40001050;
esp_test_get_rx_statistics = 0x40001054;
esp_test_set_rx_error_occurs = 0x40001058;
esp_test_get_rx_error_occurs = 0x4000105c;
esp_test_clr_rx_error_occurs = 0x40001060;
esp_test_disable_rx_statistics = 0x40001064;
esp_test_enable_rx_statistics = 0x40001068;
esp_test_rx_parse_mu = 0x4000106c;
esp_test_get_rx_mu_statistics = 0x40001070;
esp_test_clr_rx_mu_statistics = 0x40001074;
esp_test_enable_rx_mu_statistics = 0x40001078;
esp_test_disable_rx_mu_statistics = 0x4000107c;
test_tx_fail_statistics = 0x40001080;
test_tx_succ_statistics = 0x40001084;
test_rx_process_complete_noeb = 0x40001088;
test_rx_process_complete_retry = 0x4000108c;
test_mumimo_get_heltf_num = 0x40001090;
test_mimo_update_user_info = 0x40001094;
test_parse_rx_mu_mimo = 0x40001098;
test_nonmimo_update_user_info = 0x4000109c;
test_rx_parse_mumimo_complete_sigb = 0x400010a0;
test_get_nonmumimo_common = 0x400010a4;
test_rx_parse_nonmumimo_complete_sigb = 0x400010a8;
test_rx_parse_nonmumimo = 0x400010ac;
wifi_he_get_hetb_tid_bitmap = 0x400010b0;
wifi_he_get_hetb_tid_bawin = 0x400010b4;
ht_get_min_subframe_len = 0x400010b8;
vht_get_min_subframe_len = 0x400010bc;
he_get_min_subframe_len = 0x400010c0;
he_get_min_subframe_len_dcm = 0x400010c4;
is_ndpa_to_dut = 0x400010c8;
is_use_muedca = 0x400010cc;
pwr_hal_clear_mac_modem_state_wakeup_protect_signal = 0x400010d0;
get_estimated_batime = 0x400010d4;
get_sublen_offset = 0x400010d8;
/* Data (.data, .bss, .rodata) */
our_instances_ptr = 0x4004ffe0;
pTxRx = 0x4085ff74;
lmacConfMib_ptr = 0x4085ff70;
our_wait_eb = 0x4085ff6c;
our_tx_eb = 0x4085ff68;
pp_wdev_funcs = 0x4085ff64;
g_osi_funcs_p = 0x4085ff60;
wDevCtrl_ptr = 0x4085ff5c;
g_wdev_last_desc_reset_ptr = 0x4004ffdc;
wDevMacSleep_ptr = 0x4085ff58;
g_lmac_cnt_ptr = 0x4085ff54;
our_controls_ptr = 0x4004ffd8;
pp_sig_cnt_ptr = 0x4085ff50;
g_eb_list_desc_ptr = 0x4085ff4c;
s_fragment_ptr = 0x4085ff48;
if_ctrl_ptr = 0x4085ff44;
g_intr_lock_mux = 0x4085ff40;
g_wifi_global_lock = 0x4085ff3c;
s_wifi_queue = 0x4085ff38;
pp_task_hdl = 0x4085ff34;
s_pp_task_create_sem = 0x4085ff30;
s_pp_task_del_sem = 0x4085ff2c;
xphyQueue = 0x4085ff28;
ap_no_lr_ptr = 0x4085ff24;
rc11ASchedTbl_ptr = 0x4085ff20;
rc11BSchedTbl_ptr = 0x4085ff1c;
rc11NSchedTbl_ptr = 0x4085ff18;
rcLoRaSchedTbl_ptr = 0x4085ff14;
BasicOFDMSched_ptr = 0x4085ff10;
trc_ctl_ptr = 0x4085ff0c;
g_wifi_menuconfig_ptr = 0x4085ff08;
g_pm_cnt_ptr = 0x4085ff04;
g_pm_ptr = 0x4085ff00;
g_pm_cfg_ptr = 0x4085fefc;
g_pm_twt_ptr = 0x4085fef8;
g_esp_mesh_quick_funcs_ptr = 0x4085fef4;
g_txop_queue_status_ptr = 0x4085fef0;
g_mac_sleep_en_ptr = 0x4085feec;
g_mesh_is_root_ptr = 0x4085fee8;
g_mesh_topology_ptr = 0x4085fee4;
g_mesh_init_ps_type_ptr = 0x4085fee0;
g_mesh_is_started_ptr = 0x4085fedc;
g_config_func = 0x4085fed8;
g_net80211_tx_func = 0x4085fed4;
g_timer_func = 0x4085fed0;
g_dbg_interp_tsf = 0x4085fecc;
g_dbg_interp_tsf_end = 0x4085fec8;
g_dbg_closrf_tsf = 0x4085fec4;
g_dbg_closrf_idx = 0x4085fec0;
g_dbg_closrf_blk = 0x4085febc;
s_michael_mic_failure_cb = 0x4085feb8;
wifi_sta_rx_probe_req = 0x4085feb4;
g_tx_done_cb_func = 0x4085feb0;
g_per_conn_trc = 0x4085fe58;
rx_beacon_count = 0x4085fe54;
rx_beacon_tim_count = 0x4085fe50;
rx_beacon_tim_udata = 0x4085fe4c;
rx_beacon_tim_udata_bitmap = 0x4085fe48;
rx_beacon_tim_bdata = 0x4085fe44;
rx_beacon_tim_bdata_bitmapctl = 0x4085fe40;
rx_beacon_tim_bdata_bitmap_trans = 0x4085fe3c;
rx_beacon_tim_bdata_bitmap_mbssid_self = 0x4085fe38;
rx_beacon_tim_bdata_bitmap_mbssid_other = 0x4085fe34;
rx_beacon_dtim_tim = 0x4085fe30;
rx_beacon_dtim_tim_mcast = 0x4085fe2c;
ampdu_delay = 0x4085fe28;
s_ht_ampdu_density_us = 0x4085fc8a;
s_ht_ampdu_density = 0x4085fc89;
s_running_phy_type = 0x4085fc88;
complete_ena_tb_seqno = 0x4085fe24;
complete_ena_tb_final = 0x4085fe20;
complete_ena_tb_count = 0x4085fe1c;
s_he_min_len_bytes = 0x4085fde0;
s_he_dcm_min_len_bytes = 0x4085fdc0;
g_wdev_dbg_rx_ptr = 0x4085fdbc;
s_mplen_txq_bitmap = 0x4085fdac;
s_mplen_be_high_bitmap = 0x4085fda8;
s_phy_get_max_pwr = 0x4085fd50;
esp_wifi_cert_tx_mcs = 0x4085fc84;
esp_wifi_cert_tx_bcc = 0x4085fc80;
esp_wifi_cert_tx_ltf = 0x4085fc7c;
esp_wifi_cert_tx_gi = 0x4085fc78;
esp_wifi_cert_tx_nss = 0x4085fc74;
esp_wifi_cert_tx_ersu = 0x4085fd4c;
g_mac_deinit_count = 0x4085fd48;
g_mac_deinit_rxing = 0x4085fd45;
g_mac_deinit_txing = 0x4085fd44;
s_bars = 0x4085fd34;
esp_test_rx_statistics = 0x4085fd2c;
esp_test_rx_mu_statistics = 0x4085fd28;
esp_test_tx_statistics_aci_bitmap = 0x4085fd24;
esp_test_tx_statistics = 0x4085fd14;
esp_test_tx_tb_statistics = 0x4085fd04;
esp_test_tx_fail_statistics = 0x4085fca4;
esp_test_mu_print_ru_allocation = 0x4085fca0;
sigb_ru_allocation_user_num = 0x4004ffc8;
sigb_common_ru_allocation = 0x4004ff38;
mu_mimo_special_cfg_user_num_2 = 0x4004fee8;
mu_mimo_special_cfg_user_num_3 = 0x4004fe80;
mu_mimo_special_cfg_user_num_4 = 0x4004fe28;
mu_mimo_special_cfg_user_num_5 = 0x4004fdf0;
mu_mimo_special_cfg_user_num_6 = 0x4004fdd0;
mu_mimo_special_cfg_user_num_7 = 0x4004fdc0;
mu_mimo_special_cfg_user_num_8 = 0x4004fdb8;
esp_test_rx_error_occurs = 0x4085fc9c;
g_pp_tx_pkt_num = 0x4085fc98;
he_max_apep_length = 0x4004fd40;
g_pp_timer_info_ptr = 0x4085fc94;
g_rts_threshold_bytes_ptr = 0x4085fc90;
g_he_max_apep_length_tab_ptr = 0x4085fc8c;

/* ROM function interface esp32c5.rom.eco1.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 4dfe6ccc311ca3575012c23e3753353c
 *
 * Compatible with ROM where ECO version equal or greater to 2.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_pp_eco1
 ***************************************/

/* Functions */
pm_coex_schm_overall_period_get = 0x40001504;
//pm_coex_pwr_update = 0x40001508;
ppRemoveHEAMPDUflags = 0x4000150c;
tsf_hal_get_tbtt_interval = 0x40001510;
//pm_beacon_monitor_tbtt_start = 0x40001514;
pm_save_tbtt_info = 0x40001518;
//pm_get_tbtt_count = 0x4000151c;
tsf_hal_get_time = 0x40001520;
tsf_hal_get_counter_value = 0x40001524;

/* ROM function interface esp32c5.rom.eco2.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum 4dfe6ccc311ca3575012c23e3753353c
 *
 * Compatible with ROM where ECO version equal or greater to 2.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_pp_eco2
 ***************************************/

/* Functions */
/*pm_enable_max_idle_timer = 0x400015b8;*/
pm_beacon_offset_is_enabled = 0x400015bc;
pm_beacon_offset_is_sampling = 0x400015c0;
pm_beacon_offset_add_total_counter = 0x400015c4;
pm_beacon_offset_add_loss_counter = 0x400015c8;
//pm_beacon_offset_check = 0x400015cc;
//pm_beacon_offset_get_average = 0x400015d0;
//pm_beacon_offset_get_expect = 0x400015d4;
//pm_beacon_offset_get_params = 0x400015d8;
/* Data (.data, .bss, .rodata) */
s_tbttstart_ptr = 0x4085fc68;
s_pm_beacon_offset_ptr = 0x4085fc64;
s_pm_beacon_offset_config_ptr = 0x4085fc60;
