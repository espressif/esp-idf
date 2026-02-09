/* Simple HTTP + SSL Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_eth.h"
#include "protocol_examples_common.h"

#include <esp_https_server.h>
#include "esp_tls.h"
#include "sdkconfig.h"

#if CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_CUSTOM_CIPHERSUITES
#include "mbedtls/ssl_ciphersuites.h"
#endif // CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_CUSTOM_CIPHERSUITES

/* A simple example that demonstrates how to create GET and POST
 * handlers and start an HTTPS server.
*/

static const char *TAG = "example";

#ifdef CONFIG_ESP_HTTPS_SERVER_EVENTS
/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == ESP_HTTPS_SERVER_EVENT) {
        if (event_id == HTTPS_SERVER_EVENT_ERROR) {
            esp_https_server_last_error_t *last_error = (esp_tls_last_error_t *) event_data;
            ESP_LOGE(TAG, "Error event triggered: last_error = %s, last_tls_err = %d, tls_flag = %d", esp_err_to_name(last_error->last_error), last_error->esp_tls_error_code, last_error->esp_tls_flags);
        }
    }
}
#endif // CONFIG_ESP_HTTPS_SERVER_EVENTS

/* An HTTP GET handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "<h1>Hello Secure World!</h1>", HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

#if CONFIG_EXAMPLE_ENABLE_HTTPS_USER_CALLBACK
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
static void print_peer_cert_info(const mbedtls_ssl_context *ssl)
{
    const mbedtls_x509_crt *cert;
    const size_t buf_size = 1024;
    char *buf = calloc(buf_size, sizeof(char));
    if (buf == NULL) {
        ESP_LOGE(TAG, "Out of memory - Callback execution failed!");
        return;
    }

    // Logging the peer certificate info
    cert = mbedtls_ssl_get_peer_cert(ssl);
    if (cert != NULL) {
        mbedtls_x509_crt_info((char *) buf, buf_size - 1, "    ", cert);
        ESP_LOGI(TAG, "Peer certificate info:\n%s", buf);
    } else {
        ESP_LOGW(TAG, "Could not obtain the peer certificate!");
    }

    free(buf);
}
#endif
/**
 * Example callback function to get the certificate of connected clients,
 * whenever a new SSL connection is created and closed
 *
 * Can also be used to other information like Socket FD, Connection state, etc.
 *
 * NOTE: This callback will not be able to obtain the client certificate if the
 * following config `Set minimum Certificate Verification mode to Optional` is
 * not enabled (enabled by default in this example).
 *
 * The config option is found here - Component config → ESP-TLS
 *
 */
