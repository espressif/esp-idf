#ifndef _ESP_TLS_H_
#define _ESP_TLS_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <openssl/ssl.h>

struct esp_tls {
    SSL_CTX *ctx;
    SSL     *ssl;
    int      sockfd;
};

struct esp_tls *esp_tls_conn_new(const char *hostname, int hostlen, int port, bool is_tls);

static inline ssize_t esp_tls_conn_write(struct esp_tls *tls, const char *data, size_t datalen)
{
    if (tls->ssl) {
        return SSL_write(tls->ssl, data, datalen);
    } else {
        return send(tls->sockfd, data, datalen, 0);
    }
}

static inline ssize_t esp_tls_conn_read(struct esp_tls *tls, char *data, size_t datalen)
{
    if (tls->ssl) {
        return SSL_read(tls->ssl, data, datalen);
    } else {
        return recv(tls->sockfd, data, datalen, 0);
    }
}

void esp_tls_conn_delete(struct esp_tls *tls);

#endif /* ! _ESP_TLS_H_ */
