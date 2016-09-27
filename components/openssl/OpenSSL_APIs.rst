OpenSSL APIs 
======================

/**
 * @brief create the target SSL context client method
 *
 * @param none
 *
 * @return the SSLV2.3 version SSL context client method
 */
const SSL_METHOD* SSLv23_client_method(void);


/**
 * @brief create the target SSL context client method
 *
 * @param none
 *
 * @return the TLSV1.0 version SSL context client method
 */
const SSL_METHOD* TLSv1_client_method(void);


/**
 * @brief create the target SSL context client method
 *
 * @param none
 *
 * @return the SSLV1.0 version SSL context client method
 */
const SSL_METHOD* SSLv3_client_method(void);


/**
 * @brief create the target SSL context client method
 *
 * @param none
 *
 * @return the TLSV1.1 version SSL context client method
 */
const SSL_METHOD* TLSv1_1_client_method(void);


/**
 * @brief create the target SSL context client method
 *
 * @param none
 *
 * @return the TLSV1.2 version SSL context client method
 */
const SSL_METHOD* TLSv1_2_client_method(void);


/**
 * @brief create the target SSL context server method
 *
 * @param none
 *
 * @return the SSLV2.3 version SSL context server method
 */
const SSL_METHOD* SSLv23_server_method(void);

/**
 * @brief create the target SSL context server method
 *
 * @param none
 *
 * @return the TLSV1.1 version SSL context server method
 */
const SSL_METHOD* TLSv1_1_server_method(void);

/**
 * @brief create the target SSL context server method
 *
 * @param none
 *
 * @return the TLSV1.2 version SSL context server method
 */
const SSL_METHOD* TLSv1_2_server_method(void);

/**
 * @brief create the target SSL context server method
 *
 * @param none
 *
 * @return the TLSV1.0 version SSL context server method
 */
const SSL_METHOD* TLSv1_server_method(void);

/**
 * @brief create the target SSL context server method
 *
 * @param none
 *
 * @return the SSLV3.0 version SSL context server method
 */
const SSL_METHOD* SSLv3_server_method(void);

/**
 * @brief create a SSL context
 *
 * @param method - the SSL context method point
 *
 * @return the context point
 */
SSL_CTX* SSL_CTX_new(const SSL_METHOD *method);

/**
 * @brief free a SSL context
 *
 * @param method - the SSL context point
 *
 * @return none
 */
void SSL_CTX_free(SSL_CTX *ctx);

/**
 * @brief set  the SSL context version
 *
 * @param ctx  - SSL context point
 * @param meth - SSL method point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_set_ssl_version(SSL_CTX *ctx, const SSL_METHOD *meth);

/**
 * @brief get the SSL context current method
 *
 * @param ctx - SSL context point
 *
 * @return the SSL context current method
 */
const SSL_METHOD *SSL_CTX_get_ssl_method(SSL_CTX *ctx);

/**
 * @brief create a SSL
 *
 * @param ctx - the SSL context point
 *
 * @return the SSL point
 */
SSL* SSL_new(SSL_CTX *ctx);

/**
 * @brief free the SSL
 *
 * @param ssl - the SSL point
 *
 * @return none
 */
void SSL_free(SSL *ssl);

/**
 * @brief perform the SSL handshake
 *
 * @param ssl - SSL point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 *    -1 : a error catch
 */
int SSL_do_handshake(SSL *ssl);

/**
 * @brief connect to the remote SSL server
 *
 * @param ssl - the SSL point
 *
 * @return result
 *     1 : OK
 *    -1 : failed
 */
int SSL_connect(SSL *ssl);

/**
 * @brief accept the remote connection
 *
 * @param ssl - the SSL point
 *
 * @return result
 *     1 : OK
 *    -1 : failed
 */
int SSL_accept(SSL *ssl);

/**
 * @brief shutdown the connection
 *
 * @param ssl - the SSL point
 *
 * @return result
 *     1 : OK
 *     0 : shutdown is not finished
 *    -1 : an error catch
 */
int SSL_shutdown(SSL *ssl);

/**
 * @brief reset the SSL
 *
 * @param ssl - SSL point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_clear(SSL *ssl);

/**
 * @brief read data from to remote
 *
 * @param ssl    - the SSL point which has been connected
 * @param buffer - the received data buffer point
 * @param len    - the received data length
 *
 * @return result
 *     > 0 : OK, and return received data bytes
 *     = 0 : connection is closed
 *     < 0 : an error catch
 */
