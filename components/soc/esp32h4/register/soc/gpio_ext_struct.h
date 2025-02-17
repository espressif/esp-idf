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

/** Group: SDM Configure Registers */
/** Type of ext_sigmadelta_misc register
 *  MISC Register
 */
typedef union {
    struct {
        /** ext_sigmadelta_clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable the clock for sigma delta modulation.
         *  0: Not enable
         *  1: Enable
         *  %\label{fielddesc:GPIOSDSPISWAP}- [GPIOSD_SPI_SWAP] Reserved.
         */
        uint32_t ext_sigmadelta_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_ext_sigmadelta_misc_reg_t;

/** Type of ext_sigmadeltan register
 *  Duty cycle configuration register for SDM channel n
 */
typedef union {
    struct {
        /** ext_sdn_in : R/W; bitpos: [7:0]; default: 0;
         *  Configures the duty cycle of sigma delta modulation output.
         */
        uint32_t ext_sdn_in:8;
        /** ext_sdn_prescale : R/W; bitpos: [15:8]; default: 255;
         *  Configures the divider value to divide IO MUX operating clock.
         */
        uint32_t ext_sdn_prescale:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gpio_ext_sigmadeltan_reg_t;


/** Group: Glitch filter Configure Registers */
/** Type of ext_glitch_filter_chn register
 *  Glitch Filter Configure Register of Channeln
 */
typedef union {
    struct {
        /** ext_filter_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable channel n of Glitch Filter.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_filter_ch0_en:1;
        /** ext_filter_ch0_input_io_num : R/W; bitpos: [6:1]; default: 0;
         *  Configures to select the input GPIO for Glitch Filter.
         *  0: Select GPIO0
         *  1: Select GPIO1
         *  ......
         *  38: Select GPIO38
         *  39: Select GPIO39
         *  40 ~ 63: Reserved
         */
        uint32_t ext_filter_ch0_input_io_num:6;
        uint32_t reserved_7:1;
        /** ext_filter_ch0_window_thres : R/W; bitpos: [13:8]; default: 0;
         *  Configures the window threshold for Glitch Filter. The window threshold should be
         *  less than or equal to GPIOSD_FILTER_CHn_WINDOW_WIDTH.
         *  %see DOC-4768
         *  Measurement unit: IO MUX operating clock cycle
         */
        uint32_t ext_filter_ch0_window_thres:6;
        /** ext_filter_ch0_window_width : R/W; bitpos: [19:14]; default: 0;
         *  Configures the window width for Glitch Filter. The effective value of window width
         *  is 0 ~ 63.
         *  Measurement unit: IO MUX operating clock cycle
         */
        uint32_t ext_filter_ch0_window_width:6;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gpio_ext_glitch_filter_chn_reg_t;


/** Group: ETM Configuration Registers */
/** Type of ext_etm_event_chn_cfg register
 *  ETM configuration register for channel n
 */
typedef union {
    struct {
        /** ext_etm_chn_event_sel : R/W; bitpos: [5:0]; default: 0;
         *  Configures to select GPIO for ETM event channel.
         *  0: Select GPIO0
         *  1: Select GPIO1
         *  ......
         *  38: Select GPIO38
         *  39: Select GPIO39
         *  40 ~ 63: Reserved
         */
        uint32_t ext_etm_chn_event_sel:6;
        uint32_t reserved_6:1;
        /** ext_etm_chn_event_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable ETM event send.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_chn_event_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_ext_etm_event_chn_cfg_reg_t;

/** Type of ext_etm_task_p0_cfg register
 *  GPIO selection register 0 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio0_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO0.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio0_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio0_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO0 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio0_en:1;
        /** ext_etm_task_gpio1_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO1.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio1_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio1_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO1 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio1_en:1;
        /** ext_etm_task_gpio2_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO2.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio2_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio2_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO2 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio2_en:1;
        /** ext_etm_task_gpio3_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO3.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio3_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio3_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO3 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio3_en:1;
        /** ext_etm_task_gpio4_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO4.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio4_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio4_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO4 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio4_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p0_cfg_reg_t;

/** Type of ext_etm_task_p1_cfg register
 *  GPIO selection register 1 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio5_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO5.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio5_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio5_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO5 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio5_en:1;
        /** ext_etm_task_gpio6_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO6.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio6_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio6_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO6 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio6_en:1;
        /** ext_etm_task_gpio7_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO7.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio7_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio7_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO7 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio7_en:1;
        /** ext_etm_task_gpio8_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO8.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio8_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio8_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO8 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio8_en:1;
        /** ext_etm_task_gpio9_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO9.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio9_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio9_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO9 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio9_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p1_cfg_reg_t;

/** Type of ext_etm_task_p2_cfg register
 *  GPIO selection register 2 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio10_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO10.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio10_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio10_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO10 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio10_en:1;
        /** ext_etm_task_gpio11_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO11.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio11_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio11_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO11 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio11_en:1;
        /** ext_etm_task_gpio12_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO12.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio12_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio12_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO12 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio12_en:1;
        /** ext_etm_task_gpio13_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO13.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio13_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio13_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO13 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio13_en:1;
        /** ext_etm_task_gpio14_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO14.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio14_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio14_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO14 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio14_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p2_cfg_reg_t;

/** Type of ext_etm_task_p3_cfg register
 *  GPIO selection register 3 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio15_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO15.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio15_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio15_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO15 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio15_en:1;
        /** ext_etm_task_gpio16_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO16.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio16_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio16_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO16 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio16_en:1;
        /** ext_etm_task_gpio17_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO17.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio17_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio17_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO17 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio17_en:1;
        /** ext_etm_task_gpio18_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO18.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio18_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio18_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO18 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio18_en:1;
        /** ext_etm_task_gpio19_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO19.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio19_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio19_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO19 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio19_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p3_cfg_reg_t;

/** Type of ext_etm_task_p4_cfg register
 *  GPIO selection register 4 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio20_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO20.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio20_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio20_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO20 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio20_en:1;
        /** ext_etm_task_gpio21_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO21.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio21_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio21_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO21 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio21_en:1;
        /** ext_etm_task_gpio22_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO22.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio22_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio22_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO22 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio22_en:1;
        /** ext_etm_task_gpio23_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO23.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio23_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio23_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO23 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio23_en:1;
        /** ext_etm_task_gpio24_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO24.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio24_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio24_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO24 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio24_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p4_cfg_reg_t;

/** Type of ext_etm_task_p5_cfg register
 *  GPIO selection register 5 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio25_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO25.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio25_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio25_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO25 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio25_en:1;
        /** ext_etm_task_gpio26_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO26.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio26_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio26_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO26 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio26_en:1;
        /** ext_etm_task_gpio27_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO27.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio27_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio27_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO27 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio27_en:1;
        /** ext_etm_task_gpio28_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO28.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio28_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio28_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO28 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio28_en:1;
        /** ext_etm_task_gpio29_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO29.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio29_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio29_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO29 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio29_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p5_cfg_reg_t;

/** Type of ext_etm_task_p6_cfg register
 *  GPIO selection register 6 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio30_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO30.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio30_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio30_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO30 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio30_en:1;
        /** ext_etm_task_gpio31_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO31.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio31_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio31_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO31 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio31_en:1;
        /** ext_etm_task_gpio32_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO32.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio32_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio32_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO32 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio32_en:1;
        /** ext_etm_task_gpio33_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO33.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio33_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio33_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO33 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio33_en:1;
        /** ext_etm_task_gpio34_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO34.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio34_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio34_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO34 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio34_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p6_cfg_reg_t;

/** Type of ext_etm_task_p7_cfg register
 *  GPIO selection register 7 for ETM
 */
typedef union {
    struct {
        /** ext_etm_task_gpio35_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO35.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio35_sel:3;
        uint32_t reserved_3:2;
        /** ext_etm_task_gpio35_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO35 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio35_en:1;
        /** ext_etm_task_gpio36_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO36.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio36_sel:3;
        uint32_t reserved_9:2;
        /** ext_etm_task_gpio36_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO36 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio36_en:1;
        /** ext_etm_task_gpio37_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO37.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio37_sel:3;
        uint32_t reserved_15:2;
        /** ext_etm_task_gpio37_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO37 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio37_en:1;
        /** ext_etm_task_gpio38_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO38.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio38_sel:3;
        uint32_t reserved_21:2;
        /** ext_etm_task_gpio38_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO38 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio38_en:1;
        /** ext_etm_task_gpio39_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO39.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t ext_etm_task_gpio39_sel:3;
        uint32_t reserved_27:2;
        /** ext_etm_task_gpio39_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO39 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t ext_etm_task_gpio39_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p7_cfg_reg_t;


/** Group: Version Register */
/** Type of ext_version register
 *  Version control register
 */
typedef union {
    struct {
        /** ext_date : R/W; bitpos: [27:0]; default: 37818704;
         *  Version control register.
         */
        uint32_t ext_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_ext_version_reg_t;


typedef struct {
    uint32_t reserved_000;
    volatile gpio_ext_sigmadelta_misc_reg_t ext_sigmadelta_misc;
    volatile gpio_ext_sigmadeltan_reg_t ext_sigmadeltan[4];
    uint32_t reserved_018[48];
    volatile gpio_ext_glitch_filter_chn_reg_t ext_glitch_filter_chn[8];
    uint32_t reserved_0f8[8];
    volatile gpio_ext_etm_event_chn_cfg_reg_t ext_etm_event_chn_cfg[8];
    uint32_t reserved_138[8];
    volatile gpio_ext_etm_task_p0_cfg_reg_t ext_etm_task_p0_cfg;
    volatile gpio_ext_etm_task_p1_cfg_reg_t ext_etm_task_p1_cfg;
    volatile gpio_ext_etm_task_p2_cfg_reg_t ext_etm_task_p2_cfg;
    volatile gpio_ext_etm_task_p3_cfg_reg_t ext_etm_task_p3_cfg;
    volatile gpio_ext_etm_task_p4_cfg_reg_t ext_etm_task_p4_cfg;
    volatile gpio_ext_etm_task_p5_cfg_reg_t ext_etm_task_p5_cfg;
    volatile gpio_ext_etm_task_p6_cfg_reg_t ext_etm_task_p6_cfg;
    volatile gpio_ext_etm_task_p7_cfg_reg_t ext_etm_task_p7_cfg;
    uint32_t reserved_178[33];
    volatile gpio_ext_version_reg_t ext_version;
} gpio_dev_t;

extern gpio_dev_t GPIO_EXT;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_dev_t) == 0x200, "Invalid size of gpio_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
