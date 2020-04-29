// Copyright 2020-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <esp_event.h>
#include <esp_wifi.h>
#include "esp_log.h"
#include "esp_wifi_internal.h"
#include "wifi_debug.h"
#include "esp_wifi_os_adapter.h"

#define TAG "wifid"

static void wifi_debug_dump_diag(const wifi_stats_reg_diag_t *diag)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>hardware diag");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "diag0", "diag1", "diag2", "diag3", "diag4", "diag5", "diag6", "diag7", "diag8", "diag9", "diag10", "diag12", "sel");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        diag->diag0, diag->diag1, diag->diag2, diag->diag3, diag->diag4, diag->diag5, diag->diag6, diag->diag7, diag->diag8,
        diag->diag9, diag->diag10, diag->diag12, diag->diag_sel);
}

static void wifi_debug_dump_cnt(const wifi_stats_reg_cnt_t *cnt)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>hardware counter");

    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "rx_succ", "rx_end", "rx_full", "rx_full1", "rx_bufbk", "cck_err", "ofdm_err", "agc_err", 
        "rx_sf", "rx_abort", "fcs_err", "fifo_of", "apbufful", "other_uc", "tkip_err");

    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        cnt->rx_data_suc, cnt->rx_end, cnt->rx_full, cnt->rx_full1, cnt->rx_bufbk, cnt->rx_cck_err, cnt->rx_ofdm_err, cnt->rx_agc_err, cnt->rx_sf, cnt->rx_abort,
        cnt->rx_fcs_err, cnt->rx_fifo_overflow, cnt->rx_apentrybuf_ful, cnt->rx_other_uc, cnt->rx_tkip_err);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "same_err", "ack_int", "rts_int", "cts_int", "rifs_int", "blok_err", "fh_err", 
        "trts_int", "tcts_int", "track_in", "trcts_in", "trigger", "hung", "panic");

    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        cnt->rx_samebm_err, cnt->rx_ack_int, cnt->rx_rts_int, cnt->rx_cts_int, cnt->rx_rifs_int, cnt->rx_block_err, cnt->rx_freqhop_err,
        cnt->txrts_int, cnt->txcts_int, cnt->txrxack_int, cnt->txrxcts_int, cnt->rxtrigger, cnt->rxtxhung, cnt->rxtxpanic);
}

static void wifi_debug_dump_misc(const wifi_stats_reg_misc_t *misc)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>hardware misc");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "bbst", "bbwd", "frd", "frd0", "frd1", "frd2", "frd3", "frd4", "int_st");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x",
        misc->bbst, misc->bbwd, misc->frd, misc->frd0, misc->frd1, misc->frd2, misc->frd3, misc->frd4, misc->int_st);
}

static void wifi_debug_dump_int(const wifi_stats_int_t *interrupt)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>interrupt counter");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s", "rx_suc", "rx_hung", "tx_com", "tx_ena", "tx_col", "panic", "bb_wdt", "tbtt", "post");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x",
        interrupt->rx_suc, interrupt->rx_hung, interrupt->tx_complete, interrupt->tx_enable, 
        interrupt->tx_collision, interrupt->panic_reset, interrupt->bb_wdt, interrupt->tbtt, interrupt->pp_post_fail);
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "sni_full", "sni_null", "sni_rtry", "sni_succ", "sni_ovsz", "sni_mimo", "sni_nomi", "bb_wdt", "bb_brst", "bb_wrst", "bb_nwrst");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        interrupt->rx_sniffer_pingpong_full, interrupt->rx_sniffer_null, interrupt->rx_sniffer_reset_retry, interrupt->rx_sniffer_reset_succ, interrupt->rx_sniffer_oversize,
        interrupt->rx_sniffer_mimo, interrupt->rx_sniffer_non_mimo, interrupt->bb_wdt, interrupt->bb_wdt_bt, interrupt->bb_wdt_wifi_rst, interrupt->bb_wdt_wifi_not_rst);
}
 
