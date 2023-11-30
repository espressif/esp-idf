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

/** Group: configure_register */
/** Type of out_data register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_out_data : R/W/WTC; bitpos: [7:0]; default: 0;
         *  set lp gpio output data
         */
        uint32_t lp_gpio_out_data:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_out_data_reg_t;

/** Type of out_data_w1ts register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_out_data_w1ts : WT; bitpos: [7:0]; default: 0;
         *  set one time output data
         */
        uint32_t lp_gpio_out_data_w1ts:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_out_data_w1ts_reg_t;

/** Type of out_data_w1tc register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_out_data_w1tc : WT; bitpos: [7:0]; default: 0;
         *  clear one time output data
         */
        uint32_t lp_gpio_out_data_w1tc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_out_data_w1tc_reg_t;

/** Type of out_enable register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_enable : R/W/WTC; bitpos: [7:0]; default: 0;
         *  set lp gpio output data
         */
        uint32_t lp_gpio_enable:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_out_enable_reg_t;

/** Type of out_enable_w1ts register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_enable_w1ts : WT; bitpos: [7:0]; default: 0;
         *  set one time output data
         */
        uint32_t lp_gpio_enable_w1ts:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_out_enable_w1ts_reg_t;

/** Type of out_enable_w1tc register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_enable_w1tc : WT; bitpos: [7:0]; default: 0;
         *  clear one time output data
         */
        uint32_t lp_gpio_enable_w1tc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_out_enable_w1tc_reg_t;

/** Type of status register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_status_interrupt : R/W/WTC; bitpos: [7:0]; default: 0;
         *  set lp gpio output data
         */
        uint32_t lp_gpio_status_interrupt:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_status_reg_t;

/** Type of status_w1ts register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_status_w1ts : WT; bitpos: [7:0]; default: 0;
         *  set one time output data
         */
        uint32_t lp_gpio_status_w1ts:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_status_w1ts_reg_t;

/** Type of status_w1tc register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_status_w1tc : WT; bitpos: [7:0]; default: 0;
         *  clear one time output data
         */
        uint32_t lp_gpio_status_w1tc:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_status_w1tc_reg_t;

