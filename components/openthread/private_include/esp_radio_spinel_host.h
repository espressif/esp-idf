/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "sdkconfig.h"

#if CONFIG_OPENTHREAD_MULTIPAN_HOST_ENABLE
#define ESP_RADIO_SPINEL_IID_LIST_LEN 2
#define ESP_RADIO_SPINEL_HOST_INTERFACE_COUNT CONFIG_OPENTHREAD_MULTIPLE_INTERFACES_COUNT
#else
#define ESP_RADIO_SPINEL_IID_LIST_LEN 1
#endif