int SSL_read(SSL *ssl, void *buffer, int len);

/**
 * @brief send the data to remote
 *
 * @param ssl    - the SSL point which has been connected
 * @param buffer - the send data buffer point
 * @param len    - the send data length
 *
 * @return result
 *     > 0 : OK, and return sent data bytes
 *     = 0 : connection is closed
 *     < 0 : an error catch
 */
int SSL_write(SSL *ssl, const void *buffer, int len);

/**
 * @brief get SSL context of the SSL
 *
 * @param ssl - SSL point
 *
 * @return SSL context
 */
SSL_CTX *SSL_get_SSL_CTX(const SSL *ssl);

/**
 * @brief get SSL shutdown mode
 *
 * @param ssl - SSL point
 *
 * @return shutdown mode
 */
int SSL_get_shutdown(const SSL *ssl);

/**
 * @brief set SSL shutdown mode
 *
 * @param ssl  - SSL point
 * @param mode - shutdown mode
 *
 * @return none
 */
void SSL_set_shutdown(SSL *ssl, int mode);

/**
 * @brief get the SSL current method
 *
 * @param ssl - SSL point
 *
 * @return the SSL current method
 */
const SSL_METHOD *SSL_get_ssl_method(SSL *ssl);

/**
 * @brief set the SSL method
 *
 * @param ssl  - SSL point
 * @param meth - SSL method point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_set_ssl_method(SSL *ssl, const SSL_METHOD *method);

/**
 * @brief get the bytes numbers which are to be read
 *
 * @param ssl  - SSL point
 *
 * @return bytes number
 */
int SSL_pending(const SSL *ssl);

/**
 * @brief check if some data can be read
 *
 * @param ssl - SSL point
 *
 * @return
 *     1 : there are bytes to be read
 *     0 : no data
 */
int SSL_has_pending(const SSL *ssl);

/**
 * @brief get the socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return result
 *     >= 0 : yes, and return socket handle
 *      < 0 : a error catch
 */
int SSL_get_fd(const SSL *ssl);

/**
 * @brief get the read only socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return result
 *     >= 0 : yes, and return socket handle
 *      < 0 : a error catch
 */
int SSL_get_rfd(const SSL *ssl);

/**
 * @brief get the write only socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return result
 *     >= 0 : yes, and return socket handle
 *      < 0 : a error catch
 */
int SSL_get_wfd(const SSL *ssl);

/**
 * @brief bind the socket file description into the SSL
 *
 * @param ssl - the SSL point
 * @param fd  - socket handle
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_set_fd(SSL *ssl, int fd);

/**
 * @brief bind the read only socket file description into the SSL
 *
 * @param ssl - the SSL point
 * @param fd  - socket handle
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_set_rfd(SSL *ssl, int fd);

/**
 * @brief bind the write only socket file description into the SSL
 *
 * @param ssl - the SSL point
 * @param fd  - socket handle
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_set_wfd(SSL *ssl, int fd);

/**
 * @brief get SSL version
 *
 * @param ssl - SSL point
 *
 * @return SSL version
 */
int SSL_version(const SSL *ssl);

/**
 * @brief get the SSL current version
 *
 * @param ssl - SSL point
 *
 * @return the version string
 */
const char *SSL_get_version(const SSL *ssl);

/**
 * @brief get the SSL state
 *
 * @param ssl - SSL point
 *
 * @return SSL state
 */
OSSL_HANDSHAKE_STATE SSL_get_state(const SSL *ssl);

/**
 * @brief get alert description string
 *
 * @param value - alert value
 *
 * @return alert description string
 */
const char *SSL_alert_desc_string(int value);

/**
 * @brief get alert description long string
 *
 * @param value - alert value
 *
 * @return alert description long string
 */
const char *SSL_alert_desc_string_long(int value);

/**
 * @brief get alert type string
 *
 * @param value - alert value
 *
 * @return alert type string
 */
const char *SSL_alert_type_string(int value);

/**
 * @brief get alert type long string
 *
 * @param value - alert value
 *
 * @return alert type long string
 */
const char *SSL_alert_type_string_long(int value);

/**
 * @brief get the state string where SSL is reading
 *
 * @param ssl - SSL point
 *
 * @return state string
 */
