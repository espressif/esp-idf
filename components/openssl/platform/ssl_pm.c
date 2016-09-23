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

    int load;
};

struct pkey_pm
{
    mbedtls_pk_context pkey;

    int load;
};


unsigned int max_content_len;


/*********************************************************************************************/
/************************************ SSL arch interface *************************************/

int ssl_pm_new(SSL *ssl)
{
    struct ssl_pm *ssl_pm;
    int ret;

    const unsigned char pers[] = "OpenSSL PM";
    size_t pers_len = sizeof(pers);

    int endpoint;
    int mode;
    int version;

    const SSL_METHOD *method = ssl->method;

    struct x509_pm *x509_pm;
    struct pkey_pm *pkey_pm;

    ssl->session.peer = ssl_malloc(sizeof(X509));
    if (!ssl->session.peer)
        SSL_ERR(ret, failed1, "ssl_malloc\n");

    ssl_pm = ssl_malloc(sizeof(struct ssl_pm));
    if (!ssl_pm)
        SSL_ERR(ret, failed2, "ssl_malloc\n");

    mbedtls_net_init(&ssl_pm->fd);
    mbedtls_net_init(&ssl_pm->cl_fd);

    mbedtls_ssl_config_init(&ssl_pm->conf);
    mbedtls_ctr_drbg_init(&ssl_pm->ctr_drbg);
    mbedtls_entropy_init(&ssl_pm->entropy);
    mbedtls_ssl_init(&ssl_pm->ssl);

    ret = mbedtls_ctr_drbg_seed(&ssl_pm->ctr_drbg, mbedtls_entropy_func, &ssl_pm->entropy, pers, pers_len);
    if (ret)
        SSL_ERR(ret, failed3, "mbedtls_ctr_drbg_seed:[-0x%x]\n", -ret);

    if (method->endpoint) {
        endpoint = MBEDTLS_SSL_IS_SERVER;
    } else {
        endpoint = MBEDTLS_SSL_IS_CLIENT;
    }
    ret = mbedtls_ssl_config_defaults(&ssl_pm->conf, endpoint, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret)
        SSL_ERR(ret, failed3, "mbedtls_ssl_config_defaults:[-0x%x]\n", -ret);

    if (TLS1_2_VERSION == ssl->version)
        version = MBEDTLS_SSL_MINOR_VERSION_3;
    else if (TLS1_1_VERSION == ssl->version)
        version = MBEDTLS_SSL_MINOR_VERSION_2;
    else if (TLS1_VERSION == ssl->version)
        version = MBEDTLS_SSL_MINOR_VERSION_1;
    else
        version = MBEDTLS_SSL_MINOR_VERSION_0;

    mbedtls_ssl_conf_max_version(&ssl_pm->conf, MBEDTLS_SSL_MAJOR_VERSION_3, version);

    mbedtls_ssl_conf_rng(&ssl_pm->conf, mbedtls_ctr_drbg_random, &ssl_pm->ctr_drbg);

    mbedtls_ssl_conf_dbg(&ssl_pm->conf, NULL, NULL);

    x509_pm = (struct x509_pm *)ssl->client_CA->x509_pm;
    if (x509_pm->load) {
        mbedtls_ssl_conf_ca_chain(&ssl_pm->conf, &x509_pm->x509_crt, NULL);

        mode = MBEDTLS_SSL_VERIFY_REQUIRED;
    } else {
        mode = MBEDTLS_SSL_VERIFY_NONE;
    }
    mbedtls_ssl_conf_authmode(&ssl_pm->conf, mode);

    pkey_pm = (struct pkey_pm *)ssl->cert->pkey->pkey_pm;
    if (pkey_pm->load) {
        x509_pm = (struct x509_pm *)ssl->cert->x509->x509_pm;

        ret = mbedtls_ssl_conf_own_cert(&ssl_pm->conf, &x509_pm->x509_crt, &pkey_pm->pkey);
        if (ret)
            SSL_ERR(ret, failed4, "mbedtls_ssl_conf_own_cert:[%d]\n", ret);
    }

    ret = mbedtls_ssl_setup(&ssl_pm->ssl, &ssl_pm->conf);
    if (ret)
        SSL_ERR(ret, failed5, "mbedtls_ssl_setup:[-0x%x]\n", -ret);

    mbedtls_ssl_set_bio(&ssl_pm->ssl, &ssl_pm->fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    ssl->ssl_pm = ssl_pm;

    return 0;

failed5:
    mbedtls_ssl_config_free(&ssl_pm->conf);
failed4:
    mbedtls_ctr_drbg_free(&ssl_pm->ctr_drbg);
failed3:
    mbedtls_entropy_free(&ssl_pm->entropy);
failed2:
    ssl_free(ssl->session.peer);
failed1:
    return -1;
}

void ssl_pm_free(SSL *ssl)
{
    struct ssl_pm *ssl_pm = (struct ssl_pm *)ssl->ssl_pm;

    mbedtls_ctr_drbg_free(&ssl_pm->ctr_drbg);
    mbedtls_entropy_free(&ssl_pm->entropy);
    mbedtls_ssl_config_free(&ssl_pm->conf);
    mbedtls_ssl_free(&ssl_pm->ssl);

    ssl_free(ssl_pm);
    ssl->ssl_pm = NULL;
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

    if (!mbed_ret) {
        ret = 1;

        ssl->session.peer = (X509 *)mbedtls_ssl_get_peer_cert(&ssl_pm->ssl);
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

int x509_pm_new(X509 *x)
{
    struct x509_pm *x509_pm;

    x509_pm = ssl_malloc(sizeof(struct x509_pm));
    if (!x509_pm)
        return -1;

    x->x509_pm = x509_pm;

    return 0;
}

void x509_pm_unload(X509 *x)
{
    struct x509_pm *x509_pm = (struct x509_pm *)x->x509_pm;

    if (x509_pm->load)
        mbedtls_x509_crt_free(&x509_pm->x509_crt);

    x509_pm->load = 0;
}

int x509_pm_load(X509 *x, const unsigned char *buffer, int len)
{
    int ret;
    unsigned char *load_buf;
    struct x509_pm *x509_pm = (struct x509_pm *)x->x509_pm;

    load_buf = ssl_malloc(len + 1);
    if (!load_buf)
        SSL_RET(failed1, "");

    ssl_memcpy(load_buf, buffer, len);
    load_buf[len] = '\0';

    x509_pm_unload(x);

    mbedtls_x509_crt_init(&x509_pm->x509_crt);
    ret = mbedtls_x509_crt_parse(&x509_pm->x509_crt, load_buf, len);
    ssl_free(load_buf);

    if (ret)
        SSL_RET(failed1, "");

    x509_pm->load = 1;

    return 0;

failed1:
    return -1;
}

void x509_pm_free(X509 *x)
{
    x509_pm_unload(x);

    ssl_free(x->x509_pm);
    x->x509_pm = NULL;
}

int pkey_pm_new(EVP_PKEY *pkey)
{
    struct pkey_pm *pkey_pm;

    pkey_pm = ssl_malloc(sizeof(struct pkey_pm));
    if (!pkey_pm)
        return -1;

    pkey->pkey_pm = pkey_pm;

    return 0;
}

void pkey_pm_unload(EVP_PKEY *pkey)
{
    struct pkey_pm *pkey_pm = (struct pkey_pm *)pkey->pkey_pm;

    if (pkey_pm->load)
        mbedtls_pk_free(&pkey_pm->pkey);

    pkey_pm->load = 0;
}

int pkey_pm_load(EVP_PKEY *pkey, const unsigned char *buffer, int len)
{
    int ret;
    unsigned char *load_buf;
    struct pkey_pm *pkey_pm = (struct pkey_pm *)pkey->pkey_pm;

    load_buf = ssl_malloc(len + 1);
    if (!load_buf)
        SSL_RET(failed1, "");

    ssl_memcpy(load_buf, buffer, len);
    load_buf[len] = '\0';

    pkey_pm_unload(pkey);

    mbedtls_pk_init(&pkey_pm->pkey);
    ret = mbedtls_pk_parse_key(&pkey_pm->pkey, load_buf, len, NULL, 0);
    ssl_free(load_buf);

    if (ret)
        SSL_RET(failed1, "");

    pkey_pm->load = 1;

    return 0;

failed1:
    return -1;
}

void pkey_pm_free(EVP_PKEY *pkey)
{
    pkey_pm_unload(pkey);

    ssl_free(pkey->pkey_pm);
    pkey->pkey_pm = NULL;
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
