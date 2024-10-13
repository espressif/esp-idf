/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "sys/queue.h"
#include "esp_err.h"
#include "esp_event_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SIG_MODE_LEGACY = 0,
    SIG_MODE_HT = 1,
    SIG_MODE_HE = 2,
    SIG_MODE_VHT = 3,
} sig_mode_t;

typedef struct {
    uint32_t mcs : 7;
    uint32_t cbw : 1;
    uint32_t ht_length : 16;
    uint32_t smoothing : 1;
    uint32_t not_sounding : 1;
    uint32_t : 1;
    uint32_t aggregation : 1;
    uint32_t stbc : 2;
    uint32_t fec_coding : 1;
    uint32_t sgi : 1;
} esp_wifi_htsig_t;

typedef struct {
    uint32_t format : 1;
    uint32_t beam_change : 1;
    uint32_t ul_dl : 1;
    uint32_t he_mcs : 4;
    uint32_t dcm : 1;
    uint32_t bss_color : 6;
    uint32_t rsvd : 1;
    uint32_t spatial_reuse : 4;
    uint32_t bw : 2;
    uint32_t gi_and_he_ltf_size : 2;
    uint32_t nsts_and_midamble_periodicity : 3;
    uint32_t rsvd1 : 6;
} esp_wifi_su_siga1_t;

typedef struct {
    uint32_t format : 1;
    uint32_t bss_color : 6;
    uint32_t spatial_reuse1 : 4;
    uint32_t spatial_reuse2 : 4;
    uint32_t spatial_reuse3 : 4;
    uint32_t spatial_reuse4 : 4;
    uint32_t rsvd : 1;
    uint32_t bw : 2;
    uint32_t rsvd1 : 6;
} esp_wifi_tb_siga1_t;

typedef struct {
    uint32_t ul_dl : 1;
    uint32_t sigb_mcs : 3;
    uint32_t sigb_dcm : 1;
    uint32_t bss_color : 6;
    uint32_t spatial_reuse : 4;
    uint32_t bw : 3;
    uint32_t sigb_sym_num_or_mu_mimo_users : 4;
    uint32_t sigb_compression : 1;
    uint32_t gi_and_he_ltf_size : 2;
    uint32_t doppler : 1;
    uint32_t rsvd : 6;
} esp_wifi_mu_siga1_t;

typedef struct {
    uint16_t txop : 7;
    uint16_t coding : 1;
    uint16_t ldpc_extra_symbol_segment : 1;
    uint16_t stbc : 1;
    uint16_t beamformed : 1;
    uint16_t pre_fec_padding_factor : 2;
    uint16_t pe_disambiguity : 1;
    uint16_t rsvd : 1;
    uint16_t doppler : 1;
} __attribute__((packed)) esp_wifi_su_siga2_t;

typedef struct {
    uint16_t txop : 7;
    uint16_t siga2_rsvd : 9;
} __attribute__((packed)) esp_wifi_tb_siga2_t;

typedef struct {
    uint16_t txop : 7;
    uint16_t rsvd : 1;
    uint16_t nltf_and_midamble_periodicity : 3;
    uint16_t ldpc_extra_symbol_segment : 1;
    uint16_t stbc : 1;
    uint16_t pre_fec_padding_factor : 2;
    uint16_t pe_disambiguation : 1;
} __attribute__((packed)) esp_wifi_mu_siga2_t;

typedef struct {
    uint32_t cbw : 2;
    uint32_t pro_reserved : 1;
    uint32_t stbc : 1;
    uint32_t group_id : 6;
    uint32_t su_nsts : 3;
    uint32_t su_partial_aid : 9;
    uint32_t txop_ps_not_allowed : 1;
    uint32_t pro_reserved2 : 1;
    uint32_t sgi : 1;
    uint32_t sgi_nsym_disambigution : 1;
    uint32_t su_coding : 1;
    uint32_t ldpc_extra_ofdm_symbol : 1;
    uint32_t su_mcs : 4;
} __attribute__((packed)) esp_wifi_vht_siga1_t;

typedef struct {
    uint32_t ru_allocation : 8;
    uint32_t crc : 4;
    uint32_t tail : 6; //18 bits
} esp_wifi_mu_sigb_common_t;

