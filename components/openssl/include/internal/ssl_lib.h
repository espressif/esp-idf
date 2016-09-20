#ifndef _SSL_LIB_H_
#define _SSL_LIB_H_

#include "ssl_types.h"

#define SSL_want_nothing(s)     (SSL_want(s) == SSL_NOTHING)
#define SSL_want_read(s)        (SSL_want(s) == SSL_READING)
#define SSL_want_write(s)       (SSL_want(s) == SSL_WRITING)
#define SSL_want_x509_lookup(s) (SSL_want(s) == SSL_WRITING)

#endif
