/**
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
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
         *  Configures whether or not to enable the output of gpio in sleep mode.
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t mcu_oe:1;
        /** slp_sel : R/W; bitpos: [1]; default: 0;
         *  Configures whether or not to enter sleep mode for gpio.\\
         *  0: Not enter\\
         *  1: Enter\\
         */
        uint32_t slp_sel:1;
        /** mcu_wpd : R/W; bitpos: [2]; default: 0;
         *  Configure whether or not to enable pull-down resistor of gpio in sleep mode.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t mcu_wpd:1;
        /** mcu_wpu : R/W; bitpos: [3]; default: 0;
         *  Configures whether or not to enable pull-up resistor of gpio during sleep mode. \\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t mcu_wpu:1;
        /** mcu_ie : R/W; bitpos: [4]; default: 0;
         *  Configures whether or not to enable the input of gpio during sleep mode.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t mcu_ie:1;
        /** mcu_drv : R/W; bitpos: [6:5]; default: 0;
         *  Configures the drive strength of gpio during sleep mode. \\
         *  0: ~5 mA\\
         *  1: ~10 mA\\
         *  2: ~20 mA\\
         *  3: ~40 mA\\
         */
        uint32_t mcu_drv:2;
        /** fun_wpd : R/W; bitpos: [7]; default: 0;
         *  Configures whether or not to enable pull-down resistor of gpio.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t fun_wpd:1;
        /** fun_wpu : R/W; bitpos: [8]; default: 0;
         *  Configures whether or not enable pull-up resistor of gpio.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t fun_wpu:1;
        /** fun_ie : R/W; bitpos: [9]; default: 0;
         *  Configures whether or not to enable input of gpio.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t fun_ie:1;
        /** fun_drv : R/W; bitpos: [11:10]; default: 2;
         *  Configures the drive strength of gpio. \\
         *  0: ~5 mA\\
         *  1: ~10 mA\\
         *  2: ~20 mA\\
         *  3: ~40 mA\\
         */
        uint32_t fun_drv:2;
        /** mcu_sel : R/W; bitpos: [14:12]; default: 1;
         *  Configures to select IO MUX function for this signal. \\
         *  0: Select Function 0\\
         *  1: Select Function 1\\
         *  ......\\
         */
        uint32_t mcu_sel:3;
        /** filter_en : R/W; bitpos: [15]; default: 0;
         *  Configures whether or not to enable filter for pin input signals.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t filter_en:1;
        /** hys_en : R/W; bitpos: [16]; default: 0;
         *  Configures whether or not to enable the hysteresis function of the pin when
         *  IO_MUX_HYS_SEL is set to 1.\\
         *  0: Disable\\
         *  1: Enable\\
         */
        uint32_t hys_en:1;
        /** hys_sel : R/W; bitpos: [17]; default: 0;
         *  Configures to choose the signal for enabling the hysteresis function for gpio. \\
         *  0: Choose the output enable signal of eFuse\\
         *  1: Choose the output enable signal of IO_MUX_HYS_EN\\
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
    volatile io_mux_gpio_reg_t gpio[29];
    uint32_t reserved_074[98];
    volatile io_mux_date_reg_t date;
} io_mux_dev_t;

extern io_mux_dev_t IOMUX;

#ifndef __cplusplus
_Static_assert(sizeof(io_mux_dev_t) == 0x200, "Invalid size of io_mux_dev_t structure");
#endif

#ifdef __cplusplus
}
#endif
