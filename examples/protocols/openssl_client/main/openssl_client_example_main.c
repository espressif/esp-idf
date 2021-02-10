/* OpenSSL Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "openssl_client_example.h"

#include "openssl/ssl.h"

#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_log.h"

#include "protocol_examples_common.h"


static const char *TAG = "openssl_example";

static int open_connection(const char *host, char *port)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo * res;
    struct in_addr *addr;
    int sd;
    int err = getaddrinfo(host, port, &hints, &res);
    if (err < 0) {
        ESP_LOGE(TAG, "getaddrinfo() failed for IPV4 destination address. error: %d", err);
        return -1;
    }
    if (res == 0) {
        ESP_LOGE(TAG, "getaddrinfo() did not return any addresses");
        return -1;
    }
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));
    sd = socket(res->ai_family, res->ai_socktype, 0);
    if(sd < 0) {
        ESP_LOGE(TAG, "Failed to allocate socket.");
        freeaddrinfo(res);
        return -1;
    }
    if (connect(sd, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "Socket connect failed");
        return -1;
    }
    return sd;
}

static SSL_CTX* init_contex(void)
{

#if CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN
    extern const unsigned char cacert_pem_start[] asm("_binary_ca_crt_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_ca_crt_end");
#else
    extern const unsigned char cacert_pem_start[] asm("_binary_baidu_ca_crt_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_baidu_ca_crt_end");
#endif
    const unsigned int cacert_pem_bytes = cacert_pem_end - cacert_pem_start;

    const SSL_METHOD *mtd = TLSv1_1_client_method();
    SSL_CTX *ctx = SSL_CTX_new(mtd);   /* Create new context */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    X509 *x = d2i_X509(NULL, cacert_pem_start, cacert_pem_bytes);
    if(!x) {
        ESP_LOGI(TAG,"Loading certs failed \n");
    }
    SSL_CTX_add_client_CA(ctx, x);

    return ctx;
}

static void start_example(const char *host, char *port)
{
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    int sockfd;
    int ret;

    ctx = init_contex();
    if (!ctx) {
        ESP_LOGE(TAG, "Failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "Trying connect to %s port %s ...", host, port);
    sockfd = open_connection(host, port);
    if(sockfd < 0) {
        ESP_LOGE(TAG,"Failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "OK");
    ESP_LOGI(TAG, "Create SSL obj");
    ssl = SSL_new(ctx);
    if (!ssl) {
        ESP_LOGE(TAG,"Failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");
    SSL_set_fd(ssl, sockfd);
    ret = SSL_connect(ssl);
    if (ret <= 0) {
        ESP_LOGE(TAG,"SSL Connection Failed");
        goto failed3;
    }
    ESP_LOGI(TAG,"SSL Connection Succeed");
failed3:
    SSL_free(ssl);
    ssl = NULL;
failed2:
    close(sockfd);
    sockfd = -1;
failed1:
    SSL_CTX_free(ctx);
    ctx = NULL;
}

#if CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN
static void get_string(char *line, size_t size)
{
    int count = 0;
    while (count < size) {
        int c = fgetc(stdin);
        if (c == '\n') {
            line[count] = '\0';
            break;
        } else if (c > 0 && c < 127) {
            line[count] = c;
            ++count;
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}
#endif /* CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN */

void app_main(void)
{
    char host[128] = EXAMPLE_OPENSSL_TARGET_DOMAIN;
    char port[32] = EXAMPLE_OPENSSL_TARGET_PORT;

    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

#if CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN
    char line[256] = "";
    get_string(line, sizeof(line));
    sscanf(line, "%s %s", host, port);
#endif /* CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN */
    start_example(host, port);
}