const char *SSL_rstate_string(SSL *ssl);

/**
 * @brief get the statement long string where SSL is reading
 *
 * @param ssl - SSL point
 *
 * @return statement long string
 */
const char *SSL_rstate_string_long(SSL *ssl);

/**
 * @brief get SSL statement string
 *
 * @param ssl - SSL point
 *
 * @return SSL statement string
 */
char *SSL_state_string(const SSL *ssl);

/**
 * @brief get SSL statement long string
 *
 * @param ssl - SSL point
 *
 * @return SSL statement long string
 */
char *SSL_state_string_long(const SSL *ssl);

/**
 * @brief get SSL error code
 *
 * @param ssl       - SSL point
 * @param ret_code  - SSL return code
 *
 * @return SSL error number
 */
int SSL_get_error(const SSL *ssl, int ret_code);

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
 * @brief add CA client certification into the SSL
 *
 * @param ssl - SSL point
 * @param x   - CA certification point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_add_client_CA(SSL *ssl, X509 *x);

/**
 * @brief add CA client certification into the SSL context
 *
 * @param ctx - SSL context point
 * @param x   - CA certification point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_add_client_CA(SSL_CTX *ctx, X509 *x);

/**
 * @brief get the SSL certification point
 *
 * @param ssl - SSL point
 *
 * @return SSL certification point
 */
X509 *SSL_get_certificate(const SSL *ssl);

/**
 * @brief get the verifying result of the SSL certification
 *
 * @param ssl - the SSL point
 *
 * @return the result of verifying
 */
long SSL_get_verify_result(const SSL *ssl);

/**
 * @brief These functions load the certification into the SSL_CTX or SSL object
 *
 * @param ctx  - the SSL context point
 * @param pkey - certification object point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_use_certificate(SSL_CTX *ctx, X509 *x);

/**
 * @brief load the ASN1 certification into SSL context
 *
 * @param ctx - SSL context point
 * @param len - certification length
 * @param d   - data point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_use_certificate_ASN1(SSL_CTX *ctx, int len, const unsigned char *d);

/**
 * @brief These functions load the private key into the SSL_CTX or SSL object
 *
 * @param ctx  - the SSL context point
 * @param pkey - private key object point
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_use_PrivateKey(SSL_CTX *ctx, EVP_PKEY *pkey);

/**
 * @brief load the ASN1 private key into SSL context
 *
 * @param ctx - SSL context point
 * @param d   - data point
 * @param len - private key length
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_use_PrivateKey_ASN1(int pk, SSL_CTX *ctx, const unsigned char *d,  long len);

/**
 * @brief load the RSA ASN1 private key into SSL context
 *
 * @param ctx - SSL context point
 * @param d   - data point
 * @param len - RSA private key length
 *
 * @return result
 *     1 : OK
 *     0 : failed
 */
int SSL_CTX_use_RSAPrivateKey_ASN1(SSL_CTX *ctx, const unsigned char *d, long len);

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
 * @brief get peer certification
 *
 * @param ssl - SSL point
 *
 * @return certification
 */
X509 *SSL_get_peer_certificate(const SSL *ssl);

/**
 * @brief set the SSL context read buffer length
 *
 * @param ctx - SSL context point
 * @param len - read buffer length
 *
 * @return none
 */
void SSL_CTX_set_default_read_buffer_len(SSL_CTX *ctx, size_t len);

/**
 * @brief set the SSL read buffer length
 *
 * @param ssl - SSL point
 * @param len - read buffer length
 *
 * @return none
 */
void SSL_set_default_read_buffer_len(SSL *ssl, size_t len);

/**
 * @brief get the SSL specifical statement
 *
 * @param ssl - SSL point
 *
 * @return specifical statement
 */
int SSL_want(const SSL *ssl);

/**
 * @brief check if SSL want nothing
 *
 * @param ssl - SSL point
 *
 * @return result
 *     0 : false
 *     1 : true
 */
int SSL_want_nothing(const SSL *ssl);

/**
 * @brief check if SSL want to read
 *
 * @param ssl - SSL point
 *
 * @return result
 *     0 : false
 *     1 : true 
 */
int SSL_want_read(const SSL *ssl);

/**
 * @brief check if SSL want to write
 *
 * @param ssl - SSL point
 *
 * @return result
 *     0 : false
 *     1 : true 
 */
int SSL_want_write(const SSL *ssl);
