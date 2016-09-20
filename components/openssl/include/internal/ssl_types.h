#ifndef _SSL_TYPES_H_
#define _SSL_TYPES_H_

#include "ssl_code.h"
#include <stddef.h>

typedef void SSL_CIPHER;

typedef void X509_STORE_CTX;
typedef void X509_NAME;
typedef void X509_STORE;

typedef void RSA;

typedef void STACK;
typedef void BIO;

#define STACK_OF(x) x

struct ssl_method_st;
typedef struct ssl_method_st SSL_METHOD;

struct ssl_method_func_st;
typedef struct ssl_method_func_st SSL_METHOD_FUNC;

struct record_layer_st;
typedef struct record_layer_st RECORD_LAYER;

struct ossl_statem_st;
typedef struct ossl_statem_st OSSL_STATEM;

struct ssl_session_st;
typedef struct ssl_session_st SSL_SESSION;

struct ssl_ctx_st;
typedef struct ssl_ctx_st SSL_CTX;

struct ssl_st;
typedef struct ssl_st SSL;

struct cert_st;
typedef struct cert_st CERT;

struct x509_st;
typedef struct x509_st X509;

struct evp_pkey_st;
typedef struct evp_pkey_st EVP_PKEY;

struct evp_pkey_st {

    void *pkey_pm;
};

struct x509_st {

    /* X509 certification platform private point */
    void *x509_pm;
};

struct cert_st {

    int sec_level;

    X509 *x509;

    EVP_PKEY *pkey;

};

struct ossl_statem_st {
    MSG_FLOW_STATE state;

    int hand_state;
};

struct record_layer_st {

    int rstate;

    int read_ahead;
};

struct ssl_session_st {

    long timeout;

    long time;
};

struct ssl_ctx_st
{
    int version;

    int references;

    unsigned long options;

    #if 0
        struct alpn_protocols alpn_protocol;
    #endif

    const SSL_METHOD *method;

    CERT *cert;

    X509 *client_CA;

    int verify_mode;

    long session_timeout;

    int read_ahead;
};

struct ssl_st
{
    /* protocol version(one of SSL3.0, TLS1.0, etc.) */
    int version;

    unsigned long options;

    /* shut things down(0x01 : sent, 0x02 : received) */
    int shutdown;

    CERT *cert;

    SSL_CTX  *ctx;

    const SSL_METHOD *method;

    RECORD_LAYER rlayer;

    /* where we are */
    OSSL_STATEM statem;

    SSL_SESSION session;

    int rwstate;

    int err;

    void (*info_callback) (const SSL *ssl, int type, int val);

    /* SSL low-level system arch point */
    void *ssl_pm;
};

struct ssl_method_st {
    /* protocol version(one of SSL3.0, TLS1.0, etc.) */
    int version;

    /* SSL mode(client(0) , server(1), not known(-1)) */
    int endpoint;

    const SSL_METHOD_FUNC *func;
};

struct ssl_method_func_st {

    int (*ssl_new)(SSL *ssl);

    void (*ssl_free)(SSL *ssl);

    int (*ssl_handshake)(SSL *ssl);

    int (*ssl_shutdown)(SSL *ssl);

    int (*ssl_clear)(SSL *ssl);

    int (*ssl_read)(SSL *ssl, void *buffer, int len);

    int (*ssl_send)(SSL *ssl, const void *buffer, int len);

    int (*ssl_pending)(const SSL *ssl);

    void (*ssl_set_fd)(SSL *ssl, int fd, int mode);

    int (*ssl_get_fd)(const SSL *ssl, int mode);

    void (*ssl_set_bufflen)(SSL *ssl, int len);

    OSSL_HANDSHAKE_STATE (*ssl_get_state)(const SSL *ssl);
};

typedef int (*next_proto_cb)(SSL *ssl, unsigned char **out,
                             unsigned char *outlen, const unsigned char *in,
                             unsigned int inlen, void *arg);

#endif
