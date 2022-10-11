/**
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt Registers */
/** Type of in_int_raw_chn register
 *  Raw status interrupt of channel 0
 */
typedef union {
    struct {
        /** in_done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received for Rx channel 0.
         */
        uint32_t in_done_int_raw:1;
        /** in_suc_eof_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one inlink
         *  descriptor has been received for Rx channel 0. For UHCI0 the raw interrupt bit
         *  turns to high level when the last data pointed by one inlink descriptor has been
         *  received and no data error is detected for Rx channel 0.
         */
        uint32_t in_suc_eof_int_raw:1;
        /** in_err_eof_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when data error is detected only in the
         *  case that the peripheral is UHCI0 for Rx channel 0. For other peripherals this raw
         *  interrupt is reserved.
         */
        uint32_t in_err_eof_int_raw:1;
        /** in_dscr_err_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when detecting inlink descriptor error
         *  including owner error and the second and third word error of inlink descriptor for
         *  Rx channel 0.
         */
        uint32_t in_dscr_err_int_raw:1;
        /** in_dscr_empty_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt bit turns to high level when Rx buffer pointed by inlink is full
         *  and receiving data is not completed but there is no more inlink for Rx channel 0.
         */
        uint32_t in_dscr_empty_int_raw:1;
        /** infifo_ovf_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  overflow.
         */
        uint32_t infifo_ovf_int_raw:1;
        /** infifo_udf_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Rx channel 0 is
         *  underflow.
         */
        uint32_t infifo_udf_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gdma_in_int_raw_chn_reg_t;

/** Type of in_int_st_chn register
 *  Masked interrupt of channel 0
 */
typedef union {
    struct {
        /** in_done_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_int_st:1;
        /** in_suc_eof_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_int_st:1;
        /** in_err_eof_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_int_st:1;
        /** in_dscr_err_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_int_st:1;
        /** in_dscr_empty_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_int_st:1;
        /** infifo_ovf_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_int_st:1;
        /** infifo_udf_int_st : RO; bitpos: [6]; default: 0;
         *  The raw interrupt status bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gdma_in_int_st_chn_reg_t;

/** Type of in_int_ena_chn register
 *  Interrupt enable bits of channel 0
 */
typedef union {
    struct {
        /** in_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_int_ena:1;
        /** in_suc_eof_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_int_ena:1;
        /** in_err_eof_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_int_ena:1;
        /** in_dscr_err_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_int_ena:1;
        /** in_dscr_empty_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_int_ena:1;
        /** infifo_ovf_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_int_ena:1;
        /** infifo_udf_int_ena : R/W; bitpos: [6]; default: 0;
         *  The interrupt enable bit for the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gdma_in_int_ena_chn_reg_t;

/** Type of in_int_clr_chn register
 *  Interrupt clear bits of channel 0
 */
typedef union {
    struct {
        /** in_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the IN_DONE_CH_INT interrupt.
         */
        uint32_t in_done_int_clr:1;
        /** in_suc_eof_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the IN_SUC_EOF_CH_INT interrupt.
         */
        uint32_t in_suc_eof_int_clr:1;
        /** in_err_eof_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the IN_ERR_EOF_CH_INT interrupt.
         */
        uint32_t in_err_eof_int_clr:1;
        /** in_dscr_err_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the IN_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t in_dscr_err_int_clr:1;
        /** in_dscr_empty_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the IN_DSCR_EMPTY_CH_INT interrupt.
         */
        uint32_t in_dscr_empty_int_clr:1;
        /** infifo_ovf_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the INFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t infifo_ovf_int_clr:1;
        /** infifo_udf_int_clr : WT; bitpos: [6]; default: 0;
         *  Set this bit to clear the INFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t infifo_udf_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gdma_in_int_clr_chn_reg_t;

/** Type of out_int_raw_chn register
 *  Raw status interrupt of channel 0
 */
typedef union {
    struct {
        /** out_done_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been transmitted to peripherals for Tx channel 0.
         */
        uint32_t out_done_int_raw:1;
        /** out_eof_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt bit turns to high level when the last data pointed by one outlink
         *  descriptor has been read from memory for Tx channel 0.
         */
        uint32_t out_eof_int_raw:1;
        /** out_dscr_err_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt bit turns to high level when detecting outlink descriptor error
         *  including owner error and the second and third word error of outlink descriptor for
         *  Tx channel 0.
         */
        uint32_t out_dscr_err_int_raw:1;
        /** out_total_eof_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt bit turns to high level when data corresponding a outlink
         *  (includes one link descriptor or few link descriptors) is transmitted out for Tx
         *  channel 0.
         */
        uint32_t out_total_eof_int_raw:1;
        /** outfifo_ovf_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
         *  overflow.
         */
        uint32_t outfifo_ovf_int_raw:1;
        /** outfifo_udf_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  This raw interrupt bit turns to high level when level 1 fifo of Tx channel 0 is
         *  underflow.
         */
        uint32_t outfifo_udf_int_raw:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_out_int_raw_chn_reg_t;

/** Type of out_int_st_chn register
 *  Masked interrupt of channel 0
 */
typedef union {
    struct {
        /** out_done_int_st : RO; bitpos: [0]; default: 0;
         *  The raw interrupt status bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_int_st:1;
        /** out_eof_int_st : RO; bitpos: [1]; default: 0;
         *  The raw interrupt status bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_int_st:1;
        /** out_dscr_err_int_st : RO; bitpos: [2]; default: 0;
         *  The raw interrupt status bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_int_st:1;
        /** out_total_eof_int_st : RO; bitpos: [3]; default: 0;
         *  The raw interrupt status bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_int_st:1;
        /** outfifo_ovf_int_st : RO; bitpos: [4]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_int_st:1;
        /** outfifo_udf_int_st : RO; bitpos: [5]; default: 0;
         *  The raw interrupt status bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_int_st:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_out_int_st_chn_reg_t;

/** Type of out_int_ena_chn register
 *  Interrupt enable bits of channel 0
 */
typedef union {
    struct {
        /** out_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  The interrupt enable bit for the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_int_ena:1;
        /** out_eof_int_ena : R/W; bitpos: [1]; default: 0;
         *  The interrupt enable bit for the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_int_ena:1;
        /** out_dscr_err_int_ena : R/W; bitpos: [2]; default: 0;
         *  The interrupt enable bit for the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_int_ena:1;
        /** out_total_eof_int_ena : R/W; bitpos: [3]; default: 0;
         *  The interrupt enable bit for the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_int_ena:1;
        /** outfifo_ovf_int_ena : R/W; bitpos: [4]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_int_ena:1;
        /** outfifo_udf_int_ena : R/W; bitpos: [5]; default: 0;
         *  The interrupt enable bit for the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_int_ena:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_out_int_ena_chn_reg_t;

/** Type of out_int_clr_chn register
 *  Interrupt clear bits of channel 0
 */
typedef union {
    struct {
        /** out_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the OUT_DONE_CH_INT interrupt.
         */
        uint32_t out_done_int_clr:1;
        /** out_eof_int_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear the OUT_EOF_CH_INT interrupt.
         */
        uint32_t out_eof_int_clr:1;
        /** out_dscr_err_int_clr : WT; bitpos: [2]; default: 0;
         *  Set this bit to clear the OUT_DSCR_ERR_CH_INT interrupt.
         */
        uint32_t out_dscr_err_int_clr:1;
        /** out_total_eof_int_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear the OUT_TOTAL_EOF_CH_INT interrupt.
         */
        uint32_t out_total_eof_int_clr:1;
        /** outfifo_ovf_int_clr : WT; bitpos: [4]; default: 0;
         *  Set this bit to clear the OUTFIFO_OVF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_ovf_int_clr:1;
        /** outfifo_udf_int_clr : WT; bitpos: [5]; default: 0;
         *  Set this bit to clear the OUTFIFO_UDF_L1_CH_INT interrupt.
         */
        uint32_t outfifo_udf_int_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_out_int_clr_chn_reg_t;


/** Group: Debug Registers */
/** Type of ahb_test register
 *  reserved
 */
typedef union {
    struct {
        /** ahb_testmode : R/W; bitpos: [2:0]; default: 0;
         *  reserved
         */
        uint32_t ahb_testmode:3;
        uint32_t reserved_3:1;
        /** ahb_testaddr : R/W; bitpos: [5:4]; default: 0;
         *  reserved
         */
        uint32_t ahb_testaddr:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_ahb_test_reg_t;


/** Group: Configuration Registers */
/** Type of misc_conf register
 *  MISC register
 */
typedef union {
    struct {
        /** ahbm_rst_inter : R/W; bitpos: [0]; default: 0;
         *  Set this bit then clear this bit to reset the internal ahb FSM.
         */
        uint32_t ahbm_rst_inter:1;
        uint32_t reserved_1:1;
        /** arb_pri_dis : R/W; bitpos: [2]; default: 0;
         *  Set this bit to disable priority arbitration function.
         */
        uint32_t arb_pri_dis:1;
        /** clk_en : R/W; bitpos: [3]; default: 0;
         *  1'h1: Force clock on for register. 1'h0: Support clock only when application writes
         *  registers.
         */
        uint32_t clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} gdma_misc_conf_reg_t;

/** Type of in_conf0_chn register
 *  Configure 0 register of Rx channel 0
 */
typedef union {
    struct {
        /** in_rst : R/W; bitpos: [0]; default: 0;
         *  This bit is used to reset DMA channel 0 Rx FSM and Rx FIFO pointer.
         */
        uint32_t in_rst:1;
        /** in_loop_test : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t in_loop_test:1;
        /** indscr_burst_en : R/W; bitpos: [2]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t indscr_burst_en:1;
        /** in_data_burst_en : R/W; bitpos: [3]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Rx channel 0 receiving data
         *  when accessing internal SRAM.
         */
        uint32_t in_data_burst_en:1;
        /** mem_trans_en : R/W; bitpos: [4]; default: 0;
         *  Set this bit 1 to enable automatic transmitting data from memory to memory via DMA.
         */
        uint32_t mem_trans_en:1;
        /** in_etm_en : R/W; bitpos: [5]; default: 0;
         *  Set this bit to 1 to enable etm control mode, dma Rx channel 0 is triggered by etm
         *  task.
         */
        uint32_t in_etm_en:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_in_conf0_chn_reg_t;

/** Type of in_conf1_chn register
 *  Configure 1 register of Rx channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** in_check_owner : R/W; bitpos: [12]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t in_check_owner:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} gdma_in_conf1_chn_reg_t;

/** Type of in_pop_chn register
 *  Pop control register of Rx channel 0
 */
typedef union {
    struct {
        /** infifo_rdata : RO; bitpos: [11:0]; default: 2048;
         *  This register stores the data popping from DMA FIFO.
         */
        uint32_t infifo_rdata:12;
        /** infifo_pop : WT; bitpos: [12]; default: 0;
         *  Set this bit to pop data from DMA FIFO.
         */
        uint32_t infifo_pop:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} gdma_in_pop_chn_reg_t;

/** Type of in_link_chn register
 *  Link descriptor configure and control register of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_addr : R/W; bitpos: [19:0]; default: 0;
         *  This register stores the 20 least significant bits of the first inlink descriptor's
         *  address.
         */
        uint32_t inlink_addr:20;
        /** inlink_auto_ret : R/W; bitpos: [20]; default: 1;
         *  Set this bit to return to current inlink descriptor's address when there are some
         *  errors in current receiving data.
         */
        uint32_t inlink_auto_ret:1;
        /** inlink_stop : WT; bitpos: [21]; default: 0;
         *  Set this bit to stop dealing with the inlink descriptors.
         */
        uint32_t inlink_stop:1;
        /** inlink_start : WT; bitpos: [22]; default: 0;
         *  Set this bit to start dealing with the inlink descriptors.
         */
        uint32_t inlink_start:1;
        /** inlink_restart : WT; bitpos: [23]; default: 0;
         *  Set this bit to mount a new inlink descriptor.
         */
        uint32_t inlink_restart:1;
        /** inlink_park : RO; bitpos: [24]; default: 1;
         *  1: the inlink descriptor's FSM is in idle state.  0: the inlink descriptor's FSM is
         *  working.
         */
        uint32_t inlink_park:1;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gdma_in_link_chn_reg_t;

/** Type of out_conf0_chn register
 *  Configure 0 register of Tx channel 0
 */
typedef union {
    struct {
        /** out_rst : R/W; bitpos: [0]; default: 0;
         *  This bit is used to reset DMA channel 0 Tx FSM and Tx FIFO pointer.
         */
        uint32_t out_rst:1;
        /** out_loop_test : R/W; bitpos: [1]; default: 0;
         *  reserved
         */
        uint32_t out_loop_test:1;
        /** out_auto_wrback : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable automatic outlink-writeback when all the data in tx buffer
         *  has been transmitted.
         */
        uint32_t out_auto_wrback:1;
        /** out_eof_mode : R/W; bitpos: [3]; default: 1;
         *  EOF flag generation mode when transmitting data. 1: EOF flag for Tx channel 0 is
         *  generated when data need to transmit has been popped from FIFO in DMA
         */
        uint32_t out_eof_mode:1;
        /** outdscr_burst_en : R/W; bitpos: [4]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 reading link
         *  descriptor when accessing internal SRAM.
         */
        uint32_t outdscr_burst_en:1;
        /** out_data_burst_en : R/W; bitpos: [5]; default: 0;
         *  Set this bit to 1 to enable INCR burst transfer for Tx channel 0 transmitting data
         *  when accessing internal SRAM.
         */
        uint32_t out_data_burst_en:1;
        /** out_etm_en : R/W; bitpos: [6]; default: 0;
         *  Set this bit to 1 to enable etm control mode, dma Tx channel 0 is triggered by etm
         *  task.
         */
        uint32_t out_etm_en:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gdma_out_conf0_chn_reg_t;

/** Type of out_conf1_chn register
 *  Configure 1 register of Tx channel 0
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** out_check_owner : R/W; bitpos: [12]; default: 0;
         *  Set this bit to enable checking the owner attribute of the link descriptor.
         */
        uint32_t out_check_owner:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} gdma_out_conf1_chn_reg_t;

/** Type of out_push_chn register
 *  Push control register of Rx channel 0
 */
typedef union {
    struct {
        /** outfifo_wdata : R/W; bitpos: [8:0]; default: 0;
         *  This register stores the data that need to be pushed into DMA FIFO.
         */
        uint32_t outfifo_wdata:9;
        /** outfifo_push : WT; bitpos: [9]; default: 0;
         *  Set this bit to push data into DMA FIFO.
         */
        uint32_t outfifo_push:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} gdma_out_push_chn_reg_t;

/** Type of out_link_chn register
 *  Link descriptor configure and control register of Tx channel 0
 */
typedef union {
    struct {
        /** outlink_addr : R/W; bitpos: [19:0]; default: 0;
         *  This register stores the 20 least significant bits of the first outlink
         *  descriptor's address.
         */
        uint32_t outlink_addr:20;
        /** outlink_stop : WT; bitpos: [20]; default: 0;
         *  Set this bit to stop dealing with the outlink descriptors.
         */
        uint32_t outlink_stop:1;
        /** outlink_start : WT; bitpos: [21]; default: 0;
         *  Set this bit to start dealing with the outlink descriptors.
         */
        uint32_t outlink_start:1;
        /** outlink_restart : WT; bitpos: [22]; default: 0;
         *  Set this bit to restart a new outlink from the last address.
         */
        uint32_t outlink_restart:1;
        /** outlink_park : RO; bitpos: [23]; default: 1;
         *  1: the outlink descriptor's FSM is in idle state.  0: the outlink descriptor's FSM
         *  is working.
         */
        uint32_t outlink_park:1;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} gdma_out_link_chn_reg_t;

/** Group: Version Registers */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [31:0]; default: 35660368;
         *  register version.
         */
        uint32_t date:32;
    };
    uint32_t val;
} gdma_date_reg_t;


/** Group: Status Registers */
/** Type of infifo_status_chn register
 *  Receive FIFO status of Rx channel 0
 */
typedef union {
    struct {
        /** infifo_full : RO; bitpos: [0]; default: 1;
         *  L1 Rx FIFO full signal for Rx channel 0.
         */
        uint32_t infifo_full:1;
        /** infifo_empty : RO; bitpos: [1]; default: 1;
         *  L1 Rx FIFO empty signal for Rx channel 0.
         */
        uint32_t infifo_empty:1;
        /** infifo_cnt : RO; bitpos: [7:2]; default: 0;
         *  The register stores the byte number of the data in L1 Rx FIFO for Rx channel 0.
         */
        uint32_t infifo_cnt:6;
        uint32_t reserved_8:15;
        /** in_remain_under_1b : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_1b:1;
        /** in_remain_under_2b : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_2b:1;
        /** in_remain_under_3b : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_3b:1;
        /** in_remain_under_4b : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t in_remain_under_4b:1;
        /** in_buf_hungry : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t in_buf_hungry:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gdma_infifo_status_chn_reg_t;

/** Type of in_state_chn register
 *  Receive status of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_addr : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current inlink descriptor's address.
         */
        uint32_t inlink_dscr_addr:18;
        /** in_dscr_state : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t in_dscr_state:2;
        /** in_state : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t in_state:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} gdma_in_state_chn_reg_t;

/** Type of in_suc_eof_des_addr_chn register
 *  Inlink descriptor address when EOF occurs of Rx channel 0
 */
typedef union {
    struct {
        /** in_suc_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t in_suc_eof_des_addr:32;
    };
    uint32_t val;
} gdma_in_suc_eof_des_addr_chn_reg_t;

/** Type of in_err_eof_des_addr_chn register
 *  Inlink descriptor address when errors occur of Rx channel 0
 */
typedef union {
    struct {
        /** in_err_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the inlink descriptor when there are some
         *  errors in current receiving data. Only used when peripheral is UHCI0.
         */
        uint32_t in_err_eof_des_addr:32;
    };
    uint32_t val;
} gdma_in_err_eof_des_addr_chn_reg_t;

/** Type of in_dscr_chn register
 *  Current inlink descriptor address of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  The address of the current inlink descriptor x.
         */
        uint32_t inlink_dscr:32;
    };
    uint32_t val;
} gdma_in_dscr_chn_reg_t;

/** Type of in_dscr_bf0_chn register
 *  The last inlink descriptor address of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last inlink descriptor x-1.
         */
        uint32_t inlink_dscr_bf0:32;
    };
    uint32_t val;
} gdma_in_dscr_bf0_chn_reg_t;

/** Type of in_dscr_bf1_chn register
 *  The second-to-last inlink descriptor address of Rx channel 0
 */
typedef union {
    struct {
        /** inlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor x-2.
         */
        uint32_t inlink_dscr_bf1:32;
    };
    uint32_t val;
} gdma_in_dscr_bf1_chn_reg_t;

/** Type of outfifo_status_chn register
 *  Transmit FIFO status of Tx channel 0
 */
typedef union {
    struct {
        /** outfifo_full : RO; bitpos: [0]; default: 0;
         *  L1 Tx FIFO full signal for Tx channel 0.
         */
        uint32_t outfifo_full:1;
        /** outfifo_empty : RO; bitpos: [1]; default: 1;
         *  L1 Tx FIFO empty signal for Tx channel 0.
         */
        uint32_t outfifo_empty:1;
        /** outfifo_cnt : RO; bitpos: [7:2]; default: 0;
         *  The register stores the byte number of the data in L1 Tx FIFO for Tx channel 0.
         */
        uint32_t outfifo_cnt:6;
        uint32_t reserved_8:15;
        /** out_remain_under_1b : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_1b:1;
        /** out_remain_under_2b : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_2b:1;
        /** out_remain_under_3b : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_3b:1;
        /** out_remain_under_4b : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t out_remain_under_4b:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} gdma_outfifo_status_chn_reg_t;

/** Type of out_state_chn register
 *  Transmit status of Tx channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_addr : RO; bitpos: [17:0]; default: 0;
         *  This register stores the current outlink descriptor's address.
         */
        uint32_t outlink_dscr_addr:18;
        /** out_dscr_state : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t out_dscr_state:2;
        /** out_state : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t out_state:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} gdma_out_state_chn_reg_t;

/** Type of out_eof_des_addr_chn register
 *  Outlink descriptor address when EOF occurs of Tx channel 0
 */
typedef union {
    struct {
        /** out_eof_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t out_eof_des_addr:32;
    };
    uint32_t val;
} gdma_out_eof_des_addr_chn_reg_t;

/** Type of out_eof_bfr_des_addr_chn register
 *  The last outlink descriptor address when EOF occurs of Tx channel 0
 */
typedef union {
    struct {
        /** out_eof_bfr_des_addr : RO; bitpos: [31:0]; default: 0;
         *  This register stores the address of the outlink descriptor before the last outlink
         *  descriptor.
         */
        uint32_t out_eof_bfr_des_addr:32;
    };
    uint32_t val;
} gdma_out_eof_bfr_des_addr_chn_reg_t;

/** Type of out_dscr_chn register
 *  Current inlink descriptor address of Tx channel 0
 */
typedef union {
    struct {
        /** outlink_dscr : RO; bitpos: [31:0]; default: 0;
         *  The address of the current outlink descriptor y.
         */
        uint32_t outlink_dscr:32;
    };
    uint32_t val;
} gdma_out_dscr_chn_reg_t;

/** Type of out_dscr_bf0_chn register
 *  The last inlink descriptor address of Tx channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_bf0 : RO; bitpos: [31:0]; default: 0;
         *  The address of the last outlink descriptor y-1.
         */
        uint32_t outlink_dscr_bf0:32;
    };
    uint32_t val;
} gdma_out_dscr_bf0_chn_reg_t;

/** Type of out_dscr_bf1_chn register
 *  The second-to-last inlink descriptor address of Tx channel 0
 */
typedef union {
    struct {
        /** outlink_dscr_bf1 : RO; bitpos: [31:0]; default: 0;
         *  The address of the second-to-last inlink descriptor x-2.
         */
        uint32_t outlink_dscr_bf1:32;
    };
    uint32_t val;
} gdma_out_dscr_bf1_chn_reg_t;


/** Group: Priority Registers */
/** Type of in_pri_chn register
 *  Priority register of Rx channel 0
 */
typedef union {
    struct {
        /** rx_pri : R/W; bitpos: [3:0]; default: 0;
         *  The priority of Rx channel 0. The larger of the value the higher of the priority.
         */
        uint32_t rx_pri:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} gdma_in_pri_chn_reg_t;

/** Type of out_pri_chn register
 *  Priority register of Tx channel 0.
 */
typedef union {
    struct {
        /** tx_pri : R/W; bitpos: [3:0]; default: 0;
         *  The priority of Tx channel 0. The larger of the value the higher of the priority.
         */
        uint32_t tx_pri:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} gdma_out_pri_chn_reg_t;


/** Group: Peripheral Select Registers */
/** Type of in_peri_sel_chn register
 *  Peripheral selection of Rx channel 0
 */
typedef union {
    struct {
        /** peri_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  This register is used to select peripheral for Rx channel 0. 0:SPI2. 1: reserved.
         *  2: UHCI0. 3: I2S0. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC_DAC. 9:
         *  Parallel_IO.
         */
        uint32_t peri_in_sel:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_in_peri_sel_chn_reg_t;

/** Type of out_peri_sel_chn register
 *  Peripheral selection of Tx channel 0
 */
typedef union {
    struct {
        /** peri_out_sel : R/W; bitpos: [5:0]; default: 63;
         *  This register is used to select peripheral for Tx channel 0. 0:SPI2. 1: reserved.
         *  2: UHCI0. 3: I2S0. 4: reserved. 5: reserved. 6: AES. 7: SHA. 8: ADC_DAC. 9:
         *  Parallel_IO.
         */
        uint32_t peri_out_sel:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} gdma_out_peri_sel_chn_reg_t;

typedef struct {
    volatile gdma_in_int_raw_chn_reg_t raw;
    volatile gdma_in_int_st_chn_reg_t st;
    volatile gdma_in_int_ena_chn_reg_t ena;
    volatile gdma_in_int_clr_chn_reg_t clr;
} gdma_in_int_chn_reg_t;

typedef struct {
    volatile gdma_out_int_raw_chn_reg_t raw;
    volatile gdma_out_int_st_chn_reg_t st;
    volatile gdma_out_int_ena_chn_reg_t ena;
    volatile gdma_out_int_clr_chn_reg_t clr;
} gdma_out_int_chn_reg_t;

typedef struct {
    volatile gdma_in_conf0_chn_reg_t in_conf0;
    volatile gdma_in_conf1_chn_reg_t in_conf1;
    volatile gdma_infifo_status_chn_reg_t infifo_status;
    volatile gdma_in_pop_chn_reg_t in_pop;
    volatile gdma_in_link_chn_reg_t in_link;
    volatile gdma_in_state_chn_reg_t in_state;
    volatile gdma_in_suc_eof_des_addr_chn_reg_t in_suc_eof_des_addr;
    volatile gdma_in_err_eof_des_addr_chn_reg_t in_err_eof_des_addr;
    volatile gdma_in_dscr_chn_reg_t in_dscr;
    volatile gdma_in_dscr_bf0_chn_reg_t in_dscr_bf0;
    volatile gdma_in_dscr_bf1_chn_reg_t in_dscr_bf1;
    volatile gdma_in_pri_chn_reg_t in_pri;
    volatile gdma_in_peri_sel_chn_reg_t in_peri_sel;
} gdma_in_chn_reg_t;

typedef struct {
    volatile gdma_out_conf0_chn_reg_t out_conf0;
    volatile gdma_out_conf1_chn_reg_t out_conf1;
    volatile gdma_outfifo_status_chn_reg_t outfifo_status;
    volatile gdma_out_push_chn_reg_t out_push;
    volatile gdma_out_link_chn_reg_t out_link;
    volatile gdma_out_state_chn_reg_t out_state;
    volatile gdma_out_eof_des_addr_chn_reg_t out_eof_des_addr;
    volatile gdma_out_eof_bfr_des_addr_chn_reg_t out_eof_bfr_des_addr;
    volatile gdma_out_dscr_chn_reg_t out_dscr;
    volatile gdma_out_dscr_bf0_chn_reg_t out_dscr_bf0;
    volatile gdma_out_dscr_bf1_chn_reg_t out_dscr_bf1;
    volatile gdma_out_pri_chn_reg_t out_pri;
    volatile gdma_out_peri_sel_chn_reg_t out_peri_sel;
} gdma_out_chn_reg_t;

typedef struct {
    volatile gdma_in_chn_reg_t in;
    uint32_t reserved_in[11];
    volatile gdma_out_chn_reg_t out;
    uint32_t reserved_out[11];
} gdma_chn_reg_t;


typedef struct gdma_dev_s {
    volatile gdma_in_int_chn_reg_t in_intr[3];
    volatile gdma_out_int_chn_reg_t out_intr[3];
    volatile gdma_ahb_test_reg_t ahb_test;
    volatile gdma_misc_conf_reg_t misc_conf;
    volatile gdma_date_reg_t date;
    uint32_t reserved_06c;
    volatile gdma_chn_reg_t channel[3];
} gdma_dev_t;

extern gdma_dev_t GDMA;

#ifndef __cplusplus
_Static_assert(sizeof(gdma_dev_t) == 0x2B0, "Invalid size of gdma_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
