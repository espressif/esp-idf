/**
 * \file pkcs7.h
 *
 * \brief PKCS#7 Cryptographic Message Syntax
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
#ifndef MBEDTLS_PKCS7_H
#define MBEDTLS_PKCS7_H

#include "asn1.h"
#include "md.h"
#include "x509_crt.h"

#include <stddef.h>

/**
 * \name PKCS7 Error codes
 * \{
 */
#define MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA        -0x1800  /**< Bad input parameters to function. */
#define MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE   -0x1880  /**< Feature not available. */
#define MBEDTLS_ERR_PKCS7_UNKNOWN_FORMAT        -0x1900  /**< Unknown format. */
#define MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT   -0x1980  /**< ASN.1 data not as expected. */
#define MBEDTLS_ERR_PKCS7_INVALID_ARGUMENT      -0x1A00  /**< Invalid argument. */
/* \} name */

/* PKCS#7 content types, ref RFC 2315 (Section 14) */
#define MBEDTLS_PKCS7_CONTENT_TYPE_DATA                  1   /* 1.2.840.113549.1.7.1 */
#define MBEDTLS_PKCS7_CONTENT_TYPE_SIGNED                2   /* 1.2.840.113549.1.7.2 */
#define MBEDTLS_PKCS7_CONTENT_TYPE_ENVELOPE              3   /* 1.2.840.113549.1.7.3 */
#define MBEDTLS_PKCS7_CONTENT_TYPE_SIGNED_AND_ENVELOPED  4   /* 1.2.840.113549.1.7.4 */
#define MBEDTLS_PKCS7_CONTENT_TYPE_DIGEST                5   /* 1.2.840.113549.1.7.5 */
#define MBEDTLS_PKCS7_CONTENT_TYPE_ENCRYPTED             6   /* 1.2.840.113549.1.7.6 */

#define MBEDTLS_PKCS7_FORMAT_DER	1
#define MBEDTLS_PKCS7_FORMAT_PEM	2

#define MBEDTLS_PKCS7_PEM_HEADER ("-----BEGIN PKCS7-----")
#define MBEDTLS_PKCS7_PEM_FOOTER ("-----END PKCS7-----")

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Type-length-value structure that allows for ASN1 using DER.
 */
typedef mbedtls_asn1_buf mbedtls_pkcs7_buf;


/**
 * Container for PKCS#7 data
 */
struct mbedtls_pkcs7;

typedef struct mbedtls_pkcs7_issuer_and_serial {
    mbedtls_pkcs7_buf *issuer;
    mbedtls_pkcs7_buf *serial;
} mbedtls_pkcs7_issuer_and_serial;


typedef struct mbedtls_pkcs7_digest {
    mbedtls_md_type_t alg;
    unsigned char *d;
    size_t d_len;
} mbedtls_pkcs7_digest;


typedef struct mbedtls_pkcs7_signer_info {
    int version;
    mbedtls_pkcs7_issuer_and_serial *issuer_and_serial;
    mbedtls_pkcs7_digest *digest;
    mbedtls_pkcs7_digest *digest_enc;

    mbedtls_pk_context *pkey;
} mbedtls_pkcs7_signer_info;


typedef struct mbedtls_pkcs7_signed {
    unsigned char *raw;
    size_t raw_len;

    int version;
    mbedtls_pkcs7_buf *md_algs;
    mbedtls_x509_crt *certs;
    mbedtls_pkcs7_signer_info *signer_info;

    struct mbedtls_pkcs7 *content;
} mbedtls_pkcs7_signed;


typedef struct mbedtls_pkcs7 {
    unsigned char *raw;
    size_t raw_len;

    int content_type;

    union {
        mbedtls_pkcs7_buf data;
        mbedtls_pkcs7_signed signed_data;
    } d;

} mbedtls_pkcs7;


/**
 * \brief          Initialize a PKCS#7 data
 *
 * \param p7       PKCS#7 data to initialize
 */
void mbedtls_pkcs7_init( mbedtls_pkcs7 *p7 );


/**
 * \brief          Free a PKCS#7 data
 *
 * \param p7       PKCS#7 data to free
 */
void mbedtls_pkcs7_free( mbedtls_pkcs7 *p7 );


/**
 * \brief          Parse a DER formatted PKCS#7 data
 *
 * \param p7       points to the PKCS7 data type
 * \param buf      buffer holding the DER data
 * \param buflen   size of the buffer
 *
 * \return         0 if successful, or a specific PKCS7 or PEM error code
 */
int mbedtls_pkcs7_parse_der( mbedtls_pkcs7 *p7, const unsigned char *buf,
                             size_t buflen );

/**
 * \brief          Parse one or more certificates and add them
 *                 to the chained list. Parses permissively. If some
 *                 certificates can be parsed, the result is the number
 *                 of failed certificates it encountered. If none complete
 *                 correctly, the first error is returned.
 *
 * \param p7       points to PKCS#7 data type
 * \param buf      buffer holding the certificate data in PEM, DER or BASE64 format
 * \param buflen   size of the buffer
 *                 (including the terminating null byte for PEM or BASE64 data)
 *
 * \return         0 if all certificates parsed successfully, a positive number
 *                 if partly successful or a specific X509 or PEM error code
 */
int mbedtls_pkcs7_parse( mbedtls_pkcs7 *p7, const unsigned char *buf, size_t buflen );


#ifdef __cplusplus
}
#endif

#endif /* MBEDTLS_PKCS7_H */
