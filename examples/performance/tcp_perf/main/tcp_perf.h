#ifndef __TCP_PERF_H__
#define __TCP_PERF_H__



#ifdef __cplusplus
extern "C" {
#endif

/*AP info and tcp_server info*/
#define DEFAULT_SSID CONFIG_TCP_PERF_WIFI_SSID
#define DEFAULT_PWD CONFIG_TCP_PERF_WIFI_PASSWORD
#define DEFAULT_PORT CONFIG_TCP_PERF_SERVER_PORT
#define DEFAULT_SERVER_IP CONFIG_TCP_PERF_SERVER_IP
#define DEFAULT_PKTSIZE CONFIG_TCP_PERF_PKT_SIZE
#define MAX_STA_CONN 1 //how many sta can be connected(AP mode)
/*test options*/
#define ESP_WIFI_MODE_AP CONFIG_TCP_PERF_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define ESP_TCP_MODE_SERVER CONFIG_TCP_PERF_SERVER //TRUE:server FALSE:client
#define ESP_TCP_PERF_TX CONFIG_TCP_PERF_TX //TRUE:send FALSE:receive
#define ESP_TCP_DELAY_INFO CONFIG_TCP_PERF_DELAY_DEBUG //TRUE:show delay time info


#define PACK_BYTE_IS 97 //'a'
#define TAG "tcp_perf:"


extern int connectedflag;
extern int total_data;

#if ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO
extern int total_pack;
extern int send_success;
extern int send_fail;
extern int delay_classify[5];
#endif/*ESP_TCP_PERF_TX && ESP_TCP_DELAY_INFO*/


//using esp as station
void wifi_init_sta();
//using esp as softap
void wifi_init_softap();

//create a tcp server socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_server();
//create a tcp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_tcp_client();

//send data task
void send_data(void *pvParameters);
//receive data task
void recv_data(void *pvParameters);

//close all socket
void close_socket();

//show socket error code. return: error code
int show_socket_error_code(int socket);

//check working socket
int check_socket_error_code();


#ifdef __cplusplus
}
#endif


#endif /*#ifndef __TCP_PERF_H__*/

