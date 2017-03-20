#ifndef __TCP_PERF_H__
#define __TCP_PERF_H__

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "esp_log.h"




#ifdef __cplusplus
extern "C" {
#endif

/*AP info and tcp_sever info*/
#define DEFAULTSSID CONFIG_TCP_PERF_WIFI_SSID
#define DEFAULTPWD CONFIG_TCP_PERF_WIFI_PASSWORD
#define DEFAULTPORT CONFIG_TCP_PERF_SEVER_PORT
#define DEFAULTSEVERIP CONFIG_TCP_PERF_SERVER_IP
#define DEFAULT_PKTSIZE CONFIG_TCP_PERF_PKT_SIZE
#define MAXSTACONN 1 //how many sta can be connected(AP mode)
/*test options*/
#define ESP_WIFI_MODE_AP CONFIG_TCP_PERF_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define ESP_TCP_MODE_SEVER CONFIG_TCP_PERF_SEVER //TRUE:sever FALSE:client
#define ESP_TCP_PERF_TX CONFIG_TCP_PERF_TX //TRUE:send FALSE:receive
#define ESP_TCP_DELAY_INFO CONFIG_TCP_PERF_DELAY_DEBUG //TRUE:show delay time info


#define TAG "tcp_perf:"



//using esp as station
void wifi_init_sta();
//using esp as softap
void wifi_init_softap();

//creat a tcp sever socket. return 0:success -1:error
int creat_tcp_sever();
//creat a tcp client socket. return 0:success -1:error
int creat_tcp_client();

//send data task
void send_data(void *pvParameters);
//receive data task
void recv_data(void *pvParameters);

//close all socket
void close_socket();





#ifdef __cplusplus
}
#endif

#endif

