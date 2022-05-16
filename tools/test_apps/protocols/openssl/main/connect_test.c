#define ESP_OPENSSL_SUPPRESS_LEGACY_WARNING

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include "esp_log.h"

static const char *TAG = "OPENSSL_TEST";

static int open_connection(const char *host, const int port)
{
    struct sockaddr_in addr;
    struct hostent *h;
    int sd;
    if ((h = gethostbyname(host)) == NULL) {
        ESP_LOGI(TAG, "Failed to get host name %s", host);
        return -1;
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(h->h_addr);
    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        return -1;
    }
    return sd;
}

static SSL_CTX* init_ctx(const char *test_case)
{
    extern const unsigned char cacert_pem_start[] asm("_binary_ca_crt_start");
    extern const unsigned char cacert_pem_end[]   asm("_binary_ca_crt_end");
    const unsigned int cacert_pem_bytes = cacert_pem_end - cacert_pem_start;
    const SSL_METHOD *method = NULL;
    SSL_CTX *ctx = NULL;
    if (strcmp(test_case, "CONFIG_TLSV1_1_CONNECT_WRONG_CERT_VERIFY_NONE") == 0) {
        method = TLSv1_1_client_method();
        ctx = SSL_CTX_new(method);   /* Create new context */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    } else if (strcmp(test_case, "CONFIG_TLSV1_1_CONNECT_WRONG_CERT_VERIFY_PEER") == 0) {
        method = TLSv1_1_client_method();
        ctx = SSL_CTX_new(method);   /* Create new context */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    } else if (strcmp(test_case, "CONFIG_TLSV1_2_CONNECT_WRONG_CERT_VERIFY_NONE") == 0) {
        method = TLSv1_2_client_method();
        ctx = SSL_CTX_new(method);   /* Create new context */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    } else if (strcmp(test_case, "CONFIG_TLSV1_2_CONNECT_WRONG_CERT_VERIFY_PEER") == 0) {
        method = TLSv1_2_client_method();
        ctx = SSL_CTX_new(method);   /* Create new context */
        SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    }
    X509 *x = d2i_X509(NULL, cacert_pem_start, cacert_pem_bytes);
    if(!x) {
        ESP_LOGI(TAG, "Loading certs failed");
        goto failed;
    }
    SSL_CTX_add_client_CA(ctx, x);
    return ctx;
failed:
    return NULL;
}

static void start_test(const char *host, const int port, const char *test_case)
{
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    int sockfd;
    int ret;

    ESP_LOGI(TAG, "Test %s started", test_case);
    ctx = init_ctx(test_case);
    if (!ctx) {
        ESP_LOGI(TAG, "Failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "Trying connect to %s port %d test case %s ...", host, port, test_case);
    sockfd = open_connection(host, port);
    if(sockfd < 0) {
        ESP_LOGI(TAG,"Failed");
        goto failed1;
    }
    ESP_LOGI(TAG, "OK");
    ESP_LOGI(TAG, "Create SSL obj");
    ssl = SSL_new(ctx);
    if (!ssl) {
        ESP_LOGI(TAG,"Failed");
        goto failed2;
    }
    ESP_LOGI(TAG, "OK");
    SSL_set_fd(ssl, sockfd);
    ESP_LOGI(TAG, "SSL verify mode = %d connected to %s port %d ...", SSL_CTX_get_verify_mode(ctx),
        host, port);
    ret = SSL_connect(ssl);
    ESP_LOGI(TAG, "OK");
    if (ret <= 0) {
        ESP_LOGI(TAG,"SSL Connection Failed");
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

static void scan(char *s, char **test_type, char **host, int *p, char **test_case)
{
    const char *delim = " ";
    *test_type = strtok(s, delim);
    *host = strtok(NULL, delim);
    *p = atoi(strtok(NULL, delim));
    *test_case = strtok(NULL, delim);
}

void connection_test(char *line)
{
    char *test_case;
    char *test_type;
    char *host;
    int port;
    scan(line, &test_type, &host, &port, &test_case);
    start_test(host, port, test_case);
}
