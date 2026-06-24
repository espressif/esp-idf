/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_wifi_remote.h"

#define WEAK __attribute__((weak))

WEAK ESP_EVENT_DEFINE_BASE(WIFI_EVENT);

#if !CONFIG_SOC_WIFI_SUPPORTED
struct wifi_osi_funcs_t { };
#endif

WEAK wifi_osi_funcs_t g_wifi_osi_funcs;
WEAK const wpa_crypto_funcs_t g_wifi_default_wpa_crypto_funcs;
WEAK uint64_t g_wifi_feature_caps =
#if CONFIG_ESP_WIFI_ENABLE_WPA3_SAE
    CONFIG_FEATURE_WPA3_SAE_BIT |
#endif
#if CONFIG_SPIRAM
    CONFIG_FEATURE_CACHE_TX_BUF_BIT |
#endif
#if CONFIG_ESP_WIFI_FTM_INITIATOR_SUPPORT
    CONFIG_FEATURE_FTM_INITIATOR_BIT |
#endif
#if CONFIG_ESP_WIFI_FTM_RESPONDER_SUPPORT
    CONFIG_FEATURE_FTM_RESPONDER_BIT |
#endif
    0;
