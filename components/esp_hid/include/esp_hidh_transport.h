/*
 * SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "sdkconfig.h"

#if CONFIG_GATTC_ENABLE
#include "esp_hidh_gattc.h"
#endif

#if CONFIG_BLUEDROID_ENABLED
#include "esp_hidh_bluedroid.h"
#endif

#if CONFIG_BT_NIMBLE_ENABLED
#include "esp_hidh_nimble.h"
#endif

#ifdef __cplusplus
}
#endif
