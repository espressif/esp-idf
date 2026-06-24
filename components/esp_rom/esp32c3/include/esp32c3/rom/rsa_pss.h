/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ROM_RSA_PSS_H_
#define _ROM_RSA_PSS_H_

#include <stdint.h>
#include <stdbool.h>
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
