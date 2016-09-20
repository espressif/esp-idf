#include "ssl_lib.h"
#include "ssl_pkey.h"
#include "ssl_dbg.h"
#include "ssl_pm.h"

EVP_PKEY *d2i_PrivateKey(int type,
                         EVP_PKEY **a,
                         const unsigned char **pp,
                         long length)
{
    EVP_PKEY *pkey;
    void *pkey_pm;
    int ret;

    SSL_ASSERT(pp);
    SSL_ASSERT(*pp);
    SSL_ASSERT(length);

    pkey = ssl_malloc(sizeof(EVP_PKEY));
    if (!pkey)
        SSL_RET(failed1, "ssl_malloc\n");

    pkey_pm = pkey_pm_new();
    if (!pkey_pm)
        SSL_RET(failed2, "pkey_pm_new\n");

    ret = pkey_pm_load_crt(pkey_pm, *pp, length);
    if (ret)
        SSL_RET(failed3, "pkey_pm_load_crt\n");

    pkey->pkey_pm = pkey_pm;
    if (a)
        *a = pkey;

    return pkey;

failed3:
    pkey_pm_free(pkey_pm);
failed2:
    ssl_free(pkey);
failed1:
    return NULL;
}

void EVP_PKEY_free(EVP_PKEY *x)
{
    pkey_pm_unload_crt(x->pkey_pm);
    pkey_pm_free(x->pkey_pm);
    ssl_free(x);
}
