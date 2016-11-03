OpenSSL-APIs 
------------

This directory does not contain OpenSSL itself, but the code here can be used as a wrapper for applications using the OpenSSL API. 
It uses mbedTLS to do the actual work, so anyone compiling openssl code needs the mbedtls library and header file.

OpenSSL APIs not mentioned in this article are not open to public for the time,
also do not have the corresponding function.
If user calls it directly, it will always return an error or may show cannot link at compiling time.

Chapter Introduction
====================

- Chapter 1. SSL Context Method Create
- Chapter 2. SSL Context Fucntion
- Chapter 3. SSL Fucntion
- Chapter 4. SSL X509 Certification and Private Key Function


Chapter 1. SSL Context Method Create 
====================================

1.1 const SSL_METHOD* ``SSLv3_client_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        SSLV3.0 version SSL context client method point
    
    Description::
    
        create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = SSLv3_client_method();
 
            ...
        }

1.2 const SSL_METHOD* ``TLSv1_client_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        TLSV1.0 version SSL context client method point
    
    Description::
    
        create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_client_method();
 
            ...
        }

1.3 const SSL_METHOD* ``TLSv1_1_client_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        TLSV1.1 version SSL context client method point
    
    Description::
    
        create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_1_client_method();
 
            ...
        }

1.4 const SSL_METHOD* ``TLSv1_2_client_method`` (void)

    Arguments::
    
       none
    
    Return::
    
       TLSV1.2 version SSL context client method point
    
    Description::
    
       create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_2_client_method();
 
            ...
        }
        
1.5 const SSL_METHOD* ``TLS_client_method`` (void)

    Arguments::
    
       none
    
    Return::
    
       TLSV1.2 version SSL context client method point
    
    Description::
    
       create the default SSL context method, it's always to be TLSV1.2
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_2_client_method();
 
            ...
        }        

1.6 const SSL_METHOD* ``SSLv3_server_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        SSLV3.0 version SSL context server method point
    
    Description::
    
        create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = SSLv3_server_method();
 
            ...
        }

1.7 const SSL_METHOD* ``TLSv1_server_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        TLSV1.0 version SSL context server method point
    
    Description::
    
        create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_server_method();
 
            ...
        }

1.8 const SSL_METHOD* ``TLSv1_1_server_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        TLSV1.1 version SSL context server method point
    
    Description::
    
        create the target SSL context method
    
    Example :
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_1_server_method();
 
            ...
        }


1.9 const SSL_METHOD* ``TLSv1_2_server_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        TLSV1.2 version SSL context server method point
    
    Description::
    
        create the target SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_2_server_method();
 
            ...
        }

1.10 const SSL_METHOD* ``TLS_server_method`` (void)

    Arguments::
    
        none
    
    Return::
    
        TLSV1.2 version SSL context server method point
    
    Description::
    
        create the default SSL context method, it's always to be TLSV1.2
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method = TLSv1_2_server_method();
 
            ...
        }


Chapter 2. SSL Context Fucntion
===============================


2.1 SSL_CTX* ``SSL_CTX_new`` (const SSL_METHOD *method)

    Arguments::
    
        method - the SSL context method point
    
    Return::
    
        context point
    
    Description::
    
        create a SSL context
    
    Example::
    
        void example(void)
        {
            SSL_CTX *ctx = SSL_CTX_new(SSLv3_server_method());
 
            ...
        }


2.2 ``void SSL_CTX_free`` (SSL_CTX *ctx)

    Arguments::
    
        ctx - the SSL context point
    
    Return::
    
        none
    
    Description::
    
        free a SSL context
    
    Example::
    
        void example(void)
        {
            SSL_CTX *ctx;
        
            ... ...
            
            SSL_CTX_free(ctx);
        }


2.3 ``int SSL_CTX_set_ssl_version`` (SSL_CTX *ctx, const SSL_METHOD *meth)

    Arguments::
    
        ctx  - SSL context point
        meth - SSL method point
    
    Return::
    
        1 : OK
        0 : failed
    
    Description::
    
        set the SSL context version
    
    Example::
    
        void example(void)
        {
            SSL_CTX *ctx;
            const SSL_METHOD *meth;
            
            ... ...
            
            SSL_CTX_set_ssl_version(ctx, meth);    
        }


