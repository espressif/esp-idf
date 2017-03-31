
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

//creat a udp sever socket. return ESP_OK:success ESP_FAIL:error
int creat_udp_sever()
{
    ESP_LOGI(TAG, "creat_udp_sever()");
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysocket < 0) {
	perror("socket failed:");
	return ESP_FAIL;
    }
    struct sockaddr_in sever_addr;
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(DEFAULT_PORT);
    sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(mysocket, (struct sockaddr *)&sever_addr, sizeof(sever_addr)) < 0) {
	perror("bind local port error:");
	close(mysocket);
	return ESP_FAIL;
    }
    return ESP_OK;
}

//creat a udp client socket. return ESP_OK:success ESP_FAIL:error
int creat_udp_client()
{
    ESP_LOGI(TAG, "creat_udp_client()");
    mysocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (mysocket < 0) {
	perror("socket failed:");
	return ESP_FAIL;
    }
    /*for client remote_addr is also sever_addr*/
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(DEFAULT_PORT);
    remote_addr.sin_addr.s_addr = inet_addr(DEFAULT_SEVER_IP);

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
#if ESP_UDP_MODE_SEVER
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
	perror("first recv&send error:");
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
	//printf("%d\n",len);
	//vTaskDelay(100/portTICK_RATE_MS);
	if (len > 0) {
	    totle_data += len;
	    success_pack++;
	} else {
	    //perror("data_count:\n");
	    /*you'd better turn off watch dog in menuconfig
	     *Component config->ESP32-specific->Task watchdog.
	     **/
	    //vTaskDelay(1/portTICK_RATE_MS);
	}
    }
}


void close_socket()
{
    close(mysocket);
}
