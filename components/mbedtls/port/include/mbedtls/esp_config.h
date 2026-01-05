/**
 *
 * \brief Default mbedTLS configuration options for ESP-IDF
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
#ifndef ESP_CONFIG_H
#define ESP_CONFIG_H

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "sdkconfig.h"
#if (defined(MBEDTLS_MAJOR_VERSION) && (MBEDTLS_MAJOR_VERSION < 4))
#include "mbedtls/mbedtls_config.h"
#endif // MBEDTLS_MAJOR_VERSION < 4
#include "soc/soc_caps.h"


#ifndef CONFIG_IDF_TARGET_LINUX
#undef MBEDTLS_PSA_BUILTIN_GET_ENTROPY
#define MBEDTLS_PSA_DRIVER_GET_ENTROPY
#define MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG
#endif // !CONFIG_IDF_TARGET_LINUX

/**
 * \def MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS
 *
 * Assume all buffers passed to PSA functions are owned exclusively by the
 * PSA function and are not stored in shared memory.
 *
 * This option may be enabled if all buffers passed to any PSA function reside
 * in memory that is accessible only to the PSA function during its execution.
 *
 * This option MUST be disabled whenever buffer arguments are in memory shared
 * with an untrusted party, for example where arguments to PSA calls are passed
 * across a trust boundary.
 *
 * \note Enabling this option reduces memory usage and code size.
 *
 * \note Enabling this option causes overlap of input and output buffers
 *       not to be supported by PSA functions.
 */
#define MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS

#define PSA_WANT_ECC_SECP_R1_192                1

/**
 * \name SECTION: System support
 *
 * This section sets system specific settings.
 * \{
 */
/**
 * \def MBEDTLS_HAVE_TIME
 *
 * System has time.h and time().
 * The time does not need to be correct, only time differences are used,
 * by contrast with MBEDTLS_HAVE_TIME_DATE
 *
 * Defining MBEDTLS_HAVE_TIME allows you to specify MBEDTLS_PLATFORM_TIME_ALT,
 * MBEDTLS_PLATFORM_TIME_MACRO, MBEDTLS_PLATFORM_TIME_TYPE_MACRO and
 * MBEDTLS_PLATFORM_STD_TIME.
 *
 * Comment if your system does not support time functions.
 *
 * \note If MBEDTLS_TIMING_C is set - to enable the semi-portable timing
 *       interface - timing.c will include time.h on suitable platforms
 *       regardless of the setting of MBEDTLS_HAVE_TIME, unless
 *       MBEDTLS_TIMING_ALT is used. See timing.c for more information.
 */
#ifdef CONFIG_MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME
/**
 * \def MBEDTLS_PLATFORM_MS_TIME_ALT
 *
 * Define platform specific function to get time since boot up in milliseconds.
 */
#define MBEDTLS_PLATFORM_MS_TIME_ALT
#else
#undef MBEDTLS_HAVE_TIME
#undef MBEDTLS_PLATFORM_MS_TIME_ALT
#endif

/**
 * \def MBEDTLS_HAVE_TIME_DATE
 *
 * System has time.h and time(), gmtime() and the clock is correct.
 * The time needs to be correct (not necessarily very accurate, but at least
 * the date should be correct). This is used to verify the validity period of
 * X.509 certificates.
 *
 * Comment if your system does not have a correct clock.
 */
#ifdef CONFIG_MBEDTLS_HAVE_TIME_DATE
#define MBEDTLS_HAVE_TIME_DATE
#else
#undef MBEDTLS_HAVE_TIME_DATE
#endif


/**
 * \def MBEDTLS_PLATFORM_TIME_ALT
 *
 * mbed TLS will provide a function "mbedtls_platform_set_time()"
 * that allows you to set an alternative time function pointer.
 *
 * All these define require MBEDTLS_PLATFORM_C to be defined!
 *
 * \warning MBEDTLS_PLATFORM_TIME_ALT cannot be defined at the same time as
 * MBEDTLS_PLATFORM_TIME_MACRO!
 *
 * Requires: MBEDTLS_PLATFORM_TIME_ALT requires MBEDTLS_HAVE_TIME
 */
#ifdef CONFIG_MBEDTLS_PLATFORM_TIME_ALT
#define MBEDTLS_PLATFORM_TIME_ALT
#else
#undef MBEDTLS_PLATFORM_TIME_ALT
#endif

/**
 * \def MBEDTLS_PLATFORM_MEMORY
 *
 * Enable the memory allocation layer.
 *
 * By default mbed TLS uses the system-provided calloc() and free().
 * This allows different allocators (self-implemented or provided) to be
 * provided to the platform abstraction layer.
 *
 * Enabling MBEDTLS_PLATFORM_MEMORY without the
 * MBEDTLS_PLATFORM_{FREE,CALLOC}_MACROs will provide
 * "mbedtls_platform_set_calloc_free()" allowing you to set an alternative calloc() and
 * free() function pointer at runtime.
 *
 * Enabling MBEDTLS_PLATFORM_MEMORY and specifying
 * MBEDTLS_PLATFORM_{CALLOC,FREE}_MACROs will allow you to specify the
 * alternate function at compile time.
 *
 * Requires: MBEDTLS_PLATFORM_C
 *
 * Enable this layer to allow use of alternative memory allocators.
 */
#define MBEDTLS_PLATFORM_MEMORY

/** Override calloc(), free() except for case where memory allocation scheme is not set to custom */
#ifndef CONFIG_MBEDTLS_CUSTOM_MEM_ALLOC
#include "esp_mem.h"
#undef MBEDTLS_PLATFORM_STD_CALLOC
#undef MBEDTLS_PLATFORM_STD_FREE
#define MBEDTLS_PLATFORM_STD_CALLOC		esp_mbedtls_mem_calloc
#define MBEDTLS_PLATFORM_STD_FREE		esp_mbedtls_mem_free
#endif

/* \} name SECTION: System support */

/**
 * \name SECTION: mbed TLS feature support
 *
 * This section sets support for features that are or are not needed
 * within the modules that are enabled.
 * \{
 */

#ifdef CONFIG_MBEDTLS_HARDWARE_AES
#ifdef CONFIG_MBEDTLS_GCM_SUPPORT_NON_AES_CIPHER
    /* Prefer hardware and fallback to software */
    #define MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK
#else
    #undef MBEDTLS_GCM_NON_AES_CIPHER_SOFT_FALLBACK
#endif
#endif

/* MBEDTLS_SHAxx_ALT to enable hardware SHA support
   with software fallback.
*/
#ifdef CONFIG_MBEDTLS_HARDWARE_SHA
#define MBEDTLS_PSA_ACCEL_ALG_SHA_1
#define MBEDTLS_PSA_ACCEL_ALG_SHA_224
#define MBEDTLS_PSA_ACCEL_ALG_SHA_256
#if SOC_SHA_SUPPORT_SHA512
#define MBEDTLS_PSA_ACCEL_ALG_SHA_384
#define MBEDTLS_PSA_ACCEL_ALG_SHA_512
#endif
#if SOC_SHA_SUPPORT_SHA512
#else
#undef MBEDTLS_SHA512_ALT
#endif

#else
#undef MBEDTLS_SHA1_ALT
#undef MBEDTLS_SHA256_ALT
#undef MBEDTLS_SHA512_ALT
#endif

/* MBEDTLS_MDx_ALT to enable ROM MD support
   with software fallback.
*/
#ifdef CONFIG_MBEDTLS_ROM_MD5
#define MBEDTLS_PSA_ACCEL_ALG_MD5
#undef MBEDTLS_PSA_BUILTIN_ALG_MD5
#endif

/* The following MPI (bignum) functions have hardware support.
 * Uncommenting these macros will use the hardware-accelerated
 * implementations.
 */
#ifdef CONFIG_MBEDTLS_HARDWARE_MPI
#ifdef CONFIG_MBEDTLS_LARGE_KEY_SOFTWARE_MPI
    /* Prefer hardware and fallback to software */
    #define MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK
    #define MBEDTLS_MPI_EXP_MOD_ALT
#else
    /* Hardware only mode */
    #define MBEDTLS_MPI_EXP_MOD_ALT
#endif
#define MBEDTLS_MPI_MUL_MPI_ALT
#else
#undef MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK
#undef MBEDTLS_MPI_EXP_MOD_ALT
#undef MBEDTLS_MPI_MUL_MPI_ALT
#endif

#ifdef CONFIG_MBEDTLS_ATCA_HW_ECDSA_SIGN
#define MBEDTLS_ECDSA_SIGN_ALT
#endif

#ifdef CONFIG_MBEDTLS_ATCA_HW_ECDSA_VERIFY
#define MBEDTLS_ECDSA_VERIFY_ALT
#endif

#ifdef CONFIG_MBEDTLS_HARDWARE_ECC
#ifdef CONFIG_MBEDTLS_ECC_OTHER_CURVES_SOFT_FALLBACK
    /* Use hardware accelerator for SECP192R1 and SECP256R1 curves,
     * software implementation for rest of the curves
     */
    #define MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK
    #define MBEDTLS_ECP_VERIFY_ALT_SOFT_FALLBACK
#else
    /* Only hardware accelerator support */
    #define MBEDTLS_ECP_MUL_ALT
    #define MBEDTLS_ECP_VERIFY_ALT
#endif

#else
#undef MBEDTLS_ECP_MUL_ALT
#undef MBEDTLS_ECP_MUL_ALT_SOFT_FALLBACK
#undef MBEDTLS_ECP_VERIFY_ALT
#undef MBEDTLS_ECP_VERIFY_ALT_SOFT_FALLBACK
#endif

/**
 * \def MBEDTLS_AES_ROM_TABLES
 *
 * Store the AES tables in ROM.
 *
 * Uncomment this macro to store the AES tables in ROM.
 */
#ifdef CONFIG_MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_AES_ROM_TABLES
#else
#undef MBEDTLS_AES_ROM_TABLES
#endif

/**
 * \def MBEDTLS_AES_FEWER_TABLES
 *
 * Use fewer tables for AES.
 *
 * Uncomment this macro to store fewer tables for AES
 * in ROM or RAM. The values are computed at runtime.
 *
 */

#ifdef CONFIG_MBEDTLS_AES_FEWER_TABLES
#define MBEDTLS_AES_FEWER_TABLES
#else
#undef MBEDTLS_AES_FEWER_TABLES
#endif

/**
 * \def MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
 *
 * Enable support for AES with only 128-bit key length. This disables
 * support for 192-bit and 256-bit key lengths.
 *
 * Uncommenting this macro reduces the size of AES code
 */

#ifdef CONFIG_MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
#define MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
#else
#undef MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH
#endif

/**
 * \def MBEDTLS_CAMELLIA_SMALL_MEMORY
 *
 * Enable small memory usage for Camellia cipher.
 */
#ifdef CONFIG_MBEDTLS_CAMELLIA_SMALL_MEMORY
#define MBEDTLS_CAMELLIA_SMALL_MEMORY
#else
#undef MBEDTLS_CAMELLIA_SMALL_MEMORY
#endif

/**
 * \def MBEDTLS_CIPHER_MODE_CBC
 *
 * Enable Cipher Block Chaining mode (CBC) for symmetric ciphers.
 */
#ifdef CONFIG_MBEDTLS_CIPHER_MODE_CBC
#define PSA_WANT_ALG_CBC_NO_PADDING 1
#define PSA_WANT_ALG_CBC_PKCS7 1
#else
#undef PSA_WANT_ALG_CBC_NO_PADDING
#undef PSA_WANT_ALG_CBC_PKCS7
#endif

/**
 * \def MBEDTLS_CIPHER_MODE_CFB
 *
 * Enable Cipher Feedback mode (CFB) for symmetric ciphers.
 */
#ifdef CONFIG_MBEDTLS_CIPHER_MODE_CFB
#define PSA_WANT_ALG_CFB 1
#else
#undef PSA_WANT_ALG_CFB
#endif

/**
 * \def MBEDTLS_CIPHER_MODE_CTR
 *
 * Enable Counter Block Cipher mode (CTR) for symmetric ciphers.
 */
#ifdef CONFIG_MBEDTLS_CIPHER_MODE_CTR
#define PSA_WANT_ALG_CTR 1
#else
#undef PSA_WANT_ALG_CTR
#endif
/**
 * \def MBEDTLS_CIPHER_MODE_OFB
 *
 * Enable Output Feedback mode (OFB) for symmetric ciphers.
 */
#ifdef CONFIG_MBEDTLS_CIPHER_MODE_OFB
#define PSA_WANT_ALG_OFB 1
#else
#undef PSA_WANT_ALG_OFB
#endif

