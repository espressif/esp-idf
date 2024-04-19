/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

// TODO: IDF-9197 This file is created to glob the gpio_sig_map.h files correctly in esp-docs

#if CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION
#include "../../beta3/include/soc/gpio_sig_map.h"
#elif CONFIG_IDF_TARGET_ESP32C5_MP_VERSION
#include "../../mp/include/soc/gpio_sig_map.h"
#endif
