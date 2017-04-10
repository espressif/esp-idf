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


#include "tcp_perf.h"

/*socket*/
static int server_socket = 0;
static struct sockaddr_in server_addr;
static struct sockaddr_in client_addr;
static unsigned int socklen = sizeof(client_addr);
static int connect_socket = 0;

int connectedflag = 0;
int total_data = 0;

#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO

int total_pack = 0;
int send_success = 0;
int send_fail = 0;
int delay_classify[5] = { 0 };

#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/


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
        connectedflag = 1;
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
    	ESP_LOGI(TAG, "station:"MACSTR" join,AID=%d\n",
		MAC2STR(event->event_info.sta_connected.mac),
		event->event_info.sta_connected.aid);
    	connectedflag = 1;
    	break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
    	ESP_LOGI(TAG, "station:"MACSTR"leave,AID=%d\n",
		MAC2STR(event->event_info.sta_disconnected.mac),
		event->event_info.sta_disconnected.aid);
    	connectedflag = 0;
    	break;
    default:
        break;
    }
    return ESP_OK;
}

//send data
void send_data(void *pvParameters)
{
    int len = 0;
    char databuff[DEFAULT_PKTSIZE];
    memset(databuff, PACK_BYTE_IS, DEFAULT_PKTSIZE);
    vTaskDelay(100/portTICK_RATE_MS);
    ESP_LOGI(TAG, "start sending...");
#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
    //delaytime
    struct timeval tv_start;
    struct timeval tv_finish;
    unsigned long send_delay_ms;
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
    while(1) {

#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
    	total_pack++;
    	gettimeofday(&tv_start, NULL);
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
    	
	//send function
    	len = send(connect_socket, databuff, DEFAULT_PKTSIZE, 0);

#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
    	gettimeofday(&tv_finish, NULL);
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	if(len > 0) {
	    total_data += len;
	    
#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	    send_success++;
	    send_delay_ms = (tv_finish.tv_sec - tv_start.tv_sec) * 1000
		+ (tv_finish.tv_usec - tv_start.tv_usec) / 1000;
	    if(send_delay_ms < 30)
		delay_classify[0]++;
	    else if(send_delay_ms < 100)
		delay_classify[1]++;
	    else if(send_delay_ms < 300)
		delay_classify[2]++;
	    else if(send_delay_ms < 1000)
		delay_classify[3]++;
	    else
		delay_classify[4]++;
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	
	}/*if(len > 0)*/
	else {

#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	    send_fail++;
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	    
	    /*Most of the error code will be send window full.
         *So, for faster sending,don't show error code.
	     *if it can't work as expectations,unnote the two lines here.
	     **/
	    //show_socket_error_code(connect_socket);
	    //vTaskDelay(500/portTICK_RATE_MS);
	}
    } 
}
//receive data
void recv_data(void *pvParameters)
{
    int len = 0;
    char databuff[DEFAULT_PKTSIZE];
    while (1) {
	len = recv(connect_socket, databuff, DEFAULT_PKTSIZE, 0);
	if (len > 0) {
	    total_data += len;
	}
	else {
	    show_socket_error_code(connect_socket);
	    vTaskDelay(500 / portTICK_RATE_MS);
	}
    }
}


//use this esp32 as a tcp server. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_server()
{
    ESP_LOGI(TAG, "server socket....port=%d\n", DEFAULT_PORT);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
    	show_socket_error_code(server_socket);
	return ESP_FAIL;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    	show_socket_error_code(server_socket);
	close(server_socket);
	return ESP_FAIL;
    }
    if (listen(server_socket, 5) < 0) {
    	show_socket_error_code(server_socket);
	close(server_socket);
	return ESP_FAIL;
    }
    connect_socket = accept(server_socket, (struct sockaddr*)&client_addr, &socklen);
    if (connect_socket<0) {
    	show_socket_error_code(connect_socket);
	close(server_socket);
	return ESP_FAIL;
    }
    /*connection established，now can send/recv*/
    ESP_LOGI(TAG, "tcp connection established!");
    return ESP_OK;
}
//use this esp32 as a tcp client. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client()
{
    ESP_LOGI(TAG, "client socket....serverip:port=%s:%d\n", 
    		DEFAULT_SERVER_IP, DEFAULT_PORT);
    connect_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_socket < 0) {
    	show_socket_error_code(connect_socket);
	return ESP_FAIL;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEFAULT_PORT);
    server_addr.sin_addr.s_addr = inet_addr(DEFAULT_SERVER_IP);
    ESP_LOGI(TAG, "connecting to server...");
    if (connect(connect_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    	show_socket_error_code(connect_socket);
	return ESP_FAIL;
    }
    ESP_LOGI(TAG, "connect to server success!");
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
            .ssid_len = 0,
            .max_connection=MAX_STA_CONN,
            .password = DEFAULT_PWD,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
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

int check_socket_error_code()
{
    int ret;
#if ESP_TCP_MODE_SERVER
    ESP_LOGI(TAG, "check server_socket");
    ret = show_socket_error_code(server_socket);
    if(ret == ECONNRESET)
	return ret;
#endif
    ESP_LOGI(TAG, "check connect_socket");
    ret = show_socket_error_code(connect_socket);
    if(ret == ECONNRESET)
	return ret;
    return 0;
}

void close_socket()
{
    close(connect_socket);
    close(server_socket);
}


