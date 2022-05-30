/*
 * SPDX-FileCopyrightText: 2019-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_WIFI_PRIVATE_H
#define _ESP_WIFI_PRIVATE_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sys/queue.h"
#include "sdkconfig.h"
#include "esp_wifi_crypto_types.h"
#include "esp_private/wifi_os_adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_OSI_FUNCS_INITIALIZER() &g_wifi_osi_funcs

#ifdef __cplusplus
}
#endif

#endif /* _ESP_WIFI_PRIVATE_H */
