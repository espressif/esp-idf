/**
 *
 * \brief Default mbedTLS configuration options for the ESP-TEE framework
 *
 *  This set of compile-time options may be used to enable
 *  or disable features selectively, and reduce the global
 *  memory footprint.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 * This set of compile-time options may be used to enable
 * or disable features selectively, and reduce the global
 * memory footprint.
 *
 * SPDX-FileCopyrightText: The Mbed TLS Contributors
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#pragma once

#define MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
#ifndef CONFIG_IDF_TARGET_LINUX
#undef MBEDTLS_PSA_BUILTIN_GET_ENTROPY
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
#endif // !CONFIG_IDF_TARGET_LINUX
#undef MBEDTLS_PSA_KEY_STORE_DYNAMIC

#define MBEDTLS_HAVE_TIME
#define MBEDTLS_PLATFORM_MS_TIME_ALT
#undef MBEDTLS_TIMING_C
#define MBEDTLS_PLATFORM_C

#if SOC_AES_SUPPORTED
#define ESP_AES_DRIVER_ENABLED
#define MBEDTLS_PSA_ACCEL_KEY_TYPE_AES
#endif
#define MBEDTLS_CIPHER_MODE_XTS

#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_ASN1_PARSE_C
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP384R1_SIGN
#define PSA_WANT_ECC_SECP_R1_384 1
#else
#undef PSA_WANT_ECC_SECP_R1_384
#endif
#define PSA_WANT_ECC_SECP_R1_256 1

#ifdef CONFIG_MBEDTLS_ECDSA_DETERMINISTIC
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA 1
#else
#undef PSA_WANT_ALG_DETERMINISTIC_ECDSA
#undef MBEDTLS_HMAC_DRBG_C
#endif

#if SOC_SHA_SUPPORTED
#define ESP_SHA_DRIVER_ENABLED
#if CONFIG_MBEDTLS_SHA1_C
#define MBEDTLS_PSA_ACCEL_ALG_SHA_1
#undef MBEDTLS_PSA_BUILTIN_ALG_SHA_1
#undef MBEDTLS_SHA1_C
#else
#undef PSA_WANT_ALG_SHA_1
#endif
#define MBEDTLS_PSA_ACCEL_ALG_SHA_224
#undef MBEDTLS_PSA_BUILTIN_ALG_SHA_224
#undef MBEDTLS_SHA224_C
#undef MBEDTLS_PSA_BUILTIN_ALG_SHA_256
#define MBEDTLS_PSA_ACCEL_ALG_SHA_256
#undef MBEDTLS_SHA256_C
#if SOC_SHA_SUPPORT_SHA512 && CONFIG_MBEDTLS_SHA512_C
#define MBEDTLS_PSA_ACCEL_ALG_SHA_384
#undef MBEDTLS_PSA_BUILTIN_ALG_SHA_384
#define MBEDTLS_PSA_ACCEL_ALG_SHA_512
#undef MBEDTLS_PSA_BUILTIN_ALG_SHA_512
#undef MBEDTLS_SHA384_C
#undef MBEDTLS_SHA512_C
#else
#undef PSA_WANT_ALG_SHA_384
#undef PSA_WANT_ALG_SHA_512
#undef MBEDTLS_SHA512_ALT
#endif
#if !SOC_HMAC_SUPPORTED
#define ESP_HMAC_TRANSPARENT_DRIVER_ENABLED
#define MBEDTLS_PSA_ACCEL_ALG_HMAC
#undef MBEDTLS_PSA_BUILTIN_ALG_HMAC
#endif
#endif

#if SOC_ECC_SUPPORTED
#define MBEDTLS_ECP_MUL_ALT
#define MBEDTLS_ECP_VERIFY_ALT
#endif

#if SOC_HMAC_SUPPORTED
#define ESP_HMAC_OPAQUE_DRIVER_ENABLED
#endif

#undef PSA_WANT_ECC_SECP_R1_192
#undef PSA_WANT_ECC_SECP_K1_192
#undef PSA_WANT_ECC_SECP_K1_256
#undef PSA_WANT_ECC_SECP_R1_224
#undef PSA_WANT_ECC_SECP_R1_521
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_256
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_384
#undef PSA_WANT_ECC_BRAINPOOL_P_R1_512
#undef PSA_WANT_ECC_MONTGOMERY_255
#undef PSA_WANT_ECC_MONTGOMERY_448

/* Disable unused cipher/algorithm types */
#undef PSA_WANT_KEY_TYPE_ARIA
#undef PSA_WANT_KEY_TYPE_CAMELLIA
#undef PSA_WANT_KEY_TYPE_DES
#undef PSA_WANT_ALG_RIPEMD160
#undef PSA_WANT_ALG_CHACHA20
#undef PSA_WANT_ALG_CHACHA20_POLY1305
#undef PSA_WANT_ALG_CCM
#undef PSA_WANT_ALG_CMAC

