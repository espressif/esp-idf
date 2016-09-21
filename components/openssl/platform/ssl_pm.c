#include "ssl_pm.h"
#include "ssl_dbg.h"

#include <string.h>

/* mbedtls include */
#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

struct ssl_pm
{
    /* local socket file description */
    mbedtls_net_context fd;
    /* remote client socket file description */
    mbedtls_net_context cl_fd;

    mbedtls_ssl_config conf;

    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_ssl_context ssl;

    mbedtls_entropy_context entropy;
};

struct x509_pm
{
    mbedtls_x509_crt x509_crt;
};

struct pkey_pm
{
    mbedtls_pk_context pkey;
};


unsigned int max_content_len;


/*********************************************************************************************/
/********************************* SSL general interface *************************************/

void* ssl_zalloc(size_t size)
{
    void *p = malloc(size);

    if (p)
        memset(p, 0, size);

    return p;
}

void *ssl_malloc(size_t size)
{
    return ssl_zalloc(size);
}

void ssl_free(void *p)
{
    free(p);
}

void* ssl_memcpy(void *to, const void *from, size_t size)
{
    return memcpy(to, from, size);
}

void ssl_speed_up_enter(void)
{

}

void ssl_speed_up_exit(void)
{

}

/*********************************************************************************************/
/************************************ SSL arch interface *************************************/

