/* tcp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


/*
tcp_perf example

Using this example to test tcp throughput performance.
esp<->esp or esp<->ap

step1:
    init wifi as AP/STA using config SSID/PASSWORD.

step2:
    create a tcp server/client socket using config PORT/(IP).
    if server: wating for connect.
    if client connect to server.
step3:
    send/receive data to/from each other.
    if the tcp connect established. esp will send or receive data.
    you can see the info in serial output.
*/

#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"

#include "tcp_perf.h"

//this task establish a TCP connection and receive data from TCP
static void tcp_conn(void *pvParameters)
{
    while (1) {

        g_rxtx_need_restart = false;

        ESP_LOGI(TAG, "task tcp_conn.");

        /*wating for connecting to AP*/
        xEventGroupWaitBits(tcp_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);

        ESP_LOGI(TAG, "sta has connected to ap.");

        int socket_ret = ESP_FAIL;

        TaskHandle_t tx_rx_task = NULL;

#if EXAMPLE_ESP_TCP_MODE_SERVER
        if (socket_ret == ESP_FAIL) {
            /*create tcp socket*/
            ESP_LOGI(TAG, "tcp_server will start after 3s...");
            vTaskDelay(3000 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "create_tcp_server.");
            socket_ret = create_tcp_server();
        }
#else /*EXAMPLE_ESP_TCP_MODE_SERVER*/
        if (socket_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "tcp_client will start after 20s...");
            vTaskDelay(20000 / portTICK_RATE_MS);
            ESP_LOGI(TAG, "create_tcp_client.");
            socket_ret = create_tcp_client();
        }
#endif
        if (socket_ret == ESP_FAIL) {
            ESP_LOGI(TAG, "create tcp socket error,stop.");
            continue;
        }

        /*create a task to tx/rx data*/

#if EXAMPLE_ESP_TCP_PERF_TX
        if (tx_rx_task == NULL) {
            if (pdPASS != xTaskCreate(&send_data, "send_data", 4096, NULL, 4, &tx_rx_task)) {
                ESP_LOGE(TAG, "Send task create fail!");
            }
        }
#else /*EXAMPLE_ESP_TCP_PERF_TX*/
        if (tx_rx_task == NULL) {
            if (pdPASS != xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task)) {
                ESP_LOGE(TAG, "Recv task create fail!");
            }
        }
#endif
        double bps;

        while (1) {
            g_total_data = 0;
            vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
            bps = (g_total_data * 8.0 / 3.0) / 1000000.0;

            if (g_rxtx_need_restart) {
                printf("send or receive task encoutner error, need to restart\n");
                break;
            }

#if EXAMPLE_ESP_TCP_PERF_TX
            ESP_LOGI(TAG, "tcp send %.2f Mbits per sec!\n", bps);
#if EXAMPLE_ESP_TCP_DELAY_INFO
            ESP_LOGI(TAG, "tcp send packet total:%d  succeed:%d  failed:%d\n"
                     "time(ms):0-30:%d 30-100:%d 100-300:%d 300-1000:%d 1000+:%d\n",
                     g_total_pack, g_send_success, g_send_fail, g_delay_classify[0],
                     g_delay_classify[1], g_delay_classify[2], g_delay_classify[3], g_delay_classify[4]);
#endif /*EXAMPLE_ESP_TCP_DELAY_INFO*/
#else
            ESP_LOGI(TAG, "tcp recv %.2f Mbits per sec!\n", bps);
#endif /*EXAMPLE_ESP_TCP_PERF_TX*/
        }

        close_socket();
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
#if EXAMPLE_ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "EXAMPLE_ESP_WIFI_MODE_AP");
    wifi_init_softap();
#else
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
#endif /*EXAMPLE_ESP_WIFI_MODE_AP*/
    xTaskCreate(&tcp_conn, "tcp_conn", 4096, NULL, 5, NULL);
}
