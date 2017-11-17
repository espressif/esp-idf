/*
 *  Elliptic curves over GF(p): curve-specific data and functions
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_ECP_C)

#include "mbedtls/ecp.h"

#include <string.h>

#if !defined(MBEDTLS_ECP_ALT)

#if ( defined(__ARMCC_VERSION) || defined(_MSC_VER) ) && \
    !defined(inline) && !defined(__cplusplus)
#define inline __inline
#endif

/*
 * Conversion macros for embedded constants:
 * build lists of mbedtls_mpi_uint's from lists of unsigned char's grouped by 8, 4 or 2
 */
#if defined(MBEDTLS_HAVE_INT32)

#define BYTES_TO_T_UINT_4( a, b, c, d )             \
    ( (mbedtls_mpi_uint) a <<  0 ) |                          \
    ( (mbedtls_mpi_uint) b <<  8 ) |                          \
    ( (mbedtls_mpi_uint) c << 16 ) |                          \
    ( (mbedtls_mpi_uint) d << 24 )

#define BYTES_TO_T_UINT_2( a, b )                   \
    BYTES_TO_T_UINT_4( a, b, 0, 0 )

#define BYTES_TO_T_UINT_8( a, b, c, d, e, f, g, h ) \
    BYTES_TO_T_UINT_4( a, b, c, d ),                \
    BYTES_TO_T_UINT_4( e, f, g, h )

#else /* 64-bits */

#define BYTES_TO_T_UINT_8( a, b, c, d, e, f, g, h ) \
    ( (mbedtls_mpi_uint) a <<  0 ) |                          \
    ( (mbedtls_mpi_uint) b <<  8 ) |                          \
    ( (mbedtls_mpi_uint) c << 16 ) |                          \
    ( (mbedtls_mpi_uint) d << 24 ) |                          \
    ( (mbedtls_mpi_uint) e << 32 ) |                          \
    ( (mbedtls_mpi_uint) f << 40 ) |                          \
    ( (mbedtls_mpi_uint) g << 48 ) |                          \
    ( (mbedtls_mpi_uint) h << 56 )

#define BYTES_TO_T_UINT_4( a, b, c, d )             \
    BYTES_TO_T_UINT_8( a, b, c, d, 0, 0, 0, 0 )

#define BYTES_TO_T_UINT_2( a, b )                   \
    BYTES_TO_T_UINT_8( a, b, 0, 0, 0, 0, 0, 0 )

#endif /* bits in mbedtls_mpi_uint */

/*
 * Note: the constants are in little-endian order
 * to be directly usable in MPIs
 */

/*
 * Domain parameters for secp192r1
 */
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
static const mbedtls_mpi_uint secp192r1_p[] = {
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
static const mbedtls_mpi_uint secp192r1_b[] = {
    BYTES_TO_T_UINT_8( 0xB1, 0xB9, 0x46, 0xC1, 0xEC, 0xDE, 0xB8, 0xFE ),
    BYTES_TO_T_UINT_8( 0x49, 0x30, 0x24, 0x72, 0xAB, 0xE9, 0xA7, 0x0F ),
    BYTES_TO_T_UINT_8( 0xE7, 0x80, 0x9C, 0xE5, 0x19, 0x05, 0x21, 0x64 ),
};
static const mbedtls_mpi_uint secp192r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x12, 0x10, 0xFF, 0x82, 0xFD, 0x0A, 0xFF, 0xF4 ),
    BYTES_TO_T_UINT_8( 0x00, 0x88, 0xA1, 0x43, 0xEB, 0x20, 0xBF, 0x7C ),
    BYTES_TO_T_UINT_8( 0xF6, 0x90, 0x30, 0xB0, 0x0E, 0xA8, 0x8D, 0x18 ),
};
static const mbedtls_mpi_uint secp192r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x11, 0x48, 0x79, 0x1E, 0xA1, 0x77, 0xF9, 0x73 ),
    BYTES_TO_T_UINT_8( 0xD5, 0xCD, 0x24, 0x6B, 0xED, 0x11, 0x10, 0x63 ),
    BYTES_TO_T_UINT_8( 0x78, 0xDA, 0xC8, 0xFF, 0x95, 0x2B, 0x19, 0x07 ),
};
static const mbedtls_mpi_uint secp192r1_n[] = {
    BYTES_TO_T_UINT_8( 0x31, 0x28, 0xD2, 0xB4, 0xB1, 0xC9, 0x6B, 0x14 ),
    BYTES_TO_T_UINT_8( 0x36, 0xF8, 0xDE, 0x99, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
#endif /* MBEDTLS_ECP_DP_SECP192R1_ENABLED */

/*
 * Domain parameters for secp224r1
 */
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
static const mbedtls_mpi_uint secp224r1_p[] = {
    BYTES_TO_T_UINT_8( 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ),
    BYTES_TO_T_UINT_8( 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 ),
};
static const mbedtls_mpi_uint secp224r1_b[] = {
    BYTES_TO_T_UINT_8( 0xB4, 0xFF, 0x55, 0x23, 0x43, 0x39, 0x0B, 0x27 ),
    BYTES_TO_T_UINT_8( 0xBA, 0xD8, 0xBF, 0xD7, 0xB7, 0xB0, 0x44, 0x50 ),
    BYTES_TO_T_UINT_8( 0x56, 0x32, 0x41, 0xF5, 0xAB, 0xB3, 0x04, 0x0C ),
    BYTES_TO_T_UINT_4( 0x85, 0x0A, 0x05, 0xB4 ),
};
static const mbedtls_mpi_uint secp224r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x21, 0x1D, 0x5C, 0x11, 0xD6, 0x80, 0x32, 0x34 ),
    BYTES_TO_T_UINT_8( 0x22, 0x11, 0xC2, 0x56, 0xD3, 0xC1, 0x03, 0x4A ),
    BYTES_TO_T_UINT_8( 0xB9, 0x90, 0x13, 0x32, 0x7F, 0xBF, 0xB4, 0x6B ),
    BYTES_TO_T_UINT_4( 0xBD, 0x0C, 0x0E, 0xB7 ),
};
static const mbedtls_mpi_uint secp224r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x34, 0x7E, 0x00, 0x85, 0x99, 0x81, 0xD5, 0x44 ),
    BYTES_TO_T_UINT_8( 0x64, 0x47, 0x07, 0x5A, 0xA0, 0x75, 0x43, 0xCD ),
    BYTES_TO_T_UINT_8( 0xE6, 0xDF, 0x22, 0x4C, 0xFB, 0x23, 0xF7, 0xB5 ),
    BYTES_TO_T_UINT_4( 0x88, 0x63, 0x37, 0xBD ),
};
static const mbedtls_mpi_uint secp224r1_n[] = {
    BYTES_TO_T_UINT_8( 0x3D, 0x2A, 0x5C, 0x5C, 0x45, 0x29, 0xDD, 0x13 ),
    BYTES_TO_T_UINT_8( 0x3E, 0xF0, 0xB8, 0xE0, 0xA2, 0x16, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_4( 0xFF, 0xFF, 0xFF, 0xFF ),
};
#endif /* MBEDTLS_ECP_DP_SECP224R1_ENABLED */

/*
 * Domain parameters for secp256r1
 */
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
static const mbedtls_mpi_uint secp256r1_p[] = {
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 ),
    BYTES_TO_T_UINT_8( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ),
    BYTES_TO_T_UINT_8( 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF ),
};
static const mbedtls_mpi_uint secp256r1_b[] = {
    BYTES_TO_T_UINT_8( 0x4B, 0x60, 0xD2, 0x27, 0x3E, 0x3C, 0xCE, 0x3B ),
    BYTES_TO_T_UINT_8( 0xF6, 0xB0, 0x53, 0xCC, 0xB0, 0x06, 0x1D, 0x65 ),
    BYTES_TO_T_UINT_8( 0xBC, 0x86, 0x98, 0x76, 0x55, 0xBD, 0xEB, 0xB3 ),
    BYTES_TO_T_UINT_8( 0xE7, 0x93, 0x3A, 0xAA, 0xD8, 0x35, 0xC6, 0x5A ),
};
static const mbedtls_mpi_uint secp256r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x96, 0xC2, 0x98, 0xD8, 0x45, 0x39, 0xA1, 0xF4 ),
    BYTES_TO_T_UINT_8( 0xA0, 0x33, 0xEB, 0x2D, 0x81, 0x7D, 0x03, 0x77 ),
    BYTES_TO_T_UINT_8( 0xF2, 0x40, 0xA4, 0x63, 0xE5, 0xE6, 0xBC, 0xF8 ),
    BYTES_TO_T_UINT_8( 0x47, 0x42, 0x2C, 0xE1, 0xF2, 0xD1, 0x17, 0x6B ),
};
static const mbedtls_mpi_uint secp256r1_gy[] = {
    BYTES_TO_T_UINT_8( 0xF5, 0x51, 0xBF, 0x37, 0x68, 0x40, 0xB6, 0xCB ),
    BYTES_TO_T_UINT_8( 0xCE, 0x5E, 0x31, 0x6B, 0x57, 0x33, 0xCE, 0x2B ),
    BYTES_TO_T_UINT_8( 0x16, 0x9E, 0x0F, 0x7C, 0x4A, 0xEB, 0xE7, 0x8E ),
    BYTES_TO_T_UINT_8( 0x9B, 0x7F, 0x1A, 0xFE, 0xE2, 0x42, 0xE3, 0x4F ),
};
static const mbedtls_mpi_uint secp256r1_n[] = {
    BYTES_TO_T_UINT_8( 0x51, 0x25, 0x63, 0xFC, 0xC2, 0xCA, 0xB9, 0xF3 ),
    BYTES_TO_T_UINT_8( 0x84, 0x9E, 0x17, 0xA7, 0xAD, 0xFA, 0xE6, 0xBC ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF ),
};
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */

