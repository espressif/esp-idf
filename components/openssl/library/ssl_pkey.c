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

#include "ssl_lib.h"
#include "ssl_pkey.h"
#include "ssl_cert.h"
#include "ssl_methods.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

/**
 * @brief create a private key object
 */
EVP_PKEY* EVP_PKEY_new(void)
{
    int ret;
    EVP_PKEY *pkey;

    pkey = ssl_zalloc(sizeof(EVP_PKEY));
    if (!pkey)
        SSL_RET(failed1, "ssl_malloc\n");

    pkey->method = EVP_PKEY_method();

    ret = EVP_PKEY_METHOD_CALL(new, pkey);
    if (ret)
        SSL_RET(failed2, "EVP_PKEY_METHOD_CALL\n");

    return pkey;

failed2:
    ssl_free(pkey);
failed1:
    return NULL;
}

/**
 * @brief free a private key object
 */
void EVP_PKEY_free(EVP_PKEY *pkey)
{
    EVP_PKEY_METHOD_CALL(free, pkey);

    ssl_free(pkey);
}

/**
 * @brief load a character key context into system context. If '*a' is pointed to the
 *        private key, then load key into it. Or create a new private key object
 */
EVP_PKEY *d2i_PrivateKey(int type,
                         EVP_PKEY **a,
                         const unsigned char **pp,
                         long length)
{
    int m = 0;
    int ret;
    EVP_PKEY *pkey;

    SSL_ASSERT(pp);
    SSL_ASSERT(*pp);
    SSL_ASSERT(length);

    if (a && *a) {
        pkey = *a;
    } else {
        pkey = EVP_PKEY_new();;
        if (!pkey)
            SSL_RET(failed1, "EVP_PKEY_new\n");
        m = 1;
    }

    ret = EVP_PKEY_METHOD_CALL(load, pkey, *pp, length);
    if (ret)
        SSL_RET(failed2, "EVP_PKEY_METHOD_CALL\n");

    if (a)
        *a = pkey;

    return pkey;

failed2:
    if (m)
        EVP_PKEY_free(pkey);
failed1:
    return NULL;
}

/**
 * @brief set the SSL context private key
 */
int SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(pkey);

    if (ctx->cert->pkey)
        EVP_PKEY_free(ctx->cert->pkey);

    ctx->cert->pkey = pkey;

    return 1;
}

/**
 * @brief set the SSL private key
 */
int SSL_use_PrivateKey(SSL *ssl, EVP_PKEY *pkey)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(pkey);

    if (!ssl->ca_reload)
        ssl->ca_reload = 1;
    else
        EVP_PKEY_free(ssl->cert->pkey);

    ssl->cert->pkey = pkey;

    return 1;
}

/**
 * @brief load private key into the SSL context
 */
int SSL_CTX_use_PrivateKey_ASN1(int type, SSL_CTX *ctx,
                                const unsigned char *d, long len)
{
    int ret;
    EVP_PKEY *pkey;

    pkey = d2i_PrivateKey(0, &ctx->cert->pkey, &d, len);
    if (!pkey)
        SSL_RET(failed1, "d2i_PrivateKey\n");

    ret = SSL_CTX_use_PrivateKey(ctx, pkey);
    if (!ret)
        SSL_RET(failed2, "SSL_CTX_use_PrivateKey\n");

    return 1;

failed2:
    EVP_PKEY_free(pkey);
failed1:
    return 0;
}

/**
 * @brief load private key into the SSL
 */
int SSL_use_PrivateKey_ASN1(int type, SSL *ssl,
                                const unsigned char *d, long len)
{
    int ret;
    int reload;
    EVP_PKEY *pkey;
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

    pkey = d2i_PrivateKey(0, &ssl->cert->pkey, &d, len);
    if (!pkey)
        SSL_RET(failed2, "d2i_PrivateKey\n");

    ret = SSL_use_PrivateKey(ssl, pkey);
    if (!ret)
        SSL_RET(failed3, "SSL_use_PrivateKey\n");

    return 1;

failed3:
    EVP_PKEY_free(pkey);
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
 * @brief load the private key file into SSL context
 */
int SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type)
{
    return 0;
}

/**
 * @brief load the private key file into SSL
 */
int SSL_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type)
{
    return 0;
}

/**
 * @brief load the RSA ASN1 private key into SSL context
 */
int SSL_CTX_use_RSAPrivateKey_ASN1(SSL_CTX *ctx, const unsigned char *d, long len)
{
    return SSL_CTX_use_PrivateKey_ASN1(0, ctx, d, len);
}
