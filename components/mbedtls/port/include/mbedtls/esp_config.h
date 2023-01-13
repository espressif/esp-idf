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

#include "sdkconfig.h"
#include "mbedtls/mbedtls_config.h"
#include "soc/soc_caps.h"

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
#else
#undef MBEDTLS_HAVE_TIME
#endif

/**
 * \def MBEDTLS_HAVE_TIME_DATE
 *
 * System has time.h and time(), gmtime() and the clock is correct.
 * The time needs to be correct (not necesarily very accurate, but at least
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

/* The following units have ESP32 hardware support,
   uncommenting each _ALT macro will use the
   hardware-accelerated implementation. */
#ifdef CONFIG_MBEDTLS_HARDWARE_AES
#define MBEDTLS_AES_ALT
#else
#undef MBEDTLS_AES_ALT
#endif

#ifdef CONFIG_MBEDTLS_HARDWARE_AES
#define MBEDTLS_GCM_ALT
#endif

/* MBEDTLS_SHAxx_ALT to enable hardware SHA support
   with software fallback.
*/
#ifdef CONFIG_MBEDTLS_HARDWARE_SHA
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT

#if SOC_SHA_SUPPORT_SHA512
#define MBEDTLS_SHA512_ALT
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
#define MBEDTLS_MD5_ALT
#else
#undef MBEDTLS_MD5_ALT
#endif

/* The following MPI (bignum) functions have hardware support.
 * Uncommenting these macros will use the hardware-accelerated
 * implementations.
 */
#ifdef CONFIG_MBEDTLS_HARDWARE_MPI
#ifdef CONFIG_MBEDTLS_LARGE_KEY_SOFTWARE_MPI
    /* Prefer hardware and fallback to software */
    #define MBEDTLS_MPI_EXP_MOD_ALT_FALLBACK
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
 * \def MBEDTLS_ENTROPY_HARDWARE_ALT
 *
 * Uncomment this macro to let mbed TLS use your own implementation of a
 * hardware entropy collector.
 *
 * Your function must be called \c mbedtls_hardware_poll(), have the same
 * prototype as declared in entropy_poll.h, and accept NULL as first argument.
 *
 * Uncomment to use your own hardware entropy collector.
 */
#define MBEDTLS_ENTROPY_HARDWARE_ALT

/**
 * \def MBEDTLS_AES_ROM_TABLES
 *
 * Store the AES tables in ROM.
 *
 * Uncomment this macro to store the AES tables in ROM.
 */
#define MBEDTLS_AES_ROM_TABLES

/**
 * \def MBEDTLS_CIPHER_MODE_CBC
 *
 * Enable Cipher Block Chaining mode (CBC) for symmetric ciphers.
 */
#define MBEDTLS_CIPHER_MODE_CBC

/**
 * \def MBEDTLS_CIPHER_MODE_CFB
 *
 * Enable Cipher Feedback mode (CFB) for symmetric ciphers.
 */
#define MBEDTLS_CIPHER_MODE_CFB

/**
 * \def MBEDTLS_CIPHER_MODE_CTR
 *
 * Enable Counter Block Cipher mode (CTR) for symmetric ciphers.
 */
#define MBEDTLS_CIPHER_MODE_CTR

/**
 * \def MBEDTLS_CIPHER_MODE_OFB
 *
 * Enable Output Feedback mode (OFB) for symmetric ciphers.
 */
#define MBEDTLS_CIPHER_MODE_OFB

/**
 * \def MBEDTLS_CIPHER_MODE_XTS
 *
 * Enable Xor-encrypt-xor with ciphertext stealing mode (XTS) for AES.
 */
#define MBEDTLS_CIPHER_MODE_XTS

/**
 * \def MBEDTLS_CIPHER_PADDING_PKCS7
 *
 * MBEDTLS_CIPHER_PADDING_XXX: Uncomment or comment macros to add support for
 * specific padding modes in the cipher layer with cipher modes that support
 * padding (e.g. CBC)
 *
 * If you disable all padding modes, only full blocks can be used with CBC.
 *
 * Enable padding modes in the cipher layer.
 */
