// Copyright 2020-2021 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * All the APIs declared here are internal only APIs, it can only be used by
 * espressif internal modules, such as SSC, LWIP, TCPIP adapter etc, espressif
 * customers are not recommended to use them.
 *
 * If someone really want to use specified APIs declared in here, please contact
 * espressif AE/developer to make sure you know the limitations or risk of
 * the API, otherwise you may get unexpected behavior!!!
 *
 */


#ifndef __ESP_WIFI_DEBUG_H__
#define __ESP_WIFI_DEBUG_H__

typedef struct {
    uint32_t diag0;
    uint32_t diag1;
    uint32_t diag2;
    uint32_t diag3;
    uint32_t diag4;
    uint32_t diag5;
    uint32_t diag6;
    uint32_t diag7;
    uint32_t diag8;
    uint32_t diag9;
    uint32_t diag10;
    uint32_t diag12;
    uint32_t diag_sel;
} wifi_stats_reg_diag_t;

typedef struct {
    uint32_t rx_data_suc;
    uint32_t rx_end;
    uint32_t rx_full;
    uint32_t rx_full1;
    uint32_t rx_bufbk;
    uint32_t rx_cck_err;
    uint32_t rx_ofdm_err;
    uint32_t rx_agc_err;
    uint32_t rx_sf;
    uint32_t rx_abort;
    uint32_t rx_fcs_err;
    uint32_t rx_fifo_overflow;
    uint32_t rx_apentrybuf_ful;
    uint32_t rx_other_uc;
    uint32_t rx_tkip_err;
    uint32_t rx_samebm_err;
    uint32_t rx_ack_int;
    uint32_t rx_rts_int;
    uint32_t rx_cts_int;
    uint32_t rx_rifs_int;
    uint32_t rx_block_err;
    uint32_t rx_freqhop_err;
    uint32_t txrts_int;
    uint32_t txcts_int;
    uint32_t txrxack_int;
    uint32_t txrxcts_int;
    uint32_t rxtrigger;
    uint32_t rxtxhung;
    uint32_t rxtxpanic;
} wifi_stats_reg_cnt_t;

typedef struct {
} wifi_stats_reg_filter_t;

typedef struct {
} wifi_stats_reg_crypto_t;

typedef struct {
} wifi_stats_reg_ba_t;

typedef struct {
    uint32_t bbst;
    uint32_t bbwd;
    uint32_t frd;
    uint32_t frd0;
    uint32_t frd1;
    uint32_t frd2;
    uint32_t frd3;
    uint32_t frd4;
    uint32_t int_st;
} wifi_stats_reg_misc_t;

typedef struct {
    uint32_t max;
    uint32_t alloc;
    uint32_t avail;
    uint32_t total_free;
    uint32_t total_alloc;
    uint32_t total_fail;
} wifi_stats_one_buf_t;

#define WIFI_STATS_BUF_MAX 9
typedef struct {
    wifi_stats_one_buf_t buf[WIFI_STATS_BUF_MAX];
} wifi_stats_buf_t;

typedef struct {
    wifi_stats_reg_diag_t diag;
    wifi_stats_reg_misc_t misc;
    wifi_stats_reg_cnt_t cnt;
} wifi_stats_reg_t;

typedef struct {
    uint32_t rx_suc;
    uint32_t rx_hung;
    uint32_t tx_complete;
    uint32_t tx_enable;
    uint32_t tx_collision;
    uint32_t panic_reset;
    uint32_t bb_wdt;
    uint32_t tbtt;

    uint32_t pp_post_fail;

    uint32_t rx_sniffer_pingpong_full;
    uint32_t rx_sniffer_null;
    uint32_t rx_sniffer_reset_retry;
    uint32_t rx_sniffer_reset_succ;
    uint32_t rx_sniffer_reset;
    uint32_t rx_sniffer_oversize;
    uint32_t rx_sniffer_mimo;
    uint32_t rx_sniffer_non_mimo;

    uint32_t bb_wdt_bt;
    uint32_t bb_wdt_wifi_rst;
    uint32_t bb_wdt_wifi_not_rst;
} wifi_stats_int_t;

