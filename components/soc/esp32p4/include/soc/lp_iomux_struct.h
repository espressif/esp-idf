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


/** Group: pad0 */
/** Type of pad0 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad0_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad0_drv:2;
        /** reg_pad0_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad0_rde:1;
        /** reg_pad0_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad0_rue:1;
        /** reg_pad0_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad0_mux_sel:1;
        /** reg_pad0_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad0_fun_sel:2;
        /** reg_pad0_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad0_slp_sel:1;
        /** reg_pad0_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad0_slp_ie:1;
        /** reg_pad0_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad0_slp_oe:1;
        /** reg_pad0_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad0_fun_ie:1;
        /** reg_pad0_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad0_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad0_reg_t;


/** Group: pad1 */
/** Type of pad1 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad1_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad1_drv:2;
        /** reg_pad1_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad1_rde:1;
        /** reg_pad1_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad1_rue:1;
        /** reg_pad1_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad1_mux_sel:1;
        /** reg_pad1_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad1_fun_sel:2;
        /** reg_pad1_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad1_slp_sel:1;
        /** reg_pad1_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad1_slp_ie:1;
        /** reg_pad1_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad1_slp_oe:1;
        /** reg_pad1_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad1_fun_ie:1;
        /** reg_pad1_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad1_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad1_reg_t;


/** Group: pad2 */
/** Type of pad2 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad2_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad2_drv:2;
        /** reg_pad2_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad2_rde:1;
        /** reg_pad2_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad2_rue:1;
        /** reg_pad2_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad2_mux_sel:1;
        /** reg_pad2_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad2_fun_sel:2;
        /** reg_pad2_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad2_slp_sel:1;
        /** reg_pad2_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad2_slp_ie:1;
        /** reg_pad2_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad2_slp_oe:1;
        /** reg_pad2_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad2_fun_ie:1;
        /** reg_pad2_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad2_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad2_reg_t;


/** Group: pad3 */
/** Type of pad3 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad3_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad3_drv:2;
        /** reg_pad3_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad3_rde:1;
        /** reg_pad3_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad3_rue:1;
        /** reg_pad3_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad3_mux_sel:1;
        /** reg_pad3_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad3_fun_sel:2;
        /** reg_pad3_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad3_slp_sel:1;
        /** reg_pad3_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad3_slp_ie:1;
        /** reg_pad3_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad3_slp_oe:1;
        /** reg_pad3_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad3_fun_ie:1;
        /** reg_pad3_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad3_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad3_reg_t;


/** Group: pad4 */
/** Type of pad4 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad4_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad4_drv:2;
        /** reg_pad4_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad4_rde:1;
        /** reg_pad4_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad4_rue:1;
        /** reg_pad4_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad4_mux_sel:1;
        /** reg_pad4_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad4_fun_sel:2;
        /** reg_pad4_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad4_slp_sel:1;
        /** reg_pad4_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad4_slp_ie:1;
        /** reg_pad4_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad4_slp_oe:1;
        /** reg_pad4_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad4_fun_ie:1;
        /** reg_pad4_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad4_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad4_reg_t;


/** Group: pad5 */
/** Type of pad5 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad5_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad5_drv:2;
        /** reg_pad5_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad5_rde:1;
        /** reg_pad5_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad5_rue:1;
        /** reg_pad5_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad5_mux_sel:1;
        /** reg_pad5_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad5_fun_sel:2;
        /** reg_pad5_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad5_slp_sel:1;
        /** reg_pad5_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad5_slp_ie:1;
        /** reg_pad5_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad5_slp_oe:1;
        /** reg_pad5_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad5_fun_ie:1;
        /** reg_pad5_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad5_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad5_reg_t;


/** Group: pad6 */
/** Type of pad6 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad6_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad6_drv:2;
        /** reg_pad6_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad6_rde:1;
        /** reg_pad6_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad6_rue:1;
        /** reg_pad6_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad6_mux_sel:1;
        /** reg_pad6_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad6_fun_sel:2;
        /** reg_pad6_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad6_slp_sel:1;
        /** reg_pad6_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad6_slp_ie:1;
        /** reg_pad6_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad6_slp_oe:1;
        /** reg_pad6_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad6_fun_ie:1;
        /** reg_pad6_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad6_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad6_reg_t;


/** Group: pad7 */
/** Type of pad7 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad7_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad7_drv:2;
        /** reg_pad7_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad7_rde:1;
        /** reg_pad7_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad7_rue:1;
        /** reg_pad7_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad7_mux_sel:1;
        /** reg_pad7_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad7_fun_sel:2;
        /** reg_pad7_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad7_slp_sel:1;
        /** reg_pad7_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad7_slp_ie:1;
        /** reg_pad7_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad7_slp_oe:1;
        /** reg_pad7_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad7_fun_ie:1;
        /** reg_pad7_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad7_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad7_reg_t;


/** Group: pad8 */
/** Type of pad8 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad8_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad8_drv:2;
        /** reg_pad8_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad8_rde:1;
        /** reg_pad8_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad8_rue:1;
        /** reg_pad8_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad8_mux_sel:1;
        /** reg_pad8_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad8_fun_sel:2;
        /** reg_pad8_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad8_slp_sel:1;
        /** reg_pad8_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad8_slp_ie:1;
        /** reg_pad8_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad8_slp_oe:1;
        /** reg_pad8_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad8_fun_ie:1;
        /** reg_pad8_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad8_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad8_reg_t;


/** Group: pad9 */
/** Type of pad9 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad9_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad9_drv:2;
        /** reg_pad9_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad9_rde:1;
        /** reg_pad9_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad9_rue:1;
        /** reg_pad9_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad9_mux_sel:1;
        /** reg_pad9_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad9_fun_sel:2;
        /** reg_pad9_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad9_slp_sel:1;
        /** reg_pad9_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad9_slp_ie:1;
        /** reg_pad9_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad9_slp_oe:1;
        /** reg_pad9_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad9_fun_ie:1;
        /** reg_pad9_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad9_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad9_reg_t;


/** Group: pad10 */
/** Type of pad10 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad10_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad10_drv:2;
        /** reg_pad10_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad10_rde:1;
        /** reg_pad10_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad10_rue:1;
        /** reg_pad10_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad10_mux_sel:1;
        /** reg_pad10_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad10_fun_sel:2;
        /** reg_pad10_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad10_slp_sel:1;
        /** reg_pad10_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad10_slp_ie:1;
        /** reg_pad10_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad10_slp_oe:1;
        /** reg_pad10_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad10_fun_ie:1;
        /** reg_pad10_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad10_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad10_reg_t;


/** Group: pad11 */
/** Type of pad11 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad11_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad11_drv:2;
        /** reg_pad11_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad11_rde:1;
        /** reg_pad11_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad11_rue:1;
        /** reg_pad11_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad11_mux_sel:1;
        /** reg_pad11_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad11_fun_sel:2;
        /** reg_pad11_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad11_slp_sel:1;
        /** reg_pad11_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad11_slp_ie:1;
        /** reg_pad11_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad11_slp_oe:1;
        /** reg_pad11_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad11_fun_ie:1;
        /** reg_pad11_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad11_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad11_reg_t;


/** Group: pad12 */
/** Type of pad120 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad12_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad12_drv:2;
        /** reg_pad12_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad12_rde:1;
        /** reg_pad12_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad12_rue:1;
        /** reg_pad12_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad12_mux_sel:1;
        /** reg_pad12_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad12_fun_sel:2;
        /** reg_pad12_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad12_slp_sel:1;
        /** reg_pad12_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad12_slp_ie:1;
        /** reg_pad12_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad12_slp_oe:1;
        /** reg_pad12_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad12_fun_ie:1;
        /** reg_pad12_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad12_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad120_reg_t;


/** Group: pad13 */
/** Type of pad13 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad13_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad13_drv:2;
        /** reg_pad13_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad13_rde:1;
        /** reg_pad13_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad13_rue:1;
        /** reg_pad13_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad13_mux_sel:1;
        /** reg_pad13_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad13_fun_sel:2;
        /** reg_pad13_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad13_slp_sel:1;
        /** reg_pad13_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad13_slp_ie:1;
        /** reg_pad13_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad13_slp_oe:1;
        /** reg_pad13_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad13_fun_ie:1;
        /** reg_pad13_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad13_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad13_reg_t;


/** Group: pad14 */
/** Type of pad14 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad14_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad14_drv:2;
        /** reg_pad14_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad14_rde:1;
        /** reg_pad14_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad14_rue:1;
        /** reg_pad14_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad14_mux_sel:1;
        /** reg_pad14_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad14_fun_sel:2;
        /** reg_pad14_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad14_slp_sel:1;
        /** reg_pad14_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad14_slp_ie:1;
        /** reg_pad14_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad14_slp_oe:1;
        /** reg_pad14_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad14_fun_ie:1;
        /** reg_pad14_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad14_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad14_reg_t;


/** Group: pad15 */
/** Type of pad15 register
 *  Reserved
 */