/**
 * \def MBEDTLS_CIPHER_MODE_XTS
 *
 * Enable Xor-encrypt-xor with ciphertext stealing mode (XTS) for AES.
 */
#ifdef CONFIG_MBEDTLS_CIPHER_MODE_XTS
#define MBEDTLS_CIPHER_MODE_XTS
#else
#undef MBEDTLS_CIPHER_MODE_XTS
#endif

/**
 * \def MBEDTLS_ECP_RESTARTABLE
 *
 * Enable "non-blocking" ECC operations that can return early and be resumed.
 *
 * This allows various functions to pause by returning
 * #MBEDTLS_ERR_ECP_IN_PROGRESS (or, for functions in the SSL module,
 * #MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) and then be called later again in
 * order to further progress and eventually complete their operation. This is
 * controlled through mbedtls_ecp_set_max_ops() which limits the maximum
 * number of ECC operations a function may perform before pausing; see
 * mbedtls_ecp_set_max_ops() for more information.
 *
 * This is useful in non-threaded environments if you want to avoid blocking
 * for too long on ECC (and, hence, X.509 or SSL/TLS) operations.
 *
 * This option:
 * - Adds xxx_restartable() variants of existing operations in the
 *   following modules, with corresponding restart context types:
 *   - ECP (for Short Weierstrass curves only): scalar multiplication (mul),
 *     linear combination (muladd);
 *   - ECDSA: signature generation & verification;
 *   - PK: signature generation & verification;
 *   - X509: certificate chain verification.
 * - Adds mbedtls_ecdh_enable_restart() in the ECDH module.
 * - Changes the behaviour of TLS 1.2 clients (not servers) when using the
 *   ECDHE-ECDSA key exchange (not other key exchanges) to make all ECC
 *   computations restartable:
 *   - ECDH operations from the key exchange, only for Short Weierstrass
 *     curves, only when MBEDTLS_USE_PSA_CRYPTO is not enabled.
 *   - verification of the server's key exchange signature;
 *   - verification of the server's certificate chain;
 *   - generation of the client's signature if client authentication is used,
 *     with an ECC key/certificate.
 *
 * \note  In the cases above, the usual SSL/TLS functions, such as
 *        mbedtls_ssl_handshake(), can now return
 *        MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS.
 *
 * \note  This option only works with the default software implementation of
 *        elliptic curve functionality. It is incompatible with
 *        MBEDTLS_ECP_ALT, MBEDTLS_ECDH_XXX_ALT, MBEDTLS_ECDSA_XXX_ALT.
 *
 * Requires: MBEDTLS_ECP_C
 *
 * Uncomment this macro to enable restartable ECC computations.
 */
#ifdef CONFIG_MBEDTLS_ECP_RESTARTABLE
#define MBEDTLS_ECP_RESTARTABLE
#else
#undef MBEDTLS_ECP_RESTARTABLE
#endif

/**
 * \def MBEDTLS_ECDH_LEGACY_CONTEXT
 *
 * Use a backward compatible ECDH context.
 *
 * Mbed TLS supports two formats for ECDH contexts (#mbedtls_ecdh_context
 * defined in `ecdh.h`). For most applications, the choice of format makes
 * no difference, since all library functions can work with either format,
 * except that the new format is incompatible with MBEDTLS_ECP_RESTARTABLE.

 * The new format used when this option is disabled is smaller
 * (56 bytes on a 32-bit platform). In future versions of the library, it
 * will support alternative implementations of ECDH operations.
 * The new format is incompatible with applications that access
 * context fields directly and with restartable ECP operations.
 *
 * Define this macro if you enable MBEDTLS_ECP_RESTARTABLE or if you
 * want to access ECDH context fields directly. Otherwise you should
 * comment out this macro definition.
 *
 * This option has no effect if #MBEDTLS_ECDH_C is not enabled.
 *
 * \note This configuration option is experimental. Future versions of the
 *       library may modify the way the ECDH context layout is configured
 *       and may modify the layout of the new context type.
 */
#ifdef CONFIG_MBEDTLS_ECDH_LEGACY_CONTEXT
#define MBEDTLS_ECDH_LEGACY_CONTEXT
#endif

/**
 * \def MBEDTLS_CMAC_C
 *
 * Enable the CMAC (Cipher-based Message Authentication Code) mode for block
 * ciphers.
 *
 * \note When #MBEDTLS_CMAC_ALT is active, meaning that the underlying
 *       implementation of the CMAC algorithm is provided by an alternate
 *       implementation, that alternate implementation may opt to not support
 *       AES-192 or 3DES as underlying block ciphers for the CMAC operation.
 *
 * Module:  library/cmac.c
 *
 * Requires: MBEDTLS_AES_C or MBEDTLS_DES_C
 *
 */
#ifdef CONFIG_MBEDTLS_CMAC_C
#define PSA_WANT_ALG_CMAC 1
#else
#ifdef CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL
/* The mbedtls present in ROM is built with the MBEDTLS_CMAC_C symbol being enabled,
 * thus when using the mbedtls from ROM, CONFIG_MBEDTLS_CMAC_C needs to be enabled.
 */
#error "CONFIG_MBEDTLS_CMAC_C cannot be disabled when CONFIG_MBEDTLS_USE_CRYPTO_ROM_IMPL is enabled"
#endif
#undef PSA_WANT_ALG_CMAC
#endif

/**
 * \def MBEDTLS_ECP_DP_SECP192R1_ENABLED
 *
 * MBEDTLS_ECP_XXXX_ENABLED: Enables specific curves within the Elliptic Curve
 * module.  By default all supported curves are enabled.
 *
 * Comment macros to disable the curve and functions for it
 */
/* Short Weierstrass curves (supporting ECP, ECDH, ECDSA) */
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP256R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
#undef PSA_WANT_ECC_SECP_R1_384
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP256K1_ENABLED
#define MBEDTLS_ECP_DP_SECP256K1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP256K1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_BP256R1_ENABLED
#define MBEDTLS_ECP_DP_BP256R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_BP256R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_BP384R1_ENABLED
#define MBEDTLS_ECP_DP_BP384R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_BP384R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_BP512R1_ENABLED
#define MBEDTLS_ECP_DP_BP512R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_BP512R1_ENABLED
#endif
/* Montgomery curves (supporting ECP) */
#ifdef CONFIG_MBEDTLS_ECP_DP_CURVE25519_ENABLED
#define MBEDTLS_ECP_DP_CURVE25519_ENABLED
#else
#undef MBEDTLS_ECP_DP_CURVE25519_ENABLED
#endif
#ifdef MBEDTLS_ECP_DP_CURVE448_ENABLED
#undef MBEDTLS_ECP_DP_CURVE448_ENABLED
#endif

/**
 * \def MBEDTLS_ECP_NIST_OPTIM
 *
 * Enable specific 'modulo p' routines for each NIST prime.
 * Depending on the prime and architecture, makes operations 4 to 8 times
 * faster on the corresponding curve.
 *
 * Comment this macro to disable NIST curves optimisation.
 */
#ifdef CONFIG_MBEDTLS_ECP_NIST_OPTIM
#define MBEDTLS_ECP_NIST_OPTIM
#else
#undef MBEDTLS_ECP_NIST_OPTIM
#endif

/**
 * \def MBEDTLS_ECP_FIXED_POINT_OPTIM
 *
 * Enable speed up fixed-point multiplication.
 *
 * Comment this macro to disable FIXED POINT curves optimisation.
 */
#ifdef CONFIG_MBEDTLS_ECP_FIXED_POINT_OPTIM
#define MBEDTLS_ECP_FIXED_POINT_OPTIM 1
#else
#define MBEDTLS_ECP_FIXED_POINT_OPTIM 0
#endif

/**
 * \def MBEDTLS_ECDSA_DETERMINISTIC
 *
 * Enable deterministic ECDSA (RFC 6979).
 * Standard ECDSA is "fragile" in the sense that lack of entropy when signing
 * may result in a compromise of the long-term signing key. This is avoided by
 * the deterministic variant.
 *
 * Requires: MBEDTLS_HMAC_DRBG_C, MBEDTLS_ECDSA_C
 *
 * Comment this macro to disable deterministic ECDSA.
 */
#ifdef CONFIG_MBEDTLS_ECDSA_DETERMINISTIC
#define PSA_WANT_ALG_DETERMINISTIC_ECDSA 1
#else
#undef PSA_WANT_ALG_DETERMINISTIC_ECDSA
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
 *
 * Enable the PSK based ciphersuite modes in SSL / TLS.
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_PSK_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_PSK
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
 *
 * Enable the ECDHE-PSK based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_PSK
#define MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
 *
 * Enable the ECDHE-RSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_RSA_C, MBEDTLS_PKCS1_V15,
 *           MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_RSA
#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
 *
 * Enable the ECDHE-ECDSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_ECDSA_C, MBEDTLS_X509_CRT_PARSE_C,
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA
#define MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
 *
 * Enable the ECJPAKE based ciphersuite modes in SSL / TLS.
 *
 * \warning This is currently experimental. EC J-PAKE support is based on the
 * Thread v1.0.0 specification; incompatible changes to the specification
 * might still happen. For this reason, this is disabled by default.
 *
 * Requires: MBEDTLS_ECJPAKE_C
 *           MBEDTLS_SHA256_C
 *           MBEDTLS_ECP_DP_SECP256R1_ENABLED
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECJPAKE_WITH_AES_128_CCM_8
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_ECJPAKE
#define MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED
#endif

/**
 * \def MBEDTLS_PK_PARSE_EC_EXTENDED
 *
 * Enhance support for reading EC keys using variants of SEC1 not allowed by
 * RFC 5915 and RFC 5480.
 *
 * Currently this means parsing the SpecifiedECDomain choice of EC
 * parameters (only known groups are supported, not arbitrary domains, to
 * avoid validation issues).
 *
 * Disable if you only need to support RFC 5915 + 5480 key formats.
 */
#ifdef CONFIG_MBEDTLS_PK_PARSE_EC_EXTENDED
#define MBEDTLS_PK_PARSE_EC_EXTENDED
#else
#undef MBEDTLS_PK_PARSE_EC_EXTENDED
#endif

/**
 * \def MBEDTLS_PK_PARSE_EC_COMPRESSED
 *
 * Enable the support for parsing public keys of type Short Weierstrass
 * (MBEDTLS_ECP_DP_SECP_XXX and MBEDTLS_ECP_DP_BP_XXX) which are using the
 * compressed point format. This parsing is done through ECP module's functions.
 *
 * \note As explained in the description of MBEDTLS_ECP_PF_COMPRESSED (in ecp.h)
 *       the only unsupported curves are MBEDTLS_ECP_DP_SECP224R1 and
 *       MBEDTLS_ECP_DP_SECP224K1.
 */
#ifdef CONFIG_MBEDTLS_PK_PARSE_EC_COMPRESSED
#define MBEDTLS_PK_PARSE_EC_COMPRESSED
#else
#undef MBEDTLS_PK_PARSE_EC_COMPRESSED
#endif

/**
 * \def MBEDTLS_ERROR_STRERROR_DUMMY
 *
 * Enable a dummy error function to make use of mbedtls_strerror() in
 * third party libraries easier when MBEDTLS_ERROR_C is disabled
 * (no effect when MBEDTLS_ERROR_C is enabled).
 *
 * You can safely disable this if MBEDTLS_ERROR_C is enabled, or if you're
 * not using mbedtls_strerror() or error_strerror() in your application.
 *
 * Disable if you run into name conflicts and want to really remove the
 * mbedtls_strerror()
 */
#ifdef CONFIG_MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_ERROR_STRERROR_DUMMY
#else
#undef MBEDTLS_ERROR_STRERROR_DUMMY
#endif

/**
 * \def MBEDTLS_FS_IO
 *
 * Enable functions that use the filesystem.
 */
#if CONFIG_MBEDTLS_FS_IO
#define MBEDTLS_FS_IO
#else
#undef MBEDTLS_FS_IO
#undef MBEDTLS_PSA_ITS_FILE_C
#undef MBEDTLS_PSA_CRYPTO_STORAGE_C
#endif

#ifndef CONFIG_IDF_TARGET_LINUX

