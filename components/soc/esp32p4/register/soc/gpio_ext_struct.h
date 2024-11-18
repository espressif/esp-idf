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
        uint32_t duty: 8;
        /** prescale : R/W; bitpos: [15:8]; default: 255;
         *  This field is used to set a divider value to divide APB clock.
         */
        uint32_t prescale: 8;
        uint32_t reserved_16: 16;
    };
    uint32_t val;
} gpio_sigmadelta_chn_reg_t;

/** Type of sigmadelta_misc register
 *  MISC Register
 */
typedef union {
    struct {
        uint32_t reserved_0: 30;
        /** function_clk_en : R/W; bitpos: [30]; default: 0;
         *  Clock enable bit of sigma delta modulation.
         */
        uint32_t function_clk_en: 1;
        /** spi_swap : R/W; bitpos: [31]; default: 0;
         *  Reserved.
         */
        uint32_t spi_swap: 1;
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
        uint32_t filter_chn_en: 1;
        /** filter_chn_input_io_num : R/W; bitpos: [6:1]; default: 0;
         *  Glitch Filter input io number.
         */
        uint32_t filter_chn_input_io_num: 6;
        /** filter_chn_window_thres : R/W; bitpos: [12:7]; default: 0;
         *  Glitch Filter window threshold.
         */
        uint32_t filter_chn_window_thres: 6;
        /** filter_chn_window_width : R/W; bitpos: [18:13]; default: 0;
         *  Glitch Filter window width.
         */
        uint32_t filter_chn_window_width: 6;
        uint32_t reserved_19: 13;
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
        uint32_t etm_chn_event_sel: 6;
        uint32_t reserved_6: 1;
        /** etm_chn_event_en : R/W; bitpos: [7]; default: 0;
         *  Etm event send enable bit.
         */
        uint32_t etm_chn_event_en: 1;
        uint32_t reserved_8: 24;
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
        uint32_t etm_task_gpio0_en: 1;
        /** etm_task_gpio0_sel : R/W; bitpos: [3:1]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio0_sel: 3;
        uint32_t reserved_4: 4;
        /** etm_task_gpio1_en : R/W; bitpos: [8]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio1_en: 1;
        /** etm_task_gpio1_sel : R/W; bitpos: [11:9]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio1_sel: 3;
        uint32_t reserved_12: 4;
        /** etm_task_gpio2_en : R/W; bitpos: [16]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio2_en: 1;
        /** etm_task_gpio2_sel : R/W; bitpos: [19:17]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio2_sel: 3;
        uint32_t reserved_20: 4;
        /** etm_task_gpio3_en : R/W; bitpos: [24]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio3_en: 1;
        /** etm_task_gpio3_sel : R/W; bitpos: [27:25]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio3_sel: 3;
        uint32_t reserved_28: 4;
    };
    uint32_t val;
} gpio_etm_task_pn_cfg_reg_t;

/** Group: Version Register */
/** Type of version register
 *  Version Control Register
 */
typedef union {
    struct {
        /** gpio_ext_date : R/W; bitpos: [27:0]; default: 35663952;
         *  Version control register.
         */
        uint32_t gpio_ext_date: 28;
        uint32_t reserved_28: 4;
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
    volatile gpio_etm_task_pn_cfg_reg_t etm_task_pn_cfg[14];
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
