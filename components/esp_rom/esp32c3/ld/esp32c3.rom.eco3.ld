/*
ESP32C3 ECO3 ROM address table
Version 3 API's imported from the ROM
*/

/*esf_buf_alloc_dynamic = 0x400015c0;*/
/*esf_buf_recycle = 0x400015c4;*/
/*lmacTxDone = 0x4000162c;*/
/*ppMapTxQueue = 0x400016d8;*/
/*rcGetSched = 0x40001764;*/
wDevCheckBlockError = 0x400017b4;
/*ppProcTxDone = 0x40001804;*/
/*sta_input = rom_sta_input;*/

/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
rom_index_to_txbbgain = 0x40001964;
rom_pbus_xpd_tx_on = 0x400019b0;
rom_set_tx_dig_gain = 0x400019f0;
rom_set_txcap_reg = 0x400019f4;
rom_txbbgain_to_index = 0x40001a0c;
rom_agc_reg_init = 0x40001a54;
rom_bb_reg_init = 0x40001a58;
rom_set_pbus_reg = 0x40001a70;
rom_phy_xpd_rf = 0x40001a78;
rom_write_txrate_power_offset = 0x40001a8c;
rom_temp_to_power = 0x40001ab4;
rom_open_i2c_xpd = 0x40001af8;
rom_tsens_read_init = 0x40001b00;
rom_tsens_code_read = 0x40001b04;
rom_tsens_dac_cal = 0x40001b10;
rom_pll_vol_cal = 0x40001b28;

/***************************************
   Group eco3_wifi
 ***************************************/

/* Functions */
wdev_is_data_in_rxlist = 0x40001b2c;
ppProcTxCallback = 0x40001b30;
ieee80211_gettid = 0x40001b34;


/***************************************
   Group eco3_bluetooth
 ***************************************/

/* Functions */
r_lld_legacy_adv_dynamic_pti_get = 0x40001b38;
r_lld_legacy_adv_dynamic_pti_process = 0x40001b3c;
r_lld_ext_adv_dynamic_pti_get = 0x40001b40;
r_lld_ext_adv_dynamic_aux_pti_process = 0x40001b44;
r_lld_ext_adv_dynamic_pti_process = 0x40001b48;
r_lld_adv_ext_pkt_prepare_set = 0x40001b4c;
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
r_sch_prog_ble_push = 0x40001b8c;
r_sch_prog_bt_push = 0x40001b90;
r_lld_init_evt_end_type_set = 0x40001b94;
r_lld_init_evt_end_type_get = 0x40001b98;
r_lld_adv_direct_adv_use_rpa_addr_state_set = 0x40001b9c;
r_lld_adv_direct_adv_use_rpa_addr_state_get = 0x40001ba0;
r_lld_init_evt_end_type_check_state_set = 0x40001ba4;
r_lld_init_evt_end_type_check_state_get = 0x40001ba8;


/***************************************
   Group eco3_phy
 ***************************************/

/* Functions */
rom_wrtie_pll_cap = 0x40001bac;
rom_set_tx_gain_mem = 0x40001bb0;
rom_bt_tx_dig_gain = 0x40001bb4;
rom_bt_get_tx_gain = 0x40001bb8;
rom_get_chan_target_power = 0x40001bbc;
rom_get_tx_gain_value = 0x40001bc0;
rom_wifi_tx_dig_gain = 0x40001bc4;
rom_wifi_get_tx_gain = 0x40001bc8;
rom_fe_i2c_reg_renew = 0x40001bcc;
rom_wifi_agc_sat_gain = 0x40001bd0;
rom_i2c_master_reset = 0x40001bd4;
rom_bt_filter_reg = 0x40001bd8;
rom_phy_bbpll_cal = 0x40001bdc;
rom_i2c_sar2_init_code = 0x40001be0;
rom_phy_param_addr = 0x40001be4;
rom_phy_reg_init = 0x40001be8;
rom_set_chan_reg = 0x40001bec;
rom_phy_wakeup_init = 0x40001bf0;
rom_phy_i2c_init1 = 0x40001bf4;
rom_tsens_temp_read = 0x40001bf8;
rom_bt_track_pll_cap = 0x40001bfc;
rom_wifi_track_pll_cap = 0x40001c00;
rom_wifi_set_tx_gain = 0x40001c04;
rom_txpwr_cal_track = 0x40001c08;
rom_tx_pwctrl_background = 0x40001c0c;
rom_bt_set_tx_gain = 0x40001c10;
rom_noise_check_loop = 0x40001c14;
rom_phy_close_rf = 0x40001c18;
rom_phy_xpd_tsens = 0x40001c1c;
rom_phy_freq_mem_backup = 0x40001c20;
rom_phy_ant_init = 0x40001c24;
rom_bt_track_tx_power = 0x40001c28;
rom_wifi_track_tx_power = 0x40001c2c;
rom_phy_dig_reg_backup = 0x40001c30;
chip726_phyrom_version_num = 0x40001c34;
/* Data (.data, .bss, .rodata) */
phy_param_rom = 0x3fcdf830;

/***************************************
   Group eco3_esp_flash
 ***************************************/

/* Functions */
PROVIDE( esp_flash_read_chip_id = 0x40001c38 );
PROVIDE( detect_spi_flash_chip = 0x40001c3c );
PROVIDE( esp_rom_spiflash_write_disable = 0x40001c40 );
