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

/** Group: in */
/** Type of in_int_raw_chn register
 *  Raw status interrupt of channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received for Rx channel 0.
         */
        uint32_t in_done_chn_int_raw: 1;
        /** in_suc_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
         *  turns to high level when the last data pointed by one inlink descriptor has been
         *  received and no data error is detected for Rx channel 0.
         */
        uint32_t in_suc_eof_chn_int_raw: 1;
        /** in_err_eof_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when data error is detected only in the
         *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
         *  interrupt is reserved.
         */
        uint32_t in_err_eof_chn_int_raw: 1;
        /** in_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error
         *  including owner error and the second and third word error of inlink descriptor for
         *  Rx channel 0.
         */
        uint32_t in_dscr_err_chn_int_raw: 1;
        /** in_dscr_empty_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
         *  and receiving data is not completed but there is no more inlink for Rx channel 0.
         */
        uint32_t in_dscr_empty_chn_int_raw: 1;
        /** infifo_l1_ovf_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  overflow.
         */
        uint32_t infifo_l1_ovf_chn_int_raw: 1;
        /** infifo_l1_udf_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  underflow.
         */
        uint32_t infifo_l1_udf_chn_int_raw: 1;
        /** infifo_l2_ovf_chn_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  overflow.
         */
        uint32_t infifo_l2_ovf_chn_int_raw: 1;
        /** infifo_l2_udf_chn_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  underflow.
         */
        uint32_t infifo_l2_udf_chn_int_raw: 1;
        /** infifo_l3_ovf_chn_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  overflow.
         */
        uint32_t infifo_l3_ovf_chn_int_raw: 1;
        /** infifo_l3_udf_chn_int_raw : R/WTC/SS; bitpos: [10]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  underflow.
         */
        uint32_t infifo_l3_udf_chn_int_raw: 1;
        uint32_t reserved_11: 21;
    };
    uint32_t val;
} axi_dma_in_int_raw_chn_reg_t;

/** Type of in_int_st_chn register
 *  Masked interrupt of channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_chn_int_st: 1;
        /** in_suc_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_chn_int_st: 1;
        /** in_err_eof_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_chn_int_st: 1;
        /** in_dscr_err_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_chn_int_st: 1;
        /** in_dscr_empty_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_chn_int_st: 1;
        /** infifo_l1_ovf_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_l1_ovf_chn_int_st: 1;
        /** infifo_l1_udf_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_l1_udf_chn_int_st: 1;
        /** infifo_l2_ovf_chn_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_l2_ovf_chn_int_st: 1;
        /** infifo_l2_udf_chn_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_l2_udf_chn_int_st: 1;
        /** infifo_l3_ovf_chn_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t infifo_l3_ovf_chn_int_st: 1;
        /** infifo_l3_udf_chn_int_st : RO; bitpos: [10]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t infifo_l3_udf_chn_int_st: 1;
        uint32_t reserved_11: 21;
    };
    uint32_t val;
} axi_dma_in_int_st_chn_reg_t;

/** Type of in_int_ena_chn register
 *  Interrupt enable bits of channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_chn_int_ena: 1;
        /** in_suc_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_chn_int_ena: 1;
        /** in_err_eof_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_chn_int_ena: 1;
        /** in_dscr_err_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_chn_int_ena: 1;
        /** in_dscr_empty_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_chn_int_ena: 1;
        /** infifo_l1_ovf_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_l1_ovf_chn_int_ena: 1;
        /** infifo_l1_udf_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_l1_udf_chn_int_ena: 1;
        /** infifo_l2_ovf_chn_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_l2_ovf_chn_int_ena: 1;
        /** infifo_l2_udf_chn_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_l2_udf_chn_int_ena: 1;
        /** infifo_l3_ovf_chn_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t infifo_l3_ovf_chn_int_ena: 1;
        /** infifo_l3_udf_chn_int_ena : R/W; bitpos: [10]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t infifo_l3_udf_chn_int_ena: 1;
        uint32_t reserved_11: 21;
    };
    uint32_t val;
} axi_dma_in_int_ena_chn_reg_t;

/** Type of in_int_clr_chn register
 *  Interrupt clear bits of channel 0
 */
