/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_BT_BLUEDROID_ENABLED) && (CONFIG_BT_BLUEDROID_ENABLED) && (CONFIG_BT_HFP_ENABLE) && \
    defined(CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN) && (CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN == 0)

#pragma message ("BT: forcing BR/EDR max sync conn eff to 1 (Bluedroid HFP requires SCO/eSCO)")
#define UC_BR_EDR_CTRL_MAX_SYNC_CONN_EFF (1)

#elif defined(CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN)

#define UC_BR_EDR_CTRL_MAX_SYNC_CONN_EFF (CONFIG_BTDM_CTRL_BR_EDR_MAX_SYNC_CONN)

#else

#define UC_BR_EDR_CTRL_MAX_SYNC_CONN_EFF (0)

#endif

/* Enable BR/EDR power control vsc */
#if defined(CONFIG_BT_CLASSIC_ENABLE_POWER_CTRL_VSC)
#define UC_BR_EDR_POWER_CTRL_VSC_ENABLED        CONFIG_BT_CLASSIC_ENABLE_POWER_CTRL_VSC
#else
#define UC_BR_EDR_POWER_CTRL_VSC_ENABLED        (0)
#endif

#ifdef __cplusplus
}
#endif
