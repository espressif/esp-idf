/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c5.rom.net80211.ld for esp32c5
 *
 *
 * Generated from ./target/esp32c5/interface-esp32c5.yml md5sum f5c146321f24f88ad1f27234da5aed11
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x40000b18;
esp_wifi_internal_tx = 0x40000b1c;
ampdu_dispatch = 0x40000b20;
ampdu_dispatch_all = 0x40000b24;
ampdu_dispatch_as_many_as_possible = 0x40000b28;
ampdu_dispatch_movement = 0x40000b2c;
ampdu_dispatch_upto = 0x40000b30;
chm_is_at_home_channel = 0x40000b34;
cnx_node_is_existing = 0x40000b38;
cnx_node_search = 0x40000b3c;
ic_ebuf_recycle_rx = 0x40000b40;
ic_ebuf_recycle_tx = 0x40000b44;
ic_reset_rx_ba = 0x40000b48;
ic_ebuf_alloc = 0x40000b4c;
ic_reset_extra_softap_rx_ba = 0x40000b50;
ieee80211_align_eb = 0x40000b54;
ieee80211_ampdu_reorder = 0x40000b58;
ieee80211_ampdu_start_age_timer = 0x40000b5c;
ieee80211_encap_esfbuf = 0x40000b60;
ieee80211_is_tx_allowed = 0x40000b64;
ieee80211_output_pending_eb = 0x40000b68;
ieee80211_output_process = 0x40000b6c;
/*ieee80211_set_tx_desc = 0x40000b70;*/
ieee80211_classify = 0x40000b74;
ieee80211_copy_eb_header = 0x40000b78;
ieee80211_recycle_cache_eb = 0x40000b7c;
ieee80211_search_node = 0x40000b80;
ieee80211_crypto_encap = 0x40000b84;
ieee80211_crypto_decap = 0x40000b88;
ieee80211_decap = 0x40000b8c;
ieee80211_set_tx_pti = 0x40000b90;
ieee80211_gettid = 0x40000b94;
ieee80211_post_hmac_tx = 0x40000b98;
ieee80211_alloc_tx_buf = 0x40000b9c;
ieee80211_output_do = 0x40000ba0;
ieee80211_find_elem_match = 0x40000ba4;
ieee80211_find_ie_match = 0x40000ba8;
ieee80211_find_elem = 0x40000bac;
ieee80211_find_ext_elem = 0x40000bb0;
ieee80211_find_ie = 0x40000bb4;
ieee80211_find_ext_ie = 0x40000bb8;
ieee80211_merge_mbssid_profile = 0x40000bbc;
ieee80211_encap_esfbuf_htc = 0x40000bc0;
wifi_get_macaddr = 0x40000bc4;
wifi_rf_phy_disable = 0x40000bc8;
wifi_rf_phy_enable = 0x40000bcc;
wifi_is_started = 0x40000bd0;
/*sta_input = 0x40000bd4;*/
sta_rx_eapol = 0x40000bd8;
sta_reset_beacon_timeout = 0x40000bdc;
sta_get_beacon_timeout = 0x40000be0;
ampdu_process_multicast_address_qos_frame = 0x40000be4;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x4085ffac;
g_scan = 0x4085ffa8;
g_chm = 0x4085ffa4;
g_ic_ptr = 0x4085ffa0;
g_hmac_cnt_ptr = 0x4085ff9c;
g_tx_cacheq_ptr = 0x4085ff98;
s_netstack_free = 0x4085ff94;
s_encap_amsdu_func = 0x4085ff90;
mesh_rxcb = 0x4085ff8c;
sta_rxcb = 0x4085ff88;
s_netstack_ref = 0x4085ff84;
g_sta_connected_flag = 0x4085ff81;
g_itwt_fid = 0x4085ff80;
g_misc_nvs = 0x4085ff7c;
esp_test_dump_htc = 0x4085ff78;
