/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "esp_wifi_types_generic.h"
#if __has_include("esp_wifi_types_native.h")
#include "esp_wifi_types_native.h"
#else

#ifdef __cplusplus
extern "C" {
#endif

/*
 * In case we have no native types, we can still provide opaque structs,
 * so the most common APIs could work and others would compile.
 * This could happen for chipsets with no wifi, yet without local esp_wifi_remote.
 */
typedef struct wifi_csi_config_t wifi_csi_config_t;
typedef struct wifi_pkt_rx_ctrl_t wifi_pkt_rx_ctrl_t;

#ifdef __cplusplus
}
#endif

#endif // __has_include("esp_wifi_types_native.h")
