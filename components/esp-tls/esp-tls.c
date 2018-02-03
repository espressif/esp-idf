#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "esp-tls.h"


static const char *TAG = "esp-tls";

#ifdef ESP_PLATFORM
#include <esp_log.h>
#else
#define ESP_LOGD(TAG, ...) //printf(__VA_ARGS__);
#define ESP_LOGE(TAG, ...) printf(__VA_ARGS__);
#endif

static struct addrinfo *resolve_host_name(const char *host, size_t hostlen, int port)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    char *use_host = (char *)calloc(1, hostlen  + 1);
    if (!use_host) {
        return NULL;
    }
    strncpy(use_host, host, hostlen);
    use_host[hostlen] = '\0';

    char service[6];
    snprintf(service, sizeof(service), "%d", port);

    ESP_LOGD(TAG, "port is :%s: host:%s: strlen %zu\n", service, use_host, hostlen);
    struct addrinfo *res;
    if (getaddrinfo(use_host, service, &hints, &res)) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s:\n", use_host);
        free(use_host);
        return NULL;
    }
    free(use_host);
    return res;
}

static int esp_tcp_connect(const char *host, int hostlen, int port)
{
    struct addrinfo *res = resolve_host_name(host, hostlen, port);
    if (!res) {
        return -1;
    }

    int ret = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (ret < 0) {
        goto err_freeaddr;
    }

    int fd = ret;
    ret = connect(fd, res->ai_addr, res->ai_addrlen);
    if (ret < 0) {
        goto err_freesocket;
    }

    freeaddrinfo(res);
    return fd;

err_freesocket:
    close(fd);
err_freeaddr:
    freeaddrinfo(res);
    return -1;
}

static int create_ssl_handle(struct esp_tls *tls)
{
    int ret;

    SSL_CTX *ssl_ctx = SSL_CTX_new(TLSv1_2_client_method());
    if (!ssl_ctx) {
        return -1;
    }
#ifdef __APPLE__
    SSL_CTX_set_options(ssl_ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_RELEASE_BUFFERS);
#endif
    SSL *ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        SSL_CTX_free(ssl_ctx);
        return -1;
    }

    SSL_set_fd(ssl, tls->sockfd);
    ret = SSL_connect(ssl);
    if (ret < 1) {
        ESP_LOGE(TAG, "SSL handshake failed");
        SSL_free(ssl);
        SSL_CTX_free(ssl_ctx);
        return -1;
    }

    tls->ctx = ssl_ctx;
    tls->ssl = ssl;
    return 0;
}

void esp_tls_conn_delete(struct esp_tls *tls)
{
    if (!tls) {
        return;
    }
    if (tls->ssl) {
        SSL_free(tls->ssl);
    }
    if (tls->ctx) {
        SSL_CTX_free(tls->ctx);
    }
    if (tls->sockfd) {
        close(tls->sockfd);
    }
    free(tls);
};

struct esp_tls *esp_tls_conn_new(const char *hostname, int hostlen, int port, bool is_tls)
{
    int sockfd = esp_tcp_connect(hostname, hostlen, port);
    if (sockfd < 0) {
        return NULL;
    }

    struct esp_tls *tls = (struct esp_tls *)calloc(1, sizeof(struct esp_tls));
    if (!tls) {
        close(sockfd);
        return NULL;
    }
    tls->sockfd = sockfd;

    if (is_tls) {
        if (create_ssl_handle(tls) != 0) {
            esp_tls_conn_delete(tls);
            return NULL;
        }
    }
    return tls;
}

