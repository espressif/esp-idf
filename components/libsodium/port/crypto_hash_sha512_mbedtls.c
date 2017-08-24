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

#include "crypto_hash_sha512.h"

int
crypto_hash_sha512_init(crypto_hash_sha512_state *state)
{
    mbedtls_sha512_init(state);
    return 0;
}

int
crypto_hash_sha512_update(crypto_hash_sha512_state *state,
                          const unsigned char *in, unsigned long long inlen)
{
    mbedtls_sha512_update(state, in, inlen);
    return 0;
}

int
crypto_hash_sha512_final(crypto_hash_sha512_state *state, unsigned char *out)
{
    mbedtls_sha512_finish(state, out);
    return 0;
}

int
crypto_hash_sha512(unsigned char *out, const unsigned char *in,
                   unsigned long long inlen)
{
    mbedtls_sha512(in, inlen, out, 0);
    return 0;
}
