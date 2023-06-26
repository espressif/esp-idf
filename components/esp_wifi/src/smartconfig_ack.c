/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * After station connects to AP and gets IP address by smartconfig,
 * it will use UDP to send 'ACK' to cellphone.
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include <string.h>
#include "sys/socket.h"
#include "esp_smartconfig.h"
#include "smartconfig_ack.h"

#define SC_ACK_TASK_PRIORITY             2          /*!< Priority of sending smartconfig ACK task */
#define SC_ACK_TASK_STACK_SIZE           2048       /*!< Stack size of sending smartconfig ACK task */

#define SC_ACK_TOUCH_DEVICE_PORT         7001       /*!< ESPTouch UDP port of server on device */
#define SC_ACK_TOUCH_SERVER_PORT         18266      /*!< ESPTouch UDP port of server on cellphone */
#define SC_ACK_TOUCH_V2_SERVER_PORT(i)   (18266+i*10000)     /*!< ESPTouch v2 UDP port of server on cellphone */
#define SC_ACK_AIRKISS_SERVER_PORT       10000      /*!< Airkiss UDP port of server on cellphone */
#define SC_ACK_AIRKISS_DEVICE_PORT       10001      /*!< Airkiss UDP port of server on device */
#define SC_ACK_AIRKISS_TIMEOUT           1500       /*!< Airkiss read data timout millisecond */

#define SC_ACK_TOUCH_LEN                 11         /*!< Length of ESPTouch ACK context */
#define SC_ACK_AIRKISS_LEN               7          /*!< Length of Airkiss ACK context */

#define SC_ACK_MAX_COUNT                 30         /*!< Maximum count of sending smartconfig ACK */

/**
 * @brief Smartconfig parameters passed to sc_ack_send call.
 */
typedef struct sc_ack {
    smartconfig_type_t type;      /*!< Smartconfig type(ESPTouch or AirKiss) */
    struct {
        uint8_t token;            /*!< Smartconfig token from the cellphone */
        uint8_t mac[6];           /*!< MAC address of station */
        uint8_t ip[4];            /*!< IP address of cellphone */
    } ctx;
} sc_ack_t;

static const char *TAG = "smartconfig";

/* Flag to indicate sending smartconfig ACK or not. */
static bool s_sc_ack_send = false;

static int sc_ack_send_get_errno(int fd)
{
    int sock_errno = 0;
    u32_t optlen = sizeof(sock_errno);

    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen) < 0) {
        return sock_errno;
    }

    return 0;
}