#define MBEDTLS_AES_ROM_TABLES
#if SOC_AES_SUPPORTED
#define MBEDTLS_AES_FEWER_TABLES
#endif

/* Disable unused hash algorithms */
#undef PSA_WANT_ALG_MD5
#undef PSA_WANT_ALG_SHA3_224
#undef PSA_WANT_ALG_SHA3_256
#undef PSA_WANT_ALG_SHA3_384
#undef PSA_WANT_ALG_SHA3_512

/* Disable RSA — not used by TEE */
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE
#undef PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY
#undef PSA_WANT_ALG_RSA_PKCS1V15_CRYPT
#undef PSA_WANT_ALG_RSA_PKCS1V15_SIGN
#undef PSA_WANT_ALG_RSA_OAEP
#undef PSA_WANT_ALG_RSA_PSS

/* Disable DH key exchange — not used by TEE */
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT
#undef PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE
#undef PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY
#undef PSA_WANT_ALG_FFDH

/* Disable TLS and other unused modules */
#undef MBEDTLS_DEBUG_C
#undef MBEDTLS_PSA_ITS_FILE_C
#undef MBEDTLS_PSA_CRYPTO_STORAGE_C
#undef MBEDTLS_SSL_CLI_C
#undef MBEDTLS_SSL_SRV_C

#undef PSA_WANT_ALG_TLS12_PRF
#undef PSA_WANT_ALG_PBKDF2_HMAC
#undef PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128

/* Disable Diffie-Hellman groups — TEE has no TLS or DH key exchange */
#undef PSA_WANT_DH_RFC7919_2048
#undef PSA_WANT_DH_RFC7919_3072
#undef PSA_WANT_DH_RFC7919_4096
#undef PSA_WANT_DH_RFC7919_6144
#undef PSA_WANT_DH_RFC7919_8192

/* Disable key derivation and TLS KDFs not used by TEE */
#undef PSA_WANT_ALG_HKDF
#undef PSA_WANT_ALG_HKDF_EXTRACT
#undef PSA_WANT_ALG_HKDF_EXPAND
#undef PSA_WANT_ALG_TLS12_PSK_TO_MS
#undef PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS

/* Disable EC-JPAKE — not used by TEE */
#undef PSA_WANT_ALG_JPAKE

/* Disable LMS/LMOTS hash-based signatures — not used by TEE */
#undef MBEDTLS_LMS_C

/* Disable self-test functions to save code size */
#undef MBEDTLS_SELF_TEST

/* TEE uses EXTERNAL_RNG, no need for CTR-DRBG */
#undef MBEDTLS_CTR_DRBG_C

/* Disable PEM/Base64 — TEE uses DER format */
#undef MBEDTLS_PEM_PARSE_C
#undef MBEDTLS_PEM_WRITE_C
#undef MBEDTLS_BASE64_C

/* Disable PK layer — TEE uses PSA API directly */
#undef MBEDTLS_PK_C
#undef MBEDTLS_PK_PARSE_C
#undef MBEDTLS_PK_WRITE_C

/* Disable NIST key wrapping and PKCS5 — not used by TEE */
#undef MBEDTLS_NIST_KW_C
#undef MBEDTLS_PKCS5_C

/* TEE has no filesystem */
#undef MBEDTLS_FS_IO

/* ESP-TEE is single threaded so we can disable threading in mbedTLS */
#undef MBEDTLS_THREADING_C
