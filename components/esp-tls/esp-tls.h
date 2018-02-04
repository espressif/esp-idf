#ifndef _ESP_TLS_H_
#define _ESP_TLS_H_

#include <stdbool.h>
#include <sys/socket.h>
#include <openssl/ssl.h>

#ifdef __cplusplus
extern "C" {
#endif

struct esp_tls_cfg {
    /* If HTTP2/ALPN support is required, a list of protocols that
     * should be negotiated. The format is length followed by protocol
     * name.
     * For the most common cases the following is ok:
     * "\x02h2"
     * - where the first '2' is the length of the protocol and
     * - the subsequent 'h2' is the protocol name
     */
    const unsigned char *alpn_protos;
};

struct esp_tls {
    SSL_CTX  *ctx;
    SSL      *ssl;
    int       sockfd;
    ssize_t (*read)(struct esp_tls *tls, char *data, size_t datalen);
    ssize_t (*write)(struct esp_tls *tls, const char *data, size_t datalen);
};

/*
 *
 * cfg: If you wish to open non-TLS connection, keep this NULL. For TLS
 * connection, a pass pointer to 'struct esp_tls_cfg'. At a minimum, this
 * structure should be zero-initialized.
 */
struct esp_tls *esp_tls_conn_new(const char *hostname, int hostlen, int port, struct esp_tls_cfg *cfg);

/* Convenience API for HTTP URIs */
struct esp_tls *esp_tls_conn_http_new(const char *url, struct esp_tls_cfg *cfg);
    
static inline ssize_t esp_tls_conn_write(struct esp_tls *tls, const char *data, size_t datalen)
{
    return tls->write(tls, data, datalen);
}

static inline ssize_t esp_tls_conn_read(struct esp_tls *tls, char *data, size_t datalen)
{
    return tls->read(tls, data, datalen);
}

void esp_tls_conn_delete(struct esp_tls *tls);

#ifdef __cplusplus
}
#endif

#endif /* ! _ESP_TLS_H_ */
