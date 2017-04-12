

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
#include "esp_log.h"
#include "esp_err.h"

#include "tcp_perf.h"



//this task establish a TCP connection and receive data from TCP
static void tcp_conn(void *pvParameters)
{
    ESP_LOGI(TAG, "task tcp_conn start.");
    /*wating for connecting to AP*/
    do
    {
	vTaskDelay(100);
    }
    while (!connectedflag);
    ESP_LOGI(TAG, "sta has connected to ap.");
    
    /*create tcp socket*/
    int socket_ret;
    
#if ESP_TCP_MODE_SERVER
    vTaskDelay(3000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_tcp_server.");
    socket_ret=create_tcp_server();
#else /*ESP_TCP_MODE_SERVER*/
    vTaskDelay(20000 / portTICK_RATE_MS);
    ESP_LOGI(TAG, "create_tcp_client.");
    socket_ret = create_tcp_client();
#endif
    if(ESP_FAIL == socket_ret) {
	ESP_LOGI(TAG, "create tcp socket error,stop.");
	vTaskDelete(NULL);
    }
    
    /*create a task to tx/rx data*/
    TaskHandle_t tx_rx_task;
#if ESP_TCP_PERF_TX
    xTaskCreate(&send_data, "send_data", 4096, NULL, 4, &tx_rx_task);
#else /*ESP_TCP_PERF_TX*/
    xTaskCreate(&recv_data, "recv_data", 4096, NULL, 4, &tx_rx_task);
#endif
    int pps;
    while (1) {
	total_data = 0;
	vTaskDelay(3000 / portTICK_RATE_MS);//every 3s
	pps = total_data / 3;
	if (total_data <= 0) {
	    int err_ret = check_socket_error_code();
	    if (err_ret == ECONNRESET) {
		ESP_LOGI(TAG, "disconnected... stop.");
		break;
	    }
	}

#if ESP_TCP_PERF_TX
	ESP_LOGI(TAG, "tcp send %d byte per sec!", pps);
#if ESP_TCP_DELAY_INFO
	ESP_LOGI(TAG, "tcp send packet total:%d  succeed:%d  failed:%d\n"
		"time(ms):0-30:%d 30-100:%d 100-300:%d 300-1000:%d 1000+:%d\n",
		total_pack, send_success, send_fail, delay_classify[0],
		delay_classify[1], delay_classify[2], delay_classify[3], delay_classify[4]);
#endif /*ESP_TCP_DELAY_INFO*/
#else
	ESP_LOGI(TAG, "tcp recv %d byte per sec!\n", pps);
#endif /*ESP_TCP_PERF_TX*/
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
    xTaskCreate(&tcp_conn, "tcp_conn", 4096, NULL, 5, NULL);
}