static void wifi_debug_dump_buf(const wifi_stats_t *stats)
{
    const wifi_stats_buf_t *buf = &stats->mandatory.buf;
    const wifi_stats_optional_t *mem = &stats->optional;
    int i=0;

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>buf counter");
    ESP_LOGI(TAG, "%1s %3s %5s %5s %8s %8s %8s", "t", "max", "alloc", "avail", "t_free", "t_alloc", "t_fail");

    for (i=0; i<WIFI_STATS_BUF_MAX; i++) {
        ESP_LOGI(TAG, "%1x %3x %5x %5x %8x %8x %8x", i, buf->buf[i].max, buf->buf[i].alloc, buf->buf[i].avail,
            buf->buf[i].total_free, buf->buf[i].total_alloc, buf->buf[i].total_fail);
    }

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>hmac memory counter");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s",
        "op", "ap", "sta", "bss", "tap", "rap", "rap_eb", "scan_bss");
    ESP_LOGI(TAG, "%8s %8x %8x %8x %8x %8x %8x %8x",
        "alloc", mem->ap_alloc, mem->sta_alloc, mem->bss_alloc, mem->tap_alloc, mem->rap_alloc, mem->rap_eb_alloc, mem->scan_bss_alloc);
    ESP_LOGI(TAG, "%8s %8x %8x %8x %8x %8x %8x %8x",
        "free", mem->ap_free, mem->sta_free, mem->bss_free, mem->tap_free, mem->rap_free, mem->rap_eb_free, mem->scan_bss_free);
}

