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

/** Group: Configure Registers */
/** Type of pad_comp_config_0 register
 *  PAD Compare configure Register
 */
typedef union {
    struct {
        /** xpd_comp_0 : R/W; bitpos: [0]; default: 0;
         *  Pad compare enable bit.
         */
        uint32_t xpd_comp_0:1;
        /** mode_comp_0 : R/W; bitpos: [1]; default: 0;
         *  1 to enable external reference from PAD[x]. 0 to enable internal reference,
         *  meanwhile PAD[x] can be used as a regular GPIO.
         */
        uint32_t mode_comp_0:1;
        /** dref_comp_0 : R/W; bitpos: [4:2]; default: 0;
         *  internal reference voltage tuning bit. 0V to 0.7*VDDPST step 0.1*VDDPST.
         */
        uint32_t dref_comp_0:3;
        uint32_t reserved_5:27;
    };
    uint32_t val;
} gpio_ext_pad_comp_config_0_reg_t;

/** Type of pad_comp_filter_0 register
 *  Zero Detect filter Register
 */
typedef union {
    struct {
        /** zero_det_filter_cnt_0 : R/W; bitpos: [31:0]; default: 0;
         *  Zero Detect filter cycle length
         */
        uint32_t zero_det_filter_cnt_0:32;
    };
    uint32_t val;
} gpio_ext_pad_comp_filter_0_reg_t;

/** Type of pin_ctrl register
 *  Clock Output Configuration Register
 */
typedef union {
    struct {
        /** clk_out1 : R/W; bitpos: [4:0]; default: 0;
         *  If you want to output clock for I2S to CLK_OUT_out1, set this register to 0x0.
         *  CLK_OUT_out1 can be found in peripheral output signals.
         */
        uint32_t clk_out1:5;
        /** clk_out2 : R/W; bitpos: [9:5]; default: 0;
         *  If you want to output clock for I2S to CLK_OUT_out2, set this register to 0x0.
         *  CLK_OUT_out2 can be found in peripheral output signals.
         */
        uint32_t clk_out2:5;
        /** clk_out3 : R/W; bitpos: [14:10]; default: 0;
         *  If you want to output clock for I2S to CLK_OUT_out3, set this register to 0x0.
         *  CLK_OUT_out3 can be found in peripheral output signals.
         */
        uint32_t clk_out3:5;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} gpio_ext_pin_ctrl_reg_t;


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
} gpio_ext_etm_event_chn_cfg_reg_t;