#define MBEDTLS_CIPHER_PADDING_PKCS7
#define MBEDTLS_CIPHER_PADDING_ONE_AND_ZEROS
#define MBEDTLS_CIPHER_PADDING_ZEROS_AND_LEN
#define MBEDTLS_CIPHER_PADDING_ZEROS

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
 * Uncomment this macro to enable restartable ECC computations.
 *
 * \note  This option only works with the default software implementation of
 *        elliptic curve functionality. It is incompatible with
 *        MBEDTLS_ECP_ALT, MBEDTLS_ECDH_XXX_ALT, MBEDTLS_ECDSA_XXX_ALT
 *        and MBEDTLS_ECDH_LEGACY_CONTEXT.
 */
#ifdef CONFIG_MBEDTLS_ECP_RESTARTABLE
#define MBEDTLS_ECP_RESTARTABLE
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
 * Requires: MBEDTLS_CIPHER_C, MBEDTLS_AES_C or MBEDTLS_DES_C
 *
 */
#ifdef CONFIG_MBEDTLS_CMAC_C
#define MBEDTLS_CMAC_C
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
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP192R1_ENABLED
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP192R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP224R1_ENABLED
#define MBEDTLS_ECP_DP_SECP224R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP224R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP256R1_ENABLED
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP256R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP384R1_ENABLED
#define MBEDTLS_ECP_DP_SECP384R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP384R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP521R1_ENABLED
#define MBEDTLS_ECP_DP_SECP521R1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP521R1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP192K1_ENABLED
#define MBEDTLS_ECP_DP_SECP192K1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP192K1_ENABLED
#endif
#ifdef CONFIG_MBEDTLS_ECP_DP_SECP224K1_ENABLED
#define MBEDTLS_ECP_DP_SECP224K1_ENABLED
#else
#undef MBEDTLS_ECP_DP_SECP224K1_ENABLED
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
#define MBEDTLS_ECDSA_DETERMINISTIC
#else
#undef MBEDTLS_ECDSA_DETERMINISTIC
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
 * \def MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
 *
 * Enable the DHE-PSK based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_DHM_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_PSK_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_PSK
#define MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_DHE_PSK_ENABLED
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
 * \def MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
 *
 * Enable the RSA-PSK based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_RSA_C, MBEDTLS_PKCS1_V15,
 *           MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_PSK_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_RSA_PSK
#define MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_RSA_PSK_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
 *
 * Enable the RSA-only based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_RSA_C, MBEDTLS_PKCS1_V15,
 *           MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA
 *      MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_RSA
#define MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_RSA_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
 *
 * Enable the DHE-RSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_DHM_C, MBEDTLS_RSA_C, MBEDTLS_PKCS1_V15,
 *           MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_GCM_SHA384
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA
 *      MBEDTLS_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_DHE_RSA
#define MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_DHE_RSA_ENABLED
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
 * \def MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
 *
 * Enable the ECDH-ECDSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_ECDSA_C, MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_ECDSA_WITH_CAMELLIA_256_GCM_SHA384
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA
#define MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_ECDH_ECDSA_ENABLED
#endif

/**
 * \def MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
 *
 * Enable the ECDH-RSA based ciphersuite modes in SSL / TLS.
 *
 * Requires: MBEDTLS_ECDH_C, MBEDTLS_RSA_C, MBEDTLS_X509_CRT_PARSE_C
 *
 * This enables the following ciphersuites (if other requisites are
 * enabled as well):
 *      MBEDTLS_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_128_CBC_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_256_CBC_SHA384
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_128_GCM_SHA256
 *      MBEDTLS_TLS_ECDH_RSA_WITH_CAMELLIA_256_GCM_SHA384
 */
#ifdef CONFIG_MBEDTLS_KEY_EXCHANGE_ECDH_RSA
#define MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
#else
#undef MBEDTLS_KEY_EXCHANGE_ECDH_RSA_ENABLED
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
#define MBEDTLS_PK_PARSE_EC_EXTENDED

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
#define MBEDTLS_ERROR_STRERROR_DUMMY

/**
 * \def MBEDTLS_GENPRIME
 *
 * Enable the prime-number generation code.
 *
 * Requires: MBEDTLS_BIGNUM_C
 */
#define MBEDTLS_GENPRIME

/**
 * \def MBEDTLS_FS_IO
 *
 * Enable functions that use the filesystem.
 */
