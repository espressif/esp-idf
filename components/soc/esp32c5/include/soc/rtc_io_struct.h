/**
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 *  SPDX-License-Identifier: Apache-2.0 OR MIT
 */
#pragma once

#include "soc/lp_gpio_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef lp_gpio_dev_t     rtc_io_dev_t;
#define RTCIO           LP_GPIO

#ifdef __cplusplus
}
#endif
