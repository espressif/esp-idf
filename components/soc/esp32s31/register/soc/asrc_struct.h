/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/** Group: Control and configuration registers */
/** Type of cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** rs2_stg1_bypass : R/W; bitpos: [0]; default: 1;
         *  Set this bit to bypass stage 1 re-sampler in channel0.
         */
        uint32_t  rs2_stg1_bypass : 1;
        /** rs2_stg0_bypass : R/W; bitpos: [1]; default: 1;
         *  Set this bit to bypass stage 0 re-sampler in channel0.
         */
        uint32_t  rs2_stg0_bypass : 1;
        /** frac_bypass : R/W; bitpos: [2]; default: 1;
         *  Set this bit to bypass fractional re-sampler in channel0.
         */
        uint32_t  frac_bypass : 1;
        /** rs2_stg1_mode : R/W; bitpos: [3]; default: 0;
         *  Write this bit to configure stage 1 re-sampler mode in channel0, 0: interpolation
         *  by  factor of 2, 1: decimation by  factor of 2.
         */
        uint32_t  rs2_stg1_mode : 1;
        /** rs2_stg0_mode : R/W; bitpos: [4]; default: 0;
         *  Write this bit to configure stage 0 re-sampler mode in channel0, 0: interpolation
         *  by  factor of 2, 1: decimation by  factor of 2.
         */
        uint32_t  rs2_stg0_mode : 1;
        /** frac_ahead : R/W; bitpos: [5]; default: 0;
         *  Set this bit to move fraction re-sampler to the first stage in channel0, it should
         *  be 1 when input frequency is higher the output.
         */
        uint32_t  frac_ahead : 1;
        uint32_t  reserved_6 : 1;
        /** mode : R/W; bitpos: [8:7]; default: 0;
         *  Write the bit to configure the channel mode,0: in and out are both mono, 1: in and
         *  out is both dual, 2: in is mono, out is dual, 3, in is dual, out is mono.
         */
        uint32_t  mode : 2;
        /** sel : R/W; bitpos: [9]; default: 0;
         *  Write the bit to configure which 16bits data will be processing.
         */
        uint32_t  sel         : 1;
        uint32_t  reserved_10 : 22;
    };
    uint32_t val;
} asrc_cfg0_reg_t;

/** Type of cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** frac_m : R/W; bitpos: [9:0]; default: 0;
         *  Write the bits to specify the denominator of factor of fraction re-sampler in
         *  channel0, reg_frac_m and reg_frac_l are relatively prime.
         */
        uint32_t  frac_m : 10;
        /** frac_l : R/W; bitpos: [19:10]; default: 0;
         *  Write the bits to specify the nominator of factor of fraction re-sampler in
         *  channel0, reg_frac_l and reg_frac_m are relatively prime.
         */
        uint32_t  frac_l      : 10;
        uint32_t  reserved_20 : 12;
    };
    uint32_t val;
} asrc_cfg1_reg_t;

/** Type of cfg2 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** frac_recipl : R/W; bitpos: [19:0]; default: 0;
         *  Write the bits with ((2^19+L)/(2L)) round down in channel0.
         */
        uint32_t  frac_recipl : 20;
        uint32_t  reserved_20 : 12;
    };
    uint32_t val;
} asrc_cfg2_reg_t;

/** Type of cfg3 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset channel0.
         */
        uint32_t  reset      : 1;
        uint32_t  reserved_1 : 31;
    };
    uint32_t val;
} asrc_cfg3_reg_t;

/** Type of cfg4 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** start : R/W; bitpos: [0]; default: 0;
         *  Set this bit to start channel0.
         */
        uint32_t  start      : 1;
        uint32_t  reserved_1 : 31;
    };
    uint32_t val;
} asrc_cfg4_reg_t;

/** Type of cfg5 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** in_cnt_ena : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable in samples counter.
         */
        uint32_t  in_cnt_ena : 1;
        /** in_cnt_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear in samples counter.
         */
        uint32_t  in_cnt_clr : 1;
        uint32_t  reserved_2 : 6;
        /** in_samples : R/W; bitpos: [31:8]; default: 0;
         *  Write the bits to specify the samples number of data from the DMA
         */
        uint32_t  in_samples : 24;
    };
    uint32_t val;
} asrc_cfg5_reg_t;

/** Type of cfg6 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** out_eof_gen_mode : R/W; bitpos: [1:0]; default: 0;
         *  Write the bits to specify the which eof will be written to DMA. 0:  counter eof, 1:
         *  DMA ineof, 2: both counter eof and DMA ineof, 3 none.
         */
        uint32_t  out_eof_gen_mode : 2;
        /** out_cnt_ena : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable out samples counter.
         */
        uint32_t  out_cnt_ena : 1;
        /** out_cnt_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear out samples counter.
         */
        uint32_t  out_cnt_clr : 1;
        /** out_samples_comp : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable out samples counter compensation when using fractional
         *  re-sampler and decimation by factor of 2 which results in reg_out_cnt >=
         *  reg_out_samples
         */
        uint32_t  out_samples_comp : 1;
        uint32_t  reserved_5       : 3;
        /** out_samples : R/W; bitpos: [31:8]; default: 0;
         *  Write the bits to specify the samples number of data to the DMA, the counter eof
         *  will be set when the counter approaches.
         */
        uint32_t  out_samples : 24;
    };
    uint32_t val;
} asrc_cfg6_reg_t;

