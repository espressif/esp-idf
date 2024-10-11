/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: out */
/** Type of out_conf0_chn register
 *  Configures the tx direction of channel 0
 */
typedef union {
    struct {
        /** out_auto_wrback_chn : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data pointed by
         *  outlink descriptor has been received.
         */
        uint32_t out_auto_wrback_chn:1;
        /** out_eof_mode_chn : R/W; bitpos: [1]; default: 1;
         *  EOF flag generation mode when receiving data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to read has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode_chn:1;
        /** outdscr_burst_en_chn : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_chn:1;
        /** out_ecc_aes_en_chn : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t out_ecc_aes_en_chn:1;
        /** out_check_owner_chn : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_chn:1;
        /** out_loop_test_chn : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t out_loop_test_chn:1;
        /** out_mem_burst_length_chn : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Tx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t out_mem_burst_length_chn:3;
        /** out_macro_block_size_chn : R/W; bitpos: [10:9]; default: 0;
         *  Sel TX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
         *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
         *  descriptor
         */
        uint32_t out_macro_block_size_chn:2;
        /** out_dscr_port_en_chn : R/W; bitpos: [11]; default: 0;
         *  Set this bit to 1 to obtain descriptor from IP port
         */
        uint32_t out_dscr_port_en_chn:1;
        /** out_page_bound_en_chn : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI read data don't cross the address boundary which
         *  define by mem_burst_length
         */
        uint32_t out_page_bound_en_chn:1;
        uint32_t reserved_13:3;
        /** out_reorder_en_chn : R/W; bitpos: [16]; default: 0;
         *  Enable TX channel 0 macro block reorder when set to 1, only channel0 have this
         *  selection
         */
        uint32_t out_reorder_en_chn:1;
        uint32_t reserved_17:7;
        /** out_rst_chn : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset TX channel
         */
        uint32_t out_rst_chn:1;
        /** out_cmd_disable_chn : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t out_cmd_disable_chn:1;
        /** out_arb_weight_opt_dis_chn : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t out_arb_weight_opt_dis_chn:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} dma2d_out_conf0_chn_reg_t;

/** Type of out_int_raw_chn register
 *  Raw interrupt status of TX channel 0
 */
typedef union {
    struct {
        /** out_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_chn_int_raw:1;
        /** out_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_chn_int_raw:1;
        /** out_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error,
         *  including owner error, the second and third word error of outlink descriptor for Tx
         *  channel 0.
         */
        uint32_t out_dscr_err_chn_int_raw:1;
        /** out_total_eof_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_chn_int_raw:1;
        /** outfifo_ovf_l1_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l1_chn_int_raw:1;
        /** outfifo_udf_l1_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l1_chn_int_raw:1;
        /** outfifo_ovf_l2_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l2_chn_int_raw:1;
        /** outfifo_udf_l2_chn_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l2_chn_int_raw:1;
        /** outfifo_ovf_l3_chn_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is overflow.
         */
        uint32_t outfifo_ovf_l3_chn_int_raw:1;
        /** outfifo_udf_l3_chn_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when fifo is underflow.
         */
        uint32_t outfifo_udf_l3_chn_int_raw:1;
        /** outfifo_ro_ovf_chn_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw interrupt bit turns to high level when reorder fifo is overflow.
         */
        uint32_t outfifo_ro_ovf_chn_int_raw:1;
        /** outfifo_ro_udf_chn_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt bit turns to high level when reorder fifo is underflow.
         */
        uint32_t outfifo_ro_udf_chn_int_raw:1;
        /** out_dscr_task_ovf_chn_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t out_dscr_task_ovf_chn_int_raw:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dma2d_out_int_raw_chn_reg_t;

/** Type of out_int_ena_chn register
 *  Interrupt enable bits of TX channel 0
 */
typedef union {
    struct {
        /** out_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_chn_int_ena:1;
        /** out_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_chn_int_ena:1;
        /** out_dscr_err_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_chn_int_ena:1;
        /** out_total_eof_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_chn_int_ena:1;
        /** outfifo_ovf_l1_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_chn_int_ena:1;
        /** outfifo_udf_l1_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_chn_int_ena:1;
        /** outfifo_ovf_l2_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_chn_int_ena:1;
        /** outfifo_udf_l2_chn_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_chn_int_ena:1;
        /** outfifo_ovf_l3_chn_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l3_chn_int_ena:1;
        /** outfifo_udf_l3_chn_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l3_chn_int_ena:1;
        /** outfifo_ro_ovf_chn_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
         */
        uint32_t outfifo_ro_ovf_chn_int_ena:1;
        /** outfifo_ro_udf_chn_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
         */
        uint32_t outfifo_ro_udf_chn_int_ena:1;
        /** out_dscr_task_ovf_chn_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_chn_int_ena:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dma2d_out_int_ena_chn_reg_t;

/** Type of out_int_st_chn register
 *  Masked interrupt status of TX channel 0
 */
typedef union {
    struct {
        /** out_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_chn_int_st:1;
        /** out_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_chn_int_st:1;
        /** out_dscr_err_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_chn_int_st:1;
        /** out_total_eof_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_chn_int_st:1;
        /** outfifo_ovf_l1_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_chn_int_st:1;
        /** outfifo_udf_l1_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_chn_int_st:1;
        /** outfifo_ovf_l2_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_chn_int_st:1;
        /** outfifo_udf_l2_chn_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_chn_int_st:1;
        /** outfifo_ovf_l3_chn_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l3_chn_int_st:1;
        /** outfifo_udf_l3_chn_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l3_chn_int_st:1;
        /** outfifo_ro_ovf_chn_int_st : RO; bitpos: [10]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_RO_OVF_CH_INT interrupt.
         */
        uint32_t outfifo_ro_ovf_chn_int_st:1;
        /** outfifo_ro_udf_chn_int_st : RO; bitpos: [11]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_RO_UDF_CH_INT interrupt.
         */
        uint32_t outfifo_ro_udf_chn_int_st:1;
        /** out_dscr_task_ovf_chn_int_st : RO; bitpos: [12]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_chn_int_st:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dma2d_out_int_st_chn_reg_t;

/** Type of out_int_clr_chn register
 *  Interrupt clear bits of TX channel 0
 */
typedef union {
    struct {
        /** out_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_chn_int_clr:1;
        /** out_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_chn_int_clr:1;
        /** out_dscr_err_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_chn_int_clr:1;
        /** out_total_eof_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_chn_int_clr:1;
        /** outfifo_ovf_l1_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l1_chn_int_clr:1;
        /** outfifo_udf_l1_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l1_chn_int_clr:1;
        /** outfifo_ovf_l2_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l2_chn_int_clr:1;
        /** outfifo_udf_l2_chn_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l2_chn_int_clr:1;
        /** outfifo_ovf_l3_chn_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_l3_chn_int_clr:1;
        /** outfifo_udf_l3_chn_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_udf_l3_chn_int_clr:1;
        /** outfifo_ro_ovf_chn_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the OUTFIFO_RO_OVF_CH_INT interrupt.
         */
        uint32_t outfifo_ro_ovf_chn_int_clr:1;
        /** outfifo_ro_udf_chn_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear the OUTFIFO_RO_UDF_CH_INT interrupt.
         */
        uint32_t outfifo_ro_udf_chn_int_clr:1;
        /** out_dscr_task_ovf_chn_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the OUT_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t out_dscr_task_ovf_chn_int_clr:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} dma2d_out_int_clr_chn_reg_t;

/** Type of outfifo_status_chn register
 *  Represents the status of the tx fifo of channel 0
 */
typedef union {
    struct {
        /** outfifo_full_l2_chn : RO; bitpos: [0]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full_l2_chn:1;
        /** outfifo_empty_l2_chn : RO; bitpos: [1]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty_l2_chn:1;
        /** outfifo_cnt_l2_chn : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt_l2_chn:4;
        uint32_t reserved_6:1;
        /** out_remain_under_1b_chn : RO; bitpos: [7]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_1b_chn:1;
        /** out_remain_under_2b_chn : RO; bitpos: [8]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_2b_chn:1;
        /** out_remain_under_3b_chn : RO; bitpos: [9]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_3b_chn:1;
        /** out_remain_under_4b_chn : RO; bitpos: [10]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_4b_chn:1;
        /** out_remain_under_5b_chn : RO; bitpos: [11]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_5b_chn:1;
        /** out_remain_under_6b_chn : RO; bitpos: [12]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_6b_chn:1;
        /** out_remain_under_7b_chn : RO; bitpos: [13]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_7b_chn:1;
        /** out_remain_under_8b_chn : RO; bitpos: [14]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_8b_chn:1;
        /** outfifo_full_l1_chn : RO; bitpos: [15]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full_l1_chn:1;
        /** outfifo_empty_l1_chn : RO; bitpos: [16]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty_l1_chn:1;
        /** outfifo_cnt_l1_chn : RO; bitpos: [21:17]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt_l1_chn:5;
        /** outfifo_full_l3_chn : RO; bitpos: [22]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full_l3_chn:1;
        /** outfifo_empty_l3_chn : RO; bitpos: [23]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty_l3_chn:1;
        /** outfifo_cnt_l3_chn : RO; bitpos: [28:24]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt_l3_chn:5;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} dma2d_outfifo_status_chn_reg_t;

/** Type of out_push_chn register
 *  Configures the tx fifo of channel 0
 */
typedef union {
    struct {
        /** outfifo_wdata_chn : R/W; bitpos: [9:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA Tx FIFO.
         */
        uint32_t outfifo_wdata_chn:10;
        /** outfifo_push_chn : R/W/SC; bitpos: [10]; default: 0;
         *  Set this bit to push data into DMA Tx FIFO.
         */
        uint32_t outfifo_push_chn:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} dma2d_out_push_chn_reg_t;

/** Type of out_link_conf_chn register
 *  Configures the tx descriptor operations of channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** outlink_stop_chn : R/W/SC; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_chn:1;
        /** outlink_start_chn : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_chn:1;
        /** outlink_restart_chn : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_chn:1;
        /** outlink_park_chn : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_chn:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dma2d_out_link_conf_chn_reg_t;

/** Type of out_link_addr_chn register
 *  Configures the tx descriptor address of channel 0
 */
typedef union {
    struct {
        /** outlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first outlink descriptor's address.
         */
        uint32_t outlink_addr_chn:32;
    };
    uint32_t val;
} dma2d_out_link_addr_chn_reg_t;

/** Type of out_state_chn register
 *  Represents the working status of the tx descriptor of channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_chn:18;
        /** out_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  This register stores the current descriptor state machine state.
         */
        uint32_t out_dscr_state_chn:2;
        /** out_state_chn : RO; bitpos: [23:20]; default: 0;
         *  This register stores the current control module state machine state.
         */
        uint32_t out_state_chn:4;
        /** out_reset_avail_chn : RO; bitpos: [24]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t out_reset_avail_chn:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dma2d_out_state_chn_reg_t;

/** Type of out_eof_des_addr_chn register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
typedef union {
    struct {
        /** out_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_chn:32;
    };
    uint32_t val;
} dma2d_out_eof_des_addr_chn_reg_t;

/** Type of out_dscr_chn register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the next outlink descriptor address y.
         */
        uint32_t outlink_dscr_chn:32;
    };
    uint32_t val;
} dma2d_out_dscr_chn_reg_t;

/** Type of out_dscr_bf0_chn register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor's next address y-1.
         */
        uint32_t outlink_dscr_bf0_chn:32;
    };
    uint32_t val;
} dma2d_out_dscr_bf0_chn_reg_t;

/** Type of out_dscr_bf1_chn register
 *  Represents the address associated with the outlink descriptor of channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor's next address y-2.
         */
        uint32_t outlink_dscr_bf1_chn:32;
    };
    uint32_t val;
} dma2d_out_dscr_bf1_chn_reg_t;

/** Type of out_peri_sel_chn register
 *  Configures the tx peripheral of channel 0
 */
typedef union {
    struct {
        /** out_peri_sel_chn : R/W; bitpos: [2:0]; default: 7;
         *  This register is used to select peripheral for Tx channel   0:  jpeg     1:
         *  display-1     2: display-2     3: display-3      7: no choose
         */
        uint32_t out_peri_sel_chn:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} dma2d_out_peri_sel_chn_reg_t;

/** Type of out_arb_chn register
 *  Configures the tx arbiter of channel 0
 */
typedef union {
    struct {
        /** out_arb_token_num_chn : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t out_arb_token_num_chn:4;
        /** out_arb_priority_chn : R/W; bitpos: [5:4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t out_arb_priority_chn:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dma2d_out_arb_chn_reg_t;

/** Type of out_ro_status_chn register
 *  Represents the status of the tx reorder module of channel 0
 */
typedef union {
    struct {
        /** outfifo_ro_cnt_chn : RO; bitpos: [5:0]; default: 0;
         *  The register stores the byte number of the data in color convert Tx FIFO for
         *  channel 0.
         */
        uint32_t outfifo_ro_cnt_chn:6;
        /** out_ro_wr_state_chn : RO; bitpos: [7:6]; default: 0;
         *  The register stores the state of read ram of reorder
         */
        uint32_t out_ro_wr_state_chn:2;
        /** out_ro_rd_state_chn : RO; bitpos: [9:8]; default: 0;
         *  The register stores the state of write ram of reorder
         */
        uint32_t out_ro_rd_state_chn:2;
        /** out_pixel_byte_chn : RO; bitpos: [13:10]; default: 0;
         *  the number of bytes contained in a pixel at TX channel     0: 1byte     1: 1.5bytes
         *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
         */
        uint32_t out_pixel_byte_chn:4;
        /** out_burst_block_num_chn : RO; bitpos: [17:14]; default: 0;
         *  the number of macro blocks contained in a burst of data at TX channel
         */
        uint32_t out_burst_block_num_chn:4;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} dma2d_out_ro_status_chn_reg_t;

/** Type of out_ro_pd_conf_chn register
 *  Configures the tx reorder memory of channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** out_ro_ram_force_pd_chn : R/W; bitpos: [4]; default: 0;
         *  dma reorder ram power down
         */
        uint32_t out_ro_ram_force_pd_chn:1;
        /** out_ro_ram_force_pu_chn : R/W; bitpos: [5]; default: 1;
         *  dma reorder ram power up
         */
        uint32_t out_ro_ram_force_pu_chn:1;
        /** out_ro_ram_clk_fo_chn : R/W; bitpos: [6]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
         *  0: A gate-clock will be used when accessing the RAM in DMA.
         */
        uint32_t out_ro_ram_clk_fo_chn:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} dma2d_out_ro_pd_conf_chn_reg_t;

/** Type of out_color_convert_chn register
 *  Configures the tx color convert of channel 0
 */
typedef union {
    struct {
        /** out_color_output_sel_chn : R/W; bitpos: [1:0]; default: 0;
         *  Set final color convert process and output type    0: RGB888 to RGB565     1:
         *  YUV444 to YUV422     2: output directly
         */
        uint32_t out_color_output_sel_chn:2;
        /** out_color_3b_proc_en_chn : R/W; bitpos: [2]; default: 0;
         *  Enable generic color convert module between color input & color output, need to
         *  configure parameter.
         */
        uint32_t out_color_3b_proc_en_chn:1;
        /** out_color_input_sel_chn : R/W; bitpos: [5:3]; default: 7;
         *  Set first color convert process and input color type    0: RGB565 to RGB888     1:
         *  YUV422 to YUV444      2: Other 2byte/pixel type    3: Other 3byte/pixel type    7:
         *  disable color space convert
         */
        uint32_t out_color_input_sel_chn:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dma2d_out_color_convert_chn_reg_t;

/** Type of out_scramble_chn register
 *  Configures the tx scramble of channel 0
 */
typedef union {
    struct {
        /** out_scramble_sel_pre_chn : R/W; bitpos: [2:0]; default: 0;
         *  Scramble data before color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 :
         *  BYTE1-0-2    3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
         */
        uint32_t out_scramble_sel_pre_chn:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} dma2d_out_scramble_chn_reg_t;

/** Type of out_etm_conf_chn register
 *  Configures the tx etm of channel 0
 */
typedef union {
    struct {
        /** out_etm_en_chn : R/W; bitpos: [0]; default: 0;
         *  Configures the enable of the etm function, 1 is enable.
         */
        uint32_t out_etm_en_chn:1;
        /** out_etm_loop_en_chn : R/W; bitpos: [1]; default: 0;
         *  Configures the enable of the descriptors loop etm function, 1 is enable.
         */
        uint32_t out_etm_loop_en_chn:1;
        /** out_dscr_task_mak_chn : R/W; bitpos: [3:2]; default: 1;
         *  Configures the maximum number of cacheable descriptors.
         */
        uint32_t out_dscr_task_mak_chn:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dma2d_out_etm_conf_chn_reg_t;

/** Type of out_dscr_port_blk_chn register
 *  Configures the tx block size in dscr port mode
 */
typedef union {
    struct {
        /** out_dscr_port_blk_h_chn : R/W; bitpos: [13:0]; default: 18;
         *  Set the horizontal width of tx block size in dscr port mode
         */
        uint32_t out_dscr_port_blk_h_chn:14;
        /** out_dscr_port_blk_v_chn : R/W; bitpos: [27:14]; default: 18;
         *  Set the vertical height of tx block size in dscr port mode
         */
        uint32_t out_dscr_port_blk_v_chn:14;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} dma2d_out_dscr_port_blk_chn_reg_t;


/** Group: in */
/** Type of in_conf0_chn register
 *  Configures the rx direction of channel 0
 */
typedef union {
    struct {
        /** in_mem_trans_en_chn : R/W; bitpos: [0]; default: 0;
         *  enable memory trans of the same channel
         */
        uint32_t in_mem_trans_en_chn:1;
        uint32_t reserved_1:1;
        /** indscr_burst_en_chn : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx transmitting link descriptor
         *  when accessing SRAM.
         */
        uint32_t indscr_burst_en_chn:1;
        /** in_ecc_aes_en_chn : R/W; bitpos: [3]; default: 0;
         *  When access address space is ecc/aes area, this bit should be set to 1. In this
         *  case, the start address of square should be 16-bit aligned. The width of square
         *  multiply byte number of one pixel should be 16-bit aligned.
         */
        uint32_t in_ecc_aes_en_chn:1;
        /** in_check_owner_chn : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_chn:1;
        /** in_loop_test_chn : R/W; bitpos: [5]; default: 0;
         *  reserved
         */
        uint32_t in_loop_test_chn:1;
        /** in_mem_burst_length_chn : R/W; bitpos: [8:6]; default: 0;
         *  Block size of Rx channel 0. 0: 8 bytes      1: 16 bytes      2: 32 bytes    3: 64
         *  bytes    4: 128 bytes
         */
        uint32_t in_mem_burst_length_chn:3;
        /** in_macro_block_size_chn : R/W; bitpos: [10:9]; default: 0;
         *  Sel RX macro-block size 0: 8pixel*8pixel     1: 8pixel*16pixel     2:
         *  16pixel*16pixel     3: no macro-block     , only useful in mode 1 of the link
         *  descriptor
         */
        uint32_t in_macro_block_size_chn:2;
        /** in_dscr_port_en_chn : R/W; bitpos: [11]; default: 0;
         *  Set this bit to 1 to obtain descriptor from IP port
         */
        uint32_t in_dscr_port_en_chn:1;
        /** in_page_bound_en_chn : R/W; bitpos: [12]; default: 0;
         *  Set this bit to 1 to make sure AXI write data don't cross the address boundary
         *  which define by mem_burst_length
         */
        uint32_t in_page_bound_en_chn:1;
        uint32_t reserved_13:3;
        /** in_reorder_en_chn : R/W; bitpos: [16]; default: 0;
         *  Enable RX channel 0 macro block reorder when set to 1, only channel0 have this
         *  selection
         */
        uint32_t in_reorder_en_chn:1;
        uint32_t reserved_17:7;
        /** in_rst_chn : R/W; bitpos: [24]; default: 0;
         *  Write 1 then write 0 to this bit to reset Rx channel
         */
        uint32_t in_rst_chn:1;
        /** in_cmd_disable_chn : R/W; bitpos: [25]; default: 0;
         *  Write 1 before reset and write 0 after reset
         */
        uint32_t in_cmd_disable_chn:1;
        /** in_arb_weight_opt_dis_chn : R/W; bitpos: [26]; default: 0;
         *  Set this bit to 1 to disable arbiter optimum weight function.
         */
        uint32_t in_arb_weight_opt_dis_chn:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} dma2d_in_conf0_chn_reg_t;

/** Type of in_int_raw_chn register
 *  Raw interrupt status of RX channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been transmitted to peripherals for Rx channel 0.
         */
        uint32_t in_done_chn_int_raw:1;
        /** in_suc_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and no data error is detected for Rx channel 0.
         */
        uint32_t in_suc_eof_chn_int_raw:1;
        /** in_err_eof_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received and data error is detected
         */
        uint32_t in_err_eof_chn_int_raw:1;
        /** in_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error,
         *  including owner error, the second and third word error of inlink descriptor for Rx
         *  channel 0.
         */
        uint32_t in_dscr_err_chn_int_raw:1;
        /** infifo_ovf_l1_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l1_chn_int_raw:1;
        /** infifo_udf_l1_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l1_chn_int_raw:1;
        /** infifo_ovf_l2_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l2_chn_int_raw:1;
        /** infifo_udf_l2_chn_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l2_chn_int_raw:1;
        /** infifo_ovf_l3_chn_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is overflow.
         */
        uint32_t infifo_ovf_l3_chn_int_raw:1;
        /** infifo_udf_l3_chn_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  The raw interrupt bit turns to high level when fifo of Rx channel is underflow.
         */
        uint32_t infifo_udf_l3_chn_int_raw:1;
        /** in_dscr_empty_chn_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  The raw interrupt bit turns to high level when the last descriptor is done but fifo
         *  also remain data.
         */
        uint32_t in_dscr_empty_chn_int_raw:1;
        /** infifo_ro_ovf_chn_int_raw : R/WTC/SS; bitpos: [11]; default: 0;
         *  The raw interrupt bit turns to high level when reorder fifo is overflow.
         */
        uint32_t infifo_ro_ovf_chn_int_raw:1;
        /** infifo_ro_udf_chn_int_raw : R/WTC/SS; bitpos: [12]; default: 0;
         *  The raw interrupt bit turns to high level when reorder fifo is underflow.
         */
        uint32_t infifo_ro_udf_chn_int_raw:1;
        /** in_dscr_task_ovf_chn_int_raw : R/WTC/SS; bitpos: [13]; default: 0;
         *  The raw interrupt bit turns to high level when dscr ready task fifo is overflow.
         */
        uint32_t in_dscr_task_ovf_chn_int_raw:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dma2d_in_int_raw_chn_reg_t;

/** Type of in_int_ena_chn register
 *  Interrupt enable bits of RX channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_chn_int_ena:1;
        /** in_suc_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_chn_int_ena:1;
        /** in_err_eof_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_chn_int_ena:1;
        /** in_dscr_err_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_chn_int_ena:1;
        /** infifo_ovf_l1_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_chn_int_ena:1;
        /** infifo_udf_l1_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_chn_int_ena:1;
        /** infifo_ovf_l2_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_chn_int_ena:1;
        /** infifo_udf_l2_chn_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_chn_int_ena:1;
        /** infifo_ovf_l3_chn_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l3_chn_int_ena:1;
        /** infifo_udf_l3_chn_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t infifo_udf_l3_chn_int_ena:1;
        /** in_dscr_empty_chn_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_chn_int_ena:1;
        /** infifo_ro_ovf_chn_int_ena : R/W; bitpos: [11]; default: 0;
         *  The interrupt enable bit for the INFIFO_RO_OVF_CH_INT interrupt.
         */
        uint32_t infifo_ro_ovf_chn_int_ena:1;
        /** infifo_ro_udf_chn_int_ena : R/W; bitpos: [12]; default: 0;
         *  The interrupt enable bit for the INFIFO_RO_UDF_CH_INT interrupt.
         */
        uint32_t infifo_ro_udf_chn_int_ena:1;
        /** in_dscr_task_ovf_chn_int_ena : R/W; bitpos: [13]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_chn_int_ena:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dma2d_in_int_ena_chn_reg_t;

/** Type of in_int_st_chn register
 *  Masked interrupt status of RX channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_chn_int_st:1;
        /** in_suc_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_chn_int_st:1;
        /** in_err_eof_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_chn_int_st:1;
        /** in_dscr_err_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_chn_int_st:1;
        /** infifo_ovf_l1_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_chn_int_st:1;
        /** infifo_udf_l1_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_chn_int_st:1;
        /** infifo_ovf_l2_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_chn_int_st:1;
        /** infifo_udf_l2_chn_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_chn_int_st:1;
        /** infifo_ovf_l3_chn_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l3_chn_int_st:1;
        /** infifo_udf_l3_chn_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t infifo_udf_l3_chn_int_st:1;
        /** in_dscr_empty_chn_int_st : RO; bitpos: [10]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_chn_int_st:1;
        /** infifo_ro_ovf_chn_int_st : RO; bitpos: [11]; default: 0;
         *  The raw interrupt status bit for the INFIFO_RO_OVF_CH_INT interrupt.
         */
        uint32_t infifo_ro_ovf_chn_int_st:1;
        /** infifo_ro_udf_chn_int_st : RO; bitpos: [12]; default: 0;
         *  The raw interrupt status bit for the INFIFO_RO_UDF_CH_INT interrupt.
         */
        uint32_t infifo_ro_udf_chn_int_st:1;
        /** in_dscr_task_ovf_chn_int_st : RO; bitpos: [13]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_chn_int_st:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dma2d_in_int_st_chn_reg_t;

/** Type of in_int_clr_chn register
 *  Interrupt clear bits of RX channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_chn_int_clr:1;
        /** in_suc_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_chn_int_clr:1;
        /** in_err_eof_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_chn_int_clr:1;
        /** in_dscr_err_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the INDSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_chn_int_clr:1;
        /** infifo_ovf_l1_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l1_chn_int_clr:1;
        /** infifo_udf_l1_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_l1_chn_int_clr:1;
        /** infifo_ovf_l2_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l2_chn_int_clr:1;
        /** infifo_udf_l2_chn_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_udf_l2_chn_int_clr:1;
        /** infifo_ovf_l3_chn_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t infifo_ovf_l3_chn_int_clr:1;
        /** infifo_udf_l3_chn_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t infifo_udf_l3_chn_int_clr:1;
        /** in_dscr_empty_chn_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_chn_int_clr:1;
        /** infifo_ro_ovf_chn_int_clr : WT; bitpos: [11]; default: 0;
         *  Set this bit to clear the INFIFO_RO_OVF_CH_INT interrupt.
         */
        uint32_t infifo_ro_ovf_chn_int_clr:1;
        /** infifo_ro_udf_chn_int_clr : WT; bitpos: [12]; default: 0;
         *  Set this bit to clear the INFIFO_RO_UDF_CH_INT interrupt.
         */
        uint32_t infifo_ro_udf_chn_int_clr:1;
        /** in_dscr_task_ovf_chn_int_clr : WT; bitpos: [13]; default: 0;
         *  Set this bit to clear the IN_DSCR_TASK_OVF_CH_INT interrupt.
         */
        uint32_t in_dscr_task_ovf_chn_int_clr:1;
        uint32_t reserved_14:18;
    };
    uint32_t val;
} dma2d_in_int_clr_chn_reg_t;

/** Type of infifo_status_chn register
 *  Represents the status of the rx fifo of channel 0
 */
typedef union {
    struct {
        /** infifo_full_l2_chn : RO; bitpos: [0]; default: 0;
         *  Rx FIFO full signal for Rx channel.
         */
        uint32_t infifo_full_l2_chn:1;
        /** infifo_empty_l2_chn : RO; bitpos: [1]; default: 1;
         *  Rx FIFO empty signal for Rx channel.
         */
        uint32_t infifo_empty_l2_chn:1;
        /** infifo_cnt_l2_chn : RO; bitpos: [5:2]; default: 0;
         *  The register stores the byte number of the data in Rx FIFO for Rx channel.
         */
        uint32_t infifo_cnt_l2_chn:4;
        uint32_t reserved_6:1;
        /** in_remain_under_1b_chn : RO; bitpos: [7]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_1b_chn:1;
        /** in_remain_under_2b_chn : RO; bitpos: [8]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_2b_chn:1;
        /** in_remain_under_3b_chn : RO; bitpos: [9]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_3b_chn:1;
        /** in_remain_under_4b_chn : RO; bitpos: [10]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_4b_chn:1;
        /** in_remain_under_5b_chn : RO; bitpos: [11]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_5b_chn:1;
        /** in_remain_under_6b_chn : RO; bitpos: [12]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_6b_chn:1;
        /** in_remain_under_7b_chn : RO; bitpos: [13]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_7b_chn:1;
        /** in_remain_under_8b_chn : RO; bitpos: [14]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_8b_chn:1;
        /** infifo_full_l1_chn : RO; bitpos: [15]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t infifo_full_l1_chn:1;
        /** infifo_empty_l1_chn : RO; bitpos: [16]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t infifo_empty_l1_chn:1;
        /** infifo_cnt_l1_chn : RO; bitpos: [21:17]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t infifo_cnt_l1_chn:5;
        /** infifo_full_l3_chn : RO; bitpos: [22]; default: 0;
         *  Tx FIFO full signal for Tx channel 0.
         */
        uint32_t infifo_full_l3_chn:1;
        /** infifo_empty_l3_chn : RO; bitpos: [23]; default: 1;
         *  Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t infifo_empty_l3_chn:1;
        /** infifo_cnt_l3_chn : RO; bitpos: [28:24]; default: 0;
         *  The register stores the byte number of the data in Tx FIFO for Tx channel 0.
         */
        uint32_t infifo_cnt_l3_chn:5;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} dma2d_infifo_status_chn_reg_t;

/** Type of in_pop_chn register
 *  Configures the rx fifo of channel 0
 */
typedef union {
    struct {
        /** infifo_rdata_chn : RO; bitpos: [10:0]; default: 1024;
         *  This register stores the data popping from DMA Rx FIFO.
         */
        uint32_t infifo_rdata_chn:11;
        /** infifo_pop_chn : R/W/SC; bitpos: [11]; default: 0;
         *  Set this bit to pop data from DMA Rx FIFO.
         */
        uint32_t infifo_pop_chn:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} dma2d_in_pop_chn_reg_t;

/** Type of in_link_conf_chn register
 *  Configures the rx descriptor operations of channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:20;
        /** inlink_auto_ret_chn : R/W; bitpos: [20]; default: 1;
         *  Configure the value of the owner field written back to the inlink descriptor.
         *  1: Write back 1. 0: Write back 0.
         */
        uint32_t inlink_auto_ret_chn:1;
        /** inlink_stop_chn : R/W/SC; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_chn:1;
        /** inlink_start_chn : R/W/SC; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_chn:1;
        /** inlink_restart_chn : R/W/SC; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_chn:1;
        /** inlink_park_chn : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_chn:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} dma2d_in_link_conf_chn_reg_t;

/** Type of in_link_addr_chn register
 *  Configures the rx descriptor address of channel 0
 */
typedef union {
    struct {
        /** inlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the first inlink descriptor's address.
         */
        uint32_t inlink_addr_chn:32;
    };
    uint32_t val;
} dma2d_in_link_addr_chn_reg_t;

/** Type of in_state_chn register
 *  Represents the working status of the rx descriptor of channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_chn:18;
        /** in_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t in_dscr_state_chn:2;
        /** in_state_chn : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t in_state_chn:3;
        /** in_reset_avail_chn : RO; bitpos: [23]; default: 1;
         *  This register indicate that if the channel reset is safety.
         */
        uint32_t in_reset_avail_chn:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} dma2d_in_state_chn_reg_t;

/** Type of in_suc_eof_des_addr_chn register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_chn:32;
    };
    uint32_t val;
} dma2d_in_suc_eof_des_addr_chn_reg_t;

/** Type of in_err_eof_des_addr_chn register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data.
         */
        uint32_t in_err_eof_des_addr_chn:32;
    };
    uint32_t val;
} dma2d_in_err_eof_des_addr_chn_reg_t;

/** Type of in_dscr_chn register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the next inlink descriptor address x.
         */
        uint32_t inlink_dscr_chn:32;
    };
    uint32_t val;
} dma2d_in_dscr_chn_reg_t;

/** Type of in_dscr_bf0_chn register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor's next address x-1.
         */
        uint32_t inlink_dscr_bf0_chn:32;
    };
    uint32_t val;
} dma2d_in_dscr_bf0_chn_reg_t;

/** Type of in_dscr_bf1_chn register
 *  Represents the address associated with the inlink descriptor of channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor's next address x-2.
         */
        uint32_t inlink_dscr_bf1_chn:32;
    };
    uint32_t val;
} dma2d_in_dscr_bf1_chn_reg_t;

/** Type of in_peri_sel_chn register
 *  Configures the rx peripheral of channel 0
 */
typedef union {
    struct {
        /** in_peri_sel_chn : R/W; bitpos: [2:0]; default: 7;
         *  This register is used to select peripheral for Rx channel   0:  jpeg     1:
         *  display-1     2: display-2        7: no choose
         */
        uint32_t in_peri_sel_chn:3;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} dma2d_in_peri_sel_chn_reg_t;

/** Type of in_arb_chn register
 *  Configures the rx arbiter of channel 0
 */
typedef union {
    struct {
        /** in_arb_token_num_chn : R/W; bitpos: [3:0]; default: 1;
         *  Set the max number of token count of arbiter
         */
        uint32_t in_arb_token_num_chn:4;
        /** in_arb_priority_chn : R/W; bitpos: [4]; default: 1;
         *  Set the priority of channel
         */
        uint32_t in_arb_priority_chn:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} dma2d_in_arb_chn_reg_t;

/** Type of in_ro_status_chn register
 *  Represents the status of the rx reorder module of channel 0
 */
typedef union {
    struct {
        /** infifo_ro_cnt_chn : RO; bitpos: [4:0]; default: 0;
         *  The register stores the byte number of the data in color convert Rx FIFO for
         *  channel 0.
         */
        uint32_t infifo_ro_cnt_chn:5;
        /** in_ro_wr_state_chn : RO; bitpos: [6:5]; default: 0;
         *  The register stores the state of read ram of reorder
         */
        uint32_t in_ro_wr_state_chn:2;
        /** in_ro_rd_state_chn : RO; bitpos: [8:7]; default: 0;
         *  The register stores the state of write ram of reorder
         */
        uint32_t in_ro_rd_state_chn:2;
        /** in_pixel_byte_chn : RO; bitpos: [12:9]; default: 0;
         *  the number of bytes contained in a pixel at RX channel     0: 1byte     1: 1.5bytes
         *  2 : 2bytes     3: 2.5bytes     4: 3bytes     5: 4bytes
         */
        uint32_t in_pixel_byte_chn:4;
        /** in_burst_block_num_chn : RO; bitpos: [16:13]; default: 0;
         *  the number of macro blocks contained in a burst of data at RX channel
         */
        uint32_t in_burst_block_num_chn:4;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dma2d_in_ro_status_chn_reg_t;

/** Type of in_ro_pd_conf_chn register
 *  Configures the rx reorder memory of channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:4;
        /** in_ro_ram_force_pd_chn : R/W; bitpos: [4]; default: 0;
         *  dma reorder ram power down
         */
        uint32_t in_ro_ram_force_pd_chn:1;
        /** in_ro_ram_force_pu_chn : R/W; bitpos: [5]; default: 1;
         *  dma reorder ram power up
         */
        uint32_t in_ro_ram_force_pu_chn:1;
        /** in_ro_ram_clk_fo_chn : R/W; bitpos: [6]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in DMA.
         *  0: A gate-clock will be used when accessing the RAM in DMA.
         */
        uint32_t in_ro_ram_clk_fo_chn:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} dma2d_in_ro_pd_conf_chn_reg_t;

/** Type of in_color_convert_chn register
 *  Configures the tx color convert of channel 0
 */
typedef union {
    struct {
        /** in_color_output_sel_chn : R/W; bitpos: [1:0]; default: 0;
         *  Set final color convert process and output type    0: RGB888 to RGB565     1:
         *  output directly
         */
        uint32_t in_color_output_sel_chn:2;
        /** in_color_3b_proc_en_chn : R/W; bitpos: [2]; default: 0;
         *  Enable generic color convert module between color input & color output, need to
         *  configure parameter.
         */
        uint32_t in_color_3b_proc_en_chn:1;
        /** in_color_input_sel_chn : R/W; bitpos: [5:3]; default: 7;
         *  Set first color convert process and input color type    0: YUV422/420 to YUV444
         *  1: YUV422      2: YUV444/420      7: disable color space convert
         */
        uint32_t in_color_input_sel_chn:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dma2d_in_color_convert_chn_reg_t;

/** Type of in_scramble_chn register
 *  Configures the rx scramble of channel 0
 */
typedef union {
    struct {
        /** in_scramble_sel_pre_chn : R/W; bitpos: [2:0]; default: 0;
         *  Scramble data before color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 :
         *  BYTE1-0-2    3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
         */
        uint32_t in_scramble_sel_pre_chn:3;
        /** in_scramble_sel_post_chn : R/W; bitpos: [5:3]; default: 0;
         *  Scramble data after color convert :  0 : BYTE2-1-0   1 : BYTE2-0-1    2 : BYTE1-0-2
         *  3 : BYTE1-2-0    4 : BYTE0-2-1    5 : BYTE0-1-2
         */
        uint32_t in_scramble_sel_post_chn:3;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} dma2d_in_scramble_chn_reg_t;

/** Type of in_etm_conf_chn register
 *  Configures the rx etm of channel 0
 */
typedef union {
    struct {
        /** in_etm_en_chn : R/W; bitpos: [0]; default: 0;
         *  Configures the enable of the etm function, 1 is enable.
         */
        uint32_t in_etm_en_chn:1;
        /** in_etm_loop_en_chn : R/W; bitpos: [1]; default: 0;
         *  Configures the enable of the descriptors loop etm function, 1 is enable.
         */
        uint32_t in_etm_loop_en_chn:1;
        /** in_dscr_task_mak_chn : R/W; bitpos: [3:2]; default: 1;
         *  Configures the maximum number of cacheable descriptors.
         */
        uint32_t in_dscr_task_mak_chn:2;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} dma2d_in_etm_conf_chn_reg_t;


/** Group: Status Registers */
/** Type of axi_err register
 *  Represents the status of th axi bus
 */
typedef union {
    struct {
        /** rid_err_cnt : RO; bitpos: [3:0]; default: 0;
         *  AXI read id err cnt
         */
        uint32_t rid_err_cnt:4;
        /** rresp_err_cnt : RO; bitpos: [7:4]; default: 0;
         *  AXI read resp err cnt
         */
        uint32_t rresp_err_cnt:4;
        /** wresp_err_cnt : RO; bitpos: [11:8]; default: 0;
         *  AXI write resp err cnt
         */
        uint32_t wresp_err_cnt:4;
        /** rd_fifo_cnt : RO; bitpos: [14:12]; default: 0;
         *  AXI read cmd fifo remain cmd count
         */
        uint32_t rd_fifo_cnt:3;
        /** rd_bak_fifo_cnt : RO; bitpos: [18:15]; default: 0;
         *  AXI read backup cmd fifo remain cmd count
         */
        uint32_t rd_bak_fifo_cnt:4;
        /** wr_fifo_cnt : RO; bitpos: [21:19]; default: 0;
         *  AXI write cmd fifo remain cmd count
         */
        uint32_t wr_fifo_cnt:3;
        /** wr_bak_fifo_cnt : RO; bitpos: [25:22]; default: 0;
         *  AXI write backup cmd fifo remain cmd count
         */
        uint32_t wr_bak_fifo_cnt:4;
        uint32_t reserved_26:6;
    };
    uint32_t val;
} dma2d_axi_err_reg_t;

/** Type of date register
 *  register version.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36716816;
         *  register version.
         */
        uint32_t date:32;
    };
    uint32_t val;
} dma2d_date_reg_t;


/** Group: Configuration Registers */
/** Type of rst_conf register
 *  Configures the reset of axi
 */
typedef union {
    struct {
        /** axim_rd_rst : R/W; bitpos: [0]; default: 0;
         *  Write 1 then write 0  to this bit to reset axi master read data FIFO.
         */
        uint32_t axim_rd_rst:1;
        /** axim_wr_rst : R/W; bitpos: [1]; default: 0;
         *  Write 1 then write 0  to this bit to reset axi master write data FIFO.
         */
        uint32_t axim_wr_rst:1;
        /** clk_en : R/W; bitpos: [2]; default: 0;
         *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} dma2d_rst_conf_reg_t;

/** Type of intr_mem_start_addr register
 *  The start address of accessible address space.
 */
typedef union {
    struct {
        /** access_intr_mem_start_addr : R/W; bitpos: [31:0]; default: 806354944 (0x30100000);
         *  The start address of accessible address space.
         */
        uint32_t access_intr_mem_start_addr:32;
    };
    uint32_t val;
} dma2d_intr_mem_start_addr_reg_t;

/** Type of intr_mem_end_addr register
 *  The end address of accessible address space.
 */
typedef union {
    struct {
        /** access_intr_mem_end_addr : R/W; bitpos: [31:0]; default: 2415919103 (0x8FFFFFFF);
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_intr_mem_end_addr:32;
    };
    uint32_t val;
} dma2d_intr_mem_end_addr_reg_t;

/** Type of extr_mem_start_addr register
 *  The start address of accessible address space.
 */
typedef union {
    struct {
        /** access_extr_mem_start_addr : R/W; bitpos: [31:0]; default: 806354944 (0x30100000);
         *  The start address of accessible address space.
         */
        uint32_t access_extr_mem_start_addr:32;
    };
    uint32_t val;
} dma2d_extr_mem_start_addr_reg_t;

/** Type of extr_mem_end_addr register
 *  The end address of accessible address space.
 */
typedef union {
    struct {
        /** access_extr_mem_end_addr : R/W; bitpos: [31:0]; default: 2415919103 (0x8FFFFFFF);
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_extr_mem_end_addr:32;
    };
    uint32_t val;
} dma2d_extr_mem_end_addr_reg_t;

/** Type of out_arb_config register
 *  Configures the tx arbiter
 */
typedef union {
    struct {
        /** out_arb_timeout_num : R/W; bitpos: [15:0]; default: 0;
         *  Set the max number of timeout count of arbiter
         */
        uint32_t out_arb_timeout_num:16;
        /** out_weight_en : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t out_weight_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dma2d_out_arb_config_reg_t;

/** Type of in_arb_config register
 *  Configures the rx arbiter
 */
typedef union {
    struct {
        /** in_arb_timeout_num : R/W; bitpos: [15:0]; default: 0;
         *  Set the max number of timeout count of arbiter
         */
        uint32_t in_arb_timeout_num:16;
        /** in_weight_en : R/W; bitpos: [16]; default: 0;
         *  reserved
         */
        uint32_t in_weight_en:1;
        uint32_t reserved_17:15;
    };
    uint32_t val;
} dma2d_in_arb_config_reg_t;

/** Type of rdn_result register
 *  reserved
 */
typedef union {
    struct {
        /** rdn_ena : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t rdn_ena:1;
        /** rdn_result : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t rdn_result:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} dma2d_rdn_result_reg_t;

/** Type of rdn_eco_high register
 *  reserved
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  The start address of accessible address space.
         */
        uint32_t rdn_eco_high:32;
    };
    uint32_t val;
} dma2d_rdn_eco_high_reg_t;

/** Type of rdn_eco_low register
 *  reserved
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  The start address of accessible address space.
         */
        uint32_t rdn_eco_low:32;
    };
    uint32_t val;
} dma2d_rdn_eco_low_reg_t;


/** Type of in/out_color_param_h/m/l_chn register
 *  Configures the rx/tx color convert parameter of channel n
 */
typedef union {
    struct {
        struct {
            /** a: R/W; bitpos: [9:0]; default: h:298, m:298, l:298
             * Set the first parameter of the most/medium/least significant byte of pending 3 bytes
             */
            uint32_t a                             :    10;
            /** b: R/W; bitpos: [20:10]; default: h:0, in_m:1948, l:516
             * Set the second parameter of the most/medium/least significant byte of pending 3 bytes
             */
            uint32_t b                             :    11;
            uint32_t reserved21                    :    11;
        };
        struct {
            /** c: R/W; bitpos: [41:32]; default: h:409, m:816, l:0
             * Set the third parameter of the most/medium/least significant byte of pending 3 bytes
             */
            uint32_t c                             :    10;
            /** d: R/W; bitpos: [59:42]; default: h:205238, m:34707, l:191308
             * Set the fourth parameter of the most/medium/least significant byte of pending 3 bytes
             */
            uint32_t d                             :    18;
            uint32_t reserved60                    :    4;
        };
    };
    uint32_t val[2];
} dma2d_color_param_reg_t;

typedef struct {
    volatile dma2d_color_param_reg_t param_h;
    volatile dma2d_color_param_reg_t param_m;
    volatile dma2d_color_param_reg_t param_l;
} dma2d_color_param_group_chn_reg_t;

typedef struct {
    volatile dma2d_out_conf0_chn_reg_t out_conf0;
    volatile dma2d_out_int_raw_chn_reg_t out_int_raw;
    volatile dma2d_out_int_ena_chn_reg_t out_int_ena;
    volatile dma2d_out_int_st_chn_reg_t out_int_st;
    volatile dma2d_out_int_clr_chn_reg_t out_int_clr;
    volatile dma2d_outfifo_status_chn_reg_t outfifo_status;
    volatile dma2d_out_push_chn_reg_t out_push;
    volatile dma2d_out_link_conf_chn_reg_t out_link_conf;
    volatile dma2d_out_link_addr_chn_reg_t out_link_addr;
    volatile dma2d_out_state_chn_reg_t out_state;
    volatile dma2d_out_eof_des_addr_chn_reg_t out_eof_des_addr;
    volatile dma2d_out_dscr_chn_reg_t out_dscr;
    volatile dma2d_out_dscr_bf0_chn_reg_t out_dscr_bf0;
    volatile dma2d_out_dscr_bf1_chn_reg_t out_dscr_bf1;
    volatile dma2d_out_peri_sel_chn_reg_t out_peri_sel;
    volatile dma2d_out_arb_chn_reg_t out_arb;
    volatile dma2d_out_ro_status_chn_reg_t out_ro_status;
    volatile dma2d_out_ro_pd_conf_chn_reg_t out_ro_pd_conf;  /* only exist on channel0 */
    volatile dma2d_out_color_convert_chn_reg_t out_color_convert;
    volatile dma2d_out_scramble_chn_reg_t out_scramble;
    volatile dma2d_color_param_group_chn_reg_t out_color_param_group;
    volatile dma2d_out_etm_conf_chn_reg_t out_etm_conf;
    volatile dma2d_out_dscr_port_blk_chn_reg_t out_dscr_port_blk;
    uint32_t reserved_out[36];
} dma2d_out_chn_reg_t;

typedef struct {
    volatile dma2d_in_conf0_chn_reg_t in_conf0;
    volatile dma2d_in_int_raw_chn_reg_t in_int_raw;
    volatile dma2d_in_int_ena_chn_reg_t in_int_ena;
    volatile dma2d_in_int_st_chn_reg_t in_int_st;
    volatile dma2d_in_int_clr_chn_reg_t in_int_clr;
    volatile dma2d_infifo_status_chn_reg_t infifo_status;
    volatile dma2d_in_pop_chn_reg_t in_pop;
    volatile dma2d_in_link_conf_chn_reg_t in_link_conf;
    volatile dma2d_in_link_addr_chn_reg_t in_link_addr;
    volatile dma2d_in_state_chn_reg_t in_state;
    volatile dma2d_in_suc_eof_des_addr_chn_reg_t in_suc_eof_des_addr;
    volatile dma2d_in_err_eof_des_addr_chn_reg_t in_err_eof_des_addr;
    volatile dma2d_in_dscr_chn_reg_t in_dscr;
    volatile dma2d_in_dscr_bf0_chn_reg_t in_dscr_bf0;
    volatile dma2d_in_dscr_bf1_chn_reg_t in_dscr_bf1;
    volatile dma2d_in_peri_sel_chn_reg_t in_peri_sel;
    volatile dma2d_in_arb_chn_reg_t in_arb;
    volatile dma2d_in_ro_status_chn_reg_t in_ro_status;
    volatile dma2d_in_ro_pd_conf_chn_reg_t in_ro_pd_conf;
    volatile dma2d_in_color_convert_chn_reg_t in_color_convert;
    volatile dma2d_in_scramble_chn_reg_t in_scramble;
    volatile dma2d_color_param_group_chn_reg_t in_color_param_group;
    volatile dma2d_in_etm_conf_chn_reg_t in_etm_conf;
    uint32_t reserved_570[36];
} dma2d_in_ch0_reg_t;

typedef struct {
    volatile dma2d_in_conf0_chn_reg_t in_conf0;
    volatile dma2d_in_int_raw_chn_reg_t in_int_raw;
    volatile dma2d_in_int_ena_chn_reg_t in_int_ena;
    volatile dma2d_in_int_st_chn_reg_t in_int_st;
    volatile dma2d_in_int_clr_chn_reg_t in_int_clr;
    volatile dma2d_infifo_status_chn_reg_t infifo_status;
    volatile dma2d_in_pop_chn_reg_t in_pop;
    volatile dma2d_in_link_conf_chn_reg_t in_link_conf;
    volatile dma2d_in_link_addr_chn_reg_t in_link_addr;
    volatile dma2d_in_state_chn_reg_t in_state;
    volatile dma2d_in_suc_eof_des_addr_chn_reg_t in_suc_eof_des_addr;
    volatile dma2d_in_err_eof_des_addr_chn_reg_t in_err_eof_des_addr;
    volatile dma2d_in_dscr_chn_reg_t in_dscr;
    volatile dma2d_in_dscr_bf0_chn_reg_t in_dscr_bf0;
    volatile dma2d_in_dscr_bf1_chn_reg_t in_dscr_bf1;
    volatile dma2d_in_peri_sel_chn_reg_t in_peri_sel;
    volatile dma2d_in_arb_chn_reg_t in_arb;
    volatile dma2d_in_ro_status_chn_reg_t in_ro_status;
    volatile dma2d_in_etm_conf_chn_reg_t in_etm_conf;
    uint32_t reserved_64c[45];
} dma2d_in_ch1_reg_t;

typedef struct dma2d_dev_t {
    volatile dma2d_out_chn_reg_t out_channel[3];
    uint32_t reserved_300[128];
    volatile dma2d_in_ch0_reg_t in_channel0;
    volatile dma2d_in_ch1_reg_t in_channel1;
    uint32_t reserved_700[192];
    volatile dma2d_axi_err_reg_t axi_err;
    volatile dma2d_rst_conf_reg_t rst_conf;
    volatile dma2d_intr_mem_start_addr_reg_t intr_mem_start_addr;
    volatile dma2d_intr_mem_end_addr_reg_t intr_mem_end_addr;
    volatile dma2d_extr_mem_start_addr_reg_t extr_mem_start_addr;
    volatile dma2d_extr_mem_end_addr_reg_t extr_mem_end_addr;
    volatile dma2d_out_arb_config_reg_t out_arb_config;
    volatile dma2d_in_arb_config_reg_t in_arb_config;
    volatile dma2d_rdn_result_reg_t rdn_result;
    volatile dma2d_rdn_eco_high_reg_t rdn_eco_high;
    volatile dma2d_rdn_eco_low_reg_t rdn_eco_low;
    volatile dma2d_date_reg_t date;
} dma2d_dev_t;

extern dma2d_dev_t DMA2D;

#ifndef __cplusplus
_Static_assert(sizeof(dma2d_dev_t) == 0xa30, "Invalid size of dma2d_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
