/**
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: clk_en */
/** Type of clk_en register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_clk_en : R/W; bitpos: [0]; default: 1;
         *  Reserved
         */
        uint32_t reg_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_gpio_clk_en_reg_t;


/** Group: ver_date */
/** Type of ver_date register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ver_date : R/W; bitpos: [27:0]; default: 2294563;
         *  Reserved
         */
        uint32_t reg_ver_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_gpio_ver_date_reg_t;


/** Group: out */
/** Type of out register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_out_data : R/W/WTC; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_out_data:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_out_reg_t;


/** Group: out_w1ts */
/** Type of out_w1ts register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_out_data_w1ts : WT; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_out_data_w1ts:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_out_w1ts_reg_t;


/** Group: out_w1tc */
/** Type of out_w1tc register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_out_data_w1tc : WT; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_out_data_w1tc:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_out_w1tc_reg_t;


/** Group: enable */
/** Type of enable register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_enable_data : R/W/WTC; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_enable_data:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_enable_reg_t;


/** Group: enable_w1ts */
/** Type of enable_w1ts register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_enable_data_w1ts : WT; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_enable_data_w1ts:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_enable_w1ts_reg_t;


/** Group: enable_w1tc */
/** Type of enable_w1tc register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_enable_data_w1tc : WT; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_enable_data_w1tc:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_enable_w1tc_reg_t;


/** Group: status */
/** Type of status register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_status_data : R/W/WTC; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_status_data:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_status_reg_t;


/** Group: status_w1ts */
/** Type of status_w1ts register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_status_data_w1ts : WT; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_status_data_w1ts:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_status_w1ts_reg_t;


/** Group: status_w1tc */
/** Type of status_w1tc register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_status_data_w1tc : WT; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_status_data_w1tc:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_status_w1tc_reg_t;


/** Group: in */
/** Type of status_next register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_status_interrupt_next : RO; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_status_interrupt_next:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_status_next_reg_t;

/** Type of in register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_in_data_next : RO; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_in_data_next:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_gpio_in_reg_t;


/** Group: pin0 */
/** Type of pin0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin0_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin0_wakeup_enable:1;
        /** reg_gpio_pin0_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin0_int_type:3;
        /** reg_gpio_pin0_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin0_pad_driver:1;
        /** reg_gpio_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpio_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin0_reg_t;


/** Group: pin1 */
/** Type of pin1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin1_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin1_wakeup_enable:1;
        /** reg_gpio_pin1_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin1_int_type:3;
        /** reg_gpio_pin1_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin1_pad_driver:1;
        /** reg_gpi1_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi1_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin1_reg_t;


/** Group: pin2 */
/** Type of pin2 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin2_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin2_wakeup_enable:1;
        /** reg_gpio_pin2_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin2_int_type:3;
        /** reg_gpio_pin2_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin2_pad_driver:1;
        /** reg_gpi2_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi2_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin2_reg_t;


/** Group: pin3 */
/** Type of pin3 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin3_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin3_wakeup_enable:1;
        /** reg_gpio_pin3_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin3_int_type:3;
        /** reg_gpio_pin3_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin3_pad_driver:1;
        /** reg_gpi3_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi3_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin3_reg_t;


/** Group: pin4 */
/** Type of pin4 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin4_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin4_wakeup_enable:1;
        /** reg_gpio_pin4_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin4_int_type:3;
        /** reg_gpio_pin4_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin4_pad_driver:1;
        /** reg_gpi4_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi4_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin4_reg_t;


/** Group: pin5 */
/** Type of pin5 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin5_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin5_wakeup_enable:1;
        /** reg_gpio_pin5_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin5_int_type:3;
        /** reg_gpio_pin5_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin5_pad_driver:1;
        /** reg_gpi5_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi5_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin5_reg_t;


/** Group: pin6 */
/** Type of pin6 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin6_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin6_wakeup_enable:1;
        /** reg_gpio_pin6_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin6_int_type:3;
        /** reg_gpio_pin6_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin6_pad_driver:1;
        /** reg_gpi6_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi6_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin6_reg_t;


/** Group: pin7 */
/** Type of pin7 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin7_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin7_wakeup_enable:1;
        /** reg_gpio_pin7_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin7_int_type:3;
        /** reg_gpio_pin7_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin7_pad_driver:1;
        /** reg_gpi7_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi7_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin7_reg_t;


/** Group: pin8 */
/** Type of pin8 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin8_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin8_wakeup_enable:1;
        /** reg_gpio_pin8_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin8_int_type:3;
        /** reg_gpio_pin8_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin8_pad_driver:1;
        /** reg_gpi8_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi8_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin8_reg_t;


/** Group: pin9 */
/** Type of pin9 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin9_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin9_wakeup_enable:1;
        /** reg_gpio_pin9_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin9_int_type:3;
        /** reg_gpio_pin9_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin9_pad_driver:1;
        /** reg_gpi9_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi9_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin9_reg_t;


/** Group: pin10 */
/** Type of pin10 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin10_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin10_wakeup_enable:1;
        /** reg_gpio_pin10_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin10_int_type:3;
        /** reg_gpio_pin10_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin10_pad_driver:1;
        /** reg_gpi10_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi10_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin10_reg_t;


/** Group: pin11 */
/** Type of pin11 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin11_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin11_wakeup_enable:1;
        /** reg_gpio_pin11_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin11_int_type:3;
        /** reg_gpio_pin11_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin11_pad_driver:1;
        /** reg_gpi11_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi11_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin11_reg_t;


/** Group: pin12 */
/** Type of pin12 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin12_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin12_wakeup_enable:1;
        /** reg_gpio_pin12_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin12_int_type:3;
        /** reg_gpio_pin12_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin12_pad_driver:1;
        /** reg_gpi12_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi12_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin12_reg_t;


/** Group: pin13 */
/** Type of pin13 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin13_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin13_wakeup_enable:1;
        /** reg_gpio_pin13_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin13_int_type:3;
        /** reg_gpio_pin13_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin13_pad_driver:1;
        /** reg_gpi13_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi13_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin13_reg_t;


/** Group: pin14 */
/** Type of pin14 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin14_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin14_wakeup_enable:1;
        /** reg_gpio_pin14_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin14_int_type:3;
        /** reg_gpio_pin14_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin14_pad_driver:1;
        /** reg_gpi14_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi14_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin14_reg_t;


/** Group: pin15 */
/** Type of pin15 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_pin15_wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin15_wakeup_enable:1;
        /** reg_gpio_pin15_int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin15_int_type:3;
        /** reg_gpio_pin15_pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_pin15_pad_driver:1;
        /** reg_gpi15_pin0_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t reg_gpi15_pin0_edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin15_reg_t;


/** Group: func0_in_sel_cfg */
/** Type of func0_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func0_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func0_in_inv_sel:1;
        /** reg_gpio_sig0_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig0_in_sel:1;
        /** reg_gpio_func0_in_sel : R/W; bitpos: [7:2]; default: 48;
         *  reg_gpio_func0_in_sel[5:4]==2'b11->constant
         *  1,reg_gpio_func0_in_sel[5:4]==2'b10->constant 0
         */
        uint32_t reg_gpio_func0_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func0_in_sel_cfg_reg_t;


