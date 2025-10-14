/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <spinel.h>
#include <esp_openthread.h>

#if CONFIG_OPENTHREAD_NCP_VENDOR_HOOK

#define SPINEL_PROP_VENDOR_ESP_SET_COORDINATOR (SPINEL_PROP_VENDOR_ESP__BEGIN + 1)

#define SPINEL_PROP_VENDOR_ESP_SET_PENDINGMODE (SPINEL_PROP_VENDOR_ESP__BEGIN + 2)

#define SPINEL_PROP_VENDOR_ESP_COEX_EVENT (SPINEL_PROP_VENDOR_ESP__BEGIN + 3)

#endif

#ifdef __cplusplus
extern "C" {
#endif

void otAppNcpInit(otInstance *aInstance);

#ifdef __cplusplus
}
#endif
