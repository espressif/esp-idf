// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _ROM_RSA_PSS_H_
#define _ROM_RSA_PSS_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ETS_SIG_LEN 384 /* Bytes */
#define ETS_DIGEST_LEN 32 /* SHA-256, bytes */

typedef struct {
    uint8_t n[384]; /* Public key modulus */
    uint32_t e;     /* Public key exponent */
    uint8_t rinv[384];
    uint32_t mdash;
} ets_rsa_pubkey_t;

bool ets_rsa_pss_verify(const ets_rsa_pubkey_t *key, const uint8_t *sig, const uint8_t *digest, uint8_t *verified_digest);

void ets_mgf1_sha256(const uint8_t *mgfSeed, size_t seedLen, size_t maskLen, uint8_t *mask);

bool ets_emsa_pss_verify(const uint8_t *encoded_message, const uint8_t *mhash);

#ifdef __cplusplus
}
#endif

#endif
