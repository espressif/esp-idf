/**
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Interrupt Registers */
/** Type of dma_in_int_raw_chn register
 *  Raw interrupt status interrupt of RX channel n
 */
typedef union {
    struct {
        /** dma_in_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t dma_in_done_chn_int_raw:1;
        /** dma_in_suc_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t dma_in_suc_eof_chn_int_raw:1;
        /** dma_in_err_eof_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t dma_in_err_eof_chn_int_raw:1;
        /** dma_in_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_in_dscr_err_chn_int_raw:1;
        /** dma_in_dscr_empty_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t dma_in_dscr_empty_chn_int_raw:1;
        /** dma_infifo_ovf_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t dma_infifo_ovf_chn_int_raw:1;
        /** dma_infifo_udf_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t dma_infifo_udf_chn_int_raw:1;
        /** dma_in_ahbinf_resp_err_chn_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  The raw interrupt status of AHB_DMA_IN_RESP_ERR_CHn_INT.
         */
        uint32_t dma_in_ahbinf_resp_err_chn_int_raw:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_raw_chn_reg_t;

/** Type of dma_in_int_st_chn register
 *  Masked interrupt status of RX channel n
 */
typedef union {
    struct {
        /** dma_in_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t dma_in_done_chn_int_st:1;
        /** dma_in_suc_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t dma_in_suc_eof_chn_int_st:1;
        /** dma_in_err_eof_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t dma_in_err_eof_chn_int_st:1;
        /** dma_in_dscr_err_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_in_dscr_err_chn_int_st:1;
        /** dma_in_dscr_empty_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t dma_in_dscr_empty_chn_int_st:1;
        /** dma_infifo_ovf_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t dma_infifo_ovf_chn_int_st:1;
        /** dma_infifo_udf_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t dma_infifo_udf_chn_int_st:1;
        /** dma_in_ahbinf_resp_err_chn_int_st : RO; bitpos: [7]; default: 0;
         *  The masked interrupt status of AHB_DMA_IN_RESP_ERR_CHn_INT.
         */
        uint32_t dma_in_ahbinf_resp_err_chn_int_st:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_st_chn_reg_t;

/** Type of dma_in_int_ena_chn register
 *  Interrupt enable bits of RX channel n
 */
typedef union {
    struct {
        /** dma_in_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t dma_in_done_chn_int_ena:1;
        /** dma_in_suc_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t dma_in_suc_eof_chn_int_ena:1;
        /** dma_in_err_eof_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t dma_in_err_eof_chn_int_ena:1;
        /** dma_in_dscr_err_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_in_dscr_err_chn_int_ena:1;
        /** dma_in_dscr_empty_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t dma_in_dscr_empty_chn_int_ena:1;
        /** dma_infifo_ovf_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t dma_infifo_ovf_chn_int_ena:1;
        /** dma_infifo_udf_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t dma_infifo_udf_chn_int_ena:1;
        /** dma_in_ahbinf_resp_err_chn_int_ena : R/W; bitpos: [7]; default: 0;
         *  Write 1 to enable AHB_DMA_IN_RESP_ERR_CHn_INT.
         */
        uint32_t dma_in_ahbinf_resp_err_chn_int_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_ena_chn_reg_t;

/** Type of dma_in_int_clr_chn register
 *  Interrupt clear bits of RX channel n
 */
typedef union {
    struct {
        /** dma_in_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DONE_CHn_INT.
         */
        uint32_t dma_in_done_chn_int_clr:1;
        /** dma_in_suc_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_SUC_EOF_CHn_INT.
         */
        uint32_t dma_in_suc_eof_chn_int_clr:1;
        /** dma_in_err_eof_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_ERR_EOF_CHn_INT.
         */
        uint32_t dma_in_err_eof_chn_int_clr:1;
        /** dma_in_dscr_err_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_in_dscr_err_chn_int_clr:1;
        /** dma_in_dscr_empty_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_DSCR_EMPTY_CHn_INT.
         */
        uint32_t dma_in_dscr_empty_chn_int_clr:1;
        /** dma_infifo_ovf_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_OVF_CHn_INT.
         */
        uint32_t dma_infifo_ovf_chn_int_clr:1;
        /** dma_infifo_udf_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_INFIFO_UDF_CHn_INT.
         */
        uint32_t dma_infifo_udf_chn_int_clr:1;
        /** dma_in_ahbinf_resp_err_chn_int_clr : WT; bitpos: [7]; default: 0;
         *  Write 1 to clear AHB_DMA_IN_RESP_ERR_CHn_INT.
         */
        uint32_t dma_in_ahbinf_resp_err_chn_int_clr:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_int_clr_chn_reg_t;

/** Type of dma_out_int_raw_chn register
 *  Raw interrupt status of TX channel n
 */
typedef union {
    struct {
        /** dma_out_done_chn_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t dma_out_done_chn_int_raw:1;
        /** dma_out_eof_chn_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t dma_out_eof_chn_int_raw:1;
        /** dma_out_dscr_err_chn_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_out_dscr_err_chn_int_raw:1;
        /** dma_out_total_eof_chn_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t dma_out_total_eof_chn_int_raw:1;
        /** dma_outfifo_ovf_chn_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t dma_outfifo_ovf_chn_int_raw:1;
        /** dma_outfifo_udf_chn_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t dma_outfifo_udf_chn_int_raw:1;
        /** dma_out_ahbinf_resp_err_chn_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  The raw interrupt status of AHB_DMA_OUT_RESP_ERR_CHn_INT.
         */
        uint32_t dma_out_ahbinf_resp_err_chn_int_raw:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_raw_chn_reg_t;

/** Type of dma_out_int_st_chn register
 *  Masked interrupt status of TX channel n
 */
typedef union {
    struct {
        /** dma_out_done_chn_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t dma_out_done_chn_int_st:1;
        /** dma_out_eof_chn_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t dma_out_eof_chn_int_st:1;
        /** dma_out_dscr_err_chn_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_out_dscr_err_chn_int_st:1;
        /** dma_out_total_eof_chn_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t dma_out_total_eof_chn_int_st:1;
        /** dma_outfifo_ovf_chn_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t dma_outfifo_ovf_chn_int_st:1;
        /** dma_outfifo_udf_chn_int_st : RO; bitpos: [5]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t dma_outfifo_udf_chn_int_st:1;
        /** dma_out_ahbinf_resp_err_chn_int_st : RO; bitpos: [6]; default: 0;
         *  The masked interrupt status of AHB_DMA_OUT_RESP_ERR_CHn_INT.
         */
        uint32_t dma_out_ahbinf_resp_err_chn_int_st:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_st_chn_reg_t;

/** Type of dma_out_int_ena_chn register
 *  Interrupt enable bits of TX channel n
 */
typedef union {
    struct {
        /** dma_out_done_chn_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t dma_out_done_chn_int_ena:1;
        /** dma_out_eof_chn_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t dma_out_eof_chn_int_ena:1;
        /** dma_out_dscr_err_chn_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_out_dscr_err_chn_int_ena:1;
        /** dma_out_total_eof_chn_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t dma_out_total_eof_chn_int_ena:1;
        /** dma_outfifo_ovf_chn_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t dma_outfifo_ovf_chn_int_ena:1;
        /** dma_outfifo_udf_chn_int_ena : R/W; bitpos: [5]; default: 0;
         *  Write 1 to enable AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t dma_outfifo_udf_chn_int_ena:1;
        /** dma_out_ahbinf_resp_err_chn_int_ena : R/W; bitpos: [6]; default: 0;
         *  Write 1 to enable AHB_DMA_OUT_RESP_ERR_CHn_INT.
         */
        uint32_t dma_out_ahbinf_resp_err_chn_int_ena:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_ena_chn_reg_t;

/** Type of dma_out_int_clr_chn register
 *  Interrupt clear bits of TX channel n
 */
typedef union {
    struct {
        /** dma_out_done_chn_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DONE_CHn_INT.
         */
        uint32_t dma_out_done_chn_int_clr:1;
        /** dma_out_eof_chn_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_EOF_CHn_INT.
         */
        uint32_t dma_out_eof_chn_int_clr:1;
        /** dma_out_dscr_err_chn_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_DSCR_ERR_CHn_INT.
         */
        uint32_t dma_out_dscr_err_chn_int_clr:1;
        /** dma_out_total_eof_chn_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_TOTAL_EOF_CHn_INT.
         */
        uint32_t dma_out_total_eof_chn_int_clr:1;
        /** dma_outfifo_ovf_chn_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_OVF_CHn_INT.
         */
        uint32_t dma_outfifo_ovf_chn_int_clr:1;
        /** dma_outfifo_udf_chn_int_clr : WT; bitpos: [5]; default: 0;
         *  Write 1 to clear AHB_DMA_OUTFIFO_UDF_CHn_INT.
         */
        uint32_t dma_outfifo_udf_chn_int_clr:1;
        /** dma_out_ahbinf_resp_err_chn_int_clr : WT; bitpos: [6]; default: 0;
         *  Write 1 to clear AHB_DMA_OUT_RESP_ERR_CHn_INT.
         */
        uint32_t dma_out_ahbinf_resp_err_chn_int_clr:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} ahb_dma_out_int_clr_chn_reg_t;


/** Group: Debug Registers */
/** Type of dma_ahb_test register
 *  reserved
 */
typedef union {
    struct {
        /** dma_ahb_testmode : R/W; bitpos: [2:0]; default: 0;
         *  reserved
         */
        uint32_t dma_ahb_testmode:3;
        uint32_t reserved_3:1;
        /** dma_ahb_testaddr : R/W; bitpos: [5:4]; default: 0;
         *  reserved
         */
        uint32_t dma_ahb_testaddr:2;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_ahb_test_reg_t;


/** Group: Configuration Registers */
/** Type of dma_misc_conf register
 *  Miscellaneous register
 */
typedef union {
    struct {
        /** dma_ahbm_rst_inter : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset the internal AHB FSM.
         */
        uint32_t dma_ahbm_rst_inter:1;
        uint32_t reserved_1:1;
        /** dma_arb_pri_dis : R/W; bitpos: [2]; default: 0;
         *  Configures whether to disable the fixed-priority channel arbitration.
         *  0: Enable
         *  1: Disable
         */
        uint32_t dma_arb_pri_dis:1;
        /** dma_clk_en : R/W; bitpos: [3]; default: 0;
         *  Configures clock gating.
         *  0: Support clock only when the application writes registers.
         *  1: Always force the clock on for registers.
         */
        uint32_t dma_clk_en:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_misc_conf_reg_t;

/** Type of dma_in_conf0_chn register
 *  Configuration register 0 of RX channel n
 */
typedef union {
    struct {
        /** dma_in_rst_chn : R/W; bitpos: [0]; default: 0;
         *  Write 1 and then 0 to reset AHB_DMA channel 0 RX FSM and RX FIFO pointer.
         */
        uint32_t dma_in_rst_chn:1;
        /** dma_in_loop_test_chn : R/W; bitpos: [1]; default: 0;
         *  Reserved.
         */
        uint32_t dma_in_loop_test_chn:1;
        /** dma_indscr_burst_en_chn : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable INCR burst transfer for RX channel n to read
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_indscr_burst_en_chn:1;
        uint32_t reserved_3:1;
        /** dma_mem_trans_en_chn : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable memory-to-memory data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_mem_trans_en_chn:1;
        /** dma_in_etm_en_chn : R/W; bitpos: [5]; default: 0;
         *  Configures whether to enable ETM control for RX channeln.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_in_etm_en_chn:1;
        /** dma_in_data_burst_mode_sel_chn : R/W; bitpos: [7:6]; default: 0;
         *  Configures max burst size for Rx channeln.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t dma_in_data_burst_mode_sel_chn:2;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} ahb_dma_in_conf0_chn_reg_t;

/** Type of dma_in_conf1_chn register
 *  Configuration register 1 of RX channel n
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** dma_in_check_owner_chn : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for RX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_in_check_owner_chn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_conf1_chn_reg_t;

/** Type of dma_in_pop_chn register
 *  Pop control register of RX channel n
 */
typedef union {
    struct {
        /** dma_infifo_rdata_chn : RO; bitpos: [11:0]; default: 2048;
         *  Represents the data popped from AHB_DMA FIFO.
         */
        uint32_t dma_infifo_rdata_chn:12;
        /** dma_infifo_pop_chn : WT; bitpos: [12]; default: 0;
         *  Configures whether to pop data from AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Pop
         */
        uint32_t dma_infifo_pop_chn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_in_pop_chn_reg_t;

/** Type of dma_in_link_chn register
 *  Linked list descriptor configuration and control register of RX channel n
 */
typedef union {
    struct {
        /** dma_inlink_auto_ret_chn : R/W; bitpos: [0]; default: 1;
         *  Configures whether to return to current receive descriptor's address when there are
         *  some errors in current receiving data.
         *  0: Not return
         *  1: Return
         */
        uint32_t dma_inlink_auto_ret_chn:1;
        /** dma_inlink_stop_chn : WT; bitpos: [1]; default: 0;
         *  Configures whether to stop AHB_DMA's RX channel n from receiving data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t dma_inlink_stop_chn:1;
        /** dma_inlink_start_chn : WT; bitpos: [2]; default: 0;
         *  Configures whether to enable AHB_DMA's RX channel n for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_inlink_start_chn:1;
        /** dma_inlink_restart_chn : WT; bitpos: [3]; default: 0;
         *  Configures whether to restart RX channel n for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t dma_inlink_restart_chn:1;
        /** dma_inlink_park_chn : RO; bitpos: [4]; default: 1;
         *  Represents the status of the receive descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t dma_inlink_park_chn:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} ahb_dma_in_link_chn_reg_t;

/** Type of dma_in_link_addr_chn register
 *  Link list descriptor address configuration of RX channel n
 */
typedef union {
    struct {
        /** dma_inlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address.
         */
        uint32_t dma_inlink_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_link_addr_chn_reg_t;

/** Type of dma_rx_ch_arb_weigh_chn register
 *  RX channel n arbitration weight configuration register
 */
typedef union {
    struct {
        /** dma_rx_ch_arb_weigh_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of RX channeln
         */
        uint32_t dma_rx_ch_arb_weigh_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_rx_ch_arb_weigh_chn_reg_t;

/** Type of dma_rx_arb_weigh_opt_dir_chn register
 *  RX channel n weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** dma_rx_arb_weigh_opt_dir_chn : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t dma_rx_arb_weigh_opt_dir_chn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t;

/** Type of dma_out_conf0_ch0 register
 *  Configuration register 0 of TX channel 0
 */
typedef union {
    struct {
        /** dma_out_rst_ch0 : R/W; bitpos: [0]; default: 0;
         *  Configures the reset state of AHB_DMA channel 0 TX FSM and TX FIFO pointer.
         *  0: Release reset
         *  1: Reset
         */
        uint32_t dma_out_rst_ch0:1;
        /** dma_out_loop_test_ch0 : R/W; bitpos: [1]; default: 0;
         *  Reserved.
         */
        uint32_t dma_out_loop_test_ch0:1;
        /** dma_out_auto_wrback_ch0 : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable automatic outlink write-back when all the data in TX
         *  FIFO has been transmitted.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_out_auto_wrback_ch0:1;
        /** dma_out_eof_mode_ch0 : R/W; bitpos: [3]; default: 1;
         *  Configures when to generate EOF flag.
         *  0: EOF flag for TX channel 0 is generated when data to be transmitted has been
         *  pushed into FIFO in AHB_DMA.
         *  1: EOF flag for TX channel 0 is generated when data to be transmitted has been
         *  popped from FIFO in AHB_DMA.
         */
        uint32_t dma_out_eof_mode_ch0:1;
        /** dma_outdscr_burst_en_ch0 : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable INCR burst transfer for TX channel 0 reading
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_outdscr_burst_en_ch0:1;
        uint32_t reserved_5:1;
        /** dma_out_etm_en_ch0 : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable ETM control for TX channel 0.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_out_etm_en_ch0:1;
        uint32_t reserved_7:1;
        /** dma_out_data_burst_mode_sel_ch0 : R/W; bitpos: [9:8]; default: 0;
         *  Configures max burst size for TX channel0.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t dma_out_data_burst_mode_sel_ch0:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_conf0_ch0_reg_t;

/** Type of dma_out_conf1_chn register
 *  Configuration register 1 of TX channel n
 */
typedef union {
    struct {
        uint32_t reserved_0:12;
        /** dma_out_check_owner_chn : R/W; bitpos: [12]; default: 0;
         *  Configures whether to enable owner bit check for TX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_out_check_owner_chn:1;
        uint32_t reserved_13:19;
    };
    uint32_t val;
} ahb_dma_out_conf1_chn_reg_t;

/** Type of dma_out_push_chn register
 *  Push control register of TX channel n
 */
typedef union {
    struct {
        /** dma_outfifo_wdata_chn : R/W; bitpos: [8:0]; default: 0;
         *  Configures the data that need to be pushed into AHB_DMA FIFO.
         */
        uint32_t dma_outfifo_wdata_chn:9;
        /** dma_outfifo_push_chn : WT; bitpos: [9]; default: 0;
         *  Configures whether to push data into AHB_DMA FIFO.
         *  0: Invalid. No effect
         *  1: Push
         */
        uint32_t dma_outfifo_push_chn:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_push_chn_reg_t;

/** Type of dma_out_link_chn register
 *  Linked list descriptor configuration and control register of TX channel n
 */
typedef union {
    struct {
        /** dma_outlink_stop_chn : WT; bitpos: [0]; default: 0;
         *  Configures whether to stop AHB_DMA's TX channel n from transmitting data.
         *  0: Invalid. No effect
         *  1: Stop
         */
        uint32_t dma_outlink_stop_chn:1;
        /** dma_outlink_start_chn : WT; bitpos: [1]; default: 0;
         *  Configures whether to enable AHB_DMA's TX channel n for data transfer.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_outlink_start_chn:1;
        /** dma_outlink_restart_chn : WT; bitpos: [2]; default: 0;
         *  Configures whether to restart TX channel n for AHB_DMA transfer.
         *  0: Invalid. No effect
         *  1: Restart
         */
        uint32_t dma_outlink_restart_chn:1;
        /** dma_outlink_park_chn : RO; bitpos: [3]; default: 1;
         *  Represents the status of the transmit descriptor's FSM.
         *  0: Running
         *  1: Idle
         */
        uint32_t dma_outlink_park_chn:1;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_link_chn_reg_t;

/** Type of dma_out_link_addr_chn register
 *  Link list descriptor address configuration of TX channel n
 */
typedef union {
    struct {
        /** dma_outlink_addr_chn : R/W; bitpos: [31:0]; default: 0;
         *  Configures the 32 bits of the first receive descriptor's address.
         */
        uint32_t dma_outlink_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_link_addr_chn_reg_t;

/** Type of dma_tx_ch_arb_weigh_chn register
 *  TX channel n arbitration weight configuration register
 */
typedef union {
    struct {
        /** dma_tx_ch_arb_weigh_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the weight(i.e the number of tokens) of TX channeln
         */
        uint32_t dma_tx_ch_arb_weigh_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_tx_ch_arb_weigh_chn_reg_t;

/** Type of dma_tx_arb_weigh_opt_dir_chn register
 *  TX channel n weight arbitration optimization enable register
 */
typedef union {
    struct {
        /** dma_tx_arb_weigh_opt_dir_chn : R/W; bitpos: [0]; default: 0;
         *  reserved
         */
        uint32_t dma_tx_arb_weigh_opt_dir_chn:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t;

/** Type of dma_out_conf0_chn register
 *  Configuration register 0 of TX channel n
 */
typedef union {
    struct {
        /** dma_out_rst_chn : R/W; bitpos: [0]; default: 0;
         *  Configures the reset state of AHB_DMA channel n TX FSM and TX FIFO pointer.
         *  0: Release reset
         *  1: Reset
         */
        uint32_t dma_out_rst_chn:1;
        /** dma_out_loop_test_chn : R/W; bitpos: [1]; default: 0;
         *  Reserved.
         */
        uint32_t dma_out_loop_test_chn:1;
        /** dma_out_auto_wrback_chn : R/W; bitpos: [2]; default: 0;
         *  Configures whether to enable automatic outlink write-back when all the data in TX
         *  FIFO has been transmitted.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_out_auto_wrback_chn:1;
        /** dma_out_eof_mode_chn : R/W; bitpos: [3]; default: 1;
         *  Configures when to generate EOF flag.
         *  0: EOF flag for TX channel n is generated when data to be transmitted has been
         *  pushed into FIFO in AHB_DMA.
         *  1: EOF flag for TX channel n is generated when data to be transmitted has been
         *  popped from FIFO in AHB_DMA.
         */
        uint32_t dma_out_eof_mode_chn:1;
        /** dma_outdscr_burst_en_chn : R/W; bitpos: [4]; default: 0;
         *  Configures whether to enable INCR burst transfer for TX channel n reading
         *  descriptors.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_outdscr_burst_en_chn:1;
        uint32_t reserved_5:1;
        /** dma_out_etm_en_chn : R/W; bitpos: [6]; default: 0;
         *  Configures whether to enable ETM control for TX channel n.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_out_etm_en_chn:1;
        uint32_t reserved_7:1;
        /** dma_out_data_burst_mode_sel_chn : R/W; bitpos: [9:8]; default: 0;
         *  Configures max burst size for TX channeln.
         *  2'b00: single
         *  2'b01: incr4
         *  2'b10: incr8
         *  2'b11: incr16
         */
        uint32_t dma_out_data_burst_mode_sel_chn:2;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} ahb_dma_out_conf0_chn_reg_t;

/** Type of dma_intr_mem_start_addr register
 *  Accessible address space start address configuration register
 */
typedef union {
    struct {
        /** dma_access_intr_mem_start_addr : R/W; bitpos: [31:0]; default: 0;
         *  Configures the start address of accessible address space.
         */
        uint32_t dma_access_intr_mem_start_addr:32;
    };
    uint32_t val;
} ahb_dma_intr_mem_start_addr_reg_t;

/** Type of dma_intr_mem_end_addr register
 *  Accessible address space end address configuration register
 */
typedef union {
    struct {
        /** dma_access_intr_mem_end_addr : R/W; bitpos: [31:0]; default: 4294967295;
         *  Configures the end address of accessible address space.
         */
        uint32_t dma_access_intr_mem_end_addr:32;
    };
    uint32_t val;
} ahb_dma_intr_mem_end_addr_reg_t;

/** Type of dma_arb_timeout register
 *  TX arbitration timeout configuration register
 */
typedef union {
    struct {
        /** dma_arb_timeout : R/W; bitpos: [15:0]; default: 0;
         *  Configures the time slot. Measurement unit: AHB bus clock cycle.
         */
        uint32_t dma_arb_timeout:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} ahb_dma_arb_timeout_reg_t;

/** Type of dma_weight_en register
 *  TX weight arbitration enable register
 */
typedef union {
    struct {
        /** dma_weight_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether to enable weight arbitration.
         *  0: Disable
         *  1: Enable
         */
        uint32_t dma_weight_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} ahb_dma_weight_en_reg_t;

/** Type of dma_module_clk_en register
 *  Module clock force on register
 */
typedef union {
    struct {
        /** dma_ahb_apb_sync_clk_en : R/W; bitpos: [4:0]; default: 31;
         *  Configures whether to force on ahb_apb_sync 4~0 module clock. For bit n:
         *  0 : Not force on ahb_apb_sync n clock
         *  1 : Force on ahb_apb_sync n clock
         */
        uint32_t dma_ahb_apb_sync_clk_en:5;
        /** dma_out_dscr_clk_en : R/W; bitpos: [9:5]; default: 31;
         *  Configures whether to force on out_dscr 4~0 module clock. For bit n:
         *  0 : Not force on out_dscr n clock
         *  1 : Force on out_dscr n clock
         */
        uint32_t dma_out_dscr_clk_en:5;
        /** dma_out_ctrl_clk_en : R/W; bitpos: [14:10]; default: 31;
         *  Configures whether to force on out_ctrl 4~0 module clock. For bit n:
         *  0 : Not force on out_ctrl n clock
         *  1 : Force on out_ctrl n clock
         */
        uint32_t dma_out_ctrl_clk_en:5;
        /** dma_in_dscr_clk_en : R/W; bitpos: [19:15]; default: 31;
         *  Configures whether to force on in_dscr 4~0 module clock. For bit n:
         *  0 : Not force on in_dscr n clock
         *  1 : Force on in_dscr n clock
         */
        uint32_t dma_in_dscr_clk_en:5;
        /** dma_in_ctrl_clk_en : R/W; bitpos: [24:20]; default: 31;
         *  Configures whether to force on in_ctrl 4~0 module clock. For bit n:
         *  0 : Not force on in_ctrl n clock
         *  1 : Force on in_ctrl n clock
         */
        uint32_t dma_in_ctrl_clk_en:5;
        uint32_t reserved_25:2;
        /** dma_cmd_arb_clk_en : R/W; bitpos: [27]; default: 1;
         *  Configures whether to force on cmd_arb module clock.
         *  0 : Not force on cmd_arb clock
         *  1 : Force on cmd_arb clock
         */
        uint32_t dma_cmd_arb_clk_en:1;
        /** dma_ahbinf_clk_en : R/W; bitpos: [28]; default: 1;
         *  Configures whether to force on ahbinf module clock.
         *  0 : Not force on ahbinf clock
         *  1 : Force on ahbinf clock
         */
        uint32_t dma_ahbinf_clk_en:1;
        uint32_t reserved_29:3;
    };
    uint32_t val;
} ahb_dma_module_clk_en_reg_t;


/** Group: Version Registers */
/** Type of dma_date register
 *  Version control register
 */
typedef union {
    struct {
        /** dma_date : R/W; bitpos: [31:0]; default: 37778192;
         *  Version control register.
         */
        uint32_t dma_date:32;
    };
    uint32_t val;
} ahb_dma_date_reg_t;


/** Group: Status Registers */
/** Type of dma_infifo_status_chn register
 *  Receive FIFO status of RX channel n
 */
typedef union {
    struct {
        /** dma_infifo_full_chn : RO; bitpos: [0]; default: 1;
         *  Represents whether L1 RX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t dma_infifo_full_chn:1;
        /** dma_infifo_empty_chn : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 RX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t dma_infifo_empty_chn:1;
        uint32_t reserved_2:6;
        /** dma_infifo_cnt_chn : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 RX FIFO for RX channel n.
         */
        uint32_t dma_infifo_cnt_chn:7;
        uint32_t reserved_15:8;
        /** dma_in_remain_under_1b_chn : RO; bitpos: [23]; default: 1;
         *  reserved
         */
        uint32_t dma_in_remain_under_1b_chn:1;
        /** dma_in_remain_under_2b_chn : RO; bitpos: [24]; default: 1;
         *  reserved
         */
        uint32_t dma_in_remain_under_2b_chn:1;
        /** dma_in_remain_under_3b_chn : RO; bitpos: [25]; default: 1;
         *  reserved
         */
        uint32_t dma_in_remain_under_3b_chn:1;
        /** dma_in_remain_under_4b_chn : RO; bitpos: [26]; default: 1;
         *  reserved
         */
        uint32_t dma_in_remain_under_4b_chn:1;
        /** dma_in_buf_hungry_chn : RO; bitpos: [27]; default: 0;
         *  reserved
         */
        uint32_t dma_in_buf_hungry_chn:1;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} ahb_dma_infifo_status_chn_reg_t;

/** Type of dma_in_state_chn register
 *  Receive status of RX channel n
 */
typedef union {
    struct {
        /** dma_inlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  Represents the address of the lower 18 bits of the next receive descriptor to be
         *  processed.
         */
        uint32_t dma_inlink_dscr_addr_chn:18;
        /** dma_in_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t dma_in_dscr_state_chn:2;
        /** dma_in_state_chn : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t dma_in_state_chn:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_in_state_chn_reg_t;

/** Type of dma_in_suc_eof_des_addr_chn register
 *  Receive descriptor address when EOF occurs on RX channel n
 */
typedef union {
    struct {
        /** dma_in_suc_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t dma_in_suc_eof_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_suc_eof_des_addr_chn_reg_t;

/** Type of dma_in_err_eof_des_addr_chn register
 *  Receive descriptor address when errors occur of RX channel n
 */
typedef union {
    struct {
        /** dma_in_err_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the receive descriptor when there are some errors in the
         *  currently received data.
         */
        uint32_t dma_in_err_eof_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_err_eof_des_addr_chn_reg_t;

/** Type of dma_in_done_des_addr_chn register
 *  RX_done inlink descriptor address of RX channel n
 */
typedef union {
    struct {
        /** dma_in_done_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the inlink descriptor when  this descriptor is completed .
         */
        uint32_t dma_in_done_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_done_des_addr_chn_reg_t;

/** Type of dma_in_dscr_chn register
 *  Current receive descriptor address of RX channel n
 */
typedef union {
    struct {
        /** dma_inlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next receive descriptor x+1 pointed by the current
         *  receive descriptor that has already been fetched.
         */
        uint32_t dma_inlink_dscr_chn:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_chn_reg_t;

/** Type of dma_in_dscr_bf0_chn register
 *  The last receive descriptor address of RX channel n
 */
typedef union {
    struct {
        /** dma_inlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current receive descriptor x that has already been
         *  fetched.
         */
        uint32_t dma_inlink_dscr_bf0_chn:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf0_chn_reg_t;

/** Type of dma_in_dscr_bf1_chn register
 *  The second-to-last receive descriptor address of RX channel n
 */
typedef union {
    struct {
        /** dma_inlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous receive descriptor x-1 that has already been
         *  fetched.
         */
        uint32_t dma_inlink_dscr_bf1_chn:32;
    };
    uint32_t val;
} ahb_dma_in_dscr_bf1_chn_reg_t;

/** Type of dma_outfifo_status_chn register
 *  Transmit FIFO status of TX channel n
 */
typedef union {
    struct {
        /** dma_outfifo_full_chn : RO; bitpos: [0]; default: 0;
         *  Represents whether L1 TX FIFO is full.
         *  0: Not Full
         *  1: Full
         */
        uint32_t dma_outfifo_full_chn:1;
        /** dma_outfifo_empty_chn : RO; bitpos: [1]; default: 1;
         *  Represents whether L1 TX FIFO is empty.
         *  0: Not empty
         *  1: Empty
         */
        uint32_t dma_outfifo_empty_chn:1;
        uint32_t reserved_2:6;
        /** dma_outfifo_cnt_chn : RO; bitpos: [14:8]; default: 0;
         *  Represents the number of data bytes in L1 TX FIFO for TX channel n.
         */
        uint32_t dma_outfifo_cnt_chn:7;
        uint32_t reserved_15:8;
        /** dma_out_remain_under_1b_chn : RO; bitpos: [23]; default: 1;
         *  Reserved.
         */
        uint32_t dma_out_remain_under_1b_chn:1;
        /** dma_out_remain_under_2b_chn : RO; bitpos: [24]; default: 1;
         *  Reserved.
         */
        uint32_t dma_out_remain_under_2b_chn:1;
        /** dma_out_remain_under_3b_chn : RO; bitpos: [25]; default: 1;
         *  Reserved.
         */
        uint32_t dma_out_remain_under_3b_chn:1;
        /** dma_out_remain_under_4b_chn : RO; bitpos: [26]; default: 1;
         *  Reserved.
         */
        uint32_t dma_out_remain_under_4b_chn:1;
        uint32_t reserved_27:5;
    };
    uint32_t val;
} ahb_dma_outfifo_status_chn_reg_t;

/** Type of dma_out_state_chn register
 *  Transmit status of TX channel n
 */
typedef union {
    struct {
        /** dma_outlink_dscr_addr_chn : RO; bitpos: [17:0]; default: 0;
         *  Represents the lower 18 bits of the address of the next transmit descriptor to be
         *  processed.
         */
        uint32_t dma_outlink_dscr_addr_chn:18;
        /** dma_out_dscr_state_chn : RO; bitpos: [19:18]; default: 0;
         *  reserved
         */
        uint32_t dma_out_dscr_state_chn:2;
        /** dma_out_state_chn : RO; bitpos: [22:20]; default: 0;
         *  reserved
         */
        uint32_t dma_out_state_chn:3;
        uint32_t reserved_23:9;
    };
    uint32_t val;
} ahb_dma_out_state_chn_reg_t;

/** Type of dma_out_eof_des_addr_chn register
 *  Transmit descriptor address when EOF occurs on TX channel n
 */
typedef union {
    struct {
        /** dma_out_eof_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor when the EOF bit in this
         *  descriptor is 1.
         */
        uint32_t dma_out_eof_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_eof_des_addr_chn_reg_t;

/** Type of dma_out_eof_bfr_des_addr_chn register
 *  The last transmit descriptor address when EOF occurs on TX channel n
 */
typedef union {
    struct {
        /** dma_out_eof_bfr_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the transmit descriptor before the last transmit
         *  descriptor.
         */
        uint32_t dma_out_eof_bfr_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_eof_bfr_des_addr_chn_reg_t;

/** Type of dma_out_done_des_addr_chn register
 *  TX done outlink descriptor address of TX channel n
 */
typedef union {
    struct {
        /** dma_out_done_des_addr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the outlink descriptor when this descriptor is completed.
         */
        uint32_t dma_out_done_des_addr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_done_des_addr_chn_reg_t;

/** Type of dma_out_dscr_chn register
 *  Current transmit descriptor address of TX channel n
 */
typedef union {
    struct {
        /** dma_outlink_dscr_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the next transmit descriptor y+1 pointed by the current
         *  transmit descriptor that has already been fetched.
         */
        uint32_t dma_outlink_dscr_chn:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_chn_reg_t;

/** Type of dma_out_dscr_bf0_chn register
 *  The last transmit descriptor address of TX channel n
 */
typedef union {
    struct {
        /** dma_outlink_dscr_bf0_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the current transmit descriptor y that has already been
         *  fetched.
         */
        uint32_t dma_outlink_dscr_bf0_chn:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf0_chn_reg_t;

/** Type of dma_out_dscr_bf1_chn register
 *  The second-to-last transmit descriptor address of TX channel n
 */
typedef union {
    struct {
        /** dma_outlink_dscr_bf1_chn : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the previous transmit descriptor y-1 that has already
         *  been fetched.
         */
        uint32_t dma_outlink_dscr_bf1_chn:32;
    };
    uint32_t val;
} ahb_dma_out_dscr_bf1_chn_reg_t;

/** Type of dma_ahbinf_resp_err_status0 register
 *  AHB response error status 0 register
 */
typedef union {
    struct {
        /** dma_ahbinf_resp_err_addr : RO; bitpos: [31:0]; default: 0;
         *  Represents the address of the AHB response error.
         */
        uint32_t dma_ahbinf_resp_err_addr:32;
    };
    uint32_t val;
} ahb_dma_ahbinf_resp_err_status0_reg_t;

/** Type of dma_ahbinf_resp_err_status1 register
 *  AHB response error status 1 register
 */
typedef union {
    struct {
        /** dma_ahbinf_resp_err_wr : RO; bitpos: [0]; default: 0;
         *  Represents the AHB response error is write request.
         */
        uint32_t dma_ahbinf_resp_err_wr:1;
        /** dma_ahbinf_resp_err_id : RO; bitpos: [4:1]; default: 15;
         *  Represents the AHB response error request id.
         */
        uint32_t dma_ahbinf_resp_err_id:4;
        /** dma_ahbinf_resp_err_ch_id : RO; bitpos: [8:5]; default: 0;
         *  Represents the AHB response error request channel id.bit[3]=1:TX channel.
         *  bit[3]=0:RX channel.
         */
        uint32_t dma_ahbinf_resp_err_ch_id:4;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} ahb_dma_ahbinf_resp_err_status1_reg_t;


/** Group: Priority Registers */
/** Type of dma_in_pri_chn register
 *  Priority register of RX channel n
 */
typedef union {
    struct {
        /** dma_rx_pri_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of RX channel n.The larger of the value, the higher of the
         *  priority.
         */
        uint32_t dma_rx_pri_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_in_pri_chn_reg_t;

/** Type of dma_out_pri_chn register
 *  Priority register of TX channel n
 */
typedef union {
    struct {
        /** dma_tx_pri_chn : R/W; bitpos: [3:0]; default: 0;
         *  Configures the priority of TX channel n.The larger of the value, the higher of the
         *  priority.
         */
        uint32_t dma_tx_pri_chn:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} ahb_dma_out_pri_chn_reg_t;


/** Group: Peripheral Select Registers */
/** Type of dma_in_peri_sel_chn register
 *  Peripheral selection register of RX channel n
 */
typedef union {
    struct {
        /** dma_peri_in_sel_chn : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to RX channel n.
         *  0: SPI3
         *  1: SPI2
         *  2: UHCI0
         *  3: I2S0
         *  4: AES
         *  5: Sample_rate_convert-0
         *  6:  Sample_rate_convert-1
         *  7: SHA
         *  8: ADC_DAC
         *  9: PARL_IO
         *  10: Dummy
         *  11~15: Dummy
         */
        uint32_t dma_peri_in_sel_chn:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_in_peri_sel_chn_reg_t;

/** Type of dma_out_peri_sel_chn register
 *  Peripheral selection register of TX channel n
 */
typedef union {
    struct {
        /** dma_peri_out_sel_chn : R/W; bitpos: [5:0]; default: 63;
         *  Configures the peripheral connected to TX channel n.
         *  0: SPI3
         *  1: SPI2
         *  2: UHCI0
         *  3: I2S0
         *  4: AES
         *  5: Sample_rate_convert-0
         *  6:  Sample_rate_convert-1
         *  7: SHA
         *  8: ADC_DAC
         *  9: PARL_IO
         *  10: Dummy
         *  11~15: Dummy
         */
        uint32_t dma_peri_out_sel_chn:6;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} ahb_dma_out_peri_sel_chn_reg_t;


typedef struct {
    volatile ahb_dma_in_int_raw_chn_reg_t dma_in_int_raw_ch0;
    volatile ahb_dma_in_int_st_chn_reg_t dma_in_int_st_ch0;
    volatile ahb_dma_in_int_ena_chn_reg_t dma_in_int_ena_ch0;
    volatile ahb_dma_in_int_clr_chn_reg_t dma_in_int_clr_ch0;
    volatile ahb_dma_in_int_raw_chn_reg_t dma_in_int_raw_ch1;
    volatile ahb_dma_in_int_st_chn_reg_t dma_in_int_st_ch1;
    volatile ahb_dma_in_int_ena_chn_reg_t dma_in_int_ena_ch1;
    volatile ahb_dma_in_int_clr_chn_reg_t dma_in_int_clr_ch1;
    volatile ahb_dma_in_int_raw_chn_reg_t dma_in_int_raw_ch2;
    volatile ahb_dma_in_int_st_chn_reg_t dma_in_int_st_ch2;
    volatile ahb_dma_in_int_ena_chn_reg_t dma_in_int_ena_ch2;
    volatile ahb_dma_in_int_clr_chn_reg_t dma_in_int_clr_ch2;
    volatile ahb_dma_in_int_raw_chn_reg_t dma_in_int_raw_ch3;
    volatile ahb_dma_in_int_st_chn_reg_t dma_in_int_st_ch3;
    volatile ahb_dma_in_int_ena_chn_reg_t dma_in_int_ena_ch3;
    volatile ahb_dma_in_int_clr_chn_reg_t dma_in_int_clr_ch3;
    volatile ahb_dma_in_int_raw_chn_reg_t dma_in_int_raw_ch4;
    volatile ahb_dma_in_int_st_chn_reg_t dma_in_int_st_ch4;
    volatile ahb_dma_in_int_ena_chn_reg_t dma_in_int_ena_ch4;
    volatile ahb_dma_in_int_clr_chn_reg_t dma_in_int_clr_ch4;
    volatile ahb_dma_out_int_raw_chn_reg_t dma_out_int_raw_ch0;
    volatile ahb_dma_out_int_st_chn_reg_t dma_out_int_st_ch0;
    volatile ahb_dma_out_int_ena_chn_reg_t dma_out_int_ena_ch0;
    volatile ahb_dma_out_int_clr_chn_reg_t dma_out_int_clr_ch0;
    volatile ahb_dma_out_int_raw_chn_reg_t dma_out_int_raw_ch1;
    volatile ahb_dma_out_int_st_chn_reg_t dma_out_int_st_ch1;
    volatile ahb_dma_out_int_ena_chn_reg_t dma_out_int_ena_ch1;
    volatile ahb_dma_out_int_clr_chn_reg_t dma_out_int_clr_ch1;
    volatile ahb_dma_out_int_raw_chn_reg_t dma_out_int_raw_ch2;
    volatile ahb_dma_out_int_st_chn_reg_t dma_out_int_st_ch2;
    volatile ahb_dma_out_int_ena_chn_reg_t dma_out_int_ena_ch2;
    volatile ahb_dma_out_int_clr_chn_reg_t dma_out_int_clr_ch2;
    volatile ahb_dma_out_int_raw_chn_reg_t dma_out_int_raw_ch3;
    volatile ahb_dma_out_int_st_chn_reg_t dma_out_int_st_ch3;
    volatile ahb_dma_out_int_ena_chn_reg_t dma_out_int_ena_ch3;
    volatile ahb_dma_out_int_clr_chn_reg_t dma_out_int_clr_ch3;
    volatile ahb_dma_out_int_raw_chn_reg_t dma_out_int_raw_ch4;
    volatile ahb_dma_out_int_st_chn_reg_t dma_out_int_st_ch4;
    volatile ahb_dma_out_int_ena_chn_reg_t dma_out_int_ena_ch4;
    volatile ahb_dma_out_int_clr_chn_reg_t dma_out_int_clr_ch4;
    volatile ahb_dma_ahb_test_reg_t dma_ahb_test;
    volatile ahb_dma_misc_conf_reg_t dma_misc_conf;
    volatile ahb_dma_date_reg_t dma_date;
    uint32_t reserved_0ac[21];
    volatile ahb_dma_in_conf0_chn_reg_t dma_in_conf0_ch0;
    volatile ahb_dma_in_conf1_chn_reg_t dma_in_conf1_ch0;
    volatile ahb_dma_infifo_status_chn_reg_t dma_infifo_status_ch0;
    volatile ahb_dma_in_pop_chn_reg_t dma_in_pop_ch0;
    volatile ahb_dma_in_link_chn_reg_t dma_in_link_ch0;
    volatile ahb_dma_in_link_addr_chn_reg_t dma_in_link_addr_ch0;
    volatile ahb_dma_in_state_chn_reg_t dma_in_state_ch0;
    volatile ahb_dma_in_suc_eof_des_addr_chn_reg_t dma_in_suc_eof_des_addr_ch0;
    volatile ahb_dma_in_err_eof_des_addr_chn_reg_t dma_in_err_eof_des_addr_ch0;
    volatile ahb_dma_in_done_des_addr_chn_reg_t dma_in_done_des_addr_ch0;
    volatile ahb_dma_in_dscr_chn_reg_t dma_in_dscr_ch0;
    volatile ahb_dma_in_dscr_bf0_chn_reg_t dma_in_dscr_bf0_ch0;
    volatile ahb_dma_in_dscr_bf1_chn_reg_t dma_in_dscr_bf1_ch0;
    volatile ahb_dma_in_pri_chn_reg_t dma_in_pri_ch0;
    volatile ahb_dma_in_peri_sel_chn_reg_t dma_in_peri_sel_ch0;
    volatile ahb_dma_rx_ch_arb_weigh_chn_reg_t dma_rx_ch_arb_weigh_ch0;
    volatile ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t dma_rx_arb_weigh_opt_dir_ch0;
    uint32_t reserved_144[15];
    volatile ahb_dma_out_conf0_ch0_reg_t dma_out_conf0_ch0;
    volatile ahb_dma_out_conf1_chn_reg_t dma_out_conf1_ch0;
    volatile ahb_dma_outfifo_status_chn_reg_t dma_outfifo_status_ch0;
    volatile ahb_dma_out_push_chn_reg_t dma_out_push_ch0;
    volatile ahb_dma_out_link_chn_reg_t dma_out_link_ch0;
    volatile ahb_dma_out_link_addr_chn_reg_t dma_out_link_addr_ch0;
    volatile ahb_dma_out_state_chn_reg_t dma_out_state_ch0;
    volatile ahb_dma_out_eof_des_addr_chn_reg_t dma_out_eof_des_addr_ch0;
    volatile ahb_dma_out_eof_bfr_des_addr_chn_reg_t dma_out_eof_bfr_des_addr_ch0;
    volatile ahb_dma_out_done_des_addr_chn_reg_t dma_out_done_des_addr_ch0;
    volatile ahb_dma_out_dscr_chn_reg_t dma_out_dscr_ch0;
    volatile ahb_dma_out_dscr_bf0_chn_reg_t dma_out_dscr_bf0_ch0;
    volatile ahb_dma_out_dscr_bf1_chn_reg_t dma_out_dscr_bf1_ch0;
    volatile ahb_dma_out_pri_chn_reg_t dma_out_pri_ch0;
    volatile ahb_dma_out_peri_sel_chn_reg_t dma_out_peri_sel_ch0;
    volatile ahb_dma_tx_ch_arb_weigh_chn_reg_t dma_tx_ch_arb_weigh_ch0;
    volatile ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t dma_tx_arb_weigh_opt_dir_ch0;
    uint32_t reserved_1c4[15];
    volatile ahb_dma_in_conf0_chn_reg_t dma_in_conf0_ch1;
    volatile ahb_dma_in_conf1_chn_reg_t dma_in_conf1_ch1;
    volatile ahb_dma_infifo_status_chn_reg_t dma_infifo_status_ch1;
    volatile ahb_dma_in_pop_chn_reg_t dma_in_pop_ch1;
    volatile ahb_dma_in_link_chn_reg_t dma_in_link_ch1;
    volatile ahb_dma_in_link_addr_chn_reg_t dma_in_link_addr_ch1;
    volatile ahb_dma_in_state_chn_reg_t dma_in_state_ch1;
    volatile ahb_dma_in_suc_eof_des_addr_chn_reg_t dma_in_suc_eof_des_addr_ch1;
    volatile ahb_dma_in_err_eof_des_addr_chn_reg_t dma_in_err_eof_des_addr_ch1;
    volatile ahb_dma_in_done_des_addr_chn_reg_t dma_in_done_des_addr_ch1;
    volatile ahb_dma_in_dscr_chn_reg_t dma_in_dscr_ch1;
    volatile ahb_dma_in_dscr_bf0_chn_reg_t dma_in_dscr_bf0_ch1;
    volatile ahb_dma_in_dscr_bf1_chn_reg_t dma_in_dscr_bf1_ch1;
    volatile ahb_dma_in_pri_chn_reg_t dma_in_pri_ch1;
    volatile ahb_dma_in_peri_sel_chn_reg_t dma_in_peri_sel_ch1;
    volatile ahb_dma_rx_ch_arb_weigh_chn_reg_t dma_rx_ch_arb_weigh_ch1;
    volatile ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t dma_rx_arb_weigh_opt_dir_ch1;
    uint32_t reserved_244[15];
    volatile ahb_dma_out_conf0_chn_reg_t dma_out_conf0_ch1;
    volatile ahb_dma_out_conf1_chn_reg_t dma_out_conf1_ch1;
    volatile ahb_dma_outfifo_status_chn_reg_t dma_outfifo_status_ch1;
    volatile ahb_dma_out_push_chn_reg_t dma_out_push_ch1;
    volatile ahb_dma_out_link_chn_reg_t dma_out_link_ch1;
    volatile ahb_dma_out_link_addr_chn_reg_t dma_out_link_addr_ch1;
    volatile ahb_dma_out_state_chn_reg_t dma_out_state_ch1;
    volatile ahb_dma_out_eof_des_addr_chn_reg_t dma_out_eof_des_addr_ch1;
    volatile ahb_dma_out_eof_bfr_des_addr_chn_reg_t dma_out_eof_bfr_des_addr_ch1;
    volatile ahb_dma_out_done_des_addr_chn_reg_t dma_out_done_des_addr_ch1;
    volatile ahb_dma_out_dscr_chn_reg_t dma_out_dscr_ch1;
    volatile ahb_dma_out_dscr_bf0_chn_reg_t dma_out_dscr_bf0_ch1;
    volatile ahb_dma_out_dscr_bf1_chn_reg_t dma_out_dscr_bf1_ch1;
    volatile ahb_dma_out_pri_chn_reg_t dma_out_pri_ch1;
    volatile ahb_dma_out_peri_sel_chn_reg_t dma_out_peri_sel_ch1;
    volatile ahb_dma_tx_ch_arb_weigh_chn_reg_t dma_tx_ch_arb_weigh_ch1;
    volatile ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t dma_tx_arb_weigh_opt_dir_ch1;
    uint32_t reserved_2c4[15];
    volatile ahb_dma_in_conf0_chn_reg_t dma_in_conf0_ch2;
    volatile ahb_dma_in_conf1_chn_reg_t dma_in_conf1_ch2;
    volatile ahb_dma_infifo_status_chn_reg_t dma_infifo_status_ch2;
    volatile ahb_dma_in_pop_chn_reg_t dma_in_pop_ch2;
    volatile ahb_dma_in_link_chn_reg_t dma_in_link_ch2;
    volatile ahb_dma_in_link_addr_chn_reg_t dma_in_link_addr_ch2;
    volatile ahb_dma_in_state_chn_reg_t dma_in_state_ch2;
    volatile ahb_dma_in_suc_eof_des_addr_chn_reg_t dma_in_suc_eof_des_addr_ch2;
    volatile ahb_dma_in_err_eof_des_addr_chn_reg_t dma_in_err_eof_des_addr_ch2;
    volatile ahb_dma_in_done_des_addr_chn_reg_t dma_in_done_des_addr_ch2;
    volatile ahb_dma_in_dscr_chn_reg_t dma_in_dscr_ch2;
    volatile ahb_dma_in_dscr_bf0_chn_reg_t dma_in_dscr_bf0_ch2;
    volatile ahb_dma_in_dscr_bf1_chn_reg_t dma_in_dscr_bf1_ch2;
    volatile ahb_dma_in_pri_chn_reg_t dma_in_pri_ch2;
    volatile ahb_dma_in_peri_sel_chn_reg_t dma_in_peri_sel_ch2;
    volatile ahb_dma_rx_ch_arb_weigh_chn_reg_t dma_rx_ch_arb_weigh_ch2;
    volatile ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t dma_rx_arb_weigh_opt_dir_ch2;
    uint32_t reserved_344[15];
    volatile ahb_dma_out_conf0_chn_reg_t dma_out_conf0_ch2;
    volatile ahb_dma_out_conf1_chn_reg_t dma_out_conf1_ch2;
    volatile ahb_dma_outfifo_status_chn_reg_t dma_outfifo_status_ch2;
    volatile ahb_dma_out_push_chn_reg_t dma_out_push_ch2;
    volatile ahb_dma_out_link_chn_reg_t dma_out_link_ch2;
    volatile ahb_dma_out_link_addr_chn_reg_t dma_out_link_addr_ch2;
    volatile ahb_dma_out_state_chn_reg_t dma_out_state_ch2;
    volatile ahb_dma_out_eof_des_addr_chn_reg_t dma_out_eof_des_addr_ch2;
    volatile ahb_dma_out_eof_bfr_des_addr_chn_reg_t dma_out_eof_bfr_des_addr_ch2;
    volatile ahb_dma_out_done_des_addr_chn_reg_t dma_out_done_des_addr_ch2;
    volatile ahb_dma_out_dscr_chn_reg_t dma_out_dscr_ch2;
    volatile ahb_dma_out_dscr_bf0_chn_reg_t dma_out_dscr_bf0_ch2;
    volatile ahb_dma_out_dscr_bf1_chn_reg_t dma_out_dscr_bf1_ch2;
    volatile ahb_dma_out_pri_chn_reg_t dma_out_pri_ch2;
    volatile ahb_dma_out_peri_sel_chn_reg_t dma_out_peri_sel_ch2;
    volatile ahb_dma_tx_ch_arb_weigh_chn_reg_t dma_tx_ch_arb_weigh_ch2;
    volatile ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t dma_tx_arb_weigh_opt_dir_ch2;
    uint32_t reserved_3c4[15];
    volatile ahb_dma_in_conf0_chn_reg_t dma_in_conf0_ch3;
    volatile ahb_dma_in_conf1_chn_reg_t dma_in_conf1_ch3;
    volatile ahb_dma_infifo_status_chn_reg_t dma_infifo_status_ch3;
    volatile ahb_dma_in_pop_chn_reg_t dma_in_pop_ch3;
    volatile ahb_dma_in_link_chn_reg_t dma_in_link_ch3;
    volatile ahb_dma_in_link_addr_chn_reg_t dma_in_link_addr_ch3;
    volatile ahb_dma_in_state_chn_reg_t dma_in_state_ch3;
    volatile ahb_dma_in_suc_eof_des_addr_chn_reg_t dma_in_suc_eof_des_addr_ch3;
    volatile ahb_dma_in_err_eof_des_addr_chn_reg_t dma_in_err_eof_des_addr_ch3;
    volatile ahb_dma_in_done_des_addr_chn_reg_t dma_in_done_des_addr_ch3;
    volatile ahb_dma_in_dscr_chn_reg_t dma_in_dscr_ch3;
    volatile ahb_dma_in_dscr_bf0_chn_reg_t dma_in_dscr_bf0_ch3;
    volatile ahb_dma_in_dscr_bf1_chn_reg_t dma_in_dscr_bf1_ch3;
    volatile ahb_dma_in_pri_chn_reg_t dma_in_pri_ch3;
    volatile ahb_dma_in_peri_sel_chn_reg_t dma_in_peri_sel_ch3;
    volatile ahb_dma_rx_ch_arb_weigh_chn_reg_t dma_rx_ch_arb_weigh_ch3;
    volatile ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t dma_rx_arb_weigh_opt_dir_ch3;
    uint32_t reserved_444[15];
    volatile ahb_dma_out_conf0_chn_reg_t dma_out_conf0_ch3;
    volatile ahb_dma_out_conf1_chn_reg_t dma_out_conf1_ch3;
    volatile ahb_dma_outfifo_status_chn_reg_t dma_outfifo_status_ch3;
    volatile ahb_dma_out_push_chn_reg_t dma_out_push_ch3;
    volatile ahb_dma_out_link_chn_reg_t dma_out_link_ch3;
    volatile ahb_dma_out_link_addr_chn_reg_t dma_out_link_addr_ch3;
    volatile ahb_dma_out_state_chn_reg_t dma_out_state_ch3;
    volatile ahb_dma_out_eof_des_addr_chn_reg_t dma_out_eof_des_addr_ch3;
    volatile ahb_dma_out_eof_bfr_des_addr_chn_reg_t dma_out_eof_bfr_des_addr_ch3;
    volatile ahb_dma_out_done_des_addr_chn_reg_t dma_out_done_des_addr_ch3;
    volatile ahb_dma_out_dscr_chn_reg_t dma_out_dscr_ch3;
    volatile ahb_dma_out_dscr_bf0_chn_reg_t dma_out_dscr_bf0_ch3;
    volatile ahb_dma_out_dscr_bf1_chn_reg_t dma_out_dscr_bf1_ch3;
    volatile ahb_dma_out_pri_chn_reg_t dma_out_pri_ch3;
    volatile ahb_dma_out_peri_sel_chn_reg_t dma_out_peri_sel_ch3;
    volatile ahb_dma_tx_ch_arb_weigh_chn_reg_t dma_tx_ch_arb_weigh_ch3;
    volatile ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t dma_tx_arb_weigh_opt_dir_ch3;
    uint32_t reserved_4c4[15];
    volatile ahb_dma_in_conf0_chn_reg_t dma_in_conf0_ch4;
    volatile ahb_dma_in_conf1_chn_reg_t dma_in_conf1_ch4;
    volatile ahb_dma_infifo_status_chn_reg_t dma_infifo_status_ch4;
    volatile ahb_dma_in_pop_chn_reg_t dma_in_pop_ch4;
    volatile ahb_dma_in_link_chn_reg_t dma_in_link_ch4;
    volatile ahb_dma_in_link_addr_chn_reg_t dma_in_link_addr_ch4;
    volatile ahb_dma_in_state_chn_reg_t dma_in_state_ch4;
    volatile ahb_dma_in_suc_eof_des_addr_chn_reg_t dma_in_suc_eof_des_addr_ch4;
    volatile ahb_dma_in_err_eof_des_addr_chn_reg_t dma_in_err_eof_des_addr_ch4;
    volatile ahb_dma_in_done_des_addr_chn_reg_t dma_in_done_des_addr_ch4;
    volatile ahb_dma_in_dscr_chn_reg_t dma_in_dscr_ch4;
    volatile ahb_dma_in_dscr_bf0_chn_reg_t dma_in_dscr_bf0_ch4;
    volatile ahb_dma_in_dscr_bf1_chn_reg_t dma_in_dscr_bf1_ch4;
    volatile ahb_dma_in_pri_chn_reg_t dma_in_pri_ch4;
    volatile ahb_dma_in_peri_sel_chn_reg_t dma_in_peri_sel_ch4;
    volatile ahb_dma_rx_ch_arb_weigh_chn_reg_t dma_rx_ch_arb_weigh_ch4;
    volatile ahb_dma_rx_arb_weigh_opt_dir_chn_reg_t dma_rx_arb_weigh_opt_dir_ch4;
    uint32_t reserved_544[15];
    volatile ahb_dma_out_conf0_chn_reg_t dma_out_conf0_ch4;
    volatile ahb_dma_out_conf1_chn_reg_t dma_out_conf1_ch4;
    volatile ahb_dma_outfifo_status_chn_reg_t dma_outfifo_status_ch4;
    volatile ahb_dma_out_push_chn_reg_t dma_out_push_ch4;
    volatile ahb_dma_out_link_chn_reg_t dma_out_link_ch4;
    volatile ahb_dma_out_link_addr_chn_reg_t dma_out_link_addr_ch4;
    volatile ahb_dma_out_state_chn_reg_t dma_out_state_ch4;
    volatile ahb_dma_out_eof_des_addr_chn_reg_t dma_out_eof_des_addr_ch4;
    volatile ahb_dma_out_eof_bfr_des_addr_chn_reg_t dma_out_eof_bfr_des_addr_ch4;
    volatile ahb_dma_out_done_des_addr_chn_reg_t dma_out_done_des_addr_ch4;
    volatile ahb_dma_out_dscr_chn_reg_t dma_out_dscr_ch4;
    volatile ahb_dma_out_dscr_bf0_chn_reg_t dma_out_dscr_bf0_ch4;
    volatile ahb_dma_out_dscr_bf1_chn_reg_t dma_out_dscr_bf1_ch4;
    volatile ahb_dma_out_pri_chn_reg_t dma_out_pri_ch4;
    volatile ahb_dma_out_peri_sel_chn_reg_t dma_out_peri_sel_ch4;
    volatile ahb_dma_tx_ch_arb_weigh_chn_reg_t dma_tx_ch_arb_weigh_ch4;
    volatile ahb_dma_tx_arb_weigh_opt_dir_chn_reg_t dma_tx_arb_weigh_opt_dir_ch4;
    uint32_t reserved_5c4[15];
    volatile ahb_dma_intr_mem_start_addr_reg_t dma_intr_mem_start_addr;
    volatile ahb_dma_intr_mem_end_addr_reg_t dma_intr_mem_end_addr;
    volatile ahb_dma_arb_timeout_reg_t dma_arb_timeout;
    uint32_t reserved_60c;
    volatile ahb_dma_weight_en_reg_t dma_weight_en;
    uint32_t reserved_614;
    volatile ahb_dma_module_clk_en_reg_t dma_module_clk_en;
    uint32_t reserved_61c;
    volatile ahb_dma_ahbinf_resp_err_status0_reg_t dma_ahbinf_resp_err_status0;
    volatile ahb_dma_ahbinf_resp_err_status1_reg_t dma_ahbinf_resp_err_status1;
} ahb_dev_t;

extern ahb_dev_t AHB_DMA;

#ifndef __cplusplus
_Static_assert(sizeof(ahb_dev_t) == 0x628, "Invalid size of ahb_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
