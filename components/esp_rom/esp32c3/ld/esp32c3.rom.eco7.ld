/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
esf_buf_alloc = 0x400015bc;
esf_buf_alloc_dynamic = 0x400015c0;
esf_buf_recycle = 0x400015c4;
/*hal_mac_tx_set_ppdu = 0x400015d4;*/
ic_mac_deinit = 0x400015dc;
lmacDiscardMSDU = 0x400015f4;
/*lmacSetTxFrame = 0x40001628;*/
lmacTxDone = 0x4000162c;
lmacTxFrame = 0x40001630;
mac_tx_set_htsig = 0x40001638;
mac_tx_set_plcp1 = 0x40001640;
pm_check_state = 0x40001648;
/*pm_on_beacon_rx = 0x4000167c;*/
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
/*rcUpdateTxDone = 0x4000177c;*/
wDevCheckBlockError = 0x400017b4;
/* wDev_IndicateFrame = 0x400017c8;*/
wDev_ProcessFiq = 0x400017f0;
/*wDev_ProcessRxSucData = 0x400017f4;*/
/*ppProcTxDone = 0x40001804;*/
/*pm_tx_data_done_process = 0x40001808;*/
ppMapWaitTxq = 0x40001810;
/*ieee80211_encap_esfbuf = 0x4000185c;*/
/*sta_input = 0x40001870;*/
ieee80211_crypto_decap = 0x4000189c;
ieee80211_decap = 0x400018a0;
/*coex_core_timer_idx_get = 0x400018d0;*/
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
