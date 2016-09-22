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

#ifndef _SSL_H_
#define _SSL_H_

#include "platform/ssl_port.h"
#include "internal/ssl_x509.h"

/*
{
*/

/*
 * SSL_CTX_new - create a SSL context
 *
 * @param method - the SSL context method point
 *
 * @return the context point, if create failed return NULL
 */
SSL_CTX* SSL_CTX_new(const SSL_METHOD *method);

/*
 * SSL_CTX_free - free a SSL context
 *
 * @param method - the SSL context point
 *
 * @return none
 */
void SSL_CTX_free(SSL_CTX *ctx);

/*
 * SSL_new - create a SSL
 *
 * @param ctx - the SSL context point
 *
 * @return the SSL point or NULL if failed
 */
SSL* SSL_new(SSL_CTX *ctx);

/*
 * SSL_free - free the SSL
 *
 * @param ssl - the SSL point
 *
 * @return none
 */
void SSL_free(SSL *ssl);

/*
 * SSL_connect - connect to the remote SSL server
 *
 * @param ssl - the SSL point
 *
 * @return
 *         1 : OK
 *        -1 : failed
 */
int SSL_connect(SSL *ssl);

/*
 * SSL_accept - accept the remote connection
 *
 * @param ssl - the SSL point
 *
 * @return
 *         1 : OK
 *        -1 : failed
 */
int SSL_accept(SSL *ssl);

/*
 * SSL_read - read data from to remote
 *
 * @param ssl    - the SSL point which has been connected
 * @param buffer - the received data buffer point
 * @param len    - the received data length
 *
 * @return
 *       > 0 : OK, and return received data bytes
 *       = 0 : connection is closed
 *       < 0 : an error catch
 */
int SSL_read(SSL *ssl, void *buffer, int len);

/*
 * SSL_write - send the data to remote
 *
 * @param ssl    - the SSL point which has been connected
 * @param buffer - the send data buffer point
 * @param len    - the send data length
 *
 * @return
 *       > 0 : OK, and return sent data bytes
 *       = 0 : connection is closed
 *       < 0 : an error catch
 */
int SSL_write(SSL *ssl, const void *buffer, int len);

/*
 * SSL_get_verify_result - get the verifying result of the SSL certification
 *
 * @param ssl - the SSL point
 *
 * @return the result of verifying
 */
long SSL_get_verify_result(const SSL *ssl);

/*
 * SSL_shutdown - shutdown the connection
 *
 * @param ssl - the SSL point
 *
 * @return
 *        1 : OK
 *        0 : shutdown is not finished
 *       -1 : an error catch
 */
int SSL_shutdown(SSL *ssl);

/*
 * SSL_set_fd - bind the socket file description into the SSL
 *
 * @param ssl - the SSL point
 * @param fd  - socket handle
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_set_fd(SSL *ssl, int fd);

/*
 * SSL_CTX_use_PrivateKey - These functions load the private key into the SSL_CTX or SSL object
 *
 * @param ctx  - the SSL context point
 * @param pkey - private key object point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey);

/*
 * SSL_CTX_use_PrivateKey - These functions load the certification into the SSL_CTX or SSL object
 *
 * @param ctx  - the SSL context point
 * @param pkey - certification object point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x);

/*
 * SSLv23_client_method - create the target SSL context client method
 *
 * @param none
 *
 * @return the SSLV2.3 version SSL context client method
 */
const SSL_METHOD* SSLv23_client_method(void);

/*
 * TLSv1_client_method - create the target SSL context client method
 *
 * @param none
 *
 * @return the TLSV1.0 version SSL context client method
 */
const SSL_METHOD* TLSv1_client_method(void);

/*
 * SSLv3_client_method - create the target SSL context client method
 *
 * @param none
 *
 * @return the SSLV1.0 version SSL context client method
 */
const SSL_METHOD* SSLv3_client_method(void);

/*
 * TLSv1_1_client_method - create the target SSL context client method
 *
 * @param none
 *
 * @return the TLSV1.1 version SSL context client method
 */
const SSL_METHOD* TLSv1_1_client_method(void);

/*
 * TLSv1_1_client_method - create the target SSL context client method
 *
 * @param none
 *
 * @return the TLSV1.2 version SSL context client method
 */
const SSL_METHOD* TLSv1_2_client_method(void);


/*
 * SSLv23_server_method - create the target SSL context server method
 *
 * @param none
 *
 * @return the SSLV2.3 version SSL context server method
 */
const SSL_METHOD* SSLv23_server_method(void);

/*
 * TLSv1_1_server_method - create the target SSL context server method
 *
 * @param none
 *
 * @return the TLSV1.1 version SSL context server method
 */
const SSL_METHOD* TLSv1_1_server_method(void);

/*
 * TLSv1_1_server_method - create the target SSL context server method
 *
 * @param none
 *
 * @return the TLSV1.2 version SSL context server method
 */
