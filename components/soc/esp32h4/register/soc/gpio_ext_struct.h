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

/** Type of ext_etm_task_pn_cfg register
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
} gpio_ext_etm_task_pn_cfg_reg_t;


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

typedef struct gpio_etm_dev_t {
    volatile gpio_ext_etm_event_chn_cfg_reg_t etm_event_chn_cfg[8];
    uint32_t reserved_080[8];
    volatile gpio_ext_etm_task_pn_cfg_reg_t etm_task_pn_cfg[8];
} gpio_etm_dev_t;

typedef struct {
    uint32_t reserved_000;
    volatile gpio_ext_sigmadelta_misc_reg_t ext_sigmadelta_misc;
    volatile gpio_ext_sigmadeltan_reg_t ext_sigmadeltan[4];
    uint32_t reserved_018[48];
    volatile gpio_ext_glitch_filter_chn_reg_t ext_glitch_filter_chn[8];
    uint32_t reserved_0f8[8];
    volatile gpio_etm_dev_t etm;
    uint32_t reserved_178[33];
    volatile gpio_ext_version_reg_t ext_version;
} gpio_ext_dev_t;

extern gpio_etm_dev_t GPIO_ETM;
extern gpio_ext_dev_t GPIO_EXT;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_ext_dev_t) == 0x200, "Invalid size of gpio_ext_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
