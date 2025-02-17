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

/** Group: Configuration Registers */
/** Type of gpio register
 *  IO MUX configuration register for gpio
 */
typedef union {
    struct {
        /** mcu_oe : R/W; bitpos: [0]; default: 0;
         *  Output enable of the pad in sleep mode.
         *  1: output enabled.
         *  0: output disabled.
         */
        uint32_t mcu_oe:1;
        /** slp_sel : R/W; bitpos: [1]; default: 0;
         *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
         */
        uint32_t slp_sel:1;
        /** mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  Pull-down enable of the pad in sleep mode.
         *  1: internal pull-down enabled.
         *  0: internal pull-down disabled.
         */
        uint32_t mcu_wpd:1;
        /** mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  Pull-up enable of the pad during sleep mode.
         *  1: internal pull-up enabled.
         *  0: internal pull-up disabled.
         */
        uint32_t mcu_wpu:1;
        /** mcu_ie : R/W; bitpos: [4]; default: 0;
         *  Input enable of the pad during sleep mode.
         *  1: input enabled.
         *  0: input disabled.
         */
        uint32_t mcu_ie:1;
        /** mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  Select the drive strength of the pad during sleep mode.
         *  0: ~5 mA\\
         *  1: ~10 mA\\
         *  2: ~20 mA\\
         *  3: ~40 mA\\
         */
        uint32_t mcu_drv:2;
        /** fun_wpd : R/W; bitpos: [7]; default: 0;
         *  Pull-down enable of the pad.
         *  1: internal pull-down enabled.
         *  0: internal pull-down disabled.
         */
        uint32_t fun_wpd:1;
        /** fun_wpu : R/W; bitpos: [8]; default: 0;
         *  Pull-up enable of the pad.
         *  1: internal pull-up enabled.
         *  0: internal pull-up disabled.
         */
        uint32_t fun_wpu:1;
        /** fun_ie : R/W; bitpos: [9]; default: 0;
         *  Input enable of the pad.
         *  1: input enabled.
         *  0: input disabled.
         */
        uint32_t fun_ie:1;
        /** fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  Select the drive strength of the pad.
         *  0: ~5 mA\\
         *  1: ~10 mA\\
         *  2: ~20 mA\\
         *  3: ~40 mA\\
         */
        uint32_t fun_drv:2;
        /** mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  Select IO MUX function for this signal.
         *  0: Select Function 1.
         *  1: Select Function 2. etc.
         *  ......\\
         */
        uint32_t mcu_sel:3;
        /** filter_en : R/W; bitpos: [15]; default: 0;
         *  Enable filter for pin input signals.
         *  1: Filter enabled.
         *  0: Filter disabled.
         */
        uint32_t filter_en:1;
        /** hys_en : R/W; bitpos: [16]; default: 0;
         *  Software enables hysteresis function for the pad.
         *  1: Filter enabled.
         *  0: Filter disabled.
         */
        uint32_t hys_en:1;
        /** hys_sel : R/W; bitpos: [17]; default: 0;
         *  Select enabling signals of the pad from software and efuse hardware.
         *  1: Hysteresis enabled.
         *  0: Hysteresis disabled.
         */
        uint32_t hys_sel:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} io_mux_gpio_reg_t;


/** Group: Version Register */
/** Type of date register
 *  Version control register
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 36770416;
         *  Version control register
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} io_mux_date_reg_t;


typedef struct {
    volatile io_mux_gpio_reg_t gpio[26];
    uint32_t reserved_074[101];
    volatile io_mux_date_reg_t date;
} io_mux_dev_t;

extern io_mux_dev_t IO_MUX;

#ifndef __cplusplus
_Static_assert(sizeof(io_mux_dev_t) == 0x200, "Invalid size of io_mux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
