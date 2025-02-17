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

/** Group: Control and configuration registers */
/** Type of chnl0_cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_rs2_stg1_bypass : R/W; bitpos: [0]; default: 1;
         *  Set this bit to bypass stage 1 re-sampler in channel0.
         */
        uint32_t chnl0_rs2_stg1_bypass:1;
        /** chnl0_rs2_stg0_bypass : R/W; bitpos: [1]; default: 1;
         *  Set this bit to bypass stage 0 re-sampler in channel0.
         */
        uint32_t chnl0_rs2_stg0_bypass:1;
        /** chnl0_frac_bypass : R/W; bitpos: [2]; default: 1;
         *  Set this bit to bypass fractional re-sampler in channel0.
         */
        uint32_t chnl0_frac_bypass:1;
        /** chnl0_rs2_stg1_mode : R/W; bitpos: [3]; default: 0;
         *  Write this bit to configure stage 1 re-sampler mode in channel0, 0: interpolation
         *  by  factor of 2, 1: decimation by  factor of 2.
         */
        uint32_t chnl0_rs2_stg1_mode:1;
        /** chnl0_rs2_stg0_mode : R/W; bitpos: [4]; default: 0;
         *  Write this bit to configure stage 0 re-sampler mode in channel0, 0: interpolation
         *  by  factor of 2, 1: decimation by  factor of 2.
         */
        uint32_t chnl0_rs2_stg0_mode:1;
        /** chnl0_frac_ahead : R/W; bitpos: [5]; default: 0;
         *  Set this bit to move fraction re-sampler to the first stage in channel0, it should
         *  be 1 when input frequency is higher the output.
         */
        uint32_t chnl0_frac_ahead:1;
        uint32_t reserved_6:1;
        /** chnl0_mode : R/W; bitpos: [8:7]; default: 0;
         *  Write the bit to configure the channel mode,0: in and out are both mono, 1: in and
         *  out is both dual, 2: in is mono, out is dual, 3, in is dual, out is mono.
         */
        uint32_t chnl0_mode:2;
        /** chnl0_sel : R/W; bitpos: [9]; default: 0;
         *  Write the bit to configure which 16bits data will be processing.
         */
        uint32_t chnl0_sel:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} asrc_chnl0_cfg0_reg_t;

/** Type of chnl0_cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_frac_m : R/W; bitpos: [9:0]; default: 0;
         *  Write the bits to specify the denominator of factor of fraction re-sampler in
         *  channel0, reg_chnl0_frac_m and reg_chnl0_frac_l are relatively prime.
         */
        uint32_t chnl0_frac_m:10;
        /** chnl0_frac_l : R/W; bitpos: [19:10]; default: 0;
         *  Write the bits to specify the nominator of factor of fraction re-sampler in
         *  channel0, reg_chnl0_frac_l and reg_chnl0_frac_m are relatively prime.
         */
        uint32_t chnl0_frac_l:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} asrc_chnl0_cfg1_reg_t;

/** Type of chnl0_cfg2 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_frac_recipl : R/W; bitpos: [19:0]; default: 0;
         *  Write the bits with ((2^19+L)/(2L)) round down in channel0.
         */
        uint32_t chnl0_frac_recipl:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} asrc_chnl0_cfg2_reg_t;

/** Type of chnl0_cfg3 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset channel0.
         */
        uint32_t chnl0_reset:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl0_cfg3_reg_t;

/** Type of chnl0_cfg4 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_start : R/W; bitpos: [0]; default: 0;
         *  Set this bit to start channel0.
         */
        uint32_t chnl0_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl0_cfg4_reg_t;

/** Type of chnl0_cfg5 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_in_cnt_ena : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable in data byte counter.
         */
        uint32_t chnl0_in_cnt_ena:1;
        /** chnl0_in_cnt_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear in data byte counter.
         */
        uint32_t chnl0_in_cnt_clr:1;
        uint32_t reserved_2:6;
        /** chnl0_in_len : R/W; bitpos: [31:8]; default: 0;
         *  Write the bits to specify the data byte number of data from the DMA
         */
        uint32_t chnl0_in_len:24;
    };
    uint32_t val;
} asrc_chnl0_cfg5_reg_t;