/** Type of in register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_in_data_next : RO; bitpos: [7:0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_in_data_next:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_in_reg_t;

/** Type of pin n register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_sync_bypass : R/W; bitpos: [1:0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_sync_bypass:2;
        /** lp_gpio_pad_driver : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_pad_driver:1;
        /** lp_gpio_edge_wakeup_clr : WT; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_edge_wakeup_clr:1;
        uint32_t reserved_4:3;
        /** lp_gpio_int_type : R/W; bitpos: [9:7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_int_type:3;
        /** lp_gpio_wakeup_enable : R/W; bitpos: [10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_wakeup_enable:1;
        /** lp_gpio_filter_en : R/W; bitpos: [11]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_filter_en:1;
        uint32_t reserved_12:20;
    };
    uint32_t val;
} lp_io_pin_reg_t;

/** Type of gpio n register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_mcu_oe:1;
        /** lp_gpio_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_slp_sel:1;
        /** lp_gpio_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_mcu_wpd:1;
        /** lp_gpio_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_mcu_wpu:1;
        /** lp_gpio_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_mcu_ie:1;
        /** lp_gpio_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_mcu_drv:2;
        /** lp_gpio_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_fun_wpd:1;
        /** lp_gpio_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_fun_wpu:1;
        /** lp_gpio_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_fun_ie:1;
        /** lp_gpio_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_fun_drv:2;
        /** lp_gpio_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio_reg_t;

/** Type of gpio1 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio1_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_mcu_oe:1;
        /** lp_gpio1_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_slp_sel:1;
        /** lp_gpio1_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_mcu_wpd:1;
        /** lp_gpio1_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_mcu_wpu:1;
        /** lp_gpio1_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_mcu_ie:1;
        /** lp_gpio1_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_mcu_drv:2;
        /** lp_gpio1_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_fun_wpd:1;
        /** lp_gpio1_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_fun_wpu:1;
        /** lp_gpio1_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_fun_ie:1;
        /** lp_gpio1_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_fun_drv:2;
        /** lp_gpio1_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio1_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio1_reg_t;

/** Type of gpio2 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio2_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_mcu_oe:1;
        /** lp_gpio2_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_slp_sel:1;
        /** lp_gpio2_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_mcu_wpd:1;
        /** lp_gpio2_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_mcu_wpu:1;
        /** lp_gpio2_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_mcu_ie:1;
        /** lp_gpio2_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_mcu_drv:2;
        /** lp_gpio2_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_fun_wpd:1;
        /** lp_gpio2_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_fun_wpu:1;
        /** lp_gpio2_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_fun_ie:1;
        /** lp_gpio2_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_fun_drv:2;
        /** lp_gpio2_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio2_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio2_reg_t;

/** Type of gpio3 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio3_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_mcu_oe:1;
        /** lp_gpio3_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_slp_sel:1;
        /** lp_gpio3_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_mcu_wpd:1;
        /** lp_gpio3_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_mcu_wpu:1;
        /** lp_gpio3_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_mcu_ie:1;
        /** lp_gpio3_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_mcu_drv:2;
        /** lp_gpio3_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_fun_wpd:1;
        /** lp_gpio3_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_fun_wpu:1;
        /** lp_gpio3_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_fun_ie:1;
        /** lp_gpio3_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_fun_drv:2;
        /** lp_gpio3_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio3_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio3_reg_t;

/** Type of gpio4 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio4_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_mcu_oe:1;
        /** lp_gpio4_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_slp_sel:1;
        /** lp_gpio4_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_mcu_wpd:1;
        /** lp_gpio4_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_mcu_wpu:1;
        /** lp_gpio4_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_mcu_ie:1;
        /** lp_gpio4_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_mcu_drv:2;
        /** lp_gpio4_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_fun_wpd:1;
        /** lp_gpio4_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_fun_wpu:1;
        /** lp_gpio4_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_fun_ie:1;
        /** lp_gpio4_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_fun_drv:2;
        /** lp_gpio4_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio4_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio4_reg_t;

/** Type of gpio5 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio5_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_mcu_oe:1;
        /** lp_gpio5_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_slp_sel:1;
        /** lp_gpio5_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_mcu_wpd:1;
        /** lp_gpio5_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_mcu_wpu:1;
        /** lp_gpio5_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_mcu_ie:1;
        /** lp_gpio5_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_mcu_drv:2;
        /** lp_gpio5_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_fun_wpd:1;
        /** lp_gpio5_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_fun_wpu:1;
        /** lp_gpio5_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_fun_ie:1;
        /** lp_gpio5_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_fun_drv:2;
        /** lp_gpio5_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio5_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio5_reg_t;

/** Type of gpio6 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio6_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_mcu_oe:1;
        /** lp_gpio6_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_slp_sel:1;
        /** lp_gpio6_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_mcu_wpd:1;
        /** lp_gpio6_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_mcu_wpu:1;
        /** lp_gpio6_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_mcu_ie:1;
        /** lp_gpio6_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_mcu_drv:2;
        /** lp_gpio6_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_fun_wpd:1;
        /** lp_gpio6_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_fun_wpu:1;
        /** lp_gpio6_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_fun_ie:1;
        /** lp_gpio6_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_fun_drv:2;
        /** lp_gpio6_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio6_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio6_reg_t;

/** Type of gpio7 register
 *  need des
 */