static void wifi_debug_dump_rx(const wifi_stats_optional_t *rx)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>lmac rx counter");

    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "r1_all", "r1_ampdu", "r1_mblk", "r1_dmpdu", "r1_mgmt", "r1_preq", "r1_bcn", "r1_bar", "r1_ba", "r1_psp");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->r1_all, rx->r1_ampdu, rx->r1_mutiple_blocks, rx->r1_data_mpdu, rx->r1_mgmt, rx->r1_mgmt_probe_req,
        rx->r1_mgmt_beacon, rx->r1_ctrl_bar, rx->r1_ctrl_ba, rx->r1_ctrl_pspoll);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "r1_other","r1_mimo", "r1_drop", "r1d_if", "r1d_st", "r1d_type", "r1d_mblk", "r2d_mblk", "r2d_nmem", "r3d_tlon");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->r1_ctrl_others, rx->r1_sniffer_mimo, rx->r1_drop, rx->r1_drop_if_mismatch, rx->r1_drop_sniffer_state, rx->r1_drop_type_err,
        rx->r1_drop_mpdu_multiple_blocks, rx->r2_drop_multiple_blocks, rx->r2_drop_mpdu_no_mem, rx->r3_drop_ampdu_too_long);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "r3d_tsht", "r3d_st", "r3d_nmem", "r3d_len", "r3d_ds", "r4d_apro", "r4d_mpro", "r4d_frag", "r4d_st", "r4d_tkip");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->r3_drop_ampdu_too_short, rx->r3_drop_ampdu_state, rx->r3_drop_ampdu_no_mem, rx->r3_drop_ampdu_len, rx->r3_drop_ampdu_ds,
        rx->r4_drop_ampdu_proc, rx->r4_drop_mpdu_proc, rx->r4_drop_frag, rx->r4_drop_state, rx->r4_drop_tkipmic);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "r4d_sif", "r4d_aif", "r4d_if", "r5_amdu", "r5d_filt", "r5d_nmem", "r5d_qfai", "r6_mimo", "r6_sblk", "r6_mblk");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->r4_drop_sta_if, rx->r4_drop_ap_if, rx->r4_drop_if_mismatch, rx->r5_sniffer_ampdu, rx->r5_drop_sniffer_filter,
        rx->r5_drop_sniffer_no_mem, rx->r5_drop_sniffer_q_fail, rx->r6_sniffer_mimo, rx->r6_sniffer_single_block, rx->r6_sniffer_multiple_blocks);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "r6d_flt", "r6d_nmem", "r6d_qfai", "r7_mimo", "r7d_nmem", "rd7_post", "r7d_oth", "r8_reset", "r8_relo", "h1d_cnul");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->r6_drop_sniffer_filter, rx->r6_drop_sniffer_no_mem, rx->r6_drop_sniffer_q_fail, rx->r7_sniffer_mimo, rx->r7_drop_sniffer_no_mem,
        rx->r7_drop_sniffer_post, rx->r7_drop_sniffer_others, rx->r8_reset_rx_base, rx->r8_reload, rx->rh1_sta_drop_conn_null);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>sta hmac rx counter");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "h1d_ts", "h1d_ver", "h1d_bss", "h1d_mts", "h1d_inq",
        "h1d_dup", "h1d_nass", "h1d_dtsh", "h1d_ds", "h1d_key");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->rh1_sta_drop_too_short, rx->rh1_sta_drop_wrong_version, rx->rh1_sta_drop_not_my_bss, rx->rh1_sta_drop_mgmt_too_short1, rx->rh1_sta_drop_mgmt_probe_inq,
        rx->rh1_sta_drop_dup, rx->rh1_sta_drop_data_not_assoc, rx->rh1_sta_drop_data_too_short, rx->rh1_sta_drop_data_not_fromds, rx->rh1_sta_drop_data_invalid_key);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "h1d_frag", "h1d_mic", "h1d_enc", "h1d_nkey", "h1d_unen", "h1d_lwip", "h1_mgmt", "h1d_nods", "h1d_mts2", "h1d_bip");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->rh1_sta_drop_data_frag, rx->rh1_sta_drop_data_mic, rx->rh1_sta_drop_data_encap, rx->rh1_sta_drop_data_null_key, rx->rh1_sta_drop_data_unencrypt, 
        rx->rh1_sta_data_to_lwip, rx->rh1_sta_mgmt, rx->rh1_sta_drop_mgmt_not_nods, rx->rh1_sta_drop_mgmt_too_short2, rx->rh1_sta_drop_mgmt_bip_check);
       
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "h2_wpss", "h2_wpsf", "h2d_wpsn", "h2d_1x", "h2d_neap",
        "h2d_2eok", "h2d_2enl", "h2d_2est", "h2_wpas", "h2d_wpaf");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->rh2_sta_eapol_wps_succ, rx->rh2_sta_eapol_wps_fail, rx->rh2_sta_eapol_wps_null_handle, rx->rh2_sta_drop_not_8021x_auth, rx->rh2_sta_drop_not_allow_eapol,
        rx->rh2_sta_eapol_wpa2e_succ, rx->rh2_sta_drop_eapol_wpa2e_null_handle, rx->rh2_sta_drop_eapol_wpa2e_state, rx->rh2_sta_eapol_wpa_succ, rx->rh2_sta_drop_eapol_wpa_fail);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "h2d_wpan", "h3_amsdu", "h3d_ts", "h3d_nmem", "h3d_rts",
        "h3_samsdu", "h3_aamsdu", "h4_bcn", "h4_prsp", "h4_auth");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->rh2_sta_drop_eapol_wpa_null_handle, rx->rh3_data_amsdu, rx->rh3_drop_data_amsdu_too_short, rx->rh3_drop_data_amsdu_no_mem, rx->rh3_drop_data_amsdu_remain_too_short,
        rx->rh3_sta_data_amsdu_mpdu, rx->rh3_ap_data_amsdu_mpdu, rx->rh4_sta_mgmt_beacon, rx->rh4_sta_mgmt_probe_resp, rx->rh4_sta_mgmt_auth);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "h4_death", "h4_assoc", "h4_ation", "h4d_unen", "h4d_nmy",
        "h4d_munen", "h4d_disa", "h4d_munex", "h5_csa", "h5d_reg");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        rx->rh4_sta_mgmt_deauth, rx->rh4_sta_mgmt_assoc, rx->rh4_sta_mgmt_action, rx->rh4_sta_drop_mgmt_disassoc_unencrypt, rx->rh4_sta_drop_mgmt_disassoc_not_my,
        rx->rh4_sta_drop_mgmt_unencrypt, rx->rh4_sta_mgmt_disassoc, rx->rh4_sta_drop_mgmt_unexpected, rx->rh5_sta_beacon_csa, rx->rh5_sta_drop_beacon_out_of_reg);

    ESP_LOGI(TAG, "========>ap hmac rx counter");
}