#define MBEDTLS_FS_IO

/**
 * \def MBEDTLS_NO_PLATFORM_ENTROPY
 *
 * Do not use built-in platform entropy functions.
 * This is useful if your platform does not support
 * standards like the /dev/urandom or Windows CryptoAPI.
 *
 * Uncomment this macro to disable the built-in platform entropy functions.
 */
#define MBEDTLS_NO_PLATFORM_ENTROPY

/**
 * \def MBEDTLS_PK_RSA_ALT_SUPPORT
 *
 * Support external private RSA keys (eg from a HSM) in the PK layer.
 *
 * Comment this macro to disable support for external private RSA keys.
 */
#define MBEDTLS_PK_RSA_ALT_SUPPORT

/**
 * \def MBEDTLS_PKCS1_V15
 *
 * Enable support for PKCS#1 v1.5 encoding.
 *
 * Requires: MBEDTLS_RSA_C
 *
 * This enables support for PKCS#1 v1.5 operations.
 */
#define MBEDTLS_PKCS1_V15

/**
 * \def MBEDTLS_PKCS1_V21
 *
 * Enable support for PKCS#1 v2.1 encoding.
 *
 * Requires: MBEDTLS_MD_C, MBEDTLS_RSA_C
 *
 * This enables support for RSAES-OAEP and RSASSA-PSS operations.
 */
#define MBEDTLS_PKCS1_V21

/**
 * \def MBEDTLS_SELF_TEST
 *
 * Enable the checkup functions (*_self_test).
 */