/** Type of chnl0_cfg6 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_out_eof_gen_mode : R/W; bitpos: [1:0]; default: 0;
         *  Write the bits to specify the which eof will be written to DMA. 0:  counter eof, 1:
         *  DMA ineof, 2: both counter eof and DMA ineof, 3 none.
         */
        uint32_t chnl0_out_eof_gen_mode:2;
        /** chnl0_out_cnt_ena : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable out data byte counter.
         */
        uint32_t chnl0_out_cnt_ena:1;
        /** chnl0_out_cnt_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear out data byte counter.
         */
        uint32_t chnl0_out_cnt_clr:1;
        /** chnl0_out_len_comp : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable out data byte counter compensation when using fractional
         *  re-sampler and decimation by factor of 2 which results in reg_chnl0_out_cnt >=
         *  reg_chnl0_out_len
         */
        uint32_t chnl0_out_len_comp:1;
        uint32_t reserved_5:3;
        /** chnl0_out_len : R/W; bitpos: [31:8]; default: 0;
         *  Write the bits to specify the data byte number of data to the DMA, the counter eof
         *  will be set when the counter approaches.
         */
        uint32_t chnl0_out_len:24;
    };
    uint32_t val;
} asrc_chnl0_cfg6_reg_t;

/** Type of chnl0_fifo_ctrl register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl0_infifo_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset outfifo.
         */
        uint32_t chnl0_infifo_reset:1;
        /** chnl0_outfifo_reset : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset infifo.
         */
        uint32_t chnl0_outfifo_reset:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} asrc_chnl0_fifo_ctrl_reg_t;

/** Type of chnl1_cfg0 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_rs2_stg1_bypass : R/W; bitpos: [0]; default: 1;
         *  Set this bit to bypass stage 1 re-sampler in channel1.
         */
        uint32_t chnl1_rs2_stg1_bypass:1;
        /** chnl1_rs2_stg0_bypass : R/W; bitpos: [1]; default: 1;
         *  Set this bit to bypass stage 0 re-sampler in channel1.
         */
        uint32_t chnl1_rs2_stg0_bypass:1;
        /** chnl1_frac_bypass : R/W; bitpos: [2]; default: 1;
         *  Set this bit to bypass fractional re-sampler in channel1.
         */
        uint32_t chnl1_frac_bypass:1;
        /** chnl1_rs2_stg1_mode : R/W; bitpos: [3]; default: 0;
         *  Write this bit to configure stage 1 re-sampler mode in channel1, 0: interpolation
         *  by  factor of 2, 1: decimation by  factor of 2.
         */
        uint32_t chnl1_rs2_stg1_mode:1;
        /** chnl1_rs2_stg0_mode : R/W; bitpos: [4]; default: 0;
         *  Write this bit to configure stage 0 re-sampler mode in channel1, 0: interpolation
         *  by  factor of 2, 1: decimation by  factor of 2.
         */
        uint32_t chnl1_rs2_stg0_mode:1;
        /** chnl1_frac_ahead : R/W; bitpos: [5]; default: 0;
         *  Set this bit to move fraction re-sampler to the first stage in channel1, it should
         *  be 1 when input frequency is higher the output.
         */
        uint32_t chnl1_frac_ahead:1;
        uint32_t reserved_6:1;
        /** chnl1_mode : R/W; bitpos: [8:7]; default: 0;
         *  Write the bit to configure the channel mode,0: in and out are both mono, 1: in and
         *  out is both dual, 2: in is mono, out is dual, 3, in is dual, out is mono.
         */
        uint32_t chnl1_mode:2;
        /** chnl1_sel : R/W; bitpos: [9]; default: 0;
         *  Write the bit to configure which 16bits data will be processing.
         */
        uint32_t chnl1_sel:1;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} asrc_chnl1_cfg0_reg_t;