/**
 * \def MBEDTLS_PSA_ITS_FILE_C
 *
 * ESP-IDF: PSA Internal Trusted Storage (ITS) implementation.
 *
 * ESP-IDF does NOT use the file-based implementation (MBEDTLS_PSA_ITS_FILE_C)
 * when the ESP-IDF NVS-based implementation is available.
 * Instead, ESP-IDF provides its own NVS (Non-Volatile Storage) based implementation
 * in port/psa_crypto_storage/esp_psa_its.c
 *
 * If ESP_PSA_ITS_AVAILABLE is defined, it means the ESP-IDF NVS-based implementation
 * is available and we should undefine MBEDTLS_PSA_ITS_FILE_C to use it.
 * Otherwise, keep MBEDTLS_PSA_ITS_FILE_C defined to use the file-based implementation.
 *
 */
#ifdef ESP_PSA_ITS_AVAILABLE
#undef MBEDTLS_PSA_ITS_FILE_C
#endif
#endif // !CONFIG_IDF_TARGET_LINUX

/**
 * \def MBEDTLS_ENTROPY_FORCE_SHA256
 *
 * Force the entropy accumulator to use a SHA-256 accumulator instead of the
 * default SHA-512 based one (if both are available).
 *
 * Requires: MBEDTLS_SHA256_C
 *
 * On 32-bit systems SHA-256 can be much faster than SHA-512. Use this option
 * if you have performance concerns.
 *
 * This option is only useful if both MBEDTLS_SHA256_C and
 * MBEDTLS_SHA512_C are defined. Otherwise the available hash module is used.
 */
#ifdef CONFIG_MBEDTLS_ENTROPY_FORCE_SHA256
#define MBEDTLS_ENTROPY_FORCE_SHA256
#else
#undef MBEDTLS_ENTROPY_FORCE_SHA256
#endif

/**
 * \def MBEDTLS_PKCS1_V15
 *
 * Enable support for PKCS#1 v1.5 encoding.
 *
 * Requires: MBEDTLS_RSA_C
 *
 * This enables support for PKCS#1 v1.5 operations.
 */
#ifdef CONFIG_MBEDTLS_PKCS1_V15
#define PSA_WANT_ALG_RSA_PKCS1V15_CRYPT 1
#define PSA_WANT_ALG_RSA_PKCS1V15_SIGN 1
#else
#undef PSA_WANT_ALG_RSA_PKCS1V15_CRYPT
#undef PSA_WANT_ALG_RSA_PKCS1V15_SIGN
#endif

/**
 * \def MBEDTLS_PKCS1_V21
 *
 * Enable support for PKCS#1 v2.1 encoding.
 *
 * Requires: MBEDTLS_MD_C, MBEDTLS_RSA_C
 *
 * This enables support for RSAES-OAEP and RSASSA-PSS operations.
 */
#ifdef CONFIG_MBEDTLS_PKCS1_V21
#define PSA_WANT_ALG_RSA_OAEP 1
#else
#undef PSA_WANT_ALG_RSA_OAEP
#endif

/**
 * \def MBEDTLS_SELF_TEST
 *
 * Enable the checkup functions (*_self_test).
 */
#ifdef CONFIG_MBEDTLS_SELF_TEST
#define MBEDTLS_SELF_TEST
#else
#undef MBEDTLS_SELF_TEST
#endif

/**
 * \def MBEDTLS_SHA256_SMALLER
 *
 * Enable an implementation of SHA-256 that has lower ROM footprint but also
 * lower performance.
 *
 * The default implementation is meant to be a reasonable compromise between
 * performance and size. This version optimizes more aggressively for size at
 * the expense of performance. Eg on Cortex-M4 it reduces the size of
 * mbedtls_sha256_process() from ~2KB to ~0.5KB for a performance hit of about
 * 30%.
 *
 * Uncomment to enable the smaller implementation of SHA256.
 */
#ifdef CONFIG_MBEDTLS_SHA256_SMALLER
#define MBEDTLS_SHA256_SMALLER
#else
#undef MBEDTLS_SHA256_SMALLER
#endif

/**
 * \def MBEDTLS_SHA512_SMALLER
 * Enable an implementation of SHA-512 that has lower ROM footprint but also
 * lower performance.
 *
 * Uncomment to enable the smaller implementation of SHA512.
 */
#ifdef CONFIG_MBEDTLS_SHA512_SMALLER
#define MBEDTLS_SHA512_SMALLER
#else
#undef MBEDTLS_SHA512_SMALLER
#endif

/**
 * \def MBEDTLS_SSL_ALL_ALERT_MESSAGES
 *
 * Enable sending of alert messages in case of encountered errors as per RFC.
 * If you choose not to send the alert messages, mbed TLS can still communicate
 * with other servers, only debugging of failures is harder.
 *
 * The advantage of not sending alert messages, is that no information is given
 * about reasons for failures thus preventing adversaries of gaining intel.
 *
 * Enable sending of all alert messages
 */
#ifdef CONFIG_MBEDTLS_SSL_ALL_ALERT_MESSAGES
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES
#else
#undef MBEDTLS_SSL_ALL_ALERT_MESSAGES
#endif

/**
 * \def MBEDTLS_SSL_DTLS_CONNECTION_ID
 *
 * Enable support for the DTLS Connection ID (CID) extension,
 * which allows to identify DTLS connections across changes
 * in the underlying transport. The CID functionality is described
 * in RFC 9146.
 *
 * Setting this option enables the SSL APIs `mbedtls_ssl_set_cid()`,
 * mbedtls_ssl_get_own_cid()`, `mbedtls_ssl_get_peer_cid()` and
 * `mbedtls_ssl_conf_cid()`. See the corresponding documentation for
 * more information.
 *
 * The maximum lengths of outgoing and incoming CIDs can be configured
 * through the options
 * - MBEDTLS_SSL_CID_OUT_LEN_MAX
 * - MBEDTLS_SSL_CID_IN_LEN_MAX.
 *
 * Requires: MBEDTLS_SSL_PROTO_DTLS
 *
 * Uncomment to enable the Connection ID extension.
 */
#ifdef CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_DTLS_CONNECTION_ID
#else
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID
#endif

/**
 * \def MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT
 *
 * Defines whether RFC 9146 (default) or the legacy version
 * (version draft-ietf-tls-dtls-connection-id-05,
 * https://tools.ietf.org/html/draft-ietf-tls-dtls-connection-id-05)
 * is used.
 *
 * Set the value to 0 for the standard version, and
 * 1 for the legacy draft version.
 *
 * \deprecated Support for the legacy version of the DTLS
 *             Connection ID feature is deprecated. Please
 *             switch to the standardized version defined
 *             in RFC 9146 enabled by utilizing
 *             MBEDTLS_SSL_DTLS_CONNECTION_ID without use
 *             of MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT.
 *
 * Requires: MBEDTLS_SSL_DTLS_CONNECTION_ID
 */
#undef MBEDTLS_SSL_DTLS_CONNECTION_ID_COMPAT

/**
 * \def MBEDTLS_SSL_CONTEXT_SERIALIZATION
 *
 * Enable serialization of the TLS context structures, through use of the
 * functions mbedtls_ssl_context_save() and mbedtls_ssl_context_load().
 *
 * This pair of functions allows one side of a connection to serialize the
 * context associated with the connection, then free or reuse that context
 * while the serialized state is persisted elsewhere, and finally deserialize
 * that state to a live context for resuming read/write operations on the
 * connection. From a protocol perspective, the state of the connection is
 * unaffected, in particular this is entirely transparent to the peer.
 *
 * Note: this is distinct from TLS session resumption, which is part of the
 * protocol and fully visible by the peer. TLS session resumption enables
 * establishing new connections associated to a saved session with shorter,
 * lighter handshakes, while context serialization is a local optimization in
 * handling a single, potentially long-lived connection.
 *
 * Enabling these APIs makes some SSL structures larger, as 64 extra bytes are
 * saved after the handshake to allow for more efficient serialization, so if
 * you don't need this feature you'll save RAM by disabling it.
 *
 * Requires: MBEDTLS_GCM_C or MBEDTLS_CCM_C or MBEDTLS_CHACHAPOLY_C
 *
 * Comment to disable the context serialization APIs.
 */
#ifdef CONFIG_MBEDTLS_SSL_CONTEXT_SERIALIZATION
#define MBEDTLS_SSL_CONTEXT_SERIALIZATION
#else
#undef MBEDTLS_SSL_CONTEXT_SERIALIZATION
#endif

/** \def MBEDTLS_SSL_ENCRYPT_THEN_MAC
 *
 * Enable support for Encrypt-then-MAC, RFC 7366.
 *
 * This allows peers that both support it to use a more robust protection for
 * ciphersuites using CBC, providing deep resistance against timing attacks
 * on the padding or underlying cipher.
 *
 * This only affects CBC ciphersuites, and is useless if none is defined.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this macro to disable support for Encrypt-then-MAC
 */
#ifdef CONFIG_MBEDTLS_TLS_ENABLED
#define MBEDTLS_SSL_ENCRYPT_THEN_MAC
#else
#undef MBEDTLS_SSL_ENCRYPT_THEN_MAC
#endif

/** \def MBEDTLS_SSL_EXTENDED_MASTER_SECRET
 *
 * Enable support for RFC 7627: Session Hash and Extended Master Secret
 * Extension.
 *
 * This was introduced as "the proper fix" to the Triple Handshake family of
 * attacks, but it is recommended to always use it (even if you disable
 * renegotiation), since it actually fixes a more fundamental issue in the
 * original SSL/TLS design, and has implications beyond Triple Handshake.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this macro to disable support for Extended Master Secret.
 */
#ifdef CONFIG_MBEDTLS_TLS_ENABLED
#define MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#else
#undef MBEDTLS_SSL_EXTENDED_MASTER_SECRET
#endif

/**
 * \def MBEDTLS_SSL_FALLBACK_SCSV
 *
 * Enable support for RFC 7507: Fallback Signaling Cipher Suite Value (SCSV)
 * for Preventing Protocol Downgrade Attacks.
 *
 * For servers, it is recommended to always enable this, unless you support
 * only one version of TLS, or know for sure that none of your clients
 * implements a fallback strategy.
 *
 * For clients, you only need this if you're using a fallback strategy, which
 * is not recommended in the first place, unless you absolutely need it to
 * interoperate with buggy (version-intolerant) servers.
 *
 * Comment this macro to disable support for FALLBACK_SCSV
 */
#define MBEDTLS_SSL_FALLBACK_SCSV

/**
 * \def MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
 *
 * This option controls the availability of the API mbedtls_ssl_get_peer_cert()
 * giving access to the peer's certificate after completion of the handshake.
 *
 * Unless you need mbedtls_ssl_peer_cert() in your application, it is
 * recommended to disable this option for reduced RAM usage.
 *
 * \note If this option is disabled, mbedtls_ssl_get_peer_cert() is still
 *       defined, but always returns \c NULL.
 *
 * \note This option has no influence on the protection against the
 *       triple handshake attack. Even if it is disabled, Mbed TLS will
 *       still ensure that certificates do not change during renegotiation,
 *       for example by keeping a hash of the peer's certificate.
 *
 * \note This option is required if MBEDTLS_SSL_PROTO_TLS1_3 is set.
 *
 * Comment this macro to disable storing the peer's certificate
 * after the handshake.
 */
#ifdef CONFIG_MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#define MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#else
#undef MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
#endif

/**
 * \def MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
 *
 * When this option is enabled, the client and server can extract additional
 * shared symmetric keys after an SSL handshake using the function
 * mbedtls_ssl_export_keying_material().
 *
 * The process for deriving the keys is specified in RFC 5705 for TLS 1.2 and
 * in RFC 8446, Section 7.5, for TLS 1.3.
 *
 * Comment this macro to disable mbedtls_ssl_export_keying_material().
 */
#ifdef CONFIG_MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
#define MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
#else
#undef MBEDTLS_SSL_KEYING_MATERIAL_EXPORT
#endif

/**
 * \def MBEDTLS_SSL_CBC_RECORD_SPLITTING
 *
 * Enable 1/n-1 record splitting for CBC mode in SSLv3 and TLS 1.0.
 *
 * This is a countermeasure to the BEAST attack, which also minimizes the risk
 * of interoperability issues compared to sending 0-length records.
 *
 * Comment this macro to disable 1/n-1 record splitting.
 */
#if defined(MBEDTLS_SSL_PROTO_SSL3) || defined(MBEDTLS_SSL_PROTO_TLS1)
#define MBEDTLS_SSL_CBC_RECORD_SPLITTING
#else
#undef MBEDTLS_SSL_CBC_RECORD_SPLITTING
#endif

