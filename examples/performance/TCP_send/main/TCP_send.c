

/*
TCP_send example
this example will creat a TCP connect,
and send data constantly.
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
//#define DEFAULTSSID "wifi-12"
//#define DEFAULTPWD "sumof1+1=2"
#define DEFAULTSSID "tp_wifi_test1"
#define DEFAULTPWD "1234567890"
#define DEFAULTPORT 4567
#define DEFAULTSEVER "192.168.0.102"
#define BUFFSIZE 1024

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/**/
static int connectedflag = 0;
static int clientsocket;
static struct sockaddr_in serverAddr;


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


//this task establish a tcp connection and send data
static void tcp_client(void *pvParameters)
{
  do
  {
    vTaskDelay(100);
  }
  while(!connectedflag);
  //wating for connecting to AP 

  clientsocket = socket(AF_INET, SOCK_STREAM, 0);
  if(clientsocket < 0)  
  {
    printf("socket failed!\n");
    vTaskDelete(NULL);
  }
  serverAddr.sin_family = AF_INET;  
  serverAddr.sin_port = htons(DEFAULTPORT);  
  serverAddr.sin_addr.s_addr = inet_addr(DEFAULTSEVER);

  printf("connecting to sever...\n");
  if(connect(clientsocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)  
  {  
    printf("connect to sever error!\n"); 
    vTaskDelete(NULL);
  }

  printf("connect succeed!\n");
  
  //create a new task...
  //send(clientsocket, "hello", 6, 0);
  
  vTaskDelay(1000/portTICK_RATE_MS);
  char buffsend[BUFFSIZE];
  memset(buffsend,97,BUFFSIZE);//'a'=97
  //send as much as it can
  while(1)
  {
    send(clientsocket, buffsend, BUFFSIZE, 0);
    //vTaskDelay(3000/portTICK_RATE_MS);
  }
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    xTaskCreate(&tcp_client,"tcp_client",4096,NULL,5,NULL);
}