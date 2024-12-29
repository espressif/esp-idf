/*
 * SPDX-FileCopyrightText: 2018-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include <stdint.h>
#include "soc/io_mux_reg.h"
#include "soc/gpio_struct.h"
#include "soc/gpio_reg.h"
#include "soc/soc_caps.h"
#include "soc/gpio_sig_map.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const uint32_t GPIO_PIN_MUX_REG[SOC_GPIO_PIN_COUNT];

extern const uint32_t GPIO_HOLD_MASK[SOC_GPIO_PIN_COUNT];

#ifdef __cplusplus
}
#endif
