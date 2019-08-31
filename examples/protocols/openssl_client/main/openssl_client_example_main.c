/* OpenSSL client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "openssl_client_example.h"

#include <string.h>

#include "openssl/ssl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

const static char *TAG = "openssl_example";

static void openssl_example_task(void *p)
{
    int ret;
    SSL_CTX *ctx;
    SSL *ssl;
    int sockfd;
    struct sockaddr_in sock_addr;
    struct hostent *hp;
    struct ip4_addr *ip4_addr;
    
    int recv_bytes = 0;
    char recv_buf[OPENSSL_EXAMPLE_RECV_BUF_LEN];
    
    const char send_data[] = OPENSSL_EXAMPLE_REQUEST;
    const int send_bytes = sizeof(send_data);

    ESP_LOGI(TAG, "OpenSSL demo thread start OK");

    ESP_LOGI(TAG, "get target IP address");
    hp = gethostbyname(OPENSSL_EXAMPLE_TARGET_NAME);
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
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        ESP_LOGI(TAG, "failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "bind socket ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_EXAMPLE_LOCAL_TCP_PORT);
    ret = bind(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        ESP_LOGI(TAG, "failed");
        goto failed3;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "socket connect to remote %s ......", OPENSSL_EXAMPLE_TARGET_NAME);
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = ip4_addr->addr;
    sock_addr.sin_port = htons(OPENSSL_EXAMPLE_TARGET_TCP_PORT);
    ret = connect(sockfd, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
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

    SSL_set_fd(ssl, sockfd);

    ESP_LOGI(TAG, "SSL connected to %s port %d ......",
        OPENSSL_EXAMPLE_TARGET_NAME, OPENSSL_EXAMPLE_TARGET_TCP_PORT);
    ret = SSL_connect(ssl);
    if (!ret) {
        ESP_LOGI(TAG, "failed " );
        goto failed4;
    }
    ESP_LOGI(TAG, "OK");

    ESP_LOGI(TAG, "send https request to %s port %d ......",
        OPENSSL_EXAMPLE_TARGET_NAME, OPENSSL_EXAMPLE_TARGET_TCP_PORT);
    ret = SSL_write(ssl, send_data, send_bytes);
    if (ret <= 0) {
        ESP_LOGI(TAG, "failed");
        goto failed5;
    }
    ESP_LOGI(TAG, "OK");

    do {
        ret = SSL_read(ssl, recv_buf, OPENSSL_EXAMPLE_RECV_BUF_LEN - 1);
        if (ret <= 0) {
            break;
        }
        recv_buf[ret] = '\0';
        recv_bytes += ret;
        ESP_LOGI(TAG, "%s", recv_buf);
    } while (1);
    
    ESP_LOGI(TAG, "totally read %d bytes data from %s ......", recv_bytes, OPENSSL_EXAMPLE_TARGET_NAME);

failed5:
    SSL_shutdown(ssl);
failed4:
    SSL_free(ssl);
    ssl = NULL;
failed3:
    close(sockfd);
    sockfd = -1;
failed2:
    SSL_CTX_free(ctx);
    ctx = NULL;
failed1:
    vTaskDelete(NULL);
    return ;
}

static void openssl_example_client_init(void)
{
    int ret;
    xTaskHandle openssl_handle;

    ret = xTaskCreate(openssl_example_task,
                      OPENSSL_EXAMPLE_TASK_NAME,
                      OPENSSL_EXAMPLE_TASK_STACK_WORDS,
                      NULL,
                      OPENSSL_EXAMPLE_TASK_PRIORITY,
                      &openssl_handle);

    if (ret != pdPASS)  {
        ESP_LOGI(TAG, "create thread %s failed", OPENSSL_EXAMPLE_TASK_NAME);
    }
}

void app_main(void)
{
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_netif_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    openssl_example_client_init();
}