/**
 * \def MBEDTLS_SSL_RENEGOTIATION
 *
 * Enable support for TLS renegotiation.
 *
 * The two main uses of renegotiation are (1) refresh keys on long-lived
 * connections and (2) client authentication after the initial handshake.
 * If you don't need renegotiation, it's probably better to disable it, since
 * it has been associated with security issues in the past and is easy to
 * misuse/misunderstand.
 *
 * Comment this to disable support for renegotiation.
 *
 * \note   Even if this option is disabled, both client and server are aware
 *         of the Renegotiation Indication Extension (RFC 5746) used to
 *         prevent the SSL renegotiation attack (see RFC 5746 Sect. 1).
 *         (See \c mbedtls_ssl_conf_legacy_renegotiation for the
 *          configuration of this extension).
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_RENEGOTIATION
#define MBEDTLS_SSL_RENEGOTIATION
#else
#undef MBEDTLS_SSL_RENEGOTIATION
#endif

/**
 * \def MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
 *
 * Enable support for RFC 6066 max_fragment_length extension in SSL.
 *
 * Comment this macro to disable support for the max_fragment_length extension
 */
#ifdef CONFIG_MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#else
#undef MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
#endif

/**
 * \def MBEDTLS_SSL_RECORD_SIZE_LIMIT
 *
 * Enable support for RFC 8449 record_size_limit extension in SSL (TLS 1.3 only).
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_3
 *
 * Uncomment this macro to enable support for the record_size_limit extension
 */
#ifdef CONFIG_MBEDTLS_SSL_RECORD_SIZE_LIMIT
#define MBEDTLS_SSL_RECORD_SIZE_LIMIT
#else
#undef MBEDTLS_SSL_RECORD_SIZE_LIMIT
#endif

/**
 * \def MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Enable support for TLS 1.2 (and DTLS 1.2 if DTLS is enabled).
 *
 * Requires: MBEDTLS_SHA1_C or MBEDTLS_SHA256_C or MBEDTLS_SHA512_C
 *           (Depends on ciphersuites)
 *
 * Comment this macro to disable support for TLS 1.2 / DTLS 1.2
 */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_TLS1_2
#else
#undef MBEDTLS_SSL_PROTO_TLS1_2
#endif

/**
 * \def MBEDTLS_SSL_PROTO_TLS1_3
 *
 * Enable support for TLS 1.3.
 *
 * \note The support for TLS 1.3 is not comprehensive yet, in particular
 *       pre-shared keys are not supported.
 *       See docs/architecture/tls13-support.md for a description of the TLS
 *       1.3 support that this option enables.
 *
 * Requires: MBEDTLS_SSL_KEEP_PEER_CERTIFICATE
 *
 * Uncomment this macro to enable the support for TLS 1.3.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#define MBEDTLS_SSL_PROTO_TLS1_3
#else
#undef MBEDTLS_SSL_PROTO_TLS1_3
#endif

/**
 * \def MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
 *
 * Enable TLS 1.3 middlebox compatibility mode.
 *
 * As specified in Section D.4 of RFC 8446, TLS 1.3 offers a compatibility
 * mode to make a TLS 1.3 connection more likely to pass through middle boxes
 * expecting TLS 1.2 traffic.
 *
 * Turning on the compatibility mode comes at the cost of a few added bytes
 * on the wire, but it doesn't affect compatibility with TLS 1.3 implementations
 * that don't use it. Therefore, unless transmission bandwidth is critical and
 * you know that middlebox compatibility issues won't occur, it is therefore
 * recommended to set this option.
 *
 * Comment to disable compatibility mode for TLS 1.3. If
 * MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not have any
 * effect on the build.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#define MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#else
#undef MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE
#endif

/**
 * \def MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
 *
 * Enable TLS 1.3 PSK key exchange mode.
 *
 * Comment to disable support for the PSK key exchange mode in TLS 1.3. If
 * MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not have any
 * effect on the build.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_TLS1_3_KEXM_PSK
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
#else
#undef MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED
#endif

/**
 * \def MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
 *
 * Enable TLS 1.3 ephemeral key exchange mode.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_X509_CRT_PARSE_C, MBEDTLS_ECDSA_C or
 *           MBEDTLS_PKCS1_V21
 *
 * Comment to disable support for the ephemeral key exchange mode in TLS 1.3.
 * If MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not have any
 * effect on the build.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_TLS1_3_KEXM_EPHEMERAL
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
#else
#undef MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED
#endif

/**
 * \def MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
 *
 * Enable TLS 1.3 PSK ephemeral key exchange mode.
 *
 * Requires: MBEDTLS_ECDH_C
 *
 * Comment to disable support for the PSK ephemeral key exchange mode in
 * TLS 1.3. If MBEDTLS_SSL_PROTO_TLS1_3 is not enabled, this option does not
 * have any effect on the build.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_TLS1_3_KEXM_PSK_EPHEMERAL
#define MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
#else
#undef MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
#endif

/**
 * \def MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE
 *
 * Maximum time difference in milliseconds tolerated between the age of a
 * ticket from the server and client point of view.
 * From the client point of view, the age of a ticket is the time difference
 * between the time when the client proposes to the server to use the ticket
 * (time of writing of the Pre-Shared Key Extension including the ticket) and
 * the time the client received the ticket from the server.
 * From the server point of view, the age of a ticket is the time difference
 * between the time when the server receives a proposition from the client
 * to use the ticket and the time when the ticket was created by the server.
 * The server age is expected to be always greater than the client one and
 * MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE defines the
 * maximum difference tolerated for the server to accept the ticket.
 * This is not used in TLS 1.2.
 *
 */
#define MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE 6000

/**
 * \def MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH
 *
 * Size in bytes of a ticket nonce. This is not used in TLS 1.2.
 *
 * This must be less than 256.
 */
#define MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH 32

/**
 * \def MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS
 *
 * Default number of NewSessionTicket messages to be sent by a TLS 1.3 server
 * after handshake completion. This is not used in TLS 1.2 and relevant only if
 * the MBEDTLS_SSL_SESSION_TICKETS option is enabled.
 *
 */
#define MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS 1

/**
 * \def MBEDTLS_SSL_EARLY_DATA
 *
 * Enable support for RFC 8446 TLS 1.3 early data.
 *
 * Requires: MBEDTLS_SSL_SESSION_TICKETS and either
 *           MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED or
 *           MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED
 *
 * Comment this to disable support for early data. If MBEDTLS_SSL_PROTO_TLS1_3
 * is not enabled, this option does not have any effect on the build.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_EARLY_DATA
#define MBEDTLS_SSL_EARLY_DATA
#else
#undef MBEDTLS_SSL_EARLY_DATA
#endif

/**
 * \def MBEDTLS_SSL_MAX_EARLY_DATA_SIZE
 *
 * The default maximum amount of 0-RTT data. See the documentation of
 * \c mbedtls_ssl_tls13_conf_max_early_data_size() for more information.
 *
 * It must be positive and smaller than UINT32_MAX.
 *
 * If MBEDTLS_SSL_EARLY_DATA is not defined, this default value does not
 * have any impact on the build.
 *
 * This feature is experimental, not completed and thus not ready for
 * production.
 *
 */
#define MBEDTLS_SSL_MAX_EARLY_DATA_SIZE        1024


/**
 * \def MBEDTLS_SSL_PROTO_DTLS
 *
 * Enable support for DTLS (all available versions).
 *
 * Enable this and MBEDTLS_SSL_PROTO_TLS1_2 to enable DTLS 1.2.
 *
 * Requires: MBEDTLS_SSL_PROTO_TLS1_2
 *
 * Comment this macro to disable support for DTLS
 */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_PROTO_DTLS
#else
#undef MBEDTLS_SSL_PROTO_DTLS
#endif

/**
 * \def MBEDTLS_SSL_ALPN
 *
 * Enable support for RFC 7301 Application Layer Protocol Negotiation.
 *
 * Comment this macro to disable support for ALPN.
 */
#ifdef CONFIG_MBEDTLS_SSL_ALPN
#define MBEDTLS_SSL_ALPN
#else
#undef MBEDTLS_SSL_ALPN
#endif

/**
 * \def MBEDTLS_SSL_DTLS_ANTI_REPLAY
 *
 * Enable support for the anti-replay mechanism in DTLS.
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *           MBEDTLS_SSL_PROTO_DTLS
 *
 * \warning Disabling this is often a security risk!
 * See mbedtls_ssl_conf_dtls_anti_replay() for details.
 *
 * Comment this to disable anti-replay in DTLS.
 */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#else
#undef MBEDTLS_SSL_DTLS_ANTI_REPLAY
#endif

/**
 * \def MBEDTLS_SSL_DTLS_HELLO_VERIFY
 *
 * Enable support for HelloVerifyRequest on DTLS servers.
 *
 * This feature is highly recommended to prevent DTLS servers being used as
 * amplifiers in DoS attacks against other hosts. It should always be enabled
 * unless you know for sure amplification cannot be a problem in the
 * environment in which your server operates.
 *
 * \warning Disabling this can ba a security risk! (see above)
 *
 * Requires: MBEDTLS_SSL_PROTO_DTLS
 *
 * Comment this to disable support for HelloVerifyRequest.
 */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#else
#undef MBEDTLS_SSL_DTLS_HELLO_VERIFY
#endif

/**
 * \def MBEDTLS_SSL_DTLS_SRTP
 *
 * Enable support for negotiation of DTLS-SRTP (RFC 5764)
 * through the use_srtp extension.
 *
 * \note This feature provides the minimum functionality required
 * to negotiate the use of DTLS-SRTP and to allow the derivation of
 * the associated SRTP packet protection key material.
 * In particular, the SRTP packet protection itself, as well as the
 * demultiplexing of RTP and DTLS packets at the datagram layer
 * (see Section 5 of RFC 5764), are not handled by this feature.
 * Instead, after successful completion of a handshake negotiating
 * the use of DTLS-SRTP, the extended key exporter API
 * mbedtls_ssl_conf_export_keys_ext_cb() should be used to implement
 * the key exporter described in Section 4.2 of RFC 5764 and RFC 5705
 * (this is implemented in the SSL example programs).
 * The resulting key should then be passed to an SRTP stack.
 *
 * Setting this option enables the runtime API
 * mbedtls_ssl_conf_dtls_srtp_protection_profiles()
 * through which the supported DTLS-SRTP protection
 * profiles can be configured. You must call this API at
 * runtime if you wish to negotiate the use of DTLS-SRTP.
 *
 * Requires: MBEDTLS_SSL_PROTO_DTLS
 *
 * Uncomment this to enable support for use_srtp extension.
 */
#ifdef CONFIG_MBEDTLS_SSL_DTLS_SRTP
#define MBEDTLS_SSL_DTLS_SRTP
#else
#undef MBEDTLS_SSL_DTLS_SRTP
#endif

/**
 * \def MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
 *
 * Enable server-side support for clients that reconnect from the same port.
 *
 * Some clients unexpectedly close the connection and try to reconnect using the
 * same source port. This needs special support from the server to handle the
 * new connection securely, as described in section 4.2.8 of RFC 6347. This
 * flag enables that support.
 *
 * Requires: MBEDTLS_SSL_DTLS_HELLO_VERIFY
 *
 * Comment this to disable support for clients reusing the source port.
 */
#ifdef CONFIG_MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#else
#undef MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#endif

/**
 * \def MBEDTLS_SSL_SESSION_TICKETS
 *
 * Enable support for RFC 5077 session tickets in SSL.
 * Client-side, provides full support for session tickets (maintenance of a
 * session store remains the responsibility of the application, though).
 * Server-side, you also need to provide callbacks for writing and parsing
 * tickets, including authenticated encryption and key management. Example
 * callbacks are provided by MBEDTLS_SSL_TICKET_C.
 *
 * Comment this macro to disable support for SSL session tickets
 */
#ifdef CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_SESSION_TICKETS
#else
#undef MBEDTLS_SSL_SESSION_TICKETS
#endif

/**
 * \def MBEDTLS_SSL_EXPORT_KEYS
 *
 * Enable support for exporting key block and master secret.
 * This is required for certain users of TLS, e.g. EAP-TLS.
 *
 * Comment this macro to disable support for key export
 */
#define MBEDTLS_SSL_EXPORT_KEYS

/**
 * \def MBEDTLS_SSL_SERVER_NAME_INDICATION
 *
 * Enable support for RFC 6066 server name indication (SNI) in SSL.
 *
 * Requires: MBEDTLS_X509_CRT_PARSE_C
 *
 * Comment this macro to disable support for server name indication in SSL
 */
#ifdef CONFIG_MBEDTLS_SSL_SERVER_NAME_INDICATION
#define MBEDTLS_SSL_SERVER_NAME_INDICATION
#else
#undef MBEDTLS_SSL_SERVER_NAME_INDICATION
#endif

