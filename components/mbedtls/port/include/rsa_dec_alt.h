/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cpluscplus
extern "C" {
#endif

#include "sdkconfig.h"

#if defined(CONFIG_SOC_DIG_SIGN_SUPPORTED)
#include "esp_ds/esp_ds_rsa.h"
#endif

#ifdef __cpluscplus
}
#endif
