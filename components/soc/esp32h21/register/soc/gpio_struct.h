/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: configuration register */
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

/** Type of out register
 *  GPIO output register for GPIO0-29
 */
typedef union {
    struct {
        /** out_data_orig : R/W/SC/WTC; bitpos: [29:0]; default: 0;
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

/** Type of enable register
 *  GPIO output enable register for GPIO0-29
 */
typedef union {
    struct {
        /** enable_data : R/W/WTC; bitpos: [29:0]; default: 0;
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
        /** status_interrupt : R/W/WTC; bitpos: [29:0]; default: 0;
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

/** Type of procpu_int register
 *  GPIO_PROCPU_INT interrupt status register for GPIO0-29
 */
typedef union {
    struct {
        /** procpu_int : RO; bitpos: [29:0]; default: 0;
         *  GPIO_PROCPU_INT interrupt status register for GPIO0-29
         */
        uint32_t procpu_int:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_procpu_int_reg_t;

/** Type of procpu_nmi_int register
 *  GPIO_PROCPU_NMI_INT interrupt status register for GPIO0-29
 */
typedef union {
    struct {
        /** procpu_nmi_int : RO; bitpos: [29:0]; default: 0;
         *  GPIO_PROCPU_NMI_INT interrupt status register for GPIO0-29
         */
        uint32_t procpu_nmi_int:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_procpu_nmi_int_reg_t;

/** Type of status_next register
 *  GPIO interrupt source register for GPIO0-29
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [29:0]; default: 0;
         *  GPIO interrupt source register for GPIO0-29
         */
        uint32_t status_interrupt_next:30;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_status_next_reg_t;

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
        uint32_t reserved_11:2;
        /** pinn_int_ena : R/W; bitpos: [17:13]; default: 0;
         *  set bit 13 to enable gpio_procpu_int. set bit 14 to enable gpio_procpu_nmi_int.
         */
        uint32_t pinn_int_ena:5;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_pinn_reg_t;

/** Type of func0_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func0_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func0_in_sel:6;
        /** func0_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func0_in_inv_sel:1;
        /** sig0_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig0_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func0_in_sel_cfg_reg_t;

/** Type of func6_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func6_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func7_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func8_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func9_in_sel : R/W; bitpos: [5:0]; default: 32;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func10_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func11_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func12_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func13_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func14_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func15_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func16_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func17_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func19_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func19_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func28_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func28_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func29_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func30_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func31_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func32_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func33_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func34_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func35_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func40_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func40_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func41_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func42_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func45_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func45_in_sel : R/W; bitpos: [5:0]; default: 32;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func46_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func46_in_sel : R/W; bitpos: [5:0]; default: 32;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func46_in_sel:6;
        /** func46_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func46_in_inv_sel:1;
        /** sig46_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig46_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func46_in_sel_cfg_reg_t;

/** Type of func47_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func47_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func48_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func49_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func50_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func51_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func52_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func53_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func54_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func55_in_sel : R/W; bitpos: [5:0]; default: 32;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func56_in_sel : R/W; bitpos: [5:0]; default: 32;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func63_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func63_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func64_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func65_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func66_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func67_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func67_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func67_in_sel:6;
        /** func67_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func67_in_inv_sel:1;
        /** sig67_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig67_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func67_in_sel_cfg_reg_t;

/** Type of func68_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func68_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func69_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func70_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func71_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func72_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func72_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func72_in_sel:6;
        /** func72_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func72_in_inv_sel:1;
        /** sig72_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig72_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func72_in_sel_cfg_reg_t;

/** Type of func73_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func73_in_sel : R/W; bitpos: [5:0]; default: 32;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func73_in_sel:6;
        /** func73_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func73_in_inv_sel:1;
        /** sig73_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig73_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func73_in_sel_cfg_reg_t;

/** Type of func77_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func77_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func78_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func79_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func79_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func79_in_sel:6;
        /** func79_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func79_in_inv_sel:1;
        /** sig79_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig79_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func79_in_sel_cfg_reg_t;

/** Type of func80_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func80_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func81_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func81_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func81_in_sel:6;
        /** func81_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func81_in_inv_sel:1;
        /** sig81_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig81_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func81_in_sel_cfg_reg_t;

/** Type of func82_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func82_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func82_in_sel:6;
        /** func82_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func82_in_inv_sel:1;
        /** sig82_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig82_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func82_in_sel_cfg_reg_t;

/** Type of func87_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func87_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func87_in_sel:6;
        /** func87_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func87_in_inv_sel:1;
        /** sig87_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig87_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func87_in_sel_cfg_reg_t;

/** Type of func88_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func88_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func88_in_sel:6;
        /** func88_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func88_in_inv_sel:1;
        /** sig88_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig88_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func88_in_sel_cfg_reg_t;

/** Type of func89_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func89_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func90_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func91_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func92_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func93_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func94_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func95_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func97_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func97_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func98_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func99_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func100_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func101_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func102_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func103_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func104_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func105_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func106_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func107_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func108_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func109_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func110_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func111_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func112_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func113_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func114_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func115_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func115_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func115_in_sel:6;
        /** func115_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func115_in_inv_sel:1;
        /** sig115_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig115_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func115_in_sel_cfg_reg_t;

/** Type of func116_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func116_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func116_in_sel:6;
        /** func116_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func116_in_inv_sel:1;
        /** sig116_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig116_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func116_in_sel_cfg_reg_t;

/** Type of func117_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func117_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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
        /** func118_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
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

/** Type of func119_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func119_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func119_in_sel:6;
        /** func119_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func119_in_inv_sel:1;
        /** sig119_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig119_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func119_in_sel_cfg_reg_t;

/** Type of func120_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func120_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func120_in_sel:6;
        /** func120_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func120_in_inv_sel:1;
        /** sig120_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig120_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func120_in_sel_cfg_reg_t;

/** Type of func121_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func121_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func121_in_sel:6;
        /** func121_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func121_in_inv_sel:1;
        /** sig121_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig121_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func121_in_sel_cfg_reg_t;

/** Type of func122_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func122_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func122_in_sel:6;
        /** func122_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func122_in_inv_sel:1;
        /** sig122_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig122_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func122_in_sel_cfg_reg_t;

/** Type of func123_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func123_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func123_in_sel:6;
        /** func123_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func123_in_inv_sel:1;
        /** sig123_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig123_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func123_in_sel_cfg_reg_t;

/** Type of func124_in_sel_cfg register
 *  GPIO input function configuration register
 */
typedef union {
    struct {
        /** func124_in_sel : R/W; bitpos: [5:0]; default: 48;
         *  set this value: s=0-29: connect GPIO[s] to this port. s=0x20: set this port always
         *  high level. s=0x30: set this port always low level.
         */
        uint32_t func124_in_sel:6;
        /** func124_in_inv_sel : R/W; bitpos: [6]; default: 0;
         *  set this bit to invert input signal. 1:invert. 0:not invert.
         */
        uint32_t func124_in_inv_sel:1;
        /** sig124_in_sel : R/W; bitpos: [7]; default: 0;
         *  set this bit to bypass GPIO. 1:do not bypass GPIO. 0:bypass GPIO.
         */
        uint32_t sig124_in_sel:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_func124_in_sel_cfg_reg_t;

/** Type of funcn_out_sel_cfg register
 *  GPIO output function select register
 */
typedef union {
    struct {
        /** funcn_out_sel : R/W/SC; bitpos: [8:0]; default: 256;
         *  The value of the bits: 0<=s<=256. Set the value to select output signal. s=0-127:
         *  output of GPIO[n] equals input of peripheral[s]. s=128: output of GPIO[n] equals
         *  GPIO_OUT_REG[n].
         */
        uint32_t funcn_out_sel:9;
        /** funcn_out_inv_sel : R/W/SC; bitpos: [9]; default: 0;
         *  set this bit to invert output signal.1:invert.0:not invert.
         */
        uint32_t funcn_out_inv_sel:1;
        /** funcn_oe_sel : R/W; bitpos: [10]; default: 0;
         *  set this bit to select output enable signal.1:use GPIO_ENABLE_REG[n] as output
         *  enable signal.0:use peripheral output enable signal.
         */
        uint32_t funcn_oe_sel:1;
        /** funcn_oe_inv_sel : R/W; bitpos: [11]; default: 0;
         *  set this bit to invert output enable signal.1:invert.0:not invert.
         */
        uint32_t funcn_oe_inv_sel:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} gpio_funcn_out_sel_cfg_reg_t;

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

/** Type of date register
 *  GPIO version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37785872;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_date_reg_t;


typedef struct {
    volatile gpio_strap_reg_t strap;
    volatile gpio_out_reg_t out;
    volatile gpio_out_w1ts_reg_t out_w1ts;
    volatile gpio_out_w1tc_reg_t out_w1tc;
    uint32_t reserved_010[9];
    volatile gpio_enable_reg_t enable;
    volatile gpio_enable_w1ts_reg_t enable_w1ts;
    volatile gpio_enable_w1tc_reg_t enable_w1tc;
    uint32_t reserved_040[9];
    volatile gpio_in_reg_t in;
    uint32_t reserved_068[3];
    volatile gpio_status_reg_t status;
    volatile gpio_status_w1ts_reg_t status_w1ts;
    volatile gpio_status_w1tc_reg_t status_w1tc;
    uint32_t reserved_080[9];
    volatile gpio_procpu_int_reg_t procpu_int;
    volatile gpio_procpu_nmi_int_reg_t procpu_nmi_int;
    uint32_t reserved_0ac[6];
    volatile gpio_status_next_reg_t status_next;
    uint32_t reserved_0c8[3];
    volatile gpio_pinn_reg_t pinn[30];
    uint32_t reserved_14c[98];
    volatile gpio_func0_in_sel_cfg_reg_t func0_in_sel_cfg;
    uint32_t reserved_2d8[5];
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
    uint32_t reserved_31c;
    volatile gpio_func19_in_sel_cfg_reg_t func19_in_sel_cfg;
    uint32_t reserved_324[8];
    volatile gpio_func28_in_sel_cfg_reg_t func28_in_sel_cfg;
    volatile gpio_func29_in_sel_cfg_reg_t func29_in_sel_cfg;
    volatile gpio_func30_in_sel_cfg_reg_t func30_in_sel_cfg;
    volatile gpio_func31_in_sel_cfg_reg_t func31_in_sel_cfg;
    volatile gpio_func32_in_sel_cfg_reg_t func32_in_sel_cfg;
    volatile gpio_func33_in_sel_cfg_reg_t func33_in_sel_cfg;
    volatile gpio_func34_in_sel_cfg_reg_t func34_in_sel_cfg;
    volatile gpio_func35_in_sel_cfg_reg_t func35_in_sel_cfg;
    uint32_t reserved_364[4];
    volatile gpio_func40_in_sel_cfg_reg_t func40_in_sel_cfg;
    volatile gpio_func41_in_sel_cfg_reg_t func41_in_sel_cfg;
    volatile gpio_func42_in_sel_cfg_reg_t func42_in_sel_cfg;
    uint32_t reserved_380[2];
    volatile gpio_func45_in_sel_cfg_reg_t func45_in_sel_cfg;
    volatile gpio_func46_in_sel_cfg_reg_t func46_in_sel_cfg;
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
    uint32_t reserved_3b8[6];
    volatile gpio_func63_in_sel_cfg_reg_t func63_in_sel_cfg;
    volatile gpio_func64_in_sel_cfg_reg_t func64_in_sel_cfg;
    volatile gpio_func65_in_sel_cfg_reg_t func65_in_sel_cfg;
    volatile gpio_func66_in_sel_cfg_reg_t func66_in_sel_cfg;
    volatile gpio_func67_in_sel_cfg_reg_t func67_in_sel_cfg;
    volatile gpio_func68_in_sel_cfg_reg_t func68_in_sel_cfg;
    volatile gpio_func69_in_sel_cfg_reg_t func69_in_sel_cfg;
    volatile gpio_func70_in_sel_cfg_reg_t func70_in_sel_cfg;
    volatile gpio_func71_in_sel_cfg_reg_t func71_in_sel_cfg;
    volatile gpio_func72_in_sel_cfg_reg_t func72_in_sel_cfg;
    volatile gpio_func73_in_sel_cfg_reg_t func73_in_sel_cfg;
    uint32_t reserved_3fc[3];
    volatile gpio_func77_in_sel_cfg_reg_t func77_in_sel_cfg;
    volatile gpio_func78_in_sel_cfg_reg_t func78_in_sel_cfg;
    volatile gpio_func79_in_sel_cfg_reg_t func79_in_sel_cfg;
    volatile gpio_func80_in_sel_cfg_reg_t func80_in_sel_cfg;
    volatile gpio_func81_in_sel_cfg_reg_t func81_in_sel_cfg;
    volatile gpio_func82_in_sel_cfg_reg_t func82_in_sel_cfg;
    uint32_t reserved_420[4];
    volatile gpio_func87_in_sel_cfg_reg_t func87_in_sel_cfg;
    volatile gpio_func88_in_sel_cfg_reg_t func88_in_sel_cfg;
    volatile gpio_func89_in_sel_cfg_reg_t func89_in_sel_cfg;
    volatile gpio_func90_in_sel_cfg_reg_t func90_in_sel_cfg;
    volatile gpio_func91_in_sel_cfg_reg_t func91_in_sel_cfg;
    volatile gpio_func92_in_sel_cfg_reg_t func92_in_sel_cfg;
    volatile gpio_func93_in_sel_cfg_reg_t func93_in_sel_cfg;
    volatile gpio_func94_in_sel_cfg_reg_t func94_in_sel_cfg;
    volatile gpio_func95_in_sel_cfg_reg_t func95_in_sel_cfg;
    uint32_t reserved_454;
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
    volatile gpio_func115_in_sel_cfg_reg_t func115_in_sel_cfg;
    volatile gpio_func116_in_sel_cfg_reg_t func116_in_sel_cfg;
    volatile gpio_func117_in_sel_cfg_reg_t func117_in_sel_cfg;
    volatile gpio_func118_in_sel_cfg_reg_t func118_in_sel_cfg;
    volatile gpio_func119_in_sel_cfg_reg_t func119_in_sel_cfg;
    volatile gpio_func120_in_sel_cfg_reg_t func120_in_sel_cfg;
    volatile gpio_func121_in_sel_cfg_reg_t func121_in_sel_cfg;
    volatile gpio_func122_in_sel_cfg_reg_t func122_in_sel_cfg;
    volatile gpio_func123_in_sel_cfg_reg_t func123_in_sel_cfg;
    volatile gpio_func124_in_sel_cfg_reg_t func124_in_sel_cfg;
    uint32_t reserved_4c8[387];
    volatile gpio_funcn_out_sel_cfg_reg_t funcn_out_sel_cfg[30];
    uint32_t reserved_b4c[171];
    volatile gpio_clock_gate_reg_t clock_gate;
    volatile gpio_date_reg_t date;
} gpio_dev_t;

extern gpio_dev_t GPIO;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_dev_t) == 0xe00, "Invalid size of gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