/** Type of chnl1_cfg1 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_frac_m : R/W; bitpos: [9:0]; default: 0;
         *  Write the bits to specify the denominator of factor of fraction re-sampler in
         *  channel1, reg_chnl0_frac_m and reg_chnl0_frac_l are relatively prime.
         */
        uint32_t chnl1_frac_m:10;
        /** chnl1_frac_l : R/W; bitpos: [19:10]; default: 0;
         *  Write the bits to specify the nominator of factor of fraction re-sampler in
         *  channel1, reg_chnl0_frac_l and reg_chnl0_frac_m are relatively prime.
         */
        uint32_t chnl1_frac_l:10;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} asrc_chnl1_cfg1_reg_t;

/** Type of chnl1_cfg2 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_frac_recipl : R/W; bitpos: [19:0]; default: 0;
         *  Write the bits with ((2^19+L)/(2L)) round down in channel1.
         */
        uint32_t chnl1_frac_recipl:20;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} asrc_chnl1_cfg2_reg_t;

/** Type of chnl1_cfg3 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset channel1.
         */
        uint32_t chnl1_reset:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl1_cfg3_reg_t;

/** Type of chnl1_cfg4 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_start : R/W; bitpos: [0]; default: 0;
         *  Set this bit to start channel1.
         */
        uint32_t chnl1_start:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl1_cfg4_reg_t;

/** Type of chnl1_cfg5 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_in_cnt_ena : R/W; bitpos: [0]; default: 0;
         *  Set this bit to enable in data byte counter.
         */
        uint32_t chnl1_in_cnt_ena:1;
        /** chnl1_in_cnt_clr : WT; bitpos: [1]; default: 0;
         *  Set this bit to clear in data byte counter.
         */
        uint32_t chnl1_in_cnt_clr:1;
        uint32_t reserved_2:6;
        /** chnl1_in_len : R/W; bitpos: [31:8]; default: 0;
         *  Write the bits to specify the data byte numbers of data from the DMA
         */
        uint32_t chnl1_in_len:24;
    };
    uint32_t val;
} asrc_chnl1_cfg5_reg_t;

/** Type of chnl1_cfg6 register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_out_eof_gen_mode : R/W; bitpos: [1:0]; default: 0;
         *  Write the bits to specify the which eof will be written to DMA. 0:  counter eof, 1:
         *  DMA ineof, 2: both counter eof and DMA ineof, 3 none.
         */
        uint32_t chnl1_out_eof_gen_mode:2;
        /** chnl1_out_cnt_ena : R/W; bitpos: [2]; default: 0;
         *  Set this bit to enable out data byte counter.
         */
        uint32_t chnl1_out_cnt_ena:1;
        /** chnl1_out_cnt_clr : WT; bitpos: [3]; default: 0;
         *  Set this bit to clear out data byte counter.
         */
        uint32_t chnl1_out_cnt_clr:1;
        /** chnl1_out_samples_comp : R/W; bitpos: [4]; default: 0;
         *  Set this bit to enable out data byte counter compensation when using fractional
         *  re-sampler and decimation by factor of 2 which results in reg_chnl1_out_cnt >=
         *  reg_chnl1_out_len
         */
        uint32_t chnl1_out_samples_comp:1;
        uint32_t reserved_5:3;
        /** chnl1_out_len : R/W; bitpos: [31:8]; default: 0;
         *  Write the bits to specify the data byte number of data to the DMA, the counter eof
         *  will be set when the counter approaches.
         */
        uint32_t chnl1_out_len:24;
    };
    uint32_t val;
} asrc_chnl1_cfg6_reg_t;

/** Type of chnl1_fifo_ctrl register
 *  Control and configuration registers
 */
