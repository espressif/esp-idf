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

#include "ssl_x509.h"
#include "ssl_methods.h"
#include "ssl_dbg.h"
#include "ssl_port.h"
#include "ssl.h"

/**
 * @brief show X509 certification information
 */
int __X509_show_info(X509 *x)
{
    return X509_METHOD_CALL(show_info, x);
}

/**
 * @brief create a X509 certification object according to input X509 certification
 */
X509* __X509_new(X509 *ix)
{
    int ret;
    X509 *x;

    x = ssl_mem_zalloc(sizeof(X509));
    if (!x) {
        SSL_DEBUG(SSL_X509_ERROR_LEVEL, "no enough memory > (x)");
        goto no_mem;
    }

    x->ref_counter = 1;

    if (ix)
        x->method = ix->method;
    else
        x->method = X509_method();

    ret = X509_METHOD_CALL(new, x, ix);
    if (ret) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "X509_METHOD_CALL(new) return %d", ret);
        goto failed;
    }

    return x;

failed:
    ssl_mem_free(x);
no_mem:
    return NULL;
}

/**
 * @brief create a X509 certification object
 */
X509* X509_new(void)
{
    return __X509_new(NULL);
}

/**
 * @brief free a X509 certification object
 */
void X509_free(X509 *x)
{
    SSL_ASSERT3(x);

    if (--x->ref_counter > 0) {
        return;
    }

    X509_METHOD_CALL(free, x);

    ssl_mem_free(x);
};

/**
 * @brief load a character certification context into system context. If '*cert' is pointed to the
 *        certification, then load certification into it. Or create a new X509 certification object
 */
X509* d2i_X509(X509 **cert, const unsigned char *buffer, long len)
{
    int m = 0;
    int ret;
    X509 *x;

    SSL_ASSERT2(buffer);
    SSL_ASSERT2(len);

    if (cert && *cert) {
        x = *cert;
    } else {
        x = X509_new();
        if (!x) {
            SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "X509_new() return NULL");
            goto failed1;
        }
        m = 1;
    }

    ret = X509_METHOD_CALL(load, x, buffer, len);
    if (ret) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "X509_METHOD_CALL(load) return %d", ret);
        goto failed2;
    }

    return x;

failed2:
    if (m)
        X509_free(x);
failed1:
    return NULL;
}

/**
 * @brief return SSL X509 verify parameters
 */

X509_VERIFY_PARAM *SSL_get0_param(SSL *ssl)
{
    return &ssl->param;
}

/**
 * @brief set X509 host verification flags
 */

int X509_VERIFY_PARAM_set_hostflags(X509_VERIFY_PARAM *param,
                    unsigned long flags)
{
    /* flags not supported yet */
    return 0;
}

/**
 * @brief clear X509 host verification flags
 */

int X509_VERIFY_PARAM_clear_hostflags(X509_VERIFY_PARAM *param,
                      unsigned long flags)
{
    /* flags not supported yet */
    return 0;
}

/**
 * @brief set SSL context client CA certification
 */
int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT1(ctx);
    SSL_ASSERT1(x);

    if (ctx->client_CA == x)
        return 1;

    X509_free(ctx->client_CA);

    ctx->client_CA = x;

    return 1;
}

/**
 * @brief add CA client certification into the SSL
 */
int SSL_add_client_CA(SSL *ssl, X509 *x)
{
    SSL_ASSERT1(ssl);
    SSL_ASSERT1(x);

    if (ssl->client_CA == x)
        return 1;

    X509_free(ssl->client_CA);

    ssl->client_CA = x;

    return 1;
}

/**
 * @brief set the SSL context certification
 */
int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT1(ctx);
    SSL_ASSERT1(x);

    if (ctx->cert->x509 == x)
        return 1;

    X509_free(ctx->cert->x509);

    ctx->cert->x509 = x;

    return 1;
}

/**
 * @brief set the SSL certification
 */
int SSL_use_certificate(SSL *ssl, X509 *x)
{
    SSL_ASSERT1(ssl);
    SSL_ASSERT1(x);

    if (ssl->cert->x509 == x)
        return 1;

    X509_free(ssl->cert->x509);

    ssl->cert->x509 = x;

    return 1;
}