typedef union {
    struct {
        /** in_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_chn_int_clr: 1;
        /** in_suc_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_chn_int_clr: 1;
        /** in_err_eof_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_chn_int_clr: 1;
        /** in_dscr_err_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_chn_int_clr: 1;
        /** in_dscr_empty_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_chn_int_clr: 1;
        /** infifo_l1_ovf_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_l1_ovf_chn_int_clr: 1;
        /** infifo_l1_udf_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_l1_udf_chn_int_clr: 1;
        /** infifo_l2_ovf_chn_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t infifo_l2_ovf_chn_int_clr: 1;
        /** infifo_l2_udf_chn_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t infifo_l2_udf_chn_int_clr: 1;
        /** infifo_l3_ovf_chn_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t infifo_l3_ovf_chn_int_clr: 1;
        /** infifo_l3_udf_chn_int_clr : WT; bitpos: [10]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t infifo_l3_udf_chn_int_clr: 1;
        uint32_t reserved_11: 21;
    };
    uint32_t val;
} axi_dma_in_int_clr_chn_reg_t;

/** Type of in_conf0_chn register
 *  Configure 0 register of Rx channel 0
 */
typedef union {
    struct {
        /** in_rst_chn : R/W; bitpos: [0]; default: 0;
         *  This bit is used to reset AXI_DMA channel 0 Rx FSM and Rx FIFO pointer.
         */
        uint32_t in_rst_chn: 1;
        /** in_loop_test_chn : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t in_loop_test_chn: 1;
        /** mem_trans_en_chn : R/W; bitpos: [2]; default: 0;
         *  Set this bit 1 to enable automatic transmitting data from memory to memory via
         *  AXI_DMA.
         */
        uint32_t mem_trans_en_chn: 1;
        /** in_etm_en_chn : R/W; bitpos: [3]; default: 0;
         *  Set this bit to 1 to enable etm control mode, dma Rx channel 0 is triggered by etm
         *  task.
         */
        uint32_t in_etm_en_chn: 1;
        /** in_burst_size_sel_chn : R/W; bitpos: [6:4]; default: 0;
         *  3'b000-3'b100:burst length 8byte~128byte
         */
        uint32_t in_burst_size_sel_chn: 3;
        /** in_cmd_disable_chn : R/W; bitpos: [7]; default: 0;
         *  1:mean disable cmd of this ch0
         */
        uint32_t in_cmd_disable_chn: 1;
        /** in_ecc_aes_en_chn : R/W; bitpos: [8]; default: 0;
         *  1: mean access ecc or aes domain,0: mean not
         */
        uint32_t in_ecc_aes_en_chn: 1;
        /** indscr_burst_en_chn : R/W; bitpos: [9]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t indscr_burst_en_chn: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_in_conf0_chn_reg_t;

/** Type of in_conf1_chn register
 *  Configure 1 register of Rx channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0: 12;
        /** in_check_owner_chn : R/W; bitpos: [12]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner_chn: 1;
        uint32_t reserved_13: 19;
    };
    uint32_t val;
} axi_dma_in_conf1_chn_reg_t;

/** Type of infifo_status_chn register
 *  Receive FIFO status of Rx channel 0
 */
typedef union {
    struct {
        /** infifo_l3_full_chn : RO; bitpos: [0]; default: 1;
         *  L3 Rx FIFO full signal for Rx channel 0.
         */
        uint32_t infifo_l3_full_chn: 1;
        /** infifo_l3_empty_chn : RO; bitpos: [1]; default: 1;
         *  L3 Rx FIFO empty signal for Rx channel 0.
         */
        uint32_t infifo_l3_empty_chn: 1;
        /** infifo_l3_cnt_chn : RO; bitpos: [7:2]; default: 0;
         *  The register stores the byte number of the data in L3 Rx FIFO for Rx channel 0.
         */
        uint32_t infifo_l3_cnt_chn: 6;
        /** infifo_l3_udf_chn : RO; bitpos: [8]; default: 0;
         *  L3 Rx FIFO under flow signal for Rx channel 0.
         */
        uint32_t infifo_l3_udf_chn: 1;
        /** infifo_l3_ovf_chn : RO; bitpos: [9]; default: 0;
         *  L3 Rx FIFO over flow signal for Rx channel 0.
         */
        uint32_t infifo_l3_ovf_chn: 1;
        /** infifo_l1_full_chn : RO; bitpos: [10]; default: 0;
         *  L1 Rx FIFO full signal for Rx channel 0.
         */
        uint32_t infifo_l1_full_chn: 1;
        /** infifo_l1_empty_chn : RO; bitpos: [11]; default: 1;
         *  L1 Rx FIFO empty signal for Rx channel 0.
         */
        uint32_t infifo_l1_empty_chn: 1;
        /** infifo_l1_udf_chn : RO; bitpos: [12]; default: 0;
         *  L1 Rx FIFO under flow signal for Rx channel 0.
         */
        uint32_t infifo_l1_udf_chn: 1;
        /** infifo_l1_ovf_chn : RO; bitpos: [13]; default: 0;
         *  L1 Rx FIFO over flow signal for Rx channel 0.
         */
        uint32_t infifo_l1_ovf_chn: 1;
        /** infifo_l2_full_chn : RO; bitpos: [14]; default: 0;
         *  L2 Rx RAM full signal for Rx channel 0.
         */
        uint32_t infifo_l2_full_chn: 1;
        /** infifo_l2_empty_chn : RO; bitpos: [15]; default: 1;
         *  L2 Rx RAM empty signal for Rx channel 0.
         */
        uint32_t infifo_l2_empty_chn: 1;
        /** infifo_l2_udf_chn : RO; bitpos: [16]; default: 0;
         *  L2 Rx FIFO under flow signal for Rx channel 0.
         */
        uint32_t infifo_l2_udf_chn: 1;
        /** infifo_l2_ovf_chn : RO; bitpos: [17]; default: 0;
         *  L2 Rx FIFO over flow signal for Rx channel 0.
         */
        uint32_t infifo_l2_ovf_chn: 1;
        uint32_t reserved_18: 5;
        /** in_remain_under_1b_chn : RO; bitpos: [23]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_1b_chn: 1;
        /** in_remain_under_2b_chn : RO; bitpos: [24]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_2b_chn: 1;
        /** in_remain_under_3b_chn : RO; bitpos: [25]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_3b_chn: 1;
        /** in_remain_under_4b_chn : RO; bitpos: [26]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_4b_chn: 1;
        /** in_remain_under_5b_chn : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_5b_chn: 1;
        /** in_remain_under_6b_chn : RO; bitpos: [28]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_6b_chn: 1;
        /** in_remain_under_7b_chn : RO; bitpos: [29]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_7b_chn: 1;
        /** in_remain_under_8b_chn : RO; bitpos: [30]; default: 0;
         *  reserved
         */
        uint32_t in_remain_under_8b_chn: 1;
        /** in_buf_hungry_chn : RO; bitpos: [31]; default: 0;
         *  reserved
         */
        uint32_t in_buf_hungry_chn: 1;
    };
    uint32_t val;
} axi_dma_infifo_status_chn_reg_t;

/** Type of in_pop_chn register
 *  Pop control register of Rx channel 0
 */
typedef union {
    struct {
        /** infifo_rdata_chn : RO; bitpos: [11:0]; default: 2048;
         *  This register stores the data popping from AXI_DMA FIFO.
         */
        uint32_t infifo_rdata_chn: 12;
        /** infifo_pop_chn : WT; bitpos: [12]; default: 0;
         *  Set this bit to pop data from AXI_DMA FIFO.
         */
        uint32_t infifo_pop_chn: 1;
        uint32_t reserved_13: 19;
    };
    uint32_t val;
} axi_dma_in_pop_chn_reg_t;

/** Type of in_link1_chn register
 *  Link descriptor configure and control register of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_auto_ret_chn : R/W; bitpos: [0]; default: 1;
         *  Set this bit to return to current inlink descriptor's address when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret_chn: 1;
        /** inlink_stop_chn : WT; bitpos: [1]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop_chn: 1;
        /** inlink_start_chn : WT; bitpos: [2]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start_chn: 1;
        /** inlink_restart_chn : WT; bitpos: [3]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart_chn: 1;
        /** inlink_park_chn : RO; bitpos: [4]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park_chn: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} axi_dma_in_link1_chn_reg_t;

/** Type of in_link2_chn register
 *  Link descriptor configure and control register of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the 20 least significant bits of the first inlink descriptor's
         *  address.
         */
        uint32_t inlink_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_in_link2_chn_reg_t;

/** Type of in_state_chn register
 *  Receive status of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr_chn: 18;
        /** in_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t in_dscr_state_chn: 2;
        /** in_state_chn : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t in_state_chn: 3;
        uint32_t reserved_23: 9;
    };
    uint32_t val;
} axi_dma_in_state_chn_reg_t;

/** Type of in_suc_eof_des_addr_chn register
 *  Inlink descriptor address when EOF occurs of Rx channel 0
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_in_suc_eof_des_addr_chn_reg_t;

/** Type of in_err_eof_des_addr_chn register
 *  Inlink descriptor address when errors occur of Rx channel 0
 */
typedef union {
    struct {
        /** in_err_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data. Only used when peripheral is UHCI0.
         */
        uint32_t in_err_eof_des_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_in_err_eof_des_addr_chn_reg_t;

/** Type of in_dscr_chn register
 *  Current inlink descriptor address of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the current inlink descriptor x.
         */
        uint32_t inlink_dscr_chn: 32;
    };
    uint32_t val;
} axi_dma_in_dscr_chn_reg_t;

/** Type of in_dscr_bf0_chn register
 *  The last inlink descriptor address of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor x-1.
         */
        uint32_t inlink_dscr_bf0_chn: 32;
    };
    uint32_t val;
} axi_dma_in_dscr_bf0_chn_reg_t;

/** Type of in_dscr_bf1_chn register
 *  The second-to-last inlink descriptor address of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor x-2.
         */
        uint32_t inlink_dscr_bf1_chn: 32;
    };
    uint32_t val;
} axi_dma_in_dscr_bf1_chn_reg_t;