2.4 const SSL_METHOD* ``SSL_CTX_get_ssl_method`` (SSL_CTX *ctx)

    Arguments::
    
        ctx - SSL context point
    
    Return::
    
        SSL context method
    
    Description::
    
        get the SSL context method
    
    Example::
    
        void example(void)
        {
            const SSL_METHOD *method;
            SSL_CTX *ctx;
            
            ... ...
                
            method = SSL_CTX_get_ssl_method(ctx);    
        }



Chapter 3. SSL Fucntion
=======================


3.1 SSL* ``SSL_new`` (SSL_CTX *ctx)

    Arguments::
    
        ctx - SSL context point
    
    Return::
    
        SSL method
    
    Description::
    
        create a SSL
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            SSL_CTX *ctx;

            ... ...
   
            ssl = SSL_new(ctx);
        }


3.2 void ``SSL_free`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        none
    
    Description::
    
        free SSL
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            
            ... ...
            
            SSL_free(ssl);
        }
        

3.3 int ``SSL_do_handshake`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        1 : OK
        0 : failed, connect is close by remote
       -1 : a error catch
    
    Description::
    
        perform the SSL handshake
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
            
            ... ...
            
            ret = SSL_do_handshake(ssl);
        }


3.4 int ``SSL_connect`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        1 : OK
        0 : failed, connect is close by remote
       -1 : a error catch
    
    Description::
    
        connect to the remote SSL server
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
            
            ... ...
            
            ret = SSL_connect(ssl);
        }


3.5 int ``SSL_accept`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        1 : OK
        0 : failed, connect is close by remote
       -1 : a error catch
    
    Description::
    
        accept the remote connection
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
            
            ... ...
            
            ret = SSL_accept(ssl);
        }


3.6 int ``SSL_shutdown`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        1 : OK
        0 : failed, connect is close by remote
       -1 : a error catch
    
    Description::
    
        shutdown the connection
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
            
            ... ...
            
            ret = SSL_shutdown(ssl);
        }


3.7 int ``SSL_clear`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        shutdown the connection
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
            
            ... ...
            
            ret = SSL_clear(ssl);
        }


3.8 int ``SSL_read`` (SSL *ssl, void *buffer, int len)

    Arguments::
    
        ssl    - point
        buffer - data buffer point
        len    - data length
    
    Return::
    
        > 0 : OK, and return received data bytes
        = 0 : no data received or connection is closed
        < 0 : an error catch
        
    Description::
    
        read data from remote
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            char *buf;
            int len;
            int ret;
            
            ... ...
            
            ret = SSL_read(ssl, buf, len);
        }

3.9 int ``SSL_write`` (SSL *ssl, const void *buffer, int len)

    Arguments::
    
        ssl    - SSL point
        buffer - data buffer point
        len    - data length
    
    Return::
    
        > 0 : OK, and return received data bytes
        = 0 : no data sent or connection is closed
        < 0 : an error catch
        
    Description::
    
        send the data to remote
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            char *buf;
            int len;
            int ret;
            
            ... ...
            
            ret = SSL_write(ssl, buf, len);
        }


3.10 ``SSL_CTX *SSL_get_SSL_CTX`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        SSL context
        
    Description::
    
        get SSL context of the SSL
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            SSL_CTX *ctx;
            
            ... ...
            
            ctx = SSL_get_SSL_CTX(ssl);
        }


3.11 int ``SSL_get_shutdown`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        shutdown mode
        
    Description::
    
        get SSL shutdown mode
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int mode;
            
            ... ...
            
            mode = SSL_get_SSL_CTX(ssl);
        }


3.12 void ``SSL_set_shutdown`` (SSL *ssl, int mode)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        shutdown mode
        
    Description::
    
        set SSL shutdown mode
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int mode = 0;
            
            ... ...
            
            SSL_set_shutdown(ssl, mode);
        }