typedef struct {
    wifi_stats_reg_t reg;
    wifi_stats_buf_t buf;
    wifi_stats_int_t interrupt;
} wifi_stats_mandatory_t;

#define WIFI_LMAC_AC_NUM     6
#define WIFI_LMAC_QUEUE_NUM  16

typedef struct {
    uint32_t tx_data_all;
    uint32_t tx_arp;
    uint32_t tx_raw;
    uint32_t tx_dhcp;
    uint32_t tx_ps_q;
    uint32_t tx_drop_data_arg;
    uint32_t tx_drop_data_if;
    uint32_t tx_drop_data_all;
    uint32_t tx_drop_data_conn_null;
    uint32_t tx_drop_data_not_run;
    uint32_t tx_drop_data_not_assoc;
    uint32_t tx_drop_data_not_auth;
    uint32_t tx_drop_data_no_mem;
    uint32_t tx_drop_data_qfull;
    uint32_t tx_drop_data_post_fail;
    uint32_t tx_drop_data_buf;
    uint32_t tx_drop_data_ps;
    uint32_t tx_task_yield;
    uint32_t tx_buffer_not_home_chan;
    uint32_t tx_buffer_not_home_chan_free;
    uint32_t tx_buffer_not_home_chan_scan;
} wifi_stats_hmac_tx_t;

