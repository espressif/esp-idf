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
 *  GPIO output register for GPIO0-31
 */
typedef union {
    struct {
        /** out_data_orig : R/W/SC/WTC; bitpos: [31:0]; default: 0;
         *  GPIO output register for GPIO0-31
         */
        uint32_t out_data_orig:32;
    };
    uint32_t val;
} gpio_out_reg_t;

/** Type of out_w1ts register
 *  GPIO output set register for GPIO0-31
 */
typedef union {
    struct {
        /** out_w1ts : WT; bitpos: [31:0]; default: 0;
         *  GPIO output set register for GPIO0-31
         */
        uint32_t out_w1ts:32;
    };
    uint32_t val;
} gpio_out_w1ts_reg_t;

/** Type of out_w1tc register
 *  GPIO output clear register for GPIO0-31
 */
typedef union {
    struct {
        /** out_w1tc : WT; bitpos: [31:0]; default: 0;
         *  GPIO output clear register for GPIO0-31
         */
        uint32_t out_w1tc:32;
    };
    uint32_t val;
} gpio_out_w1tc_reg_t;

/** Type of out1 register
 *  GPIO output register for GPIO32-56
 */
typedef union {
    struct {
        /** out1_data_orig : R/W/SC/WTC; bitpos: [24:0]; default: 0;
         *  GPIO output register for GPIO32-56
         */
        uint32_t out1_data_orig:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_out1_reg_t;

/** Type of out1_w1ts register
 *  GPIO output set register for GPIO32-56
 */
typedef union {
    struct {
        /** out1_w1ts : WT; bitpos: [24:0]; default: 0;
         *  GPIO output set register for GPIO32-56
         */
        uint32_t out1_w1ts:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_out1_w1ts_reg_t;

/** Type of out1_w1tc register
 *  GPIO output clear register for GPIO32-56
 */
typedef union {
    struct {
        /** out1_w1tc : WT; bitpos: [24:0]; default: 0;
         *  GPIO output clear register for GPIO32-56
         */
        uint32_t out1_w1tc:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_out1_w1tc_reg_t;

/** Type of enable register
 *  GPIO output enable register for GPIO0-31
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [31:0]; default: 0;
         *  GPIO output enable register for GPIO0-31
         */
        uint32_t enable_data:32;
    };
    uint32_t val;
} gpio_enable_reg_t;

/** Type of enable_w1ts register
 *  GPIO output enable set register for GPIO0-31
 */
typedef union {
    struct {
        /** enable_w1ts : WT; bitpos: [31:0]; default: 0;
         *  GPIO output enable set register for GPIO0-31
         */
        uint32_t enable_w1ts:32;
    };
    uint32_t val;
} gpio_enable_w1ts_reg_t;

/** Type of enable_w1tc register
 *  GPIO output enable clear register for GPIO0-31
 */
typedef union {
    struct {
        /** enable_w1tc : WT; bitpos: [31:0]; default: 0;
         *  GPIO output enable clear register for GPIO0-31
         */
        uint32_t enable_w1tc:32;
    };
    uint32_t val;
} gpio_enable_w1tc_reg_t;

/** Type of enable1 register
 *  GPIO output enable register for GPIO32-56
 */
typedef union {
    struct {
        /** enable1_data : R/W/WTC; bitpos: [24:0]; default: 0;
         *  GPIO output enable register for GPIO32-56
         */
        uint32_t enable1_data:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_enable1_reg_t;

/** Type of enable1_w1ts register
 *  GPIO output enable set register for GPIO32-56
 */
typedef union {
    struct {
        /** enable1_w1ts : WT; bitpos: [24:0]; default: 0;
         *  GPIO output enable set register for GPIO32-56
         */
        uint32_t enable1_w1ts:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_enable1_w1ts_reg_t;

/** Type of enable1_w1tc register
 *  GPIO output enable clear register for GPIO32-56
 */
typedef union {
    struct {
        /** enable1_w1tc : WT; bitpos: [24:0]; default: 0;
         *  GPIO output enable clear register for GPIO32-56
         */
        uint32_t enable1_w1tc:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_enable1_w1tc_reg_t;

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
 *  GPIO input register for GPIO0-31
 */
typedef union {
    struct {
        /** in_data_next : RO; bitpos: [31:0]; default: 0;
         *  GPIO input register for GPIO0-31
         */
        uint32_t in_data_next:32;
    };
    uint32_t val;
} gpio_in_reg_t;

/** Type of in1 register
 *  GPIO input register for GPIO32-56
 */
typedef union {
    struct {
        /** in1_data_next : RO; bitpos: [24:0]; default: 0;
         *  GPIO input register for GPIO32-56
         */
        uint32_t in1_data_next:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_in1_reg_t;

/** Type of status register
 *  GPIO interrupt status register for GPIO0-31
 */
typedef union {
    struct {
        /** status_interrupt : R/W/WTC; bitpos: [31:0]; default: 0;
         *  GPIO interrupt status register for GPIO0-31
         */
        uint32_t status_interrupt:32;
    };
    uint32_t val;
} gpio_status_reg_t;

/** Type of status_w1ts register
 *  GPIO interrupt status set register for GPIO0-31
 */
typedef union {
    struct {
        /** status_w1ts : WT; bitpos: [31:0]; default: 0;
         *  GPIO interrupt status set register for GPIO0-31
         */
        uint32_t status_w1ts:32;
    };
    uint32_t val;
} gpio_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  GPIO interrupt status clear register for GPIO0-31
 */
typedef union {
    struct {
        /** status_w1tc : WT; bitpos: [31:0]; default: 0;
         *  GPIO interrupt status clear register for GPIO0-31
         */
        uint32_t status_w1tc:32;
    };
    uint32_t val;
} gpio_status_w1tc_reg_t;

/** Type of status1 register
 *  GPIO interrupt status register for GPIO32-56
 */
typedef union {
    struct {
        /** status1_interrupt : R/W/WTC; bitpos: [24:0]; default: 0;
         *  GPIO interrupt status register for GPIO32-56
         */
        uint32_t status1_interrupt:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_status1_reg_t;

/** Type of status1_w1ts register
 *  GPIO interrupt status set register for GPIO32-56
 */
typedef union {
    struct {
        /** status1_w1ts : WT; bitpos: [24:0]; default: 0;
         *  GPIO interrupt status set register for GPIO32-56
         */
        uint32_t status1_w1ts:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_status1_w1ts_reg_t;

/** Type of status1_w1tc register
 *  GPIO interrupt status clear register for GPIO32-56
 */
typedef union {
    struct {
        /** status1_w1tc : WT; bitpos: [24:0]; default: 0;
         *  GPIO interrupt status clear register for GPIO32-56
         */
        uint32_t status1_w1tc:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_status1_w1tc_reg_t;

/** Type of intr_0 register
 *  GPIO interrupt 0 status register for GPIO0-31
 */
typedef union {
    struct {
        /** int_0 : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt 0 status register for GPIO0-31
         */
        uint32_t int_0:32;
    };
    uint32_t val;
} gpio_intr_0_reg_t;

/** Type of intr1_0 register
 *  GPIO interrupt 0 status register for GPIO32-56
 */
typedef union {
    struct {
        /** int1_0 : RO; bitpos: [24:0]; default: 0;
         *  GPIO interrupt 0 status register for GPIO32-56
         */
        uint32_t int1_0:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_intr1_0_reg_t;

/** Type of intr_1 register
 *  GPIO interrupt 1 status register for GPIO0-31
 */
typedef union {
    struct {
        /** int_1 : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt 1 status register for GPIO0-31
         */
        uint32_t int_1:32;
    };
    uint32_t val;
} gpio_intr_1_reg_t;

/** Type of intr1_1 register
 *  GPIO interrupt 1 status register for GPIO32-56
 */
typedef union {
    struct {
        /** int1_1 : RO; bitpos: [24:0]; default: 0;
         *  GPIO interrupt 1 status register for GPIO32-56
         */
        uint32_t int1_1:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_intr1_1_reg_t;

/** Type of status_next register
 *  GPIO interrupt source register for GPIO0-31
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt source register for GPIO0-31
         */
        uint32_t status_interrupt_next:32;
    };
    uint32_t val;
} gpio_status_next_reg_t;

/** Type of status_next1 register
 *  GPIO interrupt source register for GPIO32-56
 */
typedef union {
    struct {
        /** status_interrupt_next1 : RO; bitpos: [24:0]; default: 0;
         *  GPIO interrupt source register for GPIO32-56
         */
        uint32_t status_interrupt_next1:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_status_next1_reg_t;

/** Type of pin register
 *  GPIO pin configuration register
 */
typedef union {
    struct {
        /** sync2_bypass : R/W; bitpos: [1:0]; default: 0;
         *  set GPIO input_sync2 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t sync2_bypass:2;
        /** pad_driver : R/W; bitpos: [2]; default: 0;
         *  set this bit to select pad driver. 1:open-drain. 0:normal.
         */
        uint32_t pad_driver:1;
        /** sync1_bypass : R/W; bitpos: [4:3]; default: 0;
         *  set GPIO input_sync1 signal mode. 0:disable. 1:trigger at negedge. 2or3:trigger at
         *  posedge.
         */
        uint32_t sync1_bypass:2;
        uint32_t reserved_5:2;
        /** int_type : R/W; bitpos: [9:7]; default: 0;
         *  set this value to choose interrupt mode. 0:disable GPIO interrupt. 1:trigger at
         *  posedge. 2:trigger at negedge. 3:trigger at any edge. 4:valid at low level. 5:valid
         *  at high level
         */
        uint32_t int_type:3;
        /** wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  set this bit to enable GPIO wakeup.(can only wakeup CPU from Light-sleep Mode)
         */
        uint32_t wakeup_enable:1;
        /** config : R/W; bitpos: [12:11]; default: 0;
         *  reserved
         */
        uint32_t config:2;
        /** int_ena : R/W; bitpos: [17:13]; default: 0;
         *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
         *  interrupt.
         */
        uint32_t int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pin_reg_t;

/** Type of func_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t in_sel:6;
        /** in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t in_inv_sel:1;
        /** sig_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func_in_sel_cfg_reg_t;

/** Type of func_out_sel_cfg register
 *  GPIO output function select register
 */
typedef union {
    struct {
        /** out_sel : R/W/SC; bitpos: [8:0]; default: 256;
         *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
         *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
         *  GPIO_OUT_REG[n].
         */
        uint32_t out_sel:9;
        /** out_inv_sel : R/W/SC; bitpos: [9]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t out_inv_sel:1;
        /** oen_sel : R/W; bitpos: [10]; default: 0;
         *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
         *  enable signal.0:use peripheral output enable signal.
         */
        uint32_t oen_sel:1;
        /** oen_inv_sel : R/W; bitpos: [11]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t oen_inv_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} gpio_func_out_sel_cfg_reg_t;

/** Type of intr_2 register
 *  GPIO interrupt 2 status register for GPIO0-31
 */
typedef union {
    struct {
        /** int_2 : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt 2 status register for GPIO0-31
         */
        uint32_t int_2:32;
    };
    uint32_t val;
} gpio_intr_2_reg_t;

/** Type of intr1_2 register
 *  GPIO interrupt 2 status register for GPIO32-56
 */
typedef union {
    struct {
        /** int1_2 : RO; bitpos: [24:0]; default: 0;
         *  GPIO interrupt 2 status register for GPIO32-56
         */
        uint32_t int1_2:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_intr1_2_reg_t;

/** Type of intr_3 register
 *  GPIO interrupt 3 status register for GPIO0-31
 */
typedef union {
    struct {
        /** int_3 : RO; bitpos: [31:0]; default: 0;
         *  GPIO interrupt 3 status register for GPIO0-31
         */
        uint32_t int_3:32;
    };
    uint32_t val;
} gpio_intr_3_reg_t;

/** Type of intr1_3 register
 *  GPIO interrupt 3 status register for GPIO32-56
 */
typedef union {
    struct {
        /** int1_3 : RO; bitpos: [24:0]; default: 0;
         *  GPIO interrupt 3 status register for GPIO32-56
         */
        uint32_t int1_3:25;
        uint32_t reserved_25:7;
    };
    uint32_t val;
} gpio_intr1_3_reg_t;

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

/** Type of zero_det_filter_cnt register
 *  GPIO analog comparator zero detect filter count
 */
typedef union {
    struct {
        /** zero_det_filter_cnt : R/W; bitpos: [31:0]; default: 4294967295;
         *  GPIO analog comparator zero detect filter count
         */
        uint32_t zero_det_filter_cnt:32;
    };
    uint32_t val;
} gpio_zero_det_filter_cnt_reg_t;

/** Type of send_seq register
 *  High speed sdio pad bist send sequence
 */
typedef union {
    struct {
        /** send_seq : R/W; bitpos: [31:0]; default: 305419896;
         *  High speed sdio pad bist send sequence
         */
        uint32_t send_seq:32;
    };
    uint32_t val;
} gpio_send_seq_reg_t;

/** Type of recive_seq register
 *  High speed sdio pad bist recive sequence
 */
typedef union {
    struct {
        /** recive_seq : RO; bitpos: [31:0]; default: 0;
         *  High speed sdio pad bist recive sequence
         */
        uint32_t recive_seq:32;
    };
    uint32_t val;
} gpio_recive_seq_reg_t;

/** Type of bistin_sel register
 *  High speed sdio pad bist in pad sel
 */
typedef union {
    struct {
        /** bistin_sel : R/W; bitpos: [3:0]; default: 15;
         *  High speed sdio pad bist in pad sel 0:pad39, 1: pad40...
         */
        uint32_t bistin_sel:4;
        uint32_t reserved_4:28;
    };
    uint32_t val;
} gpio_bistin_sel_reg_t;

/** Type of bist_ctrl register
 *  High speed sdio pad bist control
 */
typedef union {
    struct {
        /** bist_pad_oe : R/W; bitpos: [0]; default: 1;
         *  High speed sdio pad bist out pad oe
         */
        uint32_t bist_pad_oe:1;
        /** bist_start : WT; bitpos: [1]; default: 0;
         *  High speed sdio pad bist start
         */
        uint32_t bist_start:1;
        uint32_t reserved_2:30;
    };
    uint32_t val;
} gpio_bist_ctrl_reg_t;

/** Type of date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2294787;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_date_reg_t;


/** Group: GPIO INT RAW REG */
/** Type of int_raw register
 *  analog comparator interrupt raw
 */
typedef union {
    struct {
        /** comp0_neg_int_raw : R/WTC/SS; bitpos: [0]; default: 0;
         *  analog comparator pos edge interrupt raw
         */
        uint32_t comp0_neg_int_raw:1;
        /** comp0_pos_int_raw : R/WTC/SS; bitpos: [1]; default: 0;
         *  analog comparator neg edge interrupt raw
         */
        uint32_t comp0_pos_int_raw:1;
        /** comp0_all_int_raw : R/WTC/SS; bitpos: [2]; default: 0;
         *  analog comparator neg or pos edge interrupt raw
         */
        uint32_t comp0_all_int_raw:1;
        /** comp1_neg_int_raw : R/WTC/SS; bitpos: [3]; default: 0;
         *  analog comparator pos edge interrupt raw
         */
        uint32_t comp1_neg_int_raw:1;
        /** comp1_pos_int_raw : R/WTC/SS; bitpos: [4]; default: 0;
         *  analog comparator neg edge interrupt raw
         */
        uint32_t comp1_pos_int_raw:1;
        /** comp1_all_int_raw : R/WTC/SS; bitpos: [5]; default: 0;
         *  analog comparator neg or pos edge interrupt raw
         */
        uint32_t comp1_all_int_raw:1;
        /** bistok_int_raw : R/WTC/SS; bitpos: [6]; default: 0;
         *  pad bistok interrupt raw
         */
        uint32_t bistok_int_raw:1;
        /** bistfail_int_raw : R/WTC/SS; bitpos: [7]; default: 0;
         *  pad bistfail interrupt raw
         */
        uint32_t bistfail_int_raw:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_int_raw_reg_t;


/** Group: GPIO INT ST REG */
/** Type of int_st register
 *  analog comparator interrupt status
 */
typedef union {
    struct {
        /** comp0_neg_int_st : RO; bitpos: [0]; default: 0;
         *  analog comparator pos edge interrupt status
         */
        uint32_t comp0_neg_int_st:1;
        /** comp0_pos_int_st : RO; bitpos: [1]; default: 0;
         *  analog comparator neg edge interrupt status
         */
        uint32_t comp0_pos_int_st:1;
        /** comp0_all_int_st : RO; bitpos: [2]; default: 0;
         *  analog comparator neg or pos edge interrupt status
         */
        uint32_t comp0_all_int_st:1;
        /** comp1_neg_int_st : RO; bitpos: [3]; default: 0;
         *  analog comparator pos edge interrupt status
         */
        uint32_t comp1_neg_int_st:1;
        /** comp1_pos_int_st : RO; bitpos: [4]; default: 0;
         *  analog comparator neg edge interrupt status
         */
        uint32_t comp1_pos_int_st:1;
        /** comp1_all_int_st : RO; bitpos: [5]; default: 0;
         *  analog comparator neg or pos edge interrupt status
         */
        uint32_t comp1_all_int_st:1;
        /** bistok_int_st : RO; bitpos: [6]; default: 0;
         *  pad bistok interrupt status
         */
        uint32_t bistok_int_st:1;
        /** bistfail_int_st : RO; bitpos: [7]; default: 0;
         *  pad bistfail interrupt status
         */
        uint32_t bistfail_int_st:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_int_st_reg_t;


/** Group: GPIO INT ENA REG */
/** Type of int_ena register
 *  analog comparator interrupt enable
 */
typedef union {
    struct {
        /** comp0_neg_int_ena : R/W; bitpos: [0]; default: 1;
         *  analog comparator pos edge interrupt enable
         */
        uint32_t comp0_neg_int_ena:1;
        /** comp0_pos_int_ena : R/W; bitpos: [1]; default: 1;
         *  analog comparator neg edge interrupt enable
         */
        uint32_t comp0_pos_int_ena:1;
        /** comp0_all_int_ena : R/W; bitpos: [2]; default: 1;
         *  analog comparator neg or pos edge interrupt enable
         */
        uint32_t comp0_all_int_ena:1;
        /** comp1_neg_int_ena : R/W; bitpos: [3]; default: 1;
         *  analog comparator pos edge interrupt enable
         */
        uint32_t comp1_neg_int_ena:1;
        /** comp1_pos_int_ena : R/W; bitpos: [4]; default: 1;
         *  analog comparator neg edge interrupt enable
         */
        uint32_t comp1_pos_int_ena:1;
        /** comp1_all_int_ena : R/W; bitpos: [5]; default: 1;
         *  analog comparator neg or pos edge interrupt enable
         */
        uint32_t comp1_all_int_ena:1;
        /** bistok_int_ena : R/W; bitpos: [6]; default: 1;
         *  pad bistok interrupt enable
         */
        uint32_t bistok_int_ena:1;
        /** bistfail_int_ena : R/W; bitpos: [7]; default: 1;
         *  pad bistfail interrupt enable
         */
        uint32_t bistfail_int_ena:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_int_ena_reg_t;


/** Group: GPIO INT CLR REG */
/** Type of int_clr register
 *  analog comparator interrupt clear
 */
typedef union {
    struct {
        /** comp0_neg_int_clr : WT; bitpos: [0]; default: 0;
         *  analog comparator pos edge interrupt clear
         */
        uint32_t comp0_neg_int_clr:1;
        /** comp0_pos_int_clr : WT; bitpos: [1]; default: 0;
         *  analog comparator neg edge interrupt clear
         */
        uint32_t comp0_pos_int_clr:1;
        /** comp0_all_int_clr : WT; bitpos: [2]; default: 0;
         *  analog comparator neg or pos edge interrupt clear
         */
        uint32_t comp0_all_int_clr:1;
        /** comp1_neg_int_clr : WT; bitpos: [3]; default: 0;
         *  analog comparator pos edge interrupt clear
         */
        uint32_t comp1_neg_int_clr:1;
        /** comp1_pos_int_clr : WT; bitpos: [4]; default: 0;
         *  analog comparator neg edge interrupt clear
         */
        uint32_t comp1_pos_int_clr:1;
        /** comp1_all_int_clr : WT; bitpos: [5]; default: 0;
         *  analog comparator neg or pos edge interrupt clear
         */
        uint32_t comp1_all_int_clr:1;
        /** bistok_int_clr : WT; bitpos: [6]; default: 0;
         *  pad bistok interrupt enable
         */
        uint32_t bistok_int_clr:1;
        /** bistfail_int_clr : WT; bitpos: [7]; default: 0;
         *  pad bistfail interrupt enable
         */
        uint32_t bistfail_int_clr:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_int_clr_reg_t;


typedef struct gpio_dev_t {
    volatile gpio_bt_select_reg_t bt_select;
    volatile gpio_out_reg_t out;
    volatile gpio_out_w1ts_reg_t out_w1ts;
    volatile gpio_out_w1tc_reg_t out_w1tc;
    volatile gpio_out1_reg_t out1;
    volatile gpio_out1_w1ts_reg_t out1_w1ts;
    volatile gpio_out1_w1tc_reg_t out1_w1tc;
    uint32_t reserved_01c;
    volatile gpio_enable_reg_t enable;
    volatile gpio_enable_w1ts_reg_t enable_w1ts;
    volatile gpio_enable_w1tc_reg_t enable_w1tc;
    volatile gpio_enable1_reg_t enable1;
    volatile gpio_enable1_w1ts_reg_t enable1_w1ts;
    volatile gpio_enable1_w1tc_reg_t enable1_w1tc;
    volatile gpio_strap_reg_t strap;
    volatile gpio_in_reg_t in;
    volatile gpio_in1_reg_t in1;
    volatile gpio_status_reg_t status;
    volatile gpio_status_w1ts_reg_t status_w1ts;
    volatile gpio_status_w1tc_reg_t status_w1tc;
    volatile gpio_status1_reg_t status1;
    volatile gpio_status1_w1ts_reg_t status1_w1ts;
    volatile gpio_status1_w1tc_reg_t status1_w1tc;
    volatile gpio_intr_0_reg_t intr_0;
    volatile gpio_intr1_0_reg_t intr1_0;
    volatile gpio_intr_1_reg_t intr_1;
    volatile gpio_intr1_1_reg_t intr1_1;
    volatile gpio_status_next_reg_t status_next;
    volatile gpio_status_next1_reg_t status_next1;
    volatile gpio_pin_reg_t pin[57];
    volatile gpio_func_in_sel_cfg_reg_t func_in_sel_cfg[256]; /* func0-func255: reserved for func0, 46, 67, 72, 73, 79, 81, 82, 84, 85, 87, 88, 115, 116, 119-125, 157, 204-213 */
    volatile gpio_func_out_sel_cfg_reg_t func_out_sel_cfg[57];
    volatile gpio_intr_2_reg_t intr_2;
    volatile gpio_intr1_2_reg_t intr1_2;
    volatile gpio_intr_3_reg_t intr_3;
    volatile gpio_intr1_3_reg_t intr1_3;
    volatile gpio_clock_gate_reg_t clock_gate;
    uint32_t reserved_650[44];
    volatile gpio_int_raw_reg_t int_raw;
    volatile gpio_int_st_reg_t int_st;
    volatile gpio_int_ena_reg_t int_ena;
    volatile gpio_int_clr_reg_t int_clr;
    volatile gpio_zero_det_filter_cnt_reg_t zero_det_filter_cnt[2];
    volatile gpio_send_seq_reg_t send_seq;
    volatile gpio_recive_seq_reg_t recive_seq;
    volatile gpio_bistin_sel_reg_t bistin_sel;
    volatile gpio_bist_ctrl_reg_t bist_ctrl;
    uint32_t reserved_728[53];
    volatile gpio_date_reg_t date;
} gpio_dev_t;

extern gpio_dev_t GPIO;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_dev_t) == 0x800, "Invalid size of gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