static void wifi_debug_dump_tx(const wifi_stats_optional_t *tx)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========>ap hmac rx counter");

    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "t1_enwq", "t1d_sec", "t1d_eqf", "t1d_stop", "t2_atm",
        "t3_ewq", "t4_dql", "t4_wql", "t4_wq1l", "t4_rxql", "post_of", "post_f");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        tx->t1_en_waitq, tx->t1_drop_sec, tx->t1_drop_enq_fail, tx->t1_drop_stop, tx->t2_ampdu_to_mpdu,
        tx->t3_en_waitq, tx->t4_doneq_len, tx->t4_waitq0_len, tx->t4_waitq1_len, tx->t4_rxq_len,
        tx->pp_post_overflow, tx->pp_post_fail, tx->t10_complete_invalid_state);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s %3s",
        "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
    ESP_LOGI(TAG, "%3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x %3x",
        tx->t4_qlen[0], tx->t4_qlen[1], tx->t4_qlen[2], tx->t4_qlen[3], tx->t4_qlen[4], tx->t4_qlen[5], tx->t4_qlen[6], tx->t4_qlen[7],
        tx->t4_qlen[8], tx->t4_qlen[9], tx->t4_qlen[10], tx->t4_qlen[11], tx->t4_qlen[12], tx->t4_qlen[13], tx->t4_qlen[14], tx->t4_qlen[15]);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%16s %8s %8s %8s %8s %8s %8s",
        "type", "ac0", "ac1", "ac2", "ac3", "ac4", "ac5");
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t5_drop_aged", tx->t5_drop_aged[0], tx->t5_drop_aged[1], tx->t5_drop_aged[2], tx->t5_drop_aged[3], tx->t5_drop_aged[4], tx->t5_drop_aged[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t6_succ", tx->t6_succ[0], tx->t6_succ[1], tx->t6_succ[2], tx->t6_succ[3], tx->t6_succ[4], tx->t6_succ[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t7_drop_src", tx->t7_drop_src[0], tx->t7_drop_src[1], tx->t7_drop_src[2], tx->t7_drop_src[3], tx->t7_drop_src[4], tx->t7_drop_src[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t7_drop_lrc", tx->t7_drop_lrc[0], tx->t7_drop_lrc[1], tx->t7_drop_lrc[2], tx->t7_drop_lrc[3], tx->t7_drop_lrc[4], tx->t7_drop_lrc[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t7_drop_others", tx->t7_drop_others[0], tx->t7_drop_others[1], tx->t7_drop_others[2], tx->t7_drop_others[3], tx->t7_drop_others[4], tx->t7_drop_others[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t8_drop_src", tx->t8_drop_src[0], tx->t8_drop_src[1], tx->t8_drop_src[2], tx->t8_drop_src[3], tx->t8_drop_src[4], tx->t8_drop_src[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t8_drop_lrc", tx->t8_drop_lrc[0], tx->t8_drop_lrc[1], tx->t8_drop_lrc[2], tx->t8_drop_lrc[3], tx->t8_drop_lrc[4], tx->t8_drop_lrc[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t8_drop_others", tx->t8_drop_others[0], tx->t8_drop_others[1], tx->t8_drop_others[2], tx->t8_drop_others[3], tx->t8_drop_others[4], tx->t8_drop_others[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t9_drop_src", tx->t9_drop_src[0], tx->t9_drop_src[1], tx->t9_drop_src[2], tx->t9_drop_src[3], tx->t9_drop_src[4], tx->t9_drop_src[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t9_drop_lrc", tx->t9_drop_lrc[0], tx->t9_drop_lrc[1], tx->t9_drop_lrc[2], tx->t9_drop_lrc[3], tx->t9_drop_lrc[4], tx->t9_drop_lrc[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t9_drop_others", tx->t9_drop_others[0], tx->t9_drop_others[1], tx->t9_drop_others[2], tx->t9_drop_others[3], tx->t9_drop_others[4], tx->t9_drop_others[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t10_com_succ", tx->t10_complete_succ[0], tx->t10_complete_succ[1], tx->t10_complete_succ[2], tx->t10_complete_succ[3], tx->t10_complete_succ[4], tx->t10_complete_succ[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t10_com_cts", tx->t10_complete_cts_err[0], tx->t10_complete_cts_err[1], tx->t10_complete_cts_err[2], tx->t10_complete_cts_err[3], tx->t10_complete_cts_err[4], tx->t10_complete_cts_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t10_com_ack", tx->t10_complete_ack_err[0], tx->t10_complete_ack_err[1], tx->t10_complete_ack_err[2], tx->t10_complete_ack_err[3], tx->t10_complete_ack_err[4], tx->t10_complete_ack_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t10_com_data", tx->t10_complete_data_err[0], tx->t10_complete_data_err[1], tx->t10_complete_data_err[2], tx->t10_complete_data_err[3], tx->t10_complete_data_err[4], tx->t10_complete_data_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t10_com_other", tx->t10_complete_other_err[0], tx->t10_complete_other_err[1], tx->t10_complete_other_err[2], tx->t10_complete_other_err[3], tx->t10_complete_other_err[4], tx->t10_complete_other_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t11_timeout", tx->t11_timeout[0], tx->t11_timeout[1], tx->t11_timeout[2], tx->t11_timeout[3], tx->t11_timeout[4], tx->t11_timeout[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t12_collision", tx->t12_collision[0], tx->t12_collision[1], tx->t12_collision[2], tx->t12_collision[3], tx->t12_collision[4], tx->t12_collision[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t13_com_rts", tx->t13_complete_rts_collision_err[0], tx->t13_complete_rts_collision_err[1], tx->t13_complete_rts_collision_err[2], tx->t13_complete_rts_collision_err[3], tx->t13_complete_rts_collision_err[4], tx->t13_complete_rts_collision_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t13_com_rtsseck", tx->t13_complete_rts_seckid_err[0], tx->t13_complete_rts_seckid_err[1], tx->t13_complete_rts_seckid_err[2], tx->t13_complete_rts_seckid_err[3], tx->t13_complete_rts_seckid_err[4], tx->t13_complete_rts_seckid_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t14_com_data", tx->t14_complete_data_collision_err[0], tx->t14_complete_data_collision_err[1], tx->t14_complete_data_collision_err[2], tx->t14_complete_data_collision_err[3], tx->t14_complete_data_collision_err[4], tx->t14_complete_data_collision_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t14_com_dseckid", tx->t14_complete_data_seckid_err[0], tx->t14_complete_data_seckid_err[1], tx->t14_complete_data_seckid_err[2], tx->t14_complete_data_seckid_err[3], tx->t14_complete_data_seckid_err[4], tx->t14_complete_data_seckid_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t14_com_dzero", tx->t14_complete_data_zero_err[0], tx->t14_complete_data_zero_err[1], tx->t14_complete_data_zero_err[2], tx->t14_complete_data_zero_err[3], tx->t14_complete_data_zero_err[4], tx->t14_complete_data_zero_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t14_com_dother", tx->t14_complete_data_other_err[0], tx->t14_complete_data_other_err[1], tx->t14_complete_data_other_err[2], tx->t14_complete_data_other_err[3], tx->t14_complete_data_other_err[4], tx->t14_complete_data_other_err[5]);
    ESP_LOGI(TAG, "%16s %8x %8x %8x %8x %8x %8x",
        "t15_send_bar", tx->t15_send_bar[0], tx->t15_send_bar[1], tx->t15_send_bar[2], tx->t15_send_bar[3], tx->t15_send_bar[4], tx->t15_send_bar[5]);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s", 
        "ifx", "data_all", "arp", "raw", "dhcp", "ps_q",
        "drop_arg", "drop_if", "drop_all", "drop_nul", "drop_run");
    ESP_LOGI(TAG, "%8s %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x", 
        "sta", tx->tx[0].tx_data_all, tx->tx[0].tx_arp, tx->tx[0].tx_raw, tx->tx[0].tx_dhcp, tx->tx[0].tx_ps_q,
        tx->tx[0].tx_drop_data_arg, tx->tx[0].tx_drop_data_if, tx->tx[0].tx_drop_data_all, tx->tx[0].tx_drop_data_conn_null, tx->tx[0].tx_drop_data_not_run);
    ESP_LOGI(TAG, "%8s %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x", 
        "ap", tx->tx[1].tx_data_all, tx->tx[1].tx_arp, tx->tx[1].tx_raw, tx->tx[1].tx_dhcp, tx->tx[1].tx_ps_q,
        tx->tx[1].tx_drop_data_arg, tx->tx[1].tx_drop_data_if, tx->tx[1].tx_drop_data_all, tx->tx[1].tx_drop_data_conn_null, tx->tx[1].tx_drop_data_not_run);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s %8s", 
        "ifx", "drop_asso", "drop_ath", "drop_nme", "drop_qfu", "drop_pfl", "drop_buf",
        "drop_ps", "yield", "not_home", "nh_free", "nh_scan");
    ESP_LOGI(TAG, "%8s %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x", 
        "sta", tx->tx[0].tx_drop_data_not_assoc, tx->tx[0].tx_drop_data_not_auth, tx->tx[0].tx_drop_data_no_mem,
        tx->tx[0].tx_drop_data_qfull, tx->tx[0].tx_drop_data_post_fail, tx->tx[0].tx_drop_data_buf, tx->tx[0].tx_drop_data_ps,
        tx->tx[0].tx_task_yield, tx->tx[0].tx_buffer_not_home_chan, tx->tx[0].tx_buffer_not_home_chan_free, tx->tx[0].tx_buffer_not_home_chan_scan);
    ESP_LOGI(TAG, "%8s %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x %8x", 
        "ap", tx->tx[1].tx_drop_data_not_assoc, tx->tx[1].tx_drop_data_not_auth, tx->tx[1].tx_drop_data_no_mem,
        tx->tx[1].tx_drop_data_qfull, tx->tx[1].tx_drop_data_post_fail, tx->tx[1].tx_drop_data_buf, tx->tx[1].tx_drop_data_ps,
        tx->tx[1].tx_task_yield, tx->tx[1].tx_buffer_not_home_chan, tx->tx[1].tx_buffer_not_home_chan_free, tx->tx[1].tx_buffer_not_home_chan_scan);
}

static void wifi_debug_dump_sleep(const wifi_stats_optional_t *pm)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "========> PM counter");

    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %8s %8s %8s",
        "bnc_invl", "dtim", "listen", "tbtt", "rx_bcn", "act_to", "wake_tx", "wake_tbt", "wake_tim", "wake_dtim");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %8x %8x %8x",
        pm->pm_bcn_interval, pm->pm_dtim_period, pm->pm_listen_interval, pm->pm_tbtt, pm->pm_rx_bcn,
        pm->pm_active_timeout, pm->pm_wake_tx, pm->pm_wake_tbtt, pm->pm_wake_tim, pm->pm_wake_dtim);

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "%8s %8s %8s %8s %8s %8s %8s %16s %16s %16s",
        "slp_null", "wake_nul", "sleep", "wake", "bcn_dlay",
        "maxb_dl", "avgb_dl", "sleep_t", "wake_t", "total_t");
    ESP_LOGI(TAG, "%8x %8x %8x %8x %8x %8x %8x %16llx %16llu %16llu",
        pm->pm_sleep_null, pm->pm_wake_null, pm->pm_sleep, pm->pm_wake, pm->pm_bcn_delay_time,
        pm->pm_max_bcn_delay_time, pm->pm_avg_bcn_delay_time, pm->pm_sleep_time, pm->pm_wake_time, pm->pm_total_time);
 
}

