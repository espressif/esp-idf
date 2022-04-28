/*
 * ESP BLE Mesh Example
 *
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "wifi_unit.h"
#include "wifi_connect.h"
#define TAG "WIFI_UINT"

uint32_t utils_get_system_ts(void)
{
    return esp_log_timestamp();
}

esp_err_t wifi_unit_client_establish(int *sock, const char *ip, const char *port)
{
    esp_err_t ret = 0;
    uint32_t start_ts;
    uint32_t timeout = 10000;
    struct sockaddr_in sock_addr;
    int s;

    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = ipaddr_addr(ip);
    sock_addr.sin_port = htons(atoi(port));

    start_ts = utils_get_system_ts();
    do {
        s = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (s < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        }
        ret = connect(s, (struct sockaddr *)&sock_addr, sizeof(sock_addr));
        if (ret == 0) {
            *sock = s;
            break;
        } else if (s > 0) {
            close(s);
        }
    } while (utils_get_system_ts() - start_ts < timeout);
    return ret;
}

esp_err_t wifi_unit_server_establish(int *socket_id, const char *port)
{
    esp_err_t ret = -1;
    struct sockaddr_in local_addr;
    uint32_t local_ip;

    static int ls_sock = -1;

    local_ip = wifi_util_get_ip();

    if (ls_sock < 0) {
        ls_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (ls_sock < 0) {
            ESP_LOGE(TAG, "create socket failed");
            return ls_sock;
        }

        local_addr.sin_family = AF_INET;
        local_addr.sin_port = htons(atoi(port));
        local_addr.sin_addr.s_addr = local_ip;
        ret = bind(ls_sock, (struct sockaddr *)&local_addr, sizeof(local_addr));
        if (ret != 0) {
            ESP_LOGE(TAG, "socket bind failed");
            return ret;
        }

        ret = listen(ls_sock, 1);
        if (ret < 0) {
            ESP_LOGE(TAG, "socket listen failed");
            return ret;
        }
    }

    struct sockaddr_in6 sourceAddr; // Large enough for both IPv4 or IPv6
    socklen_t addrLen = sizeof(sourceAddr);
    *socket_id = accept(ls_sock, (struct sockaddr *)&sourceAddr, &addrLen);
    if (*socket_id < 0) {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        return -1;
    }
    ESP_LOGI(TAG, "Socket accepted");
    return ret;
}

esp_err_t wifi_unit_tcp_recv(int socket_id, const char *duration, uint32_t user_date[])
{
    esp_err_t ret = -1;
    uint32_t start_ts;
    uint8_t *buffer;
    struct timeval tv_t;
    uint32_t *recv_len = &user_date[1];
    tv_t.tv_sec = 1;
    tv_t.tv_usec = 0;
    ret = setsockopt(socket_id, SOL_SOCKET, SO_RCVTIMEO, &tv_t, sizeof(tv_t));

    buffer = malloc(2920);

    if (buffer == NULL) {
        ESP_LOGE(TAG, "%s malloc fail\n", __func__);
        return ESP_ERR_NO_MEM;
    }

    start_ts = utils_get_system_ts();

    while (utils_get_system_ts() - start_ts < atoi(duration)) {
        ret = recv(socket_id, buffer, 2920, 0);
        if (ret > 0) {
            *recv_len = *recv_len + ret;
        } else if (ret == 0) {
            break;
        }
    }
    free(buffer);
    if (ret > 0 ) {
        ret = ESP_OK;
    }
    return ret;
}

esp_err_t wifi_util_tcp_send(int socket_id, uint32_t len, uint32_t delay, uint32_t *sent_len, uint32_t timeout)
{
    esp_err_t ret = ESP_OK;
    uint32_t start_ts;
    uint8_t *buffer;

    if ( len == 0) {
        return ESP_ERR_INVALID_ARG;
    }
    buffer = malloc(len);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "%s malloc fail\n", __func__);
        return ESP_ERR_NO_MEM;
    }

    start_ts = utils_get_system_ts();

    while (utils_get_system_ts() - start_ts < timeout) {
        ret = send(socket_id, buffer, len, 0);
        if (ret < 0) {
            ESP_LOGE(TAG, "recv failed: errno %d", errno);
            break;
        }
        *sent_len = *sent_len + ret;

        if (delay) {
            vTaskDelay(delay / portTICK_PERIOD_MS);
        }
    }
    if (ret == len) {
        ret = ESP_OK;
    } else {
        ESP_LOGE(TAG, "tcp send error, %d", ret);
        ret = -2;
    }

    free(buffer);

    return ret;
}

uint32_t wifi_util_get_ip(void)
{
    esp_netif_ip_info_t ip_info;
    esp_netif_t *netif = get_example_netif();
    esp_netif_get_ip_info(netif, &ip_info);
    return ip_info.ip.addr;
}

void wifi_util_init(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}
