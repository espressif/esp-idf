// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
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
#ifndef _SOC_UHCI_STRUCT_H_
#define _SOC_UHCI_STRUCT_H_
#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct {
    union {
        struct {
            uint32_t in_rst:             1;
            uint32_t out_rst:            1;
            uint32_t ahbm_fifo_rst:      1;
            uint32_t ahbm_rst:           1;
            uint32_t in_loop_test:       1;
            uint32_t out_loop_test:      1;
            uint32_t out_auto_wrback:    1;
            uint32_t out_no_restart_clr: 1;
            uint32_t out_eof_mode:       1;
            uint32_t uart0_ce:           1;
            uint32_t uart1_ce:           1;
            uint32_t reserved11:         1;
            uint32_t outdscr_burst_en:   1;
            uint32_t indscr_burst_en:    1;
            uint32_t out_data_burst_en:  1;
            uint32_t mem_trans_en:       1;
            uint32_t seper_en:           1;
            uint32_t head_en:            1;
            uint32_t crc_rec_en:         1;
            uint32_t uart_idle_eof_en:   1;
            uint32_t len_eof_en:         1;
            uint32_t encode_crc_en:      1;
            uint32_t clk_en:             1;
            uint32_t uart_rx_brk_eof_en: 1;
            uint32_t reserved24:         8;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t rx_start:                   1;
            uint32_t tx_start:                   1;
            uint32_t rx_hung:                    1;
            uint32_t tx_hung:                    1;
            uint32_t in_done:                    1;
            uint32_t in_suc_eof:                 1;
            uint32_t in_err_eof:                 1;
            uint32_t out_done:                   1;
            uint32_t out_eof:                    1;
            uint32_t in_dscr_err:                1;
            uint32_t out_dscr_err:               1;
            uint32_t in_dscr_empty:              1;
            uint32_t outlink_eof_err:            1;
            uint32_t out_total_eof:              1;
            uint32_t send_s_q:                   1;
            uint32_t send_a_q:                   1;
            uint32_t dma_in_fifo_full_wm:        1;
            uint32_t reserved17:                15;
        };
        uint32_t val;
    } int_raw;
    union {
        struct {
            uint32_t rx_start:                  1;
            uint32_t tx_start:                  1;
            uint32_t rx_hung:                   1;
            uint32_t tx_hung:                   1;
            uint32_t in_done:                   1;
            uint32_t in_suc_eof:                1;
            uint32_t in_err_eof:                1;
            uint32_t out_done:                  1;
            uint32_t out_eof:                   1;
            uint32_t in_dscr_err:               1;
            uint32_t out_dscr_err:              1;
            uint32_t in_dscr_empty:             1;
            uint32_t outlink_eof_err:           1;
            uint32_t out_total_eof:             1;
            uint32_t send_s_q:                  1;
            uint32_t send_a_q:                  1;
            uint32_t dma_in_fifo_full_wm:       1;
            uint32_t reserved17:               15;
        };
        uint32_t val;
    } int_st;
    union {
        struct {
            uint32_t rx_start:                   1;
            uint32_t tx_start:                   1;
            uint32_t rx_hung:                    1;
            uint32_t tx_hung:                    1;
            uint32_t in_done:                    1;
            uint32_t in_suc_eof:                 1;
            uint32_t in_err_eof:                 1;
            uint32_t out_done:                   1;
            uint32_t out_eof:                    1;
            uint32_t in_dscr_err:                1;
            uint32_t out_dscr_err:               1;
            uint32_t in_dscr_empty:              1;
            uint32_t outlink_eof_err:            1;
            uint32_t out_total_eof:              1;
            uint32_t send_s_q:                   1;
            uint32_t send_a_q:                   1;
            uint32_t dma_in_fifo_full_wm:        1;
            uint32_t reserved17:                15;
        };
        uint32_t val;
    } int_ena;
    union {
        struct {
            uint32_t rx_start:                   1;
            uint32_t tx_start:                   1;
            uint32_t rx_hung:                    1;
            uint32_t tx_hung:                    1;
            uint32_t in_done:                    1;
            uint32_t in_suc_eof:                 1;
            uint32_t in_err_eof:                 1;
            uint32_t out_done:                   1;
            uint32_t out_eof:                    1;
            uint32_t in_dscr_err:                1;
            uint32_t out_dscr_err:               1;
            uint32_t in_dscr_empty:              1;
            uint32_t outlink_eof_err:            1;
            uint32_t out_total_eof:              1;
            uint32_t send_s_q:                   1;
            uint32_t send_a_q:                   1;
            uint32_t dma_in_fifo_full_wm:        1;
            uint32_t reserved17:                15;
        };
        uint32_t val;
    } int_clr;
    union {
        struct {
            uint32_t full:       1;
            uint32_t empty:      1;
            uint32_t reserved2: 30;
        };
        uint32_t val;
    } dma_out_status;
    union {
        struct {
            uint32_t fifo_wdata:    9;
            uint32_t reserved9:     7;
            uint32_t fifo_push:     1;
            uint32_t reserved17:   15;
        };
        uint32_t val;
    } dma_out_push;
    union {
        struct {
            uint32_t full:         1;
            uint32_t empty:        1;
            uint32_t reserved2:    2;
            uint32_t rx_err_cause: 3;
            uint32_t reserved7:   25;
        };
        uint32_t val;
    } dma_in_status;
    union {
        struct {
            uint32_t fifo_rdata:  12;
            uint32_t reserved12:   4;
            uint32_t fifo_pop:     1;
            uint32_t reserved17:  15;
        };
        uint32_t val;
    } dma_in_pop;
    union {
        struct {
            uint32_t addr:           20;
            uint32_t reserved20:      8;
            uint32_t stop:            1;
            uint32_t start:           1;
            uint32_t restart:         1;
            uint32_t park:            1;
        };
        uint32_t val;
    } dma_out_link;
    union {
        struct {
            uint32_t addr:           20;
            uint32_t auto_ret:        1;
            uint32_t reserved21:      7;
            uint32_t stop:            1;
            uint32_t start:           1;
            uint32_t restart:         1;
            uint32_t park:            1;
        };
        uint32_t val;
    } dma_in_link;
    union {
        struct {
            uint32_t check_sum_en:         1;
            uint32_t check_seq_en:         1;
            uint32_t crc_disable:          1;
            uint32_t save_head:            1;
            uint32_t tx_check_sum_re:      1;
            uint32_t tx_ack_num_re:        1;
            uint32_t check_owner:          1;
            uint32_t wait_sw_start:        1;
            uint32_t sw_start:             1;
            uint32_t dma_in_fifo_full_thrs:12;
            uint32_t reserved21:          11;
        };
        uint32_t val;
    } conf1;
    union {
        struct {
            uint32_t dscr_addr:       18;
            uint32_t in_dscr_state:    2;
            uint32_t in_state:         3;
            uint32_t fifo_cnt_debug:   5;
            uint32_t decode_state:     3;
            uint32_t reserved31:       1;
        };
        uint32_t val;
    } state0;
    union {
        struct {
            uint32_t outlink_dscr_addr:18;
            uint32_t out_dscr_state:    2;
            uint32_t out_state:         3;
            uint32_t fifo_cnt:          5;
            uint32_t encode_state:      3;
            uint32_t reserved31:        1;
        };
        uint32_t val;
    } state1;
    uint32_t dma_out_eof_des_addr;                         /**/
    uint32_t dma_in_suc_eof_des_addr;                      /**/
    uint32_t dma_in_err_eof_des_addr;                      /**/
    uint32_t dma_out_eof_bfr_des_addr;                     /**/
    union {
        struct {
            uint32_t test_mode:    3;
            uint32_t reserved3:    1;
            uint32_t test_addr:    2;
            uint32_t reserved6:   26;
        };
        uint32_t val;
    } ahb_test;
    uint32_t dma_in_dscr;                                  /**/
    uint32_t dma_in_dscr_bf0;                              /**/
    uint32_t dma_in_dscr_bf1;                              /**/
    uint32_t dma_out_dscr;                                 /**/
    uint32_t dma_out_dscr_bf0;                             /**/
    uint32_t dma_out_dscr_bf1;                             /**/
    union {
        struct {
            uint32_t tx_c0_esc_en: 1;
            uint32_t tx_db_esc_en: 1;
            uint32_t tx_11_esc_en: 1;
            uint32_t tx_13_esc_en: 1;
            uint32_t rx_c0_esc_en: 1;
            uint32_t rx_db_esc_en: 1;
            uint32_t rx_11_esc_en: 1;
            uint32_t rx_13_esc_en: 1;
            uint32_t reserved8:   24;
        };
        uint32_t val;
    } escape_conf;
    union {
        struct {
            uint32_t txfifo_timeout:       8;
            uint32_t txfifo_timeout_shift: 3;
            uint32_t txfifo_timeout_ena:   1;
            uint32_t rxfifo_timeout:       8;
            uint32_t rxfifo_timeout_shift: 3;
            uint32_t rxfifo_timeout_ena:   1;
            uint32_t reserved24:           8;
        };
        uint32_t val;
    } hung_conf;
    uint32_t ack_num;                                      /**/
    uint32_t rx_head;                                      /**/
    union {
        struct {
            uint32_t single_send_num: 3;
            uint32_t single_send_en:  1;
            uint32_t always_send_num: 3;
            uint32_t always_send_en:  1;
            uint32_t reserved8:      24;
        };
        uint32_t val;
    } quick_sent;
    struct {
        uint32_t w_data[2];                                     /**/
    } q_data[7];
    union {
        struct {
            uint32_t seper_char:      8;
            uint32_t seper_esc_char0: 8;
            uint32_t seper_esc_char1: 8;
            uint32_t reserved24:      8;
        };
        uint32_t val;
    } esc_conf0;
    union {
        struct {
            uint32_t seq0:           8;
            uint32_t seq0_char0:     8;
            uint32_t seq0_char1:     8;
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } esc_conf1;
    union {
        struct {
            uint32_t seq1:           8;
            uint32_t seq1_char0:     8;
            uint32_t seq1_char1:     8;
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } esc_conf2;
    union {
        struct {
            uint32_t seq2:           8;
            uint32_t seq2_char0:     8;
            uint32_t seq2_char1:     8;
            uint32_t reserved24:     8;
        };
        uint32_t val;
    } esc_conf3;
    union {
        struct {
            uint32_t thrs:      13;
            uint32_t reserved13:19;
        };
        uint32_t val;
    } pkt_thres;
    uint32_t reserved_c4;
    uint32_t reserved_c8;
    uint32_t reserved_cc;
    uint32_t reserved_d0;
    uint32_t reserved_d4;
    uint32_t reserved_d8;
    uint32_t reserved_dc;
    uint32_t reserved_e0;
    uint32_t reserved_e4;
    uint32_t reserved_e8;
    uint32_t reserved_ec;
    uint32_t reserved_f0;
    uint32_t reserved_f4;
    uint32_t reserved_f8;
    uint32_t date;                                         /**/
} uhci_dev_t;
extern uhci_dev_t UHCI0;
extern uhci_dev_t UHCI1;
#ifdef __cplusplus
}
#endif

#endif  /* _SOC_UHCI_STRUCT_H_ */
