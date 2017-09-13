// Copyright 2010-2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * After station connects to AP and gets IP address by smartconfig,
 * it will use UDP to send 'ACK' to cellphone.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "tcpip_adapter.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_smartconfig.h"
#include "smartconfig.h"

static const char *TAG = "smartconfig";

/* Flag to indicate sending smartconfig ACK or not. */
static bool s_sc_ack_send = false;

static int sc_ack_send_get_errno(int fd)
{
    int sock_errno = 0;
    u32_t optlen = sizeof(sock_errno);

    getsockopt(fd, SOL_SOCKET, SO_ERROR, &sock_errno, &optlen);

    return sock_errno;
}

static void sc_ack_send_task(void *pvParameters)
{
    sc_ack_t *ack = (sc_ack_t *)pvParameters;
    tcpip_adapter_ip_info_t local_ip;
    uint8_t *remote_ip = ack->ctx.ip;
    int remote_port = (ack->type == SC_ACK_TYPE_ESPTOUCH) ? SC_ACK_TOUCH_SERVER_PORT : SC_ACK_AIRKISS_SERVER_PORT;
    struct sockaddr_in server_addr;
    socklen_t sin_size = sizeof(server_addr);
    int send_sock = 0;
    int optval = 1;
    int sendlen;
    int ack_len = (ack->type == SC_ACK_TYPE_ESPTOUCH) ? SC_ACK_TOUCH_LEN : SC_ACK_AIRKISS_LEN;
    uint8_t packet_count = 1;
    int err;
    int ret;

    bzero(&server_addr, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr((const char*)remote_ip);
    server_addr.sin_port = htons(remote_port);

    esp_wifi_get_mac(WIFI_IF_STA, ack->ctx.mac);

    vTaskDelay(200 / portTICK_RATE_MS);

    while (s_sc_ack_send) {
        /* Get local IP address of station */
        ret = tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &local_ip);
        if ((ESP_OK == ret) && (local_ip.ip.addr != INADDR_ANY)) {
            /* If ESP touch, smartconfig ACK contains local IP address. */
            if (ack->type == SC_ACK_TYPE_ESPTOUCH) {
                memcpy(ack->ctx.ip, &local_ip.ip.addr, 4);
            }

            /* Create UDP socket. */
            send_sock = socket(AF_INET, SOCK_DGRAM, 0);
            if (send_sock < 0) {
                ESP_LOGE(TAG,  "Creat udp socket failed");
                free(ack);
                vTaskDelete(NULL);
            }

            setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST | SO_REUSEADDR, &optval, sizeof(int));

            while (s_sc_ack_send) {
                /* Send smartconfig ACK every 100ms. */
                vTaskDelay(100 / portTICK_RATE_MS);

                sendlen = sendto(send_sock, &ack->ctx, ack_len, 0, (struct sockaddr*) &server_addr, sin_size);
                if (sendlen > 0) {
                    /* Totally send 30 smartconfig ACKs. Then smartconfig is successful. */
                    if (packet_count++ >= SC_ACK_MAX_COUNT) {
                        if (ack->link_flag) {
                            *ack->link_flag = 1;
                        }
                        if (ack->cb) {
                            ack->cb(SC_STATUS_LINK_OVER, remote_ip);
                        }
                        close(send_sock);
                        free(ack);
                        vTaskDelete(NULL);
                    }
                }
                else {
                    err = sc_ack_send_get_errno(send_sock);
                    if (err == ENOMEM || err == EAGAIN) {
                        ESP_LOGD(TAG, "send failed, errno %d", err);
                        continue;
                    }
                    ESP_LOGE(TAG, "send failed, errno %d", err);
                    close(send_sock);
                    free(ack);
                    vTaskDelete(NULL);
                }
            }
        }
        else {
            vTaskDelay((portTickType)(100 / portTICK_RATE_MS));
        }
    }

    free(ack);
    vTaskDelete(NULL);
}

void sc_ack_send(sc_ack_t *param)
{
    sc_ack_t *ack = NULL;

    if (param == NULL) {
        ESP_LOGE(TAG, "Smart config ack parameter error");
        return;
    }

    ack = malloc(sizeof(sc_ack_t));
    if (ack == NULL) {
        ESP_LOGE(TAG, "Smart config ack parameter malloc fail");
        return;
    }
    memcpy(ack, param, sizeof(sc_ack_t));

    s_sc_ack_send = true;

    xTaskCreate(sc_ack_send_task, "sc_ack_send_task", SC_ACK_TASK_STACK_SIZE, ack, SC_ACK_TASK_PRIORITY, NULL);
}

void sc_ack_send_stop(void)
{
    s_sc_ack_send = false;
}
