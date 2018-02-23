// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef OPEN_SSL_H
#define OPEN_SSL_H

#ifdef __cplusplus

extern "C" {
#include <cstdint>
#else
#include <stdint.h>
#endif /* __cplusplus */

#include "azure_c_shared_utility/umock_c_prod.h"

// This header mocks the small subset of the the OpenSSL ssl.h needed for tlsio_openssl_compact testing

typedef void SSL;
typedef void SSL_CTX;

#define SSL_ERROR_WANT_READ 2
#define SSL_ERROR_WANT_WRITE 3

int TLSv1_2_client_method();
void SSL_CTX_set_default_read_buffer_len(SSL_CTX* dummy, int dummy2);


MOCKABLE_FUNCTION(, void, SSL_free, SSL*, ssl);
MOCKABLE_FUNCTION(, void, SSL_CTX_free, SSL_CTX*, ctx);
MOCKABLE_FUNCTION(, SSL_CTX*, SSL_CTX_new, int, dummy);
MOCKABLE_FUNCTION(, SSL*, SSL_new, SSL_CTX*, dummy);
MOCKABLE_FUNCTION(, int, SSL_set_fd, SSL*, dummy, int, dummy2);
MOCKABLE_FUNCTION(, int, SSL_connect, SSL*, dummy);
MOCKABLE_FUNCTION(, int, SSL_write, SSL*, dummy, uint8_t*, buffer, size_t, size);
MOCKABLE_FUNCTION(, int, SSL_read, SSL*, dummy, uint8_t*, buffer, size_t, size);
MOCKABLE_FUNCTION(, int, SSL_shutdown, SSL*, dummy);
MOCKABLE_FUNCTION(, int, SSL_get_error, SSL_CTX*, dummy, int, last_value);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // OPEN_SSL_H