/** Group: func1_in_sel_cfg */
/** Type of func1_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func1_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func1_in_inv_sel:1;
        /** reg_gpio_sig1_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig1_in_sel:1;
        /** reg_gpio_func1_in_sel : R/W; bitpos: [7:2]; default: 48;
         *  Reserved
         */
        uint32_t reg_gpio_func1_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func1_in_sel_cfg_reg_t;


/** Group: func2_in_sel_cfg */
/** Type of func2_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func2_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func2_in_inv_sel:1;
        /** reg_gpio_sig2_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig2_in_sel:1;
        /** reg_gpio_func2_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func2_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func2_in_sel_cfg_reg_t;


/** Group: func3_in_sel_cfg */
/** Type of func3_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func3_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func3_in_inv_sel:1;
        /** reg_gpio_sig3_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig3_in_sel:1;
        /** reg_gpio_func3_in_sel : R/W; bitpos: [7:2]; default: 48;
         *  Reserved
         */
        uint32_t reg_gpio_func3_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func3_in_sel_cfg_reg_t;


/** Group: func4_in_sel_cfg */
/** Type of func4_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func4_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func4_in_inv_sel:1;
        /** reg_gpio_sig4_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig4_in_sel:1;
        /** reg_gpio_func4_in_sel : R/W; bitpos: [7:2]; default: 48;
         *  Reserved
         */
        uint32_t reg_gpio_func4_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func4_in_sel_cfg_reg_t;


