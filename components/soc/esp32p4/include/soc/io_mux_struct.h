/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/** Type of GPIO register
 *  IO MUX gpio configuration register
 */
typedef union {
    struct {
        /** mcu_oe : R/W; bitpos: [0]; default: 0;
         *  output enable on sleep mode
         */
        uint32_t mcu_oe:1;
        /** slp_sel : R/W; bitpos: [1]; default: 0;
         *  io sleep mode enable. set 1 to enable sleep mode.
         */
        uint32_t slp_sel:1;
        /** mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  pull-down enable on sleep mode
         */
        uint32_t mcu_wpd:1;
        /** mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  pull-up enable on sleep mode
         */
        uint32_t mcu_wpu:1;
        /** mcu_ie : R/W; bitpos: [4]; default: 0;
         *  input enable on sleep mode
         */
        uint32_t mcu_ie:1;
        /** mcu_drv : R/W; bitpos: [5:6]; default: 0;
         *  select drive strength on sleep mode
         */
        uint32_t mcu_drv:2;
        /** fun_wpd : R/W; bitpos: [7]; default: 0;
         *  pull-down enable
         */
        uint32_t fun_wpd:1;
        /** fun_wpu : R/W; bitpos: [8]; default: 0;
         *  pull-up enable
         */
        uint32_t fun_wpu:1;
        /** fun_ie : R/W; bitpos: [9]; default: 0;
         *  input enable
         */
        uint32_t fun_ie:1;
        /** fun_drv : R/W; bitpos: [10:11]; default: 2;
         *  select drive strength, 0:5mA, 1:10mA, 2:20mA, 3:40mA
         */
        uint32_t fun_drv:2;
        /** mcu_sel : R/W; bitpos: [12:14]; default: 0;
         *  0:select function0, 1:select function1 ...
         */
        uint32_t mcu_sel:3;
        /** filter_en : R/W; bitpos: [15]; default: 0;
         *  input filter enable
         */
        uint32_t filter_en:1;
        uint32_t reserved16 :16;
    };
    uint32_t val;
} io_mux_gpio_reg_t;

/** Type of date register
 *  IO_MUX version register
 */
typedef union {
    struct {
        /** date : R/W; bitpos: [27:0]; default: 2101794;
         *  version register
         */
        uint32_t date:28;
        uint32_t reserved_28:4;
    };
    uint32_t val;
} io_mux_date_reg_t;


typedef struct io_mux_dev_t {
    uint32_t reserved_0;
    volatile io_mux_gpio_reg_t gpio[57];
    uint32_t reserved_e8[7];
    volatile io_mux_date_reg_t date;
} io_mux_dev_t;

extern io_mux_dev_t IO_MUX;

#ifndef __cplusplus
_Static_assert(sizeof(io_mux_dev_t) == 0x108, "Invalid size of io_mux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
