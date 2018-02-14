// Copyright 2017-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <http_parser.h>
#include "esp_tls.h"


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

    char *use_host = strndup(host, hostlen);
    if (!use_host) {
        return NULL;
    }

    ESP_LOGD(TAG, "host:%s: strlen %lu", use_host, (unsigned long)hostlen);
    struct addrinfo *res;
    if (getaddrinfo(use_host, NULL, &hints, &res)) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s:", use_host);
        free(use_host);
        return NULL;
    }
    free(use_host);
    return res;
}

static ssize_t tcp_read(esp_tls_t *tls, char *data, size_t datalen)
{
    return recv(tls->sockfd, data, datalen, 0);
}

static ssize_t tls_read(esp_tls_t *tls, char *data, size_t datalen)
{
    ssize_t ret = SSL_read(tls->ssl, data, datalen);
    if (ret < 0) {
	int err = SSL_get_error(tls->ssl, ret);
	if (err != SSL_ERROR_WANT_WRITE && err != SSL_ERROR_WANT_READ) {
	    ESP_LOGE(TAG, "read error :%d:", ret);
	}
	return -err;
    }
    return ret;
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
	struct sockaddr_in *p = (struct sockaddr_in *)res->ai_addr;
	p->sin_port = htons(port);
	addr_ptr = p;
    } else if (res->ai_family == AF_INET6) {
	struct sockaddr_in6 *p = (struct sockaddr_in6 *)res->ai_addr;
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

static int create_ssl_handle(esp_tls_t *tls, const char *hostname, size_t hostlen, const esp_tls_cfg_t *cfg)
{
    int ret;
    
    const SSL_METHOD *method = cfg->ssl_method!= NULL ? cfg->ssl_method : TLSv1_2_client_method();
    SSL_CTX *ssl_ctx = SSL_CTX_new(method);
    if (!ssl_ctx) {
        return -1;
    }
#ifdef __APPLE__
    SSL_CTX_set_options(ssl_ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_mode(ssl_ctx, SSL_MODE_RELEASE_BUFFERS);
#endif

    if (cfg->cacert_pem_buf != NULL) {
        SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);

        BIO *bio;
        bio = BIO_new(BIO_s_mem());
        BIO_write(bio, cfg->cacert_pem_buf, cfg->cacert_pem_bytes);

        X509 *ca = PEM_read_bio_X509(bio, NULL, 0, NULL);

        if (!ca) {
            ESP_LOGE(TAG, "CA Error");
            X509_free(ca);
            BIO_free(bio);
            SSL_CTX_free(ssl_ctx);
            return -1;            
        }
        ESP_LOGD(TAG, "CA OK");
            
        X509_STORE_add_cert(SSL_CTX_get_cert_store(ssl_ctx), ca);

        X509_free(ca);
        BIO_free(bio);
    }

    if (cfg->alpn_protos) {
	SSL_CTX_set_alpn_protos(ssl_ctx, cfg->alpn_protos, strlen((char *)cfg->alpn_protos));
    }
    SSL *ssl = SSL_new(ssl_ctx);
    if (!ssl) {
        SSL_CTX_free(ssl_ctx);
        return -1;
    }

    char *use_host = strndup(hostname, hostlen);
    if (!use_host) {
	    SSL_CTX_free(ssl_ctx);
	    return -1;
    }
    SSL_set_tlsext_host_name(ssl, use_host);
    free(use_host);

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

/**
 * @brief      Close the TLS connection and free any allocated resources.
 */
void esp_tls_conn_delete(esp_tls_t *tls)
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

static ssize_t tcp_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    return send(tls->sockfd, data, datalen, 0);
}

static ssize_t tls_write(esp_tls_t *tls, const char *data, size_t datalen)
{
    ssize_t ret = SSL_write(tls->ssl, data, datalen);
    if (ret < 0) {
	int err = SSL_get_error(tls->ssl, ret);
	if (err != SSL_ERROR_WANT_WRITE && err != SSL_ERROR_WANT_READ) {
	    ESP_LOGE(TAG, "write error :%d:", ret);
	}
	return -err;
    }
    return ret;
}

/**
 * @brief      Create a new TLS/SSL connection
 */
esp_tls_t *esp_tls_conn_new(const char *hostname, int hostlen, int port, const esp_tls_cfg_t *cfg)
{
    int sockfd = esp_tcp_connect(hostname, hostlen, port);
    if (sockfd < 0) {
        return NULL;
    }

    esp_tls_t *tls = (esp_tls_t *)calloc(1, sizeof(esp_tls_t));
    if (!tls) {
        close(sockfd);
        return NULL;
    }
    tls->sockfd = sockfd;
    tls->read = tcp_read;
    tls->write = tcp_write;

    if (cfg) {
        if (create_ssl_handle(tls, hostname, hostlen, cfg) != 0) {
            esp_tls_conn_delete(tls);
            return NULL;
        }
	tls->read = tls_read;
	tls->write = tls_write;
    }

    if(cfg->non_block == true) {
        int flags = fcntl(tls->sockfd, F_GETFL, 0);
        fcntl(tls->sockfd, F_SETFL, flags | O_NONBLOCK);    
    }

    return tls;
}

static int get_port(const char *url, struct http_parser_url *u)
{
    if (u->field_data[UF_PORT].len) {
        return strtol(&url[u->field_data[UF_PORT].off], NULL, 10);
    } else {
        if (strncmp(&url[u->field_data[UF_SCHEMA].off], "http", u->field_data[UF_SCHEMA].len) == 0) {
            return 80;
        } else if (strncmp(&url[u->field_data[UF_SCHEMA].off], "https", u->field_data[UF_SCHEMA].len) == 0) {
            return 443;
        }
    }
    return 0;
}

/**
 * @brief      Create a new TLS/SSL connection with a given "HTTP" url
 */
esp_tls_t *esp_tls_conn_http_new(const char *url, const esp_tls_cfg_t *cfg)
{
    /* Parse URI */
    struct http_parser_url u;
    http_parser_url_init(&u);
    http_parser_parse_url(url, strlen(url), 0, &u);

    /* Connect to host */
    return esp_tls_conn_new(&url[u.field_data[UF_HOST].off], u.field_data[UF_HOST].len,
			    get_port(url, &u), cfg);
}
