/*
 *  PKCS#7 Cryptographic Message Syntax
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
/*
 *  PKCS #7 "Cryptographic Message Syntax" version 1.5
 *  RFC 2315
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#define PKCS7_ASN1_SET (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SET)
#define PKCS7_ASN1_SEQUENCE (MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE)
#define PKCS7_ASN1_CONTEXT_0 (MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_ASN1_CONSTRUCTED | 0)
#define PKCS7_ASN1_CONTEXT_1 (MBEDTLS_ASN1_CONTEXT_SPECIFIC | MBEDTLS_ASN1_CONSTRUCTED | 1)

#if defined(MBEDTLS_PKCS7_C)

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#define mbedtls_free       free
#define mbedtls_calloc    calloc
#define mbedtls_snprintf   snprintf
#endif

#include "mbedtls/pkcs7.h"
#include "mbedtls/pem.h"

#include <string.h>

const unsigned char mbedtls_pkcs7_oid_data[] =                 { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x01 };
const unsigned char mbedtls_pkcs7_oid_signed[] =                { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x02 };
const unsigned char mbedtls_pkcs7_oid_enveloped[] =            { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x03 };
const unsigned char mbedtls_pkcs7_oid_signed_and_enveloped[] = { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x04 };
const unsigned char mbedtls_pkcs7_oid_digest[] =                { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x05 };
const unsigned char mbedtls_pkcs7_oid_encrypted[] =            { 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x07, 0x06 };

static int mbedtls_pkcs7_get_oid(unsigned char **p, const unsigned char *end, int *oid_type)
{
    size_t len = 0;
    int ret;
    unsigned char *pp;

    if (p == NULL || end == NULL || oid_type == NULL)
        return ( MBEDTLS_ERR_PKCS7_INVALID_ARGUMENT );

    pp = *p;
    ret = mbedtls_asn1_get_tag( &pp, end, &len, MBEDTLS_ASN1_OID );
    if ( ret != 0 )
        return( ret );

    if ( len == sizeof(mbedtls_pkcs7_oid_data) &&  memcmp(mbedtls_pkcs7_oid_data, pp, len) == 0)
        *oid_type = MBEDTLS_PKCS7_CONTENT_TYPE_DATA;
    else if ( len == sizeof(mbedtls_pkcs7_oid_signed) &&  memcmp(mbedtls_pkcs7_oid_signed, pp, len) == 0)
        *oid_type = MBEDTLS_PKCS7_CONTENT_TYPE_SIGNED;
    else if ( len == sizeof(mbedtls_pkcs7_oid_enveloped) &&  memcmp(mbedtls_pkcs7_oid_enveloped, pp, len) == 0)
        *oid_type = MBEDTLS_PKCS7_CONTENT_TYPE_ENVELOPE;
    else if ( len == sizeof(mbedtls_pkcs7_oid_signed_and_enveloped) &&  memcmp(mbedtls_pkcs7_oid_signed_and_enveloped, pp, len) == 0)
        *oid_type = MBEDTLS_PKCS7_CONTENT_TYPE_SIGNED_AND_ENVELOPED;
    else if ( len == sizeof(mbedtls_pkcs7_oid_digest) &&  memcmp(mbedtls_pkcs7_oid_digest, pp, len) == 0)
        *oid_type = MBEDTLS_PKCS7_CONTENT_TYPE_DIGEST;
    else if ( len == sizeof(mbedtls_pkcs7_oid_encrypted) &&  memcmp(mbedtls_pkcs7_oid_encrypted, pp, len) == 0)
        *oid_type = MBEDTLS_PKCS7_CONTENT_TYPE_ENCRYPTED;
    else
        return ( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    *p = pp + len;
    return( 0 );
}


static int mbedtls_pkcs7_signed_add_cert( mbedtls_pkcs7_signed *p7s, unsigned char *buf, size_t buflen)
{
    mbedtls_x509_crt *certs = NULL;
    int ret;

    if( p7s == NULL || buf == NULL || buflen == 0 )
        return( MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA );

    if ( p7s->certs == NULL) {
        certs = mbedtls_calloc(1, sizeof(mbedtls_x509_crt));
        if (certs == NULL)
            return( MBEDTLS_ERR_ASN1_ALLOC_FAILED );

        mbedtls_x509_crt_init(certs);
    } else {
        certs = p7s->certs;
    }

    ret = mbedtls_x509_crt_parse_der( certs, buf, buflen);
    if ( ret != 0 )
        return( ret );

    p7s->certs = certs;

    return( ret );
}


/*
 * SignedData ::= SEQUENCE {
 *   version           Version,
 *   digestAlgorithms  DigestAlgorithmIdentifiers,
 *   contentInfo       ContentInfo,
 *   certificates      [0]  CertificateSet OPTIONAL,
 *   crls              [1]  CertificateRevocationLists OPTIONAL,
 *   signerInfos       SignerInfos
 *   }
 */
