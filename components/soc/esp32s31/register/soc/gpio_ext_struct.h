/**
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: SDM Configure Registers */
/** Type of sigmadelta_misc register
 *  MISC Register
 */
typedef union {
    struct {
        /** sigmadelta_clk_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable the clock for sigma delta modulation.
         *  0: Not enable
         *  1: Enable
         *  %\label{fielddesc:GPIOSDSPISWAP}- [GPIOSD_SPI_SWAP] Reserved.
         */
        uint32_t sigmadelta_clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_ext_sigmadelta_misc_reg_t;

/** Type of sigmadeltan register
 *  Duty cycle configuration register for SDM channel n
 */
typedef union {
    struct {
        /** sdn_in : R/W; bitpos: [7:0]; default: 0;
         *  Configures the duty cycle of sigma delta modulation output.
         */
        uint32_t sdn_in:8;
        /** sdn_prescale : R/W; bitpos: [15:8]; default: 255;
         *  Configures the divider value to divide IO MUX operating clock.
         */
        uint32_t sdn_prescale:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gpio_ext_sigmadeltan_reg_t;


/** Group: Glitch filter Configure Registers */
/** Type of glitch_filter_chn register
 *  Glitch Filter Configure Register of Channeln
 */
typedef union {
    struct {
        /** filter_ch0_en : R/W; bitpos: [0]; default: 0;
         *  Configures whether or not to enable channel n of Glitch Filter.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t filter_ch0_en:1;
        /** filter_ch0_input_io_num : R/W; bitpos: [7:1]; default: 0;
         *  Configures to select the input GPIO for Glitch Filter.
         *  0: Select GPIO0
         *  1: Select GPIO1
         *  ......
         *  61: Select GPIO61
         *  62: Select GPIO62
         *  63 ~ 127: Reserved
         */
        uint32_t filter_ch0_input_io_num:7;
        /** filter_ch0_window_thres : R/W; bitpos: [13:8]; default: 0;
         *  Configures the window threshold for Glitch Filter. The window threshold should be
         *  less than or equal to GPIOSD_FILTER_CHn_WINDOW_WIDTH.
         *  %see DOC-4768
         *  Measurement unit: IO MUX operating clock cycle
         */
        uint32_t filter_ch0_window_thres:6;
        /** filter_ch0_window_width : R/W; bitpos: [19:14]; default: 0;
         *  Configures the window width for Glitch Filter. The effective value of window width
         *  is 0 ~ 63.
         *  Measurement unit: IO MUX operating clock cycle
         */
        uint32_t filter_ch0_window_width:6;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gpio_ext_glitch_filter_chn_reg_t;


/** Group: ETM Configuration Registers */
/** Type of etm_event_chn_cfg register
 *  ETM configuration register for channel n
 */
typedef union {
    struct {
        /** etm_chn_event_sel : R/W; bitpos: [6:0]; default: 0;
         *  Configures to select GPIO for ETM event channel.
         *  0: Select GPIO0
         *  1: Select GPIO1
         *  ......
         *  61: Select GPIO61
         *  62: Select GPIO62
         *  63 ~ 127: Reserved
         */
        uint32_t etm_chn_event_sel:7;
        /** etm_chn_event_en : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable ETM event send.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_chn_event_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_ext_etm_event_chn_cfg_reg_t;

/** Type of etm_task_p0_cfg register
 *  GPIO selection register 0 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio0_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO0.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio0_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio0_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO0 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio0_en:1;
        /** etm_task_gpio1_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO1.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio1_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio1_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO1 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio1_en:1;
        /** etm_task_gpio2_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO2.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio2_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio2_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO2 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio2_en:1;
        /** etm_task_gpio3_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO3.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio3_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio3_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO3 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio3_en:1;
        /** etm_task_gpio4_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO4.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio4_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio4_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO4 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio4_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p0_cfg_reg_t;

/** Type of etm_task_p1_cfg register
 *  GPIO selection register 1 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio5_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO5.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio5_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio5_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO5 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio5_en:1;
        /** etm_task_gpio6_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO6.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio6_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio6_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO6 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio6_en:1;
        /** etm_task_gpio7_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO7.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio7_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio7_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO7 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio7_en:1;
        /** etm_task_gpio8_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO8.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio8_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio8_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO8 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio8_en:1;
        /** etm_task_gpio9_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO9.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio9_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio9_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO9 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio9_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p1_cfg_reg_t;

/** Type of etm_task_p2_cfg register
 *  GPIO selection register 2 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio10_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO10.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio10_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio10_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO10 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio10_en:1;
        /** etm_task_gpio11_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO11.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio11_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio11_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO11 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio11_en:1;
        /** etm_task_gpio12_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO12.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio12_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio12_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO12 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio12_en:1;
        /** etm_task_gpio13_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO13.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio13_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio13_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO13 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio13_en:1;
        /** etm_task_gpio14_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO14.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio14_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio14_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO14 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio14_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p2_cfg_reg_t;

/** Type of etm_task_p3_cfg register
 *  GPIO selection register 3 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio15_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO15.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio15_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio15_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO15 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio15_en:1;
        /** etm_task_gpio16_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO16.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio16_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio16_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO16 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio16_en:1;
        /** etm_task_gpio17_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO17.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio17_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio17_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO17 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio17_en:1;
        /** etm_task_gpio18_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO18.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio18_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio18_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO18 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio18_en:1;
        /** etm_task_gpio19_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO19.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio19_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio19_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO19 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio19_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p3_cfg_reg_t;

/** Type of etm_task_p4_cfg register
 *  GPIO selection register 4 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio20_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO20.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio20_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio20_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO20 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio20_en:1;
        /** etm_task_gpio21_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO21.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio21_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio21_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO21 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio21_en:1;
        /** etm_task_gpio22_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO22.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio22_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio22_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO22 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio22_en:1;
        /** etm_task_gpio23_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO23.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio23_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio23_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO23 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio23_en:1;
        /** etm_task_gpio24_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO24.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio24_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio24_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO24 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio24_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p4_cfg_reg_t;

/** Type of etm_task_p5_cfg register
 *  GPIO selection register 5 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio25_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO25.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio25_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio25_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO25 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio25_en:1;
        /** etm_task_gpio26_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO26.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio26_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio26_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO26 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio26_en:1;
        /** etm_task_gpio27_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO27.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio27_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio27_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO27 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio27_en:1;
        /** etm_task_gpio28_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO28.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio28_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio28_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO28 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio28_en:1;
        /** etm_task_gpio29_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO29.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio29_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio29_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO29 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio29_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p5_cfg_reg_t;

/** Type of etm_task_p6_cfg register
 *  GPIO selection register 6 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio30_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO30.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio30_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio30_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO30 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio30_en:1;
        /** etm_task_gpio31_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO31.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio31_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio31_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO31 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio31_en:1;
        /** etm_task_gpio32_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO32.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio32_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio32_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO32 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio32_en:1;
        /** etm_task_gpio33_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO33.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio33_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio33_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO33 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio33_en:1;
        /** etm_task_gpio34_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO34.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio34_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio34_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO34 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio34_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p6_cfg_reg_t;

/** Type of etm_task_p7_cfg register
 *  GPIO selection register 7 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio35_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO35.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio35_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio35_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO35 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio35_en:1;
        /** etm_task_gpio36_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO36.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio36_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio36_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO36 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio36_en:1;
        /** etm_task_gpio37_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO37.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio37_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio37_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO37 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio37_en:1;
        /** etm_task_gpio38_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO38.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio38_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio38_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO38 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio38_en:1;
        /** etm_task_gpio39_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO39.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio39_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio39_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO39 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio39_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p7_cfg_reg_t;

/** Type of etm_task_p8_cfg register
 *  GPIO selection register 8 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio40_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO40.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio40_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio40_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO40 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio40_en:1;
        /** etm_task_gpio41_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO41.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio41_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio41_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO41 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio41_en:1;
        /** etm_task_gpio42_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO42.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio42_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio42_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO42 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio42_en:1;
        /** etm_task_gpio43_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO43.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio43_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio43_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO43 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio43_en:1;
        /** etm_task_gpio44_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO44.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio44_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio44_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO44 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio44_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p8_cfg_reg_t;

/** Type of etm_task_p9_cfg register
 *  GPIO selection register 9 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio45_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO45.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio45_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio45_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO45 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio45_en:1;
        /** etm_task_gpio46_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO46.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio46_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio46_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO46 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio46_en:1;
        /** etm_task_gpio47_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO47.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio47_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio47_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO47 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio47_en:1;
        /** etm_task_gpio48_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO48.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio48_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio48_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO48 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio48_en:1;
        /** etm_task_gpio49_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO49.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio49_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio49_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO49 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio49_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p9_cfg_reg_t;

/** Type of etm_task_p10_cfg register
 *  GPIO selection register 10 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio50_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO50.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio50_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio50_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO50 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio50_en:1;
        /** etm_task_gpio51_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO51.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio51_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio51_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO51 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio51_en:1;
        /** etm_task_gpio52_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO52.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio52_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio52_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO52 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio52_en:1;
        /** etm_task_gpio53_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO53.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio53_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio53_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO53 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio53_en:1;
        /** etm_task_gpio54_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO54.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio54_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio54_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO54 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio54_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p10_cfg_reg_t;

/** Type of etm_task_p11_cfg register
 *  GPIO selection register 11 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio55_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO55.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio55_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio55_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO55 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio55_en:1;
        /** etm_task_gpio56_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO56.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio56_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio56_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO56 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio56_en:1;
        /** etm_task_gpio57_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO57.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio57_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio57_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO57 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio57_en:1;
        /** etm_task_gpio58_sel : R/W; bitpos: [20:18]; default: 0;
         *  Configures to select an ETM task channel for GPIO58.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio58_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio58_en : R/W; bitpos: [23]; default: 0;
         *  Configures whether or not to enable GPIO58 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio58_en:1;
        /** etm_task_gpio59_sel : R/W; bitpos: [26:24]; default: 0;
         *  Configures to select an ETM task channel for GPIO59.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio59_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio59_en : R/W; bitpos: [29]; default: 0;
         *  Configures whether or not to enable GPIO59 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio59_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p11_cfg_reg_t;

/** Type of etm_task_p12_cfg register
 *  GPIO selection register 12 for ETM
 */
typedef union {
    struct {
        /** etm_task_gpio60_sel : R/W; bitpos: [2:0]; default: 0;
         *  Configures to select an ETM task channel for GPIO60.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio60_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio60_en : R/W; bitpos: [5]; default: 0;
         *  Configures whether or not to enable GPIO60 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio60_en:1;
        /** etm_task_gpio61_sel : R/W; bitpos: [8:6]; default: 0;
         *  Configures to select an ETM task channel for GPIO61.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio61_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio61_en : R/W; bitpos: [11]; default: 0;
         *  Configures whether or not to enable GPIO61 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio61_en:1;
        /** etm_task_gpio62_sel : R/W; bitpos: [14:12]; default: 0;
         *  Configures to select an ETM task channel for GPIO62.
         *  0: Select channel 0
         *  1: Select channel 1
         *  ......
         *  7: Select channel 7
         */
        uint32_t etm_task_gpio62_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio62_en : R/W; bitpos: [17]; default: 0;
         *  Configures whether or not to enable GPIO62 to response ETM task.
         *  0: Not enable
         *  1: Enable
         */
        uint32_t etm_task_gpio62_en:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} gpio_ext_etm_task_p12_cfg_reg_t;


/** Group: Version Register */
/** Type of version register
 *  Version control register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 38830416;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_ext_version_reg_t;


typedef struct gpio_sd_dev_t {
    volatile gpio_ext_sigmadelta_misc_reg_t misc;
    volatile gpio_ext_sigmadeltan_reg_t channel[8];
} gpio_sd_dev_t;

typedef struct {
    uint32_t reserved_000;
    volatile gpio_sd_dev_t sigma_delta;
    uint32_t reserved_028[44];
    volatile gpio_ext_glitch_filter_chn_reg_t glitch_filter_chn[8];
    uint32_t reserved_0f8[8];
    volatile gpio_ext_etm_event_chn_cfg_reg_t etm_event_chn_cfg[8];
    uint32_t reserved_138[8];
    volatile gpio_ext_etm_task_p0_cfg_reg_t etm_task_p0_cfg;
    volatile gpio_ext_etm_task_p1_cfg_reg_t etm_task_p1_cfg;
    volatile gpio_ext_etm_task_p2_cfg_reg_t etm_task_p2_cfg;
    volatile gpio_ext_etm_task_p3_cfg_reg_t etm_task_p3_cfg;
    volatile gpio_ext_etm_task_p4_cfg_reg_t etm_task_p4_cfg;
    volatile gpio_ext_etm_task_p5_cfg_reg_t etm_task_p5_cfg;
    volatile gpio_ext_etm_task_p6_cfg_reg_t etm_task_p6_cfg;
    volatile gpio_ext_etm_task_p7_cfg_reg_t etm_task_p7_cfg;
    volatile gpio_ext_etm_task_p8_cfg_reg_t etm_task_p8_cfg;
    volatile gpio_ext_etm_task_p9_cfg_reg_t etm_task_p9_cfg;
    volatile gpio_ext_etm_task_p10_cfg_reg_t etm_task_p10_cfg;
    volatile gpio_ext_etm_task_p11_cfg_reg_t etm_task_p11_cfg;
    volatile gpio_ext_etm_task_p12_cfg_reg_t etm_task_p12_cfg;
    uint32_t reserved_18c[28];
    volatile gpio_ext_version_reg_t version;
} gpio_ext_dev_t;


#ifndef __cplusplus
_Static_assert(sizeof(gpio_ext_dev_t) == 0x200, "Invalid size of gpio_ext_dev_t structure");
#endif

extern gpio_sd_dev_t SDM;
extern gpio_ext_dev_t GPIO_EXT;

#ifdef __cplusplus
}
#endif