/** Group: func5_in_sel_cfg */
/** Type of func5_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func5_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func5_in_inv_sel:1;
        /** reg_gpio_sig5_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig5_in_sel:1;
        /** reg_gpio_func5_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func5_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func5_in_sel_cfg_reg_t;


/** Group: func6_in_sel_cfg */
/** Type of func6_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func6_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func6_in_inv_sel:1;
        /** reg_gpio_sig6_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig6_in_sel:1;
        /** reg_gpio_func6_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func6_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func6_in_sel_cfg_reg_t;


/** Group: func7_in_sel_cfg */
/** Type of func7_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func7_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func7_in_inv_sel:1;
        /** reg_gpio_sig7_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig7_in_sel:1;
        /** reg_gpio_func7_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func7_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func7_in_sel_cfg_reg_t;


/** Group: func8_in_sel_cfg */
/** Type of func8_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func8_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func8_in_inv_sel:1;
        /** reg_gpio_sig8_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig8_in_sel:1;
        /** reg_gpio_func8_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func8_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func8_in_sel_cfg_reg_t;


/** Group: func9_in_sel_cfg */
/** Type of func9_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func9_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func9_in_inv_sel:1;
        /** reg_gpio_sig9_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig9_in_sel:1;
        /** reg_gpio_func9_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func9_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func9_in_sel_cfg_reg_t;


/** Group: func10_in_sel_cfg */
/** Type of func10_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func10_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func10_in_inv_sel:1;
        /** reg_gpio_sig10_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig10_in_sel:1;
        /** reg_gpio_func10_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func10_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func10_in_sel_cfg_reg_t;


/** Group: func11_in_sel_cfg */
/** Type of func11_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func11_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func11_in_inv_sel:1;
        /** reg_gpio_sig11_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig11_in_sel:1;
        /** reg_gpio_func11_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func11_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func11_in_sel_cfg_reg_t;


/** Group: func12_in_sel_cfg */
/** Type of func12_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func12_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func12_in_inv_sel:1;
        /** reg_gpio_sig12_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig12_in_sel:1;
        /** reg_gpio_func12_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func12_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func12_in_sel_cfg_reg_t;


/** Group: func13_in_sel_cfg */
/** Type of func13_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func13_in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func13_in_inv_sel:1;
        /** reg_gpio_sig13_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_sig13_in_sel:1;
        /** reg_gpio_func13_in_sel : R/W; bitpos: [7:2]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func13_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func13_in_sel_cfg_reg_t;


/** Group: func0_out_sel_cfg */
/** Type of func0_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func0_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func0_oe_inv_sel:1;
        /** reg_gpio_func0_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func0_oe_sel:1;
        /** reg_gpio_func0_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func0_out_inv_sel:1;
        /** reg_gpio_func0_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  reg_gpio_func0_out_sel[5:1]==16 -> output gpio register value to pad
         */
        uint32_t reg_gpio_func0_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func0_out_sel_cfg_reg_t;


