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

/** Group: CORDIC  Mode Configuration */
/** Type of csr_cfg register
 *  Cordic model config register
 */
typedef union {
    struct {
        /** func : R/W; bitpos: [3:0]; default: 0;
         *  Used to inicate the type of operation currently being performed
         */
        uint32_t func:4;
        /** press : R/W; bitpos: [7:4]; default: 0;
         *  Instruct the cordic calculate cycle number
         */
        uint32_t press:4;
        /** scale : R/W; bitpos: [11:8]; default: 0;
         *  The scaling of Input argument
         */
        uint32_t scale:4;
        uint32_t reserved_12:5;
        /** work_mode : R/W; bitpos: [18:17]; default: 0;
         *  Select Cordic Module data transfer Mode,2'd0: reg_in,reg_out, no zero mode, 2'd1:
         *  dma_in, reg_out, no zero mode, 2'd2: dma_loop, 2'd3: reg_in, reg_out, zero mode
         */
        uint32_t work_mode:2;
        /** res_num : R/W; bitpos: [19]; default: 0;
         *  Result number
         */
        uint32_t res_num:1;
        /** arg_num : R/W; bitpos: [20]; default: 0;
         *  Argument number
         */
        uint32_t arg_num:1;
        /** res_size : R/W; bitpos: [21]; default: 0;
         *  Result Size, 0: q1.15, 1: q1.31
         */
        uint32_t res_size:1;
        /** arg_size : R/W; bitpos: [22]; default: 0;
         *  Argument Size, 0: q1.15, 1: q1.31
         */
        uint32_t arg_size:1;
        uint32_t reserved_23:6;
        /** update_flag : R/W/SC; bitpos: [29]; default: 0;
         *  Used to synchronize register data to the computing core and start calculate
         */
        uint32_t update_flag:1;
        /** res_all_rdy_flag : RO/SS/SC; bitpos: [30]; default: 0;
         *  Used to indicate whether the operation all result has been stored in the result
         *  register
         */
        uint32_t res_all_rdy_flag:1;
        /** res_rdy_flag : RO/SS/SC; bitpos: [31]; default: 0;
         *  Used to indicate whether the operation result has been stored in the result register
         */
        uint32_t res_rdy_flag:1;
    };
    uint32_t val;
} cordic_csr_cfg_reg_t;


/** Group: CORDIC ARG1 Configuration */
/** Type of arg1 register
 *  Cordic argument 1 config register
 */
typedef union {
    struct {
        /** arg1_data : R/W; bitpos: [31:0]; default: 0;
         *  First argument data for calculate
         */
        uint32_t arg1_data:32;
    };
    uint32_t val;
} cordic_arg1_reg_t;


/** Group: CORDIC ARG2 Configuration */
/** Type of arg2 register
 *  Cordic argument 2 config register
 */
typedef union {
    struct {
        /** arg2_data : R/W; bitpos: [31:0]; default: 0;
         *  Second argument data for calculate
         */
        uint32_t arg2_data:32;
    };
    uint32_t val;
} cordic_arg2_reg_t;


/** Group: CORDIC RES1 Receive */
/** Type of res1 register
 *  Cordic result 1 receive register
 */
typedef union {
    struct {
        /** res1_data : RO; bitpos: [31:0]; default: 0;
         *  First result data from calculate
         */
        uint32_t res1_data:32;
    };
    uint32_t val;
} cordic_res1_reg_t;


/** Group: CORDIC RES2 Receive */
/** Type of res2 register
 *  Cordic result 2 receive register
 */
typedef union {
    struct {
        /** res2_data : RO; bitpos: [31:0]; default: 0;
         *  Second result data from calculate
         */
        uint32_t res2_data:32;
    };
    uint32_t val;
} cordic_res2_reg_t;


/** Group: CORDIC Interrupt Configuration and Status */
/** Type of io_int_ena register
 *  Cordic interrupt enable signal configuration register.
 */
