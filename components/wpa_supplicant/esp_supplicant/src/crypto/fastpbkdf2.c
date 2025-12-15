/*
 * SPDX-FileCopyrightText: 2015 Joseph Birr-Pixton <jpixton@gmail.com>
 *
 * SPDX-License-Identifier: CC0-1.0
 */

/*
 * fast-pbkdf2 - Optimal PBKDF2-HMAC calculation
 * Written in 2015 by Joseph Birr-Pixton <jpixton@gmail.com>
 *
 * To the extent possible under law, the author(s) have dedicated all
 * copyright and related and neighboring rights to this software to the
 * public domain worldwide. This software is distributed without any
 * warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication
 * along with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>.
 */
#include "utils/common.h"
#include "fastpbkdf2.h"

#include <assert.h>
#include <string.h>
#if defined(__GNUC__)
#include <endian.h>
#endif
#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS
#include "mbedtls/esp_config.h"
#include "utils/wpa_debug.h"
#include "psa/crypto.h"

/* --- MSVC doesn't support C99 --- */
#ifdef _MSC_VER
#define restrict
#define _Pragma __pragma
#endif

/* --- Common useful things --- */
#ifndef MIN
#define MIN(a, b) ((a) > (b)) ? (b) : (a)
#endif

void fastpbkdf2_hmac_sha1(const uint8_t *pw, size_t npw,
                          const uint8_t *salt, size_t nsalt,
                          uint32_t iterations,
                          uint8_t *out, size_t nout)
{
    psa_status_t status;
    psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;

    // Set up key attributes for password
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_1));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_PASSWORD);

    // Import password as key
    status = psa_import_key(&attributes, pw, npw, &key_id);
    if (status != PSA_SUCCESS) {
        psa_reset_key_attributes(&attributes);
        return;
    }

    // Set up key derivation
    status = psa_key_derivation_setup(&operation, PSA_ALG_PBKDF2_HMAC(PSA_ALG_SHA_1));
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    // Set iteration count
    status = psa_key_derivation_input_integer(&operation, PSA_KEY_DERIVATION_INPUT_COST,
                                              iterations);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    // Add salt
    status = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_SALT,
                                            salt, nsalt);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    // Add password
    status = psa_key_derivation_input_key(&operation, PSA_KEY_DERIVATION_INPUT_PASSWORD, key_id);
    if (status != PSA_SUCCESS) {
        goto cleanup;
    }

    // Generate output
    status = psa_key_derivation_output_bytes(&operation, out, nout);

cleanup:
    psa_key_derivation_abort(&operation);
    if (key_id) {
        psa_destroy_key(key_id);
    }
    psa_reset_key_attributes(&attributes);
}