/** Type of etm_task_pn_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio0_sel : R/W; bitpos: [2:0]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio0_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio0_en : R/W; bitpos: [5]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio0_en:1;
        /** etm_task_gpio1_sel : R/W; bitpos: [8:6]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio1_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio1_en : R/W; bitpos: [11]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio1_en:1;
        /** etm_task_gpio2_sel : R/W; bitpos: [14:12]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio2_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio2_en : R/W; bitpos: [17]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio2_en:1;
        /** etm_task_gpio3_sel : R/W; bitpos: [20:18]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio3_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio3_en : R/W; bitpos: [23]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio3_en:1;
        /** etm_task_gpio4_sel : R/W; bitpos: [26:24]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio4_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio4_en : R/W; bitpos: [29]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio4_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_pn_cfg_reg_t;

/** Type of etm_task_p1_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio5_sel : R/W; bitpos: [2:0]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio5_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio5_en : R/W; bitpos: [5]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio5_en:1;
        /** etm_task_gpio6_sel : R/W; bitpos: [8:6]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio6_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio6_en : R/W; bitpos: [11]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio6_en:1;
        /** etm_task_gpio7_sel : R/W; bitpos: [14:12]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio7_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio7_en : R/W; bitpos: [17]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio7_en:1;
        /** etm_task_gpio8_sel : R/W; bitpos: [20:18]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio8_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio8_en : R/W; bitpos: [23]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio8_en:1;
        /** etm_task_gpio9_sel : R/W; bitpos: [26:24]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio9_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio9_en : R/W; bitpos: [29]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio9_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p1_cfg_reg_t;

/** Type of etm_task_p2_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio10_sel : R/W; bitpos: [2:0]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio10_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio10_en : R/W; bitpos: [5]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio10_en:1;
        /** etm_task_gpio11_sel : R/W; bitpos: [8:6]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio11_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio11_en : R/W; bitpos: [11]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio11_en:1;
        /** etm_task_gpio12_sel : R/W; bitpos: [14:12]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio12_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio12_en : R/W; bitpos: [17]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio12_en:1;
        /** etm_task_gpio13_sel : R/W; bitpos: [20:18]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio13_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio13_en : R/W; bitpos: [23]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio13_en:1;
        /** etm_task_gpio14_sel : R/W; bitpos: [26:24]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio14_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio14_en : R/W; bitpos: [29]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio14_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p2_cfg_reg_t;

/** Type of etm_task_p3_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio15_sel : R/W; bitpos: [2:0]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio15_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio15_en : R/W; bitpos: [5]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio15_en:1;
        /** etm_task_gpio16_sel : R/W; bitpos: [8:6]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio16_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio16_en : R/W; bitpos: [11]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio16_en:1;
        /** etm_task_gpio17_sel : R/W; bitpos: [14:12]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio17_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio17_en : R/W; bitpos: [17]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio17_en:1;
        /** etm_task_gpio18_sel : R/W; bitpos: [20:18]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio18_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio18_en : R/W; bitpos: [23]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio18_en:1;
        /** etm_task_gpio19_sel : R/W; bitpos: [26:24]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio19_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio19_en : R/W; bitpos: [29]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio19_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p3_cfg_reg_t;

/** Type of etm_task_p4_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio20_sel : R/W; bitpos: [2:0]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio20_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio20_en : R/W; bitpos: [5]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio20_en:1;
        /** etm_task_gpio21_sel : R/W; bitpos: [8:6]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio21_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio21_en : R/W; bitpos: [11]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio21_en:1;
        /** etm_task_gpio22_sel : R/W; bitpos: [14:12]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio22_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio22_en : R/W; bitpos: [17]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio22_en:1;
        /** etm_task_gpio23_sel : R/W; bitpos: [20:18]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio23_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio23_en : R/W; bitpos: [23]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio23_en:1;
        /** etm_task_gpio24_sel : R/W; bitpos: [26:24]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio24_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio24_en : R/W; bitpos: [29]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio24_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p4_cfg_reg_t;

/** Type of etm_task_p5_cfg register
 *  Etm Configure Register to decide which GPIO been chosen
 */
typedef union {
    struct {
        /** etm_task_gpio25_sel : R/W; bitpos: [2:0]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio25_sel:3;
        uint32_t reserved_3:2;
        /** etm_task_gpio25_en : R/W; bitpos: [5]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio25_en:1;
        /** etm_task_gpio26_sel : R/W; bitpos: [8:6]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio26_sel:3;
        uint32_t reserved_9:2;
        /** etm_task_gpio26_en : R/W; bitpos: [11]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio26_en:1;
        /** etm_task_gpio27_sel : R/W; bitpos: [14:12]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio27_sel:3;
        uint32_t reserved_15:2;
        /** etm_task_gpio27_en : R/W; bitpos: [17]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio27_en:1;
        /** etm_task_gpio28_sel : R/W; bitpos: [20:18]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio28_sel:3;
        uint32_t reserved_21:2;
        /** etm_task_gpio28_en : R/W; bitpos: [23]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio28_en:1;
        /** etm_task_gpio29_sel : R/W; bitpos: [26:24]; default: 0;
         *  GPIO choose a etm task channel.
         */
        uint32_t etm_task_gpio29_sel:3;
        uint32_t reserved_27:2;
        /** etm_task_gpio29_en : R/W; bitpos: [29]; default: 0;
         *  Enable bit of GPIO response etm task.
         */
        uint32_t etm_task_gpio29_en:1;
        uint32_t reserved_30:2;
    };
    uint32_t val;
} gpio_ext_etm_task_p5_cfg_reg_t;


/** Group: Interrupt Registers */
/** Type of int_raw register
 *  GPIO_EXT interrupt raw register
 */