const SSL_METHOD* TLSv1_2_server_method(void);

/*
 * TLSv1_server_method - create the target SSL context server method
 *
 * @param none
 *
 * @return the TLSV1.0 version SSL context server method
 */
const SSL_METHOD* TLSv1_server_method(void);

/*
 * SSLv3_server_method - create the target SSL context server method
 *
 * @param none
 *
 * @return the SSLV3.0 version SSL context server method
 */
const SSL_METHOD* SSLv3_server_method(void);

/*
 * SSL_CTX_set_alpn_select_cb - set the SSL context ALPN select callback function
 *
 * @param ctx - SSL context point
 * @param cb  - ALPN select callback function
 * @param arg - ALPN select callback function entry private data point
 *
 * @return none
 */
void SSL_CTX_set_alpn_select_cb(SSL_CTX *ctx,
                                int (*cb) (SSL *ssl,
                                           const unsigned char **out,
                                           unsigned char *outlen,
                                           const unsigned char *in,
                                           unsigned int inlen,
                                           void *arg),
                                void *arg);


/*
 * SSL_CTX_set_alpn_protos - set the SSL context ALPN select protocol
 *
 * @param ctx        - SSL context point
 * @param protos     - ALPN protocol name
 * @param protos_len - ALPN protocol name bytes
 *
 * @return
 *        0 : OK
 *        1 : failed
 */
int SSL_CTX_set_alpn_protos(SSL_CTX *ctx, const unsigned char *protos, unsigned int protos_len);

/*
 * SSL_CTX_set_next_proto_select_cb - set the SSL context next ALPN select callback function
 *
 * @param ctx - SSL context point
 * @param cb  - ALPN select callback function
 * @param arg - ALPN select callback function entry private data point
 *
 * @return none
 */
void SSL_CTX_set_next_proto_select_cb(SSL_CTX *ctx,
                                      int (*cb) (SSL *ssl,
                                                 unsigned char **out,
                                                 unsigned char *outlen,
                                                 const unsigned char *in,
                                                 unsigned int inlen,
                                                 void *arg),
                                      void *arg);

/*
 * SSL_get_error - get SSL error code
 *
 * @param ssl       - SSL point
 * @param ret_code  - SSL return code
 *
 * @return SSL error number
 */
int SSL_get_error(const SSL *ssl, int ret_code);

/*
 * ERR_clear_error - clear the SSL error code
 *
 * @param none
 *
 * @return none
 */
void ERR_clear_error(void);

/*
 * ERR_get_error - get the current SSL error code
 *
 * @param none
 *
 * @return current SSL error number
 */
int ERR_get_error(void);

/*
 * ERR_load_SSL_strings - register the SSL error strings
 *
 * @param none
 *
 * @return none
 */
void ERR_load_SSL_strings(void);

/*
 * SSL_library_init - initialize the SSL library
 *
 * @param none
 *
 * @return none
 */
void SSL_library_init(void);

/*
 * ERR_error_string - generates a human-readable string representing the error code e
 *                    and store it into the "ret" point memory
 *
 * @param e   - error code
 * @param ret - memory point to store the string
 *
 * @return the result string point
 */
char *ERR_error_string(unsigned long e, char *ret);

/*
 * SSL_CTX_set_options - add the SSL context option
 *
 * @param ctx - SSL context point
 * @param opt - new SSL context option
 *
 * @return the SSL context option
 */
unsigned long SSL_CTX_set_options(SSL_CTX *ctx, unsigned long opt);

/*
 * SSL_CTX_set_options - add the SSL context mode
 *
 * @param ctx - SSL context point
 * @param mod - new SSL context mod
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_CTX_set_mode(SSL_CTX *ctx, int mod);

/*
}
*/

/*
 * SSL_do_handshake - perform the SSL handshake
 *
 * @param ssl - SSL point
 *
 * @return
 *        1 : OK
 *        0 : failed
 *       -1 : a error catch
 */
int SSL_do_handshake(SSL *ssl);

/*
 * SSL_get_version - get the SSL current version
 *
 * @param ssl - SSL point
 *
 * @return the version string
 */
const char *SSL_get_version(const SSL *ssl);

/*
 * SSL_CTX_set_ssl_version - set  the SSL context version
 *
 * @param ctx  - SSL context point
 * @param meth - SSL method point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_CTX_set_ssl_version(SSL_CTX *ctx, const SSL_METHOD *meth);

/*
 * SSL_CTX_get_ssl_method - get the SSL context current method
 *
 * @param ctx - SSL context point
 *
 * @return the SSL context current method
 */
const SSL_METHOD *SSL_CTX_get_ssl_method(SSL_CTX *ctx);

/*
 * SSL_CTX_get_ssl_method - get the SSL current method
 *
 * @param ssl - SSL point
 *
 * @return the SSL current method
 */
