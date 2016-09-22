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
#include "ssl_rsa.h"
#include "ssl_pkey.h"
#include "ssl_x509.h"
#include "ssl_dbg.h"

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

    return 1;

failed2:
    EVP_PKEY_free(pkey);
failed1:
    return 0;
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
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    ctx->client_CA = x;

    return 1;
}