3.13 const SSL_METHOD* ``SSL_get_ssl_method`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        SSL method
        
    Description::
    
        set SSL shutdown mode
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            const SSL_METHOD *method;
            
            ... ...
            
            method = SSL_get_ssl_method(ssl);
        }
        

3.14 int ``SSL_set_ssl_method`` (SSL *ssl, const SSL_METHOD *method)

    Arguments::
    
        ssl  - SSL point
        meth - SSL method point
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        set the SSL method
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            const SSL_METHOD *method;
            
            ... ...
            
            ret = SSL_set_ssl_method(ssl, method);
        }


3.15 int ``SSL_pending`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        data bytes
        
    Description::
    
        get received data bytes
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            
            ... ...
            
            ret = SSL_pending(ssl);
        }


3.16 int ``SSL_has_pending`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        1 : Yes
        0 : No
        
    Description::
    
        check if data is received
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            
            ... ...
            
            ret = SSL_has_pending(ssl);
        }


3.17 int ``SSL_get_fd`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        >= 0 : socket id
         < 0 : a error catch
        
    Description::
    
        get the socket of the SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            
            ... ...
            
            ret = SSL_get_fd(ssl);
        }


3.18 int ``SSL_get_rfd`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        >= 0 : socket id
         < 0 : a error catch
        
    Description::
    
        get the read only socket of the SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            
            ... ...
            
            ret = SSL_get_rfd(ssl);
        }


3.19 int ``SSL_get_wfd`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        >= 0 : socket id
         < 0 : a error catch
        
    Description::
    
        get the write only socket of the SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            
            ... ...
            
            ret = SSL_get_wfd(ssl);
        }


3.20 int ``SSL_set_fd`` (SSL *ssl, int fd)

    Arguments::
    
        ssl - SSL point
        fd  - socket id
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        set socket to SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            int socket;
                        
            ... ...
            
            ret = SSL_set_fd(ssl, socket);
        }


3.21 int ``SSL_set_rfd`` (SSL *ssl, int fd)

    Arguments::
    
        ssl - SSL point
        fd  - socket id
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        set read only socket to SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            int socket;
                        
            ... ...
            
            ret = SSL_set_rfd(ssl, socket);
        }
        

3.22 int ``SSL_set_wfd`` (SSL *ssl, int fd)

    Arguments::
    
        ssl - SSL point
        fd  - socket id
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        set write only socket to SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            int socket;
                        
            ... ...
            
            ret = SSL_set_wfd(ssl, socket);
        }
        

3.23 int ``SSL_version`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        SSL version
        
    Description::
    
        get SSL version
    
    Example::
    
        void example(void)
        {
            int version;
            SSL *ssl;
                        
            ... ...
            
            version = SSL_version(ssl);
        }


3.24 const char* ``SSL_get_version`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        SSL version string
        
    Description::
    
        get the SSL current version string
    
    Example::
    
        void example(void)
        {
            char *version;
            SSL *ssl;
                        
            ... ...
            
            version = SSL_get_version(ssl);
        }
        
        
3.25 OSSL_HANDSHAKE_STATE ``SSL_get_state`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        SSL state
        
    Description::
    
        get the SSL state
    
    Example::
    
        void example(void)
        {
            OSSL_HANDSHAKE_STATE state;
            SSL *ssl;
                        
            ... ...
            
            state = SSL_get_state(ssl);
        }


3.26 const char* ``SSL_alert_desc_string`` (int value)

    Arguments::
    
        value - SSL description
    
    Return::
    
        alert value string
        
    Description::
    
        get alert description string
    
    Example::
    
        void example(void)
        {
            int val;
            char *str;
                        
            ... ...
            
            str = SSL_alert_desc_string(val);
        }


3.27 const char* ``SSL_alert_desc_string_long`` (int value)

    Arguments::
    
        value - SSL description
    
    Return::
    
        alert value long string
        
    Description::
    
        get alert description long string
    
    Example::
    
        void example(void)
        {
            int val;
            char *str;
                        
            ... ...
            
            str = SSL_alert_desc_string_long(val);
        }


3.28 const char* ``SSL_alert_type_string`` (int value)

    Arguments::
    
        value - SSL type description
    
    Return::
    
        alert type string
        
    Description::
    
        get alert type string
    
    Example::
    
        void example(void)
        {
            int val;
            char *str;
                        
            ... ...
            
            str = SSL_alert_type_string(val);
        }
        
        
3.29 const char* ``SSL_alert_type_string_long`` (int value)

    Arguments::
    
        value - SSL type description
    
    Return::
    
        alert type long string
        
    Description::
    
        get alert type long string
    
    Example::
    
        void example(void)
        {
            int val;
            char *str;
                        
            ... ...
            
            str = SSL_alert_type_string_long(val);
        }

3.30 const char* ``SSL_rstate_string`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        state string
        
    Description::
    
        get the state string where SSL is reading
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            char *str;
                        
            ... ...
            
            str = SSL_rstate_string(ssl);
        }
        

