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
/** Type of gpion register
 *  IO_MUX Configure Register
 */
typedef union {
    struct {
        /** gpion_mcu_oe : R/W; bitpos: [0]; default: 0;
         *  Output enable of the pad in sleep mode. 1: output enabled. 0: output disabled.
         */
        uint32_t gpion_mcu_oe:1;
        /** gpion_slp_sel : R/W; bitpos: [1]; default: 0;
         *  Sleep mode selection of this pad. Set to 1 to put the pad in pad mode.
         */
        uint32_t gpion_slp_sel:1;
        /** gpion_mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  Pull-down enable of the pad in sleep mode. 1: internal pull-down enabled. 0:
         *  internal pull-down disabled.
         */
        uint32_t gpion_mcu_wpd:1;
        /** gpion_mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  Pull-up enable of the pad during sleep mode. 1: internal pull-up enabled. 0:
         *  internal pull-up disabled.
         */
        uint32_t gpion_mcu_wpu:1;
        /** gpion_mcu_ie : R/W; bitpos: [4]; default: 0;
         *  Input enable of the pad during sleep mode. 1: input enabled. 0: input disabled.
         */
        uint32_t gpion_mcu_ie:1;
        /** gpion_mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  Select the drive strength of the pad during sleep mode.
         */
        uint32_t gpion_mcu_drv:2;
        /** gpion_fun_wpd : R/W; bitpos: [7]; default: 0;
         *  Pull-down enable of the pad. 1: internal pull-down enabled. 0: internal pull-down
         *  disabled.
         */
        uint32_t gpion_fun_wpd:1;
        /** gpion_fun_wpu : R/W; bitpos: [8]; default: 0;
         *  Pull-up enable of the pad. 1: internal pull-up enabled. 0: internal pull-up
         *  disabled.
         */
        uint32_t gpion_fun_wpu:1;
        /** gpion_fun_ie : R/W; bitpos: [9]; default: 0;
         *  Input enable of the pad. 1: input enabled. 0: input disabled.
         */
        uint32_t gpion_fun_ie:1;
        /** gpion_fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  Select the drive strength of the pad.
         */
        uint32_t gpion_fun_drv:2;
        /** gpion_mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  Select IO MUX function for this signal. 0: Select Function 1. 1: Select Function 2.
         *  etc.
         */
        uint32_t gpion_mcu_sel:3;
        /** gpion_filter_en : R/W; bitpos: [15]; default: 0;
         *  Enable filter for pin input signals. 1: Filter enabled. 0: Filter disabled.
         */
        uint32_t gpion_filter_en:1;
        /** gpion_hys_en : R/W; bitpos: [16]; default: 0;
         *  Software enables hysteresis function for the pad. 1: Hysteresis enabled. 0:
         *  Hysteresis disabled.
         */
        uint32_t gpion_hys_en:1;
        /** gpion_hys_sel : R/W; bitpos: [17]; default: 0;
         *  Select enabling signals of the pad from software and efuse hardware. 1: Select
         *  enabling signal from slftware. 0: Select enabling signal from efuse hardware.
         */
        uint32_t gpion_hys_sel:1;
        uint32_t reserved_18:14;
    };
    uint32_t val;
} io_mux_gpion_reg_t;

/** Type of date register
 *  IO_MUX Version Control Register
 */
typedef union {
    struct {
        /** reg_date : R/W; bitpos: [27:0]; default: 37823072;
         *  Version control register
         */
        uint32_t reg_date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} io_mux_date_reg_t;


typedef struct {
    volatile io_mux_gpion_reg_t gpion[30];
    uint32_t reserved_078[97];
    volatile io_mux_date_reg_t date;
} io_mux_dev_t;

extern io_mux_dev_t IO_MUX;

#ifndef __cplusplus
_Static_assert(sizeof(io_mux_dev_t) == 0x200, "Invalid size of io_mux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
