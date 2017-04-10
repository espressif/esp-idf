// Copyright 2013-2016 Espressif Systems (Shanghai) PTE LTD
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

#include <string.h>
#include <sys/socket.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"


#include "udp_perf.h"



static int mysocket;

static struct sockaddr_in remote_addr;
static unsigned int socklen;

int connectedflag = 0;
int total_data = 0;
int success_pack = 0;


static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
    	ESP_LOGI(TAG, "event_handler:SYSTEM_EVENT_STA_GOT_IP!");
    	ESP_LOGI(TAG, "got ip:%s\n",
		ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        connectedflag=1;
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
    	ESP_LOGI(TAG, "station:"MACSTR" join,AID=%d\n",
		MAC2STR(event->event_info.sta_connected.mac),
		event->event_info.sta_connected.aid);
    	connectedflag=1;
    	break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
    	ESP_LOGI(TAG, "station:"MACSTR"leave,AID=%d\n",
		MAC2STR(event->event_info.sta_disconnected.mac),
		event->event_info.sta_disconnected.aid);
    	break;
    default:
        break;
    }
    return ESP_OK;
}


//wifi_init_sta
void wifi_init_sta()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULT_SSID,
            .password = DEFAULT_PWD
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");
    ESP_LOGI(TAG, "connect to ap SSID:%s password:%s \n",
	    DEFAULT_SSID,DEFAULT_PWD);
}
//wifi_init_softap
void wifi_init_softap()
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = DEFAULT_SSID,
            .ssid_len=0,
            .max_connection=MAX_STA_CONN,
            .password = DEFAULT_PWD,
            .authmode=WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(DEFAULT_PWD) ==0) {
	wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s \n",
    	    DEFAULT_SSID, DEFAULT_PWD);
}

//create a udp server socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_udp_server()
{
    ESP_LOGI(TAG, "create_udp_server()");
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysocket < 0) {
	show_socket_error_code(mysocket);
	return ESP_FAIL;
    }
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(mysocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
	show_socket_error_code(mysocket);
	close(mysocket);
	return ESP_FAIL;
    }
    return ESP_OK;
}

//create a udp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_udp_client()
{
    ESP_LOGI(TAG, "create_udp_client()");
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysocket < 0) {
	show_socket_error_code(mysocket);
	return ESP_FAIL;
    }
    /*for client remote_addr is also server_addr*/
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(DEFAULT_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);

    return ESP_OK;
}


//send or recv data task
void send_recv_data(void *pvParameters)
{
    ESP_LOGI(TAG, "task send_recv_data start!\n");
    
    int len;
    char databuff[DEFAULT_PKTSIZE];
    
    /*send&receive first packet*/
    socklen = sizeof(remote_addr);
    memset(databuff, PACK_BYTE_IS, DEFAULT_PKTSIZE);
#if ESP_UDP_MODE_SERVER
    ESP_LOGI(TAG, "first recvfrom:");
    len = recvfrom(mysocket, databuff, DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, &socklen);
#else
    ESP_LOGI(TAG, "first sendto:");
    len = sendto(mysocket, databuff, DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
#endif
    
    if (len > 0) {
	ESP_LOGI(TAG, "transfer data with %s:%u\n",
		inet_ntoa(remote_addr.sin_addr), ntohs(remote_addr.sin_port));
    } else {
	show_socket_error_code(mysocket);
	close(mysocket);
	vTaskDelete(NULL);
    }
    
#if ESP_UDP_PERF_TX
    vTaskDelay(500 / portTICK_RATE_MS);
#endif
    ESP_LOGI(TAG, "start count!\n");
    while(1)
    {
	/*you can add delay time for fixed-frequency*/
	//vTaskDelay(5 / portTICK_RATE_MS);
#if ESP_UDP_PERF_TX
	len = sendto(mysocket, databuff, DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, sizeof(remote_addr));
#else
	len = recvfrom(mysocket, databuff, DEFAULT_PKTSIZE, 0, (struct sockaddr *)&remote_addr, &socklen);
#endif
	if (len > 0) {
	    total_data += len;
	    success_pack++;
	} else {
	    //show_socket_error_code(mysocket);
	    /*you'd better turn off watch dog in menuconfig
	     *Component config->ESP32-specific->Task watchdog.
	     **/
	    //vTaskDelay(1/portTICK_RATE_MS);
	}
    }
}


char* tcpip_get_reason(int err)
{
    switch (err) {
    case 0:
        return "reason: other reason";
    case ENOMEM:
        return "reason: out of memory";
    case ENOBUFS:
        return "reason: buffer error";
    case EWOULDBLOCK:
        return "reason: timeout, try again";
    case EHOSTUNREACH:
        return "reason: routing problem";
    case EINPROGRESS:
        return "reason: operation in progress";
    case EINVAL:
        return "reason: invalid value";
    case EADDRINUSE:
        return "reason: address in use";
    case EALREADY:
        return "reason: conn already connected";
    case EISCONN:
        return "reason: conn already established";
    case ECONNABORTED:
        return "reason: connection aborted";
    case ECONNRESET:
        return "reason: connection is reset";
    case ENOTCONN:
        return "reason: connection closed";
    case EIO:
        return "reason: invalid argument";
    case -1:
        return "reason: low level netif error";
    default:
        return "reason not found";
    }
}

int show_socket_error_code(int socket)
{
    int result;
    u32_t optlen = sizeof(int);
    getsockopt(socket, SOL_SOCKET, SO_ERROR, &result, &optlen);
    ESP_LOGI(TAG, "socket error %d reason: %s", result, tcpip_get_reason(result));
    return result;
}


void close_socket()
{
    close(mysocket);
}