3.31 const char* ``SSL_rstate_string_long`` (SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        state long string
        
    Description::
    
        get the state long string where SSL is reading
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            char *str;
                        
            ... ...
            
            str = SSL_rstate_string_long(ssl);
        }


3.32 const char* ``SSL_state_string`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        state string
        
    Description::
    
        get the state string
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            const char *str;
                        
            ... ...
            
            str = SSL_state_string(ssl);
        }


3.33 char* ``SSL_state_string_long`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        state long string
        
    Description::
    
        get the state long string
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            char *str;
                        
            ... ...
            
            str = SSL_state_string(ssl);
        }


3.34 int ``SSL_get_error`` (const SSL *ssl, int ret_code)

    Arguments::
    
        ssl      - SSL point
        ret_code - SSL return code
    
    Return::
    
        SSL error number
        
    Description::
    
        get SSL error code
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
            int err;
                        
            ... ...
            
            err = SSL_get_error(ssl, ret);
        }

3.35 int ``SSL_want`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        specifical statement
        
    Description::
    
        get the SSL specifical statement
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int state;
                        
            ... ...
            
            state = SSL_want(ssl);
        }
        

3.36 int ``SSL_want_nothing`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        0 : false
        1 : true
        
    Description::
    
        check if SSL want nothing
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
                        
            ... ...
            
            ret = SSL_want(ssl);
        }
        
        
3.37 int ``SSL_want_read`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        0 : false
        1 : true
        
    Description::
    
        check if SSL want to read
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
                        
            ... ...
            
            ret = SSL_want_read(ssl);
        }


3.38 int ``SSL_want_write`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        0 : false
        1 : true
        
    Description::
    
        check if SSL want to write
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            int ret;
                        
            ... ...
            
            ret = SSL_want_write(ssl);
        }        


Chapter 4. SSL X509 Certification and Private Key Function
==========================================================


4.1 X509* ``d2i_X509`` (X509 **cert, const unsigned char *buffer, long len)

    Arguments::
    
        cert   - a point pointed to X509 certification
        buffer - a point pointed to the certification context memory point
        length - certification bytes
    
    Return::
    
        X509 certification object point
        
    Description::
    
        load a character certification context into system context. If '*cert' is pointed to the
        certification, then load certification into it. Or create a new X509 certification object
    
    Example::
    
        void example(void)
        {
            X509 *new;
            X509 *cert;
            unsigned char *buffer;
            long len;            
            ... ...
            
            new = d2i_X509(&cert, buffer, len);
        }


4.2 int ``SSL_add_client_CA`` (SSL *ssl, X509 *x)

    Arguments::
    
        ssl - SSL point
        x   - CA certification point
    
    Return::

        1 : OK
        0 : failed
        
    Description::
    
        add CA client certification into the SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            X509 *new;
                        
            ... ...
            
            ret = SSL_add_client_CA(ssl, new);
        }


4.3 int ``SSL_CTX_add_client_CA`` (SSL_CTX *ctx, X509 *x)

    Arguments::
    
        ctx - SSL context point
         x   - CA certification point
    
    Return::

        1 : OK
        0 : failed
        
    Description::
    
        add CA client certification into the SSL context
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL_CTX *ctx;
            X509 *new;
                        
            ... ...
            
            ret = SSL_add_clSSL_CTX_add_client_CAient_CA(ctx, new);
        }