int ssl_pm_new(SSL *ssl)
{
    struct ssl_pm *ssl_pm;
    int ret;

    char *pers;
    int endpoint;

    SSL_CTX *ctx = ssl->ctx;
    const SSL_METHOD *method = ssl->method;

    ssl_pm = malloc(sizeof(struct ssl_pm));
    if (!ssl_pm)
        return -1;

    if (method->endpoint) {
        pers = "server";
        endpoint = MBEDTLS_SSL_IS_SERVER;
    } else {
        pers = "client";
        endpoint = MBEDTLS_SSL_IS_CLIENT;
    }

    //max_content_len = 4096;

    mbedtls_net_init(&ssl_pm->fd);
    mbedtls_net_init(&ssl_pm->cl_fd);

    mbedtls_ssl_config_init(&ssl_pm->conf);
    mbedtls_ctr_drbg_init(&ssl_pm->ctr_drbg);
    mbedtls_entropy_init(&ssl_pm->entropy);
    mbedtls_ssl_init(&ssl_pm->ssl);

    ret = mbedtls_ctr_drbg_seed(&ssl_pm->ctr_drbg, mbedtls_entropy_func, &ssl_pm->entropy, (const unsigned char *)pers, strlen(pers));
    if (ret)
        SSL_ERR(ret, failed1, "mbedtls_ctr_drbg_seed:[-0x%x]\n", -ret);

    ret = mbedtls_ssl_config_defaults(&ssl_pm->conf, endpoint, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret)
        SSL_ERR(ret, failed2, "mbedtls_ssl_config_defaults:[-0x%x]\n", -ret);

    mbedtls_ssl_conf_rng(&ssl_pm->conf, mbedtls_ctr_drbg_random, &ssl_pm->ctr_drbg);
    mbedtls_ssl_conf_dbg(&ssl_pm->conf, NULL, NULL);

    if (ctx->client_CA->x509_pm) {
        struct x509_pm *x509_pm = (struct x509_pm *)ctx->client_CA->x509_pm;

         mbedtls_ssl_conf_ca_chain(&ssl_pm->conf, &x509_pm->x509_crt, NULL);
         mbedtls_ssl_conf_authmode(&ssl_pm->conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    } else {
        mbedtls_ssl_conf_authmode(&ssl_pm->conf, MBEDTLS_SSL_VERIFY_NONE);
    }
    if (ctx->cert->x509 &&
        ctx->cert->pkey) {
        struct x509_pm *x509_pm = (struct x509_pm *)ctx->cert->x509->x509_pm;
        struct pkey_pm *pkey_pm = (struct pkey_pm *)ctx->cert->pkey->pkey_pm;

        ret = mbedtls_ssl_conf_own_cert(&ssl_pm->conf, &x509_pm->x509_crt, &pkey_pm->pkey);
        if (ret)
            SSL_ERR(ret, failed3, "mbedtls_ssl_conf_own_cert:[%d]\n", ret);
    }

    ret = mbedtls_ssl_setup(&ssl_pm->ssl, &ssl_pm->conf);
    if (ret)
        SSL_ERR(ret, failed4, "mbedtls_ssl_setup:[-0x%x]\n", -ret);

    mbedtls_ssl_set_bio(&ssl_pm->ssl, &ssl_pm->fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    ssl->ssl_pm = ssl_pm;

    return 0;

failed4:
    mbedtls_ssl_config_free(&ssl_pm->conf);
failed3:
    mbedtls_ctr_drbg_free(&ssl_pm->ctr_drbg);
failed2:
    mbedtls_entropy_free(&ssl_pm->entropy);
failed1:
    return -1;
}

void ssl_pm_free(SSL *ssl)
{
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbedtls_ssl_config_free(&ssl_pm->conf);
    mbedtls_ctr_drbg_free(&ssl_pm->ctr_drbg);
    mbedtls_entropy_free(&ssl_pm->entropy);
    mbedtls_ssl_free(&ssl_pm->ssl);

    mbedtls_net_free(&ssl_pm->fd);
    mbedtls_net_free(&ssl_pm->cl_fd);
}

int ssl_pm_handshake(SSL *ssl)
{
    int ret, mbed_ret;
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    ssl_speed_up_enter();
    while((mbed_ret = mbedtls_ssl_handshake(&ssl_pm->ssl)) != 0) {
        if (mbed_ret != MBEDTLS_ERR_SSL_WANT_READ && mbed_ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
           break;
        }
    }
    ssl_speed_up_exit();

    if (!mbed_ret)
        ret = 1;
    else {
        ret = 0;
        SSL_DEBUG(1, "mbedtls_ssl_handshake [-0x%x]\n", -mbed_ret);
    }

    return ret;
}

int ssl_pm_shutdown(SSL *ssl)
{
    int ret, mbed_ret;
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbed_ret = mbedtls_ssl_close_notify(&ssl_pm->ssl);
    if (!mbed_ret)
        ret = 0;
    else
        ret = -1;

    return ret;
}

int ssl_pm_clear(SSL *ssl)
{
    return ssl_pm_shutdown(ssl);
}


int ssl_pm_read(SSL *ssl, void *buffer, int len)
{
    int ret, mbed_ret;
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbed_ret = mbedtls_ssl_read(&ssl_pm->ssl, buffer, len);
    if (mbed_ret < 0)
        ret = -1;
    else if (mbed_ret == 0)
        ret = 0;
    else
        ret = mbed_ret;

    return ret;
}

int ssl_pm_send(SSL *ssl, const void *buffer, int len)
{
    int ret, mbed_ret;
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbed_ret = mbedtls_ssl_write(&ssl_pm->ssl, buffer, len);
    if (mbed_ret < 0)
        ret = -1;
    else if (mbed_ret == 0)
        ret = 0;
    else
        ret = mbed_ret;

    return ret;
}

int ssl_pm_pending(const SSL *ssl)
{
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    return mbedtls_ssl_get_bytes_avail(&ssl_pm->ssl);
}

void ssl_pm_set_fd(SSL *ssl, int fd, int mode)
{
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    ssl_pm->fd.fd = fd;
}

int ssl_pm_get_fd(const SSL *ssl, int mode)
{
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    return ssl_pm->fd.fd;
}

OSSL_HANDSHAKE_STATE ssl_pm_get_state(const SSL *ssl)
{
    OSSL_HANDSHAKE_STATE state;

    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    switch (ssl_pm->ssl.state)
    {
        case MBEDTLS_SSL_CLIENT_HELLO:
            state = TLS_ST_CW_CLNT_HELLO;
            break;
        case MBEDTLS_SSL_SERVER_HELLO:
            state = TLS_ST_SW_SRVR_HELLO;
            break;
        case MBEDTLS_SSL_SERVER_CERTIFICATE:
            state = TLS_ST_SW_CERT;
            break;
        case MBEDTLS_SSL_SERVER_HELLO_DONE:
            state = TLS_ST_SW_SRVR_DONE;
            break;
        case MBEDTLS_SSL_CLIENT_KEY_EXCHANGE:
            state = TLS_ST_CW_KEY_EXCH;
            break;
        case MBEDTLS_SSL_CLIENT_CHANGE_CIPHER_SPEC:
            state = TLS_ST_CW_CHANGE;
            break;
        case MBEDTLS_SSL_CLIENT_FINISHED:
            state = TLS_ST_CW_FINISHED;
            break;
        case MBEDTLS_SSL_SERVER_CHANGE_CIPHER_SPEC:
            state = TLS_ST_SW_CHANGE;
            break;
        case MBEDTLS_SSL_SERVER_FINISHED:
            state = TLS_ST_SW_FINISHED;
            break;
        case MBEDTLS_SSL_CLIENT_CERTIFICATE:
            state = TLS_ST_CW_CERT;
            break;
        case MBEDTLS_SSL_SERVER_KEY_EXCHANGE:
            state = TLS_ST_SR_KEY_EXCH;
            break;
        case MBEDTLS_SSL_SERVER_NEW_SESSION_TICKET:
            state = TLS_ST_SW_SESSION_TICKET;
            break;
        case MBEDTLS_SSL_SERVER_HELLO_VERIFY_REQUEST_SENT:
            state = TLS_ST_SW_CERT_REQ;
            break;
        case MBEDTLS_SSL_HANDSHAKE_OVER:
            state = TLS_ST_OK;
            break;
        default :
            state = TLS_ST_BEFORE;
            break;
    }

    return state;
}

void* x509_pm_new(void)
{
    return ssl_malloc(sizeof(struct x509_pm));
}

void x509_pm_free(void *pm)
{
    ssl_free(pm);
}

int x509_pm_load_crt(void *pm, const unsigned char *buffer, int len)
{
    int ret;
    unsigned char *load_buf;
    struct x509_pm *x509_pm = (struct x509_pm *)pm;

    load_buf = ssl_malloc(len + 1);
    if (!load_buf)
        SSL_RET(failed1, "");

    ssl_memcpy(load_buf, buffer, len);
    load_buf[len] = '\0';

    mbedtls_x509_crt_init(&x509_pm->x509_crt);
    ret = mbedtls_x509_crt_parse(&x509_pm->x509_crt, load_buf, len);
    ssl_free(load_buf);

    if (ret)
        SSL_RET(failed1, "");

    return 0;

failed1:
    return -1;
}

void x509_pm_unload_crt(void *pm)
{
    struct x509_pm *x509_pm = (struct x509_pm *)pm;

    mbedtls_x509_crt_free(&x509_pm->x509_crt);
}

void* pkey_pm_new(void)
{
    return ssl_malloc(sizeof(struct pkey_pm));
}

void pkey_pm_free(void *pm)
{
    ssl_free(pm);
}

int pkey_pm_load_crt(void *pm, const unsigned char *buffer, int len)
{
    int ret;
    unsigned char *load_buf;
    struct pkey_pm *pkey_pm = (struct pkey_pm *)pm;

    load_buf = ssl_malloc(len + 1);
    if (!load_buf)
        SSL_RET(failed1, "");

    ssl_memcpy(load_buf, buffer, len);
    load_buf[len] = '\0';

    mbedtls_pk_init(&pkey_pm->pkey);
    ret = mbedtls_pk_parse_key(&pkey_pm->pkey, load_buf, len, NULL, 0);
    ssl_free(load_buf);

    if (ret)
        SSL_RET(failed1, "");

    return 0;

failed1:
    return -1;
}

void pkey_pm_unload_crt(void *pm)
{
    struct pkey_pm *pkey_pm = (struct pkey_pm *)pm;

    mbedtls_pk_free(&pkey_pm->pkey);
}

void ssl_pm_set_bufflen(SSL *ssl, int len)
{
    max_content_len = len;
}
