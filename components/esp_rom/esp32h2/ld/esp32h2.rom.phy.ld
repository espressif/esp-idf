/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32h2.rom.phy.ld for esp32h2
 *
 *
 * Generated from ./target/esp32h2/interface-esp32h2.yml md5sum 5d28ef7e188c1740008212a69e90de1e
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_phy
 ***************************************/

/* Functions */
phy_get_romfuncs = 0x40000e38;
rom_abs_temp = 0x40000e3c;
rom_bb_bss_cbw40_dig = 0x40000e40;
rom_bb_wdg_test_en = 0x40000e44;
rom_bb_wdt_get_status = 0x40000e48;
rom_bb_wdt_int_enable = 0x40000e4c;
rom_bb_wdt_rst_enable = 0x40000e50;
rom_bb_wdt_timeout_clear = 0x40000e54;
rom_cbw2040_cfg = 0x40000e58;
rom_check_noise_floor = 0x40000e5c;
rom_chip_i2c_readReg = 0x40000e60;
rom_chip_i2c_writeReg = 0x40000e64;
rom_correct_rf_ana_gain = 0x40000e68;
rom_dc_iq_est = 0x40000e6c;
rom_disable_agc = 0x40000e70;
rom_en_pwdet = 0x40000e74;
rom_enable_agc = 0x40000e78;
rom_get_bbgain_db = 0x40000e7c;
rom_get_data_sat = 0x40000e80;
rom_get_i2c_read_mask = 0x40000e84;
rom_get_pwctrl_correct = 0x40000e88;
rom_get_rf_gain_qdb = 0x40000e8c;
rom_i2c_readReg = 0x40000e90;
rom_i2c_readReg_Mask = 0x40000e94;
rom_i2c_writeReg = 0x40000e98;
rom_i2c_writeReg_Mask = 0x40000e9c;
rom_index_to_txbbgain = 0x40000ea0;
rom_iq_est_disable = 0x40000ea4;
rom_iq_est_enable = 0x40000ea8;
rom_linear_to_db = 0x40000eac;
rom_loopback_mode_en = 0x40000eb0;
rom_mhz2ieee = 0x40000eb4;
rom_noise_floor_auto_set = 0x40000eb8;
rom_pbus_debugmode = 0x40000ebc;
rom_pbus_force_mode = 0x40000ec0;
rom_pbus_force_test = 0x40000ec4;
rom_pbus_rd = 0x40000ec8;
rom_pbus_rd_addr = 0x40000ecc;
rom_pbus_rd_shift = 0x40000ed0;
rom_pbus_set_dco = 0x40000ed4;
rom_pbus_set_rxgain = 0x40000ed8;
rom_pbus_workmode = 0x40000edc;
rom_pbus_xpd_rx_off = 0x40000ee0;
rom_pbus_xpd_rx_on = 0x40000ee4;
rom_pbus_xpd_tx_off = 0x40000ee8;
rom_pbus_xpd_tx_on = 0x40000eec;
rom_phy_byte_to_word = 0x40000ef0;
rom_phy_disable_cca = 0x40000ef4;
rom_phy_enable_cca = 0x40000ef8;
rom_phy_get_noisefloor = 0x40000efc;
rom_phy_get_rx_freq = 0x40000f00;
rom_phy_set_bbfreq_init = 0x40000f04;
rom_pow_usr = 0x40000f08;
rom_pwdet_sar2_init = 0x40000f0c;
rom_read_hw_noisefloor = 0x40000f10;
rom_read_sar_dout = 0x40000f14;
rom_set_cal_rxdc = 0x40000f18;
rom_set_chan_cal_interp = 0x40000f1c;
rom_set_loopback_gain = 0x40000f20;
rom_set_noise_floor = 0x40000f24;
rom_set_rxclk_en = 0x40000f28;
rom_set_tx_dig_gain = 0x40000f2c;
rom_set_txcap_reg = 0x40000f30;
rom_set_txclk_en = 0x40000f34;
rom_spur_cal = 0x40000f38;
rom_spur_reg_write_one_tone = 0x40000f3c;
rom_target_power_add_backoff = 0x40000f40;
rom_tx_pwctrl_bg_init = 0x40000f44;
rom_txbbgain_to_index = 0x40000f48;
rom_wifi_11g_rate_chg = 0x40000f4c;
rom_write_gain_mem = 0x40000f50;
chip726_phyrom_version = 0x40000f54;
rom_disable_wifi_agc = 0x40000f58;
rom_enable_wifi_agc = 0x40000f5c;
rom_set_tx_gain_table = 0x40000f60;
rom_bt_index_to_bb = 0x40000f64;
rom_bt_bb_to_index = 0x40000f68;
rom_wr_bt_tx_atten = 0x40000f6c;
rom_wr_bt_tx_gain_mem = 0x40000f70;
rom_spur_coef_cfg = 0x40000f74;
rom_bb_bss_cbw40 = 0x40000f78;
rom_set_cca = 0x40000f7c;
rom_tx_paon_set = 0x40000f80;
rom_i2cmst_reg_init = 0x40000f84;
rom_iq_corr_enable = 0x40000f88;
rom_fe_reg_init = 0x40000f8c;
rom_agc_reg_init = 0x40000f90;
rom_bb_reg_init = 0x40000f94;
rom_mac_enable_bb = 0x40000f98;
rom_bb_wdg_cfg = 0x40000f9c;
rom_force_txon = 0x40000fa0;
rom_fe_txrx_reset = 0x40000fa4;
rom_set_rx_comp = 0x40000fa8;
rom_set_pbus_reg = 0x40000fac;
rom_write_chan_freq = 0x40000fb0;
rom_phy_xpd_rf = 0x40000fb4;
rom_set_xpd_sar = 0x40000fb8;
rom_write_dac_gain2 = 0x40000fbc;
rom_get_target_power_offset = 0x40000fc0;
rom_write_txrate_power_offset = 0x40000fc4;
rom_get_rate_fcc_index = 0x40000fc8;
rom_get_rate_target_power = 0x40000fcc;
rom_write_wifi_dig_gain = 0x40000fd0;
rom_bt_correct_rf_ana_gain = 0x40000fd4;
rom_pkdet_vol_start = 0x40000fd8;
rom_read_sar2_code = 0x40000fdc;
rom_get_sar2_vol = 0x40000fe0;
rom_get_pll_vol = 0x40000fe4;
rom_get_phy_target_power = 0x40000fe8;
rom_temp_to_power = 0x40000fec;
rom_phy_track_pll_cap = 0x40000ff0;
rom_phy_pwdet_always_en = 0x40000ff4;
rom_phy_pwdet_onetime_en = 0x40000ff8;
rom_get_i2c_mst0_mask = 0x40000ffc;
rom_get_i2c_hostid = 0x40001000;
rom_enter_critical_phy = 0x40001004;
rom_exit_critical_phy = 0x40001008;
rom_chip_i2c_readReg_org = 0x4000100c;
rom_i2c_paral_set_mst0 = 0x40001010;
rom_i2c_paral_set_read = 0x40001014;
rom_i2c_paral_read = 0x40001018;
rom_i2c_paral_write = 0x4000101c;
rom_i2c_paral_write_num = 0x40001020;
rom_i2c_paral_write_mask = 0x40001024;
rom_bb_bss_cbw40_ana = 0x40001028;
rom_chan_to_freq = 0x4000102c;
rom_open_i2c_xpd = 0x40001030;
rom_dac_rate_set = 0x40001034;
rom_tsens_read_init = 0x40001038;
rom_tsens_code_read = 0x4000103c;
rom_tsens_index_to_dac = 0x40001040;
rom_tsens_index_to_offset = 0x40001044;
rom_tsens_dac_cal = 0x40001048;
rom_code_to_temp = 0x4000104c;
rom_write_pll_cap_mem = 0x40001050;
rom_pll_correct_dcap = 0x40001054;
rom_phy_en_hw_set_freq = 0x40001058;
rom_phy_dis_hw_set_freq = 0x4000105c;
rom_pll_vol_cal = 0x40001060;
rom_wrtie_pll_cap = 0x40001064;
rom_set_tx_gain_mem = 0x40001068;
rom_bt_tx_dig_gain = 0x4000106c;
rom_bt_get_tx_gain = 0x40001070;
rom_get_chan_target_power = 0x40001074;
rom_get_tx_gain_value = 0x40001078;
rom_wifi_tx_dig_gain = 0x4000107c;
rom_wifi_get_tx_gain = 0x40001080;
rom_fe_i2c_reg_renew = 0x40001084;
rom_wifi_agc_sat_gain = 0x40001088;
rom_i2c_master_reset = 0x4000108c;
rom_bt_filter_reg = 0x40001090;
rom_phy_bbpll_cal = 0x40001094;
rom_i2c_sar2_init_code = 0x40001098;
rom_phy_param_addr = 0x4000109c;
rom_phy_reg_init = 0x400010a0;
rom_set_chan_reg = 0x400010a4;
rom_phy_wakeup_init = 0x400010a8;
rom_phy_i2c_init1 = 0x400010ac;
rom_tsens_temp_read = 0x400010b0;
rom_bt_track_pll_cap = 0x400010b4;
rom_wifi_track_pll_cap = 0x400010b8;
rom_wifi_set_tx_gain = 0x400010bc;
rom_txpwr_cal_track = 0x400010c0;
rom_tx_pwctrl_background = 0x400010c4;
rom_bt_set_tx_gain = 0x400010c8;
rom_noise_check_loop = 0x400010cc;
rom_phy_close_rf = 0x400010d0;
rom_phy_xpd_tsens = 0x400010d4;
rom_phy_freq_mem_backup = 0x400010d8;
rom_phy_ant_init = 0x400010dc;
rom_bt_track_tx_power = 0x400010e0;
rom_wifi_track_tx_power = 0x400010e4;
rom_phy_dig_reg_backup = 0x400010e8;
chip726_phyrom_version_num = 0x400010ec;
/* Data (.data, .bss, .rodata) */
phy_param_rom = 0x4087fcd8;
