#ifndef _SSL_X509_H_
#define _SSL_X509_H_

#include "ssl_types.h"

X509* sk_X509_NAME_new_null(void);

X509* d2i_X509(X509 **cert, const unsigned char *buffer, long len);

void X509_free(X509 *cert);

#endif