#define MBEDTLS_SELF_TEST

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
#define MBEDTLS_SSL_ALL_ALERT_MESSAGES

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
 * context associated with the connection, then free or re-use that context
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
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH


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
* This feature is experimental, not completed and thus not ready for
* production.
*
*/
//#define MBEDTLS_SSL_EARLY_DATA

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
#ifdef CONFIG_MBEDTLS_SSL_PROTO_DTLS
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
 * Client-side, provides full support for session tickets (maintainance of a
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
#define MBEDTLS_SSL_SERVER_NAME_INDICATION


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
#define MBEDTLS_VERSION_FEATURES


/**
 * \def MBEDTLS_X509_RSASSA_PSS_SUPPORT
 *
 * Enable parsing and verification of X.509 certificates, CRLs and CSRS
 * signed with RSASSA-PSS (aka PKCS#1 v2.1).
 *
 * Comment this macro to disallow using RSASSA-PSS in certificates.
 */
#define MBEDTLS_X509_RSASSA_PSS_SUPPORT


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
#define MBEDTLS_AESNI_C

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
#define MBEDTLS_AES_C
#else
#undef MBEDTLS_AES_C
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
#define MBEDTLS_ASN1_PARSE_C

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
#define MBEDTLS_ASN1_WRITE_C

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
#define MBEDTLS_BASE64_C

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
#define MBEDTLS_BIGNUM_C

/**
 * \def MBEDTLS_BLOWFISH_C
 *
 * Enable the Blowfish block cipher.
 *
 * Module:  library/blowfish.c
 */
#ifdef CONFIG_MBEDTLS_BLOWFISH_C
#define MBEDTLS_BLOWFISH_C
#else
#undef MBEDTLS_BLOWFISH_C
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
#define MBEDTLS_CAMELLIA_C
#else
#undef MBEDTLS_CAMELLIA_C
#endif

/**
 * \def MBEDTLS_CCM_C
 *
 * Enable the Counter with CBC-MAC (CCM) mode for 128-bit block cipher.
 *
 * Module:  library/ccm.c
 *
 * Requires: MBEDTLS_CIPHER_C, MBEDTLS_AES_C or MBEDTLS_CAMELLIA_C or
 *                             MBEDTLS_ARIA_C
 *
 * This module enables the AES-CCM ciphersuites, if other requisites are
 * enabled as well.
 */
#ifdef CONFIG_MBEDTLS_CCM_C
#define MBEDTLS_CCM_C
#else
#undef MBEDTLS_CCM_C
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
#define MBEDTLS_CHACHA20_C
#else
#undef MBEDTLS_CHACHA20_C
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
#define MBEDTLS_CHACHAPOLY_C
#else
#undef MBEDTLS_CHACHAPOLY_C
#endif

/**
 * \def MBEDTLS_CIPHER_C
 *
 * Enable the generic cipher layer.
 *
 * Module:  library/cipher.c
 * Caller:  library/ccm.c
 *          library/cmac.c
 *          library/gcm.c
 *          library/nist_kw.c
 *          library/pkcs12.c
 *          library/pkcs5.c
 *          library/psa_crypto_aead.c
 *          library/psa_crypto_mac.c
 *          library/ssl_ciphersuites.c
 *          library/ssl_msg.c
 *          library/ssl_ticket.c (unless MBEDTLS_USE_PSA_CRYPTO is enabled)
 *
 * Uncomment to enable generic cipher wrappers.
 */
#define MBEDTLS_CIPHER_C

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
#define MBEDTLS_CTR_DRBG_C

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
 * \def MBEDTLS_DHM_C
 *
 * Enable the Diffie-Hellman-Merkle module.
 *
 * Module:  library/dhm.c
 * Caller:  library/ssl_tls.c
 *          library/ssl*_client.c
 *          library/ssl*_server.c
 *
 * This module is used by the following key exchanges:
 *      DHE-RSA, DHE-PSK
 */
#ifdef CONFIG_MBEDTLS_DHM_C
#define MBEDTLS_DHM_C
#else
#undef MBEDTLS_DHM_C
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
 * \def MBEDTLS_ENTROPY_C
 *
 * Enable the platform-specific entropy code.
 *
 * Module:  library/entropy.c
 * Caller:
 *
 * Requires: MBEDTLS_SHA512_C or MBEDTLS_SHA256_C
 *
 * This module provides a generic entropy pool
 */
#define MBEDTLS_ENTROPY_C

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
#define MBEDTLS_ERROR_C

/**
 * \def MBEDTLS_GCM_C
 *
 * Enable the Galois/Counter Mode (GCM).
 *
 * Module:  library/gcm.c
 *
 * Requires: MBEDTLS_CIPHER_C, MBEDTLS_AES_C or MBEDTLS_CAMELLIA_C or
 *                             MBEDTLS_ARIA_C
 *
 * This module enables the AES-GCM and CAMELLIA-GCM ciphersuites, if other
 * requisites are enabled as well.
 */
#ifdef CONFIG_MBEDTLS_GCM_C
#define MBEDTLS_GCM_C
#else
#undef MBEDTLS_GCM_C
#endif

/**
 * \def MBEDTLS_HKDF_C
 *
 * Enable the HKDF algorithm (RFC 5869).
 *
 * Module:  library/hkdf.c
 * Caller:
 *
 * Requires: MBEDTLS_MD_C
 *
 * This module enables support for the Hashed Message Authentication Code
 * (HMAC)-based key derivation function (HKDF).
 */
#ifdef CONFIG_MBEDTLS_HKDF_C
#define MBEDTLS_HKDF_C
#else
#undef MBEDTLS_HKDF_C
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
 * Uncomment to enable the HMAC_DRBG random number geerator.
 */
#define MBEDTLS_HMAC_DRBG_C

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
#define MBEDTLS_MD_C

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
#define MBEDTLS_MD5_C

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
 * \def MBEDTLS_OID_C
 *
 * Enable the OID database.
 *
 * Module:  library/oid.c
 * Caller:  library/asn1write.c
 *          library/pkcs5.c
 *          library/pkparse.c
 *          library/pkwrite.c
 *          library/rsa.c
 *          library/x509.c
 *          library/x509_create.c
 *          library/mbedtls_x509_crl.c
 *          library/mbedtls_x509_crt.c
 *          library/mbedtls_x509_csr.c
 *          library/x509write_crt.c
 *          library/mbedtls_x509write_csr.c
 *
 * This modules translates between OIDs and internal values.
 */
#define MBEDTLS_OID_C

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
#define MBEDTLS_PADLOCK_C

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
#define MBEDTLS_PK_C

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
#define MBEDTLS_PK_PARSE_C

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
#define MBEDTLS_PK_WRITE_C

/**
 * \def MBEDTLS_PKCS5_C
 *
 * Enable PKCS#5 functions.
 *
 * Module:  library/pkcs5.c
 *
 * Requires: MBEDTLS_CIPHER_C and MBEDTLS_MD_C
 *
 * This module adds support for the PKCS#5 functions.
 */
#define MBEDTLS_PKCS5_C

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
 * This module is required for the PKCS7 parsing modules.
 */
//#define MBEDTLS_PKCS7_C

/**
 * \def MBEDTLS_PKCS12_C
 *
 * Enable PKCS#12 PBE functions.
 * Adds algorithms for parsing PKCS#8 encrypted private keys
 *
 * Module:  library/pkcs12.c
 * Caller:  library/pkparse.c
 *
 * Requires: MBEDTLS_ASN1_PARSE_C, MBEDTLS_CIPHER_C, MBEDTLS_MD_C
 *
 * This module enables PKCS#12 functions.
 */
#define MBEDTLS_PKCS12_C

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
#define MBEDTLS_RSA_C

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
#define MBEDTLS_SHA1_C

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
#define MBEDTLS_SHA224_C

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
#define MBEDTLS_SHA256_C

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
#define MBEDTLS_SHA384_C
#define MBEDTLS_SHA512_C
#else
#undef MBEDTLS_SHA384_C
#undef MBEDTLS_SHA512_C
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
#define MBEDTLS_SSL_CACHE_C

/**
 * \def MBEDTLS_SSL_COOKIE_C
 *
 * Enable basic implementation of DTLS cookies for hello verification.
 *
 * Module:  library/ssl_cookie.c
 * Caller:
 */
#define MBEDTLS_SSL_COOKIE_C

/**
 * \def MBEDTLS_SSL_TICKET_C
 *
 * Enable an implementation of TLS server-side callbacks for session tickets.
 *
 * Module:  library/ssl_ticket.c
 * Caller:
 *
 * Requires: (MBEDTLS_CIPHER_C) &&
 *           (MBEDTLS_GCM_C || MBEDTLS_CCM_C || MBEDTLS_CHACHAPOLY_C)
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
 * Requires: MBEDTLS_CIPHER_C, MBEDTLS_MD_C
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
#define MBEDTLS_VERSION_C

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
#define MBEDTLS_X509_USE_C

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
#define MBEDTLS_X509_CRT_PARSE_C

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
#define MBEDTLS_X509_CREATE_C

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
#define MBEDTLS_X509_CRT_WRITE_C

/**
 * \def MBEDTLS_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION
 *
  * Alow the X509 parser to not break-off when parsing an X509 certificate
 * and encountering an unknown critical extension.
 *
 * Module:  library/x509_crt.c
 *
 * Requires: MBEDTLS_X509_CRT_PARSE_C
 *
 * This module is supports loading of certificates with extensions that
 * may not be supported by mbedtls.
 */
#ifdef CONFIG_MBEDTLS_ALLOW_UNSUPPORTED_CRITICAL_EXT
#define MBEDTLS_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION
#else
#undef MBEDTLS_X509_ALLOW_UNSUPPORTED_CRITICAL_EXTENSION
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
#define MBEDTLS_X509_CSR_WRITE_C

/**
 * \def MBEDTLS_XTEA_C
 *
 * Enable the XTEA block cipher.
 *
 * Module:  library/xtea.c
 * Caller:
 */
#ifdef CONFIG_MBEDTLS_XTEA_C
#define MBEDTLS_XTEA_C
#else
#undef MBEDTLS_XTEA_C
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

#define MBEDTLS_SSL_MAX_CONTENT_LEN             CONFIG_MBEDTLS_SSL_MAX_CONTENT_LEN /**< Maxium fragment length in bytes, determines the size of each of the two internal I/O buffers */

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

/** \def MBEDTLS_SSL_CID_PADDING_GRANULARITY
 *
 * This option controls the use of record plaintext padding
 * when using the Connection ID extension in DTLS 1.2.
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
#ifdef CONFIG_MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_CID_PADDING_GRANULARITY    CONFIG_MBEDTLS_SSL_CID_PADDING_GRANULARITY
#else
#undef MBEDTLS_SSL_CID_PADDING_GRANULARITY
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
 * any functino that is deprecated by mbedtls */
#define MBEDTLS_DEPRECATED_REMOVED

#endif /* ESP_CONFIG_H */
