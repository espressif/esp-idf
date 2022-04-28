/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#ifndef _WIFI_UNIT_H
#define _WIFI_UNIT_H

#include "esp_wifi_types.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"

#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

uint32_t utils_get_system_ts(void);

void wifi_util_init(void);

uint32_t wifi_util_get_ip(void);
esp_err_t wifi_unit_client_establish(int *sock, const char *ip, const char *port);
esp_err_t wifi_unit_server_establish(int *socket_id, const char *port);

esp_err_t wifi_unit_tcp_recv(int socket_id, const char *duration, uint32_t user_date[]);
esp_err_t wifi_util_tcp_send(int socket_id, uint32_t len, uint32_t delay, uint32_t *sent_len, uint32_t timeout);
#endif /* _WIFI_UNIT_H */
