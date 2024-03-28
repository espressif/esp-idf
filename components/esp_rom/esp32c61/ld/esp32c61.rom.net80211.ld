/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32c6lite.rom.net80211.ld for esp32c6lite
 *
 *
 * Generated from ./target/esp32c6lite/interface-esp32c6lite.yml md5sum d875746dfc29a51e6d327cc923b7a095
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group rom_net80211
 ***************************************/

/* Functions */
esp_net80211_rom_version_get = 0x40000a6c;
ampdu_dispatch = 0x40000a70;
ampdu_dispatch_all = 0x40000a74;
ampdu_dispatch_as_many_as_possible = 0x40000a78;
ampdu_dispatch_movement = 0x40000a7c;
ampdu_dispatch_upto = 0x40000a80;
chm_is_at_home_channel = 0x40000a84;
cnx_node_is_existing = 0x40000a88;
cnx_node_search = 0x40000a8c;
ic_ebuf_recycle_rx = 0x40000a90;
ic_ebuf_recycle_tx = 0x40000a94;
ic_reset_rx_ba = 0x40000a98;
ieee80211_align_eb = 0x40000a9c;
/* ieee80211_ampdu_reorder = 0x40000aa0; */
ieee80211_ampdu_start_age_timer = 0x40000aa4;
ieee80211_encap_esfbuf = 0x40000aa8;
ieee80211_is_tx_allowed = 0x40000aac;
ieee80211_output_pending_eb = 0x40000ab0;
ieee80211_output_process = 0x40000ab4;
ieee80211_set_tx_desc = 0x40000ab8;
sta_input = 0x40000abc;
wifi_get_macaddr = 0x40000ac0;
wifi_rf_phy_disable = 0x40000ac4;
wifi_rf_phy_enable = 0x40000ac8;
ic_ebuf_alloc = 0x40000acc;
ieee80211_classify = 0x40000ad0;
ieee80211_copy_eb_header = 0x40000ad4;
ieee80211_recycle_cache_eb = 0x40000ad8;
ieee80211_search_node = 0x40000adc;
ieee80211_crypto_encap = 0x40000ae0;
ieee80211_crypto_decap = 0x40000ae4;
ieee80211_decap = 0x40000ae8;
ieee80211_set_tx_pti = 0x40000aec;
wifi_is_started = 0x40000af0;
ieee80211_gettid = 0x40000af4;
/* Data (.data, .bss, .rodata) */
net80211_funcs = 0x4084ffac;
g_scan = 0x4084ffa8;
g_chm = 0x4084ffa4;
g_ic_ptr = 0x4084ffa0;
g_hmac_cnt_ptr = 0x4084ff9c;
g_tx_cacheq_ptr = 0x4084ff98;
s_netstack_free = 0x4084ff94;
mesh_rxcb = 0x4084ff90;
sta_rxcb = 0x4084ff8c;
