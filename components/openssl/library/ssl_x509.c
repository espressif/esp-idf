#include "ssl_x509.h"
#include "ssl_dbg.h"
#include "ssl_pm.h"

X509* sk_X509_NAME_new_null(void)
{
    return ssl_malloc(sizeof(X509));
}

X509* d2i_X509(X509 **cert, const unsigned char *buffer, long len)
{
    X509 *x509_crt;
    void *x509_pm;
    int ret;

    SSL_ASSERT(cert);
    SSL_ASSERT(buffer);
    SSL_ASSERT(len);

    x509_crt = sk_X509_NAME_new_null();
    if (!x509_crt)
        SSL_RET(failed1, "");

    x509_pm = x509_pm_new();
    if (!x509_pm)
        SSL_RET(failed2, "");

    ret = x509_pm_load_crt(x509_pm, buffer, len);
    if (ret)
        SSL_RET(failed3, "");

    x509_crt->x509_pm = x509_pm;
    if (cert)
        *cert = x509_crt;

    return x509_crt;

failed3:
    x509_pm_free(x509_pm);
failed2:
    ssl_free(x509_crt);
failed1:
    return NULL;
}

void X509_free(X509 *cert)
{
    if (cert->x509_pm) {
        x509_pm_unload_crt(cert->x509_pm);
        x509_pm_free(cert->x509_pm);
    }
    ssl_free(cert);
};

