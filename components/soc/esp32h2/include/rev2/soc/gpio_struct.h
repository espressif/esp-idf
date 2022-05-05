/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configuration register */
/** Type of bt_select register
 *  GPIO bit select register
 */
typedef union {
    struct {
        /** bt_sel : R/W; bitpos: [31:0]; default: 0;
         *  GPIO bit select register
         */
        uint32_t bt_sel:32;
    };
    uint32_t val;
} gpio_bt_select_reg_t;

/** Type of out register
 *  GPIO output register for GPIO0-29
 */
typedef union {
    struct {
        /** out_data_orig : R/W/WS/WC; bitpos: [29:0]; default: 0;
         *  GPIO output register for GPIO0-29
         */
        uint32_t out_data_orig:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_out_reg_t;

/** Type of out_w1ts register
 *  GPIO output set register for GPIO0-29
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [29:0]; default: 0;
         *  GPIO output set register for GPIO0-29
         */
        uint32_t out_w1ts:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  GPIO output clear register for GPIO0-29
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [29:0]; default: 0;
         *  GPIO output clear register for GPIO0-29
         */
        uint32_t out_w1tc:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_out_w1tc_reg_t;

/** Type of sdio_select register
 *  GPIO sdio select register
 */
typedef union {
    struct {
        /** sdio_sel : R/W; bitpos: [7:0]; default: 0;
         *  GPIO sdio select register
         */
        uint32_t sdio_sel:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_sdio_select_reg_t;

/** Type of enable register
 *  GPIO output enable register for GPIO0-29
 */
typedef union {
    struct {
        /** enable_data : R/W/SS; bitpos: [29:0]; default: 0;
         *  GPIO output enable register for GPIO0-29
         */
        uint32_t enable_data:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  GPIO output enable set register for GPIO0-29
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [29:0]; default: 0;
         *  GPIO output enable set register for GPIO0-29
         */
        uint32_t enable_w1ts:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  GPIO output enable clear register for GPIO0-29
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [29:0]; default: 0;
         *  GPIO output enable clear register for GPIO0-29
         */
        uint32_t enable_w1tc:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_enable_w1tc_reg_t;

/** Type of strap register
 *  pad strapping register
 */
typedef union {
    struct {
        /** strapping : RO; bitpos: [15:0]; default: 0;
         *  pad strapping register
         */
        uint32_t strapping:16;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gpio_strap_reg_t;

/** Type of in register
 *  GPIO input register for GPIO0-29
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [29:0]; default: 0;
         *  GPIO input register for GPIO0-29
         */
        uint32_t in_data_next:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_in_reg_t;

/** Type of status register
 *  GPIO interrupt status register for GPIO0-29
 */
typedef union {
    struct {
        /** status_interrupt : R/W/SS; bitpos: [29:0]; default: 0;
         *  GPIO interrupt status register for GPIO0-29
         */
        uint32_t status_interrupt:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_status_reg_t;

/** Type of status_w1ts register
 *  GPIO interrupt status set register for GPIO0-29
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [29:0]; default: 0;
         *  GPIO interrupt status set register for GPIO0-29
         */
        uint32_t status_w1ts:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  GPIO interrupt status clear register for GPIO0-29
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [29:0]; default: 0;
         *  GPIO interrupt status clear register for GPIO0-29
         */
        uint32_t status_w1tc:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_status_w1tc_reg_t;

/** Type of pin register
 *  GPIO pin configuration register
 */
typedef union {
    struct {
        /** pin_sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t pin_sync2_bypass:2;
        /** pin_pad_driver : R/W; bitpos: [2]; default: 0;
         *  set this bit to select pad driver. 1:open-drain. 0:normal.
         */
        uint32_t pin_pad_driver:1;
        /** pin_sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t pin_sync1_bypass:2;
        uint32_t reserved_5:2;
        /** pin_int_type : R/W; bitpos: [9:7]; default: 0;
         *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
         *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
         *  at high level
         */
        uint32_t pin_int_type:3;
        /** pin_wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
         */
        uint32_t pin_wakeup_enable:1;
        /** pin_config : R/W; bitpos: [12:11]; default: 0;
         *  reserved
         */
        uint32_t pin_config:2;
        /** pin_int_ena : R/W; bitpos: [17:13]; default: 0;
         *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
         *  interrupt.
         */
        uint32_t pin_int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pin_reg_t;

/** Type of status_next register
 *  GPIO interrupt source register for GPIO0-29
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt source register for GPIO0-29
         */
        uint32_t status_interrupt_next:32;
    };
    uint32_t val;
} gpio_status_next_reg_t;

/** Type of func_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func_in_sel : R/W; bitpos: [4:0]; default: 0;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x38: set this port always
         *  high level. s=0x3C: set this port always low level.
         */
        uint32_t func_in_sel:5;
        /** func_in_inv_sel : R/W; bitpos: [5]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func_in_inv_sel:1;
        /** sig_in_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig_in_sel:1;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gpio_func_in_sel_cfg_reg_t;

/** Type of func_out_sel_cfg register
 *  GPIO output function select register
 */
typedef union {
    struct {
        /** func_out_sel : R/W; bitpos: [7:0]; default: 128;
         *  The value of the bits: 0<=s<=128. Set the value to select output signal. s=0-127:
         *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
         *  GPIO_OUT_REG[n].
         */
        uint32_t func_out_sel:8;
        /** func_out_inv_sel : R/W; bitpos: [8]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t func_out_inv_sel:1;
        /** func_oen_sel : R/W; bitpos: [9]; default: 0;
         *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
         *  enable signal.0:use peripheral output enable signal.
         */
        uint32_t func_oen_sel:1;
        /** func_oen_inv_sel : R/W; bitpos: [10]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t func_oen_inv_sel:1;
        uint32_t reserved_11:21;
    };
    uint32_t val;
} gpio_func_out_sel_cfg_reg_t;

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
} gpio_clock_gate_reg_t;

/** Type of reg_date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 34640016;
         *  version register
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_reg_date_reg_t;


/** Group: interrupt register */
/** Type of pcpu_int register
 *  GPIO PRO_CPU interrupt status register for GPIO0-29
 */
typedef union {
    struct {
        /** procpu_int : RO; bitpos: [29:0]; default: 0;
         *  GPIO PRO_CPU interrupt status register for GPIO0-29
         */
        uint32_t procpu_int:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_pcpu_int_reg_t;

/** Type of pcpu_nmi_int register
 *  GPIO PRO_CPU(not shielded) interrupt status register for GPIO0-29
 */
typedef union {
    struct {
        /** procpu_nmi_int : RO; bitpos: [29:0]; default: 0;
         *  GPIO PRO_CPU(not shielded) interrupt status register for GPIO0-29
         */
        uint32_t procpu_nmi_int:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_pcpu_nmi_int_reg_t;

/** Type of cpusdio_int register
 *  GPIO CPUSDIO interrupt status register for GPIO0-29
 */
typedef union {
    struct {
        /** sdio_int : RO; bitpos: [29:0]; default: 0;
         *  GPIO CPUSDIO interrupt status register for GPIO0-29
         */
        uint32_t sdio_int:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_cpusdio_int_reg_t;


typedef struct {
    volatile gpio_bt_select_reg_t bt_select;
    volatile gpio_out_reg_t out;
    volatile gpio_out_w1ts_reg_t out_w1ts;
    volatile gpio_out_w1tc_reg_t out_w1tc;
    uint32_t reserved_010[3];
    volatile gpio_sdio_select_reg_t sdio_select;
    volatile gpio_enable_reg_t enable;
    volatile gpio_enable_w1ts_reg_t enable_w1ts;
    volatile gpio_enable_w1tc_reg_t enable_w1tc;
    uint32_t reserved_02c[3];
    volatile gpio_strap_reg_t strap;
    volatile gpio_in_reg_t in;
    uint32_t reserved_040;
    volatile gpio_status_reg_t status;
    volatile gpio_status_w1ts_reg_t status_w1ts;
    volatile gpio_status_w1tc_reg_t status_w1tc;
    uint32_t reserved_050[3];
    volatile gpio_pcpu_int_reg_t pcpu_int;
    volatile gpio_pcpu_nmi_int_reg_t pcpu_nmi_int;
    volatile gpio_cpusdio_int_reg_t cpusdio_int;
    uint32_t reserved_068[3];
    volatile gpio_pin_reg_t pin[30];
    uint32_t reserved_0ec[24];
    volatile gpio_status_next_reg_t status_next;
    uint32_t reserved_150;
    volatile gpio_func_in_sel_cfg_reg_t func_in_sel_cfg[128];
    uint32_t reserved_354[128];
    volatile gpio_func_out_sel_cfg_reg_t func_out_sel_cfg[30];
    uint32_t reserved_5cc[24];
    volatile gpio_clock_gate_reg_t clock_gate;
    uint32_t reserved_630[51];
    volatile gpio_reg_date_reg_t reg_date;
} gpio_dev_t;

extern gpio_dev_t GPIO;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_dev_t) == 0x700, "Invalid size of gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
