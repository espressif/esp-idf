/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32
/* included here for ESP-IDF v4.x compatibility */
#include "soc/dport_reg.h"
#endif

#include "soc/hwcrypto_reg.h"
