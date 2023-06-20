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
        uint32_t reserved_5:2;
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
        /** pinn_config : R/W; bitpos: [12:11]; default: 0;
         *  reserved
         */
        uint32_t pinn_config:2;
        /** pinn_int_ena : R/W; bitpos: [17:13]; default: 0;
         *  set bit 13 to enable CPU interrupt. set bit 14 to enable CPU(not shielded)
         *  interrupt.
         */
        uint32_t pinn_int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pinn_reg_t;

/** Type of func1_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func1_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func1_in_sel:6;
        /** func1_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func1_in_inv_sel:1;
        /** sig1_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig1_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func1_in_sel_cfg_reg_t;

/** Type of func2_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func2_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func2_in_sel:6;
        /** func2_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func2_in_inv_sel:1;
        /** sig2_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig2_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func2_in_sel_cfg_reg_t;

/** Type of func3_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func3_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func3_in_sel:6;
        /** func3_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func3_in_inv_sel:1;
        /** sig3_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig3_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func3_in_sel_cfg_reg_t;

/** Type of func4_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func4_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func4_in_sel:6;
        /** func4_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func4_in_inv_sel:1;
        /** sig4_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig4_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func4_in_sel_cfg_reg_t;

/** Type of func5_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func5_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func5_in_sel:6;
        /** func5_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func5_in_inv_sel:1;
        /** sig5_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig5_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func5_in_sel_cfg_reg_t;

/** Type of func6_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func6_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func6_in_sel:6;
        /** func6_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func6_in_inv_sel:1;
        /** sig6_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig6_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func6_in_sel_cfg_reg_t;

/** Type of func7_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func7_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func7_in_sel:6;
        /** func7_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func7_in_inv_sel:1;
        /** sig7_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig7_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func7_in_sel_cfg_reg_t;

/** Type of func8_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func8_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func8_in_sel:6;
        /** func8_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func8_in_inv_sel:1;
        /** sig8_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig8_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func8_in_sel_cfg_reg_t;

/** Type of func9_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func9_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func9_in_sel:6;
        /** func9_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func9_in_inv_sel:1;
        /** sig9_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig9_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func9_in_sel_cfg_reg_t;

/** Type of func10_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func10_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func10_in_sel:6;
        /** func10_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func10_in_inv_sel:1;
        /** sig10_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig10_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func10_in_sel_cfg_reg_t;

/** Type of func11_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func11_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func11_in_sel:6;
        /** func11_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func11_in_inv_sel:1;
        /** sig11_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig11_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func11_in_sel_cfg_reg_t;

/** Type of func12_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func12_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func12_in_sel:6;
        /** func12_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func12_in_inv_sel:1;
        /** sig12_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig12_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func12_in_sel_cfg_reg_t;

/** Type of func13_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func13_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func13_in_sel:6;
        /** func13_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func13_in_inv_sel:1;
        /** sig13_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig13_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func13_in_sel_cfg_reg_t;

/** Type of func14_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func14_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func14_in_sel:6;
        /** func14_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func14_in_inv_sel:1;
        /** sig14_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig14_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func14_in_sel_cfg_reg_t;

/** Type of func15_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func15_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func15_in_sel:6;
        /** func15_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func15_in_inv_sel:1;
        /** sig15_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig15_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func15_in_sel_cfg_reg_t;

/** Type of func16_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func16_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func16_in_sel:6;
        /** func16_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func16_in_inv_sel:1;
        /** sig16_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig16_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func16_in_sel_cfg_reg_t;

/** Type of func17_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func17_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func17_in_sel:6;
        /** func17_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func17_in_inv_sel:1;
        /** sig17_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig17_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func17_in_sel_cfg_reg_t;

/** Type of func18_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func18_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func18_in_sel:6;
        /** func18_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func18_in_inv_sel:1;
        /** sig18_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig18_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func18_in_sel_cfg_reg_t;

/** Type of func19_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func19_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func19_in_sel:6;
        /** func19_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func19_in_inv_sel:1;
        /** sig19_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig19_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func19_in_sel_cfg_reg_t;

/** Type of func20_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func20_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func20_in_sel:6;
        /** func20_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func20_in_inv_sel:1;
        /** sig20_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig20_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func20_in_sel_cfg_reg_t;

/** Type of func21_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func21_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func21_in_sel:6;
        /** func21_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func21_in_inv_sel:1;
        /** sig21_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig21_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func21_in_sel_cfg_reg_t;

/** Type of func22_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func22_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func22_in_sel:6;
        /** func22_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func22_in_inv_sel:1;
        /** sig22_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig22_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func22_in_sel_cfg_reg_t;

/** Type of func23_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func23_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func23_in_sel:6;
        /** func23_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func23_in_inv_sel:1;
        /** sig23_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig23_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func23_in_sel_cfg_reg_t;

/** Type of func24_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func24_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func24_in_sel:6;
        /** func24_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func24_in_inv_sel:1;
        /** sig24_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig24_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func24_in_sel_cfg_reg_t;

/** Type of func25_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func25_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func25_in_sel:6;
        /** func25_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func25_in_inv_sel:1;
        /** sig25_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig25_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func25_in_sel_cfg_reg_t;

/** Type of func26_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func26_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func26_in_sel:6;
        /** func26_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func26_in_inv_sel:1;
        /** sig26_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig26_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func26_in_sel_cfg_reg_t;

/** Type of func27_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func27_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func27_in_sel:6;
        /** func27_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func27_in_inv_sel:1;
        /** sig27_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig27_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func27_in_sel_cfg_reg_t;

/** Type of func28_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func28_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func28_in_sel:6;
        /** func28_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func28_in_inv_sel:1;
        /** sig28_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig28_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func28_in_sel_cfg_reg_t;

/** Type of func29_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func29_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func29_in_sel:6;
        /** func29_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func29_in_inv_sel:1;
        /** sig29_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig29_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func29_in_sel_cfg_reg_t;

/** Type of func30_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func30_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func30_in_sel:6;
        /** func30_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func30_in_inv_sel:1;
        /** sig30_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig30_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func30_in_sel_cfg_reg_t;

/** Type of func31_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func31_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func31_in_sel:6;
        /** func31_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func31_in_inv_sel:1;
        /** sig31_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig31_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func31_in_sel_cfg_reg_t;

/** Type of func32_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func32_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func32_in_sel:6;
        /** func32_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func32_in_inv_sel:1;
        /** sig32_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig32_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func32_in_sel_cfg_reg_t;

/** Type of func33_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func33_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func33_in_sel:6;
        /** func33_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func33_in_inv_sel:1;
        /** sig33_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig33_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func33_in_sel_cfg_reg_t;

/** Type of func34_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func34_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func34_in_sel:6;
        /** func34_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func34_in_inv_sel:1;
        /** sig34_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig34_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func34_in_sel_cfg_reg_t;

/** Type of func35_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func35_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func35_in_sel:6;
        /** func35_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func35_in_inv_sel:1;
        /** sig35_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig35_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func35_in_sel_cfg_reg_t;

/** Type of func36_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func36_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func36_in_sel:6;
        /** func36_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func36_in_inv_sel:1;
        /** sig36_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig36_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func36_in_sel_cfg_reg_t;

/** Type of func37_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func37_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func37_in_sel:6;
        /** func37_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func37_in_inv_sel:1;
        /** sig37_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig37_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func37_in_sel_cfg_reg_t;

/** Type of func38_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func38_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func38_in_sel:6;
        /** func38_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func38_in_inv_sel:1;
        /** sig38_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig38_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func38_in_sel_cfg_reg_t;

/** Type of func39_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func39_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func39_in_sel:6;
        /** func39_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func39_in_inv_sel:1;
        /** sig39_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig39_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func39_in_sel_cfg_reg_t;

/** Type of func40_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func40_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func40_in_sel:6;
        /** func40_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func40_in_inv_sel:1;
        /** sig40_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig40_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func40_in_sel_cfg_reg_t;

/** Type of func41_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func41_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func41_in_sel:6;
        /** func41_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func41_in_inv_sel:1;
        /** sig41_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig41_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func41_in_sel_cfg_reg_t;

/** Type of func42_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func42_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func42_in_sel:6;
        /** func42_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func42_in_inv_sel:1;
        /** sig42_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig42_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func42_in_sel_cfg_reg_t;

/** Type of func43_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func43_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func43_in_sel:6;
        /** func43_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func43_in_inv_sel:1;
        /** sig43_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig43_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func43_in_sel_cfg_reg_t;

/** Type of func44_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func44_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func44_in_sel:6;
        /** func44_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func44_in_inv_sel:1;
        /** sig44_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig44_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func44_in_sel_cfg_reg_t;

/** Type of func45_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func45_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func45_in_sel:6;
        /** func45_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func45_in_inv_sel:1;
        /** sig45_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig45_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func45_in_sel_cfg_reg_t;

/** Type of func47_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func47_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func47_in_sel:6;
        /** func47_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func47_in_inv_sel:1;
        /** sig47_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig47_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func47_in_sel_cfg_reg_t;

/** Type of func48_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func48_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func48_in_sel:6;
        /** func48_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func48_in_inv_sel:1;
        /** sig48_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig48_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func48_in_sel_cfg_reg_t;

/** Type of func49_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func49_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func49_in_sel:6;
        /** func49_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func49_in_inv_sel:1;
        /** sig49_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig49_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func49_in_sel_cfg_reg_t;

/** Type of func50_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func50_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func50_in_sel:6;
        /** func50_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func50_in_inv_sel:1;
        /** sig50_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig50_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func50_in_sel_cfg_reg_t;

/** Type of func51_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func51_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func51_in_sel:6;
        /** func51_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func51_in_inv_sel:1;
        /** sig51_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig51_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func51_in_sel_cfg_reg_t;

/** Type of func52_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func52_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func52_in_sel:6;
        /** func52_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func52_in_inv_sel:1;
        /** sig52_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig52_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func52_in_sel_cfg_reg_t;

/** Type of func53_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func53_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func53_in_sel:6;
        /** func53_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func53_in_inv_sel:1;
        /** sig53_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig53_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func53_in_sel_cfg_reg_t;

/** Type of func54_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func54_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func54_in_sel:6;
        /** func54_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func54_in_inv_sel:1;
        /** sig54_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig54_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func54_in_sel_cfg_reg_t;

/** Type of func55_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func55_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func55_in_sel:6;
        /** func55_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func55_in_inv_sel:1;
        /** sig55_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig55_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func55_in_sel_cfg_reg_t;

/** Type of func56_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func56_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func56_in_sel:6;
        /** func56_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func56_in_inv_sel:1;
        /** sig56_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig56_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func56_in_sel_cfg_reg_t;

/** Type of func57_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func57_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func57_in_sel:6;
        /** func57_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func57_in_inv_sel:1;
        /** sig57_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig57_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func57_in_sel_cfg_reg_t;

/** Type of func58_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func58_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func58_in_sel:6;
        /** func58_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func58_in_inv_sel:1;
        /** sig58_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig58_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func58_in_sel_cfg_reg_t;

/** Type of func59_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func59_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func59_in_sel:6;
        /** func59_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func59_in_inv_sel:1;
        /** sig59_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig59_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func59_in_sel_cfg_reg_t;

/** Type of func60_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func60_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func60_in_sel:6;
        /** func60_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func60_in_inv_sel:1;
        /** sig60_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig60_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func60_in_sel_cfg_reg_t;

/** Type of func61_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func61_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func61_in_sel:6;
        /** func61_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func61_in_inv_sel:1;
        /** sig61_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig61_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func61_in_sel_cfg_reg_t;

/** Type of func62_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func62_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func62_in_sel:6;
        /** func62_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func62_in_inv_sel:1;
        /** sig62_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig62_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func62_in_sel_cfg_reg_t;

/** Type of func63_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func63_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func63_in_sel:6;
        /** func63_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func63_in_inv_sel:1;
        /** sig63_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig63_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func63_in_sel_cfg_reg_t;

/** Type of func64_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func64_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func64_in_sel:6;
        /** func64_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func64_in_inv_sel:1;
        /** sig64_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig64_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func64_in_sel_cfg_reg_t;

/** Type of func65_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func65_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func65_in_sel:6;
        /** func65_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func65_in_inv_sel:1;
        /** sig65_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig65_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func65_in_sel_cfg_reg_t;

/** Type of func66_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func66_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func66_in_sel:6;
        /** func66_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func66_in_inv_sel:1;
        /** sig66_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig66_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func66_in_sel_cfg_reg_t;

/** Type of func68_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func68_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func68_in_sel:6;
        /** func68_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func68_in_inv_sel:1;
        /** sig68_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig68_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func68_in_sel_cfg_reg_t;

/** Type of func69_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func69_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func69_in_sel:6;
        /** func69_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func69_in_inv_sel:1;
        /** sig69_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig69_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func69_in_sel_cfg_reg_t;

/** Type of func70_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func70_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func70_in_sel:6;
        /** func70_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func70_in_inv_sel:1;
        /** sig70_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig70_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func70_in_sel_cfg_reg_t;

/** Type of func71_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func71_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func71_in_sel:6;
        /** func71_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func71_in_inv_sel:1;
        /** sig71_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig71_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func71_in_sel_cfg_reg_t;

/** Type of func74_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func74_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func74_in_sel:6;
        /** func74_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func74_in_inv_sel:1;
        /** sig74_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig74_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func74_in_sel_cfg_reg_t;

/** Type of func75_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func75_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func75_in_sel:6;
        /** func75_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func75_in_inv_sel:1;
        /** sig75_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig75_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func75_in_sel_cfg_reg_t;

/** Type of func76_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func76_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func76_in_sel:6;
        /** func76_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func76_in_inv_sel:1;
        /** sig76_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig76_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func76_in_sel_cfg_reg_t;

/** Type of func77_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func77_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func77_in_sel:6;
        /** func77_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func77_in_inv_sel:1;
        /** sig77_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig77_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func77_in_sel_cfg_reg_t;

/** Type of func78_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func78_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func78_in_sel:6;
        /** func78_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func78_in_inv_sel:1;
        /** sig78_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig78_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func78_in_sel_cfg_reg_t;

/** Type of func80_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func80_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func80_in_sel:6;
        /** func80_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func80_in_inv_sel:1;
        /** sig80_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig80_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func80_in_sel_cfg_reg_t;

/** Type of func83_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func83_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func83_in_sel:6;
        /** func83_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func83_in_inv_sel:1;
        /** sig83_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig83_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func83_in_sel_cfg_reg_t;

/** Type of func86_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func86_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func86_in_sel:6;
        /** func86_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func86_in_inv_sel:1;
        /** sig86_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig86_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func86_in_sel_cfg_reg_t;

/** Type of func89_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func89_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func89_in_sel:6;
        /** func89_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func89_in_inv_sel:1;
        /** sig89_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig89_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func89_in_sel_cfg_reg_t;

/** Type of func90_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func90_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func90_in_sel:6;
        /** func90_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func90_in_inv_sel:1;
        /** sig90_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig90_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func90_in_sel_cfg_reg_t;

/** Type of func91_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func91_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func91_in_sel:6;
        /** func91_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func91_in_inv_sel:1;
        /** sig91_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig91_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func91_in_sel_cfg_reg_t;

/** Type of func92_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func92_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func92_in_sel:6;
        /** func92_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func92_in_inv_sel:1;
        /** sig92_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig92_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func92_in_sel_cfg_reg_t;

/** Type of func93_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func93_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func93_in_sel:6;
        /** func93_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func93_in_inv_sel:1;
        /** sig93_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig93_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func93_in_sel_cfg_reg_t;

/** Type of func94_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func94_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func94_in_sel:6;
        /** func94_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func94_in_inv_sel:1;
        /** sig94_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig94_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func94_in_sel_cfg_reg_t;

/** Type of func95_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func95_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func95_in_sel:6;
        /** func95_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func95_in_inv_sel:1;
        /** sig95_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig95_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func95_in_sel_cfg_reg_t;

/** Type of func96_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func96_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func96_in_sel:6;
        /** func96_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func96_in_inv_sel:1;
        /** sig96_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig96_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func96_in_sel_cfg_reg_t;

/** Type of func97_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func97_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func97_in_sel:6;
        /** func97_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func97_in_inv_sel:1;
        /** sig97_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig97_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func97_in_sel_cfg_reg_t;

/** Type of func98_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func98_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func98_in_sel:6;
        /** func98_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func98_in_inv_sel:1;
        /** sig98_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig98_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func98_in_sel_cfg_reg_t;

/** Type of func99_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func99_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func99_in_sel:6;
        /** func99_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func99_in_inv_sel:1;
        /** sig99_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig99_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func99_in_sel_cfg_reg_t;

/** Type of func100_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func100_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func100_in_sel:6;
        /** func100_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func100_in_inv_sel:1;
        /** sig100_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig100_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func100_in_sel_cfg_reg_t;

/** Type of func101_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func101_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func101_in_sel:6;
        /** func101_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func101_in_inv_sel:1;
        /** sig101_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig101_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func101_in_sel_cfg_reg_t;

/** Type of func102_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func102_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func102_in_sel:6;
        /** func102_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func102_in_inv_sel:1;
        /** sig102_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig102_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func102_in_sel_cfg_reg_t;

/** Type of func103_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func103_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func103_in_sel:6;
        /** func103_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func103_in_inv_sel:1;
        /** sig103_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig103_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func103_in_sel_cfg_reg_t;

/** Type of func104_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func104_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func104_in_sel:6;
        /** func104_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func104_in_inv_sel:1;
        /** sig104_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig104_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func104_in_sel_cfg_reg_t;

/** Type of func105_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func105_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func105_in_sel:6;
        /** func105_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func105_in_inv_sel:1;
        /** sig105_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig105_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func105_in_sel_cfg_reg_t;

/** Type of func106_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func106_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func106_in_sel:6;
        /** func106_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func106_in_inv_sel:1;
        /** sig106_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig106_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func106_in_sel_cfg_reg_t;

/** Type of func107_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func107_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func107_in_sel:6;
        /** func107_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func107_in_inv_sel:1;
        /** sig107_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig107_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func107_in_sel_cfg_reg_t;

/** Type of func108_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func108_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func108_in_sel:6;
        /** func108_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func108_in_inv_sel:1;
        /** sig108_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig108_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func108_in_sel_cfg_reg_t;

/** Type of func109_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func109_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func109_in_sel:6;
        /** func109_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func109_in_inv_sel:1;
        /** sig109_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig109_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func109_in_sel_cfg_reg_t;

/** Type of func110_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func110_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func110_in_sel:6;
        /** func110_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func110_in_inv_sel:1;
        /** sig110_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig110_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func110_in_sel_cfg_reg_t;

/** Type of func111_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func111_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func111_in_sel:6;
        /** func111_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func111_in_inv_sel:1;
        /** sig111_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig111_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func111_in_sel_cfg_reg_t;

/** Type of func112_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func112_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func112_in_sel:6;
        /** func112_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func112_in_inv_sel:1;
        /** sig112_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig112_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func112_in_sel_cfg_reg_t;

/** Type of func113_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func113_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func113_in_sel:6;
        /** func113_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func113_in_inv_sel:1;
        /** sig113_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig113_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func113_in_sel_cfg_reg_t;

/** Type of func114_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func114_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func114_in_sel:6;
        /** func114_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func114_in_inv_sel:1;
        /** sig114_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig114_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func114_in_sel_cfg_reg_t;

/** Type of func117_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func117_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func117_in_sel:6;
        /** func117_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func117_in_inv_sel:1;
        /** sig117_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig117_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func117_in_sel_cfg_reg_t;

/** Type of func118_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func118_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func118_in_sel:6;
        /** func118_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func118_in_inv_sel:1;
        /** sig118_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig118_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func118_in_sel_cfg_reg_t;

/** Type of func126_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func126_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func126_in_sel:6;
        /** func126_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func126_in_inv_sel:1;
        /** sig126_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig126_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func126_in_sel_cfg_reg_t;

/** Type of func127_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func127_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func127_in_sel:6;
        /** func127_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func127_in_inv_sel:1;
        /** sig127_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig127_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func127_in_sel_cfg_reg_t;

/** Type of func128_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func128_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func128_in_sel:6;
        /** func128_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func128_in_inv_sel:1;
        /** sig128_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig128_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func128_in_sel_cfg_reg_t;

/** Type of func129_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func129_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func129_in_sel:6;
        /** func129_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func129_in_inv_sel:1;
        /** sig129_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig129_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func129_in_sel_cfg_reg_t;

/** Type of func130_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func130_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func130_in_sel:6;
        /** func130_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func130_in_inv_sel:1;
        /** sig130_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig130_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func130_in_sel_cfg_reg_t;

/** Type of func131_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func131_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func131_in_sel:6;
        /** func131_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func131_in_inv_sel:1;
        /** sig131_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig131_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func131_in_sel_cfg_reg_t;

/** Type of func132_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func132_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func132_in_sel:6;
        /** func132_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func132_in_inv_sel:1;
        /** sig132_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig132_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func132_in_sel_cfg_reg_t;

/** Type of func133_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func133_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func133_in_sel:6;
        /** func133_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func133_in_inv_sel:1;
        /** sig133_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig133_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func133_in_sel_cfg_reg_t;

/** Type of func134_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func134_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func134_in_sel:6;
        /** func134_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func134_in_inv_sel:1;
        /** sig134_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig134_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func134_in_sel_cfg_reg_t;

/** Type of func135_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func135_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func135_in_sel:6;
        /** func135_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func135_in_inv_sel:1;
        /** sig135_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig135_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func135_in_sel_cfg_reg_t;

/** Type of func136_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func136_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func136_in_sel:6;
        /** func136_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func136_in_inv_sel:1;
        /** sig136_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig136_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func136_in_sel_cfg_reg_t;

/** Type of func137_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func137_in_sel : R/W; bitpos: [5:0]; default: 63;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func137_in_sel:6;
        /** func137_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func137_in_inv_sel:1;
        /** sig137_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig137_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func137_in_sel_cfg_reg_t;

/** Type of func138_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func138_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func138_in_sel:6;
        /** func138_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func138_in_inv_sel:1;
        /** sig138_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig138_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func138_in_sel_cfg_reg_t;

/** Type of func139_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func139_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func139_in_sel:6;
        /** func139_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func139_in_inv_sel:1;
        /** sig139_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig139_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func139_in_sel_cfg_reg_t;

/** Type of func140_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func140_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func140_in_sel:6;
        /** func140_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func140_in_inv_sel:1;
        /** sig140_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig140_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func140_in_sel_cfg_reg_t;

/** Type of func141_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func141_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func141_in_sel:6;
        /** func141_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func141_in_inv_sel:1;
        /** sig141_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig141_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func141_in_sel_cfg_reg_t;

/** Type of func142_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func142_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func142_in_sel:6;
        /** func142_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func142_in_inv_sel:1;
        /** sig142_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig142_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func142_in_sel_cfg_reg_t;

/** Type of func143_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func143_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func143_in_sel:6;
        /** func143_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func143_in_inv_sel:1;
        /** sig143_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig143_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func143_in_sel_cfg_reg_t;

/** Type of func144_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func144_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func144_in_sel:6;
        /** func144_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func144_in_inv_sel:1;
        /** sig144_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig144_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func144_in_sel_cfg_reg_t;

/** Type of func145_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func145_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func145_in_sel:6;
        /** func145_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func145_in_inv_sel:1;
        /** sig145_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig145_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func145_in_sel_cfg_reg_t;

/** Type of func146_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func146_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func146_in_sel:6;
        /** func146_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func146_in_inv_sel:1;
        /** sig146_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig146_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func146_in_sel_cfg_reg_t;

/** Type of func147_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func147_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func147_in_sel:6;
        /** func147_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func147_in_inv_sel:1;
        /** sig147_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig147_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func147_in_sel_cfg_reg_t;

/** Type of func148_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func148_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func148_in_sel:6;
        /** func148_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func148_in_inv_sel:1;
        /** sig148_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig148_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func148_in_sel_cfg_reg_t;

/** Type of func149_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func149_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func149_in_sel:6;
        /** func149_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func149_in_inv_sel:1;
        /** sig149_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig149_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func149_in_sel_cfg_reg_t;

/** Type of func150_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func150_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func150_in_sel:6;
        /** func150_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func150_in_inv_sel:1;
        /** sig150_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig150_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func150_in_sel_cfg_reg_t;

/** Type of func151_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func151_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func151_in_sel:6;
        /** func151_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func151_in_inv_sel:1;
        /** sig151_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig151_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func151_in_sel_cfg_reg_t;

/** Type of func152_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func152_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func152_in_sel:6;
        /** func152_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func152_in_inv_sel:1;
        /** sig152_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig152_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func152_in_sel_cfg_reg_t;

/** Type of func153_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func153_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func153_in_sel:6;
        /** func153_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func153_in_inv_sel:1;
        /** sig153_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig153_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func153_in_sel_cfg_reg_t;

/** Type of func154_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func154_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func154_in_sel:6;
        /** func154_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func154_in_inv_sel:1;
        /** sig154_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig154_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func154_in_sel_cfg_reg_t;

/** Type of func155_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func155_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func155_in_sel:6;
        /** func155_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func155_in_inv_sel:1;
        /** sig155_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig155_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func155_in_sel_cfg_reg_t;

/** Type of func156_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func156_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func156_in_sel:6;
        /** func156_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func156_in_inv_sel:1;
        /** sig156_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig156_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func156_in_sel_cfg_reg_t;

/** Type of func158_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func158_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func158_in_sel:6;
        /** func158_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func158_in_inv_sel:1;
        /** sig158_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig158_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func158_in_sel_cfg_reg_t;

/** Type of func159_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func159_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func159_in_sel:6;
        /** func159_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func159_in_inv_sel:1;
        /** sig159_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig159_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func159_in_sel_cfg_reg_t;

/** Type of func160_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func160_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func160_in_sel:6;
        /** func160_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func160_in_inv_sel:1;
        /** sig160_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig160_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func160_in_sel_cfg_reg_t;

/** Type of func161_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func161_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func161_in_sel:6;
        /** func161_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func161_in_inv_sel:1;
        /** sig161_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig161_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func161_in_sel_cfg_reg_t;

/** Type of func162_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func162_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func162_in_sel:6;
        /** func162_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func162_in_inv_sel:1;
        /** sig162_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig162_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func162_in_sel_cfg_reg_t;

/** Type of func163_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func163_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func163_in_sel:6;
        /** func163_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func163_in_inv_sel:1;
        /** sig163_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig163_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func163_in_sel_cfg_reg_t;

/** Type of func164_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func164_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func164_in_sel:6;
        /** func164_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func164_in_inv_sel:1;
        /** sig164_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig164_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func164_in_sel_cfg_reg_t;

/** Type of func165_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func165_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func165_in_sel:6;
        /** func165_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func165_in_inv_sel:1;
        /** sig165_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig165_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func165_in_sel_cfg_reg_t;

/** Type of func166_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func166_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func166_in_sel:6;
        /** func166_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func166_in_inv_sel:1;
        /** sig166_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig166_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func166_in_sel_cfg_reg_t;

/** Type of func167_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func167_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func167_in_sel:6;
        /** func167_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func167_in_inv_sel:1;
        /** sig167_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig167_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func167_in_sel_cfg_reg_t;

/** Type of func168_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func168_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func168_in_sel:6;
        /** func168_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func168_in_inv_sel:1;
        /** sig168_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig168_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func168_in_sel_cfg_reg_t;

/** Type of func169_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func169_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func169_in_sel:6;
        /** func169_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func169_in_inv_sel:1;
        /** sig169_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig169_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func169_in_sel_cfg_reg_t;

/** Type of func170_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func170_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func170_in_sel:6;
        /** func170_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func170_in_inv_sel:1;
        /** sig170_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig170_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func170_in_sel_cfg_reg_t;

/** Type of func171_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func171_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func171_in_sel:6;
        /** func171_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func171_in_inv_sel:1;
        /** sig171_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig171_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func171_in_sel_cfg_reg_t;

/** Type of func172_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func172_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func172_in_sel:6;
        /** func172_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func172_in_inv_sel:1;
        /** sig172_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig172_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func172_in_sel_cfg_reg_t;

/** Type of func173_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func173_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func173_in_sel:6;
        /** func173_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func173_in_inv_sel:1;
        /** sig173_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig173_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func173_in_sel_cfg_reg_t;

/** Type of func174_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func174_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func174_in_sel:6;
        /** func174_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func174_in_inv_sel:1;
        /** sig174_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig174_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func174_in_sel_cfg_reg_t;

/** Type of func175_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func175_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func175_in_sel:6;
        /** func175_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func175_in_inv_sel:1;
        /** sig175_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig175_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func175_in_sel_cfg_reg_t;

/** Type of func176_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func176_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func176_in_sel:6;
        /** func176_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func176_in_inv_sel:1;
        /** sig176_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig176_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func176_in_sel_cfg_reg_t;

/** Type of func177_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func177_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func177_in_sel:6;
        /** func177_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func177_in_inv_sel:1;
        /** sig177_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig177_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func177_in_sel_cfg_reg_t;

/** Type of func178_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func178_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func178_in_sel:6;
        /** func178_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func178_in_inv_sel:1;
        /** sig178_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig178_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func178_in_sel_cfg_reg_t;

/** Type of func179_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func179_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func179_in_sel:6;
        /** func179_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func179_in_inv_sel:1;
        /** sig179_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig179_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func179_in_sel_cfg_reg_t;

/** Type of func180_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func180_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func180_in_sel:6;
        /** func180_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func180_in_inv_sel:1;
        /** sig180_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig180_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func180_in_sel_cfg_reg_t;

/** Type of func181_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func181_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func181_in_sel:6;
        /** func181_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func181_in_inv_sel:1;
        /** sig181_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig181_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func181_in_sel_cfg_reg_t;

/** Type of func182_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func182_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func182_in_sel:6;
        /** func182_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func182_in_inv_sel:1;
        /** sig182_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig182_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func182_in_sel_cfg_reg_t;

/** Type of func183_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func183_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func183_in_sel:6;
        /** func183_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func183_in_inv_sel:1;
        /** sig183_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig183_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func183_in_sel_cfg_reg_t;

/** Type of func184_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func184_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func184_in_sel:6;
        /** func184_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func184_in_inv_sel:1;
        /** sig184_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig184_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func184_in_sel_cfg_reg_t;

/** Type of func185_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func185_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func185_in_sel:6;
        /** func185_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func185_in_inv_sel:1;
        /** sig185_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig185_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func185_in_sel_cfg_reg_t;

/** Type of func186_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func186_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func186_in_sel:6;
        /** func186_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func186_in_inv_sel:1;
        /** sig186_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig186_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func186_in_sel_cfg_reg_t;

/** Type of func187_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func187_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func187_in_sel:6;
        /** func187_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func187_in_inv_sel:1;
        /** sig187_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig187_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func187_in_sel_cfg_reg_t;

/** Type of func188_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func188_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func188_in_sel:6;
        /** func188_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func188_in_inv_sel:1;
        /** sig188_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig188_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func188_in_sel_cfg_reg_t;

/** Type of func189_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func189_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func189_in_sel:6;
        /** func189_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func189_in_inv_sel:1;
        /** sig189_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig189_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func189_in_sel_cfg_reg_t;

/** Type of func190_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func190_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func190_in_sel:6;
        /** func190_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func190_in_inv_sel:1;
        /** sig190_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig190_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func190_in_sel_cfg_reg_t;

/** Type of func191_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func191_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func191_in_sel:6;
        /** func191_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func191_in_inv_sel:1;
        /** sig191_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig191_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func191_in_sel_cfg_reg_t;

/** Type of func192_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func192_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func192_in_sel:6;
        /** func192_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func192_in_inv_sel:1;
        /** sig192_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig192_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func192_in_sel_cfg_reg_t;

/** Type of func193_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func193_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func193_in_sel:6;
        /** func193_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func193_in_inv_sel:1;
        /** sig193_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig193_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func193_in_sel_cfg_reg_t;

/** Type of func194_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func194_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func194_in_sel:6;
        /** func194_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func194_in_inv_sel:1;
        /** sig194_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig194_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func194_in_sel_cfg_reg_t;

/** Type of func195_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func195_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func195_in_sel:6;
        /** func195_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func195_in_inv_sel:1;
        /** sig195_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig195_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func195_in_sel_cfg_reg_t;

/** Type of func196_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func196_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func196_in_sel:6;
        /** func196_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func196_in_inv_sel:1;
        /** sig196_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig196_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func196_in_sel_cfg_reg_t;

/** Type of func197_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func197_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func197_in_sel:6;
        /** func197_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func197_in_inv_sel:1;
        /** sig197_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig197_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func197_in_sel_cfg_reg_t;

/** Type of func198_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func198_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func198_in_sel:6;
        /** func198_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func198_in_inv_sel:1;
        /** sig198_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig198_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func198_in_sel_cfg_reg_t;

/** Type of func199_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func199_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func199_in_sel:6;
        /** func199_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func199_in_inv_sel:1;
        /** sig199_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig199_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func199_in_sel_cfg_reg_t;

/** Type of func200_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func200_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func200_in_sel:6;
        /** func200_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func200_in_inv_sel:1;
        /** sig200_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig200_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func200_in_sel_cfg_reg_t;

/** Type of func201_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func201_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func201_in_sel:6;
        /** func201_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func201_in_inv_sel:1;
        /** sig201_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig201_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func201_in_sel_cfg_reg_t;

/** Type of func202_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func202_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func202_in_sel:6;
        /** func202_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func202_in_inv_sel:1;
        /** sig202_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig202_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func202_in_sel_cfg_reg_t;

/** Type of func203_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func203_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func203_in_sel:6;
        /** func203_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func203_in_inv_sel:1;
        /** sig203_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig203_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func203_in_sel_cfg_reg_t;

/** Type of func214_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func214_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func214_in_sel:6;
        /** func214_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func214_in_inv_sel:1;
        /** sig214_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig214_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func214_in_sel_cfg_reg_t;

/** Type of func215_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func215_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func215_in_sel:6;
        /** func215_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func215_in_inv_sel:1;
        /** sig215_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig215_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func215_in_sel_cfg_reg_t;

/** Type of func216_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func216_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func216_in_sel:6;
        /** func216_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func216_in_inv_sel:1;
        /** sig216_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig216_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func216_in_sel_cfg_reg_t;

/** Type of func217_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func217_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func217_in_sel:6;
        /** func217_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func217_in_inv_sel:1;
        /** sig217_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig217_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func217_in_sel_cfg_reg_t;

/** Type of func218_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func218_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func218_in_sel:6;
        /** func218_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func218_in_inv_sel:1;
        /** sig218_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig218_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func218_in_sel_cfg_reg_t;

/** Type of func219_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func219_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func219_in_sel:6;
        /** func219_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func219_in_inv_sel:1;
        /** sig219_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig219_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func219_in_sel_cfg_reg_t;

/** Type of func220_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func220_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func220_in_sel:6;
        /** func220_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func220_in_inv_sel:1;
        /** sig220_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig220_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func220_in_sel_cfg_reg_t;

/** Type of func221_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func221_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func221_in_sel:6;
        /** func221_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func221_in_inv_sel:1;
        /** sig221_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig221_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func221_in_sel_cfg_reg_t;

/** Type of func222_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func222_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func222_in_sel:6;
        /** func222_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func222_in_inv_sel:1;
        /** sig222_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig222_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func222_in_sel_cfg_reg_t;

/** Type of func223_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func223_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func223_in_sel:6;
        /** func223_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func223_in_inv_sel:1;
        /** sig223_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig223_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func223_in_sel_cfg_reg_t;

/** Type of func224_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func224_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func224_in_sel:6;
        /** func224_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func224_in_inv_sel:1;
        /** sig224_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig224_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func224_in_sel_cfg_reg_t;

/** Type of func225_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func225_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func225_in_sel:6;
        /** func225_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func225_in_inv_sel:1;
        /** sig225_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig225_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func225_in_sel_cfg_reg_t;

/** Type of func226_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func226_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func226_in_sel:6;
        /** func226_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func226_in_inv_sel:1;
        /** sig226_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig226_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func226_in_sel_cfg_reg_t;

/** Type of func227_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func227_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func227_in_sel:6;
        /** func227_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func227_in_inv_sel:1;
        /** sig227_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig227_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func227_in_sel_cfg_reg_t;

/** Type of func228_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func228_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func228_in_sel:6;
        /** func228_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func228_in_inv_sel:1;
        /** sig228_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig228_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func228_in_sel_cfg_reg_t;

/** Type of func229_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func229_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func229_in_sel:6;
        /** func229_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func229_in_inv_sel:1;
        /** sig229_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig229_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func229_in_sel_cfg_reg_t;

/** Type of func230_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func230_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func230_in_sel:6;
        /** func230_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func230_in_inv_sel:1;
        /** sig230_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig230_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func230_in_sel_cfg_reg_t;

/** Type of func231_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func231_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func231_in_sel:6;
        /** func231_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func231_in_inv_sel:1;
        /** sig231_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig231_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func231_in_sel_cfg_reg_t;

/** Type of func232_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func232_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func232_in_sel:6;
        /** func232_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func232_in_inv_sel:1;
        /** sig232_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig232_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func232_in_sel_cfg_reg_t;

/** Type of func233_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func233_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func233_in_sel:6;
        /** func233_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func233_in_inv_sel:1;
        /** sig233_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig233_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func233_in_sel_cfg_reg_t;

/** Type of func234_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func234_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func234_in_sel:6;
        /** func234_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func234_in_inv_sel:1;
        /** sig234_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig234_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func234_in_sel_cfg_reg_t;

/** Type of func235_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func235_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func235_in_sel:6;
        /** func235_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func235_in_inv_sel:1;
        /** sig235_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig235_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func235_in_sel_cfg_reg_t;

/** Type of func236_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func236_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func236_in_sel:6;
        /** func236_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func236_in_inv_sel:1;
        /** sig236_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig236_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func236_in_sel_cfg_reg_t;

/** Type of func237_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func237_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func237_in_sel:6;
        /** func237_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func237_in_inv_sel:1;
        /** sig237_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig237_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func237_in_sel_cfg_reg_t;

/** Type of func238_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func238_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func238_in_sel:6;
        /** func238_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func238_in_inv_sel:1;
        /** sig238_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig238_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func238_in_sel_cfg_reg_t;

/** Type of func239_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func239_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func239_in_sel:6;
        /** func239_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func239_in_inv_sel:1;
        /** sig239_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig239_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func239_in_sel_cfg_reg_t;

/** Type of func240_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func240_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func240_in_sel:6;
        /** func240_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func240_in_inv_sel:1;
        /** sig240_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig240_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func240_in_sel_cfg_reg_t;

/** Type of func241_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func241_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func241_in_sel:6;
        /** func241_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func241_in_inv_sel:1;
        /** sig241_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig241_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func241_in_sel_cfg_reg_t;

/** Type of func242_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func242_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func242_in_sel:6;
        /** func242_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func242_in_inv_sel:1;
        /** sig242_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig242_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func242_in_sel_cfg_reg_t;

/** Type of func243_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func243_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func243_in_sel:6;
        /** func243_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func243_in_inv_sel:1;
        /** sig243_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig243_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func243_in_sel_cfg_reg_t;

/** Type of func244_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func244_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func244_in_sel:6;
        /** func244_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func244_in_inv_sel:1;
        /** sig244_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig244_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func244_in_sel_cfg_reg_t;

/** Type of func245_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func245_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func245_in_sel:6;
        /** func245_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func245_in_inv_sel:1;
        /** sig245_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig245_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func245_in_sel_cfg_reg_t;

/** Type of func246_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func246_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func246_in_sel:6;
        /** func246_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func246_in_inv_sel:1;
        /** sig246_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig246_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func246_in_sel_cfg_reg_t;

/** Type of func247_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func247_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func247_in_sel:6;
        /** func247_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func247_in_inv_sel:1;
        /** sig247_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig247_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func247_in_sel_cfg_reg_t;

/** Type of func248_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func248_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func248_in_sel:6;
        /** func248_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func248_in_inv_sel:1;
        /** sig248_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig248_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func248_in_sel_cfg_reg_t;

/** Type of func249_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func249_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func249_in_sel:6;
        /** func249_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func249_in_inv_sel:1;
        /** sig249_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig249_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func249_in_sel_cfg_reg_t;

/** Type of func250_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func250_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func250_in_sel:6;
        /** func250_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func250_in_inv_sel:1;
        /** sig250_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig250_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func250_in_sel_cfg_reg_t;

/** Type of func251_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func251_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func251_in_sel:6;
        /** func251_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func251_in_inv_sel:1;
        /** sig251_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig251_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func251_in_sel_cfg_reg_t;

/** Type of func252_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func252_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func252_in_sel:6;
        /** func252_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func252_in_inv_sel:1;
        /** sig252_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig252_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func252_in_sel_cfg_reg_t;

/** Type of func253_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func253_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func253_in_sel:6;
        /** func253_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func253_in_inv_sel:1;
        /** sig253_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig253_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func253_in_sel_cfg_reg_t;

/** Type of func254_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func254_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func254_in_sel:6;
        /** func254_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func254_in_inv_sel:1;
        /** sig254_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig254_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func254_in_sel_cfg_reg_t;

/** Type of func255_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func255_in_sel : R/W; bitpos: [5:0]; default: 62;
         *  set this value: s=0-56: connect GPIO[s] to this port. s=0x3F: set this port always
         *  high level. s=0x3E: set this port always low level.
         */
        uint32_t func255_in_sel:6;
        /** func255_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func255_in_inv_sel:1;
        /** sig255_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig255_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func255_in_sel_cfg_reg_t;

/** Type of funcn_out_sel_cfg register
 *  GPIO output function select register
 */
