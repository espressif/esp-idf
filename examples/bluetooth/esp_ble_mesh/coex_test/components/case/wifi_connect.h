/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef __WIFI_CONNECT_H__
#define __WIFI_CONNECT_H__

#include "esp_err.h"
#include "esp_netif.h"
#include "esp_event.h"

esp_err_t example_connect(const char *ssid, const char *passwd);
esp_err_t example_disconnect(void);
esp_err_t example_configure_stdin_stdout(void);
esp_netif_t *get_example_netif(void);

#endif  /* __WIFI_CONNECT_H__ */
