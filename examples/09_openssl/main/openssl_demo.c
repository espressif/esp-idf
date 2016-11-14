#include <stddef.h>
#include <string.h>

#include "openssl_demo.h"
#include "openssl/ssl.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_types.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "lwip/sockets.h"
#include "lwip/api.h"

#include "nvs_flash.h"


#define os_printf(fmt, ...) ESP_LOGI("openssl_demo", fmt, ##__VA_ARGS__)

#define IP_ADDR(ip) ip.u_addr.ip4.addr

/* The examples use simple WiFi configuration that you can set via
   'make menuconfig'.

   If you'd rather not, just change the below entries to strings with
   the config you want - ie #define EXAMPLE_WIFI_SSID "mywifissid"
*/
#define EXAMPLE_WIFI_SSID CONFIG_WIFI_SSID
#define EXAMPLE_WIFI_PASS CONFIG_WIFI_PASSWORD

#define OPENSSL_DEMO_THREAD_STACK_WORDS 8192
#define OPENSSL_DEMO_THREAD_PRORIOTY 6

#define OPENSSL_DEMO_FRAGMENT_SIZE 8192

#define OPENSSL_DEMO_RECV_BUF_LEN 1024

#define OPENSSL_DEMO_LOCAL_TCP_PORT 443

#ifdef CONFIG_OPENSSL_CLIENT_DEMO

#define OPENSSL_DEMO_THREAD_NAME "ssl_client"

#define OPENSSL_DEMO_TARGET_NAME "www.baidu.com"
#define OPENSSL_DEMO_TARGET_TCP_PORT 443

#define OPENSSL_DEMO_REQUEST "{\"path\": \"/v1/ping/\", \"method\": \"GET\"}\r\n"

LOCAL void openssl_demo_thread(void *p)
{
    int ret;

    SSL_CTX *ctx;
    SSL *ssl;

    int socket;
    struct sockaddr_in sock_addr;

    ip_addr_t target_ip;

    int recv_bytes = 0;

    LOCAL char send_data[] = OPENSSL_DEMO_REQUEST;
    LOCAL int send_bytes = sizeof(send_data);
    
    LOCAL char recv_buf[OPENSSL_DEMO_RECV_BUF_LEN];

    os_printf("OpenSSL demo thread start\n");

    do {
        ret = netconn_gethostbyname(OPENSSL_DEMO_TARGET_NAME, &target_ip);
    } while(ret);
    os_printf("get target IP is %d.%d.%d.%d\n",
        (unsigned char)((IP_ADDR(target_ip) & 0x000000ff) >> 0),
        (unsigned char)((IP_ADDR(target_ip) & 0x0000ff00) >> 8),
        (unsigned char)((IP_ADDR(target_ip) & 0x00ff0000) >> 16),
        (unsigned char)((IP_ADDR(target_ip) & 0xff000000) >> 24));

    os_printf("create SSL context ......");
    ctx = SSL_CTX_new(TLSv1_1_client_method());
    if (!ctx) {
        os_printf("failed\n");
        goto failed1;
    }
    os_printf("OK\n");

    /**
     * The openssl does not support "SSL_CTX_set_default_read_buffer_len"
     * at the platform of ESP32 esp_idf now.
     * 
     * So you should not care it now. And We my let it work later.
     */
    os_printf("set SSL context read buffer size ......");
    SSL_CTX_set_default_read_buffer_len(ctx, OPENSSL_DEMO_FRAGMENT_SIZE);
    ret = 0;
    if (ret) {
        os_printf("failed, return %d\n", ret);
        goto failed2;
    }
    os_printf("OK\n");

    os_printf("create socket ......");
    socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        os_printf("failed\n");
        goto failed3;
    }
    os_printf("OK\n");

    os_printf("bind socket ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_DEMO_LOCAL_TCP_PORT);
    ret = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        os_printf("failed\n");
        goto failed4;
    }
    os_printf("OK\n");

    os_printf("socket connect to remote %s ......", OPENSSL_DEMO_TARGET_NAME);
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = IP_ADDR(target_ip);
    sock_addr.sin_port = htons(OPENSSL_DEMO_TARGET_TCP_PORT);
    ret = connect(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        os_printf("failed\n");
        goto failed5;
    }
    os_printf("OK\n");

    os_printf("create SSL ......");
    ssl = SSL_new(ctx);
    if (!ssl) {
        os_printf("failed\n");
        goto failed6;
    }
    os_printf("OK\n");

    SSL_set_fd(ssl, socket);

    os_printf("SSL connected to %s port %d ......",
        OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);
    ret = SSL_connect(ssl);
    if (!ret) {
        os_printf("failed, return [-0x%x]\n", -ret);
        goto failed7;
    }
    os_printf("OK\n");

    os_printf("send https request to %s port %d ......",
        OPENSSL_DEMO_TARGET_NAME, OPENSSL_DEMO_TARGET_TCP_PORT);
    ret = SSL_write(ssl, send_data, send_bytes);
    if (ret <= 0) {
        os_printf("failed\n");
        goto failed8;
    }
    os_printf("OK\n\n");

    do {
        ret = SSL_read(ssl, recv_buf, OPENSSL_DEMO_RECV_BUF_LEN - 1);
        if (ret <= 0) {
            break;
        }
        recv_bytes += ret;
        os_printf("%s", recv_buf);
    } while (1);
    
    os_printf("\r\ntotaly read %d bytes data from %s ......\n", recv_bytes, OPENSSL_DEMO_TARGET_NAME);