/** Type of in_pri_chn register
 *  Priority register of Rx channel 0
 */
typedef union {
    struct {
        /** rx_pri_chn : R/W; bitpos: [3:0]; default: 0;
         *  The priority of Rx channel 0. The larger of the value the higher of the priority.
         */
        uint32_t rx_pri_chn: 4;
        /** rx_ch_arb_weigh_chn : R/W; bitpos: [7:4]; default: 0;
         *  The weight of Rx channel 0
         */
        uint32_t rx_ch_arb_weigh_chn: 4;
        /** rx_arb_weigh_opt_dir_chn : R/W; bitpos: [8]; default: 0;
         *  0: mean not optimization weight function ,1: mean optimization
         */
        uint32_t rx_arb_weigh_opt_dir_chn: 1;
        uint32_t reserved_9: 23;
    };
    uint32_t val;
} axi_dma_in_pri_chn_reg_t;

/** Type of in_peri_sel_chn register
 *  Peripheral selection of Rx channel 0
 */
typedef union {
    struct {
        /** peri_in_sel_chn : R/W; bitpos: [5:0]; default: 63;
         *  This register is used to select peripheral for Rx channel 0. 0:lcdcam. 1: gpspi_2.
         *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
         */
        uint32_t peri_in_sel_chn: 6;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} axi_dma_in_peri_sel_chn_reg_t;

/** Type of in_crc_init_data_chn register
 *  This register is used to config ch0 crc initial data(max 32 bit)
 */
typedef union {
    struct {
        /** in_crc_init_data_chn : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is used to config ch0 of rx crc initial value
         */
        uint32_t in_crc_init_data_chn: 32;
    };
    uint32_t val;
} axi_dma_in_crc_init_data_chn_reg_t;

/** Type of rx_crc_width_chn register
 *  This register is used to config rx ch0 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
typedef union {
    struct {
        /** rx_crc_width_chn : R/W; bitpos: [1:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_width_chn: 2;
        /** rx_crc_latch_flag_chn : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_latch_flag_chn: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} axi_dma_rx_crc_width_chn_reg_t;

/** Type of in_crc_clear_chn register
 *  This register is used to clear ch0 crc result
 */
typedef union {
    struct {
        /** in_crc_clear_chn_reg : R/W; bitpos: [0]; default: 0;
         *  This register is used to clear ch0 of rx crc result
         */
        uint32_t in_crc_clear_chn_reg: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} axi_dma_in_crc_clear_chn_reg_t;

/** Type of in_crc_final_result_chn register
 *  This register is used to store ch0 crc result
 */
typedef union {
    struct {
        /** in_crc_final_result_chn : RO; bitpos: [31:0]; default: 0;
         *  This register is used to store result ch0 of rx
         */
        uint32_t in_crc_final_result_chn: 32;
    };
    uint32_t val;
} axi_dma_in_crc_final_result_chn_reg_t;

/** Type of rx_crc_en_wr_data_chn register
 *  This resister is used to config ch0 crc en for every bit
 */
typedef union {
    struct {
        /** rx_crc_en_wr_data_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register is used to enable rx ch0 crc 32bit on/off
         */
        uint32_t rx_crc_en_wr_data_chn: 32;
    };
    uint32_t val;
} axi_dma_rx_crc_en_wr_data_chn_reg_t;

/** Type of rx_crc_en_addr_chn register
 *  This register is used to config ch0 crc en addr
 */
typedef union {
    struct {
        /** rx_crc_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_en_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_rx_crc_en_addr_chn_reg_t;

/** Type of rx_crc_data_en_wr_data_chn register
 *  This register is used to config crc data_8bit en
 */
typedef union {
    struct {
        /** rx_crc_data_en_wr_data_chn : R/W; bitpos: [15:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_data_en_wr_data_chn: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} axi_dma_rx_crc_data_en_wr_data_chn_reg_t;

/** Type of rx_crc_data_en_addr_chn register
 *  This register is used to config addr of crc data_8bit en
 */
typedef union {
    struct {
        /** rx_crc_data_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t rx_crc_data_en_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_rx_crc_data_en_addr_chn_reg_t;

/** Group: out */
/** Type of out_int_raw_chn register
 *  Raw status interrupt of channel0
 */
typedef union {
    struct {
        /** out_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel0.
         */
        uint32_t out_done_chn_int_raw: 1;
        /** out_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel0.
         */
        uint32_t out_eof_chn_int_raw: 1;
        /** out_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error
         *  including owner error and the second and third word error of outlink descriptor for
         *  Tx channel0.
         */
        uint32_t out_dscr_err_chn_int_raw: 1;
        /** out_total_eof_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel0.
         */
        uint32_t out_total_eof_chn_int_raw: 1;
        /** outfifo_l1_ovf_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
         *  overflow.
         */
        uint32_t outfifo_l1_ovf_chn_int_raw: 1;
        /** outfifo_l1_udf_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
         *  underflow.
         */
        uint32_t outfifo_l1_udf_chn_int_raw: 1;
        /** outfifo_l2_ovf_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
         *  overflow.
         */
        uint32_t outfifo_l2_ovf_chn_int_raw: 1;
        /** outfifo_l2_udf_chn_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
         *  underflow.
         */
        uint32_t outfifo_l2_udf_chn_int_raw: 1;
        /** outfifo_l3_ovf_chn_int_raw : R/WTC/SS; bitpos: [8]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
         *  overflow.
         */
        uint32_t outfifo_l3_ovf_chn_int_raw: 1;
        /** outfifo_l3_udf_chn_int_raw : R/WTC/SS; bitpos: [9]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel0 is
         *  underflow.
         */
        uint32_t outfifo_l3_udf_chn_int_raw: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_out_int_raw_chn_reg_t;

/** Type of out_int_st_chn register
 *  Masked interrupt of channel0
 */
typedef union {
    struct {
        /** out_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_chn_int_st: 1;
        /** out_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_chn_int_st: 1;
        /** out_dscr_err_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_chn_int_st: 1;
        /** out_total_eof_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_chn_int_st: 1;
        /** outfifo_l1_ovf_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_l1_ovf_chn_int_st: 1;
        /** outfifo_l1_udf_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_l1_udf_chn_int_st: 1;
        /** outfifo_l2_ovf_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_l2_ovf_chn_int_st: 1;
        /** outfifo_l2_udf_chn_int_st : RO; bitpos: [7]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_l2_udf_chn_int_st: 1;
        /** outfifo_l3_ovf_chn_int_st : RO; bitpos: [8]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_l3_ovf_chn_int_st: 1;
        /** outfifo_l3_udf_chn_int_st : RO; bitpos: [9]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_l3_udf_chn_int_st: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_out_int_st_chn_reg_t;

/** Type of out_int_ena_chn register
 *  Interrupt enable bits of channel0
 */
typedef union {
    struct {
        /** out_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_chn_int_ena: 1;
        /** out_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_chn_int_ena: 1;
        /** out_dscr_err_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_chn_int_ena: 1;
        /** out_total_eof_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_chn_int_ena: 1;
        /** outfifo_l1_ovf_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_l1_ovf_chn_int_ena: 1;
        /** outfifo_l1_udf_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_l1_udf_chn_int_ena: 1;
        /** outfifo_l2_ovf_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_l2_ovf_chn_int_ena: 1;
        /** outfifo_l2_udf_chn_int_ena : R/W; bitpos: [7]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_l2_udf_chn_int_ena: 1;
        /** outfifo_l3_ovf_chn_int_ena : R/W; bitpos: [8]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_l3_ovf_chn_int_ena: 1;
        /** outfifo_l3_udf_chn_int_ena : R/W; bitpos: [9]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_l3_udf_chn_int_ena: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_out_int_ena_chn_reg_t;

/** Type of out_int_clr_chn register
 *  Interrupt clear bits of channel0
 */
typedef union {
    struct {
        /** out_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_chn_int_clr: 1;
        /** out_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_chn_int_clr: 1;
        /** out_dscr_err_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_chn_int_clr: 1;
        /** out_total_eof_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_chn_int_clr: 1;
        /** outfifo_l1_ovf_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_l1_ovf_chn_int_clr: 1;
        /** outfifo_l1_udf_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_l1_udf_chn_int_clr: 1;
        /** outfifo_l2_ovf_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_l2_ovf_chn_int_clr: 1;
        /** outfifo_l2_udf_chn_int_clr : WT; bitpos: [7]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L2_CH_INT interrupt.
         */
        uint32_t outfifo_l2_udf_chn_int_clr: 1;
        /** outfifo_l3_ovf_chn_int_clr : WT; bitpos: [8]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_l3_ovf_chn_int_clr: 1;
        /** outfifo_l3_udf_chn_int_clr : WT; bitpos: [9]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L3_CH_INT interrupt.
         */
        uint32_t outfifo_l3_udf_chn_int_clr: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_out_int_clr_chn_reg_t;

/** Type of out_conf0_chn register
 *  Configure 0 register of Tx channelN
 */
typedef union {
    struct {
        /** out_rst_chn : R/W; bitpos: [0]; default: 0;
         *  This bit is used to reset AXI_DMA channel0 Tx FSM and Tx FIFO pointer.
         */
        uint32_t out_rst_chn: 1;
        /** out_loop_test_chn : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t out_loop_test_chn: 1;
        /** out_auto_wrback_chn : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
         *  has been transmitted.
         */
        uint32_t out_auto_wrback_chn: 1;
        /** out_eof_mode_chn : R/W; bitpos: [3]; default: 1;
         *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel0 is
         *  generated when data need to transmit has been popped from FIFO in AXI_DMA
         */
        uint32_t out_eof_mode_chn: 1;
        /** out_etm_en_chn : R/W; bitpos: [4]; default: 0;
         *  Set this bit to 1 to enable etm control mode, dma Tx channel0 is triggered by etm
         *  task.
         */
        uint32_t out_etm_en_chn: 1;
        /** out_burst_size_sel_chn : R/W; bitpos: [7:5]; default: 0;
         *  3'b000-3'b100:burst length 8byte~128byte
         */
        uint32_t out_burst_size_sel_chn: 3;
        /** out_cmd_disable_chn : R/W; bitpos: [8]; default: 0;
         *  1:mean disable cmd of this chn
         */
        uint32_t out_cmd_disable_chn: 1;
        /** out_ecc_aes_en_chn : R/W; bitpos: [9]; default: 0;
         *  1: mean access ecc or aes domain,0: mean not
         */
        uint32_t out_ecc_aes_en_chn: 1;
        /** outdscr_burst_en_chn : R/W; bitpos: [10]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en_chn: 1;
        uint32_t reserved_11: 21;
    };
    uint32_t val;
} axi_dma_out_conf0_chn_reg_t;

/** Type of out_conf1_chn register
 *  Configure 1 register of Tx channel0
 */
typedef union {
    struct {
        uint32_t reserved_0: 12;
        /** out_check_owner_chn : R/W; bitpos: [12]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner_chn: 1;
        uint32_t reserved_13: 19;
    };
    uint32_t val;
} axi_dma_out_conf1_chn_reg_t;

/** Type of outfifo_status_chn register
 *  Transmit FIFO status of Tx channel0
 */
typedef union {
    struct {
        /** outfifo_l3_full_chn : RO; bitpos: [0]; default: 0;
         *  L3 Tx FIFO full signal for Tx channel0.
         */
        uint32_t outfifo_l3_full_chn: 1;
        /** outfifo_l3_empty_chn : RO; bitpos: [1]; default: 1;
         *  L3 Tx FIFO empty signal for Tx channel0.
         */
        uint32_t outfifo_l3_empty_chn: 1;
        /** outfifo_l3_cnt_chn : RO; bitpos: [7:2]; default: 0;
         *  The register stores the byte number of the data in L3 Tx FIFO for Tx channel0.
         */
        uint32_t outfifo_l3_cnt_chn: 6;
        /** outfifo_l3_udf_chn : RO; bitpos: [8]; default: 0;
         *  L3 Tx FIFO under flow signal for Tx channel0.
         */
        uint32_t outfifo_l3_udf_chn: 1;
        /** outfifo_l3_ovf_chn : RO; bitpos: [9]; default: 0;
         *  L3 Tx FIFO over flow signal for Tx channel0.
         */
        uint32_t outfifo_l3_ovf_chn: 1;
        /** outfifo_l1_full_chn : RO; bitpos: [10]; default: 0;
         *  L1 Tx FIFO full signal for Tx channel0.
         */
        uint32_t outfifo_l1_full_chn: 1;
        /** outfifo_l1_empty_chn : RO; bitpos: [11]; default: 1;
         *  L1 Tx FIFO empty signal for Tx channel0.
         */
        uint32_t outfifo_l1_empty_chn: 1;
        /** outfifo_l1_udf_chn : RO; bitpos: [12]; default: 0;
         *  L1 Tx FIFO under flow signal for Tx channel0.
         */
        uint32_t outfifo_l1_udf_chn: 1;
        /** outfifo_l1_ovf_chn : RO; bitpos: [13]; default: 0;
         *  L1 Tx FIFO over flow signal for Tx channel0.
         */
        uint32_t outfifo_l1_ovf_chn: 1;
        /** outfifo_l2_full_chn : RO; bitpos: [14]; default: 0;
         *  L2 Tx RAM full signal for Tx channel0.
         */
        uint32_t outfifo_l2_full_chn: 1;
        /** outfifo_l2_empty_chn : RO; bitpos: [15]; default: 1;
         *  L2 Tx RAM empty signal for Tx channel0.
         */
        uint32_t outfifo_l2_empty_chn: 1;
        /** outfifo_l2_udf_chn : RO; bitpos: [16]; default: 0;
         *  L2 Tx FIFO under flow signal for Tx channel0.
         */
        uint32_t outfifo_l2_udf_chn: 1;
        /** outfifo_l2_ovf_chn : RO; bitpos: [17]; default: 0;
         *  L2 Tx FIFO over flow signal for Tx channel0.
         */
        uint32_t outfifo_l2_ovf_chn: 1;
        uint32_t reserved_18: 5;
        /** out_remain_under_1b_chn : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_1b_chn: 1;
        /** out_remain_under_2b_chn : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_2b_chn: 1;
        /** out_remain_under_3b_chn : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_3b_chn: 1;
        /** out_remain_under_4b_chn : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_4b_chn: 1;
        /** out_remain_under_5b_chn : RO; bitpos: [27]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_5b_chn: 1;
        /** out_remain_under_6b_chn : RO; bitpos: [28]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_6b_chn: 1;
        /** out_remain_under_7b_chn : RO; bitpos: [29]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_7b_chn: 1;
        /** out_remain_under_8b_chn : RO; bitpos: [30]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_8b_chn: 1;
        uint32_t reserved_31: 1;
    };
    uint32_t val;
} axi_dma_outfifo_status_chn_reg_t;

/** Type of out_push_chn register
 *  Push control register of Tx channel0
 */
typedef union {
    struct {
        /** outfifo_wdata_chn : R/W; bitpos: [8:0]; default: 0;
         *  This register stores the data that need to be pushed into AXI_DMA FIFO.
         */
        uint32_t outfifo_wdata_chn: 9;
        /** outfifo_push_chn : WT; bitpos: [9]; default: 0;
         *  Set this bit to push data into AXI_DMA FIFO.
         */
        uint32_t outfifo_push_chn: 1;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_out_push_chn_reg_t;

/** Type of out_link1_chn register
 *  Link descriptor configure and control register of Tx channel0
 */
typedef union {
    struct {
        /** outlink_stop_chn : WT; bitpos: [0]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop_chn: 1;
        /** outlink_start_chn : WT; bitpos: [1]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start_chn: 1;
        /** outlink_restart_chn : WT; bitpos: [2]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart_chn: 1;
        /** outlink_park_chn : RO; bitpos: [3]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park_chn: 1;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} axi_dma_out_link1_chn_reg_t;

/** Type of out_link2_chn register
 *  Link descriptor configure and control register of Tx channel0
 */
typedef union {
    struct {
        /** outlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register stores the 32 least significant bits of the first outlink
         *  descriptor's address.
         */
        uint32_t outlink_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_out_link2_chn_reg_t;

/** Type of out_state_chn register
 *  Transmit status of Tx channel0
 */
typedef union {
    struct {
        /** outlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr_chn: 18;
        /** out_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t out_dscr_state_chn: 2;
        /** out_state_chn : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t out_state_chn: 3;
        uint32_t reserved_23: 9;
    };
    uint32_t val;
} axi_dma_out_state_chn_reg_t;

/** Type of out_eof_des_addr_chn register
 *  Outlink descriptor address when EOF occurs of Tx channel0
 */
typedef union {
    struct {
        /** out_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_out_eof_des_addr_chn_reg_t;

/** Type of out_eof_bfr_des_addr_chn register
 *  The last outlink descriptor address when EOF occurs of Tx channel0
 */
typedef union {
    struct {
        /** out_eof_bfr_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor before the last outlink
         *  descriptor.
         */
        uint32_t out_eof_bfr_des_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_out_eof_bfr_des_addr_chn_reg_t;

/** Type of out_dscr_chn register
 *  Current outlink descriptor address of Tx channel0
 */
typedef union {
    struct {
        /** outlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the current outlink descriptor y.
         */
        uint32_t outlink_dscr_chn: 32;
    };
    uint32_t val;
} axi_dma_out_dscr_chn_reg_t;

/** Type of out_dscr_bf0_chn register
 *  The last outlink descriptor address of Tx channel0
 */
typedef union {
    struct {
        /** outlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor y-1.
         */
        uint32_t outlink_dscr_bf0_chn: 32;
    };
    uint32_t val;
} axi_dma_out_dscr_bf0_chn_reg_t;

/** Type of out_dscr_bf1_chn register
 *  The second-to-last outlink descriptor address of Tx channel0
 */
typedef union {
    struct {
        /** outlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last outlink descriptor x-2.
         */
        uint32_t outlink_dscr_bf1_chn: 32;
    };
    uint32_t val;
} axi_dma_out_dscr_bf1_chn_reg_t;

/** Type of out_pri_chn register
 *  Priority register of Tx channel0.
 */
typedef union {
    struct {
        /** tx_pri_chn : R/W; bitpos: [3:0]; default: 0;
         *  The priority of Tx channel0. The larger of the value the higher of the priority.
         */
        uint32_t tx_pri_chn: 4;
        /** tx_ch_arb_weigh_chn : R/W; bitpos: [7:4]; default: 0;
         *  The weight of Tx channel0
         */
        uint32_t tx_ch_arb_weigh_chn: 4;
        /** tx_arb_weigh_opt_dir_chn : R/W; bitpos: [8]; default: 0;
         *  0: mean not optimization weight function ,1: mean optimization
         */
        uint32_t tx_arb_weigh_opt_dir_chn: 1;
        uint32_t reserved_9: 23;
    };
    uint32_t val;
} axi_dma_out_pri_chn_reg_t;

/** Type of out_peri_sel_chn register
 *  Peripheral selection of Tx channel0
 */
typedef union {
    struct {
        /** peri_out_sel_chn : R/W; bitpos: [5:0]; default: 63;
         *  This register is used to select peripheral for Tx channel0. 0:lcdcam. 1: gpspi_2.
         *  2: gpspi_3. 3: parl_io. 4: aes. 5: sha. 6~15: Dummy
         */
        uint32_t peri_out_sel_chn: 6;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} axi_dma_out_peri_sel_chn_reg_t;

/** Type of out_crc_init_data_chn register
 *  This register is used to config ch0 crc initial data(max 32 bit)
 */
typedef union {
    struct {
        /** out_crc_init_data_chn : R/W; bitpos: [31:0]; default: 4294967295;
         *  This register is used to config ch0 of tx crc initial value
         */
        uint32_t out_crc_init_data_chn: 32;
    };
    uint32_t val;
} axi_dma_out_crc_init_data_chn_reg_t;

/** Type of tx_crc_width_chn register
 *  This register is used to config tx ch0 crc result width,2'b00 mean crc_width
 *  <=8bit,2'b01 8<crc_width<=16 ,2'b10 mean 16<crc_width  <=24,2'b11 mean
 *  24<crc_width<=32
 */
typedef union {
    struct {
        /** tx_crc_width_chn : R/W; bitpos: [1:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_width_chn: 2;
        /** tx_crc_latch_flag_chn : R/W; bitpos: [2]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_latch_flag_chn: 1;
        uint32_t reserved_3: 29;
    };
    uint32_t val;
} axi_dma_tx_crc_width_chn_reg_t;

/** Type of out_crc_clear_chn register
 *  This register is used to clear ch0 crc result
 */
typedef union {
    struct {
        /** out_crc_clear_chn_reg : R/W; bitpos: [0]; default: 0;
         *  This register is used to clear ch0 of tx crc result
         */
        uint32_t out_crc_clear_chn_reg: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} axi_dma_out_crc_clear_chn_reg_t;

/** Type of out_crc_final_result_chn register
 *  This register is used to store ch0 crc result
 */
typedef union {
    struct {
        /** out_crc_final_result_chn : RO; bitpos: [31:0]; default: 0;
         *  This register is used to store result ch0 of tx
         */
        uint32_t out_crc_final_result_chn: 32;
    };
    uint32_t val;
} axi_dma_out_crc_final_result_chn_reg_t;

/** Type of tx_crc_en_wr_data_chn register
 *  This resister is used to config ch0 crc en for every bit
 */
typedef union {
    struct {
        /** tx_crc_en_wr_data_chn : R/W; bitpos: [31:0]; default: 0;
         *  This register is used to enable tx ch0 crc 32bit on/off
         */
        uint32_t tx_crc_en_wr_data_chn: 32;
    };
    uint32_t val;
} axi_dma_tx_crc_en_wr_data_chn_reg_t;

/** Type of tx_crc_en_addr_chn register
 *  This register is used to config ch0 crc en addr
 */
typedef union {
    struct {
        /** tx_crc_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_en_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_tx_crc_en_addr_chn_reg_t;

/** Type of tx_crc_data_en_wr_data_chn register
 *  This register is used to config crc data_8bit en
 */
typedef union {
    struct {
        /** tx_crc_data_en_wr_data_chn : R/W; bitpos: [15:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_data_en_wr_data_chn: 16;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} axi_dma_tx_crc_data_en_wr_data_chn_reg_t;

/** Type of tx_crc_data_en_addr_chn register
 *  This register is used to config addr of crc data_8bit en
 */
typedef union {
    struct {
        /** tx_crc_data_en_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  reserved
         */
        uint32_t tx_crc_data_en_addr_chn: 32;
    };
    uint32_t val;
} axi_dma_tx_crc_data_en_addr_chn_reg_t;

/** Group: Configuration Registers */
/** Type of arb_timeout register
 *  This retister is used to config arbiter time slice
 */
typedef union {
    struct {
        /** arb_timeout_tx : R/W; bitpos: [15:0]; default: 0;
         *  This register is used to config tx arbiter time out value
         */
        uint32_t arb_timeout_tx: 16;
        /** arb_timeout_rx : R/W; bitpos: [31:16]; default: 0;
         *  This register is used to config rx arbiter time out value
         */
        uint32_t arb_timeout_rx: 16;
    };
    uint32_t val;
} axi_dma_arb_timeout_reg_t;

/** Type of weight_en register
 *  This register is used to config arbiter weight function to on or off
 */
typedef union {
    struct {
        /** weight_en_tx : R/W; bitpos: [0]; default: 0;
         *  This register is used to config tx arbiter weight function  off/on
         */
        uint32_t weight_en_tx: 1;
        /** weight_en_rx : R/W; bitpos: [1]; default: 0;
         *  This register is used to config rx arbiter weight function  off/on
         */
        uint32_t weight_en_rx: 1;
        uint32_t reserved_2: 30;
    };
    uint32_t val;
} axi_dma_weight_en_reg_t;

/** Type of in_mem_conf register
 *  Mem power configure register of Rx channel
 */
typedef union {
    struct {
        /** in_mem_clk_force_en : R/W; bitpos: [0]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in
         *  AXI_DMA. 0: A gate-clock will be used when accessing the RAM in AXI_DMA.
         */
        uint32_t in_mem_clk_force_en: 1;
        /** in_mem_force_pu : R/W; bitpos: [1]; default: 0;
         *  Force power up ram
         */
        uint32_t in_mem_force_pu: 1;
        /** in_mem_force_pd : R/W; bitpos: [2]; default: 0;
         *  Force power down ram
         */
        uint32_t in_mem_force_pd: 1;
        /** out_mem_clk_force_en : R/W; bitpos: [3]; default: 0;
         *  1: Force to open the clock and bypass the gate-clock when accessing the RAM in
         *  AXI_DMA. 0: A gate-clock will be used when accessing the RAM in AXI_DMA.
         */
        uint32_t out_mem_clk_force_en: 1;
        /** out_mem_force_pu : R/W; bitpos: [4]; default: 0;
         *  Force power up ram
         */
        uint32_t out_mem_force_pu: 1;
        /** out_mem_force_pd : R/W; bitpos: [5]; default: 0;
         *  Force power down ram
         */
        uint32_t out_mem_force_pd: 1;
        uint32_t reserved_6: 26;
    };
    uint32_t val;
} axi_dma_in_mem_conf_reg_t;

/** Type of intr_mem_start_addr register
 *  The start address of accessible address space.
 */
typedef union {
    struct {
        /** access_intr_mem_start_addr : R/W; bitpos: [31:0]; default: 806354944;
         *  The start address of accessible address space.
         */
        uint32_t access_intr_mem_start_addr: 32;
    };
    uint32_t val;
} axi_dma_intr_mem_start_addr_reg_t;

/** Type of intr_mem_end_addr register
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
typedef union {
    struct {
        /** access_intr_mem_end_addr : R/W; bitpos: [31:0]; default: 2415919103;
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_intr_mem_end_addr: 32;
    };
    uint32_t val;
} axi_dma_intr_mem_end_addr_reg_t;

/** Type of extr_mem_start_addr register
 *  The start address of accessible address space.
 */
typedef union {
    struct {
        /** access_extr_mem_start_addr : R/W; bitpos: [31:0]; default: 806354944;
         *  The start address of accessible address space.
         */
        uint32_t access_extr_mem_start_addr: 32;
    };
    uint32_t val;
} axi_dma_extr_mem_start_addr_reg_t;

/** Type of extr_mem_end_addr register
 *  The end address of accessible address space. The access address beyond this range
 *  would lead to descriptor error.
 */
typedef union {
    struct {
        /** access_extr_mem_end_addr : R/W; bitpos: [31:0]; default: 2415919103;
         *  The end address of accessible address space. The access address beyond this range
         *  would lead to descriptor error.
         */
        uint32_t access_extr_mem_end_addr: 32;
    };
    uint32_t val;
} axi_dma_extr_mem_end_addr_reg_t;

/** Type of in_reset_avail_chn register
 *  The rx channel 0 reset valid_flag register.
 */
typedef union {
    struct {
        /** in_reset_avail_chn : RO; bitpos: [0]; default: 1;
         *  rx chan0 reset valid reg.
         */
        uint32_t in_reset_avail_chn: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} axi_dma_in_reset_avail_chn_reg_t;

/** Type of out_reset_avail_chn register
 *  The tx channel 0 reset valid_flag register.
 */
typedef union {
    struct {
        /** out_reset_avail_chn : RO; bitpos: [0]; default: 1;
         *  tx chan0 reset valid reg.
         */
        uint32_t out_reset_avail_chn: 1;
        uint32_t reserved_1: 31;
    };
    uint32_t val;
} axi_dma_out_reset_avail_chn_reg_t;

/** Type of misc_conf register
 *  MISC register
 */
typedef union {
    struct {
        /** axim_rst_wr_inter : R/W; bitpos: [0]; default: 0;
         *  Set this bit then clear this bit to reset the internal axi_wr FSM.
         */
        uint32_t axim_rst_wr_inter: 1;
        /** axim_rst_rd_inter : R/W; bitpos: [1]; default: 0;
         *  Set this bit then clear this bit to reset the internal axi_rd FSM.
         */
        uint32_t axim_rst_rd_inter: 1;
        uint32_t reserved_2: 1;
        /** arb_pri_dis : R/W; bitpos: [3]; default: 0;
         *  Set this bit to disable priority arbitration function.
         */
        uint32_t arb_pri_dis: 1;
        /** clk_en : R/W; bitpos: [4]; default: 0;
         *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en: 1;
        uint32_t reserved_5: 27;
    };
    uint32_t val;
} axi_dma_misc_conf_reg_t;

/** Group: Debug Registers */
/** Type of rdn_result register
 *  reserved
 */
typedef union {
    struct {
        /** rdn_ena : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t rdn_ena: 1;
        /** rdn_result : RO; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t rdn_result: 1;
        uint32_t reserved_2: 30;
    };
    uint32_t val;
} axi_dma_rdn_result_reg_t;

/** Type of rdn_eco_high register
 *  reserved
 */
typedef union {
    struct {
        /** rdn_eco_high : R/W; bitpos: [31:0]; default: 4294967295;
         *  The start address of accessible address space.
         */
        uint32_t rdn_eco_high: 32;
    };
    uint32_t val;
} axi_dma_rdn_eco_high_reg_t;

/** Type of rdn_eco_low register
 *  reserved
 */
typedef union {
    struct {
        /** rdn_eco_low : R/W; bitpos: [31:0]; default: 0;
         *  The start address of accessible address space.
         */
        uint32_t rdn_eco_low: 32;
    };
    uint32_t val;
} axi_dma_rdn_eco_low_reg_t;

/** Group: Status Registers */
/** Type of wresp_cnt register
 *  AXI wr response cnt register.
 */
typedef union {
    struct {
        /** wresp_cnt : RO; bitpos: [3:0]; default: 0;
         *  axi wr response cnt reg.
         */
        uint32_t wresp_cnt: 4;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} axi_dma_wresp_cnt_reg_t;

/** Type of rresp_cnt register
 *  AXI wr response cnt register.
 */
typedef union {
    struct {
        /** rresp_cnt : RO; bitpos: [3:0]; default: 0;
         *  axi rd response cnt reg.
         */
        uint32_t rresp_cnt: 4;
        uint32_t reserved_4: 28;
    };
    uint32_t val;
} axi_dma_rresp_cnt_reg_t;

/** Type of infifo_status1_chn register
 *  Receive FIFO status of Rx channel 0
 */
typedef union {
    struct {
        /** l1infifo_cnt_chn : RO; bitpos: [5:0]; default: 0;
         *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.
         */
        uint32_t l1infifo_cnt_chn: 6;
        /** l2infifo_cnt_chn : RO; bitpos: [9:6]; default: 0;
         *  The register stores the byte number of the data in L2 Rx FIFO for Rx channel 0.
         */
        uint32_t l2infifo_cnt_chn: 4;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_infifo_status1_chn_reg_t;

/** Type of outfifo_status1_chn register
 *  Receive FIFO status of Tx channel 0
 */
typedef union {
    struct {
        /** l1outfifo_cnt_chn : RO; bitpos: [5:0]; default: 0;
         *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.
         */
        uint32_t l1outfifo_cnt_chn: 6;
        /** l2outfifo_cnt_chn : RO; bitpos: [9:6]; default: 0;
         *  The register stores the byte number of the data in L2 Tx FIFO for Tx channel 0.
         */
        uint32_t l2outfifo_cnt_chn: 4;
        uint32_t reserved_10: 22;
    };
    uint32_t val;
} axi_dma_outfifo_status1_chn_reg_t;

/** Group: Version Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 36712768;
         *  register version.
         */
        uint32_t date: 32;
    };
    uint32_t val;
} axi_dma_date_reg_t;

typedef struct {
    volatile axi_dma_in_int_raw_chn_reg_t raw;
    volatile axi_dma_in_int_st_chn_reg_t st;
    volatile axi_dma_in_int_ena_chn_reg_t ena;
    volatile axi_dma_in_int_clr_chn_reg_t clr;
} axi_dma_in_int_chn_reg_t;

typedef struct {
    volatile axi_dma_in_conf0_chn_reg_t in_conf0;
    volatile axi_dma_in_conf1_chn_reg_t in_conf1;
    volatile axi_dma_infifo_status_chn_reg_t infifo_status;
    volatile axi_dma_in_pop_chn_reg_t in_pop;
    volatile axi_dma_in_link1_chn_reg_t in_link1;
    volatile axi_dma_in_link2_chn_reg_t in_link2;
    volatile axi_dma_in_state_chn_reg_t in_state;
    volatile axi_dma_in_suc_eof_des_addr_chn_reg_t in_suc_eof_des_addr;
    volatile axi_dma_in_err_eof_des_addr_chn_reg_t in_err_eof_des_addr;
    volatile axi_dma_in_dscr_chn_reg_t in_dscr;
    volatile axi_dma_in_dscr_bf0_chn_reg_t in_dscr_bf0;
    volatile axi_dma_in_dscr_bf1_chn_reg_t in_dscr_bf1;
    volatile axi_dma_in_pri_chn_reg_t in_pri;
    volatile axi_dma_in_peri_sel_chn_reg_t in_peri_sel;
} axi_dma_in_conf_chn_reg_t;

typedef struct {
    volatile axi_dma_in_crc_init_data_chn_reg_t in_crc_init_data;
    volatile axi_dma_rx_crc_width_chn_reg_t rx_crc_width;
    volatile axi_dma_in_crc_clear_chn_reg_t in_crc_clear;
    volatile axi_dma_in_crc_final_result_chn_reg_t in_crc_final_result;
    volatile axi_dma_rx_crc_en_wr_data_chn_reg_t rx_crc_en_wr_data;
    volatile axi_dma_rx_crc_en_addr_chn_reg_t rx_crc_en_addr;
    volatile axi_dma_rx_crc_data_en_wr_data_chn_reg_t rx_crc_data_en_wr_data;
    volatile axi_dma_rx_crc_data_en_addr_chn_reg_t rx_crc_data_en_addr;
} axi_dma_in_crc_chn_reg_t;

typedef struct {
    volatile axi_dma_in_int_chn_reg_t intr;
    volatile axi_dma_in_conf_chn_reg_t conf;
    volatile axi_dma_in_crc_chn_reg_t crc;
} axi_dma_in_reg_t;

typedef struct {
    volatile axi_dma_out_int_raw_chn_reg_t raw;
    volatile axi_dma_out_int_st_chn_reg_t st;
    volatile axi_dma_out_int_ena_chn_reg_t ena;
    volatile axi_dma_out_int_clr_chn_reg_t clr;
} axi_dma_out_int_chn_reg_t;

typedef struct {
    volatile axi_dma_out_conf0_chn_reg_t out_conf0;
    volatile axi_dma_out_conf1_chn_reg_t out_conf1;
    volatile axi_dma_outfifo_status_chn_reg_t outfifo_status;
    volatile axi_dma_out_push_chn_reg_t out_push;
    volatile axi_dma_out_link1_chn_reg_t out_link1;
    volatile axi_dma_out_link2_chn_reg_t out_link2;
    volatile axi_dma_out_state_chn_reg_t out_state;
    volatile axi_dma_out_eof_des_addr_chn_reg_t out_eof_des_addr;
    volatile axi_dma_out_eof_bfr_des_addr_chn_reg_t out_eof_bfr_des_addr;
    volatile axi_dma_out_dscr_chn_reg_t out_dscr;
    volatile axi_dma_out_dscr_bf0_chn_reg_t out_dscr_bf0;
    volatile axi_dma_out_dscr_bf1_chn_reg_t out_dscr_bf1;
    volatile axi_dma_out_pri_chn_reg_t out_pri;
    volatile axi_dma_out_peri_sel_chn_reg_t out_peri_sel;
} axi_dma_out_conf_chn_reg_t;

typedef struct {
    volatile axi_dma_out_crc_init_data_chn_reg_t out_crc_init_data;
    volatile axi_dma_tx_crc_width_chn_reg_t tx_crc_width;
    volatile axi_dma_out_crc_clear_chn_reg_t out_crc_clear;
    volatile axi_dma_out_crc_final_result_chn_reg_t out_crc_final_result;
    volatile axi_dma_tx_crc_en_wr_data_chn_reg_t tx_crc_en_wr_data;
    volatile axi_dma_tx_crc_en_addr_chn_reg_t tx_crc_en_addr;
    volatile axi_dma_tx_crc_data_en_wr_data_chn_reg_t tx_crc_data_en_wr_data;
    volatile axi_dma_tx_crc_data_en_addr_chn_reg_t tx_crc_data_en_addr;
} axi_dma_out_crc_chn_reg_t;

typedef struct {
    volatile axi_dma_out_int_chn_reg_t intr;
    volatile axi_dma_out_conf_chn_reg_t conf;
    volatile axi_dma_out_crc_chn_reg_t crc;
} axi_dma_out_reg_t;

typedef struct {
    volatile axi_dma_in_reg_t in[3];
    volatile axi_dma_out_reg_t out[3];
    volatile axi_dma_arb_timeout_reg_t arb_timeout;
    volatile axi_dma_weight_en_reg_t weight_en;
    volatile axi_dma_in_mem_conf_reg_t in_mem_conf;
    volatile axi_dma_intr_mem_start_addr_reg_t intr_mem_start_addr;
    volatile axi_dma_intr_mem_end_addr_reg_t intr_mem_end_addr;
    volatile axi_dma_extr_mem_start_addr_reg_t extr_mem_start_addr;
    volatile axi_dma_extr_mem_end_addr_reg_t extr_mem_end_addr;
    volatile axi_dma_in_reset_avail_chn_reg_t in_reset_avail_chn[3];
    volatile axi_dma_out_reset_avail_chn_reg_t out_reset_avail_chn[3];
    uint32_t reserved_2a4;
    volatile axi_dma_misc_conf_reg_t misc_conf;
    volatile axi_dma_rdn_result_reg_t rdn_result;
    volatile axi_dma_rdn_eco_high_reg_t rdn_eco_high;
    volatile axi_dma_rdn_eco_low_reg_t rdn_eco_low;
    volatile axi_dma_wresp_cnt_reg_t wresp_cnt;
    volatile axi_dma_rresp_cnt_reg_t rresp_cnt;
    volatile axi_dma_infifo_status1_chn_reg_t infifo_status1_chn[3];
    volatile axi_dma_outfifo_status1_chn_reg_t outfifo_status1_chn[3];
    volatile axi_dma_date_reg_t date;
} axi_dma_dev_t;

extern axi_dma_dev_t AXI_DMA;

#ifndef __cplusplus
_Static_assert(sizeof(axi_dma_dev_t) == 0x2dc, "Invalid size of axi_dma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