typedef union {
    struct {
        /** lp_gpio7_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_mcu_oe:1;
        /** lp_gpio7_slp_sel : R/W; bitpos: [1]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_slp_sel:1;
        /** lp_gpio7_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_mcu_wpd:1;
        /** lp_gpio7_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_mcu_wpu:1;
        /** lp_gpio7_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_mcu_ie:1;
        /** lp_gpio7_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_mcu_drv:2;
        /** lp_gpio7_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_fun_wpd:1;
        /** lp_gpio7_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_fun_wpu:1;
        /** lp_gpio7_fun_ie : R/W; bitpos: [9]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_fun_ie:1;
        /** lp_gpio7_fun_drv : R/W; bitpos: [11:10]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_fun_drv:2;
        /** lp_gpio7_mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  need des
         */
        uint32_t lp_gpio7_mcu_sel:3;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} lp_io_gpio7_reg_t;

/** Type of status_interrupt register
 *  need des
 */
typedef union {
    struct {
        /** status_interrupt_next : RO; bitpos: [7:0]; default: 0;
         *  need des
         */
        uint32_t status_interrupt_next:8;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} lp_io_status_interrupt_reg_t;

/** Type of debug_sel0 register
 *  need des
 */
typedef union {
    struct {
        /** lp_debug_sel0 : R/W; bitpos: [6:0]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel0:7;
        /** lp_debug_sel1 : R/W; bitpos: [13:7]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel1:7;
        /** lp_debug_sel2 : R/W; bitpos: [20:14]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel2:7;
        /** lp_debug_sel3 : R/W; bitpos: [27:21]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel3:7;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} lp_io_debug_sel0_reg_t;

/** Type of debug_sel1 register
 *  need des
 */
typedef union {
    struct {
        /** lp_debug_sel4 : R/W; bitpos: [6:0]; default: 0;
         *  need des
         */
        uint32_t lp_debug_sel4:7;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} lp_io_debug_sel1_reg_t;

/** Type of lpi2c register
 *  need des
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** lp_i2c_sda_ie : R/W; bitpos: [30]; default: 1;
         *  need des
         */
        uint32_t lp_i2c_sda_ie:1;
        /** lp_i2c_scl_ie : R/W; bitpos: [31]; default: 1;
         *  need des
         */
        uint32_t lp_i2c_scl_ie:1;
    };
    uint32_t val;
} lp_io_lpi2c_reg_t;

/** Type of date register
 *  need des
 */
typedef union {
    struct {
        /** lp_io_date : R/W; bitpos: [30:0]; default: 35660032;
         *  need des
         */
        uint32_t lp_io_date:31;
        /** clk_en : R/W; bitpos: [31]; default: 0;
         *  need des
         */
        uint32_t clk_en:1;
    };
    uint32_t val;
} lp_io_date_reg_t;


typedef struct lp_io_dev_t {
    volatile lp_io_out_data_reg_t out_data;
    volatile lp_io_out_data_w1ts_reg_t out_data_w1ts;
    volatile lp_io_out_data_w1tc_reg_t out_data_w1tc;
    volatile lp_io_out_enable_reg_t out_enable;
    volatile lp_io_out_enable_w1ts_reg_t out_enable_w1ts;
    volatile lp_io_out_enable_w1tc_reg_t out_enable_w1tc;
    volatile lp_io_status_reg_t status;
    volatile lp_io_status_w1ts_reg_t status_w1ts;
    volatile lp_io_status_w1tc_reg_t status_w1tc;
    volatile lp_io_in_reg_t in;
    volatile lp_io_pin_reg_t pin[8];
    volatile lp_io_gpio_reg_t gpio[8];
    volatile lp_io_status_interrupt_reg_t status_interrupt;
    volatile lp_io_debug_sel0_reg_t debug_sel0;
    volatile lp_io_debug_sel1_reg_t debug_sel1;
    volatile lp_io_lpi2c_reg_t lpi2c;
    uint32_t reserved_078[225];
    volatile lp_io_date_reg_t date;
} lp_io_dev_t;

extern lp_io_dev_t LP_IO;

#ifndef __cplusplus
_Static_assert(sizeof(lp_io_dev_t) == 0x400, "Invalid size of lp_io_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
