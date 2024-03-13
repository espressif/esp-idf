/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#if CONFIG_IDF_TARGET_ESP32
/* included here for ESP-IDF v4.x compatibility */
#include "soc/dport_reg.h"
#endif

#if SOC_SHA_SUPPORTED
#include "soc/hwcrypto_reg.h"
#endif
