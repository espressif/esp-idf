/* udp_perf Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/




/*
udp_perf example

Using this example to test udp throughput performance.
esp<->esp or esp<->ap

step1:
    init wifi as AP/STA using config SSID/PASSWORD.

step2:
    create a udp server/client socket using config PORT/(IP).
    if server: wating for the first message of client.
    if client: sending a packet to server first.

step3:
    send/receive data to/from each other.
    you can see the info in serial output.
*/


#include <errno.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"

#include "udp_perf.h"


//this task establish a UDP connection and receive data from UDP
static void udp_conn(void *pvParameters)
{
    ESP_LOGI(TAG, "task udp_conn start.");
    /*wating for connecting to AP*/
    xEventGroupWaitBits(udp_event_group, WIFI_CONNECTED_BIT,false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "sta has connected to ap.");
    
    /*create udp socket*/
    int socket_ret;
    
#if EXAMPLE_ESP_UDP_MODE_SERVER
    ESP_LOGI(TAG, "create udp server after 3s...");
    vTaskDelay(3000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_udp_server.");
    socket_ret=create_udp_server();
#else /*EXAMPLE_ESP_UDP_MODE_SERVER*/
    ESP_LOGI(TAG, "create udp client after 20s...");
    vTaskDelay(20000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_udp_client.");
    socket_ret = create_udp_client();
#endif
    if(socket_ret == ESP_FAIL) {
	ESP_LOGI(TAG, "create udp socket error,stop.");
	vTaskDelete(NULL);
    }
    
    /*create a task to tx/rx data*/
    TaskHandle_t tx_rx_task;
    xTaskCreate(&send_recv_data, "send_recv_data", 4096, NULL, 4, &tx_rx_task);

    /*waiting udp connected success*/
    xEventGroupWaitBits(udp_event_group, UDP_CONNCETED_SUCCESS,false, true, portMAX_DELAY);
    int bps;
    while (1) {
	total_data = 0;
	vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
	bps = total_data / 3;

	if (total_data <= 0) {
	    int err_ret = check_connected_socket();
	    if (err_ret == -1) {  //-1 reason: low level netif error
		ESP_LOGW(TAG, "udp send & recv stop.\n");
		break;
	    }
	}

#if EXAMPLE_ESP_UDP_PERF_TX
	ESP_LOGI(TAG, "udp send %d byte per sec! total pack: %d \n", bps, success_pack);
#else
	ESP_LOGI(TAG, "udp recv %d byte per sec! total pack: %d \n", bps, success_pack);
#endif /*EXAMPLE_ESP_UDP_PERF_TX*/
    }
    close_socket();
    vTaskDelete(tx_rx_task);
    vTaskDelete(NULL);
}



void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

#if EXAMPLE_ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "EXAMPLE_ESP_WIFI_MODE_AP");
    wifi_init_softap();
#else /*EXAMPLE_ESP_WIFI_MODE_AP*/
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    wifi_init_sta();
#endif
    xTaskCreate(&udp_conn, "udp_conn", 4096, NULL, 5, NULL);
}