failed8:
    SSL_shutdown(ssl);
failed7:
    SSL_free(ssl);
    ssl = NULL;
failed6:
failed5:
failed4:
    close(socket);
    socket = -1;
failed3:
failed2:
    SSL_CTX_free(ctx);
    ctx = NULL;
failed1:
    vTaskDelete(NULL);

    os_printf("task exit\n");

    return ;
}

#elif defined(CONFIG_OPENSSL_SERVER_DEMO)

#define OPENSSL_DEMO_THREAD_NAME "openssl_server"

#define OPENSSL_DEMO_CLIENT_REQUEST "{\"path\": \"/v1/ping/\", \"method\": \"GET\"}\r\n"

#define OPENSSL_DEMO_SERVER_ACK "HTTP/1.1 200 OK\r\n" \
                                "Content-Type: text/html\r\n" \
                                "Content-Length: 98\r\n" \
                                "<html>\r\n" \
                                "<head>\r\n" \
                                "<title>openSSL demo</title></head><body>\r\n" \
                                "OpenSSL server demo!\r\n" \
                                "</body>\r\n" \
                                "</html>\r\n"

LOCAL void openssl_demo_thread(void *p)
{
    int ret;

    SSL_CTX *ctx;
    SSL *ssl;

    int socket, new_socket;
    socklen_t addr_len;
    struct sockaddr_in sock_addr;

    LOCAL char send_data[] = OPENSSL_DEMO_SERVER_ACK;
    LOCAL int send_bytes = sizeof(send_data);

    LOCAL char recv_buf[OPENSSL_DEMO_RECV_BUF_LEN];

    os_printf("server SSL context create ......");
    ctx = SSL_CTX_new(SSLv3_server_method());
    if (!ctx) {
        os_printf("failed\n");
        goto failed1;
    }
    os_printf("OK\n");

    /**
     * The openssl does not support "SSL_CTX_set_default_read_buffer_len"
     * at the platform of ESP32 esp_idf now.
     * 
     * So you should not care it now. And We my let it work later.
     */
    os_printf("server SSL context set fragment ......");
    SSL_CTX_set_default_read_buffer_len(ctx, OPENSSL_DEMO_FRAGMENT_SIZE);
    ret = 0;
    if (ret) {
        os_printf("failed, return %d\n", ret);
        goto failed2;
    }
    os_printf("OK\n");

    os_printf("server SSL context set own certification......");
    ret = SSL_CTX_use_certificate_ASN1(ctx, cert_bytes, cert_ctx);
    if (!ret) {
        os_printf("failed, return %d\n", ret);
        goto failed2;
    }
    os_printf("OK\n");

    os_printf("server SSL context set private key......");
    ret = SSL_CTX_use_PrivateKey_ASN1(0, ctx, key_ctx, key_bytes);
    if (!ret) {
        os_printf("failed, return %d\n", ret);
        goto failed2;
    }
    os_printf("OK\n");

    os_printf("server create socket ......");
    socket = socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) {
        os_printf("failed\n");
        goto failed2;
    }
    os_printf("OK\n");

    os_printf("server socket bind ......");
    memset(&sock_addr, 0, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = 0;
    sock_addr.sin_port = htons(OPENSSL_DEMO_LOCAL_TCP_PORT);
    ret = bind(socket, (struct sockaddr*)&sock_addr, sizeof(sock_addr));
    if (ret) {
        os_printf("failed\n");
        goto failed3;
    }
    os_printf("OK\n");

    os_printf("server socket listen ......");
    ret = listen(socket, 32);
    if (ret) {
        os_printf("failed\n");
        goto failed3;
    }
    os_printf("OK\n");

reconnect:
    os_printf("server SSL create ......");
    ssl = SSL_new(ctx);
    if (!ssl) {
        os_printf("failed\n");
        goto failed3;
    }
    os_printf("OK\n");

    os_printf("server socket accept client ......");
    new_socket = accept(socket, (struct sockaddr *)&sock_addr, &addr_len);
    if (new_socket < 0) {
        os_printf("failed, return [-0x%x]\n", -new_socket);
        goto failed4;
    }
    os_printf("OK\n");

    SSL_set_fd(ssl, new_socket);

    os_printf("server SSL accept client ......");
    ret = SSL_accept(ssl);
    if (!ret) {
        os_printf("failed\n");
        goto failed5;
    }
    os_printf("OK\n");

    os_printf("server SSL read message ......");
    do {
        memset(recv_buf, 0, OPENSSL_DEMO_RECV_BUF_LEN);
        ret = SSL_read(ssl, recv_buf, OPENSSL_DEMO_RECV_BUF_LEN - 1);
        if (ret <= 0) {
            break;
        }
        if (strstr(recv_buf, "GET / HTTP/1.1")) {
            SSL_write(ssl, send_data, send_bytes);
            break;
        }
    } while (1);
    
    os_printf("result %d\n", ret);

    SSL_shutdown(ssl);
failed5:
    close(new_socket);
    new_socket = -1;
failed4:
    SSL_free(ssl);
    ssl = NULL;
    goto reconnect;
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

#else
    #error "you must choose the right demo type"
#endif

LOCAL void demo_init(void)
{
    int ret = pdFALSE;

#if defined(CONFIG_OPENSSL_CLIENT_DEMO) || defined(CONFIG_OPENSSL_SERVER_DEMO)
    xTaskHandle openssl_handle;

    ret = xTaskCreate(openssl_demo_thread,
                      OPENSSL_DEMO_THREAD_NAME,
                      OPENSSL_DEMO_THREAD_STACK_WORDS,
                      NULL,
                      OPENSSL_DEMO_THREAD_PRORIOTY,
                      &openssl_handle);
#endif

    if (ret != pdPASS)  {
        os_printf("create thread %s failed\n", OPENSSL_DEMO_THREAD_NAME);
        return ;
    }
}

LOCAL esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        demo_init();
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        break;
    default:
        break;
    }
    return ESP_OK;
}

LOCAL void wifi_conn_init(void)
{
    tcpip_adapter_init();
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
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    os_printf("start the WIFI SSID:[%s] password:[%s]\n", EXAMPLE_WIFI_SSID, EXAMPLE_WIFI_PASS);
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void app_main(void)
{
    nvs_flash_init();
    wifi_conn_init();
}