static int mbedtls_pkcs7_parse_signed( mbedtls_pkcs7_signed *p7s, const unsigned char *buf, size_t buflen )
{
    unsigned char *p, *end;
    size_t len;
    int ret;

    if( p7s == NULL || buf == NULL || buflen == 0 )
        return( MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA );

    p = (unsigned char *)buf;
    end = p + buflen;
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_SEQUENCE );
    if( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    p7s->raw = p;
    p7s->raw_len = len;

    /*
     * version Version,
     */
    ret = mbedtls_asn1_get_int( &p, end, &p7s->version );
    if ( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    /*
     * digestAlgorithms DigestAlgorithmIdentifiers,
     */
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_SET );
    if( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    if ( len != 0 )
        return (MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE);

    p += len;

    /*
     * contentInfo       ContentInfo,
     */
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_SEQUENCE );
    if( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );
    if ( len != 0) {
        unsigned char *pp = p, *eend = p + len;

        p7s->content = mbedtls_calloc( 1, sizeof( mbedtls_pkcs7 ) );
        if ( p7s->content == NULL )
            return( MBEDTLS_ERR_ASN1_ALLOC_FAILED );

        ret = mbedtls_pkcs7_get_oid(&pp, eend, &p7s->content->content_type);
        if( ret != 0 )
            return ( ret );

        if (pp < eend)
            return( MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE );
    } else {
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );
    }

    p += len;

    /*
     * certificates [0] CertificateSet OPTIONAL,
     */
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_CONTEXT_0 );
    if( ret != MBEDTLS_ERR_ASN1_UNEXPECTED_TAG && ret != 0 ) {
        return( ret );
    } else if ( ret == 0 ) {
        unsigned char *pp = p, *eend = p + len;
        size_t llen = 0;

        do {
            unsigned char *ppp = pp;

            ret = mbedtls_asn1_get_tag( &pp, eend, &llen, PKCS7_ASN1_SEQUENCE );
            if (ret == MBEDTLS_ERR_ASN1_OUT_OF_DATA)
                break;
            else if( ret != 0 )
                return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

            ret = mbedtls_pkcs7_signed_add_cert( p7s, ppp, llen + (pp - ppp));
            if ( ret != 0)
                return( ret );

            pp += llen;
        } while ( ret == 0 );

        p += len;
    }

    /*
     * crls [1] CertificateRevocationLists OPTIONAL,
     */
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_CONTEXT_1 );
    if( ret != MBEDTLS_ERR_ASN1_UNEXPECTED_TAG && ret != 0 ) {
        return( ret );
    } else if ( ret == 0 ) {
        if ( len != 0 )
            return( MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE );
        p += len;
    }

    /*
     * signerInfos SignerInfos
     */
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_SET );
    if( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    return( ret );
}


/*
 * Initialize a PKCS#7 data
 */
void mbedtls_pkcs7_init( mbedtls_pkcs7 *p7 )
{
    memset( p7, 0, sizeof(mbedtls_pkcs7) );
}


/*
 * Free PKCS#7 data
 */
void mbedtls_pkcs7_free_signed( mbedtls_pkcs7_signed *p7s )
{
    if ( p7s == NULL )
        return;

    if (p7s->certs) {
        mbedtls_x509_crt_free( p7s->certs );
        mbedtls_free( p7s->certs );
        p7s->certs = NULL;
    }

    if ( p7s->content ) {
        mbedtls_pkcs7_free( p7s->content );
        mbedtls_free( p7s->content );
        p7s->content = NULL;
    }

    memset( p7s, 0, sizeof( mbedtls_pkcs7_signed ) );
}


void mbedtls_pkcs7_free( mbedtls_pkcs7 *p7 )
{
    if ( p7 == NULL )
        return;

    switch(p7->content_type) {
    case MBEDTLS_PKCS7_CONTENT_TYPE_SIGNED:
        mbedtls_pkcs7_free_signed(&p7->d.signed_data);
        return;
    case MBEDTLS_PKCS7_CONTENT_TYPE_DATA:
        memset( &p7->d.data, 0, sizeof(p7->d.data));
        return;
    }

    memset( p7, 0, sizeof(mbedtls_pkcs7) );
}


