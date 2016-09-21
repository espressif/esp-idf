#ifndef _SSL_PKEY_H_
#define _SSL_PKEY_H_

#include "ssl_types.h"

EVP_PKEY *d2i_PrivateKey(int type,
                         EVP_PKEY **a,
                         const unsigned char **pp,
                         long length);

void EVP_PKEY_free(EVP_PKEY *x);

#endif
