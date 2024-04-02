/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include "soc/soc_caps.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (SOC_USB_OTG_PERIPH_NUM > 0)

/**
 * @brief USB WRAP pull up/down resistor override values
 *
 * Specifies whether each pull up/down resistor should be enabled/disabled when
 * overriding connected USB PHY's pull resistors.
 */
typedef struct {
    bool dp_pu;     /**< D+ pull-up resistor enable/disable */
    bool dm_pu;     /**< D- pull-up resistor enable/disable */
    bool dp_pd;     /**< D+ pull-down resistor enable/disable */
    bool dm_pd;     /**< D- pull-down resistor enable/disable */
} usb_wrap_pull_override_vals_t;

/**
 * @brief USB WRAP test mode values
 *
 * Specifies the logic values of each of the USB FSLS Serial PHY interface
 * signals when in test mode.
 *
 * @note See section "2.2.1.13 FsLsSerialMode" of UTMI+ specification for more
 * details of each signal.
 */
typedef struct {
    bool tx_enable_n;   /**< Active low output enable signal */
    bool tx_dp;         /**< Single-ended D+ line driver */
    bool tx_dm;         /**< Single-ended D- line driver */
    bool rx_dp;         /**< Single-ended D+ signal from the transceiver */
    bool rx_dm;         /**< Single-ended D- signal from the transceiver */
    bool rx_rcv;        /**< Differential receive data from D+ and D- lines */
} usb_wrap_test_mode_vals_t;

#endif // (SOC_USB_OTG_PERIPH_NUM > 0)

#ifdef __cplusplus
}
#endif