/** Type of fifo_ctrl register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** infifo_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset outfifo.
         */
        uint32_t  infifo_reset : 1;
        /** outfifo_reset : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset infifo.
         */
        uint32_t  outfifo_reset : 1;
        uint32_t  reserved_2    : 30;
    };
    uint32_t val;
} asrc_fifo_ctrl_reg_t;

/** Group: Interrupt Register */
/** Type of int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** outcnt_eof_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  This interrupt raw bit turns to high level when the counter approach to
         *  reg_out_samples.
         */
        uint32_t  outcnt_eof_int_raw : 1;
        uint32_t  reserved_1         : 31;
    };
    uint32_t val;
} asrc_int_raw_reg_t;

/** Type of int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** outcnt_eof_int_st : RO; bitpos: [0]; default: 0;
         *  This is the status bit for reg_out_cnt_eof_int_raw when reg_out_cnt_eof_int_ena is
         *  set to 1.
         */
        uint32_t  outcnt_eof_int_st : 1;
        uint32_t  reserved_1        : 31;
    };
    uint32_t val;
} asrc_int_st_reg_t;

/** Type of int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** outcnt_eof_int_ena : R/W; bitpos: [0]; default: 0;
         *  This is the enable bit for reg_out_cnt_eof_int_st register.
         */
        uint32_t  outcnt_eof_int_ena : 1;
        uint32_t  reserved_1         : 31;
    };
    uint32_t val;
} asrc_int_ena_reg_t;

/** Type of int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** outcnt_eof_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the reg_out_cnt_eof_int_raw interrupt.
         */
        uint32_t  outcnt_eof_int_clr : 1;
        uint32_t  reserved_1         : 31;
    };
    uint32_t val;
} asrc_int_clr_reg_t;

/** Group: Status registers */
/** Type of out_cnt register
 *  Status Registers
 */
typedef union {
    struct {
        /** out_cnt : RO; bitpos: [23:0]; default: 0;
         *  Represents the bytes numbers send to the DMA when EOF occurs.
         */
        uint32_t  out_cnt     : 24;
        uint32_t  reserved_24 : 8;
    };
    uint32_t val;
} asrc_out_cnt_reg_t;

/** Group: DEBUG registers */
/** Type of trace1 register
 *  Debug Register1
 */
typedef union {
    struct {
        /** out_inc : RO; bitpos: [23:0]; default: 0;
         *  Represents the samples numbers send to the DMA
         */
        uint32_t  out_inc     : 24;
        uint32_t  reserved_24 : 8;
    };
    uint32_t val;
} asrc_trace1_reg_t;

/** Group: Configuration registers */
/** Type of sys register
 *  Control and configuration samples
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t  clk_en : 1;
        /** chnl0_clk_fo : R/W; bitpos: [1]; default: 0;
         *  Set this bit to make channel0 clock free run.
         */
        uint32_t  chnl0_clk_fo : 1;
        /** chnl1_clk_fo : R/W; bitpos: [2]; default: 0;
         *  Set this bit to make channel1 clock free run.
         */
        uint32_t  chnl1_clk_fo : 1;
        /** chnl0_outfifo_clk_fo : R/W; bitpos: [3]; default: 0;
         *  Set this bit to make channel0 outfifo clock free run.
         */
        uint32_t  chnl0_outfifo_clk_fo : 1;
        /** chnl0_infifo_clk_fo : R/W; bitpos: [4]; default: 0;
         *  Set this bit to make channel0 infifo clock free run.
         */
        uint32_t  chnl0_infifo_clk_fo : 1;
        /** chnl1_outfifo_clk_fo : R/W; bitpos: [5]; default: 0;
         *  Set this bit to make channel1 outfifo clock free run.
         */
        uint32_t  chnl1_outfifo_clk_fo : 1;
        /** chnl1_infifo_clk_fo : R/W; bitpos: [6]; default: 0;
         *  Set this bit to make channel1 infifo clock free run.
         */
        uint32_t  chnl1_infifo_clk_fo : 1;
        uint32_t  reserved_7          : 25;
    };
    uint32_t val;
} asrc_sys_reg_t;

/** Group: Version register */
/** Type of date register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37777984;
         *  Reserved
         */
        uint32_t  date        : 28;
        uint32_t  reserved_28 : 4;
    };
    uint32_t val;
} asrc_date_reg_t;

typedef struct asrc_dev_t {
    struct {
        volatile asrc_cfg0_reg_t       cfg0;
        volatile asrc_cfg1_reg_t       cfg1;
        volatile asrc_cfg2_reg_t       cfg2;
        volatile asrc_cfg3_reg_t       cfg3;
        volatile asrc_cfg4_reg_t       cfg4;
        volatile asrc_cfg5_reg_t       cfg5;
        volatile asrc_cfg6_reg_t       cfg6;
        volatile asrc_fifo_ctrl_reg_t  fifo_ctrl;
        volatile asrc_int_raw_reg_t    int_raw;
        volatile asrc_int_st_reg_t     int_st;
        volatile asrc_int_ena_reg_t    int_ena;
        volatile asrc_int_clr_reg_t    int_clr;
        volatile asrc_out_cnt_reg_t    out_cnt;
        uint32_t                       reserved_32;
        volatile asrc_trace1_reg_t     trace1;
    } asrc_para[2];
    uint32_t                 reserved_078[32];
    volatile asrc_sys_reg_t  sys;
    volatile asrc_date_reg_t date;
} asrc_dev_t;

extern asrc_dev_t ASRC;

#ifndef __cplusplus
_Static_assert(sizeof(asrc_dev_t) == 0x100, "Invalid size of asrc_dev_t structure");
#endif  /* __cplusplus */

#ifdef __cplusplus
}
#endif  /* __cplusplus */
