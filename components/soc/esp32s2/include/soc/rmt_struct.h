/*
 * SPDX-FileCopyrightText: 2017-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rmt_dev_t {
    volatile uint32_t data_ch[4]; /* Data FIFO, Can only be accessed by PeriBus2 */
    volatile struct {
        union {
            struct {
                uint32_t div_cnt:            8;
                uint32_t idle_thres:        16;
                uint32_t mem_size:           3;
                uint32_t carrier_eff_en:     1;
                uint32_t carrier_en:         1;
                uint32_t carrier_out_lv:     1;
                uint32_t reserved30:         2;
            };
            uint32_t val;
        } conf0;
        union {
            struct {
                uint32_t tx_start:              1;
                uint32_t rx_en:                 1;
                uint32_t mem_wr_rst:            1;
                uint32_t mem_rd_rst:            1;
                uint32_t apb_mem_rst:           1;
                uint32_t mem_owner:             1;
                uint32_t tx_conti_mode:         1;
                uint32_t rx_filter_en:          1;
                uint32_t rx_filter_thres:       8;
                uint32_t chk_rx_carrier_en:     1;
                uint32_t ref_always_on:         1;
                uint32_t idle_out_lv:           1;
                uint32_t idle_out_en:           1;
                uint32_t tx_stop:               1;
                uint32_t reserved21:           11;
            };
            uint32_t val;
        } conf1;
    } conf_ch[4];
    volatile union {
        struct {
            uint32_t mem_waddr_ex:       9;
            uint32_t reserved9:          1;
            uint32_t mem_raddr_ex:       9;
            uint32_t reserved19:         1;
            uint32_t state:              3;
            uint32_t mem_owner_err:      1;
            uint32_t mem_full:           1;
            uint32_t mem_empty:          1;
            uint32_t apb_mem_wr_err:     1;
            uint32_t apb_mem_rd_err:     1;
            uint32_t reserved28:         4;
        };
        uint32_t val;
    } status_ch[4];
    volatile union {
        struct {
            uint32_t waddr:             9;
            uint32_t reserved9:         1;
            uint32_t raddr:             9;
            uint32_t reserved19:       13;
        };
        uint32_t val;
    } apb_mem_addr_ch[4];
    volatile union {
        struct {
            uint32_t ch0_tx_end:               1;
            uint32_t ch0_rx_end:               1;
            uint32_t ch0_err:                  1;
            uint32_t ch1_tx_end:               1;
            uint32_t ch1_rx_end:               1;
            uint32_t ch1_err:                  1;
            uint32_t ch2_tx_end:               1;
            uint32_t ch2_rx_end:               1;
            uint32_t ch2_err:                  1;
            uint32_t ch3_tx_end:               1;
            uint32_t ch3_rx_end:               1;
            uint32_t ch3_err:                  1;
            uint32_t ch0_tx_thr_event:         1;
            uint32_t ch1_tx_thr_event:         1;
            uint32_t ch2_tx_thr_event:         1;
            uint32_t ch3_tx_thr_event:         1;
            uint32_t ch0_tx_loop:              1;
            uint32_t ch1_tx_loop:              1;
            uint32_t ch2_tx_loop:              1;
            uint32_t ch3_tx_loop:              1;
            uint32_t ch0_rx_thr_event:         1;
            uint32_t ch1_rx_thr_event:         1;
            uint32_t ch2_rx_thr_event:         1;
            uint32_t ch3_rx_thr_event:         1;
            uint32_t reserved24:               8;
        };
        uint32_t val;
    } int_raw;
    volatile union {
        struct {
            uint32_t ch0_tx_end:              1;
            uint32_t ch0_rx_end:              1;
            uint32_t ch0_err:                 1;
            uint32_t ch1_tx_end:              1;
            uint32_t ch1_rx_end:              1;
            uint32_t ch1_err:                 1;
            uint32_t ch2_tx_end:              1;
            uint32_t ch2_rx_end:              1;
            uint32_t ch2_err:                 1;
            uint32_t ch3_tx_end:              1;
            uint32_t ch3_rx_end:              1;
            uint32_t ch3_err:                 1;
            uint32_t ch0_tx_thr_event:        1;
            uint32_t ch1_tx_thr_event:        1;
            uint32_t ch2_tx_thr_event:        1;
            uint32_t ch3_tx_thr_event:        1;
            uint32_t ch0_tx_loop:             1;
            uint32_t ch1_tx_loop:             1;
            uint32_t ch2_tx_loop:             1;
            uint32_t ch3_tx_loop:             1;
            uint32_t ch0_rx_thr_event:        1;
            uint32_t ch1_rx_thr_event:        1;
            uint32_t ch2_rx_thr_event:        1;
            uint32_t ch3_rx_thr_event:        1;
            uint32_t reserved24:              8;
        };
        uint32_t val;
    } int_st;
    volatile union {
        struct {
            uint32_t ch0_tx_end:               1;
            uint32_t ch0_rx_end:               1;
            uint32_t ch0_err:                  1;
            uint32_t ch1_tx_end:               1;
            uint32_t ch1_rx_end:               1;
            uint32_t ch1_err:                  1;
            uint32_t ch2_tx_end:               1;
            uint32_t ch2_rx_end:               1;
            uint32_t ch2_err:                  1;
            uint32_t ch3_tx_end:               1;
            uint32_t ch3_rx_end:               1;
            uint32_t ch3_err:                  1;
            uint32_t ch0_tx_thr_event:         1;
            uint32_t ch1_tx_thr_event:         1;
            uint32_t ch2_tx_thr_event:         1;
            uint32_t ch3_tx_thr_event:         1;
            uint32_t ch0_tx_loop:              1;
            uint32_t ch1_tx_loop:              1;
            uint32_t ch2_tx_loop:              1;
            uint32_t ch3_tx_loop:              1;
            uint32_t ch0_rx_thr_event:         1;
            uint32_t ch1_rx_thr_event:         1;
            uint32_t ch2_rx_thr_event:         1;
            uint32_t ch3_rx_thr_event:         1;
            uint32_t reserved24:               8;
        };
        uint32_t val;
    } int_ena;
    volatile union {
        struct {
            uint32_t ch0_tx_end:               1;
            uint32_t ch0_rx_end:               1;
            uint32_t ch0_err:                  1;
            uint32_t ch1_tx_end:               1;
            uint32_t ch1_rx_end:               1;
            uint32_t ch1_err:                  1;
            uint32_t ch2_tx_end:               1;
            uint32_t ch2_rx_end:               1;
            uint32_t ch2_err:                  1;
            uint32_t ch3_tx_end:               1;
            uint32_t ch3_rx_end:               1;
            uint32_t ch3_err:                  1;
            uint32_t ch0_tx_thr_event:         1;
            uint32_t ch1_tx_thr_event:         1;
            uint32_t ch2_tx_thr_event:         1;
            uint32_t ch3_tx_thr_event:         1;
            uint32_t ch0_tx_loop:              1;
            uint32_t ch1_tx_loop:              1;
            uint32_t ch2_tx_loop:              1;
            uint32_t ch3_tx_loop:              1;
            uint32_t ch0_rx_thr_event:         1;
            uint32_t ch1_rx_thr_event:         1;
            uint32_t ch2_rx_thr_event:         1;
            uint32_t ch3_rx_thr_event:         1;
            uint32_t reserved24:               8;
        };
        uint32_t val;
    } int_clr;
    volatile union {
        struct {
            uint32_t low:             16;
            uint32_t high:            16;
        };
        uint32_t val;
    } carrier_duty_ch[4];
    volatile union {
        struct {
            uint32_t limit:                9;
            uint32_t tx_loop_num:         10;
            uint32_t tx_loop_cnt_en:       1;
            uint32_t loop_count_reset:     1;
            uint32_t rx_lim:               9;
            uint32_t reserved30:           2;
        };
        uint32_t val;
    } tx_lim_ch[4];
    volatile union {
        struct {
            uint32_t fifo_mask:        1;
            uint32_t mem_tx_wrap_en:   1;
            uint32_t mem_clk_force_on: 1;
            uint32_t mem_force_pd:     1;
            uint32_t mem_force_pu:     1;
            uint32_t reserved5:       26;
            uint32_t clk_en:           1;
        };
        uint32_t val;
    } apb_conf;
    volatile union {
        struct {
            uint32_t ch0:        1;
            uint32_t ch1:        1;
            uint32_t ch2:        1;
            uint32_t ch3:        1;
            uint32_t en:         1;
            uint32_t reserved5: 27;
        };
        uint32_t val;
    } tx_sim;
    volatile union {
        struct {
            uint32_t ch0:             1;
            uint32_t ch1:             1;
            uint32_t ch2:             1;
            uint32_t ch3:             1;
            uint32_t reserved4:      28;
        };
        uint32_t val;
    } ref_cnt_rst;
    volatile union {
        struct {
            uint32_t carrier_low_thres_ch: 16;
            uint32_t carrier_high_thres_ch: 16;
        };
        uint32_t val;
    } ch_rx_carrier_rm[4];
    uint32_t reserved_9c;
    uint32_t reserved_a0;
    uint32_t reserved_a4;
    uint32_t reserved_a8;
    uint32_t reserved_ac;
    uint32_t reserved_b0;
    uint32_t reserved_b4;
    uint32_t reserved_b8;
    uint32_t reserved_bc;
    uint32_t reserved_c0;
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
    volatile uint32_t date;
} rmt_dev_t;

extern rmt_dev_t RMT;

#ifdef __cplusplus
}
#endif
