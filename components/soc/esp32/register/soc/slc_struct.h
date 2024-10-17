/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _SOC_SLC_STRUCT_H_
#define _SOC_SLC_STRUCT_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct slc_dev_s {
    union {
        struct {
            uint32_t slc0_tx_rst:         1;
            uint32_t slc0_rx_rst:         1;
            uint32_t ahbm_fifo_rst:       1;
            uint32_t ahbm_rst:            1;
            uint32_t slc0_tx_loop_test:   1;
            uint32_t slc0_rx_loop_test:   1;
            uint32_t slc0_rx_auto_wrback: 1;
            uint32_t slc0_rx_no_restart_clr: 1;
            uint32_t slc0_rxdscr_burst_en: 1;
            uint32_t slc0_rxdata_burst_en: 1;
            uint32_t slc0_rxlink_auto_ret: 1;
            uint32_t slc0_txlink_auto_ret: 1;
            uint32_t slc0_txdscr_burst_en: 1;
            uint32_t slc0_txdata_burst_en: 1;
            uint32_t slc0_token_auto_clr: 1;
            uint32_t slc0_token_sel:      1;
            uint32_t slc1_tx_rst:         1;
            uint32_t slc1_rx_rst:         1;
            uint32_t slc0_wr_retry_mask_en: 1;
            uint32_t slc1_wr_retry_mask_en: 1;
            uint32_t slc1_tx_loop_test:   1;
            uint32_t slc1_rx_loop_test:   1;
            uint32_t slc1_rx_auto_wrback: 1;
            uint32_t slc1_rx_no_restart_clr: 1;
            uint32_t slc1_rxdscr_burst_en: 1;
            uint32_t slc1_rxdata_burst_en: 1;
            uint32_t slc1_rxlink_auto_ret: 1;
            uint32_t slc1_txlink_auto_ret: 1;
            uint32_t slc1_txdscr_burst_en: 1;
            uint32_t slc1_txdata_burst_en: 1;
            uint32_t slc1_token_auto_clr: 1;
            uint32_t slc1_token_sel:      1;
        };
        uint32_t val;
    } conf0;
    union {
        struct {
            uint32_t frhost_bit0:             1;
            uint32_t frhost_bit1:             1;
            uint32_t frhost_bit2:             1;
            uint32_t frhost_bit3:             1;
            uint32_t frhost_bit4:             1;
            uint32_t frhost_bit5:             1;
            uint32_t frhost_bit6:             1;
            uint32_t frhost_bit7:             1;
            uint32_t rx_start:                1;
            uint32_t tx_start:                1;
            uint32_t rx_udf:                  1;
            uint32_t tx_ovf:                  1;
            uint32_t token0_1to0:             1;
            uint32_t token1_1to0:             1;
            uint32_t tx_done:                 1;
            uint32_t tx_suc_eof:              1;
            uint32_t rx_done:                 1;
            uint32_t rx_eof:                  1;
            uint32_t tohost:                  1;
            uint32_t tx_dscr_err:             1;
            uint32_t rx_dscr_err:             1;
            uint32_t tx_dscr_empty:           1;
            uint32_t host_rd_ack:             1;
            uint32_t wr_retry_done:           1;
            uint32_t tx_err_eof:              1;
            uint32_t cmd_dtc:                 1;
            uint32_t rx_quick_eof:            1;
            uint32_t reserved27:              5;
        };
        uint32_t val;
    } slc0_int_raw;
    union {
        struct {
            uint32_t frhost_bit0:            1;
            uint32_t frhost_bit1:            1;
            uint32_t frhost_bit2:            1;
            uint32_t frhost_bit3:            1;
            uint32_t frhost_bit4:            1;
            uint32_t frhost_bit5:            1;
            uint32_t frhost_bit6:            1;
            uint32_t frhost_bit7:            1;
            uint32_t rx_start:               1;
            uint32_t tx_start:               1;
            uint32_t rx_udf:                 1;
            uint32_t tx_ovf:                 1;
            uint32_t token0_1to0:            1;
            uint32_t token1_1to0:            1;
            uint32_t tx_done:                1;
            uint32_t tx_suc_eof:             1;
            uint32_t rx_done:                1;
            uint32_t rx_eof:                 1;
            uint32_t tohost:                 1;
            uint32_t tx_dscr_err:            1;
            uint32_t rx_dscr_err:            1;
            uint32_t tx_dscr_empty:          1;
            uint32_t host_rd_ack:            1;
            uint32_t wr_retry_done:          1;
            uint32_t tx_err_eof:             1;
            uint32_t cmd_dtc:                1;
            uint32_t rx_quick_eof:           1;
            uint32_t reserved27:             5;
        };
        uint32_t val;
    } slc0_int_st;
    union {
        struct {
            uint32_t frhost_bit0:             1;
            uint32_t frhost_bit1:             1;
            uint32_t frhost_bit2:             1;
            uint32_t frhost_bit3:             1;
            uint32_t frhost_bit4:             1;
            uint32_t frhost_bit5:             1;
            uint32_t frhost_bit6:             1;
            uint32_t frhost_bit7:             1;
            uint32_t rx_start:                1;
            uint32_t tx_start:                1;
            uint32_t rx_udf:                  1;
            uint32_t tx_ovf:                  1;
            uint32_t token0_1to0:             1;
            uint32_t token1_1to0:             1;
            uint32_t tx_done:                 1;
            uint32_t tx_suc_eof:              1;
            uint32_t rx_done:                 1;
            uint32_t rx_eof:                  1;
            uint32_t tohost:                  1;
            uint32_t tx_dscr_err:             1;
            uint32_t rx_dscr_err:             1;
            uint32_t tx_dscr_empty:           1;
            uint32_t host_rd_ack:             1;
            uint32_t wr_retry_done:           1;
            uint32_t tx_err_eof:              1;
            uint32_t cmd_dtc:                 1;
            uint32_t rx_quick_eof:            1;
            uint32_t reserved27:              5;
        };
        uint32_t val;
    } slc0_int_ena;
    union {
        struct {
            uint32_t frhost_bit0:             1;
            uint32_t frhost_bit1:             1;
            uint32_t frhost_bit2:             1;
            uint32_t frhost_bit3:             1;
            uint32_t frhost_bit4:             1;
            uint32_t frhost_bit5:             1;
            uint32_t frhost_bit6:             1;
            uint32_t frhost_bit7:             1;
            uint32_t rx_start:                1;
            uint32_t tx_start:                1;
            uint32_t rx_udf:                  1;
            uint32_t tx_ovf:                  1;
            uint32_t token0_1to0:             1;
            uint32_t token1_1to0:             1;
            uint32_t tx_done:                 1;
            uint32_t tx_suc_eof:              1;
            uint32_t rx_done:                 1;
            uint32_t rx_eof:                  1;
            uint32_t tohost:                  1;
            uint32_t tx_dscr_err:             1;
            uint32_t rx_dscr_err:             1;
            uint32_t tx_dscr_empty:           1;
            uint32_t host_rd_ack:             1;
            uint32_t wr_retry_done:           1;
            uint32_t tx_err_eof:              1;
            uint32_t cmd_dtc:                 1;
            uint32_t rx_quick_eof:            1;
            uint32_t reserved27:              5;
        };
        uint32_t val;
    } slc0_int_clr;
    union {
        struct {
            uint32_t frhost_bit8:             1;
            uint32_t frhost_bit9:             1;
            uint32_t frhost_bit10:            1;
            uint32_t frhost_bit11:            1;
            uint32_t frhost_bit12:            1;
            uint32_t frhost_bit13:            1;
            uint32_t frhost_bit14:            1;
            uint32_t frhost_bit15:            1;
            uint32_t rx_start:                1;
            uint32_t tx_start:                1;
            uint32_t rx_udf:                  1;
            uint32_t tx_ovf:                  1;
            uint32_t token0_1to0:             1;
            uint32_t token1_1to0:             1;
            uint32_t tx_done:                 1;
            uint32_t tx_suc_eof:              1;
            uint32_t rx_done:                 1;
            uint32_t rx_eof:                  1;
            uint32_t tohost:                  1;
            uint32_t tx_dscr_err:             1;
            uint32_t rx_dscr_err:             1;
            uint32_t tx_dscr_empty:           1;
            uint32_t host_rd_ack:             1;
            uint32_t wr_retry_done:           1;
            uint32_t tx_err_eof:              1;
            uint32_t reserved25:              7;
        };
        uint32_t val;
    } slc1_int_raw;
    union {
        struct {
            uint32_t frhost_bit8:            1;
            uint32_t frhost_bit9:            1;
            uint32_t frhost_bit10:           1;
            uint32_t frhost_bit11:           1;
            uint32_t frhost_bit12:           1;
            uint32_t frhost_bit13:           1;
            uint32_t frhost_bit14:           1;
            uint32_t frhost_bit15:           1;
            uint32_t rx_start:               1;
            uint32_t tx_start:               1;
            uint32_t rx_udf:                 1;
            uint32_t tx_ovf:                 1;
            uint32_t token0_1to0:            1;
            uint32_t token1_1to0:            1;
            uint32_t tx_done:                1;
            uint32_t tx_suc_eof:             1;
            uint32_t rx_done:                1;
            uint32_t rx_eof:                 1;
            uint32_t tohost:                 1;
            uint32_t tx_dscr_err:            1;
            uint32_t rx_dscr_err:            1;
            uint32_t tx_dscr_empty:          1;
            uint32_t host_rd_ack:            1;
            uint32_t wr_retry_done:          1;
            uint32_t tx_err_eof:             1;
            uint32_t reserved25:             7;
        };
        uint32_t val;
    } slc1_int_st;
    union {
        struct {
            uint32_t frhost_bit8:             1;
            uint32_t frhost_bit9:             1;
            uint32_t frhost_bit10:            1;
            uint32_t frhost_bit11:            1;
            uint32_t frhost_bit12:            1;
            uint32_t frhost_bit13:            1;
            uint32_t frhost_bit14:            1;
            uint32_t frhost_bit15:            1;
            uint32_t rx_start:                1;
            uint32_t tx_start:                1;
            uint32_t rx_udf:                  1;
            uint32_t tx_ovf:                  1;
            uint32_t token0_1to0:             1;
            uint32_t token1_1to0:             1;
            uint32_t tx_done:                 1;
            uint32_t tx_suc_eof:              1;
            uint32_t rx_done:                 1;
            uint32_t rx_eof:                  1;
            uint32_t tohost:                  1;
            uint32_t tx_dscr_err:             1;
            uint32_t rx_dscr_err:             1;
            uint32_t tx_dscr_empty:           1;
            uint32_t host_rd_ack:             1;
            uint32_t wr_retry_done:           1;
            uint32_t tx_err_eof:              1;
            uint32_t reserved25:              7;
        };
        uint32_t val;
    } slc1_int_ena;
    union {
        struct {
            uint32_t frhost_bit8:             1;
            uint32_t frhost_bit9:             1;
            uint32_t frhost_bit10:            1;
            uint32_t frhost_bit11:            1;
            uint32_t frhost_bit12:            1;
            uint32_t frhost_bit13:            1;
            uint32_t frhost_bit14:            1;
            uint32_t frhost_bit15:            1;
            uint32_t rx_start:                1;
            uint32_t tx_start:                1;
            uint32_t rx_udf:                  1;
            uint32_t tx_ovf:                  1;
            uint32_t token0_1to0:             1;
            uint32_t token1_1to0:             1;
            uint32_t tx_done:                 1;
            uint32_t tx_suc_eof:              1;
            uint32_t rx_done:                 1;
            uint32_t rx_eof:                  1;
            uint32_t tohost:                  1;
            uint32_t tx_dscr_err:             1;
            uint32_t rx_dscr_err:             1;
            uint32_t tx_dscr_empty:           1;
            uint32_t host_rd_ack:             1;
            uint32_t wr_retry_done:           1;
            uint32_t tx_err_eof:              1;
            uint32_t reserved25:              7;
        };
        uint32_t val;
    } slc1_int_clr;
    union {
        struct {
            uint32_t slc0_rx_full: 1;
            uint32_t slc0_rx_empty: 1;
            uint32_t reserved2: 14;
            uint32_t slc1_rx_full: 1;
            uint32_t slc1_rx_empty: 1;
            uint32_t reserved18:14;
        };
        uint32_t val;
    } rx_status;
    union {
        struct {
            uint32_t rxfifo_wdata:   9;
            uint32_t reserved9:      7;
            uint32_t rxfifo_push:    1;
            uint32_t reserved17:    15;
        };
        uint32_t val;
    } slc0_rxfifo_push;
    union {
        struct {
            uint32_t rxfifo_wdata:   9;
            uint32_t reserved9:      7;
            uint32_t rxfifo_push:    1;
            uint32_t reserved17:    15;
        };
        uint32_t val;
    } slc1_rxfifo_push;
    union {
        struct {
            uint32_t slc0_tx_full: 1;
            uint32_t slc0_tx_empty: 1;
            uint32_t reserved2: 14;
            uint32_t slc1_tx_full: 1;
            uint32_t slc1_tx_empty: 1;
            uint32_t reserved18:14;
        };
        uint32_t val;
    } tx_status;
    union {
        struct {
            uint32_t txfifo_rdata:  11;
            uint32_t reserved11:     5;
            uint32_t txfifo_pop:     1;
            uint32_t reserved17:    15;
        };
        uint32_t val;
    } slc0_txfifo_pop;
    union {
        struct {
            uint32_t txfifo_rdata:  11;
            uint32_t reserved11:     5;
            uint32_t txfifo_pop:     1;
            uint32_t reserved17:    15;
        };
        uint32_t val;
    } slc1_txfifo_pop;
    union {
        struct {
            uint32_t addr:            20;
            uint32_t reserved20:       8;
            uint32_t stop:             1;
            uint32_t start:            1;
            uint32_t restart:          1;
            uint32_t park:             1;
        };
        uint32_t val;
    } slc0_rx_link;
    union {
        struct {
            uint32_t addr:            20;
            uint32_t reserved20:       8;
            uint32_t stop:             1;
            uint32_t start:            1;
            uint32_t restart:          1;
            uint32_t park:             1;
        };
        uint32_t val;
    } slc0_tx_link;
    union {
        struct {
            uint32_t addr:            20;
            uint32_t bt_packet:        1;
            uint32_t reserved21:       7;
            uint32_t stop:             1;
            uint32_t start:            1;
            uint32_t restart:          1;
            uint32_t park:             1;
        };
        uint32_t val;
    } slc1_rx_link;
    union {
        struct {
            uint32_t addr:            20;
            uint32_t reserved20:       8;
            uint32_t stop:             1;
            uint32_t start:            1;
            uint32_t restart:          1;
            uint32_t park:             1;
        };
        uint32_t val;
    } slc1_tx_link;
    union {
        struct {
            uint32_t slc0_intvec:     8;
            uint32_t reserved8:       8;
            uint32_t slc1_intvec:     8;
            uint32_t reserved24:      8;
        };
        uint32_t val;
    } intvec_tohost;
    union {
        struct {
            uint32_t wdata:            12;
            uint32_t wr:                1;
            uint32_t inc:               1;
            uint32_t inc_more:          1;
            uint32_t reserved15:        1;
            uint32_t token0:           12;
            uint32_t reserved28:        4;
        };
        uint32_t val;
    } slc0_token0;
    union {
        struct {
            uint32_t wdata:            12;
            uint32_t wr:                1;
            uint32_t inc:               1;
            uint32_t inc_more:          1;
            uint32_t reserved15:        1;
            uint32_t token1:           12;
            uint32_t reserved28:        4;
        };
        uint32_t val;
    } slc0_token1;
    union {
        struct {
            uint32_t wdata:            12;
            uint32_t wr:                1;
            uint32_t inc:               1;
            uint32_t inc_more:          1;
            uint32_t reserved15:        1;
            uint32_t token0:           12;
            uint32_t reserved28:        4;
        };
        uint32_t val;
    } slc1_token0;
    union {
        struct {
            uint32_t wdata:            12;
            uint32_t wr:                1;
            uint32_t inc:               1;
            uint32_t inc_more:          1;
            uint32_t reserved15:        1;
            uint32_t token1:           12;
            uint32_t reserved28:        4;
        };
        uint32_t val;
    } slc1_token1;
    union {
        struct {
            uint32_t slc0_check_owner:   1;
            uint32_t slc0_tx_check_sum_en: 1;
            uint32_t slc0_rx_check_sum_en: 1;
            uint32_t cmd_hold_en:        1;
            uint32_t slc0_len_auto_clr:  1;
            uint32_t slc0_tx_stitch_en:  1;
            uint32_t slc0_rx_stitch_en:  1;
            uint32_t reserved7:          9;
            uint32_t slc1_check_owner:   1;
            uint32_t slc1_tx_check_sum_en: 1;
            uint32_t slc1_rx_check_sum_en: 1;
            uint32_t host_int_level_sel: 1;
            uint32_t slc1_tx_stitch_en:  1;
            uint32_t slc1_rx_stitch_en:  1;
            uint32_t clk_en:             1;
            uint32_t reserved23:         9;
        };
        uint32_t val;
    } conf1;
    uint32_t slc0_state0;                                  /**/
    uint32_t slc0_state1;                                  /**/
    uint32_t slc1_state0;                                  /**/
    uint32_t slc1_state1;                                  /**/
    union {
        struct {
            uint32_t txeof_ena:        6;
            uint32_t reserved6:        2;
            uint32_t fifo_map_ena:     4;
            uint32_t slc0_tx_dummy_mode: 1;
            uint32_t hda_map_128k:     1;
            uint32_t slc1_tx_dummy_mode: 1;
            uint32_t reserved15:       1;
            uint32_t tx_push_idle_num:16;
        };
        uint32_t val;
    } bridge_conf;
    uint32_t slc0_to_eof_des_addr;                         /**/
    uint32_t slc0_tx_eof_des_addr;                         /**/
    uint32_t slc0_to_eof_bfr_des_addr;                     /**/
    uint32_t slc1_to_eof_des_addr;                         /**/
    uint32_t slc1_tx_eof_des_addr;                         /**/
    uint32_t slc1_to_eof_bfr_des_addr;                     /**/
    union {
        struct {
            uint32_t mode:         3;
            uint32_t reserved3:    1;
            uint32_t addr:         2;
            uint32_t reserved6:   26;
        };
        uint32_t val;
    } ahb_test;
    union {
        struct {
            uint32_t cmd_st:          3;
            uint32_t reserved3:       1;
            uint32_t func_st:         4;
            uint32_t sdio_wakeup:     1;
            uint32_t reserved9:       3;
            uint32_t bus_st:          3;
            uint32_t reserved15:      1;
            uint32_t func1_acc_state: 5;
            uint32_t reserved21:      3;
            uint32_t func2_acc_state: 5;
            uint32_t reserved29:      3;
        };
        uint32_t val;
    } sdio_st;
    union {
        struct {
            uint32_t slc0_token_no_replace: 1;
            uint32_t slc0_infor_no_replace: 1;
            uint32_t slc0_rx_fill_mode:    1;
            uint32_t slc0_rx_eof_mode:     1;
            uint32_t slc0_rx_fill_en:      1;
            uint32_t slc0_rd_retry_threshold:11;
            uint32_t slc1_token_no_replace: 1;
            uint32_t slc1_infor_no_replace: 1;
            uint32_t slc1_rx_fill_mode:    1;
            uint32_t slc1_rx_eof_mode:     1;
            uint32_t slc1_rx_fill_en:      1;
            uint32_t slc1_rd_retry_threshold:11;
        };
        uint32_t val;
    } rx_dscr_conf;
    uint32_t slc0_txlink_dscr;                             /**/
    uint32_t slc0_txlink_dscr_bf0;                         /**/
    uint32_t slc0_txlink_dscr_bf1;                         /**/
    uint32_t slc0_rxlink_dscr;                             /**/
    uint32_t slc0_rxlink_dscr_bf0;                         /**/
    uint32_t slc0_rxlink_dscr_bf1;                         /**/
    uint32_t slc1_txlink_dscr;                             /**/
    uint32_t slc1_txlink_dscr_bf0;                         /**/
    uint32_t slc1_txlink_dscr_bf1;                         /**/
    uint32_t slc1_rxlink_dscr;                             /**/
    uint32_t slc1_rxlink_dscr_bf0;                         /**/
    uint32_t slc1_rxlink_dscr_bf1;                         /**/
    uint32_t slc0_tx_erreof_des_addr;                      /**/
    uint32_t slc1_tx_erreof_des_addr;                      /**/
    union {
        struct {
            uint32_t slc0_token:12;
            uint32_t reserved12: 4;
            uint32_t slc1_token:12;
            uint32_t reserved28: 4;
        };
        uint32_t val;
    } token_lat;
    union {
        struct {
            uint32_t wr_retry_threshold:11;
            uint32_t reserved11:        21;
        };
        uint32_t val;
    } tx_dscr_conf;
    uint32_t cmd_infor0;                                /**/
    uint32_t cmd_infor1;                                /**/
    union {
        struct {
            uint32_t len_wdata:          20;
            uint32_t len_wr:              1;
            uint32_t len_inc:             1;
            uint32_t len_inc_more:        1;
            uint32_t rx_packet_load_en:   1;
            uint32_t tx_packet_load_en:   1;
            uint32_t rx_get_used_dscr:    1;
            uint32_t tx_get_used_dscr:    1;
            uint32_t rx_new_pkt_ind:      1;
            uint32_t tx_new_pkt_ind:      1;
            uint32_t reserved29:          3;
        };
        uint32_t val;
    } slc0_len_conf;
    union {
        struct {
            uint32_t len:       20;
            uint32_t reserved20:12;
        };
        uint32_t val;
    } slc0_length;
    uint32_t slc0_txpkt_h_dscr;                            /**/
    uint32_t slc0_txpkt_e_dscr;                            /**/
    uint32_t slc0_rxpkt_h_dscr;                            /**/
    uint32_t slc0_rxpkt_e_dscr;                            /**/
    uint32_t slc0_txpktu_h_dscr;                           /**/
    uint32_t slc0_txpktu_e_dscr;                           /**/
    uint32_t slc0_rxpktu_h_dscr;                           /**/
    uint32_t slc0_rxpktu_e_dscr;                           /**/
    uint32_t reserved_10c;
    uint32_t reserved_110;
    union {
        struct {
            uint32_t slc0_position:  8;
            uint32_t slc1_position:  8;
            uint32_t reserved16:    16;
        };
        uint32_t val;
    } seq_position;
    union {
        struct {
            uint32_t rx_dscr_rec_lim:  10;
            uint32_t reserved10:       22;
        };
        uint32_t val;
    } slc0_dscr_rec_conf;
    union {
        struct {
            uint32_t dat0_crc_err_cnt: 8;
            uint32_t dat1_crc_err_cnt: 8;
            uint32_t dat2_crc_err_cnt: 8;
            uint32_t dat3_crc_err_cnt: 8;
        };
        uint32_t val;
    } sdio_crc_st0;
    union {
        struct {
            uint32_t cmd_crc_err_cnt: 8;
            uint32_t reserved8:      23;
            uint32_t err_cnt_clr:     1;
        };
        uint32_t val;
    } sdio_crc_st1;
    uint32_t slc0_eof_start_des;                           /**/
    uint32_t slc0_push_dscr_addr;                          /**/
    uint32_t slc0_done_dscr_addr;                          /**/
    uint32_t slc0_sub_start_des;                           /**/
    union {
        struct {
            uint32_t rx_dscr_cnt_lat:  10;
            uint32_t reserved10:        6;
            uint32_t rx_get_eof_occ:    1;
            uint32_t reserved17:       15;
        };
        uint32_t val;
    } slc0_dscr_cnt;
    union {
        struct {
            uint32_t len_lim:   20;
            uint32_t reserved20:12;
        };
        uint32_t val;
    } slc0_len_lim_conf;
    union {
        struct {
            uint32_t frhost_bit01:            1;
            uint32_t frhost_bit11:            1;
            uint32_t frhost_bit21:            1;
            uint32_t frhost_bit31:            1;
            uint32_t frhost_bit41:            1;
            uint32_t frhost_bit51:            1;
            uint32_t frhost_bit61:            1;
            uint32_t frhost_bit71:            1;
            uint32_t rx_start1:               1;
            uint32_t tx_start1:               1;
            uint32_t rx_udf1:                 1;
            uint32_t tx_ovf1:                 1;
            uint32_t token0_1to01:            1;
            uint32_t token1_1to01:            1;
            uint32_t tx_done1:                1;
            uint32_t tx_suc_eof1:             1;
            uint32_t rx_done1:                1;
            uint32_t rx_eof1:                 1;
            uint32_t tohost1:                 1;
            uint32_t tx_dscr_err1:            1;
            uint32_t rx_dscr_err1:            1;
            uint32_t tx_dscr_empty1:          1;
            uint32_t host_rd_ack1:            1;
            uint32_t wr_retry_done1:          1;
            uint32_t tx_err_eof1:             1;
            uint32_t cmd_dtc1:                1;
            uint32_t rx_quick_eof1:           1;
            uint32_t reserved27:              5;
        };
        uint32_t val;
    } slc0_int_st1;
    union {
        struct {
            uint32_t frhost_bit01:             1;
            uint32_t frhost_bit11:             1;
            uint32_t frhost_bit21:             1;
            uint32_t frhost_bit31:             1;
            uint32_t frhost_bit41:             1;
            uint32_t frhost_bit51:             1;
            uint32_t frhost_bit61:             1;
            uint32_t frhost_bit71:             1;
            uint32_t rx_start1:                1;
            uint32_t tx_start1:                1;
            uint32_t rx_udf1:                  1;
            uint32_t tx_ovf1:                  1;
            uint32_t token0_1to01:             1;
            uint32_t token1_1to01:             1;
            uint32_t tx_done1:                 1;
            uint32_t tx_suc_eof1:              1;
            uint32_t rx_done1:                 1;
            uint32_t rx_eof1:                  1;
            uint32_t tohost1:                  1;
            uint32_t tx_dscr_err1:             1;
            uint32_t rx_dscr_err1:             1;
            uint32_t tx_dscr_empty1:           1;
            uint32_t host_rd_ack1:             1;
            uint32_t wr_retry_done1:           1;
            uint32_t tx_err_eof1:              1;
            uint32_t cmd_dtc1:                 1;
            uint32_t rx_quick_eof1:            1;
            uint32_t reserved27:               5;
        };
        uint32_t val;
    } slc0_int_ena1;
    union {
        struct {
            uint32_t frhost_bit81:            1;
            uint32_t frhost_bit91:            1;
            uint32_t frhost_bit101:           1;
            uint32_t frhost_bit111:           1;
            uint32_t frhost_bit121:           1;
            uint32_t frhost_bit131:           1;
            uint32_t frhost_bit141:           1;
            uint32_t frhost_bit151:           1;
            uint32_t rx_start1:               1;
            uint32_t tx_start1:               1;
            uint32_t rx_udf1:                 1;
            uint32_t tx_ovf1:                 1;
            uint32_t token0_1to01:            1;
            uint32_t token1_1to01:            1;
            uint32_t tx_done1:                1;
            uint32_t tx_suc_eof1:             1;
            uint32_t rx_done1:                1;
            uint32_t rx_eof1:                 1;
            uint32_t tohost1:                 1;
            uint32_t tx_dscr_err1:            1;
            uint32_t rx_dscr_err1:            1;
            uint32_t tx_dscr_empty1:          1;
            uint32_t host_rd_ack1:            1;
            uint32_t wr_retry_done1:          1;
            uint32_t tx_err_eof1:             1;
            uint32_t reserved25:              7;
        };
        uint32_t val;
    } slc1_int_st1;
    union {
        struct {
            uint32_t frhost_bit81:             1;
            uint32_t frhost_bit91:             1;
            uint32_t frhost_bit101:            1;
            uint32_t frhost_bit111:            1;
            uint32_t frhost_bit121:            1;
            uint32_t frhost_bit131:            1;
            uint32_t frhost_bit141:            1;
            uint32_t frhost_bit151:            1;
            uint32_t rx_start1:                1;
            uint32_t tx_start1:                1;
            uint32_t rx_udf1:                  1;
            uint32_t tx_ovf1:                  1;
            uint32_t token0_1to01:             1;
            uint32_t token1_1to01:             1;
            uint32_t tx_done1:                 1;
            uint32_t tx_suc_eof1:              1;
            uint32_t rx_done1:                 1;
            uint32_t rx_eof1:                  1;
            uint32_t tohost1:                  1;
            uint32_t tx_dscr_err1:             1;
            uint32_t rx_dscr_err1:             1;
            uint32_t tx_dscr_empty1:           1;
            uint32_t host_rd_ack1:             1;
            uint32_t wr_retry_done1:           1;
            uint32_t tx_err_eof1:              1;
            uint32_t reserved25:               7;
        };
        uint32_t val;
    } slc1_int_ena1;
    uint32_t reserved_14c;
    uint32_t reserved_150;
    uint32_t reserved_154;
    uint32_t reserved_158;
    uint32_t reserved_15c;
    uint32_t reserved_160;
    uint32_t reserved_164;
    uint32_t reserved_168;
    uint32_t reserved_16c;
    uint32_t reserved_170;
    uint32_t reserved_174;
    uint32_t reserved_178;
    uint32_t reserved_17c;
    uint32_t reserved_180;
    uint32_t reserved_184;
    uint32_t reserved_188;
    uint32_t reserved_18c;
    uint32_t reserved_190;
    uint32_t reserved_194;
    uint32_t reserved_198;
    uint32_t reserved_19c;
    uint32_t reserved_1a0;
    uint32_t reserved_1a4;
    uint32_t reserved_1a8;
    uint32_t reserved_1ac;
    uint32_t reserved_1b0;
    uint32_t reserved_1b4;
    uint32_t reserved_1b8;
    uint32_t reserved_1bc;
    uint32_t reserved_1c0;
    uint32_t reserved_1c4;
    uint32_t reserved_1c8;
    uint32_t reserved_1cc;
    uint32_t reserved_1d0;
    uint32_t reserved_1d4;
    uint32_t reserved_1d8;
    uint32_t reserved_1dc;
    uint32_t reserved_1e0;
    uint32_t reserved_1e4;
    uint32_t reserved_1e8;
    uint32_t reserved_1ec;
    uint32_t reserved_1f0;
    uint32_t reserved_1f4;
    uint32_t date;                                      /**/
    uint32_t id;                                        /**/
} slc_dev_t;
extern slc_dev_t SLC;

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_SLC_STRUCT_H_ */