/**
 * \def MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
 *
 * When this option is enabled, the SSL buffer will be resized automatically
 * based on the negotiated maximum fragment length in each direction.
 *
 * Requires: MBEDTLS_SSL_MAX_FRAGMENT_LENGTH
 */
#ifdef CONFIG_MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
#define MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
#else
#undef MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH
#endif

/**
 *
 * \def MBEDTLS_VERSION_FEATURES
 *
 * Allow run-time checking of compile-time enabled features. Thus allowing users
 * to check at run-time if the library is for instance compiled with threading
 * support via mbedtls_version_check_feature().
 *
 * Requires: MBEDTLS_VERSION_C
 *
 * Comment this to disable run-time checking and save ROM space
 */
#ifdef CONFIG_MBEDTLS_VERSION_FEATURES
#define MBEDTLS_VERSION_FEATURES
#else
#undef MBEDTLS_VERSION_FEATURES
#endif


/**
 * \def MBEDTLS_X509_RSASSA_PSS_SUPPORT
 *
 * Enable parsing and verification of X.509 certificates, CRLs and CSRS
 * signed with RSASSA-PSS (aka PKCS#1 v2.1).
 *
 * Comment this macro to disallow using RSASSA-PSS in certificates.
 */
#ifdef CONFIG_MBEDTLS_X509_RSASSA_PSS_SUPPORT
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT
#else
#undef MBEDTLS_X509_RSASSA_PSS_SUPPORT
#endif


/* \} name SECTION: mbed TLS feature support */

/**
 * \name SECTION: mbed TLS modules
 *
 * This section enables or disables entire modules in mbed TLS
 * \{
 */

/**
 * \def MBEDTLS_AESNI_C
 *
 * Enable AES-NI support on x86-64.
 *
 * Module:  library/aesni.c
 * Caller:  library/aes.c
 *
 * Requires: MBEDTLS_HAVE_ASM
 *
 * This modules adds support for the AES-NI instructions on x86-64
 */
#undef MBEDTLS_AESNI_C

/**
 * \def MBEDTLS_AESCE_C
 *
 * Enable AES cryptographic extension support on Armv8.
 *
 * Module:  library/aesce.c
 * Caller:  library/aes.c
 *
 * Requires: MBEDTLS_AES_C
 *
 * \warning Runtime detection only works on Linux. For non-Linux operating
 *          system, Armv8-A Cryptographic Extensions must be supported by
 *          the CPU when this option is enabled.
 *
 * \note    Minimum compiler versions for this feature when targeting aarch64
 *          are Clang 4.0; armclang 6.6; GCC 6.0; or MSVC 2019 version 16.11.2.
 *          Minimum compiler versions for this feature when targeting 32-bit
 *          Arm or Thumb are Clang 11.0; armclang 6.20; or GCC 6.0.
 *
 * \note \c CFLAGS must be set to a minimum of \c -march=armv8-a+crypto for
 * armclang <= 6.9
 *
 * This module adds support for the AES Armv8-A Cryptographic Extensions on Armv8 systems.
 */
#undef MBEDTLS_AESCE_C

/**
 * \def MBEDTLS_AES_C
 *
 * Enable the AES block cipher.
 *
 * Module:  library/aes.c
 * Caller:  library/ssl_tls.c
 *          library/pem.c
 *          library/ctr_drbg.c
 *
 * This module enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_PSK_WITH_AES_128_CBC_SHA
 *
 * PEM_PARSE uses AES for decrypting encrypted keys.
 */
#ifdef CONFIG_MBEDTLS_AES_C
#define PSA_WANT_KEY_TYPE_AES 1
#else
#undef PSA_WANT_KEY_TYPE_AES
#endif

/* The following units have ESP32 hardware support,
   uncommenting each _ALT macro will use the
   hardware-accelerated implementation. */
#ifdef CONFIG_MBEDTLS_HARDWARE_AES
#define MBEDTLS_PSA_ACCEL_ALG_CBC_NO_PADDING
#undef MBEDTLS_PSA_BUILTIN_ALG_CBC_NO_PADDING
#define MBEDTLS_PSA_ACCEL_ALG_CBC_PKCS7
#undef MBEDTLS_PSA_ACCEL_ALG_CBC_PKCS7
#define MBEDTLS_PSA_ACCEL_ALG_CCM
#undef MBEDTLS_PSA_BUILTIN_ALG_CCM
#define MBEDTLS_PSA_ACCEL_ALG_CCM_STAR_NO_TAG
#undef MBEDTLS_PSA_ACCEL_ALG_CCM_STAR_NO_TAG
#define MBEDTLS_PSA_ACCEL_ALG_CMAC
#undef MBEDTLS_PSA_BUILTIN_ALG_CMAC
#define MBEDTLS_PSA_ACCEL_ALG_CFB
#undef MBEDTLS_PSA_BUILTIN_ALG_CFB
#undef MBEDTLS_AES_C
#define MBEDTLS_PSA_ACCEL_ALG_CTR
#undef MBEDTLS_PSA_BUILTIN_ALG_CTR
#else
#undef MBEDTLS_AES_ALT
#endif

/**
 * \def MBEDTLS_ASN1_PARSE_C
 *
 * Enable the generic ASN1 parser.
 *
 * Module:  library/asn1.c
 * Caller:  library/x509.c
 *          library/dhm.c
 *          library/pkcs12.c
 *          library/pkcs5.c
 *          library/pkparse.c
 */
#ifdef CONFIG_MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_PARSE_C
#else
#undef MBEDTLS_ASN1_PARSE_C
#endif

/**
 * \def MBEDTLS_ASN1_WRITE_C
 *
 * Enable the generic ASN1 writer.
 *
 * Module:  library/asn1write.c
 * Caller:  library/ecdsa.c
 *          library/pkwrite.c
 *          library/x509_create.c
 *          library/x509write_crt.c
 *          library/mbedtls_x509write_csr.c
 */
#ifdef CONFIG_MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_ASN1_WRITE_C
#else
#undef MBEDTLS_ASN1_WRITE_C
#endif

/**
 * \def MBEDTLS_BASE64_C
 *
 * Enable the Base64 module.
 *
 * Module:  library/base64.c
 * Caller:  library/pem.c
 *
 * This module is required for PEM support (required by X.509).
 */
#ifdef CONFIG_MBEDTLS_BASE64_C
#define MBEDTLS_BASE64_C
#else
#undef MBEDTLS_BASE64_C
#endif

/**
 * \def MBEDTLS_BIGNUM_C
 *
 * Enable the multi-precision integer library.
 *
 * Module:  library/bignum.c
 *          library/bignum_core.c
 *          library/bignum_mod.c
 *          library/bignum_mod_raw.c
 * Caller:  library/dhm.c
 *          library/ecp.c
 *          library/ecdsa.c
 *          library/rsa.c
 *          library/rsa_alt_helpers.c
 *          library/ssl_tls.c
 *
 * This module is required for RSA, DHM and ECC (ECDH, ECDSA) support.
 */
#ifdef CONFIG_MBEDTLS_BIGNUM_C
#define MBEDTLS_BIGNUM_C
#else
#undef MBEDTLS_BIGNUM_C
#endif

/**
 * \def MBEDTLS_CAMELLIA_C
 *
 * Enable the Camellia block cipher.
 *
 * Module:  library/camellia.c
 * Caller:  library/ssl_tls.c
 *
 * This module enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_CAMELLIA_128_CBC_SHA256
 */
#ifdef CONFIG_MBEDTLS_CAMELLIA_C
#define PSA_WANT_KEY_TYPE_CAMELLIA 1
#else
#undef MBEDTLS_CAMELLIA_C
#undef PSA_WANT_KEY_TYPE_CAMELLIA
#endif

/**
 * \def MBEDTLS_ARIA_C
 *
 * Enable the ARIA block cipher.
 *
 * Module:  library/aria.c
 * Caller:  library/cipher.c
 *
 * This module enables the following ciphersuites (if other requisites are
 * enabled as well):
 *
 *      MBEDTLS_TLS_RSA_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_RSA_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_PSK_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_PSK_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_ARIA_256_CBC_SHA384
 *      MBEDTLS_TLS_PSK_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_PSK_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_ARIA_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_ARIA_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_ARIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_ARIA_256_CBC_SHA384
 */
#ifdef CONFIG_MBEDTLS_ARIA_C
#define PSA_WANT_KEY_TYPE_ARIA 1
#else
#undef MBEDTLS_ARIA_C
#undef PSA_WANT_KEY_TYPE_ARIA
#endif

/**
 * \def MBEDTLS_CCM_C
 *
 * Enable the Counter with CBC-MAC (CCM) mode for 128-bit block cipher.
 *
 * Module:  library/ccm.c
 *
 * Requires: MBEDTLS_AES_C or MBEDTLS_CAMELLIA_C or
 *                             MBEDTLS_ARIA_C
 *
 * This module enables the AES-CCM ciphersuites, if other requisites are
 * enabled as well.
 */
#ifdef CONFIG_MBEDTLS_CCM_C
#define PSA_WANT_ALG_CCM 1
#else
#undef PSA_WANT_ALG_CCM
#endif

/**
 * \def MBEDTLS_CERTS_C
 *
 * Enable the test certificates.
 *
 * Module:  library/certs.c
 * Caller:
 *
 * This module is used for testing (ssl_client/server).
 */
#define MBEDTLS_CERTS_C

/**
 * \def MBEDTLS_CHACHA20_C
 *
 * Enable the ChaCha20 stream cipher.
 *
 * Module:  library/chacha20.c
 */
#ifdef CONFIG_MBEDTLS_CHACHA20_C
#define PSA_WANT_KEY_TYPE_CHACHA20 1
#else
#undef MBEDTLS_CHACHA20_C
#undef PSA_WANT_KEY_TYPE_CHACHA20
#endif

/**
 * \def MBEDTLS_CHACHAPOLY_C
 *
 * Enable the ChaCha20-Poly1305 AEAD algorithm.
 *
 * Module:  library/chachapoly.c
 *
 * This module requires: MBEDTLS_CHACHA20_C, MBEDTLS_POLY1305_C
 */
#ifdef CONFIG_MBEDTLS_CHACHAPOLY_C
#define PSA_WANT_ALG_CHACHA20_POLY1305 1
#else
#undef MBEDTLS_CHACHAPOLY_C
#undef PSA_WANT_ALG_CHACHA20_POLY1305
#endif

/**
 * \def MBEDTLS_CTR_DRBG_C
 *
 * Enable the CTR_DRBG AES-256-based random generator.
 *
 * Module:  library/ctr_drbg.c
 * Caller:
 *
 * Requires: MBEDTLS_AES_C
 *
 * This module provides the CTR_DRBG AES-256 random number generator.
 */
#ifdef CONFIG_MBEDTLS_CTR_DRBG_C
#define MBEDTLS_CTR_DRBG_C
#else
#undef MBEDTLS_CTR_DRBG_C
#endif

/**
 * \def MBEDTLS_DEBUG_C
 *
 * Enable the debug functions.
 *
 * Module:  library/debug.c
 * Caller:  library/ssl_msg.c
 *          library/ssl_tls.c
 *          library/ssl_tls12_*.c
 *          library/ssl_tls13_*.c
 *
 * This module provides debugging functions.
 */
#if CONFIG_MBEDTLS_DEBUG
#define MBEDTLS_DEBUG_C
#else
#undef MBEDTLS_DEBUG_C
#endif

/**
 * \def MBEDTLS_DES_C
 *
 * Enable the DES block cipher.
 *
 * Module:  library/des.c
 * Caller:  library/pem.c
 *          library/ssl_tls.c
 *
 * This module enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_ECDHE_PSK_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_PSK_WITH_3DES_EDE_CBC_SHA
 *
 * PEM_PARSE uses DES/3DES for decrypting encrypted keys.
 */
#ifdef CONFIG_MBEDTLS_DES_C
#define MBEDTLS_DES_C
#else
#undef MBEDTLS_DES_C
#endif

/**
 * \def MBEDTLS_ECDH_C
 *
 * Enable the elliptic curve Diffie-Hellman library.
 *
 * Module:  library/ecdh.c
 * Caller:  library/psa_crypto.c
 *          library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * This module is used by the following key exchanges:
 *      ECDHE-ECDSA, ECDHE-RSA, DHE-PSK
 *
 * Requires: MBEDTLS_ECP_C
 */
#ifdef CONFIG_MBEDTLS_ECDH_C
#define MBEDTLS_ECDH_C
#else
#undef MBEDTLS_ECDH_C
#endif

