// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef crypto_hash_sha256_H
#define crypto_hash_sha256_H

/* This is a wrapper for libsodium sha256 that calls back to
   the mbedTLS implementation (to reduce code size, improve
   performance, provide hardware acceleration option).
*/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <mbedtls/sha256.h>

#include "sodium/export.h"

#ifdef __cplusplus
# ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wlong-long"
# endif
extern "C" {
#endif

typedef mbedtls_sha256_context crypto_hash_sha256_state;

SODIUM_EXPORT
size_t crypto_hash_sha256_statebytes(void);

#define crypto_hash_sha256_BYTES 32U
SODIUM_EXPORT
size_t crypto_hash_sha256_bytes(void);

SODIUM_EXPORT
int crypto_hash_sha256(unsigned char *out, const unsigned char *in,
                       unsigned long long inlen);

SODIUM_EXPORT
int crypto_hash_sha256_init(crypto_hash_sha256_state *state);

SODIUM_EXPORT
int crypto_hash_sha256_update(crypto_hash_sha256_state *state,
                              const unsigned char *in,
                              unsigned long long inlen);

SODIUM_EXPORT
int crypto_hash_sha256_final(crypto_hash_sha256_state *state,
                             unsigned char *out);

#ifdef __cplusplus
}
#endif

#endif
