/**
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Group: Configure Registers */
/** Type of pin_ctrl register
 *  Clock Output Configuration Register
 */
typedef union {
    struct {
        /** clk_out1 : R/W; bitpos: [4:0]; default: 15;
         *  If you want to output clock for I2S to CLK_OUT_out1, set this register to 0x0.
         *  CLK_OUT_out1 can be found in peripheral output signals.
         */
        uint32_t clk_out1:5;
        /** clk_out2 : R/W; bitpos: [9:5]; default: 15;
         *  If you want to output clock for I2S to CLK_OUT_out2, set this register to 0x0.
         *  CLK_OUT_out2 can be found in peripheral output signals.
         */
        uint32_t clk_out2:5;
        /** clk_out3 : R/W; bitpos: [14:10]; default: 7;
         *  If you want to output clock for I2S to CLK_OUT_out3, set this register to 0x0.
         *  CLK_OUT_out3 can be found in peripheral output signals.
         */
        uint32_t clk_out3:5;
        uint32_t reserved_15:17;
    };
    uint32_t val;
} iomux_pin_ctrl_reg_t;

/** Type of gpio register
 *  IO MUX Configure Register for pad XTAL_32K_P
 */
typedef union {
    struct {
        /** mcu_oe : R/W; bitpos: [0]; default: 0;
         *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
         */
        uint32_t mcu_oe:1;
        /** slp_sel : R/W; bitpos: [1]; default: 0;
         *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
         */
        uint32_t slp_sel:1;
        /** mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
         *  internal pull-down disabled.
         */
        uint32_t mcu_wpd:1;
        /** mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
         *  internal pull-up disabled.
         */
        uint32_t mcu_wpu:1;
        /** mcu_ie : R/W; bitpos: [4]; default: 0;
         *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
         */
        uint32_t mcu_ie:1;
        /** mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  Select the drive strength of the pad during sleep mode. 0: ~5 mA. 1: ~10mA. 2:
         *  ~20mA. 3: ~40mA.
         */
        uint32_t mcu_drv:2;
        /** fun_wpd : R/W; bitpos: [7]; default: 0;
         *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
         *  disabled.
         */
        uint32_t fun_wpd:1;
        /** fun_wpu : R/W; bitpos: [8]; default: 0;
         *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
         *  disabled.
         */
        uint32_t fun_wpu:1;
        /** fun_ie : R/W; bitpos: [9]; default: 0;
         *  Input enable of the pad. 1: input enabled. 0: input disabled.
         */
        uint32_t fun_ie:1;
        /** fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  Select the drive strength of the pad. 0: ~5 mA. 1: ~10mA. 2: ~20mA. 3: ~40mA.
         */
        uint32_t fun_drv:2;
        /** mcu_sel : R/W; bitpos: [14:12]; default: 0;
         *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
         *  etc.
         */
        uint32_t mcu_sel:3;
        /** filter_en : R/W; bitpos: [15]; default: 0;
         *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
         */
        uint32_t filter_en:1;
        /** hys_en : R/W; bitpos: [16]; default: 0;
         *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
         *  Hysteresis disabled.
         */
        uint32_t hys_en:1;
        /** hys_sel : R/W; bitpos: [17]; default: 0;
         *  Select enabling signals of the pad from software and efuse hardware. 1: Select
         *  enabling siganl from slftware. 0: Select enabling signal from efuse hardware.
         */
        uint32_t hys_sel:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} iomux_gpio_reg_t;

/** Type of date register
 *  IO MUX Version Control Register
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 36708704;
         *  Version control register
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} iomux_date_reg_t;


typedef struct iomux_dev_t {
    volatile iomux_pin_ctrl_reg_t pin_ctrl;
    volatile iomux_gpio_reg_t gpio[27];
    uint32_t reserved_070[35];
    volatile iomux_date_reg_t date;
} iomux_dev_t;

extern iomux_dev_t IOMUX;

#ifndef __cplusplus
_Static_assert(sizeof(iomux_dev_t) == 0x100, "Invalid size of iomux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
