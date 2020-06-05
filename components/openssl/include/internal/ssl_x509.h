// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SSL_X509_H_
#define _SSL_X509_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "ssl_types.h"
#include "ssl_stack.h"

DEFINE_STACK_OF(X509_NAME)

/**
 * @brief create a X509 certification object according to input X509 certification
 *
 * @param ix - input X509 certification point
 *
 * @return new X509 certification object point
 */
X509* __X509_new(X509 *ix);

/**
 * @brief create a X509 certification object
 *
 * @param none
 *
 * @return X509 certification object point
 */
X509* X509_new(void);

/**
 * @brief load a character certification context into system context. If '*cert' is pointed to the
 *        certification, then load certification into it. Or create a new X509 certification object
 *
 * @param cert   - a point pointed to X509 certification
 * @param buffer - a point pointed to the certification context memory point
 * @param length - certification bytes
 *
 * @return X509 certification object point
 */
X509* d2i_X509(X509 **cert, const unsigned char *buffer, long len);

/**
 * @brief free a X509 certification object
 *
 * @param x - X509 certification object point
 *
 * @return none
 */
void X509_free(X509 *x);

/**
 * @brief set SSL context client CA certification
 *
 * @param ctx - SSL context point
 * @param x   - X509 certification point
 *
 * @return result
 *     0 : failed
 *     1 : OK
 */
int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x);

/**
 * @brief add CA client certification into the SSL
 *
 * @param ssl - SSL point
 * @param x   - X509 certification point
 *
 * @return result
 *     0 : failed
 *     1 : OK
 */
int SSL_add_client_CA(SSL *ssl, X509 *x);

/**
 * @brief load certification into the SSL
 *
 * @param ssl - SSL point
 * @param len - data bytes
 * @param d   - data point
 *
 * @return result
 *     0 : failed
 *     1 : OK
 *
 */
int SSL_use_certificate_ASN1(SSL *ssl, int len, const unsigned char *d);


/**
 * @brief set SSL context client CA certification
 *
 * @param store - pointer to X509_STORE
 * @param x     -  pointer to X509 certification point
 *
 * @return result
 *     0 : failed
 *     1 : OK
 */
int X509_STORE_add_cert(X509_STORE *store, X509 *x);

/**
 * @brief load a character certification context into system context.
 *
 * If '*cert' is pointed to the certification, then load certification 
 * into it, or create a new X509 certification object.
 *
 * @param bp     - pointer to  BIO
 * @param buffer - pointer to the certification context memory
 * @param cb     - pointer to a callback which queries pass phrase used
                   for encrypted PEM structure
 * @param u      - pointer to arbitary data passed by application to callback
 *
 * @return X509 certification object point
 */
X509 * PEM_read_bio_X509(BIO *bp, X509 **x, pem_password_cb cb, void *u);

/**
 * @brief load a character certification context into system context.
 *
 * Current implementation directly calls PEM_read_bio_X509
 *
 * @param bp     - pointer to  BIO
 * @param buffer - pointer to the certification context memory
 * @param cb     - pointer to the callback (not implemented)
 * @param u      - pointer to arbitrary data (not implemented)
 *
 * @return X509 certification object point
 */
X509 *PEM_read_bio_X509_AUX(BIO *bp, X509 **cert, pem_password_cb *cb, void *u);


#ifdef __cplusplus
}
#endif

#endif