/*
 * Parse one PKCS#7 data in DER format from a buffer
 */
int mbedtls_pkcs7_parse_der( mbedtls_pkcs7 *p7, const unsigned char *buf, size_t buflen )
{
    unsigned char *p, *end;
    size_t len;
    int ret;

    /*
     * Check for valid input
     */
    if( p7 == NULL || buf == NULL || buflen == 0 )
        return( MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA );

    // Use the original buffer until we figure out actual length
    p = (unsigned char*) buf;
    len = buflen;
    end = p + len;

    /*
     * ContentInfo ::= SEQUENCE {
     *   content-type   PKCS7-CONTENT-TYPE.&id({PKCS7ContentTable}),
     *   pkcs7-content  [0]  PKCS7-CONTENT-TYPE.&Type({PKCS7ContentTable})
     *   }
     */
    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_SEQUENCE );
    if( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    p7->raw = (unsigned char *)buf;
    p7->raw_len = len + (p - buf);

    end = p + len;
    ret = mbedtls_pkcs7_get_oid(&p, end, &p7->content_type);
    if( ret != 0 )
        return ( ret );

    ret = mbedtls_asn1_get_tag( &p, end, &len, PKCS7_ASN1_CONTEXT_0);
    if ( ret != 0 )
        return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

    switch(p7->content_type) {
    case MBEDTLS_PKCS7_CONTENT_TYPE_SIGNED:
        ret = mbedtls_pkcs7_parse_signed( &p7->d.signed_data, p, len );
        break;
    case MBEDTLS_PKCS7_CONTENT_TYPE_DATA:
        ret = mbedtls_asn1_get_tag( &p, end, &len, MBEDTLS_ASN1_OCTET_STRING);
        if ( ret != 0 )
            return( MBEDTLS_ERR_PKCS7_INVALID_ASN1_FORMAT );

        p7->d.data.p = p;
        p7->d.data.len = len;
        p7->d.data.tag = MBEDTLS_ASN1_OCTET_STRING;
        break;
    default:
        ret = MBEDTLS_ERR_PKCS7_FEATURE_UNAVAILABLE;
        break;
    }

    return ( ret );
}

/*
 * Parse one or more PEM certificates from a buffer and add them to the chained
 * list
 */
int mbedtls_pkcs7_parse( mbedtls_pkcs7 *p7, const unsigned char *buf, size_t buflen )
{
    int buf_format = MBEDTLS_PKCS7_FORMAT_DER;
    int ret;

    /*
     * Check for valid input
     */
    if( p7 == NULL || buf == NULL || buflen == 0)
        return( MBEDTLS_ERR_PKCS7_BAD_INPUT_DATA );

    /*
     * Determine buffer content. Buffer contains either one DER certificate or
     * one or more PEM certificates.
     */
#if defined(MBEDTLS_PEM_PARSE_C)
    if( buflen != 0 && buf[buflen - 1] == '\0')
        if (strstr( (const char *) buf, MBEDTLS_PKCS7_PEM_HEADER ) == (const char *) buf )
            buf_format = MBEDTLS_PKCS7_FORMAT_PEM;

    if( buf_format == MBEDTLS_PKCS7_FORMAT_DER ) {
        ret = mbedtls_pkcs7_parse_der( p7, buf, buflen );
        return ( ret );
    }
#else
    ret = mbedtls_pkcs7_parse_der( p7, buf, buflen );
    return ( ret );
#endif

#if defined(MBEDTLS_PEM_PARSE_C)
    if( buf_format == MBEDTLS_PKCS7_FORMAT_PEM ) {
        mbedtls_pem_context pem;
        size_t use_len;
        int ret;

        mbedtls_pem_init( &pem );

        /* If we get there, we know the string is null-terminated */
        ret = mbedtls_pem_read_buffer( &pem, MBEDTLS_PKCS7_PEM_HEADER, MBEDTLS_PKCS7_PEM_FOOTER, buf, NULL, 0, &use_len );
        if( ret != 0 )
            return( ret );

        ret = mbedtls_pkcs7_parse_der( p7, pem.buf, pem.buflen );
        mbedtls_pem_free( &pem );

        return( ret );
    }

    return( MBEDTLS_ERR_PKCS7_UNKNOWN_FORMAT );
#endif /* MBEDTLS_PEM_PARSE_C */
}

#endif /* MBEDTLS_PKCS7_C */
