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
#include "sdkconfig.h"

#ifndef ESP_TEE_MBEDTLS_CONFIG_H
#define ESP_TEE_MBEDTLS_CONFIG_H

#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_PLATFORM_MS_TIME_ALT
#undef MBEDTLS_TIMING_C
#define MBEDTLS_PLATFORM_C

#define MBEDTLS_CIPHER_C
#define MBEDTLS_AES_C
#define MBEDTLS_GCM_C
#define MBEDTLS_AES_ALT
#define MBEDTLS_GCM_ALT
#define MBEDTLS_CIPHER_MODE_XTS

#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_BIGNUM_C
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
#if CONFIG_SECURE_TEE_SEC_STG_SUPPORT_SECP192R1_SIGN
#define MBEDTLS_ECP_DP_SECP192R1_ENABLED
#endif
#define MBEDTLS_ECP_C
#define MBEDTLS_ECDSA_C

#define MBEDTLS_SHA1_C
#define MBEDTLS_SHA224_C
#define MBEDTLS_SHA256_C

#if CONFIG_MBEDTLS_HARDWARE_SHA
#define MBEDTLS_SHA1_ALT
#define MBEDTLS_SHA256_ALT
#endif

#ifdef CONFIG_MBEDTLS_HARDWARE_ECC
#define MBEDTLS_ECP_MUL_ALT
#define MBEDTLS_ECP_VERIFY_ALT
#endif

#define MBEDTLS_ENTROPY_C

#endif /* ESP_TEE_MBEDTLS_CONFIG_H */