typedef union {
    struct {
        /** comp_neg_0_int_raw : RO/WTC/SS; bitpos: [0]; default: 0;
         *  analog comparator pos edge interrupt raw
         */
        uint32_t comp_neg_0_int_raw:1;
        /** comp_pos_0_int_raw : RO/WTC/SS; bitpos: [1]; default: 0;
         *  analog comparator neg edge interrupt raw
         */
        uint32_t comp_pos_0_int_raw:1;
        /** comp_all_0_int_raw : RO/WTC/SS; bitpos: [2]; default: 0;
         *  analog comparator neg or pos edge interrupt raw
         */
        uint32_t comp_all_0_int_raw:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_ext_int_raw_reg_t;

/** Type of int_st register
 *  GPIO_EXT interrupt masked register
 */
typedef union {
    struct {
        /** comp_neg_0_int_st : RO; bitpos: [0]; default: 0;
         *  analog comparator pos edge interrupt status
         */
        uint32_t comp_neg_0_int_st:1;
        /** comp_pos_0_int_st : RO; bitpos: [1]; default: 0;
         *  analog comparator neg edge interrupt status
         */
        uint32_t comp_pos_0_int_st:1;
        /** comp_all_0_int_st : RO; bitpos: [2]; default: 0;
         *  analog comparator neg or pos edge interrupt status
         */
        uint32_t comp_all_0_int_st:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_ext_int_st_reg_t;

/** Type of int_ena register
 *  GPIO_EXT interrupt enable register
 */
typedef union {
    struct {
        /** comp_neg_0_int_ena : R/W; bitpos: [0]; default: 1;
         *  analog comparator pos edge interrupt enable
         */
        uint32_t comp_neg_0_int_ena:1;
        /** comp_pos_0_int_ena : R/W; bitpos: [1]; default: 1;
         *  analog comparator neg edge interrupt enable
         */
        uint32_t comp_pos_0_int_ena:1;
        /** comp_all_0_int_ena : R/W; bitpos: [2]; default: 1;
         *  analog comparator neg or pos edge interrupt enable
         */
        uint32_t comp_all_0_int_ena:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_ext_int_ena_reg_t;

/** Type of int_clr register
 *  GPIO_EXT interrupt clear register
 */
typedef union {
    struct {
        /** comp_neg_0_int_clr : WT; bitpos: [0]; default: 0;
         *  analog comparator pos edge interrupt clear
         */
        uint32_t comp_neg_0_int_clr:1;
        /** comp_pos_0_int_clr : WT; bitpos: [1]; default: 0;
         *  analog comparator neg edge interrupt clear
         */
        uint32_t comp_pos_0_int_clr:1;
        /** comp_all_0_int_clr : WT; bitpos: [2]; default: 0;
         *  analog comparator neg or pos edge interrupt clear
         */
        uint32_t comp_all_0_int_clr:1;
        uint32_t reserved_3:29;
    };
    uint32_t val;
} gpio_ext_int_clr_reg_t;


/** Group: Version Register */
/** Type of version register
 *  Version Control Register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 37823120;
         *  Version control register.
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} gpio_ext_version_reg_t;

typedef struct gpio_etm_dev_t {
    volatile gpio_ext_etm_event_chn_cfg_reg_t etm_event_chn_cfg[8];
    uint32_t reserved_080[8];
    volatile gpio_ext_etm_task_pn_cfg_reg_t etm_task_pn_cfg[6];
} gpio_etm_dev_t;

typedef struct {
    uint32_t reserved_000[22];
    volatile gpio_ext_pad_comp_config_0_reg_t pad_comp_config_0;
    volatile gpio_ext_pad_comp_filter_0_reg_t pad_comp_filter_0;
    uint32_t reserved_060[46];
    volatile gpio_etm_dev_t etm;
    uint32_t reserved_170[24];
    volatile gpio_ext_int_raw_reg_t int_raw;
    volatile gpio_ext_int_st_reg_t int_st;
    volatile gpio_ext_int_ena_reg_t int_ena;
    volatile gpio_ext_int_clr_reg_t int_clr;
    volatile gpio_ext_pin_ctrl_reg_t pin_ctrl;
    uint32_t reserved_1e4[6];
    volatile gpio_ext_version_reg_t version;
} gpio_ext_dev_t;

extern gpio_etm_dev_t GPIO_ETM;
extern gpio_ext_dev_t GPIO_EXT;

#ifndef __cplusplus
_Static_assert(sizeof(gpio_ext_dev_t) == 0x200, "Invalid size of gpio_ext_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