typedef union {
    struct {
        /** reg_pad15_drv : R/W; bitpos: [1:0]; default: 2;
         *  Reserved
         */
        uint32_t reg_pad15_drv:2;
        /** reg_pad15_rde : R/W; bitpos: [2]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad15_rde:1;
        /** reg_pad15_rue : R/W; bitpos: [3]; default: 0;
         *  Reserved
         */
        uint32_t reg_pad15_rue:1;
        /** reg_pad15_mux_sel : R/W; bitpos: [4]; default: 0;
         *  1:use LP GPIO,0: use digital GPIO
         */
        uint32_t reg_pad15_mux_sel:1;
        /** reg_pad15_fun_sel : R/W; bitpos: [6:5]; default: 0;
         *  function sel
         */
        uint32_t reg_pad15_fun_sel:2;
        /** reg_pad15_slp_sel : R/W; bitpos: [7]; default: 0;
         *  1: enable sleep mode during sleep,0: no sleep mode
         */
        uint32_t reg_pad15_slp_sel:1;
        /** reg_pad15_slp_ie : R/W; bitpos: [8]; default: 0;
         *  input enable in sleep mode
         */
        uint32_t reg_pad15_slp_ie:1;
        /** reg_pad15_slp_oe : R/W; bitpos: [9]; default: 0;
         *  output enable in sleep mode
         */
        uint32_t reg_pad15_slp_oe:1;
        /** reg_pad15_fun_ie : R/W; bitpos: [10]; default: 0;
         *  input enable in work mode
         */
        uint32_t reg_pad15_fun_ie:1;
        /** reg_pad15_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t reg_pad15_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_iomux_pad15_reg_t;


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
    volatile lp_iomux_pad0_reg_t pad0;
    volatile lp_iomux_pad1_reg_t pad1;
    volatile lp_iomux_pad2_reg_t pad2;
    volatile lp_iomux_pad3_reg_t pad3;
    volatile lp_iomux_pad4_reg_t pad4;
    volatile lp_iomux_pad5_reg_t pad5;
    volatile lp_iomux_pad6_reg_t pad6;
    volatile lp_iomux_pad7_reg_t pad7;
    volatile lp_iomux_pad8_reg_t pad8;
    volatile lp_iomux_pad9_reg_t pad9;
    volatile lp_iomux_pad10_reg_t pad10;
    volatile lp_iomux_pad11_reg_t pad11;
    volatile lp_iomux_pad120_reg_t pad120;
    volatile lp_iomux_pad13_reg_t pad13;
    volatile lp_iomux_pad14_reg_t pad14;
    volatile lp_iomux_pad15_reg_t pad15;
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
