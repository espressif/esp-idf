
/*
TCP_recv example
This example use esp32 as station and TCP sever,
when esp32 start,it will connect to an AP and will create a TCP socket as a sever,
use a TCP client connect to esp32,
then it will send/receive data,(set work mode in menuconfig)
And calculate the speed of sending/receiving data.
*/
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

/*AP info and tcp_sever info*/
#define DEFAULTSSID CONFIG_WIFI_SSID
#define DEFAULTPWD CONFIG_WIFI_PASSWORD
#define DEFAULTPORT CONFIG_SEVER_PORT
#define BUFFSIZE CONFIG_BUFF_SIZE

static int totle_data=0;

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/**/
static int connectedflag = 0;
static int sever_socket;
static struct sockaddr_in sever_addr;
static struct sockaddr_in client_addr;
static unsigned int socklen = sizeof(client_addr);
static int connect_soc;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        //printf("event_handler:SYSTEM_EVENT_STA_START\n");
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        //printf("event_handler:SYSTEM_EVENT_STA_DISCONNECTED\n");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        //printf("event_handler:SYSTEM_EVENT_STA_CONNECTED!\n");
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        printf("event_handler:SYSTEM_EVENT_STA_GOT_IP!\n");
		printf("ip:%s\n",ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
        connectedflag=1;
        break;
    default:
        break;
    }
    return ESP_OK;
}
//wifi_init
static void wifi_init()
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = DEFAULTSSID,
            .password = DEFAULTPWD
        },
    };

    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    esp_wifi_connect();
    
    printf("wifi_init over.\n");
}

static void data_count(void *pvParameters)
{
  	int len=0;
  	char databuff[BUFFSIZE];
#ifdef CONFIG_MODE_TCP_SEND
  	memset(databuff,97,BUFFSIZE);
#endif
	while(1)
  	{
#ifdef CONFIG_MODE_TCP_SEND
  		len=send(connect_soc, databuff, BUFFSIZE, 0);
#else
  		len=recv(connect_soc, databuff, BUFFSIZE, 0);
#endif
  		if(len>0)
  		{
			totle_data+=len;
 		}
		else
		{
			/*for faster send&receive,don't show error code.
			 *if it can't work as expectations,unnote the two lines here
			 **/
			//perror("data_count error");
			//vTaskDelay(500/portTICK_RATE_MS);
		}
  	} 
}

//this task establish a TCP connection and receive data from TCP
static void tcp_client(void *pvParameters)
{
  	do
  	{
		vTaskDelay(100);
  	}
  	while(!connectedflag);
  	//wating for connecting to AP 
	
	printf("socket....port=%d\n",DEFAULTPORT);
	sever_socket = socket(AF_INET, SOCK_STREAM, 0);
  	if(sever_socket	< 0)  
  	{
		perror("socket() error:");
		vTaskDelete(NULL);
  	}
	sever_addr.sin_family = AF_INET;
	sever_addr.sin_port = htons(DEFAULTPORT);
	sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(sever_socket,(struct sockaddr*)&sever_addr,sizeof(sever_addr))<0)
	{
		perror("bind() error");
		close(sever_socket);
		vTaskDelete(NULL);
	}
	if(listen(sever_socket,5)<0)
	{
		perror("listen() error");
		close(sever_socket);
		vTaskDelete(NULL);
	}
	connect_soc = accept(sever_socket,(struct sockaddr*)&client_addr,&socklen);
	if(connect_soc<0)
	{
		perror("accept() error");
		close(sever_socket);
		vTaskDelete(NULL);
	}
	/*connection established，now can send/recv*/
	printf("connection established!");
  	TaskHandle_t tasksend;
  	xTaskCreate(&data_count,"data_count",4096,NULL,5,&tasksend);

	int pps;
  	while(1)
  	{
		totle_data=0;
		//calc every 3s 
		vTaskDelay(3000/ portTICK_RATE_MS);
		pps=totle_data/3;
#ifdef CONFIG_MODE_TCP_SEND
		printf("tcp send %d byte per sec!\n",pps);
#else
  		printf("tcp recv %d byte per sec!\n",pps);
#endif
  	} 
	vTaskDelete(tasksend);
	close(connect_soc);
	close(sever_socket);
	vTaskDelete(NULL);
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    xTaskCreate(&tcp_client,"tcp_client",4096,NULL,5,NULL);
}
