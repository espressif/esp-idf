

/*
udp_esp2ap test
This example use esp32 as station,
when esp32 start,it will connect to an AP and will create a UDP socket,
use a UDP client send a message first to let it know the client IP and port,
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
#include "esp_task_wdt.h"
#include "nvs_flash.h"
#include <sys/socket.h>  
#include <netinet/in.h>  
#include "driver/uart.h"
#include "soc/uart_struct.h"

/*AP info and tcp_sever info*/
//#define DEFAULTSSID "wifi-12"
//#define DEFAULTPWD "sumof1+1=2"
#define DEFAULTSSID CONFIG_WIFI_SSID
#define DEFAULTPWD CONFIG_WIFI_PASSWORD
#define DEFAULTPORT CONFIG_SEVER_PORT
#define BUFFSIZE CONFIG_BUFF_SIZE

static int totle_data=0;

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/**/
static int connectedflag = 0;
static int mysocket;



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
	printf("task data_count start!\n");
	int len;
	char databuff[BUFFSIZE];
	struct sockaddr_in client_addr;
	unsigned int socklen = sizeof(client_addr);
	len=recvfrom(mysocket, databuff, BUFFSIZE, 0,(struct sockaddr *)&client_addr,&socklen);
	if(len<0)
	{
		perror("first recv error:");
		close(mysocket);
		vTaskDelete(NULL);
	}
	else
	{
#ifdef CONFIG_MODE_TCP_SEND
		printf("send data to %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		memset(databuff,97,BUFFSIZE);
#else
		printf("recv data from %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
#endif

	}
	socklen=sizeof(client_addr);
	printf("start calc!\n");

	while(1)
  	{
#ifdef CONFIG_MODE_UDP_SEND
  		len=sendto(mysocket, databuff, BUFFSIZE, 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
#else
  		len=recvfrom(mysocket, databuff, BUFFSIZE, 0,(struct sockaddr *)&client_addr,&socklen);
#endif


		//printf("len= %d\n",len);
		if(len>0)
		{
			totle_data+=len;
		}
		else
		{
			//perror("data_count:\n");
			/*you'd better turn off watch dog in menuconfig
			 *Component config->ESP32-specific->Task watchdog.
			 **/
			//vTaskDelay(1/portTICK_RATE_MS);
		}
	}

}
//this task establish a udp connection and send/recv data
static void udp_connect(void *pvParameters)
{
	TaskHandle_t tasksend;
	do
	{
		vTaskDelay(100);
	}
	while(!connectedflag);
	//wating for connecting to AP


	mysocket = socket(AF_INET, SOCK_DGRAM, 0);
	if(mysocket < 0)
	{
		perror("socket failed:");
		vTaskDelete(NULL);
	}
	struct sockaddr_in myaddr;
	myaddr.sin_family=AF_INET;
	myaddr.sin_port=htons(DEFAULTPORT);
	if(bind(mysocket,(struct sockaddr *)&myaddr,sizeof(myaddr))<0)
	{
		perror("bind local port error:");
		close(mysocket);
		vTaskDelete(NULL);
	}

	vTaskDelay(2000/portTICK_RATE_MS);

	//create a new task...
	xTaskCreate(&data_count,"data_count",4096,NULL,5,&tasksend);
	int pps;
	while(1)
	{ 
		totle_data=0;
		vTaskDelay(3000/ portTICK_RATE_MS);
		pps = totle_data/3;
#ifdef CONFIG_MODE_UDP_SEND
		printf("udp send %d byte per sec!(just reference)\n",pps);
#else
  		printf("udp recv %d byte per sec!\n",pps);
#endif
	}
	vTaskDelete(NULL);
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    xTaskCreate(&udp_connect,"udp_connect",4096,NULL,5,NULL);
}