const SSL_METHOD *SSL_get_ssl_method(SSL *ssl);

/*
 * SSL_set_ssl_method - set the SSL method
 *
 * @param ssl  - SSL point
 * @param meth - SSL method point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_set_ssl_method(SSL *ssl, const SSL_METHOD *method);

/*
 * SSL_add_client_CA - add CA client certification into the SSL
 *
 * @param ssl - SSL point
 * @param x   - CA certification point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_add_client_CA(SSL *ssl, X509 *x);

/*
 * SSL_add_client_CA - add CA client certification into the SSL context
 *
 * @param ctx - SSL context point
 * @param x   - CA certification point
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x);

/*
 * SSL_set_client_CA_list - set the SSL CA certification list
 *
 * @param ssl       - SSL point
 * @param name_list - CA certification list
 *
 * @return none
 */
void SSL_set_client_CA_list(SSL *ssl, STACK_OF(X509_NAME) *name_list);

/*
 * SSL_CTX_set_client_CA_list - set the SSL context CA certification list
 *
 * @param ctx       - SSL context point
 * @param name_list - CA certification list
 *
 * @return none
 */
void SSL_CTX_set_client_CA_list(SSL_CTX *ctx, STACK_OF(X509_NAME) *name_list);

/*
 * SSL_get_client_CA_list - get the SSL CA certification list
 *
 * @param ssl - SSL point
 *
 * @return CA certification list
 */
STACK_OF(X509_NAME) *SSL_get_client_CA_list(const SSL *ssl);

/*
 * SSL_CTX_get_client_CA_list - get the SSL context CA certification list
 *
 * @param ctx - SSL context point
 *
 * @return CA certification list
 */
STACK_OF(X509_NAME) *SSL_CTX_get_client_CA_list(const SSL_CTX *ctx);

/*
 * SSL_get_certificate - get the SSL certification point
 *
 * @param ssl - SSL point
 *
 * @return SSL certification point
 */
X509 *SSL_get_certificate(const SSL *ssl);

/*
 * SSL_get_privatekey - get the SSL private key point
 *
 * @param ssl - SSL point
 *
 * @return SSL private key point
 */
EVP_PKEY *SSL_get_privatekey(const SSL *ssl);

/*
 * SSL_set_info_callback - set the SSL information callback function
 *
 * @param ssl - SSL point
 * @param cb  - information callback function
 *
 * @return none
 */
void SSL_set_info_callback(SSL *ssl, void (*cb) (const SSL *ssl, int type, int val));

/*
 * SSL_get_state - get the SSL state
 *
 * @param ssl - SSL point
 *
 * @return SSL state
 */
OSSL_HANDSHAKE_STATE SSL_get_state(const SSL *ssl);

/*
 * SSL_CTX_set_default_read_buffer_len - set the SSL context read buffer length
 *
 * @param ctx - SSL context point
 * @param len - read buffer length
 *
 * @return none
 */
void SSL_CTX_set_default_read_buffer_len(SSL_CTX *ctx, size_t len);

/*
 * SSL_set_default_read_buffer_len - set the SSL read buffer length
 *
 * @param ssl - SSL point
 * @param len - read buffer length
 *
 * @return none
 */
void SSL_set_default_read_buffer_len(SSL *ssl, size_t len);

/*
 * SSL_set_security_level - set the SSL security level
 *
 * @param ssl   - SSL point
 * @param level - security level
 *
 * @return none
 */
void SSL_set_security_level(SSL *ssl, int level);

/*
 * SSL_get_security_level - get the SSL security level
 *
 * @param ssl - SSL point
 *
 * @return security level
 */
int SSL_get_security_level(const SSL *ssl);

/*
 * SSL_CTX_get_verify_mode - get the SSL verifying mode of the SSL context
 *
 * @param ctx - SSL context point
 *
 * @return verifying mode
 */
int SSL_CTX_get_verify_mode(const SSL_CTX *ctx);

/*
 * SSL_CTX_get_verify_depth - get the SSL verifying depth of the SSL context
 *
 * @param ctx - SSL context point
 *
 * @return verifying depth
 */
int SSL_CTX_get_verify_depth(const SSL_CTX *ctx);

/*
 * SSL_CTX_set_verify - set the SSL context verifying of the SSL context
 *
 * @param ctx             - SSL context point
 * @param mode            - verifying mode
 * @param verify_callback - verifying callback function
 *
 * @return none
 */
void SSL_CTX_set_verify(SSL_CTX *ctx, int mode, int (*verify_callback)(int, X509_STORE_CTX *));

/*
 * SSL_set_verify - set the SSL verifying of the SSL context
 *
 * @param ctx             - SSL point
 * @param mode            - verifying mode
 * @param verify_callback - verifying callback function
 *
 * @return none
 */
