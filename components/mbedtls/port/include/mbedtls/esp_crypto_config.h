/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#define MBEDTLS_PLATFORM_GET_ENTROPY_ALT
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG

// #ifdef CONFIG_MBEDTLS_RIPEMD160_C
// #define MBEDTLS_RIPEMD160_C
// #else
// #undef MBEDTLS_RIPEMD160_C
// #undef PSA_WANT_ALG_RIPEMD160
// #endif

// #if CONFIG_MBEDTLS_SHA1_C
// #define MBEDTLS_SHA1_C
// #else
// #undef MBEDTLS_SHA1_C
// #undef PSA_WANT_ALG_SHA_1
// #endif

// #if CONFIG_MBEDTLS_SHA384_C
// #define MBEDTLS_SHA384_C
// #else
// #undef MBEDTLS_SHA384_C
// #undef PSA_WANT_ALG_SHA_384
// #endif

// #if CONFIG_MBEDTLS_SHA512_C
// #define MBEDTLS_SHA512_C
// #else
// #undef MBEDTLS_SHA512_C
// #undef PSA_WANT_ALG_SHA_512
// #endif

// #ifdef CONFIG_MBEDTLS_CAMELLIA_C
// #error "MBEDTLS_CAMELLIA_C defined in config"
// #define MBEDTLS_CAMELLIA_C
// #else
// #undef MBEDTLS_CAMELLIA_C
// #undef PSA_WANT_KEY_TYPE_CAMELLIA
// #endif

// #ifdef CONFIG_MBEDTLS_MD5_C
// #define MBEDTLS_MD5_C
// #else
// #undef MBEDTLS_MD5_C
// #undef PSA_WANT_ALG_MD5
// #endif

// #ifdef CONFIG_MBEDTLS_ECP_DP_SECP384R1_ENABLED
// #define MBEDTLS_ECP_DP_SECP384R1_ENABLED
// #else
// #undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
// #undef PSA_WANT_ECC_SECP_R1_384
// #endif