/**
 * \def MBEDTLS_ECDSA_C
 *
 * Enable the elliptic curve DSA library.
 *
 * Module:  library/ecdsa.c
 * Caller:
 *
 * This module is used by the following key exchanges:
 *      ECDHE-ECDSA
 *
 * Requires: MBEDTLS_ECP_C, MBEDTLS_ASN1_WRITE_C, MBEDTLS_ASN1_PARSE_C,
 *           and at least one MBEDTLS_ECP_DP_XXX_ENABLED for a
 *           short Weierstrass curve.
 */
#ifdef CONFIG_MBEDTLS_ECDSA_C
#define MBEDTLS_ECDSA_C
#else
#undef MBEDTLS_ECDSA_C
#endif

/**
 * \def MBEDTLS_ECJPAKE_C
 *
 * Enable the elliptic curve J-PAKE library.
 *
 * \warning This is currently experimental. EC J-PAKE support is based on the
 * Thread v1.0.0 specification; incompatible changes to the specification
 * might still happen. For this reason, this is disabled by default.
 *
 * Module:  library/ecjpake.c
 * Caller:
 *
 * This module is used by the following key exchanges:
 *      ECJPAKE
 *
 * Requires: MBEDTLS_ECP_C and MBEDTLS_MD_C
 *
 */
#ifdef CONFIG_MBEDTLS_ECJPAKE_C
#define MBEDTLS_ECJPAKE_C
#else
#undef MBEDTLS_ECJPAKE_C
#endif

/**
 * \def MBEDTLS_ECP_C
 *
 * Enable the elliptic curve over GF(p) library.
 *
 * Module:  library/ecp.c
 * Caller:  library/ecdh.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *
 * Requires: MBEDTLS_BIGNUM_C and at least one MBEDTLS_ECP_DP_XXX_ENABLED
 */
#ifdef CONFIG_MBEDTLS_ECP_C
#define MBEDTLS_ECP_C
#else
#undef MBEDTLS_ECP_C
#endif

/**
 * \def MBEDTLS_ERROR_C
 *
 * Enable error code to error string conversion.
 *
 * Module:  library/error.c
 * Caller:
 *
 * This module enables mbedtls_strerror().
 */
#if CONFIG_MBEDTLS_ERROR_STRINGS
#define MBEDTLS_ERROR_C
#else
#undef MBEDTLS_ERROR_C
#endif

/**
 * \def MBEDTLS_SSL_CLI_ALLOW_WEAK_CERTIFICATE_VERIFICATION_WITHOUT_HOSTNAME
 *
 * Caller: library/ssl_tls.c
 *
 * Allow weak certificate verification without a hostname.
 * This option is not recommended for production use.
 */

#if CONFIG_MBEDTLS_ALLOW_WEAK_CERTIFICATE_VERIFICATION
#define MBEDTLS_SSL_CLI_ALLOW_WEAK_CERTIFICATE_VERIFICATION_WITHOUT_HOSTNAME
#else
#undef MBEDTLS_SSL_CLI_ALLOW_WEAK_CERTIFICATE_VERIFICATION_WITHOUT_HOSTNAME
#endif

/**
 * \def MBEDTLS_GCM_C
 *
 * Enable the Galois/Counter Mode (GCM).
 *
 * Module:  library/gcm.c
 *
 * Requires: MBEDTLS_AES_C or MBEDTLS_CAMELLIA_C or
 *                             MBEDTLS_ARIA_C
 *
 * This module enables the AES-GCM and CAMELLIA-GCM ciphersuites, if other
 * requisites are enabled as well.
 */
#ifdef CONFIG_MBEDTLS_GCM_C
#define PSA_WANT_ALG_GCM 1
#else
#undef PSA_WANT_ALG_GCM
#endif

/**
 * \def MBEDTLS_HMAC_DRBG_C
 *
 * Enable the HMAC_DRBG random generator.
 *
 * Module:  library/hmac_drbg.c
 * Caller:
 *
 * Requires: MBEDTLS_MD_C
 *
 * Uncomment to enable the HMAC_DRBG random number generator.
 */
#ifdef CONFIG_MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_HMAC_DRBG_C
#else
#undef MBEDTLS_HMAC_DRBG_C
#endif

/**
 * \def MBEDTLS_LMS_C
 *
 * Enable the LMS stateful-hash asymmetric signature algorithm.
 *
 * Module:  library/lms.c
 * Caller:
 *
 * Requires: MBEDTLS_PSA_CRYPTO_C
 *
 * Uncomment to enable the LMS verification algorithm and public key operations.
 *
 * This is disable by now. When we shift to PSA, we will enable it.
 */
#undef MBEDTLS_LMS_C

/**
 * \def MBEDTLS_MD_C
 *
 * Enable the generic message digest layer.
 *
 * Requires: one of: MBEDTLS_MD5_C, MBEDTLS_RIPEMD160_C, MBEDTLS_SHA1_C,
 *                   MBEDTLS_SHA224_C, MBEDTLS_SHA256_C, MBEDTLS_SHA384_C,
 *                   MBEDTLS_SHA512_C.
 *
 * Module:  library/md.c
 * Caller:  library/constant_time.c
 *          library/ecdsa.c
 *          library/ecjpake.c
 *          library/hkdf.c
 *          library/hmac_drbg.c
 *          library/pk.c
 *          library/pkcs5.c
 *          library/pkcs12.c
 *          library/psa_crypto_ecp.c
 *          library/psa_crypto_rsa.c
 *          library/rsa.c
 *          library/ssl_cookie.c
 *          library/ssl_msg.c
 *          library/ssl_tls.c
 *          library/x509.c
 *          library/x509_crt.c
 *          library/x509write_crt.c
 *          library/x509write_csr.c
 *
 * Uncomment to enable generic message digest wrappers.
 */
#ifdef CONFIG_MBEDTLS_MD_C
#define MBEDTLS_MD_C
#else
#undef MBEDTLS_MD_C
#endif

/**
 * \def MBEDTLS_MD5_C
 *
 * Enable the MD5 hash algorithm.
 *
 * Module:  library/mbedtls_md5.c
 * Caller:  library/mbedtls_md.c
 *          library/pem.c
 *          library/ssl_tls.c
 *
 * This module is required for SSL/TLS and X.509.
 * PEM_PARSE uses MD5 for decrypting encrypted keys.
 */
#ifdef CONFIG_MBEDTLS_MD5_C
#define PSA_WANT_ALG_MD5 1
#else
#undef MBEDTLS_MD5_C
#undef PSA_WANT_ALG_MD5
#endif

/**
 * \def MBEDTLS_NET_C
 *
 * Enable the TCP and UDP over IPv6/IPv4 networking routines.
 *
 * \note This module only works on POSIX/Unix (including Linux, BSD and OS X)
 * and Windows. For other platforms, you'll want to disable it, and write your
 * own networking callbacks to be passed to \c mbedtls_ssl_set_bio().
 *
 * \note See also our Knowledge Base article about porting to a new
 * environment:
 * https://mbed-tls.readthedocs.io/en/latest/kb/how-to/how-do-i-port-mbed-tls-to-a-new-environment-OS
 *
 * Module:  library/net_sockets.c
 *
 * This module provides networking routines.
 */
#ifdef MBEDTLS_NET_C
#undef MBEDTLS_NET_C
#endif

/**
 * \def MBEDTLS_PADLOCK_C
 *
 * Enable VIA Padlock support on x86.
 *
 * Module:  library/padlock.c
 * Caller:  library/aes.c
 *
 * Requires: MBEDTLS_HAVE_ASM
 *
 * This modules adds support for the VIA PadLock on x86.
 */
#undef MBEDTLS_PADLOCK_C

/**
 * \def MBEDTLS_PEM_PARSE_C
 *
 * Enable PEM decoding / parsing.
 *
 * Module:  library/pem.c
 * Caller:  library/dhm.c
 *          library/pkparse.c
 *          library/mbedtls_x509_crl.c
 *          library/mbedtls_x509_crt.c
 *          library/mbedtls_x509_csr.c
 *
 * Requires: MBEDTLS_BASE64_C
 *
 * This modules adds support for decoding / parsing PEM files.
 */
#ifdef CONFIG_MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_PARSE_C
#else
#undef MBEDTLS_PEM_PARSE_C
#endif

/**
 * \def MBEDTLS_PEM_WRITE_C
 *
 * Enable PEM encoding / writing.
 *
 * Module:  library/pem.c
 * Caller:  library/pkwrite.c
 *          library/x509write_crt.c
 *          library/mbedtls_x509write_csr.c
 *
 * Requires: MBEDTLS_BASE64_C
 *
 * This modules adds support for encoding / writing PEM files.
 */
#ifdef CONFIG_MBEDTLS_PEM_WRITE_C
#define MBEDTLS_PEM_WRITE_C
#else
#undef MBEDTLS_PEM_WRITE_C
#endif

/**
 * \def MBEDTLS_PK_C
 *
 * Enable the generic public (asymmetric) key layer.
 *
 * Module:  library/pk.c
 * Caller:  library/psa_crypto_rsa.c
 *          library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *          library/x509.c
 *
 * Requires: MBEDTLS_MD_C, MBEDTLS_RSA_C or MBEDTLS_ECP_C
 *
 * Requires: MBEDTLS_RSA_C or MBEDTLS_ECP_C
 *
 * Uncomment to enable generic public key wrappers.
 */
#ifdef CONFIG_MBEDTLS_PK_C
#define MBEDTLS_PK_C
#else
#undef MBEDTLS_PK_C
#endif

/**
 * \def MBEDTLS_PK_PARSE_C
 *
 * Enable the generic public (asymmetric) key parser.
 *
 * Module:  library/pkparse.c
 * Caller:  library/mbedtls_x509_crt.c
 *          library/mbedtls_x509_csr.c
 *
 * Requires: MBEDTLS_PK_C
 *
 * Uncomment to enable generic public key parse functions.
 */
#ifdef CONFIG_MBEDTLS_PK_PARSE_C
#define MBEDTLS_PK_PARSE_C
#else
#undef MBEDTLS_PK_PARSE_C
#endif

/**
 * \def MBEDTLS_PK_WRITE_C
 *
 * Enable the generic public (asymmetric) key writer.
 *
 * Module:  library/pkwrite.c
 * Caller:  library/x509write.c
 *
 * Requires: MBEDTLS_PK_C
 *
 * Uncomment to enable generic public key write functions.
 */
#ifdef CONFIG_MBEDTLS_PK_WRITE_C
#define MBEDTLS_PK_WRITE_C
#else
#undef MBEDTLS_PK_WRITE_C
#endif

/**
 * \def MBEDTLS_PKCS5_C
 *
 * Enable PKCS#5 functions.
 *
 * Module:  library/pkcs5.c
 *
 * Requires: MBEDTLS_MD_C
 *
 * This module adds support for the PKCS#5 functions.
 */
#ifdef CONFIG_MBEDTLS_PKCS5_C
#define MBEDTLS_PKCS5_C
#else
#undef MBEDTLS_PKCS5_C
#endif

/**
 * \def MBEDTLS_PKCS7_C
 *
 * This feature is a work in progress and not ready for production. Testing and
 * validation is incomplete, and handling of malformed inputs may not be robust.
 * The API may change.
 *
 * Enable PKCS7 core for using PKCS7 formatted signatures.
 * RFC Link - https://tools.ietf.org/html/rfc2315
 *
 * Module:  library/pkcs7.c
 *
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_OID_C, MBEDTLS_PK_PARSE_C,
 *           MBEDTLS_X509_CRT_PARSE_C MBEDTLS_X509_CRL_PARSE_C,
 *           MBEDTLS_BIGNUM_C, MBEDTLS_MD_C
 *
 * This module is required for the PKCS #7 parsing modules.
 */
#ifdef CONFIG_MBEDTLS_PKCS7_C
#define MBEDTLS_PKCS7_C
#else
#undef MBEDTLS_PKCS7_C
#endif

/**
 * \def MBEDTLS_PLATFORM_C
 *
 * Enable the platform abstraction layer that allows you to re-assign
 * functions like calloc(), free(), snprintf(), printf(), fprintf(), exit().
 *
 * Enabling MBEDTLS_PLATFORM_C enables to use of MBEDTLS_PLATFORM_XXX_ALT
 * or MBEDTLS_PLATFORM_XXX_MACRO directives, allowing the functions mentioned
 * above to be specified at runtime or compile time respectively.
 *
 * \note This abstraction layer must be enabled on Windows (including MSYS2)
 * as other modules rely on it for a fixed snprintf implementation.
 *
 * Module:  library/platform.c
 * Caller:  Most other .c files
 *
 * This module enables abstraction of common (libc) functions.
 */
