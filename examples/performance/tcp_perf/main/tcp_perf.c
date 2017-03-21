
#include "tcp_perf.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;
/*socket*/
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
    	//vTaskDelay(1000/portTICK_RATE_MS);
    	totle_pack++;
    	gettimeofday(&tv_start, NULL);
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
    	
	//send function
    	len = send(connect_soc, databuff, DEFAULT_PKTSIZE, 0);

#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
    	gettimeofday(&tv_finish, NULL);
#endif /*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/
	if(len > 0) {
	    totle_data += len;
	    
#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
	    send_success++;
	    send_delay_ms = (tv_finish.tv_sec - tv_start.tv_sec) * 1000
		+ (tv_finish.tv_usec - tv_start.tv_usec) / 1000;
	    //ESP_LOGI(TAG, "send_delay_ms=%ld",send_delay_ms);
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
	    
	    /*for faster sending,don't show error code
	     *if it can't work as expectations,unnote the two lines here
	     **/
	    //perror("data_count error");
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
	len = recv(connect_soc, databuff, DEFAULT_PKTSIZE, 0);
	if (len > 0) {
	    totle_data += len;
	}
	else {
	    perror("recv_data error");
	    vTaskDelay(500 / portTICK_RATE_MS);
	}
    }
}


//use this esp32 as a tcp sever. return 0:success -1:error
int creat_tcp_sever()
{
    ESP_LOGI(TAG, "sever socket....port=%d\n", DEFAULT_PORT);
    sever_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (sever_socket < 0) {
	perror("socket() error:");
	return -1;
    }
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(DEFAULT_PORT);
    sever_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sever_socket, (struct sockaddr*)&sever_addr, sizeof(sever_addr)) < 0) {
	perror("bind() error");
	close(sever_socket);
	return -1;
    }
    if (listen(sever_socket, 5) < 0) {
	perror("listen() error");
	close(sever_socket);
	return -1;
    }
    connect_soc = accept(sever_socket, (struct sockaddr*)&client_addr, &socklen);
    if (connect_soc<0) {
	perror("accept() error");
	close(sever_socket);
	return -1;
    }
    /*connection established，now can send/recv*/
    ESP_LOGI(TAG, "tcp connection established!");
    return 0;
}
//use this esp32 as a tcp client. return 0:success -1:error
int creat_tcp_client()
{
    ESP_LOGI(TAG, "client socket....severip:port=%s:%d\n", 
    		DEFAULT_SEVER_IP, DEFAULT_PORT);
    connect_soc = socket(AF_INET, SOCK_STREAM, 0);
    if (connect_soc < 0) {
	perror("socket failed!");
	return -1;
    }
    sever_addr.sin_family = AF_INET;
    sever_addr.sin_port = htons(DEFAULT_PORT);
    sever_addr.sin_addr.s_addr = inet_addr(DEFAULT_SEVER_IP);
    ESP_LOGI(TAG, "connecting to sever...");
    if (connect(connect_soc, (struct sockaddr *)&sever_addr, sizeof(sever_addr)) < 0) {
	perror("connect to sever error!");
	return -1;
    }
    ESP_LOGI(TAG, "connect to sever success!");
    return 0;
}

//wifi_init_sta
void wifi_init_sta()
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
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
    wifi_event_group = xEventGroupCreate();
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

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s \n",
    	    DEFAULT_SSID, DEFAULT_PWD);
}

void close_socket()
{
    close(connect_soc);
    close(sever_socket);
}



