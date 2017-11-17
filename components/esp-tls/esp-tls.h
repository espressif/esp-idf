#ifndef _ESP_TLS_H_
#define _ESP_TLS_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <openssl/ssl.h>

struct esp_tls {
    SSL_CTX  *ctx;
    SSL      *ssl;
    int       sockfd;
    ssize_t (*read)(struct esp_tls *tls, char *data, size_t datalen);
    ssize_t (*write)(struct esp_tls *tls, const char *data, size_t datalen);
};

struct esp_tls *esp_tls_conn_new(const char *hostname, int hostlen, int port, bool is_tls);

static inline ssize_t esp_tls_conn_write(struct esp_tls *tls, const char *data, size_t datalen)
{
    return tls->write(tls, data, datalen);
}

static inline ssize_t esp_tls_conn_read(struct esp_tls *tls, char *data, size_t datalen)
{
    return tls->read(tls, data, datalen);
}

void esp_tls_conn_delete(struct esp_tls *tls);

#endif /* ! _ESP_TLS_H_ */
