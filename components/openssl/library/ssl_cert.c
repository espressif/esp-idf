#include "ssl_cert.h"
#include "ssl_pm.h"

CERT *ssl_cert_new(void)
{
    return ssl_zalloc(sizeof(CERT));
}

void ssl_cert_free(CERT *c)
{
    if (c->x509)
        X509_free(c->x509);

    if (c->pkey)
        EVP_PKEY_free(c->pkey);

    ssl_free(c);
}

int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(x);

    ctx->client_CA = x;

    return 1;
}
