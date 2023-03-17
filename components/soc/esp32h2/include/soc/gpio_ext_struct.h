/**
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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


/** Group: Clock gate Register */
/** Type of clock_gate register
 *  Clock Gating Configure Register
 */
typedef union {
    struct {
        /** clk_en : R/W; bitpos: [0]; default: 0;
         *  Clock enable bit of configuration registers for sigma delta modulation.
         */
        uint32_t clk_en:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_sigmadelta_clock_gate_reg_t;


/** Group: Configure Registers */
/** Type of pad_comp_config register
 *  PAD Compare configure Register
 */
typedef union {
    struct {
        /** xpd_comp : R/W; bitpos: [0]; default: 0;
         *  Pad compare enable bit.
         */
        uint32_t xpd_comp:1;
        /** mode_comp : R/W; bitpos: [1]; default: 0;
         *  1 to enable external reference from PAD[10]. 0 to enable internal reference,
         *  meanwhile PAD[10] can be used as a regular GPIO.
         */
        uint32_t mode_comp:1;
        /** dref_comp : R/W; bitpos: [4:2]; default: 0;
         *  internal reference voltage tuning bit. 0V to 0.7*VDDPST step 0.1*VDDPST.
         */
        uint32_t dref_comp:3;
        /** zero_det_mode : R/W; bitpos: [6:5]; default: 0;
         *  Zero Detect mode select.
         */
        uint32_t zero_det_mode:2;
        uint32_t reserved_7:25;
    };
    uint32_t val;
} gpio_pad_comp_config_reg_t;

/** Type of pad_comp_filter register
 *  Zero Detect filter Register
 */
typedef union {
    struct {
        /** zero_det_filter_cnt : R/W; bitpos: [31:0]; default: 0;
         *  Zero Detect filter cycle length
         */
        uint32_t zero_det_filter_cnt:32;
    };
    uint32_t val;
} gpio_pad_comp_filter_reg_t;


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
        /** etm_ch0_event_sel : R/W; bitpos: [4:0]; default: 0;
         *  Etm event channel select gpio.
         */
        uint32_t etm_ch0_event_sel:5;
        uint32_t reserved_5:2;
        /** etm_ch0_event_en : R/W; bitpos: [7]; default: 0;
         *  Etm event send enable bit.
         */
        uint32_t etm_ch0_event_en:1;
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
} gpio_etm_task_pn_cfg_reg_t;

/** Group: Interrupt Registers */
/** Type of int_raw register
 *  GPIOSD interrupt raw register
 */
typedef union {
    struct {
        /** pad_comp_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  Pad compare raw interrupt
         */
        uint32_t pad_comp_int_raw:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_ext_int_raw_reg_t;

/** Type of int_st register
 *  GPIOSD interrupt masked register
 */
typedef union {
    struct {
        /** pad_comp_int_st : RO; bitpos: [0]; default: 0;
         *  Pad compare masked interrupt
         */
        uint32_t pad_comp_int_st:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_ext_int_st_reg_t;

/** Type of int_ena register
 *  GPIOSD interrupt enable register
 */
typedef union {
    struct {
        /** pad_comp_int_ena : R/W; bitpos: [0]; default: 0;
         *  Pad compare interrupt enable
         */
        uint32_t pad_comp_int_ena:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_ext_int_ena_reg_t;

/** Type of int_clr register
 *  GPIOSD interrupt clear register
 */
typedef union {
    struct {
        /** pad_comp_int_clr : WT; bitpos: [0]; default: 0;
         *  Pad compare interrupt clear
         */
        uint32_t pad_comp_int_clr:1;
        uint32_t reserved_1:31;
    };
    uint32_t val;
} gpio_ext_int_clr_reg_t;


/** Group: Version Register */
/** Type of version register
 *  Version Control Register
 */
typedef union {
    struct {
        /** gpio_sd_date : R/W; bitpos: [27:0]; default: 35684640;
         *  Version control register.
         */
        uint32_t gpio_sd_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_ext_version_reg_t;

typedef struct gpio_sd_dev_t {
    volatile gpio_sigmadelta_chn_reg_t channel[4];
    uint32_t reserved_010[4];
    volatile gpio_sigmadelta_clock_gate_reg_t clock_gate;
    volatile gpio_sigmadelta_misc_reg_t misc;
} gpio_sd_dev_t;

typedef struct {
    volatile gpio_glitch_filter_chn_reg_t glitch_filter_chn[8];
} gpio_glitch_filter_dev_t;

typedef struct {
    volatile gpio_etm_event_chn_cfg_reg_t etm_event_chn_cfg[8];
    uint32_t reserved_080[8];
    volatile gpio_etm_task_pn_cfg_reg_t etm_task_pn_cfg[7];
} gpio_etm_dev_t;

typedef struct {
    volatile gpio_sd_dev_t sigma_delta;
    volatile gpio_pad_comp_config_reg_t pad_comp_config;
    volatile gpio_pad_comp_filter_reg_t pad_comp_filter;
    volatile gpio_glitch_filter_dev_t glitch_filter;
    uint32_t reserved_050[4];
    volatile gpio_etm_dev_t etm;
    uint32_t reserved_0bc[9];
    volatile gpio_ext_int_raw_reg_t int_raw;
    volatile gpio_ext_int_st_reg_t int_st;
    volatile gpio_ext_int_ena_reg_t int_ena;
    volatile gpio_ext_int_clr_reg_t int_clr;
    uint32_t reserved_0f0[3];
    volatile gpio_ext_version_reg_t version;
} gpio_ext_dev_t;

// analog comparator is a stand alone peripheral, but it is connected to GPIO
// so we rename it to analog_cmpr_dev_t from user's perspective
typedef gpio_ext_dev_t analog_cmpr_dev_t;

extern gpio_sd_dev_t SDM;
extern gpio_glitch_filter_dev_t GLITCH_FILTER;
extern gpio_etm_dev_t GPIO_ETM;
extern gpio_ext_dev_t GPIO_EXT;
extern analog_cmpr_dev_t ANALOG_CMPR;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_ext_dev_t) == 0x100, "Invalid size of gpio_ext_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
