#ifndef __UDP_PERF_H__
#define __UDP_PERF_H__



#ifdef __cplusplus
extern "C" {
#endif

/*AP info and tcp_server info*/
#define DEFAULT_SSID CONFIG_UDP_PERF_WIFI_SSID
#define DEFAULT_PWD CONFIG_UDP_PERF_WIFI_PASSWORD
#define DEFAULT_PORT CONFIG_UDP_PERF_SERVER_PORT
#define DEFAULT_SERVER_IP CONFIG_UDP_PERF_SERVER_IP
#define DEFAULT_PKTSIZE CONFIG_UDP_PERF_PKT_SIZE
#define MAX_STA_CONN 1 //how many sta can be connected(AP mode)
/*test options*/
#define ESP_WIFI_MODE_AP CONFIG_UDP_PERF_WIFI_MODE_AP //TRUE:AP FALSE:STA
#define ESP_UDP_MODE_SERVER CONFIG_UDP_PERF_SERVER //TRUE:server FALSE:client
#define ESP_UDP_PERF_TX CONFIG_UDP_PERF_TX //TRUE:send FALSE:receive
#define PACK_BYTE_IS 97 //'a'

#define TAG "udp_perf:"


extern int connectedflag;
extern int total_data;
extern int success_pack;


//using esp as station
void wifi_init_sta();
//using esp as softap
void wifi_init_softap();

//create a udp server socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_udp_server();
//create a udp client socket. return ESP_OK:success ESP_FAIL:error
esp_err_t create_udp_client();

//send or recv data task
void send_recv_data(void *pvParameters);

//show socket error code. return: error code
int show_socket_error_code(int socket);

//close all socket
void close_socket();





#ifdef __cplusplus
}
#endif


#endif /*#ifndef __UDP_PERF_H__*/

