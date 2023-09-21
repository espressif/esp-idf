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
} lp_iomux_clk_en_reg_t;


/** Group: ver_date */
/** Type of ver_date register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_ver_date : R/W; bitpos: [27:0]; default: 2294547;
         *  Reserved
         */
        uint32_t reg_ver_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_iomux_ver_date_reg_t;


/** Group: pad */
/** Type of pad register
 *  Reserved
 */
typedef union {
    struct {
        /** drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t drv:2;
        /** rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t rde:1;
        /** rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t rue:1;
        /** mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t mux_sel:1;
        /** fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t fun_sel:2;
        /** slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t slp_sel:1;
        /** slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t slp_ie:1;
        /** slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t slp_oe:1;
        /** fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
        */
        uint32_t fun_ie:1;
        /** filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad_reg_t;


/** Group: ext_wakeup0_sel */
/** Type of ext_wakeup0_sel register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_xtl_ext_ctr_sel : R/W; bitpos: [4:0]; default: 0;
         *  select LP GPIO 0 ~ 15 to control XTAL
         */
        uint32_t reg_xtl_ext_ctr_sel:5;
        /** reg_ext_wakeup0_sel : R/W; bitpos: [9:5]; default: 0;
         *  Reserved
         */
        uint32_t reg_ext_wakeup0_sel:5;
        uint32_t reserved_10:22;
    };
    uint32_t val;
} lp_iomux_ext_wakeup0_sel_reg_t;


/** Group: lp_pad_hold */
/** Type of lp_pad_hold register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_lp_gpio_hold : R/W; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_lp_gpio_hold:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_iomux_lp_pad_hold_reg_t;


/** Group: lp_pad_hys */
/** Type of lp_pad_hys register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_lp_gpio_hys : R/W; bitpos: [15:0]; default: 0;
         *  Reserved
         */
        uint32_t reg_lp_gpio_hys:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} lp_iomux_lp_pad_hys_reg_t;


typedef struct lp_iomux_dev_t {
    volatile lp_iomux_clk_en_reg_t clk_en;
    volatile lp_iomux_ver_date_reg_t ver_date;
    volatile lp_iomux_pad_reg_t pad[16];
    volatile lp_iomux_ext_wakeup0_sel_reg_t ext_wakeup0_sel;
    volatile lp_iomux_lp_pad_hold_reg_t lp_pad_hold;
    volatile lp_iomux_lp_pad_hys_reg_t lp_pad_hys;
} lp_iomux_dev_t;

extern lp_iomux_dev_t LP_IOMUX;

#ifndef __cplusplus
_Static_assert(sizeof(lp_iomux_dev_t) == 0x54, "Invalid size of lp_iomux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