typedef struct {
    uint32_t ru_allocation : 16;
    uint32_t center_26tone_ru : 1;
    uint32_t crc : 4;
    uint32_t tail : 6; //not included into the sigb_common_info (21bits)
} esp_wifi_mu_sigb_common_80mhz_ppdu_t;

typedef struct {
    uint32_t sta_id : 11;
    uint32_t nsts : 3;
    uint32_t beamformed : 1;
    uint32_t he_mcs : 4;
    uint32_t dcm : 1;
    uint32_t coding : 1;
} esp_wifi_mu_sigb_user_non_mimo_t;

typedef struct {
    uint32_t sta_id : 11;
    uint32_t spatial_config : 4;
    uint32_t he_mcs : 4;
    uint32_t rsvd : 1;
    uint32_t coding : 1;
} esp_wifi_mu_sigb_user_mimo_t;

#define ESP_TEST_RX_MU_USER_NUM        (9)
//support buffer mu-users for 4 duts
typedef struct {
    uint16_t aid;
    /* MIMO */
    uint32_t mimo_rx;
    uint32_t mimo_sigb_mcs[6];      //MCS[0, 5]
    /*
     * count sigb info, max: 8 users
     *
     * mimo_user_num_occu[0] = the number of occurrences of user_num=2 within a period of rx
     * mimo_user_num_occu[1] = the number of occurrences of user_num=3 within a period of rx
     * ......
     * mimo_user_num_occu[6] = the number of occurrences of user_num=8 within a period of rx
     */
    uint32_t mimo_user_num_occu[7]; //UserNum[2, 8]
    struct {
        uint16_t aid;
        uint32_t occu_mcs[12];
        /*
         * occu_ss[0] = the number of occurrences of SS0 within a period of rx
         * occu_ss[1] = the number of occurrences of SS1 within a period of rx
         * ......
         * occu_ss[7] = the number of occurrences of SS7 within a period of rx
         */
        uint32_t occu_ss[8];
    } mimo[ESP_TEST_RX_MU_USER_NUM];
    /* Non-MIMO */
    uint32_t nonmimo_rx;
    uint32_t nonmimo_sigb_mcs[6];      //MCS[0, 5]
    uint32_t nonmimo_ru_alloc[256][9]; // size: 9216 bytes
    uint32_t nonmimo_user_num_occu[9]; // UserNum[1, 9]
    struct {
        uint16_t aid;
        uint32_t occu_nsts[8];
        uint32_t occu_mcs[12];
        uint32_t txbf;
        uint32_t dcm;
    } nonmimo[ESP_TEST_RX_MU_USER_NUM];
#if CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61
    uint32_t mu_bru_id_0: 16;
    uint32_t mu_bru_id_bssidx: 16;
    uint32_t mu_bru_id_2047: 16;
    uint32_t mu_uru_id_2046: 16;
#else
    uint32_t ru_alloc_96_num_2046;
    uint32_t ru_alloc_112_num_2046;
#endif
} esp_test_rx_mu_statistics_t; //10932 bytes

#if CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61
typedef struct {
    uint32_t legacy;
    uint32_t legacy_noeb;
    uint32_t ht;
    uint32_t ht_noeb;
    uint32_t ht_stbc;
    uint32_t ht_retry;
    uint32_t ersu;
    uint32_t ersu_noeb;
    uint32_t ersu_txbf;
    uint32_t ersu_dcm;
    uint32_t ersu_dcm_txbf;
    uint32_t ersu_retry;
    uint32_t su;
    uint32_t su_noeb;
    uint32_t su_stbc;
    uint32_t su_txbf;
    uint32_t su_retry;
    uint32_t su_frag;
    uint32_t mu;
    uint32_t mu_noeb;
    uint32_t mu_stbc;
    uint32_t mu_mimo;
    uint32_t mu_ofdma;
    uint32_t mu_txbf;
    uint32_t mu_retry;
    uint32_t mu_frag;
    /*
     * mu_bw[0] count the 20MHz MU PPDUs
     * mu_bw[1] count the 40MHz MU PPDUs
     * mu_bw[2] count the 80MHz MU PPDUs
     */
    uint32_t mu_bw[3];
    uint32_t mu_sigb_dump;
    uint32_t vht;
    uint32_t vht_noeb;
    uint32_t vht_stbc;
    uint32_t vht_txbf;
    uint32_t vht_retry;
    uint32_t rx_isr;
    uint32_t rx_nblks;
    uint32_t rx_ndpa;
    uint32_t rx_reset_rxbase_cnt;
    uint32_t rx_base_null_cnt;
} esp_test_rx_statistics_t; //140 bytes