void SSL_set_verify(SSL *s, int mode, int (*verify_callback)(int, X509_STORE_CTX *));

/*
 * SSL_CTX_set_verify_depth - set the SSL verify depth of the SSL context
 *
 * @param ctx   - SSL context point
 * @param depth - verifying depth
 *
 * @return one
 */
void SSL_CTX_set_verify_depth(SSL_CTX *ctx, int depth);

/*
 * verify_callback - certification verifying callback function
 *
 * @param preverify_ok - verifying result
 * @param x509_ctx     - X509 certification point
 *
 * @return verifying result
 */
int verify_callback(int preverify_ok, X509_STORE_CTX *x509_ctx);

/*
 * SSL_CTX_set_timeout - set the session timeout time
 *
 * @param ctx - SSL context point
 * @param t   - new session timeout time
 *
 * @return old session timeout time
 */
long SSL_CTX_set_timeout(SSL_CTX *ctx, long t);

/*
 * SSL_CTX_get_timeout - get the session timeout time
 *
 * @param ctx - SSL context point
 *
 * @return current session timeout time
 */
long SSL_CTX_get_timeout(const SSL_CTX *ctx);

/*
 * SSL_CTX_set_cipher_list - set the SSL context cipher through the list string
 *
 * @param ctx - SSL context point
 * @param str - cipher controller list string
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_set_cipher_list(SSL_CTX *ctx, const char *str);

/*
 * SSL_set_cipher_list - set the SSL cipher through the list string
 *
 * @param ssl - SSL point
 * @param str - cipher controller list string
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_set_cipher_list(SSL *ssl, const char *str);

/*
 * SSL_get_cipher_list - get the SSL cipher list string
 *
 * @param ssl - SSL point
 *
 * @return cipher controller list string
 */
const char *SSL_get_cipher_list(const SSL *ssl, int n);

/*
 * SSL_get_current_cipher - get the SSL cipher
 *
 * @param ssl - SSL point
 *
 * @return current cipher
 */
const SSL_CIPHER *SSL_get_current_cipher(const SSL *ssl);

/*
 * SSL_get_cipher - get the SSL cipher string
 *
 * @param ssl - SSL point
 *
 * @return cipher string
 */
const char *SSL_get_cipher(const SSL *ssl);

/*
 * SSL_CTX_get_cert_store - get the SSL context object X509 certification storage
 *
 * @param ctx - SSL context point
 *
 * @return x509 certification storage
 */
X509_STORE *SSL_CTX_get_cert_store(const SSL_CTX *ctx);

/*
 * SSL_CTX_set_cert_store - set the SSL context object X509 certification store
 *
 * @param ctx   - SSL context point
 * @param store - X509 certification store
 *
 * @return none
 */
void SSL_CTX_set_cert_store(SSL_CTX *ctx, X509_STORE *store);

/*
 * SSL_want - get the SSL specifical statement
 *
 * @param ssl - SSL point
 *
 * @return specifical statement
 */
int SSL_want(const SSL *ssl);

/*
 * SSL_want_x509_lookup - check if the SSL is SSL_X509_LOOKUP state
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : yes
 *         0 : no
 */
int SSL_want_x509_lookup(const SSL *ssl);

/*
 * SSL_clear - reset the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : yes
 *         0 : no
 */
int SSL_clear(SSL *ssl);

/*
 * SSL_get_fd - get the socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *      >= 0 : yes, and return socket handle
 *       < 0 : a error catch
 */
int SSL_get_fd(const SSL *ssl);

/*
 * SSL_get_rfd - get the read only socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *      >= 0 : yes, and return socket handle
 *       < 0 : a error catch
 */
int SSL_get_rfd(const SSL *ssl);

/*
 * SSL_get_wfd - get the write only socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *      >= 0 : yes, and return socket handle
 *       < 0 : a error catch
 */
int SSL_get_wfd(const SSL *ssl);

/*
 * SSL_set_read_ahead - set the SSL if we can read as many as data
 *
 * @param ssl - SSL point
 * @param yes - enable the function
 *
 * @return none
 */
void SSL_set_read_ahead(SSL *s, int yes);

/*
 * SSL_set_read_ahead - set the SSL context if we can read as many as data
 *
 * @param ctx - SSL context point
 * @param yes - enbale the function
 *
 * @return none
 */
void SSL_CTX_set_read_ahead(SSL_CTX *ctx, int yes);

/*
 * SSL_set_read_ahead - get the SSL ahead signal if we can read as many as data
 *
 * @param ssl - SSL point
 *
 * @return SSL context ahead signal
 */
int SSL_get_read_ahead(const SSL *ssl);

/*
 * SSL_set_read_ahead - get the SSL context ahead signal if we can read as many as data
 *
 * @param ctx - SSL context point
 *
 * @return SSL context ahead signal
 */
