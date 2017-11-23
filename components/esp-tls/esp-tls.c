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

static struct addrinfo *resolve_host_name(const char *host, size_t hostlen)
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

    ESP_LOGD(TAG, "host:%s: strlen %zu\n", use_host, hostlen);
    struct addrinfo *res;
    if (getaddrinfo(use_host, NULL, &hints, &res)) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s:\n", use_host);
        free(use_host);
        return NULL;
    }
    free(use_host);
    return res;
}

static ssize_t tcp_read(struct esp_tls *tls, char *data, size_t datalen)
{
    return recv(tls->sockfd, data, datalen, 0);
}

static ssize_t tls_read(struct esp_tls *tls, char *data, size_t datalen)
{
    return SSL_read(tls->ssl, data, datalen);
}

static int esp_tcp_connect(const char *host, int hostlen, int port)
{
    struct addrinfo *res = resolve_host_name(host, hostlen);
    if (!res) {
        return -1;
    }

    int ret = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (ret < 0) {
        goto err_freeaddr;
    }
    int fd = ret;

    void *addr_ptr;
    if (res->ai_family == AF_INET) {
	struct sockaddr_in *p = res->ai_addr;
	p->sin_port = htons(port);
	addr_ptr = p;
    } else if (res->ai_family == AF_INET6) {
	struct sockaddr_in6 *p = res->ai_addr;
	p->sin6_port = htons(port);
	p->sin6_family = AF_INET6;
	addr_ptr = p;
    } else {
	/* Unsupported Protocol Family */
	goto err_freesocket;
    }

    ret = connect(fd, addr_ptr, res->ai_addrlen);
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

static ssize_t tcp_write(struct esp_tls *tls, const char *data, size_t datalen)
{
    return send(tls->sockfd, data, datalen, 0);
}

static ssize_t tls_write(struct esp_tls *tls, const char *data, size_t datalen)
{
    return SSL_write(tls->ssl, data, datalen);
}

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
    tls->read = tcp_read;
    tls->write = tcp_write;

    if (is_tls) {
        if (create_ssl_handle(tls) != 0) {
            esp_tls_conn_delete(tls);
            return NULL;
        }
	tls->read = tls_read;
	tls->write = tls_write;
    }
    return tls;
}

