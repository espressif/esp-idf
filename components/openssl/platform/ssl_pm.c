// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
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

#include "ssl_pm.h"
#include "ssl_port.h"
#include "ssl_dbg.h"

/* mbedtls include */
#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#if 0
    #define DEBUG_LOAD_BUF_STRING(str) SSL_DEBUG(1, "%s\n", str)
#else
    #define DEBUG_LOAD_BUF_STRING(str)
#endif

#define X509_INFO_STRING_LENGTH 1024

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
    mbedtls_x509_crt *x509_crt;

    mbedtls_x509_crt *ex_crt;
};

struct pkey_pm
{
    mbedtls_pk_context *pkey;

    mbedtls_pk_context *ex_pkey;
};


unsigned int max_content_len;


/*********************************************************************************************/
/************************************ SSL arch interface *************************************/

/**
 * @brief create SSL low-level object
 */
int ssl_pm_new(SSL *ssl)
{
    struct ssl_pm *ssl_pm;
    int ret;

    const unsigned char pers[] = "OpenSSL PM";
    size_t pers_len = sizeof(pers);

    int endpoint;
    int version;

    const SSL_METHOD *method = ssl->method;

    ssl_pm = ssl_mem_zalloc(sizeof(struct ssl_pm));
    if (!ssl_pm)
        SSL_ERR(ret, failed1, "ssl_mem_zalloc\n");

    max_content_len = ssl->ctx->read_buffer_len;
    
    mbedtls_net_init(&ssl_pm->fd);
    mbedtls_net_init(&ssl_pm->cl_fd);

    mbedtls_ssl_config_init(&ssl_pm->conf);
    mbedtls_ctr_drbg_init(&ssl_pm->ctr_drbg);
    mbedtls_entropy_init(&ssl_pm->entropy);
    mbedtls_ssl_init(&ssl_pm->ssl);

    ret = mbedtls_ctr_drbg_seed(&ssl_pm->ctr_drbg, mbedtls_entropy_func, &ssl_pm->entropy, pers, pers_len);
    if (ret)
        SSL_ERR(ret, failed2, "mbedtls_ctr_drbg_seed:[-0x%x]\n", -ret);

    if (method->endpoint) {
        endpoint = MBEDTLS_SSL_IS_SERVER;
    } else {
        endpoint = MBEDTLS_SSL_IS_CLIENT;
    }
    ret = mbedtls_ssl_config_defaults(&ssl_pm->conf, endpoint, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret)
        SSL_ERR(ret, failed2, "mbedtls_ssl_config_defaults:[-0x%x]\n", -ret);

    if (TLS_ANY_VERSION != ssl->version) {
        if (TLS1_2_VERSION == ssl->version)
            version = MBEDTLS_SSL_MINOR_VERSION_3;
        else if (TLS1_1_VERSION == ssl->version)
            version = MBEDTLS_SSL_MINOR_VERSION_2;
        else if (TLS1_VERSION == ssl->version)
            version = MBEDTLS_SSL_MINOR_VERSION_1;
        else
            version = MBEDTLS_SSL_MINOR_VERSION_0;

        mbedtls_ssl_conf_max_version(&ssl_pm->conf, MBEDTLS_SSL_MAJOR_VERSION_3, version);
        mbedtls_ssl_conf_min_version(&ssl_pm->conf, MBEDTLS_SSL_MAJOR_VERSION_3, version);
    } else {
        mbedtls_ssl_conf_max_version(&ssl_pm->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_3);
        mbedtls_ssl_conf_min_version(&ssl_pm->conf, MBEDTLS_SSL_MAJOR_VERSION_3, MBEDTLS_SSL_MINOR_VERSION_0);
    }

    mbedtls_ssl_conf_rng(&ssl_pm->conf, mbedtls_ctr_drbg_random, &ssl_pm->ctr_drbg);

    mbedtls_ssl_conf_dbg(&ssl_pm->conf, NULL, NULL);

    ret = mbedtls_ssl_setup(&ssl_pm->ssl, &ssl_pm->conf);
    if (ret)
        SSL_ERR(ret, failed3, "mbedtls_ssl_setup:[-0x%x]\n", -ret);

    mbedtls_ssl_set_bio(&ssl_pm->ssl, &ssl_pm->fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    ssl->ssl_pm = ssl_pm;

    return 0;

failed3:
    mbedtls_ssl_config_free(&ssl_pm->conf);
    mbedtls_ctr_drbg_free(&ssl_pm->ctr_drbg);
failed2:
    mbedtls_entropy_free(&ssl_pm->entropy);
    ssl_mem_free(ssl_pm);
failed1:
    return -1;
}

/**
 * @brief free SSL low-level object
 */
void ssl_pm_free(SSL *ssl)
{
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbedtls_ctr_drbg_free(&ssl_pm->ctr_drbg);
    mbedtls_entropy_free(&ssl_pm->entropy);
    mbedtls_ssl_config_free(&ssl_pm->conf);
    mbedtls_ssl_free(&ssl_pm->ssl);

    ssl_mem_free(ssl_pm);
    ssl->ssl_pm = NULL;
}

/**
 * @brief reload SSL low-level certification object
 */
static int ssl_pm_reload_crt(SSL *ssl)
{
    int ret;
    int mode;
    struct ssl_pm *ssl_pm = ssl->ssl_pm;
    struct x509_pm *ca_pm = (struct x509_pm *)ssl->client_CA->x509_pm;

    struct pkey_pm *pkey_pm = (struct pkey_pm *)ssl->cert->pkey->pkey_pm;
    struct x509_pm *crt_pm = (struct x509_pm *)ssl->cert->x509->x509_pm;

    if (ssl->verify_mode == SSL_VERIFY_PEER)
        mode = MBEDTLS_SSL_VERIFY_REQUIRED;
    else if (ssl->verify_mode == SSL_VERIFY_FAIL_IF_NO_PEER_CERT)
        mode = MBEDTLS_SSL_VERIFY_OPTIONAL;
    else if (ssl->verify_mode == SSL_VERIFY_CLIENT_ONCE)
        mode = MBEDTLS_SSL_VERIFY_UNSET;
    else
        mode = MBEDTLS_SSL_VERIFY_NONE;

    mbedtls_ssl_conf_authmode(&ssl_pm->conf, mode);

    if (ca_pm->x509_crt) {
        mbedtls_ssl_conf_ca_chain(&ssl_pm->conf, ca_pm->x509_crt, NULL);
    } else if (ca_pm->ex_crt) {
        mbedtls_ssl_conf_ca_chain(&ssl_pm->conf, ca_pm->ex_crt, NULL);
    }

    if (crt_pm->x509_crt && pkey_pm->pkey) {
        ret = mbedtls_ssl_conf_own_cert(&ssl_pm->conf, crt_pm->x509_crt, pkey_pm->pkey);
    } else if (crt_pm->ex_crt && pkey_pm->ex_pkey) {
        ret = mbedtls_ssl_conf_own_cert(&ssl_pm->conf, crt_pm->ex_crt, pkey_pm->ex_pkey);
    } else {
        ret = 0;
    }

    if (ret)
        return -1;

    return 0;
}

/*
 * Perform the mbedtls SSL handshake instead of mbedtls_ssl_handshake.
 * We can add debug here.
 */
static int mbedtls_handshake( mbedtls_ssl_context *ssl )
{
    int ret = 0;

    if (ssl == NULL || ssl->conf == NULL)
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;

    while (ssl->state != MBEDTLS_SSL_HANDSHAKE_OVER)
    {
        ret = mbedtls_ssl_handshake_step(ssl);
        
        SSL_DEBUG(1, "ssl ret %d state %d heap %d\n", 
            ret, ssl->state, system_get_free_heap_size());
        
        if (ret != 0)
            break;
    }

    return ret;
}

int ssl_pm_handshake(SSL *ssl)
{
    int ret, mbed_ret;
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbed_ret = ssl_pm_reload_crt(ssl);
    if (mbed_ret)
        return 0;

    SSL_DEBUG(1, "ssl_speed_up_enter ");
    ssl_speed_up_enter();
    SSL_DEBUG(1, "OK\n");
    
    while((mbed_ret = mbedtls_handshake(&ssl_pm->ssl)) != 0) {
        if (mbed_ret != MBEDTLS_ERR_SSL_WANT_READ && mbed_ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
           break;
        }
    }
    
    SSL_DEBUG(1, "ssl_speed_up_exit ");
    ssl_speed_up_exit();
    SSL_DEBUG(1, "OK\n");

    if (!mbed_ret) {
        struct x509_pm *x509_pm = (struct x509_pm *)ssl->session->peer->x509_pm;

        ret = 1;

        x509_pm->ex_crt = (mbedtls_x509_crt *)mbedtls_ssl_get_peer_cert(&ssl_pm->ssl);
    } else {
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
    if (!mbed_ret) {
        struct x509_pm *x509_pm = (struct x509_pm *)ssl->session->peer->x509_pm;

        ret = 0;

        x509_pm->ex_crt = NULL;
    }
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

int x509_pm_show_info(X509 *x)
{
    int ret;
    char *buf;
    mbedtls_x509_crt *x509_crt;
    struct x509_pm *x509_pm = x->x509_pm;

    if (x509_pm->x509_crt)
        x509_crt = x509_pm->x509_crt;
    else if (x509_pm->ex_crt)
        x509_crt = x509_pm->ex_crt;
    else
        x509_crt = NULL;

    if (!x509_crt)
        return -1;

    buf = ssl_mem_malloc(X509_INFO_STRING_LENGTH);
    if (!buf)
        SSL_RET(failed1, "");

    ret = mbedtls_x509_crt_info(buf, X509_INFO_STRING_LENGTH - 1, "", x509_crt);
    if (ret <= 0)
        SSL_RET(failed2, "");
    buf[ret] = 0;

    ssl_mem_free(buf);

    SSL_DEBUG(1, "%s", buf);

    return 0;

failed2:
    ssl_mem_free(buf);
failed1:
    return -1;
}

int x509_pm_new(X509 *x, X509 *m_x)
{
    struct x509_pm *x509_pm;

    x509_pm = ssl_mem_zalloc(sizeof(struct x509_pm));
    if (!x509_pm)
        SSL_RET(failed1, "ssl_mem_zalloc\n");

    x->x509_pm = x509_pm;

    if (m_x) {
        struct x509_pm *m_x509_pm = (struct x509_pm *)m_x->x509_pm;

        x509_pm->ex_crt = m_x509_pm->x509_crt;
    }

    return 0;

failed1:
    return -1;
}

void x509_pm_free(X509 *x)
{
    struct x509_pm *x509_pm = (struct x509_pm *)x->x509_pm;

    if (x509_pm->x509_crt) {
        mbedtls_x509_crt_free(x509_pm->x509_crt);

        ssl_mem_free(x509_pm->x509_crt);
        x509_pm->x509_crt = NULL;
    }

    ssl_mem_free(x->x509_pm);
    x->x509_pm = NULL;
}

int x509_pm_load(X509 *x, const unsigned char *buffer, int len)
{
    int ret;
    unsigned char *load_buf;
    struct x509_pm *x509_pm = (struct x509_pm *)x->x509_pm;

	if (x509_pm->x509_crt)
        mbedtls_x509_crt_free(x509_pm->x509_crt);

    if (!x509_pm->x509_crt) {
        x509_pm->x509_crt = ssl_mem_malloc(sizeof(mbedtls_x509_crt));
        if (!x509_pm->x509_crt)
            SSL_RET(failed1, "ssl_mem_malloc\n");
    }

    load_buf = ssl_mem_malloc(len + 1);
    if (!load_buf)
        SSL_RET(failed2, "ssl_mem_malloc\n");

    ssl_memcpy(load_buf, buffer, len);
    load_buf[len] = '\0';

    DEBUG_LOAD_BUF_STRING(load_buf);

    mbedtls_x509_crt_init(x509_pm->x509_crt);

    ret = mbedtls_x509_crt_parse(x509_pm->x509_crt, load_buf, len + 1);
    ssl_mem_free(load_buf);

    if (ret)
        SSL_RET(failed2, "mbedtls_x509_crt_parse, return [-0x%x]\n", -ret);

    return 0;

failed2:
    mbedtls_x509_crt_free(x509_pm->x509_crt);
    ssl_mem_free(x509_pm->x509_crt);
    x509_pm->x509_crt = NULL;
failed1:
    return -1;
}

int pkey_pm_new(EVP_PKEY *pk, EVP_PKEY *m_pkey)
{
    struct pkey_pm *pkey_pm;

    pkey_pm = ssl_mem_zalloc(sizeof(struct pkey_pm));
    if (!pkey_pm)
        return -1;

    pk->pkey_pm = pkey_pm;

    if (m_pkey) {
        struct pkey_pm *m_pkey_pm = (struct pkey_pm *)m_pkey->pkey_pm;

        pkey_pm->ex_pkey = m_pkey_pm->pkey;
    }

    return 0;
}

void pkey_pm_free(EVP_PKEY *pk)
{
    struct pkey_pm *pkey_pm = (struct pkey_pm *)pk->pkey_pm;

    if (pkey_pm->pkey) {
        mbedtls_pk_free(pkey_pm->pkey);

        ssl_mem_free(pkey_pm->pkey);
        pkey_pm->pkey = NULL;
    }

    ssl_mem_free(pk->pkey_pm);
    pk->pkey_pm = NULL;
}

int pkey_pm_load(EVP_PKEY *pk, const unsigned char *buffer, int len)
{
    int ret;
    unsigned char *load_buf;
    struct pkey_pm *pkey_pm = (struct pkey_pm *)pk->pkey_pm;

    if (pkey_pm->pkey)
        mbedtls_pk_free(pkey_pm->pkey);

    if (!pkey_pm->pkey) {
        pkey_pm->pkey = ssl_mem_malloc(sizeof(mbedtls_pk_context));
        if (!pkey_pm->pkey)
            SSL_RET(failed1, "ssl_mem_malloc\n");
    }

    load_buf = ssl_mem_malloc(len + 1);
    if (!load_buf)
        SSL_RET(failed2, "ssl_mem_malloc\n");

    ssl_memcpy(load_buf, buffer, len);
    load_buf[len] = '\0';

    DEBUG_LOAD_BUF_STRING(load_buf);

    mbedtls_pk_init(pkey_pm->pkey);

    ret = mbedtls_pk_parse_key(pkey_pm->pkey, load_buf, len + 1, NULL, 0);
    ssl_mem_free(load_buf);

    if (ret)
        SSL_RET(failed2, "mbedtls_pk_parse_key, return [-0x%x]\n", -ret);

    return 0;

failed2:
    mbedtls_pk_free(pkey_pm->pkey);
    ssl_mem_free(pkey_pm->pkey);
    pkey_pm->pkey = NULL;
failed1:
    return -1;
}



void ssl_pm_set_bufflen(SSL *ssl, int len)
{
    max_content_len = len;
}

long ssl_pm_get_verify_result(const SSL *ssl)
{
    long ret;
    long verify_result;
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    ret = mbedtls_ssl_get_verify_result(&ssl_pm->ssl);

    if (!ret)
        verify_result = X509_V_OK;
    else
        verify_result = X509_V_ERR_UNSPECIFIED;

    return verify_result;
}
