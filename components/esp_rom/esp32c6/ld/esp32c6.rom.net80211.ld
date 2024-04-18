/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6.rom.net80211.ld for esp32c6
 *
 *
 * Generated from ./target/esp32c6/interface-esp32c6.yml md5sum 06c13e133e0743d09b87aba30d3e213b
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x40000b4c;
ampdu_dispatch = 0x40000b50;
ampdu_dispatch_all = 0x40000b54;
ampdu_dispatch_as_many_as_possible = 0x40000b58;
ampdu_dispatch_movement = 0x40000b5c;
ampdu_dispatch_upto = 0x40000b60;
chm_is_at_home_channel = 0x40000b64;
cnx_node_is_existing = 0x40000b68;
cnx_node_search = 0x40000b6c;
ic_ebuf_recycle_rx = 0x40000b70;
ic_ebuf_recycle_tx = 0x40000b74;
ic_reset_rx_ba = 0x40000b78;
ieee80211_align_eb = 0x40000b7c;
/*ieee80211_ampdu_reorder = 0x40000b80;*/
ieee80211_ampdu_start_age_timer = 0x40000b84;
/*ieee80211_encap_esfbuf = 0x40000b88;*/
ieee80211_is_tx_allowed = 0x40000b8c;
ieee80211_output_pending_eb = 0x40000b90;
/*ieee80211_output_process = 0x40000b94;*/
ieee80211_set_tx_desc = 0x40000b98;
//sta_input = 0x40000b9c;
wifi_get_macaddr = 0x40000ba0;
wifi_rf_phy_disable = 0x40000ba4;
wifi_rf_phy_enable = 0x40000ba8;
ic_ebuf_alloc = 0x40000bac;
/*ieee80211_classify = 0x40000bb0;*/
ieee80211_copy_eb_header = 0x40000bb4;
ieee80211_recycle_cache_eb = 0x40000bb8;
ieee80211_search_node = 0x40000bbc;
ieee80211_crypto_encap = 0x40000bc0;
/* ieee80211_crypto_decap = 0x40000bc4; */
ieee80211_decap = 0x40000bc8;
wifi_is_started = 0x40000bcc;
ieee80211_gettid = 0x40000bd0;
ieee80211_encap_esfbuf_htc = 0x40000bd4;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x4087ffac;
g_scan = 0x4087ffa8;
g_chm = 0x4087ffa4;
g_ic_ptr = 0x4087ffa0;
g_hmac_cnt_ptr = 0x4087ff9c;
g_tx_cacheq_ptr = 0x4087ff98;
s_netstack_free = 0x4087ff94;
mesh_rxcb = 0x4087ff90;
sta_rxcb = 0x4087ff8c;
g_itwt_fid = 0x4087ff88;
esp_test_tx_addba_request = 0x4087ff84;