typedef struct {
    uint32_t r1_all;
    uint32_t r1_ampdu;
    uint32_t r1_mutiple_blocks;
    uint32_t r1_data_mpdu;
    uint32_t r1_mgmt;
    uint32_t r1_mgmt_probe_req;
    uint32_t r1_mgmt_beacon;
    uint32_t r1_ctrl_bar;
    uint32_t r1_ctrl_ba;
    uint32_t r1_ctrl_pspoll;
    uint32_t r1_ctrl_others;
    uint32_t r1_sniffer_mimo;
    uint32_t r1_drop;
    uint32_t r1_drop_if_mismatch;
    uint32_t r1_drop_sniffer_state;
    uint32_t r1_drop_type_err;
    uint32_t r1_drop_mpdu_multiple_blocks;

    uint32_t r2_drop_multiple_blocks;
    uint32_t r2_drop_mpdu_no_mem;

    uint32_t r3_drop_ampdu_too_long;
    uint32_t r3_drop_ampdu_too_short;
    uint32_t r3_drop_ampdu_state;
    uint32_t r3_drop_ampdu_no_mem;
    uint32_t r3_drop_ampdu_len;
    uint32_t r3_drop_ampdu_ds;

    uint32_t r4_drop_ampdu_proc;
    uint32_t r4_drop_mpdu_proc;
    uint32_t r4_drop_frag;
    uint32_t r4_drop_state;
    uint32_t r4_drop_tkipmic;
    uint32_t r4_drop_sta_if;
    uint32_t r4_drop_ap_if;
    uint32_t r4_drop_if_mismatch;

    uint32_t r5_sniffer_ampdu;
    uint32_t r5_drop_sniffer_filter;
    uint32_t r5_drop_sniffer_no_mem;
    uint32_t r5_drop_sniffer_q_fail;

    uint32_t r6_sniffer_mimo;
    uint32_t r6_sniffer_single_block;
    uint32_t r6_sniffer_multiple_blocks;
    uint32_t r6_drop_sniffer_filter;
    uint32_t r6_drop_sniffer_no_mem;
    uint32_t r6_drop_sniffer_q_fail;

    uint32_t r7_sniffer_mimo;
    uint32_t r7_drop_sniffer_no_mem;
    uint32_t r7_drop_sniffer_post;
    uint32_t r7_drop_sniffer_others;

    uint32_t r8_reset_rx_base;
    uint32_t r8_reload;

    uint32_t rh1_sta_drop_conn_null;
    uint32_t rh1_sta_drop_too_short;
    uint32_t rh1_sta_drop_wrong_version;
    uint32_t rh1_sta_drop_not_my_bss;
    uint32_t rh1_sta_drop_mgmt_too_short1;
    uint32_t rh1_sta_drop_mgmt_probe_inq;
    uint32_t rh1_sta_drop_dup;
    uint32_t rh1_sta_drop_data_not_assoc;
    uint32_t rh1_sta_drop_data_too_short;
    uint32_t rh1_sta_drop_data_not_fromds;
    uint32_t rh1_sta_drop_data_invalid_key;
    uint32_t rh1_sta_drop_data_frag;
    uint32_t rh1_sta_drop_data_mic;
    uint32_t rh1_sta_drop_data_encap;
    uint32_t rh1_sta_drop_data_null_key;
    uint32_t rh1_sta_drop_data_unencrypt;
    uint32_t rh1_sta_data_to_lwip;
    uint32_t rh1_sta_mgmt;
    uint32_t rh1_sta_drop_mgmt_not_nods;
    uint32_t rh1_sta_drop_mgmt_too_short2;
    uint32_t rh1_sta_drop_mgmt_bip_check;
    uint32_t rh1_sta_ctrl;

    uint32_t rh2_sta_eapol_wps_succ;
    uint32_t rh2_sta_eapol_wps_fail;
    uint32_t rh2_sta_eapol_wps_null_handle;
    uint32_t rh2_sta_drop_not_8021x_auth;
    uint32_t rh2_sta_drop_not_allow_eapol;
    uint32_t rh2_sta_eapol_wpa2e_succ;
    uint32_t rh2_sta_drop_eapol_wpa2e_null_handle;
    uint32_t rh2_sta_drop_eapol_wpa2e_state;
    uint32_t rh2_sta_eapol_wpa_succ;
    uint32_t rh2_sta_drop_eapol_wpa_fail;
    uint32_t rh2_sta_drop_eapol_wpa_null_handle;

    uint32_t rh3_data_amsdu;
    uint32_t rh3_drop_data_amsdu_too_short;
    uint32_t rh3_drop_data_amsdu_no_mem;
    uint32_t rh3_drop_data_amsdu_remain_too_short;
    uint32_t rh3_sta_data_amsdu_mpdu;
    uint32_t rh3_ap_data_amsdu_mpdu;

    uint32_t rh4_sta_mgmt_beacon;
    uint32_t rh4_sta_mgmt_probe_resp;
    uint32_t rh4_sta_mgmt_auth;
    uint32_t rh4_sta_mgmt_deauth;
    uint32_t rh4_sta_mgmt_assoc;
    uint32_t rh4_sta_mgmt_action;
    uint32_t rh4_sta_drop_mgmt_disassoc_unencrypt;
    uint32_t rh4_sta_drop_mgmt_disassoc_not_my;
    uint32_t rh4_sta_drop_mgmt_unencrypt;
    uint32_t rh4_sta_mgmt_disassoc;
    uint32_t rh4_sta_drop_mgmt_unexpected;

    uint32_t rh5_sta_beacon_csa;
    uint32_t rh5_sta_drop_beacon_out_of_reg;

    uint32_t rh1_ap_data_fwd;
    uint32_t rh1_ap_drop_data_null_cb;
    uint32_t rh1_ap_data_to_lwip;
    uint32_t rh1_ap_drop_data_rxcb_null;
    uint32_t rh1_ap_drop_wrong_version;
    uint32_t rh1_ap_drop_ds2ds;
    uint32_t rh1_ap_drop_data_null_bss;
    uint32_t rh1_ap_drop_nondata_null_bss;
    uint32_t rh1_ap_drop_null_bss;
    uint32_t rh1_ap_drop_bcast_null_bss;
    uint32_t rh1_ap_drop_not_my_bss1;
    uint32_t rh1_ap_drop_not_my_bss2;
    uint32_t rh1_ap_drop_too_short1;
    uint32_t rh1_ap_drop_too_short2;
    uint32_t rh1_ap_drop_mgmt_too_short1;
    uint32_t rh1_ap_drop_mgmt_too_short2;
    uint32_t rh1_ap_drop_data_too_short;
    uint32_t rh1_ap_drop_data_not_tods;
    uint32_t rh1_ap_drop_data_not_assoc;
    uint32_t rh1_ap_drop_data_wep;
    uint32_t rh1_ap_drop_data_key;
    uint32_t rh1_ap_drop_data_decap;
    uint32_t rh1_ap_drop_data_8021x_auth;
    uint32_t rh1_ap_eapol_succ1;
    uint32_t rh1_ap_eapol_succ2;
    uint32_t rh1_ap_drop_eapol_null_handle1;
    uint32_t rh1_ap_drop_eapol_null_handle2;
    uint32_t rh1_ap_drop_data_unencrypt;
    uint32_t rh1_ap_mgmt;
    uint32_t rh1_ap_drop_mgmt_nods;
    uint32_t rh1_ap_drop_mgmt_mcast;
    uint32_t rh1_ap_drop_mgmt_encrypted;
    uint32_t rh1_ap_drop_mgmt_privacy;
    uint32_t rh1_ap_drop_mgmt_key;
    uint32_t rh1_ap_ctrl;
    uint32_t rh1_ap_drop_unknow_type;
    uint32_t rh1_ap_drop_dup;

    uint32_t rh2_ap_mgmt_beacon;
    uint32_t rh2_ap_mgmt_probe_resp;
    uint32_t rh2_ap_mgmt_probe_req;
    uint32_t rh2_ap_mgmt_auth;
    uint32_t rh2_ap_mgmt_assoc;
    uint32_t rh2_ap_mgmt_deauth_deassoc;
    uint32_t rh2_ap_mgmt_action;
    uint32_t rh2_ap_mgmt_other;

    uint32_t rh5_drop_ampdu_oos1;
    uint32_t rh5_drop_ampdu_oos2;
    uint32_t rh5_drop_ampdu_tid;
    uint32_t rh5_drop_ampdu_frag;
    uint32_t rh5_drop_ampdu_dup;

    uint32_t pm_bcn_interval;
    uint32_t pm_dtim_period;
    uint32_t pm_listen_interval;
    uint32_t pm_tbtt;
    uint32_t pm_rx_bcn;
    uint32_t pm_active_timeout;
    uint32_t pm_wake_tx;
    uint32_t pm_wake_tbtt;
    uint32_t pm_wake_tim;
    uint32_t pm_wake_dtim;
    uint32_t pm_sleep_null;
    uint32_t pm_wake_null;
    uint32_t pm_sleep;
    uint32_t pm_wake;
    uint32_t pm_bcn_delay_time;
    uint32_t pm_max_bcn_delay_time;
    uint32_t pm_avg_bcn_delay_time;
    uint64_t pm_sleep_time;
    uint64_t pm_wake_time;
    uint64_t pm_total_time;

    uint32_t t1_en_waitq;
    uint32_t t1_drop_sec;
    uint32_t t1_drop_enq_fail;
    uint32_t t1_drop_stop;

    uint32_t t2_ampdu_to_mpdu;
    uint32_t t3_en_waitq;

    uint16_t t4_doneq_len;
    uint16_t t4_waitq0_len;
    uint16_t t4_waitq1_len;
    uint16_t t4_rxq_len;
    uint16_t t4_qlen[WIFI_LMAC_QUEUE_NUM];
    uint32_t t5_drop_aged[WIFI_LMAC_AC_NUM];
    uint16_t t6_succ[WIFI_LMAC_AC_NUM];
    uint32_t t7_drop_src[WIFI_LMAC_AC_NUM];
    uint32_t t7_drop_lrc[WIFI_LMAC_AC_NUM];
    uint32_t t7_drop_others[WIFI_LMAC_AC_NUM];
    uint32_t t8_drop_src[WIFI_LMAC_AC_NUM];
    uint32_t t8_drop_lrc[WIFI_LMAC_AC_NUM];
    uint32_t t8_drop_others[WIFI_LMAC_AC_NUM];
    uint32_t t9_drop_src[WIFI_LMAC_AC_NUM];
    uint32_t t9_drop_lrc[WIFI_LMAC_AC_NUM];
    uint32_t t9_drop_others[WIFI_LMAC_AC_NUM];
    uint32_t t10_complete_invalid_state;
    uint32_t t10_complete_succ[WIFI_LMAC_AC_NUM];
    uint32_t t10_complete_cts_err[WIFI_LMAC_AC_NUM];
    uint32_t t10_complete_ack_err[WIFI_LMAC_AC_NUM];
    uint32_t t10_complete_data_err[WIFI_LMAC_AC_NUM];
    uint32_t t10_complete_other_err[WIFI_LMAC_AC_NUM];
    uint32_t t11_timeout[WIFI_LMAC_AC_NUM];
    uint32_t t12_collision[WIFI_LMAC_AC_NUM];
    uint32_t t13_complete_rts_collision_err[WIFI_LMAC_AC_NUM];
    uint32_t t13_complete_rts_seckid_err[WIFI_LMAC_AC_NUM];
    uint32_t t14_complete_data_collision_err[WIFI_LMAC_AC_NUM];
    uint32_t t14_complete_data_seckid_err[WIFI_LMAC_AC_NUM];
    uint32_t t14_complete_data_zero_err[WIFI_LMAC_AC_NUM];
    uint32_t t14_complete_data_other_err[WIFI_LMAC_AC_NUM];
    uint32_t t15_send_bar[WIFI_LMAC_AC_NUM];
    uint32_t t16_txq_free_len;
    uint32_t t16_txq_forward_len;
    uint32_t t16_txq_wait_len;

    wifi_stats_hmac_tx_t tx[2];

    uint32_t pp_post_overflow;
    uint32_t pp_post_fail;

    uint32_t ap_alloc;
    uint32_t ap_free;
    uint32_t sta_alloc;
    uint32_t sta_free;
    uint32_t bss_alloc;
    uint32_t bss_free;
    uint32_t tap_alloc;
    uint32_t tap_free;
    uint32_t rap_alloc;
    uint32_t rap_free;
    uint32_t rap_eb_alloc;
    uint32_t rap_eb_free;
    uint32_t scan_bss_alloc;
    uint32_t scan_bss_free;
} wifi_stats_optional_t;