static void sc_ack_send_task(void *pvParameters)
{
    sc_ack_t *ack = (sc_ack_t *)pvParameters;
    esp_netif_ip_info_t local_ip;
    uint8_t remote_ip[4];
    memcpy(remote_ip, ack->ctx.ip, sizeof(remote_ip));
    struct sockaddr_in server_addr;
    socklen_t sin_size = sizeof(server_addr);
    int send_sock = -1;
    int optval = 1;
    int sendlen;
    int ack_len = (ack->type == SC_TYPE_ESPTOUCH) ? SC_ACK_TOUCH_LEN : SC_ACK_AIRKISS_LEN;
    uint8_t packet_count = 1;
    int err;
    int ret;

    int remote_port = 0;
    if (ack->type == SC_TYPE_ESPTOUCH) {
        remote_port = SC_ACK_TOUCH_SERVER_PORT;
    } else if (ack->type == SC_TYPE_ESPTOUCH_V2) {
        uint8_t port_bit =  ack->ctx.token;
        if(port_bit > 3) {
            port_bit = 0;
        }
        remote_port = SC_ACK_TOUCH_V2_SERVER_PORT(port_bit);
        memset(remote_ip, 0xFF, sizeof(remote_ip));
    } else {
        remote_port = SC_ACK_AIRKISS_SERVER_PORT;
    }

    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    memcpy(&server_addr.sin_addr.s_addr, remote_ip, sizeof(remote_ip));
    server_addr.sin_port = htons(remote_port);

    esp_wifi_get_mac(WIFI_IF_STA, ack->ctx.mac);

    vTaskDelay(200 / portTICK_PERIOD_MS);

    while (s_sc_ack_send) {
        /* Get local IP address of station */
        ret = esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &local_ip);
        if ((ESP_OK == ret) && (local_ip.ip.addr != INADDR_ANY)) {
            /* If ESPTouch, smartconfig ACK contains local IP address. */
            if (ack->type == SC_TYPE_ESPTOUCH) {
                memcpy(ack->ctx.ip, &local_ip.ip.addr, 4);
            }

            /* Create UDP socket. */
            send_sock = socket(AF_INET, SOCK_DGRAM, 0);
            if ((send_sock < LWIP_SOCKET_OFFSET) || (send_sock > (FD_SETSIZE - 1))) {
                ESP_LOGE(TAG,  "Creat udp socket failed");
                goto _end;
            }

            if (setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int)) < 0) {
                ESP_LOGE(TAG,  "setsockopt SO_BROADCAST failed");
                goto _end;
            }

            if (setsockopt(send_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) < 0) {
                ESP_LOGE(TAG,  "setsockopt SO_REUSEADDR failed");
                goto _end;
            }

            if (ack->type == SC_TYPE_AIRKISS) {
                char data = 0;
                struct sockaddr_in local_addr, from;
                socklen_t sockadd_len = sizeof(struct sockaddr);
                struct timeval timeout = {
                    SC_ACK_AIRKISS_TIMEOUT / 1000,
                    SC_ACK_AIRKISS_TIMEOUT % 1000 * 1000
                };

                bzero(&local_addr, sizeof(struct sockaddr_in));
                bzero(&from, sizeof(struct sockaddr_in));
                local_addr.sin_family = AF_INET;
                local_addr.sin_addr.s_addr = INADDR_ANY;
                if (ack->type == SC_TYPE_AIRKISS) {
                    local_addr.sin_port = htons(SC_ACK_AIRKISS_DEVICE_PORT);
                } else {
                    local_addr.sin_port = htons(SC_ACK_TOUCH_DEVICE_PORT);
                }

                if (bind(send_sock, (struct sockaddr *)&local_addr, sockadd_len) < 0) {
                    ESP_LOGE(TAG,  "socket bind failed");
                    goto _end;
                }
                if (setsockopt(send_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
                    goto _end;
                }

                if (recvfrom(send_sock, &data, 1, 0, (struct sockaddr *)&from, &sockadd_len) < 0) {
                    goto _end;
                }
                if (from.sin_addr.s_addr != INADDR_ANY) {
                    memcpy(remote_ip, &from.sin_addr, 4);
                    server_addr.sin_addr.s_addr = from.sin_addr.s_addr;
                } else {
                    server_addr.sin_addr.s_addr = INADDR_BROADCAST;
                }
            }

            uint32_t ip_addr = server_addr.sin_addr.s_addr;
            while (s_sc_ack_send) {
                /* Send smartconfig ACK every 100ms. */
                vTaskDelay(100 / portTICK_PERIOD_MS);

                if (ip_addr != INADDR_BROADCAST) {
                    sendto(send_sock, &ack->ctx, ack_len, 0, (struct sockaddr*) &server_addr, sin_size);
                    server_addr.sin_addr.s_addr = INADDR_BROADCAST;
                    sendlen = sendto(send_sock, &ack->ctx, ack_len, 0, (struct sockaddr*) &server_addr, sin_size);
                    server_addr.sin_addr.s_addr = ip_addr;
                } else {
                    sendlen = sendto(send_sock, &ack->ctx, ack_len, 0, (struct sockaddr*) &server_addr, sin_size);
                }

                if (sendlen <= 0) {
                    err = sc_ack_send_get_errno(send_sock);
                    ESP_LOGD(TAG, "send failed, errno %d", err);
                    vTaskDelay(100 / portTICK_PERIOD_MS);
                }

                /*  Send 30 smartconfig ACKs. Then smartconfig is successful. */
                if (packet_count++ >= SC_ACK_MAX_COUNT) {
                    esp_event_post(SC_EVENT, SC_EVENT_SEND_ACK_DONE, NULL, 0, portMAX_DELAY);
                    goto _end;
                }
            }
        }
        else {
            vTaskDelay((TickType_t)(100 / portTICK_PERIOD_MS));
        }
    }

_end:
    close(send_sock);
    free(ack);
    vTaskDelete(NULL);
}

esp_err_t sc_send_ack_start(smartconfig_type_t type, uint8_t token, uint8_t *cellphone_ip)
{
    sc_ack_t *ack = NULL;

    if (cellphone_ip == NULL) {
        ESP_LOGE(TAG, "Cellphone IP address is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    ack = malloc(sizeof(sc_ack_t));
    if (ack == NULL) {
        ESP_LOGE(TAG, "ACK parameter malloc fail");
        return ESP_ERR_NO_MEM;
    }
    ack->type = type;
    ack->ctx.token = token;
    memcpy(ack->ctx.ip, cellphone_ip, 4);

    s_sc_ack_send = true;

    if (xTaskCreate(sc_ack_send_task, "sc_ack_send_task", SC_ACK_TASK_STACK_SIZE, ack, SC_ACK_TASK_PRIORITY, NULL) != pdPASS) {
        ESP_LOGE(TAG, "Create sending smartconfig ACK task fail");
        free(ack);
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}

void sc_send_ack_stop(void)
{
    s_sc_ack_send = false;
}
