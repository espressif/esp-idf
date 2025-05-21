/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ROM function interface
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x40000a80;
esp_wifi_internal_tx = 0x40000a84;
ampdu_dispatch = 0x40000a88;
ampdu_dispatch_all = 0x40000a8c;
ampdu_dispatch_as_many_as_possible = 0x40000a90;
ampdu_dispatch_movement = 0x40000a94;
ampdu_dispatch_upto = 0x40000a98;
chm_is_at_home_channel = 0x40000a9c;
cnx_node_is_existing = 0x40000aa0;
cnx_node_search = 0x40000aa4;
ic_ebuf_recycle_rx = 0x40000aa8;
ic_ebuf_recycle_tx = 0x40000aac;
ic_reset_rx_ba = 0x40000ab0;
ic_ebuf_alloc = 0x40000ab4;
ic_reset_extra_softap_rx_ba = 0x40000ab8;
ieee80211_align_eb = 0x40000abc;
ieee80211_ampdu_reorder = 0x40000ac0;
ieee80211_ampdu_start_age_timer = 0x40000ac4;
ieee80211_encap_esfbuf = 0x40000ac8;
ieee80211_is_tx_allowed = 0x40000acc;
ieee80211_output_pending_eb = 0x40000ad0;
ieee80211_output_process = 0x40000ad4;
ieee80211_set_tx_desc = 0x40000ad8;
ieee80211_classify = 0x40000adc;
ieee80211_copy_eb_header = 0x40000ae0;
ieee80211_recycle_cache_eb = 0x40000ae4;
ieee80211_search_node = 0x40000ae8;
ieee80211_crypto_encap = 0x40000aec;
ieee80211_crypto_decap = 0x40000af0;
ieee80211_decap = 0x40000af4;
ieee80211_set_tx_pti = 0x40000af8;
ieee80211_gettid = 0x40000afc;
ieee80211_post_hmac_tx = 0x40000b00;
ieee80211_alloc_tx_buf = 0x40000b04;
ieee80211_output_do = 0x40000b08;
ieee80211_find_elem_match = 0x40000b0c;
ieee80211_find_ie_match = 0x40000b10;
ieee80211_find_elem = 0x40000b14;
ieee80211_find_ext_elem = 0x40000b18;
ieee80211_find_ie = 0x40000b1c;
ieee80211_find_ext_ie = 0x40000b20;
ieee80211_merge_mbssid_profile = 0x40000b24;
ieee80211_encap_esfbuf_htc = 0x40000b28;
wifi_get_macaddr = 0x40000b2c;
wifi_rf_phy_disable = 0x40000b30;
wifi_rf_phy_enable = 0x40000b34;
wifi_is_started = 0x40000b38;
sta_input = 0x40000b3c;
sta_rx_eapol = 0x40000b40;
sta_reset_beacon_timeout = 0x40000b44;
sta_get_beacon_timeout = 0x40000b48;
ampdu_process_multicast_address_qos_frame = 0x40000b4c;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x4084ffac;
g_scan = 0x4084ffa8;
g_chm = 0x4084ffa4;
g_ic_ptr = 0x4084ffa0;
g_hmac_cnt_ptr = 0x4084ff9c;
g_tx_cacheq_ptr = 0x4084ff98;
s_netstack_free = 0x4084ff94;
s_encap_amsdu_func = 0x4084ff90;
mesh_rxcb = 0x4084ff8c;
sta_rxcb = 0x4084ff88;
s_netstack_ref = 0x4084ff84;
g_sta_connected_flag = 0x4084ff81;
g_itwt_fid = 0x4084ff80;
g_misc_nvs = 0x4084ff7c;
esp_test_dump_htc = 0x4084ff78;