typedef union {
    struct {
        /** funcn_out_sel : R/W/SC; bitpos: [8:0]; default: 256;
         *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-255:
         *  output of GPIO[n] equals input of peripheral[s]. s=256: output of GPIO[n] equals
         *  GPIO_OUT_REG[n].
         */
        uint32_t funcn_out_sel:9;
        /** funcn_out_inv_sel : R/W/SC; bitpos: [9]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t funcn_out_inv_sel:1;
        /** funcn_oen_sel : R/W; bitpos: [10]; default: 0;
         *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
         *  enable signal.0:use peripheral output enable signal.
         */
        uint32_t funcn_oen_sel:1;
        /** funcn_oen_inv_sel : R/W; bitpos: [11]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t funcn_oen_inv_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} gpio_funcn_out_sel_cfg_reg_t;

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

/** Type of zero_det0_filter_cnt register
 *  GPIO analog comparator zero detect filter count
 */
typedef union {
    struct {
        /** zero_det0_filter_cnt : R/W; bitpos: [31:0]; default: 4294967295;
         *  GPIO analog comparator zero detect filter count
         */
        uint32_t zero_det0_filter_cnt:32;
    };
    uint32_t val;
} gpio_zero_det0_filter_cnt_reg_t;

/** Type of zero_det1_filter_cnt register
 *  GPIO analog comparator zero detect filter count
 */
typedef union {
    struct {
        /** zero_det1_filter_cnt : R/W; bitpos: [31:0]; default: 4294967295;
         *  GPIO analog comparator zero detect filter count
         */
        uint32_t zero_det1_filter_cnt:32;
    };
    uint32_t val;
} gpio_zero_det1_filter_cnt_reg_t;

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


typedef struct {
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
    volatile gpio_pinn_reg_t pinn[57];
    uint32_t reserved_158;
    volatile gpio_func1_in_sel_cfg_reg_t func1_in_sel_cfg;
    volatile gpio_func2_in_sel_cfg_reg_t func2_in_sel_cfg;
    volatile gpio_func3_in_sel_cfg_reg_t func3_in_sel_cfg;
    volatile gpio_func4_in_sel_cfg_reg_t func4_in_sel_cfg;
    volatile gpio_func5_in_sel_cfg_reg_t func5_in_sel_cfg;
    volatile gpio_func6_in_sel_cfg_reg_t func6_in_sel_cfg;
    volatile gpio_func7_in_sel_cfg_reg_t func7_in_sel_cfg;
    volatile gpio_func8_in_sel_cfg_reg_t func8_in_sel_cfg;
    volatile gpio_func9_in_sel_cfg_reg_t func9_in_sel_cfg;
    volatile gpio_func10_in_sel_cfg_reg_t func10_in_sel_cfg;
    volatile gpio_func11_in_sel_cfg_reg_t func11_in_sel_cfg;
    volatile gpio_func12_in_sel_cfg_reg_t func12_in_sel_cfg;
    volatile gpio_func13_in_sel_cfg_reg_t func13_in_sel_cfg;
    volatile gpio_func14_in_sel_cfg_reg_t func14_in_sel_cfg;
    volatile gpio_func15_in_sel_cfg_reg_t func15_in_sel_cfg;
    volatile gpio_func16_in_sel_cfg_reg_t func16_in_sel_cfg;
    volatile gpio_func17_in_sel_cfg_reg_t func17_in_sel_cfg;
    volatile gpio_func18_in_sel_cfg_reg_t func18_in_sel_cfg;
    volatile gpio_func19_in_sel_cfg_reg_t func19_in_sel_cfg;
    volatile gpio_func20_in_sel_cfg_reg_t func20_in_sel_cfg;
    volatile gpio_func21_in_sel_cfg_reg_t func21_in_sel_cfg;
    volatile gpio_func22_in_sel_cfg_reg_t func22_in_sel_cfg;
    volatile gpio_func23_in_sel_cfg_reg_t func23_in_sel_cfg;
    volatile gpio_func24_in_sel_cfg_reg_t func24_in_sel_cfg;
    volatile gpio_func25_in_sel_cfg_reg_t func25_in_sel_cfg;
    volatile gpio_func26_in_sel_cfg_reg_t func26_in_sel_cfg;
    volatile gpio_func27_in_sel_cfg_reg_t func27_in_sel_cfg;
    volatile gpio_func28_in_sel_cfg_reg_t func28_in_sel_cfg;
    volatile gpio_func29_in_sel_cfg_reg_t func29_in_sel_cfg;
    volatile gpio_func30_in_sel_cfg_reg_t func30_in_sel_cfg;
    volatile gpio_func31_in_sel_cfg_reg_t func31_in_sel_cfg;
    volatile gpio_func32_in_sel_cfg_reg_t func32_in_sel_cfg;
    volatile gpio_func33_in_sel_cfg_reg_t func33_in_sel_cfg;
    volatile gpio_func34_in_sel_cfg_reg_t func34_in_sel_cfg;
    volatile gpio_func35_in_sel_cfg_reg_t func35_in_sel_cfg;
    volatile gpio_func36_in_sel_cfg_reg_t func36_in_sel_cfg;
    volatile gpio_func37_in_sel_cfg_reg_t func37_in_sel_cfg;
    volatile gpio_func38_in_sel_cfg_reg_t func38_in_sel_cfg;
    volatile gpio_func39_in_sel_cfg_reg_t func39_in_sel_cfg;
    volatile gpio_func40_in_sel_cfg_reg_t func40_in_sel_cfg;
    volatile gpio_func41_in_sel_cfg_reg_t func41_in_sel_cfg;
    volatile gpio_func42_in_sel_cfg_reg_t func42_in_sel_cfg;
    volatile gpio_func43_in_sel_cfg_reg_t func43_in_sel_cfg;
    volatile gpio_func44_in_sel_cfg_reg_t func44_in_sel_cfg;
    volatile gpio_func45_in_sel_cfg_reg_t func45_in_sel_cfg;
    uint32_t reserved_210;
    volatile gpio_func47_in_sel_cfg_reg_t func47_in_sel_cfg;
    volatile gpio_func48_in_sel_cfg_reg_t func48_in_sel_cfg;
    volatile gpio_func49_in_sel_cfg_reg_t func49_in_sel_cfg;
    volatile gpio_func50_in_sel_cfg_reg_t func50_in_sel_cfg;
    volatile gpio_func51_in_sel_cfg_reg_t func51_in_sel_cfg;
    volatile gpio_func52_in_sel_cfg_reg_t func52_in_sel_cfg;
    volatile gpio_func53_in_sel_cfg_reg_t func53_in_sel_cfg;
    volatile gpio_func54_in_sel_cfg_reg_t func54_in_sel_cfg;
    volatile gpio_func55_in_sel_cfg_reg_t func55_in_sel_cfg;
    volatile gpio_func56_in_sel_cfg_reg_t func56_in_sel_cfg;
    volatile gpio_func57_in_sel_cfg_reg_t func57_in_sel_cfg;
    volatile gpio_func58_in_sel_cfg_reg_t func58_in_sel_cfg;
    volatile gpio_func59_in_sel_cfg_reg_t func59_in_sel_cfg;
    volatile gpio_func60_in_sel_cfg_reg_t func60_in_sel_cfg;
    volatile gpio_func61_in_sel_cfg_reg_t func61_in_sel_cfg;
    volatile gpio_func62_in_sel_cfg_reg_t func62_in_sel_cfg;
    volatile gpio_func63_in_sel_cfg_reg_t func63_in_sel_cfg;
    volatile gpio_func64_in_sel_cfg_reg_t func64_in_sel_cfg;
    volatile gpio_func65_in_sel_cfg_reg_t func65_in_sel_cfg;
    volatile gpio_func66_in_sel_cfg_reg_t func66_in_sel_cfg;
    uint32_t reserved_264;
    volatile gpio_func68_in_sel_cfg_reg_t func68_in_sel_cfg;
    volatile gpio_func69_in_sel_cfg_reg_t func69_in_sel_cfg;
    volatile gpio_func70_in_sel_cfg_reg_t func70_in_sel_cfg;
    volatile gpio_func71_in_sel_cfg_reg_t func71_in_sel_cfg;
    uint32_t reserved_278[2];
    volatile gpio_func74_in_sel_cfg_reg_t func74_in_sel_cfg;
    volatile gpio_func75_in_sel_cfg_reg_t func75_in_sel_cfg;
    volatile gpio_func76_in_sel_cfg_reg_t func76_in_sel_cfg;
    volatile gpio_func77_in_sel_cfg_reg_t func77_in_sel_cfg;
    volatile gpio_func78_in_sel_cfg_reg_t func78_in_sel_cfg;
    uint32_t reserved_294;
    volatile gpio_func80_in_sel_cfg_reg_t func80_in_sel_cfg;
    uint32_t reserved_29c[2];
    volatile gpio_func83_in_sel_cfg_reg_t func83_in_sel_cfg;
    uint32_t reserved_2a8[2];
    volatile gpio_func86_in_sel_cfg_reg_t func86_in_sel_cfg;
    uint32_t reserved_2b4[2];
    volatile gpio_func89_in_sel_cfg_reg_t func89_in_sel_cfg;
    volatile gpio_func90_in_sel_cfg_reg_t func90_in_sel_cfg;
    volatile gpio_func91_in_sel_cfg_reg_t func91_in_sel_cfg;
    volatile gpio_func92_in_sel_cfg_reg_t func92_in_sel_cfg;
    volatile gpio_func93_in_sel_cfg_reg_t func93_in_sel_cfg;
    volatile gpio_func94_in_sel_cfg_reg_t func94_in_sel_cfg;
    volatile gpio_func95_in_sel_cfg_reg_t func95_in_sel_cfg;
    volatile gpio_func96_in_sel_cfg_reg_t func96_in_sel_cfg;
    volatile gpio_func97_in_sel_cfg_reg_t func97_in_sel_cfg;
    volatile gpio_func98_in_sel_cfg_reg_t func98_in_sel_cfg;
    volatile gpio_func99_in_sel_cfg_reg_t func99_in_sel_cfg;
    volatile gpio_func100_in_sel_cfg_reg_t func100_in_sel_cfg;
    volatile gpio_func101_in_sel_cfg_reg_t func101_in_sel_cfg;
    volatile gpio_func102_in_sel_cfg_reg_t func102_in_sel_cfg;
    volatile gpio_func103_in_sel_cfg_reg_t func103_in_sel_cfg;
    volatile gpio_func104_in_sel_cfg_reg_t func104_in_sel_cfg;
    volatile gpio_func105_in_sel_cfg_reg_t func105_in_sel_cfg;
    volatile gpio_func106_in_sel_cfg_reg_t func106_in_sel_cfg;
    volatile gpio_func107_in_sel_cfg_reg_t func107_in_sel_cfg;
    volatile gpio_func108_in_sel_cfg_reg_t func108_in_sel_cfg;
    volatile gpio_func109_in_sel_cfg_reg_t func109_in_sel_cfg;
    volatile gpio_func110_in_sel_cfg_reg_t func110_in_sel_cfg;
    volatile gpio_func111_in_sel_cfg_reg_t func111_in_sel_cfg;
    volatile gpio_func112_in_sel_cfg_reg_t func112_in_sel_cfg;
    volatile gpio_func113_in_sel_cfg_reg_t func113_in_sel_cfg;
    volatile gpio_func114_in_sel_cfg_reg_t func114_in_sel_cfg;
    uint32_t reserved_324[2];
    volatile gpio_func117_in_sel_cfg_reg_t func117_in_sel_cfg;
    volatile gpio_func118_in_sel_cfg_reg_t func118_in_sel_cfg;
    uint32_t reserved_334[7];
    volatile gpio_func126_in_sel_cfg_reg_t func126_in_sel_cfg;
    volatile gpio_func127_in_sel_cfg_reg_t func127_in_sel_cfg;
    volatile gpio_func128_in_sel_cfg_reg_t func128_in_sel_cfg;
    volatile gpio_func129_in_sel_cfg_reg_t func129_in_sel_cfg;
    volatile gpio_func130_in_sel_cfg_reg_t func130_in_sel_cfg;
    volatile gpio_func131_in_sel_cfg_reg_t func131_in_sel_cfg;
    volatile gpio_func132_in_sel_cfg_reg_t func132_in_sel_cfg;
    volatile gpio_func133_in_sel_cfg_reg_t func133_in_sel_cfg;
    volatile gpio_func134_in_sel_cfg_reg_t func134_in_sel_cfg;
    volatile gpio_func135_in_sel_cfg_reg_t func135_in_sel_cfg;
    volatile gpio_func136_in_sel_cfg_reg_t func136_in_sel_cfg;
    volatile gpio_func137_in_sel_cfg_reg_t func137_in_sel_cfg;
    volatile gpio_func138_in_sel_cfg_reg_t func138_in_sel_cfg;
    volatile gpio_func139_in_sel_cfg_reg_t func139_in_sel_cfg;
    volatile gpio_func140_in_sel_cfg_reg_t func140_in_sel_cfg;
    volatile gpio_func141_in_sel_cfg_reg_t func141_in_sel_cfg;
    volatile gpio_func142_in_sel_cfg_reg_t func142_in_sel_cfg;
    volatile gpio_func143_in_sel_cfg_reg_t func143_in_sel_cfg;
    volatile gpio_func144_in_sel_cfg_reg_t func144_in_sel_cfg;
    volatile gpio_func145_in_sel_cfg_reg_t func145_in_sel_cfg;
    volatile gpio_func146_in_sel_cfg_reg_t func146_in_sel_cfg;
    volatile gpio_func147_in_sel_cfg_reg_t func147_in_sel_cfg;
    volatile gpio_func148_in_sel_cfg_reg_t func148_in_sel_cfg;
    volatile gpio_func149_in_sel_cfg_reg_t func149_in_sel_cfg;
    volatile gpio_func150_in_sel_cfg_reg_t func150_in_sel_cfg;
    volatile gpio_func151_in_sel_cfg_reg_t func151_in_sel_cfg;
    volatile gpio_func152_in_sel_cfg_reg_t func152_in_sel_cfg;
    volatile gpio_func153_in_sel_cfg_reg_t func153_in_sel_cfg;
    volatile gpio_func154_in_sel_cfg_reg_t func154_in_sel_cfg;
    volatile gpio_func155_in_sel_cfg_reg_t func155_in_sel_cfg;
    volatile gpio_func156_in_sel_cfg_reg_t func156_in_sel_cfg;
    uint32_t reserved_3cc;
    volatile gpio_func158_in_sel_cfg_reg_t func158_in_sel_cfg;
    volatile gpio_func159_in_sel_cfg_reg_t func159_in_sel_cfg;
    volatile gpio_func160_in_sel_cfg_reg_t func160_in_sel_cfg;
    volatile gpio_func161_in_sel_cfg_reg_t func161_in_sel_cfg;
    volatile gpio_func162_in_sel_cfg_reg_t func162_in_sel_cfg;
    volatile gpio_func163_in_sel_cfg_reg_t func163_in_sel_cfg;
    volatile gpio_func164_in_sel_cfg_reg_t func164_in_sel_cfg;
    volatile gpio_func165_in_sel_cfg_reg_t func165_in_sel_cfg;
    volatile gpio_func166_in_sel_cfg_reg_t func166_in_sel_cfg;
    volatile gpio_func167_in_sel_cfg_reg_t func167_in_sel_cfg;
    volatile gpio_func168_in_sel_cfg_reg_t func168_in_sel_cfg;
    volatile gpio_func169_in_sel_cfg_reg_t func169_in_sel_cfg;
    volatile gpio_func170_in_sel_cfg_reg_t func170_in_sel_cfg;
    volatile gpio_func171_in_sel_cfg_reg_t func171_in_sel_cfg;
    volatile gpio_func172_in_sel_cfg_reg_t func172_in_sel_cfg;
    volatile gpio_func173_in_sel_cfg_reg_t func173_in_sel_cfg;
    volatile gpio_func174_in_sel_cfg_reg_t func174_in_sel_cfg;
    volatile gpio_func175_in_sel_cfg_reg_t func175_in_sel_cfg;
    volatile gpio_func176_in_sel_cfg_reg_t func176_in_sel_cfg;
    volatile gpio_func177_in_sel_cfg_reg_t func177_in_sel_cfg;
    volatile gpio_func178_in_sel_cfg_reg_t func178_in_sel_cfg;
    volatile gpio_func179_in_sel_cfg_reg_t func179_in_sel_cfg;
    volatile gpio_func180_in_sel_cfg_reg_t func180_in_sel_cfg;
    volatile gpio_func181_in_sel_cfg_reg_t func181_in_sel_cfg;
    volatile gpio_func182_in_sel_cfg_reg_t func182_in_sel_cfg;
    volatile gpio_func183_in_sel_cfg_reg_t func183_in_sel_cfg;
    volatile gpio_func184_in_sel_cfg_reg_t func184_in_sel_cfg;
    volatile gpio_func185_in_sel_cfg_reg_t func185_in_sel_cfg;
    volatile gpio_func186_in_sel_cfg_reg_t func186_in_sel_cfg;
    volatile gpio_func187_in_sel_cfg_reg_t func187_in_sel_cfg;
    volatile gpio_func188_in_sel_cfg_reg_t func188_in_sel_cfg;
    volatile gpio_func189_in_sel_cfg_reg_t func189_in_sel_cfg;
    volatile gpio_func190_in_sel_cfg_reg_t func190_in_sel_cfg;
    volatile gpio_func191_in_sel_cfg_reg_t func191_in_sel_cfg;
    volatile gpio_func192_in_sel_cfg_reg_t func192_in_sel_cfg;
    volatile gpio_func193_in_sel_cfg_reg_t func193_in_sel_cfg;
    volatile gpio_func194_in_sel_cfg_reg_t func194_in_sel_cfg;
    volatile gpio_func195_in_sel_cfg_reg_t func195_in_sel_cfg;
    volatile gpio_func196_in_sel_cfg_reg_t func196_in_sel_cfg;
    volatile gpio_func197_in_sel_cfg_reg_t func197_in_sel_cfg;
    volatile gpio_func198_in_sel_cfg_reg_t func198_in_sel_cfg;
    volatile gpio_func199_in_sel_cfg_reg_t func199_in_sel_cfg;
    volatile gpio_func200_in_sel_cfg_reg_t func200_in_sel_cfg;
    volatile gpio_func201_in_sel_cfg_reg_t func201_in_sel_cfg;
    volatile gpio_func202_in_sel_cfg_reg_t func202_in_sel_cfg;
    volatile gpio_func203_in_sel_cfg_reg_t func203_in_sel_cfg;
    uint32_t reserved_488[10];
    volatile gpio_func214_in_sel_cfg_reg_t func214_in_sel_cfg;
    volatile gpio_func215_in_sel_cfg_reg_t func215_in_sel_cfg;
    volatile gpio_func216_in_sel_cfg_reg_t func216_in_sel_cfg;
    volatile gpio_func217_in_sel_cfg_reg_t func217_in_sel_cfg;
    volatile gpio_func218_in_sel_cfg_reg_t func218_in_sel_cfg;
    volatile gpio_func219_in_sel_cfg_reg_t func219_in_sel_cfg;
    volatile gpio_func220_in_sel_cfg_reg_t func220_in_sel_cfg;
    volatile gpio_func221_in_sel_cfg_reg_t func221_in_sel_cfg;
    volatile gpio_func222_in_sel_cfg_reg_t func222_in_sel_cfg;
    volatile gpio_func223_in_sel_cfg_reg_t func223_in_sel_cfg;
    volatile gpio_func224_in_sel_cfg_reg_t func224_in_sel_cfg;
    volatile gpio_func225_in_sel_cfg_reg_t func225_in_sel_cfg;
    volatile gpio_func226_in_sel_cfg_reg_t func226_in_sel_cfg;
    volatile gpio_func227_in_sel_cfg_reg_t func227_in_sel_cfg;
    volatile gpio_func228_in_sel_cfg_reg_t func228_in_sel_cfg;
    volatile gpio_func229_in_sel_cfg_reg_t func229_in_sel_cfg;
    volatile gpio_func230_in_sel_cfg_reg_t func230_in_sel_cfg;
    volatile gpio_func231_in_sel_cfg_reg_t func231_in_sel_cfg;
    volatile gpio_func232_in_sel_cfg_reg_t func232_in_sel_cfg;
    volatile gpio_func233_in_sel_cfg_reg_t func233_in_sel_cfg;
    volatile gpio_func234_in_sel_cfg_reg_t func234_in_sel_cfg;
    volatile gpio_func235_in_sel_cfg_reg_t func235_in_sel_cfg;
    volatile gpio_func236_in_sel_cfg_reg_t func236_in_sel_cfg;
    volatile gpio_func237_in_sel_cfg_reg_t func237_in_sel_cfg;
    volatile gpio_func238_in_sel_cfg_reg_t func238_in_sel_cfg;
    volatile gpio_func239_in_sel_cfg_reg_t func239_in_sel_cfg;
    volatile gpio_func240_in_sel_cfg_reg_t func240_in_sel_cfg;
    volatile gpio_func241_in_sel_cfg_reg_t func241_in_sel_cfg;
    volatile gpio_func242_in_sel_cfg_reg_t func242_in_sel_cfg;
    volatile gpio_func243_in_sel_cfg_reg_t func243_in_sel_cfg;
    volatile gpio_func244_in_sel_cfg_reg_t func244_in_sel_cfg;
    volatile gpio_func245_in_sel_cfg_reg_t func245_in_sel_cfg;
    volatile gpio_func246_in_sel_cfg_reg_t func246_in_sel_cfg;
    volatile gpio_func247_in_sel_cfg_reg_t func247_in_sel_cfg;
    volatile gpio_func248_in_sel_cfg_reg_t func248_in_sel_cfg;
    volatile gpio_func249_in_sel_cfg_reg_t func249_in_sel_cfg;
    volatile gpio_func250_in_sel_cfg_reg_t func250_in_sel_cfg;
    volatile gpio_func251_in_sel_cfg_reg_t func251_in_sel_cfg;
    volatile gpio_func252_in_sel_cfg_reg_t func252_in_sel_cfg;
    volatile gpio_func253_in_sel_cfg_reg_t func253_in_sel_cfg;
    volatile gpio_func254_in_sel_cfg_reg_t func254_in_sel_cfg;
    volatile gpio_func255_in_sel_cfg_reg_t func255_in_sel_cfg;
    volatile gpio_funcn_out_sel_cfg_reg_t funcn_out_sel_cfg[57];
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
    volatile gpio_zero_det0_filter_cnt_reg_t zero_det0_filter_cnt;
    volatile gpio_zero_det1_filter_cnt_reg_t zero_det1_filter_cnt;
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