long SSL_CTX_get_read_ahead(SSL_CTX *ctx);

/*
 * SSL_has_pending - check if some data can be read
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : there are bytes to be read
 *         0 : no data
 */
int SSL_has_pending(const SSL *ssl);

/*
 * SSL_CTX_use_certificate - load the X509 certification into SSL context
 *
 * @param ctx - SSL context point
 * @param x   - X509 certification point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x);//loads the certificate x into ctx

/*
 * SSL_CTX_use_certificate_ASN1 - load the ASN1 certification into SSL context
 *
 * @param ctx - SSL context point
 * @param len - certification length
 * @param d   - data point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_certificate_ASN1(SSL_CTX *ctx, int len, const unsigned char *d);

/*
 * SSL_CTX_use_certificate_file - load the certification file into SSL context
 *
 * @param ctx  - SSL context point
 * @param file - certification file name
 * @param type - certification encoding type
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_certificate_file(SSL_CTX *ctx, const char *file, int type);

/*
 * SSL_CTX_use_certificate_chain_file - load the certification chain file into SSL context
 *
 * @param ctx  - SSL context point
 * @param file - certification chain file name
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_certificate_chain_file(SSL_CTX *ctx, const char *file);


/*
 * SSL_CTX_use_certificate_ASN1 - load the ASN1 private key into SSL context
 *
 * @param ctx - SSL context point
 * @param d   - data point
 * @param len - private key length
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_PrivateKey_ASN1(int pk, SSL_CTX *ctx, const unsigned char *d,  long len);//adds the private key of type pk stored at memory location d (length len) to ctx

/*
 * SSL_CTX_use_certificate_file - load the private key file into SSL context
 *
 * @param ctx  - SSL context point
 * @param file - private key file name
 * @param type - private key encoding type
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_PrivateKey_file(SSL_CTX *ctx, const char *file, int type);//adds the first private key found in file to ctx, The formatting type of the certificate must be specified from the known types SSL_FILETYPE_PEM, SSL_FILETYPE_ASN1.

/*
 * SSL_CTX_use_certificate - load the RSA private key into SSL context
 *
 * @param ctx - SSL context point
 * @param x   - RSA private key point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_RSAPrivateKey(SSL_CTX *ctx, RSA *rsa);

/*
 * SSL_CTX_use_certificate_ASN1 - load the RSA ASN1 private key into SSL context
 *
 * @param ctx - SSL context point
 * @param d   - data point
 * @param len - RSA private key length
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_RSAPrivateKey_ASN1(SSL_CTX *ctx, unsigned char *d, long len);

/*
 * SSL_CTX_use_certificate_file - load the RSA private key file into SSL context
 *
 * @param ctx  - SSL context point
 * @param file - RSA private key file name
 * @param type - private key encoding type
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_RSAPrivateKey_file(SSL_CTX *ctx, const char *file, int type);


/*
 * SSL_CTX_check_private_key - check if the private key and certification is matched
 *
 * @param ctx  - SSL context point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_check_private_key(const SSL_CTX *ctx);

/*
 * SSL_CTX_use_serverinfo - set the SSL context server information
 *
 * @param ctx               - SSL context point
 * @param serverinfo        - server information string
 * @param serverinfo_length - server information length
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_serverinfo(SSL_CTX *ctx, const unsigned char *serverinfo, size_t serverinfo_length);

/*
 * SSL_CTX_use_serverinfo - load  the SSL context server infomation file into SSL context
 *
 * @param ctx  - SSL context point
 * @param file - server information file
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_serverinfo_file(SSL_CTX *ctx, const char *file);

/*
 * SSL_select_next_proto - SSL select next function
 *
 * @param out        - point of output data point
 * @param outlen     - output data length
 * @param in         - input data
 * @param inlen      - input data length
 * @param client     - client data point
 * @param client_len -client data length
 *
 * @return
 *         OPENSSL_NPN_UNSUPPORTED : not support
 *         OPENSSL_NPN_NEGOTIATED  : negotiated
 *         OPENSSL_NPN_NO_OVERLAP  : no overlap
 */
int SSL_select_next_proto(unsigned char **out, unsigned char *outlen,
                          const unsigned char *in, unsigned int inlen,
                          const unsigned char *client, unsigned int client_len);

/*
 * SSL_CTX_add_extra_chain_cert - load the extra certification chain into the SSL context
 *
 * @param ctx  - SSL context point
 * @param x509 - X509 certification
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
long SSL_CTX_add_extra_chain_cert(SSL_CTX *ctx, X509 *);

/*
 * SSL_CTX_ctrl - control the SSL context
 *
 * @param ctx  - SSL context point
 * @param cmd  - command
 * @param larg - parameter length
 * @param parg - parameter point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
long SSL_CTX_ctrl(SSL_CTX *ctx, int cmd, long larg, char *parg);

/*
 * SSL_CTX_get_ciphers - get the SSL context cipher
 *
 * @param ctx - SSL context point
 *
 * @return SSL context cipher
 */
