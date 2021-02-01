/* multiple network interface Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <sys/socket.h>
#include <netdb.h>
#include "protocol_examples_common.h"

static const char *TAG = "tcp_client_multiple";

#define HOST_NAME CONFIG_EXAMPLE_HOST_NAME
#define HOST_IP_PORT CONFIG_EXAMPLE_HOST_PORT

static const char *payload = "GET / HTTP/1.1\r\n\r\n";

static void app_multiple_handle(esp_ip4_addr_t *ip4_addr, esp_netif_t *esp_netif)
{
    char rx_buffer[128] = {0};
    const char *netif_name = esp_netif_get_desc(esp_netif);

    /* Create a socket */
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sock < 0) {
        ESP_LOGE(TAG, "\"%s\" Unable to create socket: errno %d", netif_name, errno);
        goto app_multiple_handle_fail;
    }
    ESP_LOGI(TAG, "\"%s\" Socket created", netif_name);

    /* Bind the socket to an interface (based on example config option)
     * - using netif local IP address
     * - using netif name
     */
#if CONFIG_EXAMPLE_BIND_SOCKET_TO_NETIF_NAME
    struct ifreq ifr;
#if !CONFIG_LWIP_NETIF_API
    esp_netif_get_netif_impl_name(esp_netif, ifr.ifr_name);
#else
    if_indextoname(esp_netif_get_netif_impl_index(esp_netif), ifr.ifr_name);
#endif
    int ret = setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE,  (void*)&ifr, sizeof(struct ifreq));
    if (ret < 0) {
        ESP_LOGE(TAG, "\"%s\" Unable to bind socket to specified interface: errno %d", netif_name, errno);
        goto app_multiple_handle_fail;
    }
#else
    esp_netif_ip_info_t ip;
    memset(&ip, 0, sizeof(esp_netif_ip_info_t));
    ESP_ERROR_CHECK(esp_netif_get_ip_info(esp_netif, &ip));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(0);
    addr.sin_addr.s_addr = ip.ip.addr;

    int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        ESP_LOGE(TAG, "\"%s\" Unable to bind socket: errno %d", netif_name, errno);
        goto app_multiple_handle_fail;
    }
#endif /* CONFIG_EXAMPLE_BIND_SOCKET_TO_NETIF_NAME */

    /* Connect to the host by the network interface */
    struct sockaddr_in destAddr;
    destAddr.sin_addr.s_addr = ip4_addr->addr;
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(HOST_IP_PORT);
    ret = connect(sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
    if (ret != 0) {
        ESP_LOGE(TAG, "\"%s\" Socket unable to connect: errno %d", netif_name, errno);
        goto app_multiple_handle_fail;
    }
    ESP_LOGI(TAG, "\"%s\" Successfully connected", netif_name);

    ret = send(sock, payload, strlen(payload), 0);
    if (ret < 0) {
        ESP_LOGE(TAG, "\"%s\" Error occured during sending: errno %d", netif_name, errno);
        goto app_multiple_handle_fail;
    }

    ret = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (ret < 0) {
        ESP_LOGE(TAG, "\"%s\" Error occured during receiving: errno %d", netif_name, errno);
    } else if (ret > 0){
        rx_buffer[ret] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TAG, "\"%s\" Received Data %d bytes", netif_name, ret);
        ESP_LOGI(TAG, "%s", rx_buffer);
    } else {
        ESP_LOGE(TAG, "\"%s\" Closed connection during receiving", netif_name);
    }

app_multiple_handle_fail:
    close(sock);
}

static void app_connection_task(void *pvParameters)
{
    esp_ip4_addr_t ip4_addr;
    const char *netif_desc = pvParameters;

    esp_netif_t *netif = get_example_netif_from_desc(netif_desc);
    ESP_LOGD(TAG, "netif described as \"%s\" corresponds to esp-netif ptr:%p", netif_desc, netif);
    while(netif) {
        /* Wait for the host name to get */
        const struct addrinfo hints = {
                .ai_family = AF_INET,
                .ai_socktype = SOCK_STREAM,
        };
        struct addrinfo *res;

        int err = getaddrinfo(HOST_NAME, NULL, &hints, &res);
        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            break;
        }
        memcpy(&ip4_addr, &((struct sockaddr_in *)(res->ai_addr))->sin_addr, sizeof(ip4_addr));
        freeaddrinfo(res);

        /* Connect the host using the corresponding network interface */
        app_multiple_handle(&ip4_addr, netif);

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    ESP_LOGE(TAG, "%s with netif desc:%s Failed! exiting", __func__, netif_desc);
    vTaskDelete(NULL);
}

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_netif_init();

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());

    xTaskCreate(&app_connection_task, "app_ethernet_task", 4096, "eth", 5, NULL);
    xTaskCreate(&app_connection_task, "app_wifi_task", 4096, "sta", 5, NULL);
}
