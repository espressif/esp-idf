

/*
udp_perf example

Using this example to test udp throughput performance.
esp<->esp or esp<->ap

step1:
    init wifi as AP/STA using config SSID/PASSWORD.

step2:
    creat a udp sever/client socket using config PORT/(IP).
    if sever: wating for the first message of client.
    if client: sending a packet to sever first.

step3:
    send/receive data to/from each other.
    you can see the info in com port output.
*/

#include "udp_perf.h"

int connectedflag = 0;
int totle_data = 0;
int success_pack = 0;

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
    int socret;
    
#if ESP_UDP_MODE_SEVER
    vTaskDelay(3000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "creat_udp_sever.");
    socret=creat_udp_sever();
    //vTaskDelay(1000/portTICK_RATE_MS);
#else /*ESP_UDP_MODE_SEVER*/
    vTaskDelay(20000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "creat_udp_client.");
    socret = creat_udp_client();
#endif
    if(-1 == socret) {
	ESP_LOGI(TAG, "creat udp socket error,stop.");
	vTaskDelete(NULL);
    }
    
    /*create a task to tx/rx data*/
    TaskHandle_t tx_rx_task;
    xTaskCreate(&send_recv_data, "send_recv_data", 4096, NULL, 4, &tx_rx_task);

    int pps;
    while (1) {
	totle_data = 0;
	vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
	pps = totle_data / 3;

#if ESP_UDP_PERF_TX
	ESP_LOGI(TAG, "udp send %d byte per sec! totle pack: %d \n", pps, success_pack);
#else
	ESP_LOGI(TAG, "udp recv %d byte per sec! totle pack: %d \n", pps, success_pack);
#endif /*ESP_UDP_PERF_TX*/
    }
    close_socket();
    vTaskDelete(tx_rx_task);
    vTaskDelete(NULL);
}



void app_main(void)
{
    nvs_flash_init();
#if ESP_WIFI_MODE_AP
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP\n");
    wifi_init_softap();
#else /*ESP_WIFI_MODE_AP*/
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA\n");
    wifi_init_sta();
#endif
    xTaskCreate(&udp_conn, "udp_conn", 4096, NULL, 5, NULL);
}