STACK *SSL_CTX_get_ciphers(const SSL_CTX *ctx);

/*
 * SSL_CTX_get_ciphers - check if the SSL context can read as many as data
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : Yes
 *         0 : No
 */
long SSL_CTX_get_default_read_ahead(SSL_CTX *ctx);

/*
 * SSL_CTX_get_ex_data - get the SSL context extra data
 *
 * @param ctx - SSL context point
 * @param idx - index
 *
 * @return data point
 */
char *SSL_CTX_get_ex_data(const SSL_CTX *ctx, int idx);

/*
 * SSL_CTX_get_quiet_shutdown - get the SSL context quiet shutdown option
 *
 * @param ctx - SSL context point
 *
 * @return quiet shutdown option
 */
int SSL_CTX_get_quiet_shutdown(const SSL_CTX *ctx);

/*
 * SSL_CTX_get_quiet_shutdown - load the SSL context CA file
 *
 * @param ctx    - SSL context point
 * @param CAfile - CA certification file
 * @param CApath - CA certification file path
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_load_verify_locations(SSL_CTX *ctx, const char *CAfile, const char *CApath);

/*
 * SSL_CTX_up_ref - add SSL context reference count by '1'
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_up_ref(SSL_CTX *ctx);

/*
 * SSL_CTX_set_app_data - set SSL context application private data
 *
 * @param ctx - SSL context point
 * @param arg - private data
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_set_app_data(SSL_CTX *ctx, void *arg);

/*
 * SSL_CTX_set_client_cert_cb - set SSL context client certification callback function
 *
 * @param ctx - SSL context point
 * @param cb  - callback function
 *
 * @return none
 */
void SSL_CTX_set_client_cert_cb(SSL_CTX *ctx, int (*cb)(SSL *ssl, X509 **x509, EVP_PKEY **pkey));

/*
 * SSL_CTX_set_default_read_ahead - set the SSL context if we can read as many as data
 *
 * @param ctx - SSL context point
 * @param m   - enable the fuction
 *
 * @return none
 */
void SSL_CTX_set_default_read_ahead(SSL_CTX *ctx, int m);

/*
 * SSL_CTX_set_default_verify_paths - set SSL context default verifying path
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_set_default_verify_paths(SSL_CTX *ctx);

/*
 * SSL_CTX_set_default_verify_paths - set SSL context default verifying directory
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_set_default_verify_dir(SSL_CTX *ctx);

/*
 * SSL_CTX_set_default_verify_paths - set SSL context default verifying file
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_set_default_verify_file(SSL_CTX *ctx);

/*
 * SSL_CTX_set_ex_data - set SSL context extra data
 *
 * @param ctx - SSL context point
 * @param idx - data index
 * @param arg - data point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_set_ex_data(SSL_CTX *s, int idx, char *arg);

/*
 * SSL_CTX_clear_options - clear the SSL context option bit of "op"
 *
 * @param ctx - SSL context point
 * @param op  - option
 *
 * @return SSL context option
 */
unsigned long SSL_CTX_clear_options(SSL_CTX *ctx, unsigned long op);

/*
 * SSL_CTX_clear_options - get the SSL context option
 *
 * @param ctx - SSL context point
 * @param op  - option
 *
 * @return SSL context option
 */
unsigned long SSL_CTX_get_options(SSL_CTX *ctx);

/*
 * SSL_CTX_set_quiet_shutdown - set the SSL context quiet shutdown mode
 *
 * @param ctx  - SSL context point
 * @param mode - mode
 *
 * @return none
 */
void SSL_CTX_set_quiet_shutdown(SSL_CTX *ctx, int mode);

/*
 * SSL_CTX_get0_certificate - get the SSL context X509 certification
 *
 * @param ctx - SSL context point
 *
 * @return X509 certification
 */
X509 *SSL_CTX_get0_certificate(const SSL_CTX *ctx);

/*
 * SSL_CTX_get0_certificate - get the SSL context private key
 *
 * @param ctx - SSL context point
 *
 * @return private key
 */
EVP_PKEY *SSL_CTX_get0_privatekey(const SSL_CTX *ctx);

/*
 * SSL_CTX_use_psk_identity_hint - set SSL context PSK identity hint
 *
 * @param ctx  - SSL context point
 * @param hint - PSK identity hint
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_use_psk_identity_hint(SSL_CTX *ctx, const char *hint);

/*
 * SSL_CTX_set_psk_server_callback - set SSL context PSK server callback function
 *
 * @param ctx      - SSL context point
 * @param callback - callback function
 *
 */
