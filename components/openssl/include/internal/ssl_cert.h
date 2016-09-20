#ifndef _SSL_CERT_H_
#define _SSL_CERT_H_

#include "ssl_pkey.h"
#include "ssl_x509.h"

CERT *ssl_cert_new(void);

void ssl_cert_free(CERT *c);

#endif