typedef union {
    struct {
        /** chnl1_infifo_reset : WT; bitpos: [0]; default: 0;
         *  Set this bit to reset outfifo.
         */
        uint32_t chnl1_infifo_reset:1;
        /** chnl1_outfifo_reset : WT; bitpos: [1]; default: 0;
         *  Set this bit to reset infifo.
         */
        uint32_t chnl1_outfifo_reset:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} asrc_chnl1_fifo_ctrl_reg_t;


/** Group: Interrupt Register */
/** Type of chnl0_int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** chnl0_outcnt_eof_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  This interrupt raw bit turns to high level when the counter approach to reg_out_len.
         */
        uint32_t chnl0_outcnt_eof_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl0_int_raw_reg_t;

/** Type of chnl0_int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** chnl0_outcnt_eof_int_st : RO; bitpos: [0]; default: 0;
         *  This is the status bit for reg_out_cnt_eof_int_raw when reg_out_cnt_eof_int_ena is
         *  set to 1.
         */
        uint32_t chnl0_outcnt_eof_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl0_int_st_reg_t;

/** Type of chnl0_int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** chnl0_outcnt_eof_int_ena : R/W; bitpos: [0]; default: 0;
         *  This is the enable bit for reg_out_cnt_eof_int_st register.
         */
        uint32_t chnl0_outcnt_eof_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl0_int_ena_reg_t;

/** Type of chnl0_int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** chnl0_outcnt_eof_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the reg_out_cnt_eof_int_raw interrupt.
         */
        uint32_t chnl0_outcnt_eof_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl0_int_clr_reg_t;

/** Type of chnl1_int_raw register
 *  Raw interrupt status
 */
typedef union {
    struct {
        /** chnl1_outcnt_eof_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  This interrupt raw bit turns to high level when the counter approach to reg_out_len.
         */
        uint32_t chnl1_outcnt_eof_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl1_int_raw_reg_t;

/** Type of chnl1_int_st register
 *  Masked interrupt status
 */
typedef union {
    struct {
        /** chnl1_outcnt_eof_int_st : RO; bitpos: [0]; default: 0;
         *  This is the status bit for reg_out_cnt_eof_int_raw when reg_out_cnt_eof_int_ena is
         *  set to 1.
         */
        uint32_t chnl1_outcnt_eof_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl1_int_st_reg_t;

/** Type of chnl1_int_ena register
 *  Interrupt enable bits
 */
typedef union {
    struct {
        /** chnl1_outcnt_eof_int_ena : R/W; bitpos: [0]; default: 0;
         *  This is the enable bit for reg_out_cnt_eof_int_st register.
         */
        uint32_t chnl1_outcnt_eof_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl1_int_ena_reg_t;

/** Type of chnl1_int_clr register
 *  Interrupt clear bits
 */
typedef union {
    struct {
        /** chnl1_outcnt_eof_int_clr : WT; bitpos: [0]; default: 0;
         *  Set this bit to clear the reg_out_cnt_eof_int_raw interrupt.
         */
        uint32_t chnl1_outcnt_eof_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} asrc_chnl1_int_clr_reg_t;


/** Group: Status registers */
/** Type of chnl0_out_cnt register
 *  Status Registers
 */
typedef union {
    struct {
        /** chnl0_out_cnt : RO; bitpos: [23:0]; default: 0;
         *  Represents the bytes numbers send to the DMA when EOF occurs.
         */
        uint32_t chnl0_out_cnt:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} asrc_chnl0_out_cnt_reg_t;

/** Type of chnl1_out_cnt register
 *  Status Registers
 */
typedef union {
    struct {
        /** chnl1_out_cnt : RO; bitpos: [23:0]; default: 0;
         *  Represents the data byte numbers send to the DMA when EOF occurs.
         */
        uint32_t chnl1_out_cnt:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} asrc_chnl1_out_cnt_reg_t;


/** Group: DEBUG registers */
/** Type of chnl0_trace1 register
 *  Debug Register1
 */
typedef union {
    struct {
        /** chnl0_out_inc : RO; bitpos: [23:0]; default: 0;
         *  Represents the samples numbers send to the DMA
         */
        uint32_t chnl0_out_inc:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} asrc_chnl0_trace1_reg_t;

/** Type of chnl1_trace1 register
 *  Debug Register1
 */
typedef union {
    struct {
        /** chnl1_out_inc : RO; bitpos: [23:0]; default: 0;
         *  Represents the data byte numbers send to the DMA
         */
        uint32_t chnl1_out_inc:24;
        uint32_t reserved_24:8;
    };
    uint32_t val;
} asrc_chnl1_trace1_reg_t;


/** Group: Configuration registers */
/** Type of sys register
 *  Control and configuration
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t clk_en:1;
        /** chnl0_clk_fo : R/W; bitpos: [1]; default: 0;
         *  Set this bit to make channel0 clock free run.
         */
        uint32_t chnl0_clk_fo:1;
        /** chnl1_clk_fo : R/W; bitpos: [2]; default: 0;
         *  Set this bit to make channel1 clock free run.
         */
        uint32_t chnl1_clk_fo:1;
        /** chnl0_outfifo_clk_fo : R/W; bitpos: [3]; default: 0;
         *  Set this bit to make channel0 outfifo clock free run.
         */
        uint32_t chnl0_outfifo_clk_fo:1;
        /** chnl0_infifo_clk_fo : R/W; bitpos: [4]; default: 0;
         *  Set this bit to make channel0 infifo clock free run.
         */
        uint32_t chnl0_infifo_clk_fo:1;
        /** chnl1_outfifo_clk_fo : R/W; bitpos: [5]; default: 0;
         *  Set this bit to make channel1 outfifo clock free run.
         */
        uint32_t chnl1_outfifo_clk_fo:1;
        /** chnl1_infifo_clk_fo : R/W; bitpos: [6]; default: 0;
         *  Set this bit to make channel1 infifo clock free run.
         */
        uint32_t chnl1_infifo_clk_fo:1;
        uint32_t reserved_7:25;
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
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} asrc_date_reg_t;


typedef struct {
    volatile asrc_chnl0_cfg0_reg_t chnl0_cfg0;
    volatile asrc_chnl0_cfg1_reg_t chnl0_cfg1;
    volatile asrc_chnl0_cfg2_reg_t chnl0_cfg2;
    volatile asrc_chnl0_cfg3_reg_t chnl0_cfg3;
    volatile asrc_chnl0_cfg4_reg_t chnl0_cfg4;
    volatile asrc_chnl0_cfg5_reg_t chnl0_cfg5;
    volatile asrc_chnl0_cfg6_reg_t chnl0_cfg6;
    volatile asrc_chnl0_fifo_ctrl_reg_t chnl0_fifo_ctrl;
    volatile asrc_chnl0_int_raw_reg_t chnl0_int_raw;
    volatile asrc_chnl0_int_st_reg_t chnl0_int_st;
    volatile asrc_chnl0_int_ena_reg_t chnl0_int_ena;
    volatile asrc_chnl0_int_clr_reg_t chnl0_int_clr;
    volatile asrc_chnl0_out_cnt_reg_t chnl0_out_cnt;
    uint32_t reserved_034;
    volatile asrc_chnl0_trace1_reg_t chnl0_trace1;
    volatile asrc_chnl1_cfg0_reg_t chnl1_cfg0;
    volatile asrc_chnl1_cfg1_reg_t chnl1_cfg1;
    volatile asrc_chnl1_cfg2_reg_t chnl1_cfg2;
    volatile asrc_chnl1_cfg3_reg_t chnl1_cfg3;
    volatile asrc_chnl1_cfg4_reg_t chnl1_cfg4;
    volatile asrc_chnl1_cfg5_reg_t chnl1_cfg5;
    volatile asrc_chnl1_cfg6_reg_t chnl1_cfg6;
    volatile asrc_chnl1_fifo_ctrl_reg_t chnl1_fifo_ctrl;
    volatile asrc_chnl1_int_raw_reg_t chnl1_int_raw;
    volatile asrc_chnl1_int_st_reg_t chnl1_int_st;
    volatile asrc_chnl1_int_ena_reg_t chnl1_int_ena;
    volatile asrc_chnl1_int_clr_reg_t chnl1_int_clr;
    volatile asrc_chnl1_out_cnt_reg_t chnl1_out_cnt;
    uint32_t reserved_070;
    volatile asrc_chnl1_trace1_reg_t chnl1_trace1;
    uint32_t reserved_078[32];
    volatile asrc_sys_reg_t sys;
    volatile asrc_date_reg_t date;
} asrc_dev_t;

extern asrc_dev_t SAMPLE_RATE_CONVERTER;

#ifndef __cplusplus
_Static_assert(sizeof(asrc_dev_t) == 0x100, "Invalid size of asrc_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