void SSL_CTX_set_psk_server_callback(SSL_CTX *ctx,
                                     unsigned int (*callback)(SSL *ssl,
                                                              const char *identity,
                                                              unsigned char *psk,
                                                              int max_psk_len));
/*
 * SSL_alert_desc_string - get alert description string
 *
 * @param value - alert value
 *
 * @return alert description string
 */
const char *SSL_alert_desc_string(int value);

/*
 * SSL_alert_desc_string - get alert description long string
 *
 * @param value - alert value
 *
 * @return alert description long string
 */
const char *SSL_alert_desc_string_long(int value);

/*
 * SSL_alert_type_string - get alert type string
 *
 * @param value - alert value
 *
 * @return alert type string
 */
const char *SSL_alert_type_string(int value);

/*
 * SSL_alert_type_string_long - get alert type long string
 *
 * @param value - alert value
 *
 * @return alert type long string
 */
const char *SSL_alert_type_string_long(int value);

/*
 * SSL_get_SSL_CTX - get SSL context of the SSL
 *
 * @param ssl - SSL point
 *
 * @return SSL context
 */
SSL_CTX *SSL_get_SSL_CTX(const SSL *ssl);

/*
 * SSL_get_app_data - get SSL application data
 *
 * @param ssl - SSL point
 *
 * @return application data
 */
char *SSL_get_app_data(SSL *ssl);

/*
 * SSL_get_cipher_bits - get SSL cipher bits
 *
 * @param ssl - SSL point
 * @param alg_bits - algorithm bits
 *
 * @return strength bits
 */
int SSL_get_cipher_bits(const SSL *ssl, int *alg_bits);

/*
 * SSL_get_cipher_name - get SSL cipher name
 *
 * @param ssl - SSL point
 *
 * @return SSL cipher name
 */
char *SSL_get_cipher_name(const SSL *ssl);

/*
 * SSL_get_cipher_version - get SSL cipher version
 *
 * @param ssl - SSL point
 *
 * @return SSL cipher version
 */
char *SSL_get_cipher_version(const SSL *ssl);

/*
 * SSL_get_ex_data - get SSL extra data
 *
 * @param ssl - SSL point
 * @param idx - data index
 *
 * @return extra data
 */
char *SSL_get_ex_data(const SSL *ssl, int idx);

/*
 * SSL_get_ex_data_X509_STORE_CTX_idx - get index of the SSL extra data X509 storage context
 *
 * @param none
 *
 * @return data index
 */
int SSL_get_ex_data_X509_STORE_CTX_idx(void);

/*
 * SSL_get_peer_cert_chain - get peer certification chain
 *
 * @param ssl - SSL point
 *
 * @return certification chain
 */
STACK *SSL_get_peer_cert_chain(const SSL *ssl);

/*
 * SSL_get_peer_certificate - get peer certification
 *
 * @param ssl - SSL point
 *
 * @return certification
 */
X509 *SSL_get_peer_certificate(const SSL *ssl);

/*
 * SSL_get_quiet_shutdown - get SSL quiet shutdown mode
 *
 * @param ssl - SSL point
 *
 * @return quiet shutdown mode
 */
int SSL_get_quiet_shutdown(const SSL *ssl);

/*
 * SSL_get_rbio - get SSL read only IO handle
 *
 * @param ssl - SSL point
 *
 * @return IO handle
 */
BIO *SSL_get_rbio(const SSL *ssl);

/*
 * SSL_get_shared_ciphers - get SSL shared ciphers
 *
 * @param ssl - SSL point
 * @param buf - buffer to store the ciphers
 * @param len - buffer len
 *
 * @return shared ciphers or NULL if failed
 */
char *SSL_get_shared_ciphers(const SSL *ssl, char *buf, int len);

/*
 * SSL_get_shutdown - get SSL shutdown mode
 *
 * @param ssl - SSL point
 *
 * @return shutdown mode
 */
int SSL_get_shutdown(const SSL *ssl);

/*
 * SSL_get_time - get SSL session time
 *
 * @param ssl - SSL point
 *
 * @return session time
 */
long SSL_get_time(const SSL *ssl);

/*
 * SSL_get_timeout - get SSL session timeout time
 *
 * @param ssl - SSL point
 *
 * @return session timeout time
 */
long SSL_get_timeout(const SSL *ssl);

/*
 * SSL_get_verify_mode - get SSL verifying mode
 *
 * @param ssl - SSL point
 *
 * @return verifying mode
 */
int SSL_get_verify_mode(const SSL *ssl);

/*
 * SSL_get_wbio - get SSL write only IO handle
 *
 * @param ssl - SSL point
 *
 * @return IO handle
 */
BIO *SSL_get_wbio(const SSL *ssl);

/*
 * SSL_load_client_CA_file - load SSL client CA certification file
 *
 * @param file - file name
 *
 * @return certification loading object
 */
STACK *SSL_load_client_CA_file(const char *file);