/*
 * Domain parameters for secp384r1
 */
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
static const mbedtls_mpi_uint secp384r1_p[] = {
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00 ),
    BYTES_TO_T_UINT_8( 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
static const mbedtls_mpi_uint secp384r1_b[] = {
    BYTES_TO_T_UINT_8( 0xEF, 0x2A, 0xEC, 0xD3, 0xED, 0xC8, 0x85, 0x2A ),
    BYTES_TO_T_UINT_8( 0x9D, 0xD1, 0x2E, 0x8A, 0x8D, 0x39, 0x56, 0xC6 ),
    BYTES_TO_T_UINT_8( 0x5A, 0x87, 0x13, 0x50, 0x8F, 0x08, 0x14, 0x03 ),
    BYTES_TO_T_UINT_8( 0x12, 0x41, 0x81, 0xFE, 0x6E, 0x9C, 0x1D, 0x18 ),
    BYTES_TO_T_UINT_8( 0x19, 0x2D, 0xF8, 0xE3, 0x6B, 0x05, 0x8E, 0x98 ),
    BYTES_TO_T_UINT_8( 0xE4, 0xE7, 0x3E, 0xE2, 0xA7, 0x2F, 0x31, 0xB3 ),
};
static const mbedtls_mpi_uint secp384r1_gx[] = {
    BYTES_TO_T_UINT_8( 0xB7, 0x0A, 0x76, 0x72, 0x38, 0x5E, 0x54, 0x3A ),
    BYTES_TO_T_UINT_8( 0x6C, 0x29, 0x55, 0xBF, 0x5D, 0xF2, 0x02, 0x55 ),
    BYTES_TO_T_UINT_8( 0x38, 0x2A, 0x54, 0x82, 0xE0, 0x41, 0xF7, 0x59 ),
    BYTES_TO_T_UINT_8( 0x98, 0x9B, 0xA7, 0x8B, 0x62, 0x3B, 0x1D, 0x6E ),
    BYTES_TO_T_UINT_8( 0x74, 0xAD, 0x20, 0xF3, 0x1E, 0xC7, 0xB1, 0x8E ),
    BYTES_TO_T_UINT_8( 0x37, 0x05, 0x8B, 0xBE, 0x22, 0xCA, 0x87, 0xAA ),
};
static const mbedtls_mpi_uint secp384r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x5F, 0x0E, 0xEA, 0x90, 0x7C, 0x1D, 0x43, 0x7A ),
    BYTES_TO_T_UINT_8( 0x9D, 0x81, 0x7E, 0x1D, 0xCE, 0xB1, 0x60, 0x0A ),
    BYTES_TO_T_UINT_8( 0xC0, 0xB8, 0xF0, 0xB5, 0x13, 0x31, 0xDA, 0xE9 ),
    BYTES_TO_T_UINT_8( 0x7C, 0x14, 0x9A, 0x28, 0xBD, 0x1D, 0xF4, 0xF8 ),
    BYTES_TO_T_UINT_8( 0x29, 0xDC, 0x92, 0x92, 0xBF, 0x98, 0x9E, 0x5D ),
    BYTES_TO_T_UINT_8( 0x6F, 0x2C, 0x26, 0x96, 0x4A, 0xDE, 0x17, 0x36 ),
};
static const mbedtls_mpi_uint secp384r1_n[] = {
    BYTES_TO_T_UINT_8( 0x73, 0x29, 0xC5, 0xCC, 0x6A, 0x19, 0xEC, 0xEC ),
    BYTES_TO_T_UINT_8( 0x7A, 0xA7, 0xB0, 0x48, 0xB2, 0x0D, 0x1A, 0x58 ),
    BYTES_TO_T_UINT_8( 0xDF, 0x2D, 0x37, 0xF4, 0x81, 0x4D, 0x63, 0xC7 ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */

/*
 * Domain parameters for secp521r1
 */
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
static const mbedtls_mpi_uint secp521r1_p[] = {
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_2( 0xFF, 0x01 ),
};
static const mbedtls_mpi_uint secp521r1_b[] = {
    BYTES_TO_T_UINT_8( 0x00, 0x3F, 0x50, 0x6B, 0xD4, 0x1F, 0x45, 0xEF ),
    BYTES_TO_T_UINT_8( 0xF1, 0x34, 0x2C, 0x3D, 0x88, 0xDF, 0x73, 0x35 ),
    BYTES_TO_T_UINT_8( 0x07, 0xBF, 0xB1, 0x3B, 0xBD, 0xC0, 0x52, 0x16 ),
    BYTES_TO_T_UINT_8( 0x7B, 0x93, 0x7E, 0xEC, 0x51, 0x39, 0x19, 0x56 ),
    BYTES_TO_T_UINT_8( 0xE1, 0x09, 0xF1, 0x8E, 0x91, 0x89, 0xB4, 0xB8 ),
    BYTES_TO_T_UINT_8( 0xF3, 0x15, 0xB3, 0x99, 0x5B, 0x72, 0xDA, 0xA2 ),
    BYTES_TO_T_UINT_8( 0xEE, 0x40, 0x85, 0xB6, 0xA0, 0x21, 0x9A, 0x92 ),
    BYTES_TO_T_UINT_8( 0x1F, 0x9A, 0x1C, 0x8E, 0x61, 0xB9, 0x3E, 0x95 ),
    BYTES_TO_T_UINT_2( 0x51, 0x00 ),
};
static const mbedtls_mpi_uint secp521r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x66, 0xBD, 0xE5, 0xC2, 0x31, 0x7E, 0x7E, 0xF9 ),
    BYTES_TO_T_UINT_8( 0x9B, 0x42, 0x6A, 0x85, 0xC1, 0xB3, 0x48, 0x33 ),
    BYTES_TO_T_UINT_8( 0xDE, 0xA8, 0xFF, 0xA2, 0x27, 0xC1, 0x1D, 0xFE ),
    BYTES_TO_T_UINT_8( 0x28, 0x59, 0xE7, 0xEF, 0x77, 0x5E, 0x4B, 0xA1 ),
    BYTES_TO_T_UINT_8( 0xBA, 0x3D, 0x4D, 0x6B, 0x60, 0xAF, 0x28, 0xF8 ),
    BYTES_TO_T_UINT_8( 0x21, 0xB5, 0x3F, 0x05, 0x39, 0x81, 0x64, 0x9C ),
    BYTES_TO_T_UINT_8( 0x42, 0xB4, 0x95, 0x23, 0x66, 0xCB, 0x3E, 0x9E ),
    BYTES_TO_T_UINT_8( 0xCD, 0xE9, 0x04, 0x04, 0xB7, 0x06, 0x8E, 0x85 ),
    BYTES_TO_T_UINT_2( 0xC6, 0x00 ),
};
static const mbedtls_mpi_uint secp521r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x50, 0x66, 0xD1, 0x9F, 0x76, 0x94, 0xBE, 0x88 ),
    BYTES_TO_T_UINT_8( 0x40, 0xC2, 0x72, 0xA2, 0x86, 0x70, 0x3C, 0x35 ),
    BYTES_TO_T_UINT_8( 0x61, 0x07, 0xAD, 0x3F, 0x01, 0xB9, 0x50, 0xC5 ),
    BYTES_TO_T_UINT_8( 0x40, 0x26, 0xF4, 0x5E, 0x99, 0x72, 0xEE, 0x97 ),
    BYTES_TO_T_UINT_8( 0x2C, 0x66, 0x3E, 0x27, 0x17, 0xBD, 0xAF, 0x17 ),
    BYTES_TO_T_UINT_8( 0x68, 0x44, 0x9B, 0x57, 0x49, 0x44, 0xF5, 0x98 ),
    BYTES_TO_T_UINT_8( 0xD9, 0x1B, 0x7D, 0x2C, 0xB4, 0x5F, 0x8A, 0x5C ),
    BYTES_TO_T_UINT_8( 0x04, 0xC0, 0x3B, 0x9A, 0x78, 0x6A, 0x29, 0x39 ),
    BYTES_TO_T_UINT_2( 0x18, 0x01 ),
};
static const mbedtls_mpi_uint secp521r1_n[] = {
    BYTES_TO_T_UINT_8( 0x09, 0x64, 0x38, 0x91, 0x1E, 0xB7, 0x6F, 0xBB ),
    BYTES_TO_T_UINT_8( 0xAE, 0x47, 0x9C, 0x89, 0xB8, 0xC9, 0xB5, 0x3B ),
    BYTES_TO_T_UINT_8( 0xD0, 0xA5, 0x09, 0xF7, 0x48, 0x01, 0xCC, 0x7F ),
    BYTES_TO_T_UINT_8( 0x6B, 0x96, 0x2F, 0xBF, 0x83, 0x87, 0x86, 0x51 ),
    BYTES_TO_T_UINT_8( 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_2( 0xFF, 0x01 ),
};
#endif /* MBEDTLS_ECP_DP_SECP521R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
static const mbedtls_mpi_uint secp192k1_p[] = {
    BYTES_TO_T_UINT_8( 0x37, 0xEE, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
static const mbedtls_mpi_uint secp192k1_a[] = {
    BYTES_TO_T_UINT_2( 0x00, 0x00 ),
};
static const mbedtls_mpi_uint secp192k1_b[] = {
    BYTES_TO_T_UINT_2( 0x03, 0x00 ),
};
static const mbedtls_mpi_uint secp192k1_gx[] = {
    BYTES_TO_T_UINT_8( 0x7D, 0x6C, 0xE0, 0xEA, 0xB1, 0xD1, 0xA5, 0x1D ),
    BYTES_TO_T_UINT_8( 0x34, 0xF4, 0xB7, 0x80, 0x02, 0x7D, 0xB0, 0x26 ),
    BYTES_TO_T_UINT_8( 0xAE, 0xE9, 0x57, 0xC0, 0x0E, 0xF1, 0x4F, 0xDB ),
};
static const mbedtls_mpi_uint secp192k1_gy[] = {
    BYTES_TO_T_UINT_8( 0x9D, 0x2F, 0x5E, 0xD9, 0x88, 0xAA, 0x82, 0x40 ),
    BYTES_TO_T_UINT_8( 0x34, 0x86, 0xBE, 0x15, 0xD0, 0x63, 0x41, 0x84 ),
    BYTES_TO_T_UINT_8( 0xA7, 0x28, 0x56, 0x9C, 0x6D, 0x2F, 0x2F, 0x9B ),
};
static const mbedtls_mpi_uint secp192k1_n[] = {
    BYTES_TO_T_UINT_8( 0x8D, 0xFD, 0xDE, 0x74, 0x6A, 0x46, 0x69, 0x0F ),
    BYTES_TO_T_UINT_8( 0x17, 0xFC, 0xF2, 0x26, 0xFE, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
#endif /* MBEDTLS_ECP_DP_SECP192K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
static const mbedtls_mpi_uint secp224k1_p[] = {
    BYTES_TO_T_UINT_8( 0x6D, 0xE5, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_4( 0xFF, 0xFF, 0xFF, 0xFF ),
};
static const mbedtls_mpi_uint secp224k1_a[] = {
    BYTES_TO_T_UINT_2( 0x00, 0x00 ),
};
static const mbedtls_mpi_uint secp224k1_b[] = {
    BYTES_TO_T_UINT_2( 0x05, 0x00 ),
};
static const mbedtls_mpi_uint secp224k1_gx[] = {
    BYTES_TO_T_UINT_8( 0x5C, 0xA4, 0xB7, 0xB6, 0x0E, 0x65, 0x7E, 0x0F ),
    BYTES_TO_T_UINT_8( 0xA9, 0x75, 0x70, 0xE4, 0xE9, 0x67, 0xA4, 0x69 ),
    BYTES_TO_T_UINT_8( 0xA1, 0x28, 0xFC, 0x30, 0xDF, 0x99, 0xF0, 0x4D ),
    BYTES_TO_T_UINT_4( 0x33, 0x5B, 0x45, 0xA1 ),
};
static const mbedtls_mpi_uint secp224k1_gy[] = {
    BYTES_TO_T_UINT_8( 0xA5, 0x61, 0x6D, 0x55, 0xDB, 0x4B, 0xCA, 0xE2 ),
    BYTES_TO_T_UINT_8( 0x59, 0xBD, 0xB0, 0xC0, 0xF7, 0x19, 0xE3, 0xF7 ),
    BYTES_TO_T_UINT_8( 0xD6, 0xFB, 0xCA, 0x82, 0x42, 0x34, 0xBA, 0x7F ),
    BYTES_TO_T_UINT_4( 0xED, 0x9F, 0x08, 0x7E ),
};
static const mbedtls_mpi_uint secp224k1_n[] = {
    BYTES_TO_T_UINT_8( 0xF7, 0xB1, 0x9F, 0x76, 0x71, 0xA9, 0xF0, 0xCA ),
    BYTES_TO_T_UINT_8( 0x84, 0x61, 0xEC, 0xD2, 0xE8, 0xDC, 0x01, 0x00 ),
    BYTES_TO_T_UINT_8( 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 ),
    BYTES_TO_T_UINT_8( 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 ),
};
#endif /* MBEDTLS_ECP_DP_SECP224K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
static const mbedtls_mpi_uint secp256k1_p[] = {
    BYTES_TO_T_UINT_8( 0x2F, 0xFC, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
static const mbedtls_mpi_uint secp256k1_a[] = {
    BYTES_TO_T_UINT_2( 0x00, 0x00 ),
};
static const mbedtls_mpi_uint secp256k1_b[] = {
    BYTES_TO_T_UINT_2( 0x07, 0x00 ),
};
static const mbedtls_mpi_uint secp256k1_gx[] = {
    BYTES_TO_T_UINT_8( 0x98, 0x17, 0xF8, 0x16, 0x5B, 0x81, 0xF2, 0x59 ),
    BYTES_TO_T_UINT_8( 0xD9, 0x28, 0xCE, 0x2D, 0xDB, 0xFC, 0x9B, 0x02 ),
    BYTES_TO_T_UINT_8( 0x07, 0x0B, 0x87, 0xCE, 0x95, 0x62, 0xA0, 0x55 ),
    BYTES_TO_T_UINT_8( 0xAC, 0xBB, 0xDC, 0xF9, 0x7E, 0x66, 0xBE, 0x79 ),
};
static const mbedtls_mpi_uint secp256k1_gy[] = {
    BYTES_TO_T_UINT_8( 0xB8, 0xD4, 0x10, 0xFB, 0x8F, 0xD0, 0x47, 0x9C ),
    BYTES_TO_T_UINT_8( 0x19, 0x54, 0x85, 0xA6, 0x48, 0xB4, 0x17, 0xFD ),
    BYTES_TO_T_UINT_8( 0xA8, 0x08, 0x11, 0x0E, 0xFC, 0xFB, 0xA4, 0x5D ),
    BYTES_TO_T_UINT_8( 0x65, 0xC4, 0xA3, 0x26, 0x77, 0xDA, 0x3A, 0x48 ),
};
static const mbedtls_mpi_uint secp256k1_n[] = {
    BYTES_TO_T_UINT_8( 0x41, 0x41, 0x36, 0xD0, 0x8C, 0x5E, 0xD2, 0xBF ),
    BYTES_TO_T_UINT_8( 0x3B, 0xA0, 0x48, 0xAF, 0xE6, 0xDC, 0xAE, 0xBA ),
    BYTES_TO_T_UINT_8( 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
    BYTES_TO_T_UINT_8( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ),
};
#endif /* MBEDTLS_ECP_DP_SECP256K1_ENABLED */

/*
 * Domain parameters for brainpoolP256r1 (RFC 5639 3.4)
 */
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
static const mbedtls_mpi_uint brainpoolP256r1_p[] = {
    BYTES_TO_T_UINT_8( 0x77, 0x53, 0x6E, 0x1F, 0x1D, 0x48, 0x13, 0x20 ),
    BYTES_TO_T_UINT_8( 0x28, 0x20, 0x26, 0xD5, 0x23, 0xF6, 0x3B, 0x6E ),
    BYTES_TO_T_UINT_8( 0x72, 0x8D, 0x83, 0x9D, 0x90, 0x0A, 0x66, 0x3E ),
    BYTES_TO_T_UINT_8( 0xBC, 0xA9, 0xEE, 0xA1, 0xDB, 0x57, 0xFB, 0xA9 ),
};
static const mbedtls_mpi_uint brainpoolP256r1_a[] = {
    BYTES_TO_T_UINT_8( 0xD9, 0xB5, 0x30, 0xF3, 0x44, 0x4B, 0x4A, 0xE9 ),
    BYTES_TO_T_UINT_8( 0x6C, 0x5C, 0xDC, 0x26, 0xC1, 0x55, 0x80, 0xFB ),
    BYTES_TO_T_UINT_8( 0xE7, 0xFF, 0x7A, 0x41, 0x30, 0x75, 0xF6, 0xEE ),
    BYTES_TO_T_UINT_8( 0x57, 0x30, 0x2C, 0xFC, 0x75, 0x09, 0x5A, 0x7D ),
};
static const mbedtls_mpi_uint brainpoolP256r1_b[] = {
    BYTES_TO_T_UINT_8( 0xB6, 0x07, 0x8C, 0xFF, 0x18, 0xDC, 0xCC, 0x6B ),
    BYTES_TO_T_UINT_8( 0xCE, 0xE1, 0xF7, 0x5C, 0x29, 0x16, 0x84, 0x95 ),
    BYTES_TO_T_UINT_8( 0xBF, 0x7C, 0xD7, 0xBB, 0xD9, 0xB5, 0x30, 0xF3 ),
    BYTES_TO_T_UINT_8( 0x44, 0x4B, 0x4A, 0xE9, 0x6C, 0x5C, 0xDC, 0x26 ),
};
static const mbedtls_mpi_uint brainpoolP256r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x62, 0x32, 0xCE, 0x9A, 0xBD, 0x53, 0x44, 0x3A ),
    BYTES_TO_T_UINT_8( 0xC2, 0x23, 0xBD, 0xE3, 0xE1, 0x27, 0xDE, 0xB9 ),
    BYTES_TO_T_UINT_8( 0xAF, 0xB7, 0x81, 0xFC, 0x2F, 0x48, 0x4B, 0x2C ),
    BYTES_TO_T_UINT_8( 0xCB, 0x57, 0x7E, 0xCB, 0xB9, 0xAE, 0xD2, 0x8B ),
};
static const mbedtls_mpi_uint brainpoolP256r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x97, 0x69, 0x04, 0x2F, 0xC7, 0x54, 0x1D, 0x5C ),
    BYTES_TO_T_UINT_8( 0x54, 0x8E, 0xED, 0x2D, 0x13, 0x45, 0x77, 0xC2 ),
    BYTES_TO_T_UINT_8( 0xC9, 0x1D, 0x61, 0x14, 0x1A, 0x46, 0xF8, 0x97 ),
    BYTES_TO_T_UINT_8( 0xFD, 0xC4, 0xDA, 0xC3, 0x35, 0xF8, 0x7E, 0x54 ),
};
static const mbedtls_mpi_uint brainpoolP256r1_n[] = {
    BYTES_TO_T_UINT_8( 0xA7, 0x56, 0x48, 0x97, 0x82, 0x0E, 0x1E, 0x90 ),
    BYTES_TO_T_UINT_8( 0xF7, 0xA6, 0x61, 0xB5, 0xA3, 0x7A, 0x39, 0x8C ),
    BYTES_TO_T_UINT_8( 0x71, 0x8D, 0x83, 0x9D, 0x90, 0x0A, 0x66, 0x3E ),
    BYTES_TO_T_UINT_8( 0xBC, 0xA9, 0xEE, 0xA1, 0xDB, 0x57, 0xFB, 0xA9 ),
};
#endif /* MBEDTLS_ECP_DP_BP256R1_ENABLED */

/*
 * Domain parameters for brainpoolP384r1 (RFC 5639 3.6)
 */
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
static const mbedtls_mpi_uint brainpoolP384r1_p[] = {
    BYTES_TO_T_UINT_8( 0x53, 0xEC, 0x07, 0x31, 0x13, 0x00, 0x47, 0x87 ),
    BYTES_TO_T_UINT_8( 0x71, 0x1A, 0x1D, 0x90, 0x29, 0xA7, 0xD3, 0xAC ),
    BYTES_TO_T_UINT_8( 0x23, 0x11, 0xB7, 0x7F, 0x19, 0xDA, 0xB1, 0x12 ),
    BYTES_TO_T_UINT_8( 0xB4, 0x56, 0x54, 0xED, 0x09, 0x71, 0x2F, 0x15 ),
    BYTES_TO_T_UINT_8( 0xDF, 0x41, 0xE6, 0x50, 0x7E, 0x6F, 0x5D, 0x0F ),
    BYTES_TO_T_UINT_8( 0x28, 0x6D, 0x38, 0xA3, 0x82, 0x1E, 0xB9, 0x8C ),
};
static const mbedtls_mpi_uint brainpoolP384r1_a[] = {
    BYTES_TO_T_UINT_8( 0x26, 0x28, 0xCE, 0x22, 0xDD, 0xC7, 0xA8, 0x04 ),
    BYTES_TO_T_UINT_8( 0xEB, 0xD4, 0x3A, 0x50, 0x4A, 0x81, 0xA5, 0x8A ),
    BYTES_TO_T_UINT_8( 0x0F, 0xF9, 0x91, 0xBA, 0xEF, 0x65, 0x91, 0x13 ),
    BYTES_TO_T_UINT_8( 0x87, 0x27, 0xB2, 0x4F, 0x8E, 0xA2, 0xBE, 0xC2 ),
    BYTES_TO_T_UINT_8( 0xA0, 0xAF, 0x05, 0xCE, 0x0A, 0x08, 0x72, 0x3C ),
    BYTES_TO_T_UINT_8( 0x0C, 0x15, 0x8C, 0x3D, 0xC6, 0x82, 0xC3, 0x7B ),
};
static const mbedtls_mpi_uint brainpoolP384r1_b[] = {
    BYTES_TO_T_UINT_8( 0x11, 0x4C, 0x50, 0xFA, 0x96, 0x86, 0xB7, 0x3A ),
    BYTES_TO_T_UINT_8( 0x94, 0xC9, 0xDB, 0x95, 0x02, 0x39, 0xB4, 0x7C ),
    BYTES_TO_T_UINT_8( 0xD5, 0x62, 0xEB, 0x3E, 0xA5, 0x0E, 0x88, 0x2E ),
    BYTES_TO_T_UINT_8( 0xA6, 0xD2, 0xDC, 0x07, 0xE1, 0x7D, 0xB7, 0x2F ),
    BYTES_TO_T_UINT_8( 0x7C, 0x44, 0xF0, 0x16, 0x54, 0xB5, 0x39, 0x8B ),
    BYTES_TO_T_UINT_8( 0x26, 0x28, 0xCE, 0x22, 0xDD, 0xC7, 0xA8, 0x04 ),
};
static const mbedtls_mpi_uint brainpoolP384r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x1E, 0xAF, 0xD4, 0x47, 0xE2, 0xB2, 0x87, 0xEF ),
    BYTES_TO_T_UINT_8( 0xAA, 0x46, 0xD6, 0x36, 0x34, 0xE0, 0x26, 0xE8 ),
    BYTES_TO_T_UINT_8( 0xE8, 0x10, 0xBD, 0x0C, 0xFE, 0xCA, 0x7F, 0xDB ),
    BYTES_TO_T_UINT_8( 0xE3, 0x4F, 0xF1, 0x7E, 0xE7, 0xA3, 0x47, 0x88 ),
    BYTES_TO_T_UINT_8( 0x6B, 0x3F, 0xC1, 0xB7, 0x81, 0x3A, 0xA6, 0xA2 ),
    BYTES_TO_T_UINT_8( 0xFF, 0x45, 0xCF, 0x68, 0xF0, 0x64, 0x1C, 0x1D ),
};
static const mbedtls_mpi_uint brainpoolP384r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x15, 0x53, 0x3C, 0x26, 0x41, 0x03, 0x82, 0x42 ),
    BYTES_TO_T_UINT_8( 0x11, 0x81, 0x91, 0x77, 0x21, 0x46, 0x46, 0x0E ),
    BYTES_TO_T_UINT_8( 0x28, 0x29, 0x91, 0xF9, 0x4F, 0x05, 0x9C, 0xE1 ),
    BYTES_TO_T_UINT_8( 0x64, 0x58, 0xEC, 0xFE, 0x29, 0x0B, 0xB7, 0x62 ),
    BYTES_TO_T_UINT_8( 0x52, 0xD5, 0xCF, 0x95, 0x8E, 0xEB, 0xB1, 0x5C ),
    BYTES_TO_T_UINT_8( 0xA4, 0xC2, 0xF9, 0x20, 0x75, 0x1D, 0xBE, 0x8A ),
};
static const mbedtls_mpi_uint brainpoolP384r1_n[] = {
    BYTES_TO_T_UINT_8( 0x65, 0x65, 0x04, 0xE9, 0x02, 0x32, 0x88, 0x3B ),
    BYTES_TO_T_UINT_8( 0x10, 0xC3, 0x7F, 0x6B, 0xAF, 0xB6, 0x3A, 0xCF ),
    BYTES_TO_T_UINT_8( 0xA7, 0x25, 0x04, 0xAC, 0x6C, 0x6E, 0x16, 0x1F ),
    BYTES_TO_T_UINT_8( 0xB3, 0x56, 0x54, 0xED, 0x09, 0x71, 0x2F, 0x15 ),
    BYTES_TO_T_UINT_8( 0xDF, 0x41, 0xE6, 0x50, 0x7E, 0x6F, 0x5D, 0x0F ),
    BYTES_TO_T_UINT_8( 0x28, 0x6D, 0x38, 0xA3, 0x82, 0x1E, 0xB9, 0x8C ),
};
#endif /* MBEDTLS_ECP_DP_BP384R1_ENABLED */

/*
 * Domain parameters for brainpoolP512r1 (RFC 5639 3.7)
 */
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
static const mbedtls_mpi_uint brainpoolP512r1_p[] = {
    BYTES_TO_T_UINT_8( 0xF3, 0x48, 0x3A, 0x58, 0x56, 0x60, 0xAA, 0x28 ),
    BYTES_TO_T_UINT_8( 0x85, 0xC6, 0x82, 0x2D, 0x2F, 0xFF, 0x81, 0x28 ),
    BYTES_TO_T_UINT_8( 0xE6, 0x80, 0xA3, 0xE6, 0x2A, 0xA1, 0xCD, 0xAE ),
    BYTES_TO_T_UINT_8( 0x42, 0x68, 0xC6, 0x9B, 0x00, 0x9B, 0x4D, 0x7D ),
    BYTES_TO_T_UINT_8( 0x71, 0x08, 0x33, 0x70, 0xCA, 0x9C, 0x63, 0xD6 ),
    BYTES_TO_T_UINT_8( 0x0E, 0xD2, 0xC9, 0xB3, 0xB3, 0x8D, 0x30, 0xCB ),
    BYTES_TO_T_UINT_8( 0x07, 0xFC, 0xC9, 0x33, 0xAE, 0xE6, 0xD4, 0x3F ),
    BYTES_TO_T_UINT_8( 0x8B, 0xC4, 0xE9, 0xDB, 0xB8, 0x9D, 0xDD, 0xAA ),
};
static const mbedtls_mpi_uint brainpoolP512r1_a[] = {
    BYTES_TO_T_UINT_8( 0xCA, 0x94, 0xFC, 0x77, 0x4D, 0xAC, 0xC1, 0xE7 ),
    BYTES_TO_T_UINT_8( 0xB9, 0xC7, 0xF2, 0x2B, 0xA7, 0x17, 0x11, 0x7F ),
    BYTES_TO_T_UINT_8( 0xB5, 0xC8, 0x9A, 0x8B, 0xC9, 0xF1, 0x2E, 0x0A ),
    BYTES_TO_T_UINT_8( 0xA1, 0x3A, 0x25, 0xA8, 0x5A, 0x5D, 0xED, 0x2D ),
    BYTES_TO_T_UINT_8( 0xBC, 0x63, 0x98, 0xEA, 0xCA, 0x41, 0x34, 0xA8 ),
    BYTES_TO_T_UINT_8( 0x10, 0x16, 0xF9, 0x3D, 0x8D, 0xDD, 0xCB, 0x94 ),
    BYTES_TO_T_UINT_8( 0xC5, 0x4C, 0x23, 0xAC, 0x45, 0x71, 0x32, 0xE2 ),
    BYTES_TO_T_UINT_8( 0x89, 0x3B, 0x60, 0x8B, 0x31, 0xA3, 0x30, 0x78 ),
};
static const mbedtls_mpi_uint brainpoolP512r1_b[] = {
    BYTES_TO_T_UINT_8( 0x23, 0xF7, 0x16, 0x80, 0x63, 0xBD, 0x09, 0x28 ),
    BYTES_TO_T_UINT_8( 0xDD, 0xE5, 0xBA, 0x5E, 0xB7, 0x50, 0x40, 0x98 ),
    BYTES_TO_T_UINT_8( 0x67, 0x3E, 0x08, 0xDC, 0xCA, 0x94, 0xFC, 0x77 ),
    BYTES_TO_T_UINT_8( 0x4D, 0xAC, 0xC1, 0xE7, 0xB9, 0xC7, 0xF2, 0x2B ),
    BYTES_TO_T_UINT_8( 0xA7, 0x17, 0x11, 0x7F, 0xB5, 0xC8, 0x9A, 0x8B ),
    BYTES_TO_T_UINT_8( 0xC9, 0xF1, 0x2E, 0x0A, 0xA1, 0x3A, 0x25, 0xA8 ),
    BYTES_TO_T_UINT_8( 0x5A, 0x5D, 0xED, 0x2D, 0xBC, 0x63, 0x98, 0xEA ),
    BYTES_TO_T_UINT_8( 0xCA, 0x41, 0x34, 0xA8, 0x10, 0x16, 0xF9, 0x3D ),
};
static const mbedtls_mpi_uint brainpoolP512r1_gx[] = {
    BYTES_TO_T_UINT_8( 0x22, 0xF8, 0xB9, 0xBC, 0x09, 0x22, 0x35, 0x8B ),
    BYTES_TO_T_UINT_8( 0x68, 0x5E, 0x6A, 0x40, 0x47, 0x50, 0x6D, 0x7C ),
    BYTES_TO_T_UINT_8( 0x5F, 0x7D, 0xB9, 0x93, 0x7B, 0x68, 0xD1, 0x50 ),
    BYTES_TO_T_UINT_8( 0x8D, 0xD4, 0xD0, 0xE2, 0x78, 0x1F, 0x3B, 0xFF ),
    BYTES_TO_T_UINT_8( 0x8E, 0x09, 0xD0, 0xF4, 0xEE, 0x62, 0x3B, 0xB4 ),
    BYTES_TO_T_UINT_8( 0xC1, 0x16, 0xD9, 0xB5, 0x70, 0x9F, 0xED, 0x85 ),
    BYTES_TO_T_UINT_8( 0x93, 0x6A, 0x4C, 0x9C, 0x2E, 0x32, 0x21, 0x5A ),
    BYTES_TO_T_UINT_8( 0x64, 0xD9, 0x2E, 0xD8, 0xBD, 0xE4, 0xAE, 0x81 ),
};
static const mbedtls_mpi_uint brainpoolP512r1_gy[] = {
    BYTES_TO_T_UINT_8( 0x92, 0x08, 0xD8, 0x3A, 0x0F, 0x1E, 0xCD, 0x78 ),
    BYTES_TO_T_UINT_8( 0x06, 0x54, 0xF0, 0xA8, 0x2F, 0x2B, 0xCA, 0xD1 ),
    BYTES_TO_T_UINT_8( 0xAE, 0x63, 0x27, 0x8A, 0xD8, 0x4B, 0xCA, 0x5B ),
    BYTES_TO_T_UINT_8( 0x5E, 0x48, 0x5F, 0x4A, 0x49, 0xDE, 0xDC, 0xB2 ),
    BYTES_TO_T_UINT_8( 0x11, 0x81, 0x1F, 0x88, 0x5B, 0xC5, 0x00, 0xA0 ),
    BYTES_TO_T_UINT_8( 0x1A, 0x7B, 0xA5, 0x24, 0x00, 0xF7, 0x09, 0xF2 ),
    BYTES_TO_T_UINT_8( 0xFD, 0x22, 0x78, 0xCF, 0xA9, 0xBF, 0xEA, 0xC0 ),
    BYTES_TO_T_UINT_8( 0xEC, 0x32, 0x63, 0x56, 0x5D, 0x38, 0xDE, 0x7D ),
};
static const mbedtls_mpi_uint brainpoolP512r1_n[] = {
    BYTES_TO_T_UINT_8( 0x69, 0x00, 0xA9, 0x9C, 0x82, 0x96, 0x87, 0xB5 ),
    BYTES_TO_T_UINT_8( 0xDD, 0xDA, 0x5D, 0x08, 0x81, 0xD3, 0xB1, 0x1D ),
    BYTES_TO_T_UINT_8( 0x47, 0x10, 0xAC, 0x7F, 0x19, 0x61, 0x86, 0x41 ),
    BYTES_TO_T_UINT_8( 0x19, 0x26, 0xA9, 0x4C, 0x41, 0x5C, 0x3E, 0x55 ),
    BYTES_TO_T_UINT_8( 0x70, 0x08, 0x33, 0x70, 0xCA, 0x9C, 0x63, 0xD6 ),
    BYTES_TO_T_UINT_8( 0x0E, 0xD2, 0xC9, 0xB3, 0xB3, 0x8D, 0x30, 0xCB ),
    BYTES_TO_T_UINT_8( 0x07, 0xFC, 0xC9, 0x33, 0xAE, 0xE6, 0xD4, 0x3F ),
    BYTES_TO_T_UINT_8( 0x8B, 0xC4, 0xE9, 0xDB, 0xB8, 0x9D, 0xDD, 0xAA ),
};
#endif /* MBEDTLS_ECP_DP_BP512R1_ENABLED */

/*
 * Create an MPI from embedded constants
 * (assumes len is an exact multiple of sizeof mbedtls_mpi_uint)
 */
static inline void ecp_mpi_load( mbedtls_mpi *X, const mbedtls_mpi_uint *p, size_t len )
{
    X->s = 1;
    X->n = len / sizeof( mbedtls_mpi_uint );
    X->p = (mbedtls_mpi_uint *) p;
}

/*
 * Set an MPI to static value 1
 */
static inline void ecp_mpi_set1( mbedtls_mpi *X )
{
    static mbedtls_mpi_uint one[] = { 1 };
    X->s = 1;
    X->n = 1;
    X->p = one;
}

/*
 * Make group available from embedded constants
 */
static int ecp_group_load( mbedtls_ecp_group *grp,
                           const mbedtls_mpi_uint *p,  size_t plen,
                           const mbedtls_mpi_uint *a,  size_t alen,
                           const mbedtls_mpi_uint *b,  size_t blen,
                           const mbedtls_mpi_uint *gx, size_t gxlen,
                           const mbedtls_mpi_uint *gy, size_t gylen,
                           const mbedtls_mpi_uint *n,  size_t nlen)
{
    ecp_mpi_load( &grp->P, p, plen );
    if( a != NULL )
        ecp_mpi_load( &grp->A, a, alen );
    ecp_mpi_load( &grp->B, b, blen );
    ecp_mpi_load( &grp->N, n, nlen );

    ecp_mpi_load( &grp->G.X, gx, gxlen );
    ecp_mpi_load( &grp->G.Y, gy, gylen );
    ecp_mpi_set1( &grp->G.Z );

    grp->pbits = mbedtls_mpi_bitlen( &grp->P );
    grp->nbits = mbedtls_mpi_bitlen( &grp->N );

    grp->h = 1;

    return( 0 );
}

#if defined(MBEDTLS_ECP_NIST_OPTIM)
/* Forward declarations */
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
static int ecp_mod_p192( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
static int ecp_mod_p224( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
static int ecp_mod_p256( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
static int ecp_mod_p384( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
static int ecp_mod_p521( mbedtls_mpi * );
#endif

#define NIST_MODP( P )      grp->modp = ecp_mod_ ## P;
#else
#define NIST_MODP( P )
#endif /* MBEDTLS_ECP_NIST_OPTIM */

/* Additional forward declarations */
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
static int ecp_mod_p255( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
static int ecp_mod_p192k1( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
static int ecp_mod_p224k1( mbedtls_mpi * );
#endif
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
static int ecp_mod_p256k1( mbedtls_mpi * );
#endif

#define LOAD_GROUP_A( G )   ecp_group_load( grp,            \
                            G ## _p,  sizeof( G ## _p  ),   \
                            G ## _a,  sizeof( G ## _a  ),   \
                            G ## _b,  sizeof( G ## _b  ),   \
                            G ## _gx, sizeof( G ## _gx ),   \
                            G ## _gy, sizeof( G ## _gy ),   \
                            G ## _n,  sizeof( G ## _n  ) )

#define LOAD_GROUP( G )     ecp_group_load( grp,            \
                            G ## _p,  sizeof( G ## _p  ),   \
                            NULL,     0,                    \
                            G ## _b,  sizeof( G ## _b  ),   \
                            G ## _gx, sizeof( G ## _gx ),   \
                            G ## _gy, sizeof( G ## _gy ),   \
                            G ## _n,  sizeof( G ## _n  ) )

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
/*
 * Specialized function for creating the Curve25519 group
 */
static int ecp_use_curve25519( mbedtls_ecp_group *grp )
{
    int ret;

    /* Actually ( A + 2 ) / 4 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_string( &grp->A, 16, "01DB42" ) );

    /* P = 2^255 - 19 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &grp->P, 1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_shift_l( &grp->P, 255 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_int( &grp->P, &grp->P, 19 ) );
    grp->pbits = mbedtls_mpi_bitlen( &grp->P );

    /* Y intentionaly not set, since we use x/z coordinates.
     * This is used as a marker to identify Montgomery curves! */
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &grp->G.X, 9 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_lset( &grp->G.Z, 1 ) );
    mbedtls_mpi_free( &grp->G.Y );

    /* Actually, the required msb for private keys */
    grp->nbits = 254;

cleanup:
    if( ret != 0 )
        mbedtls_ecp_group_free( grp );

    return( ret );
}
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */

/*
 * Set a group using well-known domain parameters
 */
int mbedtls_ecp_group_load( mbedtls_ecp_group *grp, mbedtls_ecp_group_id id )
{
    mbedtls_ecp_group_free( grp );

    grp->id = id;

    switch( id )
    {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
        case MBEDTLS_ECP_DP_SECP192R1:
            NIST_MODP( p192 );
            return( LOAD_GROUP( secp192r1 ) );
#endif /* MBEDTLS_ECP_DP_SECP192R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
        case MBEDTLS_ECP_DP_SECP224R1:
            NIST_MODP( p224 );
            return( LOAD_GROUP( secp224r1 ) );
#endif /* MBEDTLS_ECP_DP_SECP224R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
        case MBEDTLS_ECP_DP_SECP256R1:
            NIST_MODP( p256 );
            return( LOAD_GROUP( secp256r1 ) );
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
        case MBEDTLS_ECP_DP_SECP384R1:
            NIST_MODP( p384 );
            return( LOAD_GROUP( secp384r1 ) );
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
        case MBEDTLS_ECP_DP_SECP521R1:
            NIST_MODP( p521 );
            return( LOAD_GROUP( secp521r1 ) );
#endif /* MBEDTLS_ECP_DP_SECP521R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
        case MBEDTLS_ECP_DP_SECP192K1:
            grp->modp = ecp_mod_p192k1;
            return( LOAD_GROUP_A( secp192k1 ) );
#endif /* MBEDTLS_ECP_DP_SECP192K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
        case MBEDTLS_ECP_DP_SECP224K1:
            grp->modp = ecp_mod_p224k1;
            return( LOAD_GROUP_A( secp224k1 ) );
#endif /* MBEDTLS_ECP_DP_SECP224K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
        case MBEDTLS_ECP_DP_SECP256K1:
            grp->modp = ecp_mod_p256k1;
            return( LOAD_GROUP_A( secp256k1 ) );
#endif /* MBEDTLS_ECP_DP_SECP256K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
        case MBEDTLS_ECP_DP_BP256R1:
            return( LOAD_GROUP_A( brainpoolP256r1 ) );
#endif /* MBEDTLS_ECP_DP_BP256R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
        case MBEDTLS_ECP_DP_BP384R1:
            return( LOAD_GROUP_A( brainpoolP384r1 ) );
#endif /* MBEDTLS_ECP_DP_BP384R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
        case MBEDTLS_ECP_DP_BP512R1:
            return( LOAD_GROUP_A( brainpoolP512r1 ) );
#endif /* MBEDTLS_ECP_DP_BP512R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
        case MBEDTLS_ECP_DP_CURVE25519:
            grp->modp = ecp_mod_p255;
            return( ecp_use_curve25519( grp ) );
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */

        default:
            mbedtls_ecp_group_free( grp );
            return( MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE );
    }
}

#if defined(MBEDTLS_ECP_NIST_OPTIM)
/*
 * Fast reduction modulo the primes used by the NIST curves.
 *
 * These functions are critical for speed, but not needed for correct
 * operations. So, we make the choice to heavily rely on the internals of our
 * bignum library, which creates a tight coupling between these functions and
 * our MPI implementation.  However, the coupling between the ECP module and
 * MPI remains loose, since these functions can be deactivated at will.
 */

#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
/*
 * Compared to the way things are presented in FIPS 186-3 D.2,
 * we proceed in columns, from right (least significant chunk) to left,
 * adding chunks to N in place, and keeping a carry for the next chunk.
 * This avoids moving things around in memory, and uselessly adding zeros,
 * compared to the more straightforward, line-oriented approach.
 *
 * For this prime we need to handle data in chunks of 64 bits.
 * Since this is always a multiple of our basic mbedtls_mpi_uint, we can
 * use a mbedtls_mpi_uint * to designate such a chunk, and small loops to handle it.
 */

/* Add 64-bit chunks (dst += src) and update carry */
static inline void add64( mbedtls_mpi_uint *dst, mbedtls_mpi_uint *src, mbedtls_mpi_uint *carry )
{
    unsigned char i;
    mbedtls_mpi_uint c = 0;
    for( i = 0; i < 8 / sizeof( mbedtls_mpi_uint ); i++, dst++, src++ )
    {
        *dst += c;      c  = ( *dst < c );
        *dst += *src;   c += ( *dst < *src );
    }
    *carry += c;
}

/* Add carry to a 64-bit chunk and update carry */
static inline void carry64( mbedtls_mpi_uint *dst, mbedtls_mpi_uint *carry )
{
    unsigned char i;
    for( i = 0; i < 8 / sizeof( mbedtls_mpi_uint ); i++, dst++ )
    {
        *dst += *carry;
        *carry  = ( *dst < *carry );
    }
}

#define WIDTH       8 / sizeof( mbedtls_mpi_uint )
#define A( i )      N->p + i * WIDTH
#define ADD( i )    add64( p, A( i ), &c )
#define NEXT        p += WIDTH; carry64( p, &c )
#define LAST        p += WIDTH; *p = c; while( ++p < end ) *p = 0

/*
 * Fast quasi-reduction modulo p192 (FIPS 186-3 D.2.1)
 */
static int ecp_mod_p192( mbedtls_mpi *N )
{
    int ret;
    mbedtls_mpi_uint c = 0;
    mbedtls_mpi_uint *p, *end;

    /* Make sure we have enough blocks so that A(5) is legal */
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( N, 6 * WIDTH ) );

    p = N->p;
    end = p + N->n;

    ADD( 3 ); ADD( 5 );             NEXT; // A0 += A3 + A5
    ADD( 3 ); ADD( 4 ); ADD( 5 );   NEXT; // A1 += A3 + A4 + A5
    ADD( 4 ); ADD( 5 );             LAST; // A2 += A4 + A5

cleanup:
    return( ret );
}

#undef WIDTH
#undef A
#undef ADD
#undef NEXT
#undef LAST
#endif /* MBEDTLS_ECP_DP_SECP192R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
/*
 * The reader is advised to first understand ecp_mod_p192() since the same
 * general structure is used here, but with additional complications:
 * (1) chunks of 32 bits, and (2) subtractions.
 */

/*
 * For these primes, we need to handle data in chunks of 32 bits.
 * This makes it more complicated if we use 64 bits limbs in MPI,
 * which prevents us from using a uniform access method as for p192.
 *
 * So, we define a mini abstraction layer to access 32 bit chunks,
 * load them in 'cur' for work, and store them back from 'cur' when done.
 *
 * While at it, also define the size of N in terms of 32-bit chunks.
 */
#define LOAD32      cur = A( i );

#if defined(MBEDTLS_HAVE_INT32)  /* 32 bit */

#define MAX32       N->n
#define A( j )      N->p[j]
#define STORE32     N->p[i] = cur;

#else                               /* 64-bit */

#define MAX32       N->n * 2
#define A( j ) j % 2 ? (uint32_t)( N->p[j/2] >> 32 ) : (uint32_t)( N->p[j/2] )
#define STORE32                                   \
    if( i % 2 ) {                                 \
        N->p[i/2] &= 0x00000000FFFFFFFF;          \
        N->p[i/2] |= ((mbedtls_mpi_uint) cur) << 32;        \
    } else {                                      \
        N->p[i/2] &= 0xFFFFFFFF00000000;          \
        N->p[i/2] |= (mbedtls_mpi_uint) cur;                \
    }

#endif /* sizeof( mbedtls_mpi_uint ) */

/*
 * Helpers for addition and subtraction of chunks, with signed carry.
 */
static inline void add32( uint32_t *dst, uint32_t src, signed char *carry )
{
    *dst += src;
    *carry += ( *dst < src );
}

static inline void sub32( uint32_t *dst, uint32_t src, signed char *carry )
{
    *carry -= ( *dst < src );
    *dst -= src;
}

#define ADD( j )    add32( &cur, A( j ), &c );
#define SUB( j )    sub32( &cur, A( j ), &c );

/*
 * Helpers for the main 'loop'
 * (see fix_negative for the motivation of C)
 */
#define INIT( b )                                           \
    int ret;                                                \
    signed char c = 0, cc;                                  \
    uint32_t cur;                                           \
    size_t i = 0, bits = b;                                 \
    mbedtls_mpi C;                                                  \
    mbedtls_mpi_uint Cp[ b / 8 / sizeof( mbedtls_mpi_uint) + 1 ];               \
                                                            \
    C.s = 1;                                                \
    C.n = b / 8 / sizeof( mbedtls_mpi_uint) + 1;                      \
    C.p = Cp;                                               \
    memset( Cp, 0, C.n * sizeof( mbedtls_mpi_uint ) );                \
                                                            \
    MBEDTLS_MPI_CHK( mbedtls_mpi_grow( N, b * 2 / 8 / sizeof( mbedtls_mpi_uint ) ) ); \
    LOAD32;

#define NEXT                    \
    STORE32; i++; LOAD32;       \
    cc = c; c = 0;              \
    if( cc < 0 )                \
        sub32( &cur, -cc, &c ); \
    else                        \
        add32( &cur, cc, &c );  \

#define LAST                                    \
    STORE32; i++;                               \
    cur = c > 0 ? c : 0; STORE32;               \
    cur = 0; while( ++i < MAX32 ) { STORE32; }  \
    if( c < 0 ) fix_negative( N, c, &C, bits );

/*
 * If the result is negative, we get it in the form
 * c * 2^(bits + 32) + N, with c negative and N positive shorter than 'bits'
 */
static inline int fix_negative( mbedtls_mpi *N, signed char c, mbedtls_mpi *C, size_t bits )
{
    int ret;

    /* C = - c * 2^(bits + 32) */
#if !defined(MBEDTLS_HAVE_INT64)
    ((void) bits);
#else
    if( bits == 224 )
        C->p[ C->n - 1 ] = ((mbedtls_mpi_uint) -c) << 32;
    else
#endif
        C->p[ C->n - 1 ] = (mbedtls_mpi_uint) -c;

    /* N = - ( C - N ) */
    MBEDTLS_MPI_CHK( mbedtls_mpi_sub_abs( N, C, N ) );
    N->s = -1;

cleanup:

    return( ret );
}

#if defined(MBEDTLS_ECP_DP_SECP224R1_ENABLED)
/*
 * Fast quasi-reduction modulo p224 (FIPS 186-3 D.2.2)
 */
static int ecp_mod_p224( mbedtls_mpi *N )
{
    INIT( 224 );

    SUB(  7 ); SUB( 11 );               NEXT; // A0 += -A7 - A11
    SUB(  8 ); SUB( 12 );               NEXT; // A1 += -A8 - A12
    SUB(  9 ); SUB( 13 );               NEXT; // A2 += -A9 - A13
    SUB( 10 ); ADD(  7 ); ADD( 11 );    NEXT; // A3 += -A10 + A7 + A11
    SUB( 11 ); ADD(  8 ); ADD( 12 );    NEXT; // A4 += -A11 + A8 + A12
    SUB( 12 ); ADD(  9 ); ADD( 13 );    NEXT; // A5 += -A12 + A9 + A13
    SUB( 13 ); ADD( 10 );               LAST; // A6 += -A13 + A10

cleanup:
    return( ret );
}
#endif /* MBEDTLS_ECP_DP_SECP224R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
/*
 * Fast quasi-reduction modulo p256 (FIPS 186-3 D.2.3)
 */
static int ecp_mod_p256( mbedtls_mpi *N )
{
    INIT( 256 );

    ADD(  8 ); ADD(  9 );
    SUB( 11 ); SUB( 12 ); SUB( 13 ); SUB( 14 );             NEXT; // A0

    ADD(  9 ); ADD( 10 );
    SUB( 12 ); SUB( 13 ); SUB( 14 ); SUB( 15 );             NEXT; // A1

    ADD( 10 ); ADD( 11 );
    SUB( 13 ); SUB( 14 ); SUB( 15 );                        NEXT; // A2

    ADD( 11 ); ADD( 11 ); ADD( 12 ); ADD( 12 ); ADD( 13 );
    SUB( 15 ); SUB(  8 ); SUB(  9 );                        NEXT; // A3

    ADD( 12 ); ADD( 12 ); ADD( 13 ); ADD( 13 ); ADD( 14 );
    SUB(  9 ); SUB( 10 );                                   NEXT; // A4

    ADD( 13 ); ADD( 13 ); ADD( 14 ); ADD( 14 ); ADD( 15 );
    SUB( 10 ); SUB( 11 );                                   NEXT; // A5

    ADD( 14 ); ADD( 14 ); ADD( 15 ); ADD( 15 ); ADD( 14 ); ADD( 13 );
    SUB(  8 ); SUB(  9 );                                   NEXT; // A6

    ADD( 15 ); ADD( 15 ); ADD( 15 ); ADD( 8 );
    SUB( 10 ); SUB( 11 ); SUB( 12 ); SUB( 13 );             LAST; // A7

cleanup:
    return( ret );
}
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
/*
 * Fast quasi-reduction modulo p384 (FIPS 186-3 D.2.4)
 */
static int ecp_mod_p384( mbedtls_mpi *N )
{
    INIT( 384 );

    ADD( 12 ); ADD( 21 ); ADD( 20 );
    SUB( 23 );                                              NEXT; // A0

    ADD( 13 ); ADD( 22 ); ADD( 23 );
    SUB( 12 ); SUB( 20 );                                   NEXT; // A2

    ADD( 14 ); ADD( 23 );
    SUB( 13 ); SUB( 21 );                                   NEXT; // A2

    ADD( 15 ); ADD( 12 ); ADD( 20 ); ADD( 21 );
    SUB( 14 ); SUB( 22 ); SUB( 23 );                        NEXT; // A3

    ADD( 21 ); ADD( 21 ); ADD( 16 ); ADD( 13 ); ADD( 12 ); ADD( 20 ); ADD( 22 );
    SUB( 15 ); SUB( 23 ); SUB( 23 );                        NEXT; // A4

    ADD( 22 ); ADD( 22 ); ADD( 17 ); ADD( 14 ); ADD( 13 ); ADD( 21 ); ADD( 23 );
    SUB( 16 );                                              NEXT; // A5

    ADD( 23 ); ADD( 23 ); ADD( 18 ); ADD( 15 ); ADD( 14 ); ADD( 22 );
    SUB( 17 );                                              NEXT; // A6

    ADD( 19 ); ADD( 16 ); ADD( 15 ); ADD( 23 );
    SUB( 18 );                                              NEXT; // A7

    ADD( 20 ); ADD( 17 ); ADD( 16 );
    SUB( 19 );                                              NEXT; // A8

    ADD( 21 ); ADD( 18 ); ADD( 17 );
    SUB( 20 );                                              NEXT; // A9

    ADD( 22 ); ADD( 19 ); ADD( 18 );
    SUB( 21 );                                              NEXT; // A10

    ADD( 23 ); ADD( 20 ); ADD( 19 );
    SUB( 22 );                                              LAST; // A11

cleanup:
    return( ret );
}
#endif /* MBEDTLS_ECP_DP_SECP384R1_ENABLED */

#undef A
#undef LOAD32
#undef STORE32
#undef MAX32
#undef INIT
#undef NEXT
#undef LAST

#endif /* MBEDTLS_ECP_DP_SECP224R1_ENABLED ||
          MBEDTLS_ECP_DP_SECP256R1_ENABLED ||
          MBEDTLS_ECP_DP_SECP384R1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
/*
 * Here we have an actual Mersenne prime, so things are more straightforward.
 * However, chunks are aligned on a 'weird' boundary (521 bits).
 */

/* Size of p521 in terms of mbedtls_mpi_uint */
#define P521_WIDTH      ( 521 / 8 / sizeof( mbedtls_mpi_uint ) + 1 )

/* Bits to keep in the most significant mbedtls_mpi_uint */
#define P521_MASK       0x01FF

/*
 * Fast quasi-reduction modulo p521 (FIPS 186-3 D.2.5)
 * Write N as A1 + 2^521 A0, return A0 + A1
 */
static int ecp_mod_p521( mbedtls_mpi *N )
{
    int ret;
    size_t i;
    mbedtls_mpi M;
    mbedtls_mpi_uint Mp[P521_WIDTH + 1];
    /* Worst case for the size of M is when mbedtls_mpi_uint is 16 bits:
     * we need to hold bits 513 to 1056, which is 34 limbs, that is
     * P521_WIDTH + 1. Otherwise P521_WIDTH is enough. */

    if( N->n < P521_WIDTH )
        return( 0 );

    /* M = A1 */
    M.s = 1;
    M.n = N->n - ( P521_WIDTH - 1 );
    if( M.n > P521_WIDTH + 1 )
        M.n = P521_WIDTH + 1;
    M.p = Mp;
    memcpy( Mp, N->p + P521_WIDTH - 1, M.n * sizeof( mbedtls_mpi_uint ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &M, 521 % ( 8 * sizeof( mbedtls_mpi_uint ) ) ) );

    /* N = A0 */
    N->p[P521_WIDTH - 1] &= P521_MASK;
    for( i = P521_WIDTH; i < N->n; i++ )
        N->p[i] = 0;

    /* N = A0 + A1 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_abs( N, N, &M ) );

cleanup:
    return( ret );
}

#undef P521_WIDTH
#undef P521_MASK
#endif /* MBEDTLS_ECP_DP_SECP521R1_ENABLED */

#endif /* MBEDTLS_ECP_NIST_OPTIM */

#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)

/* Size of p255 in terms of mbedtls_mpi_uint */
#define P255_WIDTH      ( 255 / 8 / sizeof( mbedtls_mpi_uint ) + 1 )

/*
 * Fast quasi-reduction modulo p255 = 2^255 - 19
 * Write N as A0 + 2^255 A1, return A0 + 19 * A1
 */
static int ecp_mod_p255( mbedtls_mpi *N )
{
    int ret;
    size_t i;
    mbedtls_mpi M;
    mbedtls_mpi_uint Mp[P255_WIDTH + 2];

    if( N->n < P255_WIDTH )
        return( 0 );

    /* M = A1 */
    M.s = 1;
    M.n = N->n - ( P255_WIDTH - 1 );
    if( M.n > P255_WIDTH + 1 )
        M.n = P255_WIDTH + 1;
    M.p = Mp;
    memset( Mp, 0, sizeof Mp );
    memcpy( Mp, N->p + P255_WIDTH - 1, M.n * sizeof( mbedtls_mpi_uint ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &M, 255 % ( 8 * sizeof( mbedtls_mpi_uint ) ) ) );
    M.n++; /* Make room for multiplication by 19 */

    /* N = A0 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_set_bit( N, 255, 0 ) );
    for( i = P255_WIDTH; i < N->n; i++ )
        N->p[i] = 0;

    /* N = A0 + 19 * A1 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_int( &M, &M, 19 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_abs( N, N, &M ) );

cleanup:
    return( ret );
}
#endif /* MBEDTLS_ECP_DP_CURVE25519_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED) ||   \
    defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
/*
 * Fast quasi-reduction modulo P = 2^s - R,
 * with R about 33 bits, used by the Koblitz curves.
 *
 * Write N as A0 + 2^224 A1, return A0 + R * A1.
 * Actually do two passes, since R is big.
 */
#define P_KOBLITZ_MAX   ( 256 / 8 / sizeof( mbedtls_mpi_uint ) )  // Max limbs in P
#define P_KOBLITZ_R     ( 8 / sizeof( mbedtls_mpi_uint ) )        // Limbs in R
static inline int ecp_mod_koblitz( mbedtls_mpi *N, mbedtls_mpi_uint *Rp, size_t p_limbs,
                                   size_t adjust, size_t shift, mbedtls_mpi_uint mask )
{
    int ret;
    size_t i;
    mbedtls_mpi M, R;
    mbedtls_mpi_uint Mp[P_KOBLITZ_MAX + P_KOBLITZ_R + 1];

    if( N->n < p_limbs )
        return( 0 );

    /* Init R */
    R.s = 1;
    R.p = Rp;
    R.n = P_KOBLITZ_R;

    /* Common setup for M */
    M.s = 1;
    M.p = Mp;

    /* M = A1 */
    M.n = N->n - ( p_limbs - adjust );
    if( M.n > p_limbs + adjust )
        M.n = p_limbs + adjust;
    memset( Mp, 0, sizeof Mp );
    memcpy( Mp, N->p + p_limbs - adjust, M.n * sizeof( mbedtls_mpi_uint ) );
    if( shift != 0 )
        MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &M, shift ) );
    M.n += R.n; /* Make room for multiplication by R */

    /* N = A0 */
    if( mask != 0 )
        N->p[p_limbs - 1] &= mask;
    for( i = p_limbs; i < N->n; i++ )
        N->p[i] = 0;

    /* N = A0 + R * A1 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &M, &M, &R ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_abs( N, N, &M ) );

    /* Second pass */

    /* M = A1 */
    M.n = N->n - ( p_limbs - adjust );
    if( M.n > p_limbs + adjust )
        M.n = p_limbs + adjust;
    memset( Mp, 0, sizeof Mp );
    memcpy( Mp, N->p + p_limbs - adjust, M.n * sizeof( mbedtls_mpi_uint ) );
    if( shift != 0 )
        MBEDTLS_MPI_CHK( mbedtls_mpi_shift_r( &M, shift ) );
    M.n += R.n; /* Make room for multiplication by R */

    /* N = A0 */
    if( mask != 0 )
        N->p[p_limbs - 1] &= mask;
    for( i = p_limbs; i < N->n; i++ )
        N->p[i] = 0;

    /* N = A0 + R * A1 */
    MBEDTLS_MPI_CHK( mbedtls_mpi_mul_mpi( &M, &M, &R ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_add_abs( N, N, &M ) );

cleanup:
    return( ret );
}
#endif /* MBEDTLS_ECP_DP_SECP192K1_ENABLED) ||
          MBEDTLS_ECP_DP_SECP224K1_ENABLED) ||
          MBEDTLS_ECP_DP_SECP256K1_ENABLED) */

#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
/*
 * Fast quasi-reduction modulo p192k1 = 2^192 - R,
 * with R = 2^32 + 2^12 + 2^8 + 2^7 + 2^6 + 2^3 + 1 = 0x0100001119
 */
static int ecp_mod_p192k1( mbedtls_mpi *N )
{
    static mbedtls_mpi_uint Rp[] = {
        BYTES_TO_T_UINT_8( 0xC9, 0x11, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 ) };

    return( ecp_mod_koblitz( N, Rp, 192 / 8 / sizeof( mbedtls_mpi_uint ), 0, 0, 0 ) );
}
#endif /* MBEDTLS_ECP_DP_SECP192K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP224K1_ENABLED)
/*
 * Fast quasi-reduction modulo p224k1 = 2^224 - R,
 * with R = 2^32 + 2^12 + 2^11 + 2^9 + 2^7 + 2^4 + 2 + 1 = 0x0100001A93
 */
static int ecp_mod_p224k1( mbedtls_mpi *N )
{
    static mbedtls_mpi_uint Rp[] = {
        BYTES_TO_T_UINT_8( 0x93, 0x1A, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 ) };

#if defined(MBEDTLS_HAVE_INT64)
    return( ecp_mod_koblitz( N, Rp, 4, 1, 32, 0xFFFFFFFF ) );
#else
    return( ecp_mod_koblitz( N, Rp, 224 / 8 / sizeof( mbedtls_mpi_uint ), 0, 0, 0 ) );
#endif
}

#endif /* MBEDTLS_ECP_DP_SECP224K1_ENABLED */

#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
/*
 * Fast quasi-reduction modulo p256k1 = 2^256 - R,
 * with R = 2^32 + 2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1 = 0x01000003D1
 */
static int ecp_mod_p256k1( mbedtls_mpi *N )
{
    static mbedtls_mpi_uint Rp[] = {
        BYTES_TO_T_UINT_8( 0xD1, 0x03, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00 ) };
    return( ecp_mod_koblitz( N, Rp, 256 / 8 / sizeof( mbedtls_mpi_uint ), 0, 0, 0 ) );
}
#endif /* MBEDTLS_ECP_DP_SECP256K1_ENABLED */

#endif /* !MBEDTLS_ECP_ALT */

#endif /* MBEDTLS_ECP_C */