#define MBEDTLS_PLATFORM_C

/**
 * \def MBEDTLS_POLY1305_C
 *
 * Enable the Poly1305 MAC algorithm.
 *
 * Module:  library/poly1305.c
 * Caller:  library/chachapoly.c
 */
#ifdef CONFIG_MBEDTLS_POLY1305_C
#define MBEDTLS_POLY1305_C
#else
#undef MBEDTLS_POLY1305_C
#endif

/**
 * \def MBEDTLS_RIPEMD160_C
 *
 * Enable the RIPEMD-160 hash algorithm.
 *
 * Module:  library/mbedtls_ripemd160.c
 * Caller:  library/mbedtls_md.c
 *
 */
#ifdef CONFIG_MBEDTLS_RIPEMD160_C
#define MBEDTLS_RIPEMD160_C
#else
#undef MBEDTLS_RIPEMD160_C
#undef PSA_WANT_ALG_RIPEMD160
#endif

/**
 * \def MBEDTLS_RSA_C
 *
 * Enable the RSA public-key cryptosystem.
 *
 * Module:  library/rsa.c
 *          library/rsa_alt_helpers.c
 * Caller:  library/pk.c
 *          library/psa_crypto.c
 *          library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * This module is used by the following key exchanges:
 *      RSA, DHE-RSA, ECDHE-RSA, RSA-PSK
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_OID_C
 */
#ifdef CONFIG_MBEDTLS_RSA_C
#define PSA_WANT_KEY_TYPE_RSA_KEY_PAIR 1
#define PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY 1
#else
#undef PSA_WANT_KEY_TYPE_RSA_KEY_PAIR
#undef PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY
#endif

/**
 * \def MBEDTLS_SHA1_C
 *
 * Enable the SHA1 cryptographic hash algorithm.
 *
 * Module:  library/sha1.c
 * Caller:  library/md.c
 *          library/psa_crypto_hash.c
 *
 * This module is required for TLS 1.2 depending on the handshake parameters,
 * and for SHA1-signed certificates.
 *
 * \warning   SHA-1 is considered a weak message digest and its use constitutes
 *            a security risk. If possible, we recommend avoiding dependencies
 *            on it, and considering stronger message digests instead.
 *
 */
#if CONFIG_MBEDTLS_SHA1_C
#define PSA_WANT_ALG_SHA_1 1
#else
#undef PSA_WANT_ALG_SHA_1
#endif
/**
 * \def MBEDTLS_SHA224_C
 *
 * Enable the SHA-224 cryptographic hash algorithm.
 *
 * Requires: MBEDTLS_SHA256_C. The library does not currently support enabling
 *           SHA-224 without SHA-256.
 *
 * Module:  library/sha256.c
 * Caller:  library/md.c
 *          library/ssl_cookie.c
 *
 * This module adds support for SHA-224.
 */
#ifdef CONFIG_MBEDTLS_SHA224_C
#define MBEDTLS_SHA224_C
#else
#undef MBEDTLS_SHA224_C
#endif

/**
 * \def MBEDTLS_SHA512_C
 *
 * Enable the SHA-384 and SHA-512 cryptographic hash algorithms.
 *
 * Module:  library/sha512.c
 * Caller:  library/entropy.c
 *          library/md.c
 *          library/ssl_tls.c
 *          library/ssl_cookie.c
 *
 * This module adds support for SHA-384 and SHA-512.
 */
#ifdef CONFIG_MBEDTLS_SHA512_C
#define PSA_WANT_ALG_SHA_512 1
#else
#undef PSA_WANT_ALG_SHA_512
#endif

/**
 * \def MBEDTLS_SHA384_C
 *
 * Enable the SHA-384 cryptographic hash algorithm.
 *
 * Module:  library/sha512.c
 * Caller:  library/md.c
 *          library/psa_crypto_hash.c
 *          library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * Comment to disable SHA-384
 */
#ifdef CONFIG_MBEDTLS_SHA384_C
#define PSA_WANT_ALG_SHA_384 1
#else
#undef PSA_WANT_ALG_SHA_384
#endif

/**
 * \def MBEDTLS_SHA256_C
 *
 * Enable the SHA-224 and SHA-256 cryptographic hash algorithms.
 *
 * Module:  library/mbedtls_sha256.c
 * Caller:  library/entropy.c
 *          library/mbedtls_md.c
 *          library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c=
 *
 * This module adds support for SHA-224 and SHA-256.
 * This module is required for the SSL/TLS 1.2 PRF function.
 */
#ifdef CONFIG_MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_C
#define PSA_WANT_ALG_SHA_256 1
#define PSA_WANT_ALG_SHA_224 1
#else
#undef PSA_WANT_ALG_SHA_256
#undef PSA_WANT_ALG_SHA_224
#endif

/* MBEDTLS_SHAxx_ALT to enable hardware SHA support
   with software fallback.
*/
#ifdef CONFIG_MBEDTLS_HARDWARE_SHA
    #define MBEDTLS_PSA_ACCEL_ALG_SHA_1
    #undef MBEDTLS_PSA_BUILTIN_ALG_SHA_1
    #define MBEDTLS_PSA_ACCEL_ALG_SHA_224
    #undef MBEDTLS_PSA_BUILTIN_ALG_SHA_224
    #define MBEDTLS_PSA_ACCEL_ALG_SHA_256
    #undef MBEDTLS_PSA_BUILTIN_ALG_SHA_256
    #undef MBEDTLS_SHA1_C
    #undef MBEDTLS_SHA224_C
    #if SOC_SHA_SUPPORT_SHA512
        #define MBEDTLS_PSA_ACCEL_ALG_SHA_512
        #undef MBEDTLS_PSA_BUILTIN_ALG_SHA_512
        #define MBEDTLS_PSA_ACCEL_ALG_SHA_384
        #undef MBEDTLS_PSA_BUILTIN_ALG_SHA_384
        #undef MBEDTLS_SHA512_C
        #undef MBEDTLS_SHA384_C
    #else
        #undef MBEDTLS_SHA512_ALT
    #endif
#else
    #undef MBEDTLS_SHA1_ALT
    #undef MBEDTLS_SHA256_ALT
    #undef MBEDTLS_SHA512_ALT
#endif

/* MBEDTLS_MD_CAN_SHA* macros indicate whether a hash algorithm is available
 * either via legacy implementation (MBEDTLS_SHA*_C) or via PSA (PSA_WANT_ALG_SHA_*).
 * These are used for TLS 1.3 signature algorithm configuration.
 */
#if defined(MBEDTLS_SHA1_C) || defined(PSA_WANT_ALG_SHA_1)
#define MBEDTLS_MD_CAN_SHA1
#endif

#if defined(MBEDTLS_SHA224_C) || defined(PSA_WANT_ALG_SHA_224)
#define MBEDTLS_MD_CAN_SHA224
#endif

#if defined(MBEDTLS_SHA256_C) || defined(PSA_WANT_ALG_SHA_256)
#define MBEDTLS_MD_CAN_SHA256
#endif

#if defined(MBEDTLS_SHA384_C) || defined(PSA_WANT_ALG_SHA_384)
#define MBEDTLS_MD_CAN_SHA384
#endif

#if defined(MBEDTLS_SHA512_C) || defined(PSA_WANT_ALG_SHA_512)
#define MBEDTLS_MD_CAN_SHA512
#endif

/**
 * \def MBEDTLS_SHA3_C
 *
 *  Enable the SHA3 cryptographic hash algorithm.
 *
 * Module:  library/sha3.c
 *
 * This module adds support for SHA3.
 */
#ifdef CONFIG_MBEDTLS_SHA3_C
#define PSA_WANT_ALG_SHA3_224 1
#define PSA_WANT_ALG_SHA3_256 1
#define PSA_WANT_ALG_SHA3_384 1
#define PSA_WANT_ALG_SHA3_512 1
#else
#undef PSA_WANT_ALG_SHA3_224
#undef PSA_WANT_ALG_SHA3_256
#undef PSA_WANT_ALG_SHA3_384
#undef PSA_WANT_ALG_SHA3_512
#endif

/**
 * \def MBEDTLS_SSL_CACHE_C
 *
 * Enable simple SSL cache implementation.
 *
 * Module:  library/ssl_cache.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_CACHE_C
 */
#ifdef CONFIG_MBEDTLS_SSL_CACHE_C
#define MBEDTLS_SSL_CACHE_C
#else
#undef MBEDTLS_SSL_CACHE_C
#endif

/**
 * \def MBEDTLS_SSL_COOKIE_C
 *
 * Enable basic implementation of DTLS cookies for hello verification.
 *
 * Module:  library/ssl_cookie.c
 * Caller:
 */
#ifdef CONFIG_MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_COOKIE_C
#else
#undef MBEDTLS_SSL_COOKIE_C
#endif

/**
 * \def MBEDTLS_SSL_TICKET_C
 *
 * Enable an implementation of TLS server-side callbacks for session tickets.
 *
 * Module:  library/ssl_ticket.c
 * Caller:
 *
 * Requires: (MBEDTLS_GCM_C || MBEDTLS_CCM_C || MBEDTLS_CHACHAPOLY_C)
 */
#ifdef CONFIG_MBEDTLS_SERVER_SSL_SESSION_TICKETS
#define MBEDTLS_SSL_TICKET_C
#else
#undef MBEDTLS_SSL_TICKET_C
#endif

/**
 * \def MBEDTLS_SSL_CLI_C
 *
 * Enable the SSL/TLS client code.
 *
 * Module:  library/ssl*_client.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *
 * This module is required for SSL/TLS client support.
 */
#ifdef CONFIG_MBEDTLS_TLS_CLIENT
#define MBEDTLS_SSL_CLI_C
#else
#undef MBEDTLS_SSL_CLI_C
#endif

/**
 * \def MBEDTLS_SSL_SRV_C
 *
 * Enable the SSL/TLS server code.
 *
 * Module:  library/ssl_srv.c
 * Caller:
 *
 * Requires: MBEDTLS_SSL_TLS_C
 *
 * This module is required for SSL/TLS server support.
 */
#ifdef CONFIG_MBEDTLS_TLS_SERVER
#define MBEDTLS_SSL_SRV_C
#else
#undef MBEDTLS_SSL_SRV_C
#endif

/**
 * \def MBEDTLS_SSL_TLS_C
 *
 * Enable the generic SSL/TLS code.
 *
 * Module:  library/ssl_tls.c
 * Caller:  library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * Requires: MBEDTLS_MD_C
 *           and at least one of the MBEDTLS_SSL_PROTO_XXX defines
 *
 * This module is required for SSL/TLS.
 */
#ifdef CONFIG_MBEDTLS_TLS_ENABLED
#define MBEDTLS_SSL_TLS_C
#else
#undef MBEDTLS_SSL_TLS_C
#endif

/**
 * \def MBEDTLS_TIMING_C
 *
 * Enable the semi-portable timing interface.
 *
 * \note The provided implementation only works on POSIX/Unix (including Linux,
 * BSD and OS X) and Windows. On other platforms, you can either disable that
 * module and provide your own implementations of the callbacks needed by
 * \c mbedtls_ssl_set_timer_cb() for DTLS, or leave it enabled and provide
 * your own implementation of the whole module by setting
 * \c MBEDTLS_TIMING_ALT in the current file.
 *
 * \note See also our Knowledge Base article about porting to a new
 * environment:
 * https://mbed-tls.readthedocs.io/en/latest/kb/how-to/how-do-i-port-mbed-tls-to-a-new-environment-OS
 *
 * Module:  library/timing.c
 * Caller:  library/havege.c
 *
 * This module is used by the HAVEGE random number generator.
 */
#ifdef MBEDTLS_TIMING_C
#undef MBEDTLS_TIMING_C
#endif

/**
 * \def MBEDTLS_VERSION_C
 *
 * Enable run-time version information.
 *
 * Module:  library/version.c
 *
 * This module provides run-time version information.
 */
#ifdef CONFIG_MBEDTLS_VERSION_C
#define MBEDTLS_VERSION_C
#else
#undef MBEDTLS_VERSION_C
#endif

/**
 * \def MBEDTLS_X509_USE_C
 *
 * Enable X.509 core for using certificates.
 *
 * Module:  library/x509.c
 * Caller:  library/mbedtls_x509_crl.c
 *          library/mbedtls_x509_crt.c
 *          library/mbedtls_x509_csr.c
 *
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_BIGNUM_C, MBEDTLS_OID_C,
 *           MBEDTLS_PK_PARSE_C, MBEDTLS_MD_C
 *
 * This module is required for the X.509 parsing modules.
 */