void esp_wifi_dump_stats(const wifi_stats_t *stats, uint64_t modules)
{
    if (modules & WIFI_MODULE_HW_DIAG) {
        wifi_debug_dump_diag(&stats->mandatory.reg.diag);
    }

    if (modules & WIFI_MODULE_HW_COUNTERS) {
        wifi_debug_dump_cnt(&stats->mandatory.reg.cnt);
    }

    if (modules & WIFI_MODULE_HW_MISC) {
        wifi_debug_dump_misc(&stats->mandatory.reg.misc);
    }

    if (modules & WIFI_MODULE_INT_COUNTERS) {
        wifi_debug_dump_int(&stats->mandatory.interrupt);
    }

    if (modules & WIFI_MODULE_BUFFER_COUNTERS) {
        wifi_debug_dump_buf(stats);
    }

    if (modules & WIFI_MODULE_RX_COUNTERS) {
        wifi_debug_dump_rx(&stats->optional);
    }

    if (modules & WIFI_MODULE_TX_COUNTERS) {
        wifi_debug_dump_tx(&stats->optional);
    }

    if (modules & WIFI_MODULE_SLEEP) {
        wifi_debug_dump_sleep(&stats->optional);
    }

    if (modules & WIFI_MODULE_SLEEP) {
        wifi_debug_dump_sleep(&stats->optional);
    }
}

void esp_wifi_dump(uint64_t modules)
{
    if (modules == 0) {
        return;
    }

    wifi_stats_t *stats = (wifi_stats_t*)wifi_malloc(sizeof(wifi_stats_t));

    if (!stats) {
        ESP_LOGE(TAG, "poll request: fail allocate wifi stats");
        return;
    }
    
    esp_wifi_stats_get(stats);
    esp_wifi_dump_stats(stats, modules);
    free(stats);
}
