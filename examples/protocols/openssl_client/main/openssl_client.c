/* OpenSSL client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "openssl_client.h"

#include <string.h>

#include "openssl/ssl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "nvs_flash.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const static int CONNECTED_BIT = BIT0;

const static char *TAG = "Openssl_demo";

void openssl_demo_thread(void *p)
{
    int ret;
    SSL_CTX *ctx;
    SSL *ssl;
    int socket;
    struct sockaddr_in sock_addr;
    struct hostent *hp;
    struct ip4_addr *ip4_addr;
    
    int recv_bytes = 0;
    char recv_buf[OPENSSL_DEMO_RECV_BUF_LEN];
    
    const char send_data[] = OPENSSL_DEMO_REQUEST;
    const int send_bytes = sizeof(send_data);

    ESP_LOGI(TAG, "OpenSSL demo thread start OK");

    ESP_LOGI(TAG, "get target IP address");
    hp = gethostbyname(OPENSSL_DEMO_TARGET_NAME);
    if (!hp) {
        ESP_LOGI(TAG, "failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "OK");

    ip4_addr = (struct ip4_addr *)hp->h_addr;
    ESP_LOGI(TAG, IPSTR, IP2STR(ip4_addr));

    ESP_LOGI(TAG, "create SSL context ......");
    ctx = SSL_CTX_new(TLSv1_1_client_method());
    if (!ctx) {
        ESP_LOGI(TAG, "failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "create socket ......");
    socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        ESP_LOGI(TAG, "failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "bind socket ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_DEMO_LOCAL_TCP_PORT);
    ret = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "socket connect to remote %s ......", OPENSSL_DEMO_TARGET_NAME);
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = ip4_addr->addr;
    sock_addr.sin_port = htons(OPENSSL_DEMO_TARGET_TCP_PORT);
    ret = connect(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "create SSL ......");
    ssl = SSL_new(ctx);
    if (!ssl) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");

    SSL_set_fd(ssl, socket);

    ESP_LOGI(TAG, "SSL connected to %s port %d ......",
        OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);
    ret = SSL_connect(ssl);
    if (!ret) {
        ESP_LOGI(TAG, "failed " );
        goto failed4;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "send https request to %s port %d ......",
        OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);
    ret = SSL_write(ssl, send_data, send_bytes);
    if (ret <= 0) {
        ESP_LOGI(TAG, "failed");
        goto failed5;
    }
    ESP_LOGI(TAG, "OK");

    do {
        ret = SSL_read(ssl, recv_buf, OPENSSL_DEMO_RECV_BUF_LEN - 1);
        if (ret <= 0) {
            break;
        }
        recv_bytes += ret;
        ESP_LOGI(TAG, "%s", recv_buf);
    } while (1);
    
    ESP_LOGI(TAG, "totaly read %d bytes data from %s ......", recv_bytes, OPENSSL_DEMO_TARGET_NAME);

failed5:
    SSL_shutdown(ssl);
failed4:
    SSL_free(ssl);
    ssl = NULL;
failed3:
    close(socket);
    socket = -1;
failed2:
    SSL_CTX_free(ctx);
    ctx = NULL;
failed1:
    vTaskDelete(NULL);
    return ;
}

static void openssl_client_init(void)
{
    int ret;
    xTaskHandle openssl_handle;

    ret = xTaskCreate(openssl_demo_thread,
                      OPENSSL_DEMO_THREAD_NAME,
                      OPENSSL_DEMO_THREAD_STACK_WORDS,
                      NULL,
                      OPENSSL_DEMO_THREAD_PRORIOTY,
                      &openssl_handle); 

    if (ret != pdPASS)  {
        ESP_LOGI(TAG, "create thread %s failed", OPENSSL_DEMO_THREAD_NAME);
    }
}

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        openssl_client_init();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect(); 
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

static void wifi_conn_init(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(wifi_event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_WIFI_SSID,
            .password = EXAMPLE_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) );
    ESP_LOGI(TAG, "start the WIFI SSID:[%s] password:[%s]\n", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    wifi_conn_init();
}
