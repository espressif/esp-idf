/*
 * SPDX-FileCopyrightText: 2019-2023, Arm Limited or its affiliates. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * SPDX-FileContributor: 2026 Espressif Systems (Shanghai) CO LTD
 */
#ifndef _TEST_DATA_H_
#define _TEST_DATA_H_

#include "psa/crypto_values.h"
#include "psa/initial_attestation.h"

/* Define TOKEN_SIZE and MAX_CHALLENGE_SIZE if not already defined */
#ifndef TOKEN_SIZE
#define TOKEN_SIZE PSA_INITIAL_ATTEST_MAX_TOKEN_SIZE
#endif

#ifndef MAX_CHALLENGE_SIZE
#define MAX_CHALLENGE_SIZE PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64
#endif

typedef struct {
    char                    test_desc[100];
    size_t                  challenge_size;
    size_t                  actual_challenge_size;
    size_t                  token_size;
    psa_status_t            expected_status;
} test_data;

static const test_data check1[] = {
    {
        "Test psa_initial_attestation_get_token with Challenge 32\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32, TOKEN_SIZE, PSA_SUCCESS
    },

    {
        "Test psa_initial_attestation_get_token with Challenge 48\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48, TOKEN_SIZE, PSA_SUCCESS
    },

    {
        "Test psa_initial_attestation_get_token with Challenge 64\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64, TOKEN_SIZE, PSA_SUCCESS
    },

    {
        "Test psa_initial_attestation_get_token with zero challenge size\n",
        0, 0, TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token with small challenge size\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1,
        TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token with invalid challenge size\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 + 1, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 + 1,
        TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token with large challenge size\n",
        MAX_CHALLENGE_SIZE + 1,  MAX_CHALLENGE_SIZE + 1, TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token with zero as token size\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32,
        0, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token with small token size\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32,
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1, PSA_ERROR_BUFFER_TOO_SMALL
    },
};

static const test_data check2[] = {
    {
        "Test psa_initial_attestation_get_token_size with Challenge 32\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32, TOKEN_SIZE, PSA_SUCCESS
    },

    {
        "Test psa_initial_attestation_get_token_size with Challenge 48\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_48, TOKEN_SIZE, PSA_SUCCESS
    },

    {
        "Test psa_initial_attestation_get_token_size with Challenge 64\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_64, TOKEN_SIZE, PSA_SUCCESS
    },

    {
        "Test psa_initial_attestation_get_token_size with zero challenge size\n",
        0, 0,
        TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token_size with small challenge size\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 - 1,
        TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token_size with invalid challenge size\n",
        PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 + 1, PSA_INITIAL_ATTEST_CHALLENGE_SIZE_32 + 1,
        TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },

    {
        "Test psa_initial_attestation_get_token_size with large challenge size\n",
        MAX_CHALLENGE_SIZE + 1,  MAX_CHALLENGE_SIZE + 1,
        TOKEN_SIZE, PSA_ERROR_INVALID_ARGUMENT
    },
};

#endif /* _TEST_DATA_H_ */