typedef union {
    struct {
        /** cal_done_int_ena : R/W; bitpos: [0]; default: 0;
         *  Write 1 to enable CAL_DONE_INT.
         */
        uint32_t cal_done_int_ena:1;
        /** dma_all_cal_done_int_ena : R/W; bitpos: [1]; default: 0;
         *  Write 1 to enable DMA_ALL_CAL_DONE_INT.
         */
        uint32_t dma_all_cal_done_int_ena:1;
        /** out_afifo_wovf_int_ena : R/W; bitpos: [2]; default: 0;
         *  Write 1 to enable OUT_AFIFO_WOVF_INT.
         */
        uint32_t out_afifo_wovf_int_ena:1;
        /** in_afifo_rudf_int_ena : R/W; bitpos: [3]; default: 0;
         *  Write 1 to enable IN_AFIFO_RUDF_INT.
         */
        uint32_t in_afifo_rudf_int_ena:1;
        /** arg_num_err_int_ena : R/W; bitpos: [4]; default: 0;
         *  Write 1 to enable ARG_NUM_ERR_INT.
         */
        uint32_t arg_num_err_int_ena:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cordic_io_int_ena_reg_t;

/** Type of io_int_raw register
 *  Cordic interrupt raw signal status register.
 */
typedef union {
    struct {
        /** cal_done_int_raw : R/SS/WTC; bitpos: [0]; default: 0;
         *  The raw interrupt status of CAL_DONE_INT.
         */
        uint32_t cal_done_int_raw:1;
        /** dma_all_cal_done_int_raw : R/SS/WTC; bitpos: [1]; default: 0;
         *  The raw interrupt status of DMA_ALL_CAL_DONE_INT.
         */
        uint32_t dma_all_cal_done_int_raw:1;
        /** out_afifo_wovf_int_raw : R/SS/WTC; bitpos: [2]; default: 0;
         *  The raw interrupt status of OUT_AFIFO_WOVF_INT.
         */
        uint32_t out_afifo_wovf_int_raw:1;
        /** in_afifo_rudf_int_raw : R/SS/WTC; bitpos: [3]; default: 0;
         *  The raw interrupt status of IN_AFIFO_RUDF_INT.
         */
        uint32_t in_afifo_rudf_int_raw:1;
        /** arg_num_err_int_raw : R/SS/WTC; bitpos: [4]; default: 0;
         *  The raw interrupt status of ARG_NUM_ERR_INT.
         */
        uint32_t arg_num_err_int_raw:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cordic_io_int_raw_reg_t;

/** Type of io_int_st register
 *  Cordic interrupt signal status register.
 */
typedef union {
    struct {
        /** cal_done_int_st : RO; bitpos: [0]; default: 0;
         *  The masked interrupt status of CAL_DONE_INT.
         */
        uint32_t cal_done_int_st:1;
        /** dma_all_cal_done_int_st : RO; bitpos: [1]; default: 0;
         *  The masked interrupt status of DMA_ALL_CAL_DONE_INT.
         */
        uint32_t dma_all_cal_done_int_st:1;
        /** out_afifo_wovf_int_st : RO; bitpos: [2]; default: 0;
         *  The masked interrupt status of OUT_AFIFO_WOVF_INT.
         */
        uint32_t out_afifo_wovf_int_st:1;
        /** in_afifo_rudf_int_st : RO; bitpos: [3]; default: 0;
         *  The masked interrupt status of IN_AFIFO_RUDF_INT.
         */
        uint32_t in_afifo_rudf_int_st:1;
        /** arg_num_err_int_st : RO; bitpos: [4]; default: 0;
         *  The masked interrupt status of ARG_NUM_ERR_INT.
         */
        uint32_t arg_num_err_int_st:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cordic_io_int_st_reg_t;

/** Type of io_int_clr register
 *  Cordic interrupt  clear signal configuration register.
 */
typedef union {
    struct {
        /** cal_done_int_clr : WT; bitpos: [0]; default: 0;
         *  Write 1 to clear CAL_DONE_INT.
         */
        uint32_t cal_done_int_clr:1;
        /** dma_all_cal_done_int_clr : WT; bitpos: [1]; default: 0;
         *  Write 1 to clear DMA_ALL_CAL_DONE_INT.
         */
        uint32_t dma_all_cal_done_int_clr:1;
        /** out_afifo_wovf_int_clr : WT; bitpos: [2]; default: 0;
         *  Write 1 to clear OUT_AFIFO_WOVF_INT.
         */
        uint32_t out_afifo_wovf_int_clr:1;
        /** in_afifo_rudf_int_clr : WT; bitpos: [3]; default: 0;
         *  Write 1 to clear IN_AFIFO_RUDF_INT.
         */
        uint32_t in_afifo_rudf_int_clr:1;
        /** arg_num_err_int_clr : WT; bitpos: [4]; default: 0;
         *  Write 1 to clear ARG_NUM_ERR_INT.
         */
        uint32_t arg_num_err_int_clr:1;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} cordic_io_int_clr_reg_t;


/** Group: CORDIC FIFO Configuration */
/** Type of fifo_cfg register
 *  Cordic FIFO configuration register.
 */
typedef union {
    struct {
        /** infifo_reset : WT; bitpos: [0]; default: 0;
         *  infifo_reset_register
         */
        uint32_t infifo_reset:1;
        /** outfifo_reset : WT; bitpos: [1]; default: 0;
         *  outfifo_reset_register
         */
        uint32_t outfifo_reset:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} cordic_fifo_cfg_reg_t;


/** Group: CORDIC Clock Configuration */
/** Type of clk register
 *  Cordic clk configuration register.
 */
typedef union {
    struct {
        uint32_t reserved_0:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  Force clock on for this register file
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} cordic_clk_reg_t;


/** Group: CORDIC Version Register */
/** Type of version register
 *  Version register.
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37818960;
         *  Version of this register file
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} cordic_version_reg_t;


typedef struct {
    volatile cordic_csr_cfg_reg_t csr_cfg;
    volatile cordic_arg1_reg_t arg1;
    volatile cordic_arg2_reg_t arg2;
    volatile cordic_res1_reg_t res1;
    volatile cordic_res2_reg_t res2;
    uint32_t reserved_014[5];
    volatile cordic_io_int_ena_reg_t io_int_ena;
    volatile cordic_io_int_raw_reg_t io_int_raw;
    volatile cordic_io_int_st_reg_t io_int_st;
    volatile cordic_io_int_clr_reg_t io_int_clr;
    volatile cordic_fifo_cfg_reg_t fifo_cfg;
    volatile cordic_clk_reg_t clk;
    volatile cordic_version_reg_t version;
} cordic_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(cordic_dev_t) == 0x44, "Invalid size of cordic_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