#else

typedef struct {
    uint32_t legacy;
    uint32_t legacy_noeb;
    uint32_t ht;
    uint32_t ht_noeb;
    uint32_t ht_retry;
    uint32_t ersu;
    uint32_t ersu_noeb;
    uint32_t ersu_dcm;
    uint32_t su;
    uint32_t su_noeb;
    uint32_t su_stbc;
    uint32_t su_txbf;
    uint32_t su_retry;
    uint32_t mu;
    uint32_t mu_noeb;
    uint32_t mu_stbc;
    uint32_t mu_mimo;
    uint32_t mu_ofdma;
    uint32_t mu_txbf;
    uint32_t mu_retry;
    uint32_t rx_isr;
    uint32_t rx_nblks;
} esp_test_rx_statistics_t; //88 bytes
#endif

typedef enum {
    TEST_TX_SUCCESS,
    TEST_TX_FAIL_RTS,
    TEST_TX_WAIT_CTS,    //RX
    TEST_TX_FAIL_CTS,
    TEST_TX_FAIL_DATA,
    TEST_TX_WAIT_ACK,    //RX
    TEST_TX_FAIL_MAX,
} esp_test_tx_fail_state_t;

/* only happen when TEST_TX_WAIT_CTS(2), TEST_TX_WAIT_ACK(5) */
typedef enum {
    TEST_TX_WAIT_MATCH,
    TEST_TX_WAIT_NOT2SELF,
    TEST_TX_MISMATCH,
    TEST_TX_WAIT_TIMEOUT,
    TEST_TX_WAIT_MAX,
} esp_test_tx_fail_match_t;

/* only happen when TEST_TX_WAIT_MATCH(0) */
typedef enum {
    TEST_TX_FAIL_ERROR_H00,
    TEST_TX_FAIL_ERROR_H53,
    TEST_TX_FAIL_ERROR_H63,
    TEST_TX_FAIL_ERROR_H75,
    TEST_TX_FAIL_ERROR_H41,
    TEST_TX_FAIL_ERROR_H42,
    TEST_TX_FAIL_ERROR_H47,
    TEST_TX_FAIL_ERROR_H80,
    TEST_TX_FAIL_ERROR_H5A,
    TEST_TX_FAIL_ERROR_HXX,
    TEST_TX_FAIL_ERROR_MAX, //10
} esp_test_tx_fail_error_t;

typedef struct {
    uint32_t match[TEST_TX_WAIT_MAX][TEST_TX_FAIL_ERROR_MAX];
    uint32_t count;
} esp_test_tx_fail_statistics_t; //164 bytes

typedef struct {
    uint32_t tb_last;         /* count times of the last TX through TB */
    uint32_t tb_times;        /* count total TX times through TB */
    uint32_t tb_rx_ba;        /* can't know the exact packets number of BA or ACK*/
    uint32_t retry_edca;
    uint32_t retry_tb;
    uint32_t rx_ack;          /* count ACKs response to TX through EDCA */
    uint32_t rx_ba;           /* count BAs response to TX through EDCA */
    uint32_t rx_dump_ba;
    uint8_t rx_max_bitmap;
    uint8_t rx_min_bitmap;
    uint32_t rx_tot_bitmap;
    uint32_t tx_enable;
    uint32_t tx_complete;
    uint32_t tx_succ;
    uint32_t tx_no_mem;
    uint32_t tx_max_rtt;
    uint32_t tx_min_rtt;
    uint32_t tx_tot_rtt;
    uint32_t tx_seq_max_rtt; /* rtt of a sequence number containing the time of retries */
    uint32_t tx_seq_min_rtt;
    int64_t tx_start_time;
    int64_t tx_seqno_time;
    int64_t tx_muedca_time;
    uint32_t tx_max_muedca_time;
    uint32_t tx_min_muedca_time;
    uint32_t tx_tot_muedca_time;
    uint32_t muedca_times;
    uint32_t tx_muedca_enable; /* count TX times within mu-timer working */
    uint32_t collision;
    uint32_t timeout;
} esp_test_tx_statistics_t; //136 bytes

