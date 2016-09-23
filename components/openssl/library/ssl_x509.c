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
#include "ssl_cert.h"
#include "ssl_methods.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

/**
 * @brief create a X509 certification object
 */
X509* X509_new(void)
{
    int ret;
    X509 *x;

    x = ssl_zalloc(sizeof(X509));
    if (!x)
        SSL_RET(failed1, "ssl_malloc\n");

    x->method = X509_method();

    ret = X509_METHOD_CALL(new, x);
    if (ret)
        SSL_RET(failed2, "x509_new\n");

    return x;

failed2:
    ssl_free(x);
failed1:
    return NULL;
}

/**
 * @brief free a X509 certification object
 */
void X509_free(X509 *x)
{
    X509_METHOD_CALL(free, x);

    ssl_free(x);
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

    SSL_ASSERT(buffer);
    SSL_ASSERT(len);

    if (cert && *cert) {
        x = *cert;
    } else {
        x = X509_new();
        if (!x)
            SSL_RET(failed1, "X509_new\n");
        m = 1;
    }

    ret = X509_METHOD_CALL(load, x, buffer, len);
    if (ret)
        SSL_RET(failed2, "X509_METHOD_CALL\n");

    return x;

failed2:
    if (m)
        X509_free(x);
failed1:
    return NULL;
}

/**
 * @brief set SSL context client CA certification
 */
int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    if (ctx->client_CA)
        X509_free(ctx->client_CA);

    ctx->client_CA = x;

    return 1;
}

/**
 * @brief add CA client certification into the SSL
 */
int SSL_add_client_CA(SSL *ssl, X509 *x)
{
    SSL_ASSERT(ssl);
    SSL_ASSERT(x);

    if (!ssl->ca_reload)
        ssl->ca_reload = 1;
    else
        X509_free(ssl->client_CA);

    ssl->client_CA = x;

    return 1;
}

/**
 * @brief set the SSL context certification
 */
int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    ctx->cert->x509 = x;

    return 1;
}

/**
 * @brief set the SSL certification
 */
int SSL_use_certificate(SSL *ssl, X509 *x)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    ssl->cert->x509 = x;

    return 1;
}

/**
 * @brief get the SSL certification point
 */
X509 *SSL_get_certificate(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->cert->x509;
}

/**
 * @brief load certification into the SSL context
 */
int SSL_CTX_use_certificate_ASN1(SSL_CTX *ctx, int len,
                                 const unsigned char *d)
{
    int ret;
    X509 *cert;

    cert = d2i_X509(&ctx->cert->x509, d, len);
    if (!cert)
        SSL_RET(failed1, "d2i_X509\n");

    ret = SSL_CTX_use_certificate(ctx, cert);
    if (!ret)
        SSL_RET(failed2, "SSL_CTX_use_certificate\n");

    return 1;

failed2:
    X509_free(cert);
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
    int reload;
    X509 *x;
    CERT *cert;
    CERT *old_cert;

    if (!ssl->crt_reload) {
        cert = ssl_cert_new();
        if (!cert)
            SSL_RET(failed1, "ssl_cert_new\n");

        old_cert = ssl->cert ;
        ssl->cert = cert;

        ssl->crt_reload = 1;

        reload = 1;
    } else {
        reload = 0;
    }

    x = d2i_X509(&ssl->cert->x509, d, len);
    if (!x)
        SSL_RET(failed2, "d2i_X509\n");

    ret = SSL_use_certificate(ssl, x);
    if (!ret)
        SSL_RET(failed3, "SSL_use_certificate\n");

    return 1;

failed3:
    X509_free(x);
failed2:
    if (reload) {
        ssl->cert = old_cert;
        ssl_cert_free(cert);
        ssl->crt_reload = 0;
    }
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
    SSL_ASSERT(ssl);

    return ssl->session->peer;
}
