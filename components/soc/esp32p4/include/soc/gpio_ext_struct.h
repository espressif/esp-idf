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

/** Group: SDM Configure Registers */
/** Type of sigmadeltan register
 *  Duty Cycle Configure Register of SDMn
 */
typedef union {
    struct {
        /** duty : R/W; bitpos: [7:0]; default: 0;
         *  This field is used to configure the duty cycle of sigma delta modulation output.
         */
        uint32_t duty:8;
        /** prescale : R/W; bitpos: [15:8]; default: 255;
         *  This field is used to set a divider value to divide APB clock.
         */
        uint32_t prescale:8;
        uint32_t reserved_16:16;
    };
    uint32_t val;
} gpio_sigmadelta_chn_reg_t;

/** Type of sigmadelta_misc register
 *  MISC Register
 */
typedef union {
    struct {
        uint32_t reserved_0:30;
        /** function_clk_en : R/W; bitpos: [30]; default: 0;
         *  Clock enable bit of sigma delta modulation.
         */
        uint32_t function_clk_en:1;
        /** spi_swap : R/W; bitpos: [31]; default: 0;
         *  Reserved.
         */
        uint32_t spi_swap:1;
    };
    uint32_t val;
} gpio_sigmadelta_misc_reg_t;


/** Group: Glitch filter Configure Registers */
/** Type of glitch_filter_chn register
 *  Glitch Filter Configure Register of Channeln
 */
typedef union {
    struct {
        /** filter_chn_en : R/W; bitpos: [0]; default: 0;
         *  Glitch Filter channel enable bit.
         */
        uint32_t filter_chn_en:1;
        /** filter_chn_input_io_num : R/W; bitpos: [6:1]; default: 0;
         *  Glitch Filter input io number.
         */
        uint32_t filter_chn_input_io_num:6;
        /** filter_chn_window_thres : R/W; bitpos: [12:7]; default: 0;
         *  Glitch Filter window threshold.
         */
        uint32_t filter_chn_window_thres:6;
        /** filter_chn_window_width : R/W; bitpos: [18:13]; default: 0;
         *  Glitch Filter window width.
         */
        uint32_t filter_chn_window_width:6;
        uint32_t reserved_19:13;
    };
    uint32_t val;
} gpio_glitch_filter_chn_reg_t;


/** Group: Etm Configure Registers */
/** Type of etm_event_chn_cfg register
 *  Etm Config register of Channeln
 */
typedef union {
    struct {
        /** etm_chn_event_sel : R/W; bitpos: [5:0]; default: 0;
         *  Etm event channel select gpio.
         */
        uint32_t etm_chn_event_sel:6;
        uint32_t reserved_6:1;
        /** etm_chn_event_en : R/W; bitpos: [7]; default: 0;
         *  Etm event send enable bit.
         */
        uint32_t etm_chn_event_en:1;
        uint32_t reserved_8:24;
    };
    uint32_t val;
} gpio_etm_event_chn_cfg_reg_t;