4.4 X509* ``SSL_get_certificate`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        SSL certification point
        
    Description::
    
        get the SSL certification point
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            X509 *cert;
                        
            ... ...
            
            cert = SSL_get_certificate(ssl);
        }


4.5 long ``SSL_get_verify_result`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        the result of verifying
        
    Description::
    
        get the verifying result of the SSL certification
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            long ret;
                        
            ... ...
            
            ret = SSL_get_verify_result(ssl);
        }


4.6 int ``SSL_CTX_use_certificate`` (SSL_CTX *ctx, X509 *x)

    Arguments::
    
        ctx  - the SSL context point
        pkey - certification object point
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        load the certification into the SSL_CTX or SSL object
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL_CTX *ctx
            X509 *new;
                        
            ... ...
            
            ret = SSL_CTX_use_certificate(ctx, new);
        }


4.7 int ``SSL_CTX_use_certificate_ASN1`` (SSL_CTX *ctx, int len, const unsigned char *d)

    Arguments::
    
        ctx - SSL context point
        len - certification length
        d   - data point
    
    Return::
     
        1 : OK
        0 : failed
        
    Description::
    
        load the ASN1 certification into SSL context
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL_CTX *ctx;
            const unsigned char *buf;
            int len;
                        
            ... ...
            
            ret = SSL_CTX_use_certificate_ASN1(ctx, len, buf);
        }
        

4.8 int ``SSL_CTX_use_PrivateKey`` (SSL_CTX *ctx, EVP_PKEY *pkey)

    Arguments::
    
        ctx - SSL context point
        pkey - private key object point
    
    Return::

        1 : OK
        0 : failed
        
    Description::
    
        load the private key into the context object
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL_CTX *ctx;
            EVP_PKEY *pkey;
                        
            ... ...
            
            ret = SSL_CTX_use_PrivateKey(ctx, pkey);
        }


4.9 int ``SSL_CTX_use_PrivateKey_ASN1`` (int pk, SSL_CTX *ctx, const unsigned char *d,  long len)

    Arguments::
    
        ctx - SSL context point
        d   - data point
        len - private key length
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        load the ASN1 private key into SSL context
    
    Example::
    
        void example(void)
        {
            int ret;
            int pk;
            SSL_CTX *ctx;
            const unsigned char *buf;
            long len;
                        
            ... ...
            
            ret = SSL_CTX_use_PrivateKey_ASN1(pk, ctx, buf, len);
        }
        

4.10 int ``SSL_CTX_use_RSAPrivateKey_ASN1`` (SSL_CTX *ctx, const unsigned char *d, long len)

    Arguments::
    
        ctx - SSL context point
        d   - data point
        len - private key length
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        load the RSA ASN1 private key into SSL context
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL_CTX *ctx;
            const unsigned char *buf;
            long len;
                        
            ... ...
            
            ret = SSL_CTX_use_RSAPrivateKey_ASN1(ctx, buf, len);
        }


4.11 int ``SSL_use_certificate_ASN1`` (SSL *ssl, int len, const unsigned char *d)

    Arguments::
    
        ssl - SSL point
        len - data bytes
        d   - data point
    
    Return::
    
        1 : OK
        0 : failed
        
    Description::
    
        load certification into the SSL
    
    Example::
    
        void example(void)
        {
            int ret;
            SSL *ssl;
            const unsigned char *buf;
            long len;
                        
            ... ...
            
            ret = SSL_use_certificate_ASN1(ssl, len, buf);
        }


4.12 X509* ``SSL_get_peer_certificate`` (const SSL *ssl)

    Arguments::
    
        ssl - SSL point
    
    Return::
    
        peer certification
        
    Description::
    
        get peer certification
    
    Example::
    
        void example(void)
        {
            SSL *ssl;
            X509 *peer;
                        
            ... ...
            
            peer = SSL_get_peer_certificate(ssl);
        }