static void https_server_user_callback(esp_https_server_user_cb_arg_t *user_cb)
{
    ESP_LOGI(TAG, "User callback invoked!");
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
    mbedtls_ssl_context *ssl_ctx = NULL;
#endif
    switch(user_cb->user_cb_state) {
        case HTTPD_SSL_USER_CB_SESS_CREATE:
            ESP_LOGD(TAG, "At session creation");

            // Logging the socket FD
            int sockfd = -1;
            esp_err_t esp_ret;
            esp_ret = esp_tls_get_conn_sockfd(user_cb->tls, &sockfd);
            if (esp_ret != ESP_OK) {
                ESP_LOGE(TAG, "Error in obtaining the sockfd from tls context");
                break;
            }
            ESP_LOGI(TAG, "Socket FD: %d", sockfd);
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
            ssl_ctx = (mbedtls_ssl_context *) esp_tls_get_ssl_context(user_cb->tls);
            if (ssl_ctx == NULL) {
                ESP_LOGE(TAG, "Error in obtaining ssl context");
                break;
            }
            // Logging the current ciphersuite
            ESP_LOGI(TAG, "Current Ciphersuite: %s", mbedtls_ssl_get_ciphersuite(ssl_ctx));
#endif
            break;

        case HTTPD_SSL_USER_CB_SESS_CLOSE:
            ESP_LOGD(TAG, "At session close");
#ifdef CONFIG_ESP_TLS_USING_MBEDTLS
            // Logging the peer certificate
            ssl_ctx = (mbedtls_ssl_context *) esp_tls_get_ssl_context(user_cb->tls);
            if (ssl_ctx == NULL) {
                ESP_LOGE(TAG, "Error in obtaining ssl context");
                break;
            }
            print_peer_cert_info(ssl_ctx);
#endif
            break;

        case HTTPD_SSL_USER_CB_SESS_ERROR:
            ESP_LOGD(TAG, "At session error (handshake failed)");
            // Get socket FD to log failing client IP address
            sockfd = -1;
            esp_ret = esp_tls_get_conn_sockfd(user_cb->tls, &sockfd);
            if (esp_ret == ESP_OK && sockfd >= 0) {
                struct sockaddr_storage addr;
                socklen_t len = sizeof(addr);
                if (getpeername(sockfd, (struct sockaddr*)&addr, &len) == 0) {
                    char ip_str[INET6_ADDRSTRLEN];
                    if (addr.ss_family == AF_INET) {
                        inet_ntop(AF_INET, &((struct sockaddr_in*)&addr)->sin_addr, ip_str, sizeof(ip_str));
                    } else if (addr.ss_family == AF_INET6) {
                        inet_ntop(AF_INET6, &((struct sockaddr_in6*)&addr)->sin6_addr, ip_str, sizeof(ip_str));
                    } else {
                        strcpy(ip_str, "unknown");
                    }
                    ESP_LOGW(TAG, "SSL handshake failed from client IP: %s", ip_str);
                }
            }
            break;

        default:
            ESP_LOGE(TAG, "Illegal state!");
            return;
    }
}
#endif

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");

    extern const unsigned char cacert_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_end[]   asm("_binary_cacert_pem_end");
    conf.servercert = servercert_start;
    conf.servercert_len = servercert_end - servercert_start;

#if CONFIG_EXAMPLE_ENABLE_SKIP_CLIENT_CERT
    conf.client_cert_authmode_optional = true;
#endif // EXAMPLE_ENABLE_SKIP_CLIENT_CERT

    conf.cacert_pem = cacert_start;
    conf.cacert_len = cacert_end - cacert_start;

    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

#if CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_CUSTOM_CIPHERSUITES
    static const int ciphersuites_to_use[] = {
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
        MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
        MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,
        MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
        0,
    };
    conf.ciphersuites_list = ciphersuites_to_use;
#else
    conf.ciphersuites_list = NULL;
#endif // CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_CUSTOM_CIPHERSUITES

#if CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_TLS_1_3_ONLY
    conf.tls_version = ESP_TLS_VER_TLS_1_3;
#elif CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_TLS_1_2_ONLY
    conf.tls_version = ESP_TLS_VER_TLS_1_2;
#else
    conf.tls_version = ESP_TLS_VER_ANY;
#endif // CONFIG_EXAMPLE_ENABLE_HTTPS_SERVER_TLS_1_3_ONLY

#if CONFIG_EXAMPLE_ENABLE_HTTPS_USER_CALLBACK
    conf.user_cb = https_server_user_callback;
#endif
    esp_err_t ret = httpd_ssl_start(&server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
        return NULL;
    }

    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    return server;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    return httpd_ssl_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop https server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        *server = start_webserver();
    }
}

void app_main(void)
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Register event handlers to start server when Wi-Fi or Ethernet is connected,
     * and stop server when disconnection happens.
     */

#ifdef CONFIG_EXAMPLE_CONNECT_WIFI
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_WIFI
#ifdef CONFIG_EXAMPLE_CONNECT_ETHERNET
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(ETH_EVENT, ETHERNET_EVENT_DISCONNECTED, &disconnect_handler, &server));
#endif // CONFIG_EXAMPLE_CONNECT_ETHERNET
#ifdef CONFIG_ESP_HTTPS_SERVER_EVENTS
    ESP_ERROR_CHECK(esp_event_handler_register(ESP_HTTPS_SERVER_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
#endif // CONFIG_ESP_HTTPS_SERVER_EVENTS

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
}