/** Type of etm_task_p0_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio0_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio0_en:1;
        /** etm_task_gpio0_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio0_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio1_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio1_en:1;
        /** etm_task_gpio1_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio1_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio2_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio2_en:1;
        /** etm_task_gpio2_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio2_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio3_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio3_en:1;
        /** etm_task_gpio3_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio3_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p0_cfg_reg_t;

/** Type of etm_task_p1_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio4_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio4_en:1;
        /** etm_task_gpio4_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio4_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio5_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio5_en:1;
        /** etm_task_gpio5_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio5_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio6_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio6_en:1;
        /** etm_task_gpio6_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio6_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio7_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio7_en:1;
        /** etm_task_gpio7_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio7_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p1_cfg_reg_t;

/** Type of etm_task_p2_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio8_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio8_en:1;
        /** etm_task_gpio8_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio8_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio9_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio9_en:1;
        /** etm_task_gpio9_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio9_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio10_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio10_en:1;
        /** etm_task_gpio10_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio10_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio11_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio11_en:1;
        /** etm_task_gpio11_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio11_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p2_cfg_reg_t;

/** Type of etm_task_p3_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio12_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio12_en:1;
        /** etm_task_gpio12_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio12_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio13_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio13_en:1;
        /** etm_task_gpio13_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio13_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio14_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio14_en:1;
        /** etm_task_gpio14_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio14_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio15_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio15_en:1;
        /** etm_task_gpio15_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio15_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p3_cfg_reg_t;

/** Type of etm_task_p4_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio16_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio16_en:1;
        /** etm_task_gpio16_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio16_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio17_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio17_en:1;
        /** etm_task_gpio17_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio17_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio18_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio18_en:1;
        /** etm_task_gpio18_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio18_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio19_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio19_en:1;
        /** etm_task_gpio19_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio19_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p4_cfg_reg_t;

/** Type of etm_task_p5_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio20_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio20_en:1;
        /** etm_task_gpio20_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio20_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio21_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio21_en:1;
        /** etm_task_gpio21_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio21_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio22_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio22_en:1;
        /** etm_task_gpio22_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio22_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio23_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio23_en:1;
        /** etm_task_gpio23_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio23_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p5_cfg_reg_t;

/** Type of etm_task_p6_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio24_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio24_en:1;
        /** etm_task_gpio24_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio24_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio25_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio25_en:1;
        /** etm_task_gpio25_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio25_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio26_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio26_en:1;
        /** etm_task_gpio26_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio26_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio27_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio27_en:1;
        /** etm_task_gpio27_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio27_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p6_cfg_reg_t;

/** Type of etm_task_p7_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio28_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio28_en:1;
        /** etm_task_gpio28_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio28_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio29_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio29_en:1;
        /** etm_task_gpio29_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio29_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio30_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio30_en:1;
        /** etm_task_gpio30_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio30_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio31_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio31_en:1;
        /** etm_task_gpio31_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio31_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p7_cfg_reg_t;

/** Type of etm_task_p8_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio32_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio32_en:1;
        /** etm_task_gpio32_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio32_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio33_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio33_en:1;
        /** etm_task_gpio33_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio33_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio34_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio34_en:1;
        /** etm_task_gpio34_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio34_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio35_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio35_en:1;
        /** etm_task_gpio35_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio35_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p8_cfg_reg_t;

/** Type of etm_task_p9_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio36_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio36_en:1;
        /** etm_task_gpio36_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio36_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio37_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio37_en:1;
        /** etm_task_gpio37_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio37_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio38_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio38_en:1;
        /** etm_task_gpio38_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio38_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio39_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio39_en:1;
        /** etm_task_gpio39_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio39_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p9_cfg_reg_t;

/** Type of etm_task_p10_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio40_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio40_en:1;
        /** etm_task_gpio40_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio40_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio41_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio41_en:1;
        /** etm_task_gpio41_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio41_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio42_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio42_en:1;
        /** etm_task_gpio42_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio42_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio43_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio43_en:1;
        /** etm_task_gpio43_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio43_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p10_cfg_reg_t;

/** Type of etm_task_p11_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio44_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio44_en:1;
        /** etm_task_gpio44_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio44_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio45_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio45_en:1;
        /** etm_task_gpio45_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio45_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio46_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio46_en:1;
        /** etm_task_gpio46_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio46_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio47_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio47_en:1;
        /** etm_task_gpio47_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio47_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p11_cfg_reg_t;

/** Type of etm_task_p12_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio48_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio48_en:1;
        /** etm_task_gpio48_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio48_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio49_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio49_en:1;
        /** etm_task_gpio49_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio49_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio50_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio50_en:1;
        /** etm_task_gpio50_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio50_sel:3;
        uint32_t reserved_20:4;
        /** etm_task_gpio51_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio51_en:1;
        /** etm_task_gpio51_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio51_sel:3;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_etm_task_p12_cfg_reg_t;

/** Type of etm_task_p13_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio52_en : R/W; bitpos: [0]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio52_en:1;
        /** etm_task_gpio52_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio52_sel:3;
        uint32_t reserved_4:4;
        /** etm_task_gpio53_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio53_en:1;
        /** etm_task_gpio53_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio53_sel:3;
        uint32_t reserved_12:4;
        /** etm_task_gpio54_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio54_en:1;
        /** etm_task_gpio54_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio54_sel:3;
        uint32_t reserved_20:12;
    };
    uint32_t val;
} gpio_etm_task_p13_cfg_reg_t;


/** Group: Version Register */
/** Type of version register
 *  Version Control Register
 */
typedef union {
    struct {
        /** gpio_ext_date : R/W; bitpos: [27:0]; default: 35663952;
         *  Version control register.
         */
        uint32_t gpio_ext_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_ext_version_reg_t;


typedef struct gpio_sd_dev_t {
    volatile gpio_sigmadelta_chn_reg_t channel[8];
    uint32_t reserved_020;
    volatile gpio_sigmadelta_misc_reg_t misc;
} gpio_sd_dev_t;

typedef struct gpio_glitch_filter_dev_t {
    volatile gpio_glitch_filter_chn_reg_t glitch_filter_chn[8];
} gpio_glitch_filter_dev_t;

typedef struct gpio_etm_dev_t {
    volatile gpio_etm_event_chn_cfg_reg_t etm_event_chn_cfg[8];
    uint32_t reserved_080[8];
    volatile gpio_etm_task_p0_cfg_reg_t etm_task_p0_cfg;
    volatile gpio_etm_task_p1_cfg_reg_t etm_task_p1_cfg;
    volatile gpio_etm_task_p2_cfg_reg_t etm_task_p2_cfg;
    volatile gpio_etm_task_p3_cfg_reg_t etm_task_p3_cfg;
    volatile gpio_etm_task_p4_cfg_reg_t etm_task_p4_cfg;
    volatile gpio_etm_task_p5_cfg_reg_t etm_task_p5_cfg;
    volatile gpio_etm_task_p6_cfg_reg_t etm_task_p6_cfg;
    volatile gpio_etm_task_p7_cfg_reg_t etm_task_p7_cfg;
    volatile gpio_etm_task_p8_cfg_reg_t etm_task_p8_cfg;
    volatile gpio_etm_task_p9_cfg_reg_t etm_task_p9_cfg;
    volatile gpio_etm_task_p10_cfg_reg_t etm_task_p10_cfg;
    volatile gpio_etm_task_p11_cfg_reg_t etm_task_p11_cfg;
    volatile gpio_etm_task_p12_cfg_reg_t etm_task_p12_cfg;
    volatile gpio_etm_task_p13_cfg_reg_t etm_task_p13_cfg;
} gpio_etm_dev_t;

typedef struct {
    volatile gpio_sd_dev_t sigma_delta;
    uint32_t reserved_028[2];
    volatile gpio_glitch_filter_dev_t glitch_filter;
    uint32_t reserved_050[4];
    volatile gpio_etm_dev_t etm;
    uint32_t reserved_0d8[9];
    volatile gpio_ext_version_reg_t version;
} gpio_ext_dev_t;

extern gpio_sd_dev_t SDM;
extern gpio_glitch_filter_dev_t GLITCH_FILTER;
extern gpio_etm_dev_t GPIO_ETM;
extern gpio_ext_dev_t GPIO_EXT;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_ext_dev_t) == 0x100, "Invalid size of gpio_ext_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
