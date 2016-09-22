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
#include "ssl_methods.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

/*
 * EVP_PKEY_new - create a private key object
 *
 * @param none
 *
 * @return private key object point or NULL if failed
 */
EVP_PKEY* EVP_PKEY_new(void)
{
    int ret;
    EVP_PKEY *pkey;

    pkey = ssl_malloc(sizeof(EVP_PKEY));
    if (!pkey)
        SSL_RET(failed1, "ssl_malloc\n");

    pkey->method = EVP_PKEY_method();

    ret = EVP_PKEY_METHOD_CALL(new, pkey);
    if (ret)
        SSL_RET(failed2, "pkey_new\n");

    return pkey;

failed2:
    ssl_free(pkey);
failed1:
    return NULL;
}

/*
 * EVP_PKEY_free - free a private key object
 *
 * @param pkey - private key object point
 *
 * @return none
 */
void EVP_PKEY_free(EVP_PKEY *pkey)
{
    EVP_PKEY_METHOD_CALL(free, pkey);

    ssl_free(pkey);
}

/*
 * d2i_PrivateKey - load a character key context into system context. If '*a' is pointed to the
 *                  private key, then load key into it. Or create a new private key object
 *
 * @param type   - private key type
 * @param a      - a point pointed to a private key point
 * @param pp     - a point pointed to the key context memory point
 * @param length - key bytes
 *
 * @return private key object point or NULL if failed
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
            SSL_RET(failed1, "ssl_malloc\n");
        m = 1;
    }

    ret = EVP_PKEY_METHOD_CALL(load, pkey, *pp, length);
    if (ret)
        SSL_RET(failed2, "pkey_pm_load_crt\n");

    if (a)
        *a = pkey;

    return pkey;

failed2:
    if (m)
        EVP_PKEY_free(pkey);
failed1:
    return NULL;
}

/*
 * SSL_CTX_use_certificate - set the SSL context private key
 *
 * @param ctx - SSL context point
 * @param x   - private key point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(pkey);

    ctx->cert->pkey = pkey;

    return 1;
}

/*
 * SSL_CTX_use_certificate - set the SSL private key
 *
 * @param ctx - SSL point
 * @param x   - private key point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_use_PrivateKey(SSL *ssl, EVP_PKEY *pkey)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(pkey);

    ssl->cert->pkey = pkey;

    return 1;
}

/*
 * SSL_CTX_use_PrivateKey_ASN1 - load private key into the SSL context
 *
 * @param type - private key type
 * @param ctx  - SSL context point
 * @param d    - private key context point
 * @param len  - private key context bytes
 *
 * @return
 *         1 : OK
 *         0 : failed
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

    ctx->cert->pkey->ref = 1;

    return 1;

failed2:
    EVP_PKEY_free(pkey);
failed1:
    return 0;
}

/*
 * SSL_use_PrivateKey_ASN1 - load private key into the SSL
 *
 * @param type - private key type
 * @param ctx  - SSL context point
 * @param d    - private key context point
 * @param len  - private key context bytes
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_use_PrivateKey_ASN1(int type, SSL *ssl,
                                const unsigned char *d, long len)
{
    int ret;
    EVP_PKEY *pkey;

    if (ssl->cert->pkey->ref)
        SSL_RET(failed1);

    pkey = d2i_PrivateKey(0, NULL, &d, len);
    if (!pkey)
        SSL_RET(failed1, "d2i_PrivateKey\n");

    ret = SSL_use_PrivateKey(ssl, pkey);
    if (!ret)
        SSL_RET(failed2, "SSL_CTX_use_PrivateKey\n");

    ssl->cert->pkey->ref = 1;

    return 1;

failed2:
    EVP_PKEY_free(pkey);
failed1:
    return 0;
}

/*
 * SSL_CTX_use_certificate_file - load the private key file into SSL context
 *
 * @param ctx  - SSL context point
 * @param file - private key file name
 * @param type - private key encoding type
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type)
{
    return 0;
}

/*
 * SSL_use_PrivateKey_file - load the private key file into SSL
 *
 * @param ctx  - SSL point
 * @param file - private key file name
 * @param type - private key encoding type
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type)
{
    return 0;
}

/*
 * SSL_CTX_use_certificate_ASN1 - load the RSA ASN1 private key into SSL context
 *
 * @param ctx - SSL context point
 * @param d   - data point
 * @param len - RSA private key length
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_RSAPrivateKey_ASN1(SSL_CTX *ctx, const unsigned char *d, long len)
{
    return SSL_CTX_use_PrivateKey_ASN1(0, ctx, d, len);
}
