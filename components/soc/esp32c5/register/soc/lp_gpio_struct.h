/**
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configuration register */
/** Type of out register
 *  GPIO output register for GPIO0-6
 */
typedef union {
    struct {
        /** out_data_orig : R/W/WTC; bitpos: [6:0]; default: 0;
         *  GPIO output register for GPIO0-6
         */
        uint32_t out_data_orig:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_out_reg_t;

/** Type of out_w1ts register
 *  GPIO output set register for GPIO0-6
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [6:0]; default: 0;
         *  GPIO output set register for GPIO0-6
         */
        uint32_t out_w1ts:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  GPIO output clear register for GPIO0-6
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [6:0]; default: 0;
         *  GPIO output clear register for GPIO0-6
         */
        uint32_t out_w1tc:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_out_w1tc_reg_t;

/** Type of enable register
 *  GPIO output enable register for GPIO0-6
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [6:0]; default: 0;
         *  GPIO output enable register for GPIO0-6
         */
        uint32_t enable_data:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  GPIO output enable set register for GPIO0-6
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [6:0]; default: 0;
         *  GPIO output enable set register for GPIO0-6
         */
        uint32_t enable_w1ts:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  GPIO output enable clear register for GPIO0-6
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [6:0]; default: 0;
         *  GPIO output enable clear register for GPIO0-6
         */
        uint32_t enable_w1tc:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_enable_w1tc_reg_t;

/** Type of in register
 *  GPIO input register for GPIO0-6
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [6:0]; default: 0;
         *  GPIO input register for GPIO0-6
         */
        uint32_t in_data_next:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_in_reg_t;

/** Type of status register
 *  GPIO interrupt status register for GPIO0-6
 */
typedef union {
    struct {
        /** status_interrupt : R/W/WTC; bitpos: [6:0]; default: 0;
         *  GPIO interrupt status register for GPIO0-6
         */
        uint32_t status_interrupt:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_reg_t;

/** Type of status_w1ts register
 *  GPIO interrupt status set register for GPIO0-6
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [6:0]; default: 0;
         *  GPIO interrupt status set register for GPIO0-6
         */
        uint32_t status_w1ts:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  GPIO interrupt status clear register for GPIO0-6
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [6:0]; default: 0;
         *  GPIO interrupt status clear register for GPIO0-6
         */
        uint32_t status_w1tc:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_w1tc_reg_t;

/** Type of status_next register
 *  GPIO interrupt source register for GPIO0-6
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [6:0]; default: 0;
         *  GPIO interrupt source register for GPIO0-6
         */
        uint32_t status_interrupt_next:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_gpio_status_next_reg_t;

/** Type of pinn register
 *  GPIO pin configuration register
 */
typedef union {
    struct {
        /** pinn_sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t pinn_sync2_bypass:2;
        /** pinn_pad_driver : R/W; bitpos: [2]; default: 0;
         *  set this bit to select pad driver. 1:open-drain. 0:normal.
         */
        uint32_t pinn_pad_driver:1;
        /** pinn_sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t pinn_sync1_bypass:2;
        /** pinn_edge_wakeup_clr : WT; bitpos: [5]; default: 0;
         *  GPIO wakeup clear register.
         */
        uint32_t pinn_edge_wakeup_clr:1;
        uint32_t reserved_6:1;
        /** pinn_int_type : R/W; bitpos: [9:7]; default: 0;
         *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
         *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
         *  at high level
         */
        uint32_t pinn_int_type:3;
        /** pinn_wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
         */
        uint32_t pinn_wakeup_enable:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} lp_gpio_pinn_reg_t;

/** Type of funcn_out_sel_cfg register
 *  GPIO output function select register
 */
typedef union {
    struct {
        /** funcn_out_inv_sel : R/W; bitpos: [0]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t funcn_out_inv_sel:1;
        uint32_t reserved_1:1;
        /** funcn_oe_inv_sel : R/W; bitpos: [2]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t funcn_oe_inv_sel:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} lp_gpio_funcn_out_sel_cfg_reg_t;

/** Type of clock_gate register
 *  GPIO clock gate register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 1;
         *  set this bit to enable GPIO clock gate
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} lp_gpio_clock_gate_reg_t;

/** Type of date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37814832;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_gpio_date_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile lp_gpio_out_reg_t out;
    volatile lp_gpio_out_w1ts_reg_t out_w1ts;
    volatile lp_gpio_out_w1tc_reg_t out_w1tc;
    volatile lp_gpio_enable_reg_t enable;
    volatile lp_gpio_enable_w1ts_reg_t enable_w1ts;
    volatile lp_gpio_enable_w1tc_reg_t enable_w1tc;
    volatile lp_gpio_in_reg_t in;
    volatile lp_gpio_status_reg_t status;
    volatile lp_gpio_status_w1ts_reg_t status_w1ts;
    volatile lp_gpio_status_w1tc_reg_t status_w1tc;
    volatile lp_gpio_status_next_reg_t status_next;
    volatile lp_gpio_pinn_reg_t pinn[7];
    uint32_t reserved_04c[153];
    volatile lp_gpio_funcn_out_sel_cfg_reg_t funcn_out_sel_cfg[7];
    uint32_t reserved_2cc[75];
    volatile lp_gpio_clock_gate_reg_t clock_gate;
    volatile lp_gpio_date_reg_t date;
} lp_gpio_dev_t;

extern lp_gpio_dev_t LP_GPIO;

#ifndef __cplusplus
_Static_assert(sizeof(lp_gpio_dev_t) == 0x400, "Invalid size of lp_gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
