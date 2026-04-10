/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32s31.rom.net80211.ld for esp32s31
 *
 *
 * Generated from ../../rom/target/esp32s31/interface-esp32s31.yml md5sum 0cb775308c66eebf0757e406021ad5be
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x2f800c40;
esp_wifi_internal_tx = 0x2f800c44;
ampdu_dispatch = 0x2f800c48;
ampdu_dispatch_all = 0x2f800c4c;
ampdu_dispatch_as_many_as_possible = 0x2f800c50;
ampdu_dispatch_movement = 0x2f800c54;
ampdu_dispatch_upto = 0x2f800c58;
chm_is_at_home_channel = 0x2f800c5c;
cnx_node_is_existing = 0x2f800c60;
cnx_node_search = 0x2f800c64;
ic_ebuf_recycle_rx = 0x2f800c68;
ic_ebuf_recycle_tx = 0x2f800c6c;
ic_reset_rx_ba = 0x2f800c70;
ic_ebuf_alloc = 0x2f800c74;
ic_reset_extra_softap_rx_ba = 0x2f800c78;
ieee80211_align_eb = 0x2f800c7c;
ieee80211_ampdu_reorder = 0x2f800c80;
ieee80211_ampdu_start_age_timer = 0x2f800c84;
ieee80211_encap_esfbuf = 0x2f800c88;
ieee80211_is_tx_allowed = 0x2f800c8c;
ieee80211_output_pending_eb = 0x2f800c90;
ieee80211_output_process = 0x2f800c94;
ieee80211_set_tx_desc = 0x2f800c98;
ieee80211_classify = 0x2f800c9c;
ieee80211_copy_eb_header = 0x2f800ca0;
ieee80211_recycle_cache_eb = 0x2f800ca4;
ieee80211_search_node = 0x2f800ca8;
ieee80211_crypto_encap = 0x2f800cac;
ieee80211_crypto_decap = 0x2f800cb0;
ieee80211_decap = 0x2f800cb4;
ieee80211_set_tx_pti = 0x2f800cb8;
ieee80211_gettid = 0x2f800cbc;
ieee80211_post_hmac_tx = 0x2f800cc0;
ieee80211_alloc_tx_buf = 0x2f800cc4;
ieee80211_output_do = 0x2f800cc8;
ieee80211_find_elem_match = 0x2f800ccc;
ieee80211_find_ie_match = 0x2f800cd0;
ieee80211_find_elem = 0x2f800cd4;
ieee80211_find_ext_elem = 0x2f800cd8;
ieee80211_find_ie = 0x2f800cdc;
ieee80211_find_ext_ie = 0x2f800ce0;
ieee80211_merge_mbssid_profile = 0x2f800ce4;
ieee80211_encap_esfbuf_htc = 0x2f800ce8;
wifi_get_macaddr = 0x2f800cec;
wifi_rf_phy_disable = 0x2f800cf0;
wifi_rf_phy_enable = 0x2f800cf4;
wifi_is_started = 0x2f800cf8;
sta_input = 0x2f800cfc;
sta_rx_eapol = 0x2f800d00;
sta_reset_beacon_timeout = 0x2f800d04;
sta_get_beacon_timeout = 0x2f800d08;
ampdu_process_multicast_address_qos_frame = 0x2f800d0c;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x2f07ff90;
g_scan = 0x2f07ff8c;
g_chm = 0x2f07ff88;
g_ic_ptr = 0x2f07ff84;
g_hmac_cnt_ptr = 0x2f07ff80;
g_tx_cacheq_ptr = 0x2f07ff7c;
s_netstack_free = 0x2f07ff78;
s_encap_amsdu_func = 0x2f07ff74;
mesh_rxcb = 0x2f07ff70;
sta_rxcb = 0x2f07ff6c;
s_netstack_ref = 0x2f07ff68;
g_sta_connected_flag = 0x2f07ff65;
g_itwt_fid = 0x2f07ff64;
g_misc_nvs = 0x2f07ff60;
esp_test_dump_htc = 0x2f07ff5c;
