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

/** Group: IOMUX Register */
/** Type of gpio0 register
 *  iomux control register for gpio0
 */
typedef union {
    struct {
        /** gpio0_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio0_mcu_oe:1;
        /** gpio0_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio0_slp_sel:1;
        /** gpio0_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio0_mcu_wpd:1;
        /** gpio0_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio0_mcu_wpu:1;
        /** gpio0_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio0_mcu_ie:1;
        /** gpio0_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio0_mcu_drv:2;
        /** gpio0_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio0_fun_wpd:1;
        /** gpio0_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio0_fun_wpu:1;
        /** gpio0_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio0_fun_ie:1;
        /** gpio0_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio0_fun_drv:2;
        /** gpio0_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio0_mcu_sel:3;
        /** gpio0_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio0_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio0_reg_t;

/** Type of gpio1 register
 *  iomux control register for gpio1
 */
typedef union {
    struct {
        /** gpio1_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio1_mcu_oe:1;
        /** gpio1_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio1_slp_sel:1;
        /** gpio1_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio1_mcu_wpd:1;
        /** gpio1_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio1_mcu_wpu:1;
        /** gpio1_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio1_mcu_ie:1;
        /** gpio1_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio1_mcu_drv:2;
        /** gpio1_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio1_fun_wpd:1;
        /** gpio1_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio1_fun_wpu:1;
        /** gpio1_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio1_fun_ie:1;
        /** gpio1_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio1_fun_drv:2;
        /** gpio1_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio1_mcu_sel:3;
        /** gpio1_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio1_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio1_reg_t;

/** Type of gpio2 register
 *  iomux control register for gpio2
 */
typedef union {
    struct {
        /** gpio2_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio2_mcu_oe:1;
        /** gpio2_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio2_slp_sel:1;
        /** gpio2_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio2_mcu_wpd:1;
        /** gpio2_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio2_mcu_wpu:1;
        /** gpio2_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio2_mcu_ie:1;
        /** gpio2_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio2_mcu_drv:2;
        /** gpio2_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio2_fun_wpd:1;
        /** gpio2_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio2_fun_wpu:1;
        /** gpio2_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio2_fun_ie:1;
        /** gpio2_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio2_fun_drv:2;
        /** gpio2_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio2_mcu_sel:3;
        /** gpio2_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio2_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio2_reg_t;

/** Type of gpio3 register
 *  iomux control register for gpio3
 */
typedef union {
    struct {
        /** gpio3_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio3_mcu_oe:1;
        /** gpio3_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio3_slp_sel:1;
        /** gpio3_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio3_mcu_wpd:1;
        /** gpio3_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio3_mcu_wpu:1;
        /** gpio3_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio3_mcu_ie:1;
        /** gpio3_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio3_mcu_drv:2;
        /** gpio3_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio3_fun_wpd:1;
        /** gpio3_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio3_fun_wpu:1;
        /** gpio3_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio3_fun_ie:1;
        /** gpio3_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio3_fun_drv:2;
        /** gpio3_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio3_mcu_sel:3;
        /** gpio3_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio3_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio3_reg_t;

/** Type of gpio4 register
 *  iomux control register for gpio4
 */
typedef union {
    struct {
        /** gpio4_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio4_mcu_oe:1;
        /** gpio4_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio4_slp_sel:1;
        /** gpio4_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio4_mcu_wpd:1;
        /** gpio4_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio4_mcu_wpu:1;
        /** gpio4_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio4_mcu_ie:1;
        /** gpio4_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio4_mcu_drv:2;
        /** gpio4_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio4_fun_wpd:1;
        /** gpio4_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio4_fun_wpu:1;
        /** gpio4_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio4_fun_ie:1;
        /** gpio4_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio4_fun_drv:2;
        /** gpio4_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio4_mcu_sel:3;
        /** gpio4_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio4_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio4_reg_t;

/** Type of gpio5 register
 *  iomux control register for gpio5
 */
typedef union {
    struct {
        /** gpio5_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio5_mcu_oe:1;
        /** gpio5_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio5_slp_sel:1;
        /** gpio5_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio5_mcu_wpd:1;
        /** gpio5_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio5_mcu_wpu:1;
        /** gpio5_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio5_mcu_ie:1;
        /** gpio5_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio5_mcu_drv:2;
        /** gpio5_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio5_fun_wpd:1;
        /** gpio5_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio5_fun_wpu:1;
        /** gpio5_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio5_fun_ie:1;
        /** gpio5_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio5_fun_drv:2;
        /** gpio5_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio5_mcu_sel:3;
        /** gpio5_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio5_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio5_reg_t;

/** Type of gpio6 register
 *  iomux control register for gpio6
 */
typedef union {
    struct {
        /** gpio6_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio6_mcu_oe:1;
        /** gpio6_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio6_slp_sel:1;
        /** gpio6_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio6_mcu_wpd:1;
        /** gpio6_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio6_mcu_wpu:1;
        /** gpio6_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio6_mcu_ie:1;
        /** gpio6_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio6_mcu_drv:2;
        /** gpio6_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio6_fun_wpd:1;
        /** gpio6_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio6_fun_wpu:1;
        /** gpio6_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio6_fun_ie:1;
        /** gpio6_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio6_fun_drv:2;
        /** gpio6_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio6_mcu_sel:3;
        /** gpio6_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio6_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio6_reg_t;

/** Type of gpio7 register
 *  iomux control register for gpio7
 */
typedef union {
    struct {
        /** gpio7_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio7_mcu_oe:1;
        /** gpio7_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio7_slp_sel:1;
        /** gpio7_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio7_mcu_wpd:1;
        /** gpio7_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio7_mcu_wpu:1;
        /** gpio7_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio7_mcu_ie:1;
        /** gpio7_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio7_mcu_drv:2;
        /** gpio7_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio7_fun_wpd:1;
        /** gpio7_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio7_fun_wpu:1;
        /** gpio7_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio7_fun_ie:1;
        /** gpio7_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio7_fun_drv:2;
        /** gpio7_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio7_mcu_sel:3;
        /** gpio7_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio7_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio7_reg_t;

/** Type of gpio8 register
 *  iomux control register for gpio8
 */
typedef union {
    struct {
        /** gpio8_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio8_mcu_oe:1;
        /** gpio8_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio8_slp_sel:1;
        /** gpio8_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio8_mcu_wpd:1;
        /** gpio8_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio8_mcu_wpu:1;
        /** gpio8_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio8_mcu_ie:1;
        /** gpio8_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio8_mcu_drv:2;
        /** gpio8_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio8_fun_wpd:1;
        /** gpio8_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio8_fun_wpu:1;
        /** gpio8_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio8_fun_ie:1;
        /** gpio8_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio8_fun_drv:2;
        /** gpio8_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio8_mcu_sel:3;
        /** gpio8_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio8_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio8_reg_t;

/** Type of gpio9 register
 *  iomux control register for gpio9
 */
typedef union {
    struct {
        /** gpio9_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio9_mcu_oe:1;
        /** gpio9_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio9_slp_sel:1;
        /** gpio9_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio9_mcu_wpd:1;
        /** gpio9_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio9_mcu_wpu:1;
        /** gpio9_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio9_mcu_ie:1;
        /** gpio9_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio9_mcu_drv:2;
        /** gpio9_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio9_fun_wpd:1;
        /** gpio9_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio9_fun_wpu:1;
        /** gpio9_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio9_fun_ie:1;
        /** gpio9_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio9_fun_drv:2;
        /** gpio9_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio9_mcu_sel:3;
        /** gpio9_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio9_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio9_reg_t;

/** Type of gpio10 register
 *  iomux control register for gpio10
 */
typedef union {
    struct {
        /** gpio10_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio10_mcu_oe:1;
        /** gpio10_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio10_slp_sel:1;
        /** gpio10_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio10_mcu_wpd:1;
        /** gpio10_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio10_mcu_wpu:1;
        /** gpio10_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio10_mcu_ie:1;
        /** gpio10_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio10_mcu_drv:2;
        /** gpio10_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio10_fun_wpd:1;
        /** gpio10_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio10_fun_wpu:1;
        /** gpio10_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio10_fun_ie:1;
        /** gpio10_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio10_fun_drv:2;
        /** gpio10_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio10_mcu_sel:3;
        /** gpio10_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio10_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio10_reg_t;

/** Type of gpio11 register
 *  iomux control register for gpio11
 */
typedef union {
    struct {
        /** gpio11_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio11_mcu_oe:1;
        /** gpio11_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio11_slp_sel:1;
        /** gpio11_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio11_mcu_wpd:1;
        /** gpio11_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio11_mcu_wpu:1;
        /** gpio11_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio11_mcu_ie:1;
        /** gpio11_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio11_mcu_drv:2;
        /** gpio11_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio11_fun_wpd:1;
        /** gpio11_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio11_fun_wpu:1;
        /** gpio11_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio11_fun_ie:1;
        /** gpio11_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio11_fun_drv:2;
        /** gpio11_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio11_mcu_sel:3;
        /** gpio11_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio11_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio11_reg_t;

/** Type of gpio12 register
 *  iomux control register for gpio12
 */
typedef union {
    struct {
        /** gpio12_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio12_mcu_oe:1;
        /** gpio12_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio12_slp_sel:1;
        /** gpio12_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio12_mcu_wpd:1;
        /** gpio12_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio12_mcu_wpu:1;
        /** gpio12_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio12_mcu_ie:1;
        /** gpio12_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio12_mcu_drv:2;
        /** gpio12_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio12_fun_wpd:1;
        /** gpio12_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio12_fun_wpu:1;
        /** gpio12_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio12_fun_ie:1;
        /** gpio12_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio12_fun_drv:2;
        /** gpio12_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio12_mcu_sel:3;
        /** gpio12_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio12_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio12_reg_t;

/** Type of gpio13 register
 *  iomux control register for gpio13
 */
typedef union {
    struct {
        /** gpio13_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio13_mcu_oe:1;
        /** gpio13_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio13_slp_sel:1;
        /** gpio13_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio13_mcu_wpd:1;
        /** gpio13_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio13_mcu_wpu:1;
        /** gpio13_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio13_mcu_ie:1;
        /** gpio13_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio13_mcu_drv:2;
        /** gpio13_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio13_fun_wpd:1;
        /** gpio13_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio13_fun_wpu:1;
        /** gpio13_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio13_fun_ie:1;
        /** gpio13_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio13_fun_drv:2;
        /** gpio13_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio13_mcu_sel:3;
        /** gpio13_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio13_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio13_reg_t;

/** Type of gpio14 register
 *  iomux control register for gpio14
 */
typedef union {
    struct {
        /** gpio14_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio14_mcu_oe:1;
        /** gpio14_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio14_slp_sel:1;
        /** gpio14_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio14_mcu_wpd:1;
        /** gpio14_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio14_mcu_wpu:1;
        /** gpio14_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio14_mcu_ie:1;
        /** gpio14_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio14_mcu_drv:2;
        /** gpio14_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio14_fun_wpd:1;
        /** gpio14_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio14_fun_wpu:1;
        /** gpio14_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio14_fun_ie:1;
        /** gpio14_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio14_fun_drv:2;
        /** gpio14_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio14_mcu_sel:3;
        /** gpio14_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio14_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio14_reg_t;

/** Type of gpio15 register
 *  iomux control register for gpio15
 */
typedef union {
    struct {
        /** gpio15_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio15_mcu_oe:1;
        /** gpio15_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio15_slp_sel:1;
        /** gpio15_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio15_mcu_wpd:1;
        /** gpio15_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio15_mcu_wpu:1;
        /** gpio15_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio15_mcu_ie:1;
        /** gpio15_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio15_mcu_drv:2;
        /** gpio15_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio15_fun_wpd:1;
        /** gpio15_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio15_fun_wpu:1;
        /** gpio15_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio15_fun_ie:1;
        /** gpio15_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio15_fun_drv:2;
        /** gpio15_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio15_mcu_sel:3;
        /** gpio15_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio15_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio15_reg_t;

/** Type of gpio16 register
 *  iomux control register for gpio16
 */
typedef union {
    struct {
        /** gpio16_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio16_mcu_oe:1;
        /** gpio16_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio16_slp_sel:1;
        /** gpio16_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio16_mcu_wpd:1;
        /** gpio16_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio16_mcu_wpu:1;
        /** gpio16_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio16_mcu_ie:1;
        /** gpio16_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio16_mcu_drv:2;
        /** gpio16_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio16_fun_wpd:1;
        /** gpio16_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio16_fun_wpu:1;
        /** gpio16_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio16_fun_ie:1;
        /** gpio16_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio16_fun_drv:2;
        /** gpio16_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio16_mcu_sel:3;
        /** gpio16_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio16_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio16_reg_t;

/** Type of gpio17 register
 *  iomux control register for gpio17
 */
typedef union {
    struct {
        /** gpio17_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio17_mcu_oe:1;
        /** gpio17_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio17_slp_sel:1;
        /** gpio17_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio17_mcu_wpd:1;
        /** gpio17_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio17_mcu_wpu:1;
        /** gpio17_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio17_mcu_ie:1;
        /** gpio17_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio17_mcu_drv:2;
        /** gpio17_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio17_fun_wpd:1;
        /** gpio17_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio17_fun_wpu:1;
        /** gpio17_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio17_fun_ie:1;
        /** gpio17_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio17_fun_drv:2;
        /** gpio17_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio17_mcu_sel:3;
        /** gpio17_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio17_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio17_reg_t;

/** Type of gpio18 register
 *  iomux control register for gpio18
 */
typedef union {
    struct {
        /** gpio18_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio18_mcu_oe:1;
        /** gpio18_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio18_slp_sel:1;
        /** gpio18_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio18_mcu_wpd:1;
        /** gpio18_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio18_mcu_wpu:1;
        /** gpio18_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio18_mcu_ie:1;
        /** gpio18_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio18_mcu_drv:2;
        /** gpio18_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio18_fun_wpd:1;
        /** gpio18_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio18_fun_wpu:1;
        /** gpio18_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio18_fun_ie:1;
        /** gpio18_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio18_fun_drv:2;
        /** gpio18_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio18_mcu_sel:3;
        /** gpio18_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio18_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio18_reg_t;

/** Type of gpio19 register
 *  iomux control register for gpio19
 */
typedef union {
    struct {
        /** gpio19_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio19_mcu_oe:1;
        /** gpio19_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio19_slp_sel:1;
        /** gpio19_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio19_mcu_wpd:1;
        /** gpio19_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio19_mcu_wpu:1;
        /** gpio19_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio19_mcu_ie:1;
        /** gpio19_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio19_mcu_drv:2;
        /** gpio19_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio19_fun_wpd:1;
        /** gpio19_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio19_fun_wpu:1;
        /** gpio19_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio19_fun_ie:1;
        /** gpio19_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio19_fun_drv:2;
        /** gpio19_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio19_mcu_sel:3;
        /** gpio19_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio19_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio19_reg_t;

/** Type of gpio20 register
 *  iomux control register for gpio20
 */
typedef union {
    struct {
        /** gpio20_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio20_mcu_oe:1;
        /** gpio20_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio20_slp_sel:1;
        /** gpio20_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio20_mcu_wpd:1;
        /** gpio20_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio20_mcu_wpu:1;
        /** gpio20_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio20_mcu_ie:1;
        /** gpio20_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio20_mcu_drv:2;
        /** gpio20_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio20_fun_wpd:1;
        /** gpio20_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio20_fun_wpu:1;
        /** gpio20_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio20_fun_ie:1;
        /** gpio20_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio20_fun_drv:2;
        /** gpio20_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio20_mcu_sel:3;
        /** gpio20_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio20_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio20_reg_t;

/** Type of gpio21 register
 *  iomux control register for gpio21
 */
typedef union {
    struct {
        /** gpio21_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio21_mcu_oe:1;
        /** gpio21_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio21_slp_sel:1;
        /** gpio21_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio21_mcu_wpd:1;
        /** gpio21_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio21_mcu_wpu:1;
        /** gpio21_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio21_mcu_ie:1;
        /** gpio21_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio21_mcu_drv:2;
        /** gpio21_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio21_fun_wpd:1;
        /** gpio21_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio21_fun_wpu:1;
        /** gpio21_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio21_fun_ie:1;
        /** gpio21_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio21_fun_drv:2;
        /** gpio21_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio21_mcu_sel:3;
        /** gpio21_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio21_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio21_reg_t;

/** Type of gpio22 register
 *  iomux control register for gpio22
 */
typedef union {
    struct {
        /** gpio22_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio22_mcu_oe:1;
        /** gpio22_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio22_slp_sel:1;
        /** gpio22_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio22_mcu_wpd:1;
        /** gpio22_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio22_mcu_wpu:1;
        /** gpio22_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio22_mcu_ie:1;
        /** gpio22_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio22_mcu_drv:2;
        /** gpio22_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio22_fun_wpd:1;
        /** gpio22_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio22_fun_wpu:1;
        /** gpio22_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio22_fun_ie:1;
        /** gpio22_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio22_fun_drv:2;
        /** gpio22_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio22_mcu_sel:3;
        /** gpio22_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio22_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio22_reg_t;

/** Type of gpio23 register
 *  iomux control register for gpio23
 */
typedef union {
    struct {
        /** gpio23_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio23_mcu_oe:1;
        /** gpio23_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio23_slp_sel:1;
        /** gpio23_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio23_mcu_wpd:1;
        /** gpio23_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio23_mcu_wpu:1;
        /** gpio23_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio23_mcu_ie:1;
        /** gpio23_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio23_mcu_drv:2;
        /** gpio23_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio23_fun_wpd:1;
        /** gpio23_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio23_fun_wpu:1;
        /** gpio23_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio23_fun_ie:1;
        /** gpio23_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio23_fun_drv:2;
        /** gpio23_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio23_mcu_sel:3;
        /** gpio23_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio23_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio23_reg_t;

/** Type of gpio24 register
 *  iomux control register for gpio24
 */
typedef union {
    struct {
        /** gpio24_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio24_mcu_oe:1;
        /** gpio24_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio24_slp_sel:1;
        /** gpio24_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio24_mcu_wpd:1;
        /** gpio24_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio24_mcu_wpu:1;
        /** gpio24_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio24_mcu_ie:1;
        /** gpio24_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio24_mcu_drv:2;
        /** gpio24_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio24_fun_wpd:1;
        /** gpio24_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio24_fun_wpu:1;
        /** gpio24_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio24_fun_ie:1;
        /** gpio24_fun_drv : R/W; bitpos: [11:10]; default: 3;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio24_fun_drv:2;
        /** gpio24_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio24_mcu_sel:3;
        /** gpio24_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio24_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio24_reg_t;

/** Type of gpio25 register
 *  iomux control register for gpio25
 */
typedef union {
    struct {
        /** gpio25_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio25_mcu_oe:1;
        /** gpio25_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio25_slp_sel:1;
        /** gpio25_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio25_mcu_wpd:1;
        /** gpio25_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio25_mcu_wpu:1;
        /** gpio25_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio25_mcu_ie:1;
        /** gpio25_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio25_mcu_drv:2;
        /** gpio25_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio25_fun_wpd:1;
        /** gpio25_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio25_fun_wpu:1;
        /** gpio25_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio25_fun_ie:1;
        /** gpio25_fun_drv : R/W; bitpos: [11:10]; default: 3;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio25_fun_drv:2;
        /** gpio25_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio25_mcu_sel:3;
        /** gpio25_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio25_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio25_reg_t;

/** Type of gpio26 register
 *  iomux control register for gpio26
 */
typedef union {
    struct {
        /** gpio26_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio26_mcu_oe:1;
        /** gpio26_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio26_slp_sel:1;
        /** gpio26_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio26_mcu_wpd:1;
        /** gpio26_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio26_mcu_wpu:1;
        /** gpio26_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio26_mcu_ie:1;
        /** gpio26_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio26_mcu_drv:2;
        /** gpio26_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio26_fun_wpd:1;
        /** gpio26_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio26_fun_wpu:1;
        /** gpio26_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio26_fun_ie:1;
        /** gpio26_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio26_fun_drv:2;
        /** gpio26_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio26_mcu_sel:3;
        /** gpio26_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio26_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio26_reg_t;

/** Type of gpio27 register
 *  iomux control register for gpio27
 */
typedef union {
    struct {
        /** gpio27_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio27_mcu_oe:1;
        /** gpio27_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio27_slp_sel:1;
        /** gpio27_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio27_mcu_wpd:1;
        /** gpio27_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio27_mcu_wpu:1;
        /** gpio27_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio27_mcu_ie:1;
        /** gpio27_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio27_mcu_drv:2;
        /** gpio27_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio27_fun_wpd:1;
        /** gpio27_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio27_fun_wpu:1;
        /** gpio27_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio27_fun_ie:1;
        /** gpio27_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio27_fun_drv:2;
        /** gpio27_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio27_mcu_sel:3;
        /** gpio27_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio27_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio27_reg_t;

/** Type of gpio28 register
 *  iomux control register for gpio28
 */
typedef union {
    struct {
        /** gpio28_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio28_mcu_oe:1;
        /** gpio28_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio28_slp_sel:1;
        /** gpio28_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio28_mcu_wpd:1;
        /** gpio28_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio28_mcu_wpu:1;
        /** gpio28_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio28_mcu_ie:1;
        /** gpio28_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio28_mcu_drv:2;
        /** gpio28_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio28_fun_wpd:1;
        /** gpio28_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio28_fun_wpu:1;
        /** gpio28_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio28_fun_ie:1;
        /** gpio28_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio28_fun_drv:2;
        /** gpio28_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio28_mcu_sel:3;
        /** gpio28_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio28_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio28_reg_t;

/** Type of gpio29 register
 *  iomux control register for gpio29
 */
typedef union {
    struct {
        /** gpio29_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio29_mcu_oe:1;
        /** gpio29_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio29_slp_sel:1;
        /** gpio29_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio29_mcu_wpd:1;
        /** gpio29_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio29_mcu_wpu:1;
        /** gpio29_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio29_mcu_ie:1;
        /** gpio29_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio29_mcu_drv:2;
        /** gpio29_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio29_fun_wpd:1;
        /** gpio29_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio29_fun_wpu:1;
        /** gpio29_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio29_fun_ie:1;
        /** gpio29_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio29_fun_drv:2;
        /** gpio29_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio29_mcu_sel:3;
        /** gpio29_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio29_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio29_reg_t;

/** Type of gpio30 register
 *  iomux control register for gpio30
 */
typedef union {
    struct {
        /** gpio30_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio30_mcu_oe:1;
        /** gpio30_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio30_slp_sel:1;
        /** gpio30_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio30_mcu_wpd:1;
        /** gpio30_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio30_mcu_wpu:1;
        /** gpio30_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio30_mcu_ie:1;
        /** gpio30_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio30_mcu_drv:2;
        /** gpio30_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio30_fun_wpd:1;
        /** gpio30_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio30_fun_wpu:1;
        /** gpio30_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio30_fun_ie:1;
        /** gpio30_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio30_fun_drv:2;
        /** gpio30_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio30_mcu_sel:3;
        /** gpio30_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio30_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio30_reg_t;

/** Type of gpio31 register
 *  iomux control register for gpio31
 */
typedef union {
    struct {
        /** gpio31_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio31_mcu_oe:1;
        /** gpio31_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio31_slp_sel:1;
        /** gpio31_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio31_mcu_wpd:1;
        /** gpio31_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio31_mcu_wpu:1;
        /** gpio31_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio31_mcu_ie:1;
        /** gpio31_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio31_mcu_drv:2;
        /** gpio31_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio31_fun_wpd:1;
        /** gpio31_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio31_fun_wpu:1;
        /** gpio31_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio31_fun_ie:1;
        /** gpio31_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio31_fun_drv:2;
        /** gpio31_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio31_mcu_sel:3;
        /** gpio31_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio31_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio31_reg_t;

/** Type of gpio32 register
 *  iomux control register for gpio32
 */
typedef union {
    struct {
        /** gpio32_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio32_mcu_oe:1;
        /** gpio32_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio32_slp_sel:1;
        /** gpio32_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio32_mcu_wpd:1;
        /** gpio32_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio32_mcu_wpu:1;
        /** gpio32_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio32_mcu_ie:1;
        /** gpio32_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio32_mcu_drv:2;
        /** gpio32_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio32_fun_wpd:1;
        /** gpio32_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio32_fun_wpu:1;
        /** gpio32_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio32_fun_ie:1;
        /** gpio32_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio32_fun_drv:2;
        /** gpio32_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio32_mcu_sel:3;
        /** gpio32_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio32_filter_en:1;
        /** gpio32_rue_i3c : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t gpio32_rue_i3c:1;
        /** gpio32_ru_i3c : R/W; bitpos: [18:17]; default: 0;
         *  NA
         */
        uint32_t gpio32_ru_i3c:2;
        /** gpio32_rue_sel_i3c : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t gpio32_rue_sel_i3c:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} io_mux_gpio32_reg_t;

/** Type of gpio33 register
 *  iomux control register for gpio33
 */
typedef union {
    struct {
        /** gpio33_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio33_mcu_oe:1;
        /** gpio33_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio33_slp_sel:1;
        /** gpio33_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio33_mcu_wpd:1;
        /** gpio33_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio33_mcu_wpu:1;
        /** gpio33_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio33_mcu_ie:1;
        /** gpio33_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio33_mcu_drv:2;
        /** gpio33_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio33_fun_wpd:1;
        /** gpio33_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio33_fun_wpu:1;
        /** gpio33_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio33_fun_ie:1;
        /** gpio33_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio33_fun_drv:2;
        /** gpio33_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio33_mcu_sel:3;
        /** gpio33_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio33_filter_en:1;
        /** gpio33_rue_i3c : R/W; bitpos: [16]; default: 0;
         *  NA
         */
        uint32_t gpio33_rue_i3c:1;
        /** gpio33_ru_i3c : R/W; bitpos: [18:17]; default: 0;
         *  NA
         */
        uint32_t gpio33_ru_i3c:2;
        /** gpio33_rue_sel_i3c : R/W; bitpos: [19]; default: 0;
         *  NA
         */
        uint32_t gpio33_rue_sel_i3c:1;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} io_mux_gpio33_reg_t;

/** Type of gpio34 register
 *  iomux control register for gpio34
 */
typedef union {
    struct {
        /** gpio34_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio34_mcu_oe:1;
        /** gpio34_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio34_slp_sel:1;
        /** gpio34_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio34_mcu_wpd:1;
        /** gpio34_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio34_mcu_wpu:1;
        /** gpio34_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio34_mcu_ie:1;
        /** gpio34_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio34_mcu_drv:2;
        /** gpio34_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio34_fun_wpd:1;
        /** gpio34_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio34_fun_wpu:1;
        /** gpio34_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio34_fun_ie:1;
        /** gpio34_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio34_fun_drv:2;
        /** gpio34_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio34_mcu_sel:3;
        /** gpio34_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio34_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio34_reg_t;

/** Type of gpio35 register
 *  iomux control register for gpio35
 */
typedef union {
    struct {
        /** gpio35_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio35_mcu_oe:1;
        /** gpio35_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio35_slp_sel:1;
        /** gpio35_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio35_mcu_wpd:1;
        /** gpio35_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio35_mcu_wpu:1;
        /** gpio35_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio35_mcu_ie:1;
        /** gpio35_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio35_mcu_drv:2;
        /** gpio35_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio35_fun_wpd:1;
        /** gpio35_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio35_fun_wpu:1;
        /** gpio35_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio35_fun_ie:1;
        /** gpio35_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio35_fun_drv:2;
        /** gpio35_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio35_mcu_sel:3;
        /** gpio35_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio35_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio35_reg_t;

/** Type of gpio36 register
 *  iomux control register for gpio36
 */
typedef union {
    struct {
        /** gpio36_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio36_mcu_oe:1;
        /** gpio36_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio36_slp_sel:1;
        /** gpio36_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio36_mcu_wpd:1;
        /** gpio36_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio36_mcu_wpu:1;
        /** gpio36_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio36_mcu_ie:1;
        /** gpio36_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio36_mcu_drv:2;
        /** gpio36_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio36_fun_wpd:1;
        /** gpio36_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio36_fun_wpu:1;
        /** gpio36_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio36_fun_ie:1;
        /** gpio36_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio36_fun_drv:2;
        /** gpio36_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio36_mcu_sel:3;
        /** gpio36_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio36_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio36_reg_t;

/** Type of gpio37 register
 *  iomux control register for gpio37
 */
typedef union {
    struct {
        /** gpio37_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio37_mcu_oe:1;
        /** gpio37_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio37_slp_sel:1;
        /** gpio37_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio37_mcu_wpd:1;
        /** gpio37_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio37_mcu_wpu:1;
        /** gpio37_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio37_mcu_ie:1;
        /** gpio37_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio37_mcu_drv:2;
        /** gpio37_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio37_fun_wpd:1;
        /** gpio37_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio37_fun_wpu:1;
        /** gpio37_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio37_fun_ie:1;
        /** gpio37_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio37_fun_drv:2;
        /** gpio37_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio37_mcu_sel:3;
        /** gpio37_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio37_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio37_reg_t;

/** Type of gpio38 register
 *  iomux control register for gpio38
 */
typedef union {
    struct {
        /** gpio38_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio38_mcu_oe:1;
        /** gpio38_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio38_slp_sel:1;
        /** gpio38_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio38_mcu_wpd:1;
        /** gpio38_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio38_mcu_wpu:1;
        /** gpio38_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio38_mcu_ie:1;
        /** gpio38_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio38_mcu_drv:2;
        /** gpio38_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio38_fun_wpd:1;
        /** gpio38_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio38_fun_wpu:1;
        /** gpio38_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio38_fun_ie:1;
        /** gpio38_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio38_fun_drv:2;
        /** gpio38_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio38_mcu_sel:3;
        /** gpio38_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio38_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio38_reg_t;

/** Type of gpio39 register
 *  iomux control register for gpio39
 */
typedef union {
    struct {
        /** gpio39_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio39_mcu_oe:1;
        /** gpio39_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio39_slp_sel:1;
        /** gpio39_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio39_mcu_wpd:1;
        /** gpio39_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio39_mcu_wpu:1;
        /** gpio39_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio39_mcu_ie:1;
        /** gpio39_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio39_mcu_drv:2;
        /** gpio39_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio39_fun_wpd:1;
        /** gpio39_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio39_fun_wpu:1;
        /** gpio39_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio39_fun_ie:1;
        /** gpio39_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio39_fun_drv:2;
        /** gpio39_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio39_mcu_sel:3;
        /** gpio39_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio39_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio39_reg_t;

/** Type of gpio40 register
 *  iomux control register for gpio40
 */
typedef union {
    struct {
        /** gpio40_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio40_mcu_oe:1;
        /** gpio40_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio40_slp_sel:1;
        /** gpio40_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio40_mcu_wpd:1;
        /** gpio40_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio40_mcu_wpu:1;
        /** gpio40_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio40_mcu_ie:1;
        /** gpio40_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio40_mcu_drv:2;
        /** gpio40_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio40_fun_wpd:1;
        /** gpio40_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio40_fun_wpu:1;
        /** gpio40_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio40_fun_ie:1;
        /** gpio40_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio40_fun_drv:2;
        /** gpio40_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio40_mcu_sel:3;
        /** gpio40_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio40_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio40_reg_t;

/** Type of gpio41 register
 *  iomux control register for gpio41
 */
typedef union {
    struct {
        /** gpio41_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio41_mcu_oe:1;
        /** gpio41_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio41_slp_sel:1;
        /** gpio41_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio41_mcu_wpd:1;
        /** gpio41_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio41_mcu_wpu:1;
        /** gpio41_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio41_mcu_ie:1;
        /** gpio41_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio41_mcu_drv:2;
        /** gpio41_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio41_fun_wpd:1;
        /** gpio41_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio41_fun_wpu:1;
        /** gpio41_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio41_fun_ie:1;
        /** gpio41_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio41_fun_drv:2;
        /** gpio41_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio41_mcu_sel:3;
        /** gpio41_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio41_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio41_reg_t;

/** Type of gpio42 register
 *  iomux control register for gpio42
 */
typedef union {
    struct {
        /** gpio42_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio42_mcu_oe:1;
        /** gpio42_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio42_slp_sel:1;
        /** gpio42_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio42_mcu_wpd:1;
        /** gpio42_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio42_mcu_wpu:1;
        /** gpio42_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio42_mcu_ie:1;
        /** gpio42_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio42_mcu_drv:2;
        /** gpio42_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio42_fun_wpd:1;
        /** gpio42_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio42_fun_wpu:1;
        /** gpio42_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio42_fun_ie:1;
        /** gpio42_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio42_fun_drv:2;
        /** gpio42_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio42_mcu_sel:3;
        /** gpio42_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio42_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio42_reg_t;

/** Type of gpio43 register
 *  iomux control register for gpio43
 */
typedef union {
    struct {
        /** gpio43_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio43_mcu_oe:1;
        /** gpio43_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio43_slp_sel:1;
        /** gpio43_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio43_mcu_wpd:1;
        /** gpio43_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio43_mcu_wpu:1;
        /** gpio43_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio43_mcu_ie:1;
        /** gpio43_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio43_mcu_drv:2;
        /** gpio43_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio43_fun_wpd:1;
        /** gpio43_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio43_fun_wpu:1;
        /** gpio43_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio43_fun_ie:1;
        /** gpio43_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio43_fun_drv:2;
        /** gpio43_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio43_mcu_sel:3;
        /** gpio43_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio43_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio43_reg_t;

/** Type of gpio44 register
 *  iomux control register for gpio44
 */
typedef union {
    struct {
        /** gpio44_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio44_mcu_oe:1;
        /** gpio44_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio44_slp_sel:1;
        /** gpio44_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio44_mcu_wpd:1;
        /** gpio44_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio44_mcu_wpu:1;
        /** gpio44_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio44_mcu_ie:1;
        /** gpio44_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio44_mcu_drv:2;
        /** gpio44_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio44_fun_wpd:1;
        /** gpio44_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio44_fun_wpu:1;
        /** gpio44_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio44_fun_ie:1;
        /** gpio44_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio44_fun_drv:2;
        /** gpio44_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio44_mcu_sel:3;
        /** gpio44_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio44_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio44_reg_t;

/** Type of gpio45 register
 *  iomux control register for gpio45
 */
typedef union {
    struct {
        /** gpio45_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio45_mcu_oe:1;
        /** gpio45_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio45_slp_sel:1;
        /** gpio45_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio45_mcu_wpd:1;
        /** gpio45_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio45_mcu_wpu:1;
        /** gpio45_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio45_mcu_ie:1;
        /** gpio45_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio45_mcu_drv:2;
        /** gpio45_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio45_fun_wpd:1;
        /** gpio45_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio45_fun_wpu:1;
        /** gpio45_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio45_fun_ie:1;
        /** gpio45_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio45_fun_drv:2;
        /** gpio45_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio45_mcu_sel:3;
        /** gpio45_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio45_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio45_reg_t;

/** Type of gpio46 register
 *  iomux control register for gpio46
 */
typedef union {
    struct {
        /** gpio46_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio46_mcu_oe:1;
        /** gpio46_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio46_slp_sel:1;
        /** gpio46_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio46_mcu_wpd:1;
        /** gpio46_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio46_mcu_wpu:1;
        /** gpio46_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio46_mcu_ie:1;
        /** gpio46_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio46_mcu_drv:2;
        /** gpio46_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio46_fun_wpd:1;
        /** gpio46_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio46_fun_wpu:1;
        /** gpio46_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio46_fun_ie:1;
        /** gpio46_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio46_fun_drv:2;
        /** gpio46_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio46_mcu_sel:3;
        /** gpio46_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio46_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio46_reg_t;

/** Type of gpio47 register
 *  iomux control register for gpio47
 */
typedef union {
    struct {
        /** gpio47_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio47_mcu_oe:1;
        /** gpio47_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio47_slp_sel:1;
        /** gpio47_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio47_mcu_wpd:1;
        /** gpio47_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio47_mcu_wpu:1;
        /** gpio47_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio47_mcu_ie:1;
        /** gpio47_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio47_mcu_drv:2;
        /** gpio47_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio47_fun_wpd:1;
        /** gpio47_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio47_fun_wpu:1;
        /** gpio47_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio47_fun_ie:1;
        /** gpio47_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio47_fun_drv:2;
        /** gpio47_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio47_mcu_sel:3;
        /** gpio47_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio47_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio47_reg_t;

/** Type of gpio48 register
 *  iomux control register for gpio48
 */
typedef union {
    struct {
        /** gpio48_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio48_mcu_oe:1;
        /** gpio48_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio48_slp_sel:1;
        /** gpio48_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio48_mcu_wpd:1;
        /** gpio48_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio48_mcu_wpu:1;
        /** gpio48_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio48_mcu_ie:1;
        /** gpio48_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio48_mcu_drv:2;
        /** gpio48_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio48_fun_wpd:1;
        /** gpio48_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio48_fun_wpu:1;
        /** gpio48_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio48_fun_ie:1;
        /** gpio48_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio48_fun_drv:2;
        /** gpio48_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio48_mcu_sel:3;
        /** gpio48_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio48_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio48_reg_t;

/** Type of gpio49 register
 *  iomux control register for gpio49
 */
typedef union {
    struct {
        /** gpio49_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio49_mcu_oe:1;
        /** gpio49_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio49_slp_sel:1;
        /** gpio49_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio49_mcu_wpd:1;
        /** gpio49_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio49_mcu_wpu:1;
        /** gpio49_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio49_mcu_ie:1;
        /** gpio49_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio49_mcu_drv:2;
        /** gpio49_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio49_fun_wpd:1;
        /** gpio49_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio49_fun_wpu:1;
        /** gpio49_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio49_fun_ie:1;
        /** gpio49_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio49_fun_drv:2;
        /** gpio49_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio49_mcu_sel:3;
        /** gpio49_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio49_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio49_reg_t;

/** Type of gpio50 register
 *  iomux control register for gpio50
 */
typedef union {
    struct {
        /** gpio50_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio50_mcu_oe:1;
        /** gpio50_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio50_slp_sel:1;
        /** gpio50_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio50_mcu_wpd:1;
        /** gpio50_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio50_mcu_wpu:1;
        /** gpio50_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio50_mcu_ie:1;
        /** gpio50_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio50_mcu_drv:2;
        /** gpio50_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio50_fun_wpd:1;
        /** gpio50_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio50_fun_wpu:1;
        /** gpio50_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio50_fun_ie:1;
        /** gpio50_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio50_fun_drv:2;
        /** gpio50_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio50_mcu_sel:3;
        /** gpio50_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio50_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio50_reg_t;

/** Type of gpio51 register
 *  iomux control register for gpio51
 */
typedef union {
    struct {
        /** gpio51_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio51_mcu_oe:1;
        /** gpio51_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio51_slp_sel:1;
        /** gpio51_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio51_mcu_wpd:1;
        /** gpio51_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio51_mcu_wpu:1;
        /** gpio51_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio51_mcu_ie:1;
        /** gpio51_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio51_mcu_drv:2;
        /** gpio51_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio51_fun_wpd:1;
        /** gpio51_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio51_fun_wpu:1;
        /** gpio51_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio51_fun_ie:1;
        /** gpio51_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio51_fun_drv:2;
        /** gpio51_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio51_mcu_sel:3;
        /** gpio51_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio51_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio51_reg_t;

/** Type of gpio52 register
 *  iomux control register for gpio52
 */
typedef union {
    struct {
        /** gpio52_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio52_mcu_oe:1;
        /** gpio52_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio52_slp_sel:1;
        /** gpio52_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio52_mcu_wpd:1;
        /** gpio52_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio52_mcu_wpu:1;
        /** gpio52_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio52_mcu_ie:1;
        /** gpio52_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio52_mcu_drv:2;
        /** gpio52_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio52_fun_wpd:1;
        /** gpio52_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio52_fun_wpu:1;
        /** gpio52_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio52_fun_ie:1;
        /** gpio52_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio52_fun_drv:2;
        /** gpio52_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio52_mcu_sel:3;
        /** gpio52_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio52_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio52_reg_t;

/** Type of gpio53 register
 *  iomux control register for gpio53
 */
typedef union {
    struct {
        /** gpio53_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio53_mcu_oe:1;
        /** gpio53_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio53_slp_sel:1;
        /** gpio53_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio53_mcu_wpd:1;
        /** gpio53_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio53_mcu_wpu:1;
        /** gpio53_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio53_mcu_ie:1;
        /** gpio53_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio53_mcu_drv:2;
        /** gpio53_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio53_fun_wpd:1;
        /** gpio53_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio53_fun_wpu:1;
        /** gpio53_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio53_fun_ie:1;
        /** gpio53_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio53_fun_drv:2;
        /** gpio53_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio53_mcu_sel:3;
        /** gpio53_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio53_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio53_reg_t;

/** Type of gpio54 register
 *  iomux control register for gpio54
 */
typedef union {
    struct {
        /** gpio54_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio54_mcu_oe:1;
        /** gpio54_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio54_slp_sel:1;
        /** gpio54_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio54_mcu_wpd:1;
        /** gpio54_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio54_mcu_wpu:1;
        /** gpio54_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio54_mcu_ie:1;
        /** gpio54_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio54_mcu_drv:2;
        /** gpio54_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio54_fun_wpd:1;
        /** gpio54_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio54_fun_wpu:1;
        /** gpio54_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio54_fun_ie:1;
        /** gpio54_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio54_fun_drv:2;
        /** gpio54_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio54_mcu_sel:3;
        /** gpio54_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio54_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio54_reg_t;

/** Type of gpio55 register
 *  iomux control register for gpio55
 */
typedef union {
    struct {
        /** gpio55_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio55_mcu_oe:1;
        /** gpio55_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio55_slp_sel:1;
        /** gpio55_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio55_mcu_wpd:1;
        /** gpio55_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio55_mcu_wpu:1;
        /** gpio55_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio55_mcu_ie:1;
        /** gpio55_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio55_mcu_drv:2;
        /** gpio55_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio55_fun_wpd:1;
        /** gpio55_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio55_fun_wpu:1;
        /** gpio55_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio55_fun_ie:1;
        /** gpio55_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio55_fun_drv:2;
        /** gpio55_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio55_mcu_sel:3;
        /** gpio55_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio55_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio55_reg_t;

/** Type of gpio56 register
 *  iomux control register for gpio56
 */
typedef union {
    struct {
        /** gpio56_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t gpio56_mcu_oe:1;
        /** gpio56_slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t gpio56_slp_sel:1;
        /** gpio56_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t gpio56_mcu_wpd:1;
        /** gpio56_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t gpio56_mcu_wpu:1;
        /** gpio56_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t gpio56_mcu_ie:1;
        /** gpio56_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t gpio56_mcu_drv:2;
        /** gpio56_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t gpio56_fun_wpd:1;
        /** gpio56_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t gpio56_fun_wpu:1;
        /** gpio56_fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t gpio56_fun_ie:1;
        /** gpio56_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t gpio56_fun_drv:2;
        /** gpio56_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t gpio56_mcu_sel:3;
        /** gpio56_filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t gpio56_filter_en:1;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} io_mux_gpio56_reg_t;

/** Type of date register
 *  iomux version
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2101794;
         *  csv date
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} io_mux_date_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile io_mux_gpio0_reg_t gpio0;
    volatile io_mux_gpio1_reg_t gpio1;
    volatile io_mux_gpio2_reg_t gpio2;
    volatile io_mux_gpio3_reg_t gpio3;
    volatile io_mux_gpio4_reg_t gpio4;
    volatile io_mux_gpio5_reg_t gpio5;
    volatile io_mux_gpio6_reg_t gpio6;
    volatile io_mux_gpio7_reg_t gpio7;
    volatile io_mux_gpio8_reg_t gpio8;
    volatile io_mux_gpio9_reg_t gpio9;
    volatile io_mux_gpio10_reg_t gpio10;
    volatile io_mux_gpio11_reg_t gpio11;
    volatile io_mux_gpio12_reg_t gpio12;
    volatile io_mux_gpio13_reg_t gpio13;
    volatile io_mux_gpio14_reg_t gpio14;
    volatile io_mux_gpio15_reg_t gpio15;
    volatile io_mux_gpio16_reg_t gpio16;
    volatile io_mux_gpio17_reg_t gpio17;
    volatile io_mux_gpio18_reg_t gpio18;
    volatile io_mux_gpio19_reg_t gpio19;
    volatile io_mux_gpio20_reg_t gpio20;
    volatile io_mux_gpio21_reg_t gpio21;
    volatile io_mux_gpio22_reg_t gpio22;
    volatile io_mux_gpio23_reg_t gpio23;
    volatile io_mux_gpio24_reg_t gpio24;
    volatile io_mux_gpio25_reg_t gpio25;
    volatile io_mux_gpio26_reg_t gpio26;
    volatile io_mux_gpio27_reg_t gpio27;
    volatile io_mux_gpio28_reg_t gpio28;
    volatile io_mux_gpio29_reg_t gpio29;
    volatile io_mux_gpio30_reg_t gpio30;
    volatile io_mux_gpio31_reg_t gpio31;
    volatile io_mux_gpio32_reg_t gpio32;
    volatile io_mux_gpio33_reg_t gpio33;
    volatile io_mux_gpio34_reg_t gpio34;
    volatile io_mux_gpio35_reg_t gpio35;
    volatile io_mux_gpio36_reg_t gpio36;
    volatile io_mux_gpio37_reg_t gpio37;
    volatile io_mux_gpio38_reg_t gpio38;
    volatile io_mux_gpio39_reg_t gpio39;
    volatile io_mux_gpio40_reg_t gpio40;
    volatile io_mux_gpio41_reg_t gpio41;
    volatile io_mux_gpio42_reg_t gpio42;
    volatile io_mux_gpio43_reg_t gpio43;
    volatile io_mux_gpio44_reg_t gpio44;
    volatile io_mux_gpio45_reg_t gpio45;
    volatile io_mux_gpio46_reg_t gpio46;
    volatile io_mux_gpio47_reg_t gpio47;
    volatile io_mux_gpio48_reg_t gpio48;
    volatile io_mux_gpio49_reg_t gpio49;
    volatile io_mux_gpio50_reg_t gpio50;
    volatile io_mux_gpio51_reg_t gpio51;
    volatile io_mux_gpio52_reg_t gpio52;
    volatile io_mux_gpio53_reg_t gpio53;
    volatile io_mux_gpio54_reg_t gpio54;
    volatile io_mux_gpio55_reg_t gpio55;
    volatile io_mux_gpio56_reg_t gpio56;
    uint32_t reserved_0e8[7];
    volatile io_mux_date_reg_t date;
} io_mux_dev_t;

extern io_mux_dev_t IO_MUX;

#ifndef __cplusplus
_Static_assert(sizeof(io_mux_dev_t) == 0x108, "Invalid size of io_mux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
