/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <spinel.h>

#if CONFIG_OPENTHREAD_NCP_VENDOR_HOOK

#define SPINEL_PROP_VENDOR_ESP_SET_COORDINATOR (SPINEL_PROP_VENDOR_ESP__BEGIN + 1)

#define SPINEL_PROP_VENDOR_ESP_SET_PENDINGMODE (SPINEL_PROP_VENDOR_ESP__BEGIN + 2)

#endif