typedef struct {
    wifi_stats_mandatory_t mandatory;
    wifi_stats_optional_t optional;
} wifi_stats_t;

//WiFi Internal Statistics
#define WIFI_MODULE_HW_DIAG              (1)
#define WIFI_MODULE_HW_COUNTERS          (1<<1)
#define WIFI_MODULE_HW_MISC              (1<<2)
#define WIFI_MODULE_INT_COUNTERS         (1<<3)
#define WIFI_MODULE_BUFFER_COUNTERS      (1<<4)
#define WIFI_MODULE_RX_COUNTERS          (1<<5)
#define WIFI_MODULE_TX_COUNTERS          (1<<6)
#define WIFI_MODULE_SLEEP                (1<<7)
#define WIFI_MODULE_EVENT_COUNTERS       (1<<8)
#define WIFI_MODULE_NVS                  (1<<9)
#define WIFI_MODULE_STA                  (1<<10)
#define WIFI_MODULE_AP                   (1<<11)
#define WIFI_MODULE_TRC                  (1<<12)

esp_err_t  esp_wifi_stats_get(wifi_stats_t *stats);
void esp_wifi_dump_stats(const wifi_stats_t *stats, uint64_t modules);
void esp_wifi_dump_info(uint64_t modules);
esp_err_t  esp_wifi_stats_init(void);
esp_err_t  esp_wifi_stats_deinit(void);
void esp_wifi_dump(uint64_t modules);

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ESP_WIFI_DEBUG_H__ */
