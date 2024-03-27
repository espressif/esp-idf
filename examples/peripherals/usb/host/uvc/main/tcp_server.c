/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "addr_from_stdin.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "tcp_server.h"
#include "mdns.h"

#define TAG "tcp_server"
#define PORT 2222

typedef struct {
    int sock;
    int listen_sock;
    RingbufHandle_t buffer;
    volatile bool close_request;
    bool is_active;
} tcp_server_t;

#ifdef CONFIG_EXAMPLE_ENABLE_STREAMING

static tcp_server_t *s_server;

void socket_close(tcp_server_t *server)
{
    ESP_LOGI(TAG, "Closing socket");
    shutdown(server->sock, 0);
    close(server->sock);
    close(server->listen_sock);
}

static void sender_task(void *arg)
{
    tcp_server_t *server = (tcp_server_t *)arg;
    server->is_active = true;

    while (1) {
        size_t bytes_received = 0;
        char *payload = (char *)xRingbufferReceiveUpTo(
                            server->buffer, &bytes_received, pdMS_TO_TICKS(2500), 20000);

        if (payload != NULL && server->is_active) {
            int sent = send(server->sock, payload, bytes_received, 0);
            if (sent < 0) {
                ESP_LOGE(TAG, "Error occurred during sending: errno %d, \
                               Shutting down tcp server...", errno);
                server->is_active = false;
            }
            vRingbufferReturnItem(server->buffer, (void *)payload);
        }

        if (server->close_request) {
            socket_close(server);
            vRingbufferDelete(server->buffer);
            vTaskDelete(NULL);
            s_server = NULL;
            free(server);
            return;
        }
    }
}

esp_err_t tcp_server_send(uint8_t *payload, size_t size)
{
    if (!s_server || !s_server->is_active) {
        return ESP_OK;
    }

    if (xRingbufferSend(s_server->buffer, payload, size, pdMS_TO_TICKS(1)) != pdTRUE) {
        ESP_LOGW(TAG, "Failed to send frame to ring buffer.");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t start_mdns_service(void)
{
    esp_err_t err = mdns_init();
    if (err) {
        printf("MDNS Init failed: %d\n", err);
        return ESP_FAIL;
    }

    mdns_hostname_set("esp-cam");

    return ESP_OK;
}

static esp_err_t create_server(tcp_server_t *server)
{
    char addr_str[128];
    int ip_protocol = 0;
    int addr_family = AF_INET;
    struct sockaddr_storage dest_addr;

    struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
    dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
    dest_addr_ip4->sin_family = addr_family;
    dest_addr_ip4->sin_port = htons(PORT);
    ip_protocol = IPPROTO_IP;

    server->listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (server->listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }
    int opt = 1;
    setsockopt(server->listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(server->listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        close(server->listen_sock);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(server->listen_sock, 1);
    if (err != 0) {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        close(server->listen_sock);
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Socket listening...");
    ESP_LOGI(TAG, "Execute player.py script");

    struct sockaddr_storage source_addr;
    socklen_t addr_len = sizeof(source_addr);
    server->sock = accept(server->listen_sock, (struct sockaddr *)&source_addr, &addr_len);
    if (server->sock < 0) {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        close(server->listen_sock);
        return ESP_FAIL;
    }

    // Convert ip address to string
    if (source_addr.ss_family == PF_INET) {
        inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
    }
    ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

    return ESP_OK;
}

esp_err_t tcp_server_wait_for_connection(void)
{
    TaskHandle_t task_handle = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(start_mdns_service());
    ESP_ERROR_CHECK(example_connect());

    tcp_server_t *server = calloc(1, sizeof(tcp_server_t));
    if (server == NULL) {
        return ESP_ERR_NO_MEM;
    }

    server->buffer = xRingbufferCreate(100000, RINGBUF_TYPE_BYTEBUF);
    if (server->buffer == NULL) {
        free(server);
        return ESP_ERR_NO_MEM;;
    }

    if (create_server(server) != ESP_OK) {
        vRingbufferDelete(server->buffer);
        free(server);
        return ESP_FAIL;
    }

    BaseType_t task_created = xTaskCreate(sender_task, "sender_task", 4096, server, 10, &task_handle);
    if (!task_created) {
        socket_close(server);
        vRingbufferDelete(server->buffer);
        free(server);
        return ESP_ERR_NO_MEM;
    }

    s_server = server;
    return ESP_OK;
}

void tcp_server_close_when_done(void)
{
    if (s_server) {
        s_server->close_request = true;
    }
}

#else

esp_err_t tcp_server_wait_for_connection(void)
{
    return ESP_OK;
}

esp_err_t tcp_server_send(uint8_t *payload, size_t size)
{
    return ESP_OK;
}

void tcp_server_close_when_done(void) { }

#endif
