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

/*
 * sk_X509_NAME_new_null - create a X509 certification object
 *
 * @param none
 *
 * @return X509 certification object point or NULL if failed
 */
X509* X509_new(void)
{
    int ret;
    X509 *x;

    x = ssl_malloc(sizeof(X509));
    if (!x)
        SSL_RET(failed1, "ssl_malloc\n");

    x->method = X509_method();

    ret = x->method->x509_new(x);
    if (ret)
        SSL_RET(failed2, "x509_new\n");

    return x;

failed2:
    ssl_free(x);
failed1:
    return NULL;
}

/*
 * X509_free - free a X509 certification object
 *
 * @param x - X509 certification object point
 *
 * @return none
 */
void X509_free(X509 *x)
{
    X509_METHOD_CALL(free, x);

    ssl_free(x);
};

/*
 * d2i_X509 - load a character certification context into system context. If '*cert' is pointed to the
 *            certification, then load certification into it. Or create a new X509 certification object
 *
 * @param cert   - a point pointed to X509 certification
 * @param buffer - a point pointed to the certification context memory point
 * @param length - certification bytes
 *
 * @return X509 certification object point or NULL if failed
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
            SSL_RET(failed1, "sk_X509_NAME_new_null\n");
        m = 1;
    }

    ret = X509_METHOD_CALL(load, x, buffer, len);
    if (ret)
        SSL_RET(failed2, "x509_load\n");

    return x;

failed2:
    if (m)
        X509_free(x);
failed1:
    return NULL;
}

/*
 * SSL_CTX_add_client_CA - set SSL context client CA certification
 *
 * @param ctx - SSL context point
 * @param x   - client CA certification point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x)
{
    int ret;

    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    if (ctx->client_CA)
        X509_free(ctx->client_CA);

    ctx->client_CA = x;

    return 1;
}

/*
 * SSL_add_client_CA - add CA client certification into the SSL
 *
 * @param ssl - SSL point
 * @param x   - CA certification point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_add_client_CA(SSL *ssl, X509 *x)
{
    int ret;

    SSL_ASSERT(ssl);
    SSL_ASSERT(x);

    if (ssl->client_CA)
        X509_free(ssl->client_CA);

    ssl->client_CA = x;

    return 1;
}

/*
 * SSL_CTX_use_certificate - set the SSL context certification
 *
 * @param ctx - SSL context point
 * @param x   - X509 certification point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    ctx->cert->x509 = x;

    return 1;
}

/*
 * SSL_get_certificate - get the SSL certification point
 *
 * @param ssl - SSL point
 *
 * @return SSL certification point
 */
X509 *SSL_get_certificate(const SSL *ssl)
{
    return ssl->cert->x509;
}

/*
 * SSL_CTX_use_certificate_ASN1 - load certification into the SSL context
 *
 * @param ctx - SSL context point
 * @param len - certification context bytes
 * @param d   - certification context point
 *
 * @return
 *         1 : OK
 *         0 : failed
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

