/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "soc/soc_pins.h"
#include "soc/gpio_sig_map.h"
#include "soc/usb_reg.h"
#include "soc/usb_types.h"
#include "soc/usb_struct.h"
#include "soc/usb_wrap_reg.h"
#include "soc/usb_wrap_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A pin descriptor for init
 */
typedef struct {
    const int pin;
    const int func;
    const bool is_output;
    const int ext_phy_only;
} usb_iopin_dsc_t;

extern const usb_iopin_dsc_t usb_periph_iopins[];

#ifdef __cplusplus
}
#endif
