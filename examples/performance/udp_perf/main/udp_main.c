

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


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "udp_perf.h"


//this task establish a UDP connection and receive data from UDP
static void udp_conn(void *pvParameters)
{
    ESP_LOGI(TAG, "task udp_conn start.");
    /*wating for connecting to AP*/
    do
    {
	vTaskDelay(100);
    }
    while (!connectedflag);
    ESP_LOGI(TAG, "sta has connected to ap.");
    
    /*create udp socket*/
    int socket_ret;
    
#if ESP_UDP_MODE_SERVER
    vTaskDelay(3000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_udp_server.");
    socket_ret=create_udp_server();
#else /*ESP_UDP_MODE_SERVER*/
    vTaskDelay(20000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_udp_client.");
    socket_ret = create_udp_client();
#endif
    if(ESP_FAIL == socket_ret) {
	ESP_LOGI(TAG, "create udp socket error,stop.");
	vTaskDelete(NULL);
    }
    
    /*create a task to tx/rx data*/
    TaskHandle_t tx_rx_task;
    xTaskCreate(&send_recv_data, "send_recv_data", 4096, NULL, 4, &tx_rx_task);

    int pps;
    while (1) {
	total_data = 0;
	vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
	pps = total_data / 3;

#if ESP_UDP_PERF_TX
	ESP_LOGI(TAG, "udp send %d byte per sec! total pack: %d \n", pps, success_pack);
#else
	ESP_LOGI(TAG, "udp recv %d byte per sec! total pack: %d \n", pps, success_pack);
#endif /*ESP_UDP_PERF_TX*/
    }
    close_socket();
    vTaskDelete(tx_rx_task);
    vTaskDelete(NULL);
}



void app_main(void)
{
#if ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP\n");
    wifi_init_softap();
#else /*ESP_WIFI_MODE_AP*/
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA\n");
    wifi_init_sta();
#endif
    xTaskCreate(&udp_conn, "udp_conn", 4096, NULL, 5, NULL);
}
