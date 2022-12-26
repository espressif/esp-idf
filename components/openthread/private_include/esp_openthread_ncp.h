/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_openthread.h"

#include "lib/spinel/spinel.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SPINEL_PROP_VENDOR_ESP_OTA_TRIGGER = (SPINEL_PROP_VENDOR_ESP__BEGIN + 0),
    SPINEL_PROP_VENDOR_ESP_OTA_DATA = (SPINEL_PROP_VENDOR_ESP__BEGIN + 1),
    SPINEL_PROP_VENDOR_ESP_OTA_FINISH = (SPINEL_PROP_VENDOR_ESP__BEGIN + 2),
};

#ifdef __cplusplus
}
#endif