/** Group: func1_out_sel_cfg */
/** Type of func1_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func1_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func1_oe_inv_sel:1;
        /** reg_gpio_func1_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func1_oe_sel:1;
        /** reg_gpio_func1_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func1_out_inv_sel:1;
        /** reg_gpio_func1_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func1_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func1_out_sel_cfg_reg_t;


/** Group: func2_out_sel_cfg */
/** Type of func2_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func2_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func2_oe_inv_sel:1;
        /** reg_gpio_func2_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func2_oe_sel:1;
        /** reg_gpio_func2_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func2_out_inv_sel:1;
        /** reg_gpio_func2_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func2_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func2_out_sel_cfg_reg_t;


/** Group: func3_out_sel_cfg */
/** Type of func3_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func3_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func3_oe_inv_sel:1;
        /** reg_gpio_func3_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func3_oe_sel:1;
        /** reg_gpio_func3_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func3_out_inv_sel:1;
        /** reg_gpio_func3_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func3_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func3_out_sel_cfg_reg_t;


/** Group: func4_out_sel_cfg */
/** Type of func4_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func4_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func4_oe_inv_sel:1;
        /** reg_gpio_func4_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func4_oe_sel:1;
        /** reg_gpio_func4_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func4_out_inv_sel:1;
        /** reg_gpio_func4_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func4_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func4_out_sel_cfg_reg_t;


/** Group: func5_out_sel_cfg */
/** Type of func5_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func5_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func5_oe_inv_sel:1;
        /** reg_gpio_func5_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func5_oe_sel:1;
        /** reg_gpio_func5_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func5_out_inv_sel:1;
        /** reg_gpio_func5_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func5_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func5_out_sel_cfg_reg_t;


/** Group: func6_out_sel_cfg */
/** Type of func6_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func6_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func6_oe_inv_sel:1;
        /** reg_gpio_func6_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func6_oe_sel:1;
        /** reg_gpio_func6_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func6_out_inv_sel:1;
        /** reg_gpio_func6_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func6_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func6_out_sel_cfg_reg_t;


/** Group: func7_out_sel_cfg */
/** Type of func7_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func7_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func7_oe_inv_sel:1;
        /** reg_gpio_func7_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func7_oe_sel:1;
        /** reg_gpio_func7_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func7_out_inv_sel:1;
        /** reg_gpio_func7_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func7_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func7_out_sel_cfg_reg_t;


/** Group: func8_out_sel_cfg */
/** Type of func8_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func8_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func8_oe_inv_sel:1;
        /** reg_gpio_func8_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func8_oe_sel:1;
        /** reg_gpio_func8_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func8_out_inv_sel:1;
        /** reg_gpio_func8_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func8_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func8_out_sel_cfg_reg_t;


/** Group: func9_out_sel_cfg */
/** Type of func9_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func9_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func9_oe_inv_sel:1;
        /** reg_gpio_func9_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func9_oe_sel:1;
        /** reg_gpio_func9_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func9_out_inv_sel:1;
        /** reg_gpio_func9_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func9_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func9_out_sel_cfg_reg_t;


/** Group: func10_out_sel_cfg */
/** Type of func10_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func10_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func10_oe_inv_sel:1;
        /** reg_gpio_func10_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func10_oe_sel:1;
        /** reg_gpio_func10_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func10_out_inv_sel:1;
        /** reg_gpio_func10_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func10_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func10_out_sel_cfg_reg_t;


/** Group: func11_out_sel_cfg */
/** Type of func11_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func11_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func11_oe_inv_sel:1;
        /** reg_gpio_func11_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func11_oe_sel:1;
        /** reg_gpio_func11_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func11_out_inv_sel:1;
        /** reg_gpio_func11_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func11_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func11_out_sel_cfg_reg_t;


/** Group: func12_out_sel_cfg */
/** Type of func12_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func12_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func12_oe_inv_sel:1;
        /** reg_gpio_func12_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func12_oe_sel:1;
        /** reg_gpio_func12_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func12_out_inv_sel:1;
        /** reg_gpio_func12_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func12_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func12_out_sel_cfg_reg_t;


/** Group: func13_out_sel_cfg */
/** Type of func13_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func13_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func13_oe_inv_sel:1;
        /** reg_gpio_func13_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func13_oe_sel:1;
        /** reg_gpio_func13_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func13_out_inv_sel:1;
        /** reg_gpio_func13_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func13_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func13_out_sel_cfg_reg_t;


/** Group: func14_out_sel_cfg */
/** Type of func14_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func14_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func14_oe_inv_sel:1;
        /** reg_gpio_func14_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func14_oe_sel:1;
        /** reg_gpio_func14_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func14_out_inv_sel:1;
        /** reg_gpio_func14_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func14_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func14_out_sel_cfg_reg_t;


/** Group: func15_out_sel_cfg */
/** Type of func15_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_gpio_func15_oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func15_oe_inv_sel:1;
        /** reg_gpio_func15_oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func15_oe_sel:1;
        /** reg_gpio_func15_out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_gpio_func15_out_inv_sel:1;
        /** reg_gpio_func15_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  Reserved
         */
        uint32_t reg_gpio_func15_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func15_out_sel_cfg_reg_t;


