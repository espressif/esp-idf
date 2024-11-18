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


/** Group: pin */
/** Type of pin register
 *  Reserved
 */
typedef union {
    struct {
        /** wakeup_enable : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t wakeup_enable:1;
        /** int_type : R/W; bitpos: [3:1]; default: 0;
         *  Reserved
         */
        uint32_t int_type:3;
        /** pad_driver : R/W; bitpos: [4]; default: 0;
         *  Reserved
         */
        uint32_t pad_driver:1;
        /** edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  need des
         */
        uint32_t edge_wakeup_clr:1;
        uint32_t reserved_6:26;
    };
    uint32_t val;
} lp_gpio_pin_reg_t;


/** Group: func_in_sel_cfg */
/** Type of func_in_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** in_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t in_inv_sel:1;
        /** sig_in_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t sig_in_sel:1;
        /** func_in_sel : R/W; bitpos: [7:2]; default: 48 (for func0/1/3/4) 32 (for the rest);
         *  func_in_sel[5:4]==2'b11 (s=0x30) -> constant 1
         *  func_in_sel[5:4]==2'b10 (s=0x20) -> constant 0
         */
        uint32_t func_in_sel:6;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_gpio_func_in_sel_cfg_reg_t;


/** Group: func_out_sel_cfg */
/** Type of func0_out_sel_cfg register
 *  Reserved
 */
typedef union {
    struct {
        /** oe_inv_sel : R/W; bitpos: [0]; default: 0;
         *  Reserved
         */
        uint32_t oe_inv_sel:1;
        /** oe_sel : R/W; bitpos: [1]; default: 0;
         *  Reserved
         */
        uint32_t oe_sel:1;
        /** out_inv_sel : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t out_inv_sel:1;
        /** func_out_sel : R/W; bitpos: [8:3]; default: 32;
         *  func_out_sel[5:1]==16 (s=32) -> output gpio register value to pad
         */
        uint32_t func_out_sel:6;
        uint32_t reserved_9:23;
    };
    uint32_t val;
} lp_gpio_func_out_sel_cfg_reg_t;


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
    volatile lp_gpio_pin_reg_t pin[16];
    volatile lp_gpio_func_in_sel_cfg_reg_t func_in_sel_cfg[14];
    uint32_t reserved_0ac[18];
    volatile lp_gpio_func_out_sel_cfg_reg_t func_out_sel_cfg[16];
} lp_gpio_dev_t;

extern lp_gpio_dev_t LP_GPIO;


#ifndef __cplusplus
_Static_assert(sizeof(lp_gpio_dev_t) == 0x134, "Invalid size of lp_gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
