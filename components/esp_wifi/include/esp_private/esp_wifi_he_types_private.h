/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
    unsigned mcs : 7;
    unsigned cwb : 1;
    unsigned ht_length : 16;
    unsigned smoothing : 1;
    unsigned not_sounding : 1;
    unsigned : 1;
    unsigned aggregation : 1;
    unsigned stbc : 2;
    unsigned fec_coding : 1;
    unsigned sgi : 1;
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

#define ESP_TEST_RX_MU_USER_NUM        (9)
//support buffer mu-users for 4 duts
typedef struct {
    uint16_t aid;
    /* MIMO */
    int mimo_rx;
    int mimo_sigb_mcs[6];      //MCS[0, 5]
    /*
     * count sigb info, max: 8 users
     *
     * mimo_user_num_occu[0] = the number of occurrences of user_num=2 within a period of rx
     * mimo_user_num_occu[1] = the number of occurrences of user_num=3 within a period of rx
     * ......
     * mimo_user_num_occu[6] = the number of occurrences of user_num=8 within a period of rx
     */
    int mimo_user_num_occu[7]; //UserNum[2, 8]
    struct {
        uint16_t aid;
        int occu_mcs[12];
        /*
         * occu_ss[0] = the number of occurrences of SS0 within a period of rx
         * occu_ss[1] = the number of occurrences of SS1 within a period of rx
         * ......
         * occu_ss[7] = the number of occurrences of SS7 within a period of rx
         */
        int occu_ss[8];
    } mimo[ESP_TEST_RX_MU_USER_NUM];
    /* Non-MIMO */
    int nonmimo_rx;
    int nonmimo_sigb_mcs[6];      //MCS[0, 5]
    int nonmimo_ru_alloc[256][9]; //size: 9216 bytes
    int nonmimo_user_num_occu[9]; //UserNum[1, 9]
    struct {
        uint16_t aid;
        int occu_nsts[8];
        int occu_mcs[12];
        int txbf;
        int dcm;
    } nonmimo[ESP_TEST_RX_MU_USER_NUM];
    int ru_alloc_96_num_2046;    //106+106
    int ru_alloc_112_num_2046;   //52+52+52+52
} esp_test_rx_mu_statistics_t; //10932 bytes

typedef struct {
    int legacy;
    int legacy_noeb;
    int ht;
    int ht_noeb;
    int ht_retry;
    int ersu;
    int ersu_noeb;
    int ersu_dcm;
    int su;
    int su_noeb;
    int su_stbc;
    int su_txbf;
    int su_retry;
    int mu;
    int mu_noeb;
    int mu_stbc;
    int mu_mimo;
    int mu_ofdma;
    int mu_txbf;
    int mu_retry;
    int rx_isr;
    int rx_nblks;
} esp_test_rx_statistics_t; //88 bytes

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
    int match[TEST_TX_WAIT_MAX][TEST_TX_FAIL_ERROR_MAX];
    int count;
} esp_test_tx_fail_statistics_t; //164 bytes

typedef struct {
    int tb_last;              /* count times of the last TX through TB */
    int tb_times;             /* count total TX times through TB */
    int tb_rx_ba;             /* can't know the exact packets number of BA or ACK*/
    int retry_edca;
    int retry_tb;
    int rx_ack;               /* count ACKs response to TX through EDCA */
    int rx_ba;                /* count BAs response to TX through EDCA */
    int rx_dump_ba;
    uint8_t rx_max_bitmap;
    uint8_t rx_min_bitmap;
    int rx_tot_bitmap;
    int tx_enable;
    int tx_complete;
    int tx_succ;
    int tx_no_mem;
    int tx_max_rtt;
    int tx_min_rtt;
    int tx_tot_rtt;
    int tx_seq_max_rtt;       /* rtt of a sequence number containing the time of retries */
    int tx_seq_min_rtt;
    int64_t tx_start_time;
    int64_t tx_seqno_time;
    int64_t tx_muedca_time;
    int tx_max_muedca_time;
    int tx_min_muedca_time;
    int tx_tot_muedca_time;
    int muedca_times;
    int tx_muedca_enable;     /* count TX times within mu-timer working */
    int collision;
    int timeout;
} esp_test_tx_statistics_t; //136 bytes

typedef struct {
    int complete_suc_tb;
    int complete_ack_tb;
    int complete_err_tb;
    int complete_tb_suc_count;
    int complete_tb_ack_count;
    int complete_tb_err_count;
    int complete_tb_tot_count;
    int complete_tb_pack_sent;
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
    int rxtxhung;
} esp_test_hw_rx_statistics_t; //76 bytes

typedef struct {
    int tot;
    int occurs[2]; //0: 0xc6 same bitmap; 1: 0xf5 tkip error
} esp_test_rx_error_occurs_t; //12 bytes


#ifdef __cplusplus
}
#endif