/*
 * SSL_up_ref - add SSL reference by '1'
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_up_ref(SSL *ssl);

/*
 * SSL_peek - read and put data into buf, but not clear the SSL low-level storage
 *
 * @param ssl - SSL point
 * @param buf - storage buffer point
 * @param num - data bytes
 *
 * @return
 *         > 0 : OK, and return read bytes
 *         = 0 : connect is closed
 *         < 0 : a error catch
 */
int SSL_peek(SSL *ssl, void *buf, int num);

/*
 * SSL_renegotiate - make SSL renegotiate
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_renegotiate(SSL *ssl);

/*
 * SSL_rstate_string - get the state string where SSL is reading
 *
 * @param ssl - SSL point
 *
 * @return state string
 */
const char *SSL_rstate_string(SSL *ssl);

/*
 * SSL_rstate_string_long - get the statement long string where SSL is reading
 *
 * @param ssl - SSL point
 *
 * @return statement long string
 */
const char *SSL_rstate_string_long(SSL *ssl);

/*
 * SSL_set_accept_state - set SSL accept statement
 *
 * @param ssl - SSL point
 *
 * @return none
 */
void SSL_set_accept_state(SSL *ssl);

/*
 * SSL_set_app_data - set SSL application data
 *
 * @param ssl - SSL point
 * @param arg - SSL application data point
 *
 * @return none
 */
void SSL_set_app_data(SSL *ssl, char *arg);

/*
 * SSL_set_bio - set SSL BIO
 *
 * @param ssl  - SSL point
 * @param rbio - read only IO
 * @param wbio - write only IO
 *
 * @return none
 */
void SSL_set_bio(SSL *ssl, BIO *rbio, BIO *wbio);

/*
 * SSL_clear_options - clear SSL option
 *
 * @param ssl - SSL point
 * @param op  - clear option
 *
 * @return SSL option
 */
unsigned long SSL_clear_options(SSL *ssl, unsigned long op);

/*
 * SSL_clear_options - get SSL option
 *
 * @param ssl - SSL point
 *
 * @return SSL option
 */
unsigned long SSL_get_options(SSL *ssl);

/*
 * SSL_clear_options - clear SSL option
 *
 * @param ssl - SSL point
 * @param op  - setting option
 *
 * @return SSL option
 */
unsigned long SSL_set_options(SSL *ssl, unsigned long op);

/*
 * SSL_set_quiet_shutdown - set SSL quiet shutdown mode
 *
 * @param ssl  - SSL point
 * @param mode - quiet shutdown mode
 *
 * @return none
 */
void SSL_set_quiet_shutdown(SSL *ssl, int mode);

/*
 * SSL_set_quiet_shutdown - set SSL shutdown mode
 *
 * @param ssl  - SSL point
 * @param mode - shutdown mode
 *
 * @return none
 */
void SSL_set_shutdown(SSL *ssl, int mode);

/*
 * SSL_set_time - set SSL session time
 *
 * @param ssl - SSL point
 * @param t   - session time
 *
 * @return session time
 */
void SSL_set_time(SSL *ssl, long t);

/*
 * SSL_set_time - set SSL session timeout time
 *
 * @param ssl - SSL point
 * @param t   - session timeout time
 *
 * @return session timeout time
 */
void SSL_set_timeout(SSL *ssl, long t);

/*
 * SSL_state_string - get SSL statement string
 *
 * @param ssl - SSL point
 *
 * @return SSL statement string
 */
char *SSL_state_string(const SSL *ssl);

/*
 * SSL_state_string_long - get SSL statement long string
 *
 * @param ssl - SSL point
 *
 * @return SSL statement long string
 */
char *SSL_state_string_long(const SSL *ssl);

/*
 * SSL_total_renegotiations - get SSL renegotiation count
 *
 * @param ssl - SSL point
 *
 * @return renegotiation count
 */
long SSL_total_renegotiations(SSL *ssl);

/*
 * SSL_version - get SSL version
 *
 * @param ssl - SSL point
 *
 * @return SSL version
 */
int SSL_version(const SSL *ssl);

/*
 * SSL_use_psk_identity_hint - set SSL PSK identity hint
 *
 * @param ssl  - SSL point
 * @param hint - identity hint
 *
 * @return
 *         1 : oK
 *         0 : failed
 */
int SSL_use_psk_identity_hint(SSL *ssl, const char *hint);

/*
 * SSL_get_psk_identity_hint - get SSL PSK identity hint
 *
 * @param ssl - SSL point
 *
 * @return identity hint
 */
const char *SSL_get_psk_identity_hint(SSL *ssl);

/*
 * SSL_get_psk_identity - get SSL PSK identity
 *
 * @param ssl - SSL point
 *
 * @return identity
 */
const char *SSL_get_psk_identity(SSL *ssl);


#endif
