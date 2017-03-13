

/*
UDP_Client RECV
This Demo use esp32 as station,
when esp32 start,it will connect to an AP and will create a UDP socket and receive data.
And calculate speed of receiving data.
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
#define DEFAULTLOCALPORT 7654 //bind local port so it can be easy to debug
#define BUFFSIZE 1024
static int sizepersec=0;

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


static void recvdata(void *pvParameters)
{

  int len=0;
  char buffrev[BUFFSIZE];
  unsigned int socklen;
  serverAddr.sin_family = AF_INET;  
  serverAddr.sin_port = htons(DEFAULTPORT);  
  serverAddr.sin_addr.s_addr = inet_addr(DEFAULTSEVER);
  printf("recvdata!\n");
  socklen=sizeof(serverAddr);
  if(sendto(clientsocket, "hello", 6, 0,(struct sockaddr *)&serverAddr,socklen)<0)
  {
    perror("sendto hello:");
  }
  memset(buffrev,0,sizeof(buffrev));
  while(1)
  {
      len=recvfrom(clientsocket, buffrev, BUFFSIZE, 0,(struct sockaddr *)&serverAddr,&socklen);
      //printf("len= %d\n",len);
      if(len>0)
      {
        sizepersec+=len;
        //printf("%s\n",buffrev);
      }
      else if(len<0)
      {
        perror("recvfrom:\n");
        vTaskDelay(100/portTICK_RATE_MS);
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

  clientsocket = socket(AF_INET, SOCK_DGRAM, 0);
  if(clientsocket < 0)  
  {
    printf("socket failed!\n");
    vTaskDelete(NULL);
  }
  struct sockaddr_in myaddr;
  myaddr.sin_family=AF_INET;
  myaddr.sin_port=htons(DEFAULTLOCALPORT);
  if(bind(clientsocket,(struct sockaddr *)&myaddr,sizeof(myaddr))<0)
  {
    printf("bind local port error!\n");
    vTaskDelete(NULL);
  }

  
  vTaskDelay(2000/portTICK_RATE_MS);
  
  //create a new task...
  
  xTaskCreate(&recvdata,"recvdata",4096,NULL,5,&tasksend);

  while(1)
  {
    sizepersec=0;
    vTaskDelay(3000/ portTICK_RATE_MS);
    printf("udp recv %d byte per sec!\n",sizepersec/3);
  }

  vTaskDelete(NULL);
}


void app_main(void)
{
    nvs_flash_init();
    wifi_init();
    xTaskCreate(&udp_connect,"udp_connect",4096,NULL,5,NULL);
}