#ifdef CONFIG_MBEDTLS_X509_USE_C
#define MBEDTLS_X509_USE_C
#else
#undef MBEDTLS_X509_USE_C
#endif

/**
 * \def MBEDTLS_X509_CRT_PARSE_C
 *
 * Enable X.509 certificate parsing.
 *
 * Module:  library/mbedtls_x509_crt.c
 * Caller:  library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is required for X.509 certificate parsing.
 */
#ifdef CONFIG_MBEDTLS_X509_CRT_PARSE_C
#define MBEDTLS_X509_CRT_PARSE_C
#else
#undef MBEDTLS_X509_CRT_PARSE_C
#endif

/**
 * \def MBEDTLS_X509_CRL_PARSE_C
 *
 * Enable X.509 CRL parsing.
 *
 * Module:  library/mbedtls_x509_crl.c
 * Caller:  library/mbedtls_x509_crt.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is required for X.509 CRL parsing.
 */
#ifdef CONFIG_MBEDTLS_X509_CRL_PARSE_C
#define MBEDTLS_X509_CRL_PARSE_C
#else
#undef MBEDTLS_X509_CRL_PARSE_C
#endif

/**
 * \def MBEDTLS_X509_CSR_PARSE_C
 *
 * Enable X.509 Certificate Signing Request (CSR) parsing.
 *
 * Module:  library/mbedtls_x509_csr.c
 * Caller:  library/x509_crt_write.c
 *
 * Requires: MBEDTLS_X509_USE_C
 *
 * This module is used for reading X.509 certificate request.
 */
#ifdef CONFIG_MBEDTLS_X509_CSR_PARSE_C
#define MBEDTLS_X509_CSR_PARSE_C
#else
#undef MBEDTLS_X509_CSR_PARSE_C
#endif

/**
 * \def MBEDTLS_X509_CREATE_C
 *
 * Enable X.509 core for creating certificates.
 *
 * Module:  library/x509_create.c
 *
 * Requires: MBEDTLS_BIGNUM_C, MBEDTLS_OID_C, MBEDTLS_PK_WRITE_C,
 *           MBEDTLS_MD_C
 *
 * This module is the basis for creating X.509 certificates and CSRs.
 */
#ifdef CONFIG_MBEDTLS_X509_CREATE_C
#define MBEDTLS_X509_CREATE_C
#else
#undef MBEDTLS_X509_CREATE_C
#endif

/**
 * \def MBEDTLS_X509_CRT_WRITE_C
 *
 * Enable creating X.509 certificates.
 *
 * Module:  library/x509_crt_write.c
 *
 * Requires: MBEDTLS_X509_CREATE_C
 *
 * This module is required for X.509 certificate creation.
 */
#ifdef CONFIG_MBEDTLS_X509_CRT_WRITE_C
#define MBEDTLS_X509_CRT_WRITE_C
#else
#undef MBEDTLS_X509_CRT_WRITE_C
#endif

/**
 * \def MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK
 *
 * If set, this enables the X.509 API `mbedtls_x509_crt_verify_with_ca_cb()`
 * and the SSL API `mbedtls_ssl_conf_ca_cb()` which allow users to configure
 * the set of trusted certificates through a callback instead of a linked
 * list.
 *
 * This is useful for example in environments where a large number of trusted
 * certificates is present and storing them in a linked list isn't efficient
 * enough, or when the set of trusted certificates changes frequently.
 *
 * See the documentation of `mbedtls_x509_crt_verify_with_ca_cb()` and
 * `mbedtls_ssl_conf_ca_cb()` for more information.
 *
 * Uncomment to enable trusted certificate callbacks.
 */
#ifdef CONFIG_MBEDTLS_X509_TRUSTED_CERT_CALLBACK
#define MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK
#else
#undef MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK
#endif

/**
 * \def MBEDTLS_X509_REMOVE_INFO
 *
 * Disable mbedtls_x509_*_info() and related APIs.
 *
 * Uncomment to omit mbedtls_x509_*_info(), as well as mbedtls_debug_print_crt()
 * and other functions/constants only used by these functions, thus reducing
 * the code footprint by several KB.
 */
#ifdef CONFIG_MBEDTLS_X509_REMOVE_INFO
#define MBEDTLS_X509_REMOVE_INFO
#else
#undef MBEDTLS_X509_REMOVE_INFO
#endif

/**
 * \def MBEDTLS_X509_CSR_WRITE_C
 *
 * Enable creating X.509 Certificate Signing Requests (CSR).
 *
 * Module:  library/x509_csr_write.c
 *
 * Requires: MBEDTLS_X509_CREATE_C
 *
 * This module is required for X.509 certificate request writing.
 */
#ifdef CONFIG_MBEDTLS_X509_CSR_WRITE_C
#define MBEDTLS_X509_CSR_WRITE_C
#else
#undef MBEDTLS_X509_CSR_WRITE_C
#endif

/* \} name SECTION: mbed TLS modules */

/**
 * \name SECTION: Module configuration options
 *
 * This section allows for the setting of module specific sizes and
 * configuration options. The default values are already present in the
 * relevant header files and should suffice for the regular use cases.
 *
 * Our advice is to enable options and change their values here
 * only if you have a good reason and know the consequences.
 *
 * Please check the respective header file for documentation on these
 * parameters (to prevent duplicate documentation).
 * \{
 */

/* SSL options */
#ifndef CONFIG_MBEDTLS_ASYMMETRIC_CONTENT_LEN

#define MBEDTLS_SSL_MAX_CONTENT_LEN             CONFIG_MBEDTLS_SSL_MAX_CONTENT_LEN /**< Maximum fragment length in bytes, determines the size of each of the two internal I/O buffers */

#else

/** \def MBEDTLS_SSL_IN_CONTENT_LEN
 *
 * Maximum incoming fragment length in bytes.
 *
 * Uncomment to set the size of the inward TLS buffer independently of the
 * outward buffer.
 */
#define MBEDTLS_SSL_IN_CONTENT_LEN              CONFIG_MBEDTLS_SSL_IN_CONTENT_LEN

/** \def MBEDTLS_SSL_CID_IN_LEN_MAX
 *
 * The maximum length of CIDs used for incoming DTLS messages.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_CID_IN_LEN_MAX              CONFIG_MBEDTLS_SSL_CID_IN_LEN_MAX
#else
#undef MBEDTLS_SSL_CID_IN_LEN_MAX
#endif


/** \def MBEDTLS_SSL_CID_OUT_LEN_MAX
 *
 * The maximum length of CIDs used for outgoing DTLS messages.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_CID_OUT_LEN_MAX            CONFIG_MBEDTLS_SSL_CID_OUT_LEN_MAX
#else
#undef MBEDTLS_SSL_CID_OUT_LEN_MAX
#endif

/** \def MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY
 *
 * This option controls the use of record plaintext padding
 * in TLS 1.3 and when using the Connection ID extension in DTLS 1.2.
 *
 * The padding will always be chosen so that the length of the
 * padded plaintext is a multiple of the value of this option.
 *
 * Note: A value of \c 1 means that no padding will be used
 *       for outgoing records.
 *
 * Note: On systems lacking division instructions,
 *       a power of two should be preferred.
 *
 */
#ifdef CONFIG_MBEDTLS_SSL_CID_PADDING_GRANULARITY
#define MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY    CONFIG_MBEDTLS_SSL_CID_PADDING_GRANULARITY
#else
#undef MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY
#endif


/** \def MBEDTLS_SSL_OUT_CONTENT_LEN
 *
 * Maximum outgoing fragment length in bytes.
 *
 * Uncomment to set the size of the outward TLS buffer independently of the
 * inward buffer.
 *
 * It is possible to save RAM by setting a smaller outward buffer, while keeping
 * the default inward 16384 byte buffer to conform to the TLS specification.
 *
 * The minimum required outward buffer size is determined by the handshake
 * protocol's usage. Handshaking will fail if the outward buffer is too small.
 * The specific size requirement depends on the configured ciphers and any
 * certificate data which is sent during the handshake.
 *
 * For absolute minimum RAM usage, it's best to enable
 * MBEDTLS_SSL_MAX_FRAGMENT_LENGTH and reduce MBEDTLS_SSL_MAX_CONTENT_LEN. This
 * reduces both incoming and outgoing buffer sizes. However this is only
 * guaranteed if the other end of the connection also supports the TLS
 * max_fragment_len extension. Otherwise the connection may fail.
 */
#define MBEDTLS_SSL_OUT_CONTENT_LEN             CONFIG_MBEDTLS_SSL_OUT_CONTENT_LEN

#endif /* !CONFIG_MBEDTLS_ASYMMETRIC_CONTENT_LEN */

/**
 * Allow SHA-1 in the default TLS configuration for TLS 1.2 handshake
 * signature and ciphersuite selection. Without this build-time option, SHA-1
 * support must be activated explicitly through mbedtls_ssl_conf_sig_hashes.
 * The use of SHA-1 in TLS <= 1.1 and in HMAC-SHA-1 is always allowed by
 * default. At the time of writing, there is no practical attack on the use
 * of SHA-1 in handshake signatures, hence this option is turned on by default
 * for compatibility with existing peers.
 *
 * \warning   SHA-1 is considered a weak message digest and its use constitutes
 *            a security risk. If possible, we recommend avoiding dependencies
 *            on it, and considering stronger message digests instead.
 */
#define MBEDTLS_TLS_DEFAULT_ALLOW_SHA1_IN_KEY_EXCHANGE

/**
 * \def MBEDTLS_THREADING_C
 *
 * Enable the threading abstraction layer.
 * By default mbed TLS assumes it is used in a non-threaded environment or that
 * contexts are not shared between threads. If you do intend to use contexts
 * between threads, you will need to enable this layer to prevent race
 * conditions. See also our Knowledge Base article about threading:
 * https://mbed-tls.readthedocs.io/en/latest/kb/development/thread-safety-and-multi-threading
 *
 * Module:  library/threading.c
 *
 * This allows different threading implementations (self-implemented or
 * provided).
 *
 * You will have to enable either MBEDTLS_THREADING_ALT or
 * MBEDTLS_THREADING_PTHREAD.
 *
 * Enable this layer to allow use of mutexes within mbed TLS
 */
#ifdef CONFIG_MBEDTLS_THREADING_C
#define MBEDTLS_THREADING_C
#else
#undef MBEDTLS_THREADING_C
#endif

/**
 * \def MBEDTLS_THREADING_ALT
 *
 * Provide your own alternate threading implementation.
 *
 * Requires: MBEDTLS_THREADING_C
 *
 * Uncomment this to allow your own alternate threading implementation.
 */
#ifdef CONFIG_MBEDTLS_THREADING_ALT
#define MBEDTLS_THREADING_ALT
#else
#undef MBEDTLS_THREADING_ALT
#endif

/**
 * \def MBEDTLS_THREADING_PTHREAD
 *
 * Enable the pthread wrapper layer for the threading layer.
 *
 * Requires: MBEDTLS_THREADING_C
 *
 * Uncomment this to enable pthread mutexes.
 */
#ifdef CONFIG_MBEDTLS_THREADING_PTHREAD
#define MBEDTLS_THREADING_PTHREAD
#else
#undef MBEDTLS_THREADING_PTHREAD
#endif

/**
 * \def MBEDTLS_NIST_KW_C
 *
 * Enable AES key wrapping as per NIST
 *
 * Requires: MBEDTLS_AES_C
 *
 * Uncomment this to enable aes key wrap.
 */
#ifdef CONFIG_MBEDTLS_NIST_KW_C
#define MBEDTLS_NIST_KW_C
#else
#undef MBEDTLS_NIST_KW_C
#endif

/* \} name SECTION: Module configuration options */

#if defined(TARGET_LIKE_MBED)
#include "mbedtls/target_config.h"
#endif

/*
 * Allow user to override any previous default.
 *
 * Use two macro names for that, as:
 * - with yotta the prefix YOTTA_CFG_ is forced
 * - without yotta is looks weird to have a YOTTA prefix.
 */
#if defined(YOTTA_CFG_MBEDTLS_USER_CONFIG_FILE)
#include YOTTA_CFG_MBEDTLS_USER_CONFIG_FILE
#elif defined(MBEDTLS_USER_CONFIG_FILE)
#include MBEDTLS_USER_CONFIG_FILE
#endif

/* This flag makes sure that we are not using
 * any function that is deprecated by mbedtls */
#define MBEDTLS_DEPRECATED_REMOVED

#endif /* ESP_CONFIG_H */