typedef struct {
    uint32_t complete_suc_tb;
    uint32_t complete_ack_tb;
    uint32_t complete_err_tb;
    uint32_t complete_tb_suc_count;
    uint32_t complete_tb_ack_count;
    uint32_t complete_tb_err_count;
    uint32_t complete_tb_tot_count;
    uint32_t complete_tb_pack_sent;
} esp_test_tx_tb_statistics_t; //32 bytes

typedef struct {
    uint16_t rx_trig;
    uint16_t tb_times;
    uint16_t tb_qos_null;
    uint16_t tx_bfrpt;
    uint16_t tb_cca_cancel;
    uint16_t tb_sifs_abort;
    uint16_t tb_pwr_outof_range;
} esp_test_hw_tb_statistics_t; //14 bytes

typedef struct {
    uint16_t rx_fcs_err;
    uint16_t rx_abort;
    uint16_t rx_abort_fcs_pass;
    uint16_t nrx_err_pwrdrop;
    uint16_t nrx_hesigb_err;
    uint16_t rx_samebm_errcnt;
    uint16_t rx_mpdu;
    uint16_t rx_end_cnt;
    uint16_t rx_datasuc;
    int16_t rx_cfo_hz;
    uint16_t rx_sf;
    uint16_t rx_other_ucast;
    uint16_t rx_buf_fullcnt;
    uint16_t rx_fifo_ovfcnt;
    uint16_t rx_tkip_errcnt;
    uint16_t rx_btblock_err;
    uint16_t rx_freqhop_err;
    uint16_t rx_lastunmatch_err;
    uint16_t rx_ack_int_cnt;
    uint16_t rx_rts_int_cnt;
    uint16_t brx_err_agc;
    uint16_t brx_err;
    uint16_t nrx_err;
    uint16_t nrx_err_abort;
    uint16_t nrx_err_agcexit;
    uint16_t nrx_err_bboff;
    uint16_t nrx_err_fdm_wdg;
    uint16_t nrx_err_restart;
    uint16_t nrx_err_serv;
    uint16_t nrx_err_txover;
    uint16_t nrx_err_unsupport;
    uint16_t nrx_htsig_err;
    uint16_t nrx_heunsupport;
    uint16_t nrx_hesiga_crc;
    uint16_t rxhung_statis;
    uint16_t txhung_statis;
    uint32_t rxtxhung;
#if CONFIG_IDF_TARGET_ESP32C5 || CONFIG_IDF_TARGET_ESP32C61
    uint32_t rxtxpanic;
    uint8_t bf_ndp_timeout;
    uint8_t bf_report_err;
#endif
} esp_test_hw_rx_statistics_t; //76->80 bytes

typedef struct {
    uint32_t tot;
    uint32_t occurs[2];       // 0: 0xc6 same bitmap; 1: 0xf5 tkip error
} esp_test_rx_error_occurs_t; //12 bytes

typedef struct {
    int ndpa;
    int ndpa_su;
    int ndpa_su_bcast;
    int ndpa_su_ucast;
    int ndpa_mu;
    int ndpa_cqi;
    int basic;
    int bsrp;
    int mubar;
    int bfrp;
    int nfrp;
    int bar;
} esp_test_rx_ctrl_t;

typedef enum {
    SU_NG4_CODEBOOKSIZE_0,
    SU_NG4_CODEBOOKSIZE_1,
    SU_NG16_CODEBOOKSIZE_0,
    SU_NG16_CODEBOOKSIZE_1,
    MU_NG4_CODEBOOKSIZE_0,
    MU_NG4_CODEBOOKSIZE_1,
    CQI,
    MU_NG16_CODEBOOKSIZE_1,
    NON_TB_SU_NG4_CODEBOOKSIZE_0,
    NON_TB_SU_NG4_CODEBOOKSIZE_1,
    NON_TB_SU_NG16_CODEBOOKSIZE_0,
    NON_TB_SU_NG16_CODEBOOKSIZE_1,
    NON_TB_SU_NG16_CODEBOOKSIZE_1_NC_2,   /* fault injection */
    NON_TB_SU_NG16_CODEBOOKSIZE_1_NSTS_2, /* fault injection */
    NON_TB_SU_NG16_CODEBOOKSIZE_1_NSTS_6, /* fault injection */
    NON_TB_CQI,
    NON_TB_CQI_NC_2,                      /* fault injection */
} esp_test_he_sounding_type_t;

#ifdef __cplusplus
}
#endif
