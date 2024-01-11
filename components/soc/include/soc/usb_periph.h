/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_pins.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SOC_USB_OTG_SUPPORTED

/**
 * @brief A pin descriptor for init (DEPRECATED)
 *
 * Todo: Remove in IDF v6.0 (IDF-9029)
 *
 * @note These IO pins are deprecated. When connecting USB OTG to an external
 * FSLS PHY, the FSLS Serial Interface signals can be routed to any GPIO via the
 * GPI0 matrix. Thus, this mapping of signals to IO pins is meaningless.
 */
typedef struct {
    const int pin;
    const int func;
    const bool is_output;
    const int ext_phy_only;
} usb_iopin_dsc_t;

extern const usb_iopin_dsc_t usb_periph_iopins[];

#endif // SOC_USB_OTG_SUPPORTED

#ifdef __cplusplus
}
#endif
