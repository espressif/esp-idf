/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "esp_check.h"

#define EXAMPLE_ESP_MAXIMUM_RETRY  5

static const char *TAG = "tcp_stream";

static int s_tcp_client_connect(const char *host_ip, int port)
{
    int addr_family = 0;
    int ip_protocol = 0;

    struct sockaddr_in dest_addr = {};
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        goto _exit;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, port);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        goto _exit;
    }
    ESP_LOGI(TAG, "Successfully connected");
    return sock;
_exit:
    if (sock > 1) {
        close(sock);
    }
    return -1;
}

FILE* esp_probe_open_tcp_stream(const char *host_ip, int port)
{
    esp_err_t ret = ESP_FAIL;

    // Create a TCP socket
    int sock = s_tcp_client_connect(host_ip, port);
    if (sock < 0) {
        goto err;
    }
    // LWIP socket has been registered to the VFS, associate the TCP socket with the file descriptor for output stream
    FILE *f = fdopen(sock, "wb");
    ESP_GOTO_ON_FALSE(f, ESP_FAIL, err, TAG, "open tcp stream failed, error %s", strerror(errno));
    return f;

err:
    ESP_LOGE(TAG, "Open TCP stream failed with error code %s", esp_err_to_name(ret));
    return NULL;
}

void esp_probe_close_tcp_stream(FILE *f)
{
    // Get the socket by the file descriptor
    int sock = fileno(f);
    // Close the file stream
    fclose(f);
    // Shutdown explicitly to inform the server no further data will be sent
    shutdown(sock, SHUT_RDWR);
    // Close the socket
    close(sock);
    ESP_LOGI(TAG, "TCP stream closed");
}