/**
 * @brief get the SSL certification point
 */
X509 *SSL_get_certificate(const SSL *ssl)
{
    SSL_ASSERT2(ssl);

    return ssl->cert->x509;
}

/**
 * @brief load certification into the SSL context
 */
int SSL_CTX_use_certificate_ASN1(SSL_CTX *ctx, int len,
                                 const unsigned char *d)
{
    int ret;
    X509 *x;

    x = d2i_X509(NULL, d, len);
    if (!x) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "d2i_X509() return NULL");
        goto failed1;
    }

    ret = SSL_CTX_use_certificate(ctx, x);
    if (!ret) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "SSL_CTX_use_certificate() return %d", ret);
        goto failed2;
    }

    return 1;

failed2:
    X509_free(x);
failed1:
    return 0;
}

/**
 * @brief load certification into the SSL
 */
int SSL_use_certificate_ASN1(SSL *ssl, int len,
                             const unsigned char *d)
{
    int ret;
    X509 *x;

    x = d2i_X509(NULL, d, len);
    if (!x) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "d2i_X509() return NULL");
        goto failed1;
    }

    ret = SSL_use_certificate(ssl, x);
    if (!ret) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "SSL_use_certificate() return %d", ret);
        goto failed2;
    }

    return 1;

failed2:
    X509_free(x);
failed1:
    return 0;
}

/**
 * @brief load the certification file into SSL context
 */
int SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type)
{
    return 0;
}

/**
 * @brief load the certification file into SSL
 */
int SSL_use_certificate_file(SSL *ssl, const char *file, int type)
{
    return 0;
}

/**
 * @brief get peer certification
 */
X509 *SSL_get_peer_certificate(const SSL *ssl)
{
    SSL_ASSERT2(ssl);

    return ssl->session->peer;
}

/**
 * @brief set SSL context client CA certification
 */
int X509_STORE_add_cert(X509_STORE *store, X509 *x) {

    x->ref_counter++;

    SSL_CTX *ctx = (SSL_CTX *)store;
    SSL_ASSERT1(ctx);
    SSL_ASSERT1(x);

    if (ctx->client_CA == x) {
        return 1;
    }

    if (ctx->client_CA!=NULL) {
        X509_free(ctx->client_CA);
    }

    ctx->client_CA = x;
    return 1;
}

/**
 * @brief create a BIO object
 */
BIO *BIO_new(void  *method) {
    BIO *b = (BIO *)malloc(sizeof(BIO));
    return b;
}

/**
 * @brief load data into BIO.
 *
 * Normally BIO_write should append data but doesn't happen here, and
 * 'data' cannot be freed after the function is called, it should remain valid 
 * until BIO object is in use.
 */
int BIO_write(BIO *b, const void * data, int dlen) {
    b->data = data;
    b->dlen = dlen;
    return 1;
}

/**
 * @brief load a character certification context into system context.
 * 
 * If '*cert' is pointed to the certification, then load certification
 * into it, or create a new X509 certification object.
 */
X509 * PEM_read_bio_X509(BIO *bp, X509 **cert, void *cb, void *u) {
    int m = 0;
    int ret;
    X509 *x;

    SSL_ASSERT2(bp->data);
    SSL_ASSERT2(bp->dlen);

    if (cert && *cert) {
        x = *cert;
    } else {
        x = X509_new();
        if (!x) {
            SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "X509_new() return NULL");
            goto failed;
        }
        m = 1;
    }

    ret = X509_METHOD_CALL(load, x, bp->data, bp->dlen);
    if (ret) {
        SSL_DEBUG(SSL_PKEY_ERROR_LEVEL, "X509_METHOD_CALL(load) return %d", ret);
        goto failed;
    }

    return x;

failed:
    if (m) {
        X509_free(x);
    }

    return NULL;
}

/**
 * @brief get the memory BIO method function
 */
void *BIO_s_mem(void) {
    return NULL;
}

/**
 * @brief get the SSL context object X509 certification storage
 */
X509_STORE *SSL_CTX_get_cert_store(const SSL_CTX *ctx) {
    return (X509_STORE *)ctx;
}

/**
 * @brief free a BIO object
 */
void BIO_free(BIO *b) {
    free(b);
}