typedef struct lp_gpio_dev_t {
    volatile lp_gpio_clk_en_reg_t clk_en;
    volatile lp_gpio_ver_date_reg_t ver_date;
    volatile lp_gpio_out_reg_t out;
    volatile lp_gpio_out_w1ts_reg_t out_w1ts;
    volatile lp_gpio_out_w1tc_reg_t out_w1tc;
    volatile lp_gpio_enable_reg_t enable;
    volatile lp_gpio_enable_w1ts_reg_t enable_w1ts;
    volatile lp_gpio_enable_w1tc_reg_t enable_w1tc;
    volatile lp_gpio_status_reg_t status;
    volatile lp_gpio_status_w1ts_reg_t status_w1ts;
    volatile lp_gpio_status_w1tc_reg_t status_w1tc;
    volatile lp_gpio_status_next_reg_t status_next;
    volatile lp_gpio_in_reg_t in;
    volatile lp_gpio_pin0_reg_t pin0;
    volatile lp_gpio_pin1_reg_t pin1;
    volatile lp_gpio_pin2_reg_t pin2;
    volatile lp_gpio_pin3_reg_t pin3;
    volatile lp_gpio_pin4_reg_t pin4;
    volatile lp_gpio_pin5_reg_t pin5;
    volatile lp_gpio_pin6_reg_t pin6;
    volatile lp_gpio_pin7_reg_t pin7;
    volatile lp_gpio_pin8_reg_t pin8;
    volatile lp_gpio_pin9_reg_t pin9;
    volatile lp_gpio_pin10_reg_t pin10;
    volatile lp_gpio_pin11_reg_t pin11;
    volatile lp_gpio_pin12_reg_t pin12;
    volatile lp_gpio_pin13_reg_t pin13;
    volatile lp_gpio_pin14_reg_t pin14;
    volatile lp_gpio_pin15_reg_t pin15;
    volatile lp_gpio_func0_in_sel_cfg_reg_t func0_in_sel_cfg;
    volatile lp_gpio_func1_in_sel_cfg_reg_t func1_in_sel_cfg;
    volatile lp_gpio_func2_in_sel_cfg_reg_t func2_in_sel_cfg;
    volatile lp_gpio_func3_in_sel_cfg_reg_t func3_in_sel_cfg;
    volatile lp_gpio_func4_in_sel_cfg_reg_t func4_in_sel_cfg;
    volatile lp_gpio_func5_in_sel_cfg_reg_t func5_in_sel_cfg;
    volatile lp_gpio_func6_in_sel_cfg_reg_t func6_in_sel_cfg;
    volatile lp_gpio_func7_in_sel_cfg_reg_t func7_in_sel_cfg;
    volatile lp_gpio_func8_in_sel_cfg_reg_t func8_in_sel_cfg;
    volatile lp_gpio_func9_in_sel_cfg_reg_t func9_in_sel_cfg;
    volatile lp_gpio_func10_in_sel_cfg_reg_t func10_in_sel_cfg;
    volatile lp_gpio_func11_in_sel_cfg_reg_t func11_in_sel_cfg;
    volatile lp_gpio_func12_in_sel_cfg_reg_t func12_in_sel_cfg;
    volatile lp_gpio_func13_in_sel_cfg_reg_t func13_in_sel_cfg;
    uint32_t reserved_0ac[18];
    volatile lp_gpio_func0_out_sel_cfg_reg_t func0_out_sel_cfg;
    volatile lp_gpio_func1_out_sel_cfg_reg_t func1_out_sel_cfg;
    volatile lp_gpio_func2_out_sel_cfg_reg_t func2_out_sel_cfg;
    volatile lp_gpio_func3_out_sel_cfg_reg_t func3_out_sel_cfg;
    volatile lp_gpio_func4_out_sel_cfg_reg_t func4_out_sel_cfg;
    volatile lp_gpio_func5_out_sel_cfg_reg_t func5_out_sel_cfg;
    volatile lp_gpio_func6_out_sel_cfg_reg_t func6_out_sel_cfg;
    volatile lp_gpio_func7_out_sel_cfg_reg_t func7_out_sel_cfg;
    volatile lp_gpio_func8_out_sel_cfg_reg_t func8_out_sel_cfg;
    volatile lp_gpio_func9_out_sel_cfg_reg_t func9_out_sel_cfg;
    volatile lp_gpio_func10_out_sel_cfg_reg_t func10_out_sel_cfg;
    volatile lp_gpio_func11_out_sel_cfg_reg_t func11_out_sel_cfg;
    volatile lp_gpio_func12_out_sel_cfg_reg_t func12_out_sel_cfg;
    volatile lp_gpio_func13_out_sel_cfg_reg_t func13_out_sel_cfg;
    volatile lp_gpio_func14_out_sel_cfg_reg_t func14_out_sel_cfg;
    volatile lp_gpio_func15_out_sel_cfg_reg_t func15_out_sel_cfg;
} lp_gpio_dev_t;

extern lp_gpio_dev_t LP_GPIO;


#ifndef __cplusplus
_Static_assert(sizeof(lp_gpio_dev_t) == 0x134, "Invalid size of lp_gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
