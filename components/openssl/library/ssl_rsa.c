#include "ssl_lib.h"
#include "ssl_rsa.h"
#include "ssl_pkey.h"
#include "ssl_x509.h"
#include "ssl_dbg.h"
#include "ssl_pm.h"

int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    ctx->cert->x509 = x;

    return 1;
}

int SSL_CTX_use_certificate_ASN1(SSL_CTX *ctx, int len,
                                 const unsigned char *d)
{
    int ret;
    X509 *cert;

    cert = d2i_X509(NULL, d, len);
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

int SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(pkey);

    ctx->cert->pkey = pkey;

    return 1;
}

int SSL_CTX_use_PrivateKey_ASN1(int type, SSL_CTX *ctx,
                                const unsigned char *d, long len)
{
    int ret;
    EVP_PKEY *pkey;

    pkey = d2i_PrivateKey(0, NULL, &d, len);
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
