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

#include "ssl_lib.h"
#include "ssl_pkey.h"
#include "ssl_x509.h"
#include "ssl_cert.h"
#include "ssl_dbg.h"
#include "ssl_port.h"

#define SSL_SEND_DATA_MAX_LENGTH 1460

/*
 * ossl_statem_in_error - Discover whether the current connection is in the error state
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : Yes
 *         0 : no
 */
int ossl_statem_in_error(const SSL *ssl)
{
    if (ssl->statem.state == MSG_FLOW_ERROR)
        return 1;

    return 0;
}

/*
 * SSL_want - get the SSL specifical statement
 *
 * @param ssl - SSL point
 *
 * @return specifical statement
 */
int SSL_want(const SSL *ssl)
{
    return ssl->rwstate;
}

/*
 * SSL_want_nothing - check if SSL want nothing
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : yes
 *         0 : no
 */
int SSL_want_nothing(const SSL *ssl)
{
    return (SSL_want(ssl) == SSL_NOTHING);
}

/*
 * SSL_want_read - check if SSL want to read
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : yes
 *         0 : no
 */
int SSL_want_read(const SSL *ssl)
{
    return (SSL_want(ssl) == SSL_READING);
}

/*
 * SSL_want_read - check if SSL want to write
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : yes
 *         0 : no
 */
int SSL_want_write(const SSL *ssl)
{
    return (SSL_want(ssl) == SSL_WRITING);
}

/*
 * SSL_want_read - check if SSL want to lookup X509 certification
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : yes
 *         0 : no
 */
int SSL_want_x509_lookup(const SSL *ssl)
{
    return (SSL_want(ssl) == SSL_WRITING);
}

/*
 * SSL_get_error - get SSL error code
 *
 * @param ssl       - SSL point
 * @param ret_code  - SSL return code
 *
 * @return SSL error number
 */
int SSL_get_error(const SSL *ssl, int ret_code)
{
    int ret = SSL_ERROR_SYSCALL;

    SSL_ASSERT(ssl);

    if (ret_code > 0)
        ret = SSL_ERROR_NONE;
    else if (ret_code < 0)
    {
        if (SSL_want_read(ssl))
            ret = SSL_ERROR_WANT_READ;
        else if (SSL_want_write(ssl))
            ret = SSL_ERROR_WANT_WRITE;
        else
            ret = SSL_ERROR_SYSCALL; //unknown
    }
    else // ret_code == 0
    {
        if (ssl->shutdown & SSL_RECEIVED_SHUTDOWN)
            ret = SSL_ERROR_ZERO_RETURN;
        else
            ret = SSL_ERROR_SYSCALL;
    }

    return ret;
}

/*
 * SSL_get_state - get the SSL state
 *
 * @param ssl - SSL point
 *
 * @return SSL state
 */
OSSL_HANDSHAKE_STATE SSL_get_state(const SSL *ssl)
{
    OSSL_HANDSHAKE_STATE state;

    SSL_ASSERT(ssl);

    state = SSL_METHOD_CALL(get_state, ssl);

    return state;
}

/*
 * SSL_CTX_new - create a SSL context
 *
 * @param method - the SSL context configuration file
 *
 * @return the context point, if create failed return NULL
 */
SSL_CTX* SSL_CTX_new(const SSL_METHOD *method)
{
    SSL_CTX *ctx;
    CERT *cert;
    X509 *client_ca;

    if (!method) SSL_RET(go_failed1, "method\n");

    client_ca = X509_new();
    if (!client_ca)
        SSL_RET(go_failed1, "sk_X509_NAME_new_null\n");

    cert = ssl_cert_new();
    if (!cert)
        SSL_RET(go_failed2, "ssl_cert_new\n");

    ctx = (SSL_CTX *)ssl_zalloc(sizeof(SSL_CTX));
    if (!ctx)
        SSL_RET(go_failed3, "ssl_ctx_new:ctx\n");

    ctx->method = method;
    ctx->client_CA = client_ca;
    ctx->cert = cert;

    ctx->version = method->version;

    return ctx;

go_failed3:
    ssl_cert_free(cert);
go_failed2:
    X509_free(client_ca);
go_failed1:
    return NULL;
}

/*
 * SSL_CTX_free - free a SSL context
 *
 * @param method - the SSL context point
 *
 * @return none
 */
void SSL_CTX_free(SSL_CTX* ctx)
{
    SSL_ASSERT(ctx);

    ssl_cert_free(ctx->cert);

    X509_free(ctx->client_CA);

    ssl_free(ctx);
}

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
int SSL_CTX_set_ssl_version(SSL_CTX *ctx, const SSL_METHOD *meth)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(meth);

    ctx->method = meth;

    ctx->version = meth->version;

    return 1;
}

/*
 * SSL_CTX_get_ssl_method - get the SSL context current method
 *
 * @param ctx - SSL context point
 *
 * @return the SSL context current method
 */
const SSL_METHOD *SSL_CTX_get_ssl_method(SSL_CTX *ctx)
{
    SSL_ASSERT(ctx);

    return ctx->method;
}

/*
 * SSL_new - create a SSL
 *
 * @param ctx - the SSL context point
 *
 * @return the SSL point or NULL if failed
 */
SSL *SSL_new(SSL_CTX *ctx)
{
    int ret;
    SSL *ssl;

    if (!ctx)
        SSL_RET(failed1, "ctx:NULL\n");

    ssl = (SSL *)ssl_zalloc(sizeof(SSL));
    if (!ssl)
        SSL_RET(failed1, "ssl_zalloc\n");

    ssl->ctx = ctx;
    ssl->method = ctx->method;

    ssl->version = ctx->version;
    ssl->options = ctx->options;

    ret = SSL_METHOD_CALL(new, ssl);
    if (ret)
        SSL_RET(failed2, "ssl_new\n");

    ssl->rwstate = SSL_NOTHING;

    return ssl;

failed2:
    ssl_free(ssl);
failed1:
    return NULL;
}

/*
 * SSL_free - free the SSL
 *
 * @param ssl - the SSL point
 *
 * @return none
 */
void SSL_free(SSL *ssl)
{
    SSL_ASSERT(ssl);

    SSL_METHOD_CALL(free, ssl);

    ssl_free(ssl);
}

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
int SSL_do_handshake(SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    ret = SSL_METHOD_CALL(handshake, ssl);

    return ret;
}

/*
 * SSL_connect - connect to the remote SSL server
 *
 * @param ssl - the SSL point
 *
 * @return
 *         1 : OK
 *        -1 : failed
 */
int SSL_connect(SSL *ssl)
{
    SSL_ASSERT(ssl);

    return SSL_do_handshake(ssl);
}

/*
 * SSL_accept - accept the remote connection
 *
 * @param ssl - the SSL point
 *
 * @return
 *         1 : OK
 *        -1 : failed
 */
int SSL_accept(SSL *ssl)
{
    SSL_ASSERT(ssl);

    return SSL_do_handshake(ssl);
}

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
int SSL_shutdown(SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    if (SSL_get_state(ssl) != TLS_ST_OK) return 0;

    ret = SSL_METHOD_CALL(shutdown, ssl);

    return ret;
}

/*
 * SSL_clear - reset the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_clear(SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    ret = SSL_shutdown(ssl);
    if (1 != ret)
        SSL_ERR(0, go_failed1, "SSL_shutdown\n");

    SSL_METHOD_CALL(free, ssl);

    ret = SSL_METHOD_CALL(new, ssl);
    if (!ret)
        SSL_ERR(0, go_failed1, "ssl_new\n");

    return 1;

go_failed1:
    return ret;
}

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
int SSL_read(SSL *ssl, void *buffer, int len)
{
    int ret;

    SSL_ASSERT(ssl);
    SSL_ASSERT(buffer);
    SSL_ASSERT(len);

    ssl->rwstate = SSL_READING;

    ret = SSL_METHOD_CALL(read, ssl, buffer, len);

    ssl->rwstate = SSL_NOTHING;

    return ret;
}

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
int SSL_write(SSL *ssl, const void *buffer, int len)
{
    int ret;
    int send_bytes;
    const unsigned char *pbuf;

    SSL_ASSERT(ssl);
    SSL_ASSERT(buffer);
    SSL_ASSERT(len);

    ssl->rwstate = SSL_WRITING;

    send_bytes = len;
    pbuf = (const unsigned char *)buffer;

    do {
        int bytes;

        if (send_bytes > SSL_SEND_DATA_MAX_LENGTH)
            bytes = SSL_SEND_DATA_MAX_LENGTH;
        else
            bytes = send_bytes;

        ret = SSL_METHOD_CALL(send, ssl, buffer, bytes);
        if (ret > 0) {
            pbuf += ret;
            send_bytes -= ret;
        }
    } while (ret > 0 && send_bytes);

    ssl->rwstate = SSL_NOTHING;

    send_bytes = len - send_bytes;
    if (send_bytes >= 0)
        ret = send_bytes;
    else
        ret = -1;

    return ret;
}

/*
 * SSL_get_SSL_CTX - get SSL context of the SSL
 *
 * @param ssl - SSL point
 *
 * @return SSL context
 */
SSL_CTX *SSL_get_SSL_CTX(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->ctx;
}

/*
 * SSL_CTX_get_ssl_method - get the SSL current method
 *
 * @param ssl - SSL point
 *
 * @return the SSL current method
 */
const SSL_METHOD *SSL_get_ssl_method(SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->method;
}

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
int SSL_set_ssl_method(SSL *ssl, const SSL_METHOD *method)
{
    int ret;

    SSL_ASSERT(ssl);
    SSL_ASSERT(method);

    if (ssl->version != method->version) {

        ret = SSL_shutdown(ssl);
        if (1 != ret)
            SSL_ERR(0, go_failed1, "SSL_shutdown\n");

        SSL_METHOD_CALL(free, ssl);

        ssl->method = method;

        ret = SSL_METHOD_CALL(new, ssl);
        if (!ret)
            SSL_ERR(0, go_failed1, "ssl_new\n");
    } else {
        ssl->method = method;
    }


    return 1;

go_failed1:
    return ret;
}

/*
 * SSL_get_shutdown - get SSL shutdown mode
 *
 * @param ssl - SSL point
 *
 * @return shutdown mode
 */
int SSL_get_shutdown(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->shutdown;
}

/*
 * SSL_set_quiet_shutdown - set SSL shutdown mode
 *
 * @param ssl  - SSL point
 * @param mode - shutdown mode
 *
 * @return none
 */
void SSL_set_shutdown(SSL *ssl, int mode)
{
    SSL_ASSERT(ssl);

    ssl->shutdown = mode;
}


/*
 * SSL_pending - get the number of the bytes to be read
 *
 * @param ssl - SSL point
 *
 * @return number of the bytes
 */
int SSL_pending(const SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    ret = SSL_METHOD_CALL(pending, ssl);

    return ret;
}

/*
 * SSL_has_pending - check if some data can be read
 *
 * @param ssl - SSL point
 *
 * @return
 *         1 : there are bytes to be read
 *         0 : no data
 */
int SSL_has_pending(const SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    if (SSL_pending(ssl))
        ret = 1;
    else
        ret = 0;

    return ret;
}

/*
 * SSL_CTX_clear_options - clear the SSL context option bit of "op"
 *
 * @param ctx - SSL context point
 * @param op  - option
 *
 * @return SSL context option
 */
unsigned long SSL_CTX_clear_options(SSL_CTX *ctx, unsigned long op)
{
    return ctx->options &= ~op;
}

/*
 * SSL_CTX_clear_options - get the SSL context option
 *
 * @param ctx - SSL context point
 * @param op  - option
 *
 * @return SSL context option
 */
unsigned long SSL_CTX_get_options(SSL_CTX *ctx)
{
    return ctx->options;
}

/*
 * SSL_CTX_set_option - set the option of the SSL context
 *
 * @param ctx - the SSL context
 *
 * @return the SSL context option
 *
 */
unsigned long SSL_CTX_set_options(SSL_CTX *ctx, unsigned long opt)
{
    return ctx->options |= opt;
}

/*
 * SSL_clear_options - clear SSL option
 *
 * @param ssl - SSL point
 * @param op  - clear option
 *
 * @return SSL option
 */
unsigned long SSL_clear_options(SSL *ssl, unsigned long op)
{
    SSL_ASSERT(ssl);

    return ssl->options & ~op;
}

/*
 * SSL_clear_options - get SSL option
 *
 * @param ssl - SSL point
 *
 * @return SSL option
 */
unsigned long SSL_get_options(SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->options;
}

/*
 * SSL_clear_options - clear SSL option
 *
 * @param ssl - SSL point
 * @param op  - setting option
 *
 * @return SSL option
 */
unsigned long SSL_set_options(SSL *ssl, unsigned long op)
{
    SSL_ASSERT(ssl);

    return ssl->options |= op;
}

/*
 * SSL_get_fd - get the socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *      >= 0 : yes, and return socket handle
 *       < 0 : a error catch
 */
int SSL_get_fd(const SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    ret = SSL_METHOD_CALL(get_fd, ssl, 0);

    return ret;
}

/*
 * SSL_get_rfd - get the read only socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *      >= 0 : yes, and return socket handle
 *       < 0 : a error catch
 */
int SSL_get_rfd(const SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    ret = SSL_METHOD_CALL(get_fd, ssl, 0);

    return ret;
}

/*
 * SSL_get_wfd - get the write only socket handle of the SSL
 *
 * @param ssl - SSL point
 *
 * @return
 *      >= 0 : yes, and return socket handle
 *       < 0 : a error catch
 */
int SSL_get_wfd(const SSL *ssl)
{
    int ret;

    SSL_ASSERT(ssl);

    ret = SSL_METHOD_CALL(get_fd, ssl, 0);

    return ret;
}

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
int SSL_set_fd(SSL *ssl, int fd)
{
    SSL_ASSERT(ssl);
    SSL_ASSERT(fd >= 0);

    SSL_METHOD_CALL(set_fd, ssl, fd, 0);

    return 1;
}

/*
 * SSL_set_fd - bind the read only socket file description into the SSL
 *
 * @param ssl - the SSL point
 * @param fd  - socket handle
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_set_rfd(SSL *ssl, int fd)
{
    SSL_ASSERT(ssl);
    SSL_ASSERT(fd >= 0);

    SSL_METHOD_CALL(set_fd, ssl, fd, 0);

    return 1;
}

/*
 * SSL_set_fd - bind the write only socket file description into the SSL
 *
 * @param ssl - the SSL point
 * @param fd  - socket handle
 *
 * @return
 *        1 : OK
 *        0 : failed
 */
int SSL_set_wfd(SSL *ssl, int fd)
{
    SSL_ASSERT(ssl);
    SSL_ASSERT(fd >= 0);

    SSL_METHOD_CALL(set_fd, ssl, fd, 0);

    return 1;
}

/*
 * SSL_version - get SSL version
 *
 * @param ssl - SSL point
 *
 * @return SSL version
 */
int SSL_version(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->version;
}

/*
 * ssl_protocol_to_string - get the SSL version string
 *
 * @param version - the SSL version
 *
 * @return the SSL version string
 */
static const char* ssl_protocol_to_string(int version)
{
    const char *str;

    if (version == TLS1_2_VERSION)
        str = "TLSv1.2";
    else if (version == TLS1_1_VERSION)
        str = "TLSv1.1";
    else if (version == TLS1_VERSION)
        str = "TLSv1";
    else if (version == SSL3_VERSION)
        str = "SSLv3";
    else
        str = "unknown";

    return str;
}

/*
 * SSL_get_version - get the SSL current version
 *
 * @param ssl - SSL point
 *
 * @return the version string
 */
const char *SSL_get_version(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl_protocol_to_string(SSL_version(ssl));
}

/*
 * SSL_alert_desc_string - get alert description string
 *
 * @param value - alert value
 *
 * @return alert description string
 */
const char* SSL_alert_desc_string(int value)
{
    const char *str;

    switch (value & 0xff)
    {
        case SSL3_AD_CLOSE_NOTIFY:
            str = "CN";
            break;
        case SSL3_AD_UNEXPECTED_MESSAGE:
            str = "UM";
            break;
        case SSL3_AD_BAD_RECORD_MAC:
            str = "BM";
            break;
        case SSL3_AD_DECOMPRESSION_FAILURE:
            str = "DF";
            break;
        case SSL3_AD_HANDSHAKE_FAILURE:
            str = "HF";
            break;
        case SSL3_AD_NO_CERTIFICATE:
            str = "NC";
            break;
        case SSL3_AD_BAD_CERTIFICATE:
            str = "BC";
            break;
        case SSL3_AD_UNSUPPORTED_CERTIFICATE:
            str = "UC";
            break;
        case SSL3_AD_CERTIFICATE_REVOKED:
            str = "CR";
            break;
        case SSL3_AD_CERTIFICATE_EXPIRED:
            str = "CE";
            break;
        case SSL3_AD_CERTIFICATE_UNKNOWN:
            str = "CU";
            break;
        case SSL3_AD_ILLEGAL_PARAMETER:
            str = "IP";
            break;
        case TLS1_AD_DECRYPTION_FAILED:
            str = "DC";
            break;
        case TLS1_AD_RECORD_OVERFLOW:
            str = "RO";
            break;
        case TLS1_AD_UNKNOWN_CA:
            str = "CA";
            break;
        case TLS1_AD_ACCESS_DENIED:
            str = "AD";
            break;
        case TLS1_AD_DECODE_ERROR:
            str = "DE";
            break;
        case TLS1_AD_DECRYPT_ERROR:
            str = "CY";
            break;
        case TLS1_AD_EXPORT_RESTRICTION:
            str = "ER";
            break;
        case TLS1_AD_PROTOCOL_VERSION:
            str = "PV";
            break;
        case TLS1_AD_INSUFFICIENT_SECURITY:
            str = "IS";
            break;
        case TLS1_AD_INTERNAL_ERROR:
            str = "IE";
            break;
        case TLS1_AD_USER_CANCELLED:
            str = "US";
            break;
        case TLS1_AD_NO_RENEGOTIATION:
            str = "NR";
            break;
        case TLS1_AD_UNSUPPORTED_EXTENSION:
            str = "UE";
            break;
        case TLS1_AD_CERTIFICATE_UNOBTAINABLE:
            str = "CO";
            break;
        case TLS1_AD_UNRECOGNIZED_NAME:
            str = "UN";
            break;
        case TLS1_AD_BAD_CERTIFICATE_STATUS_RESPONSE:
            str = "BR";
            break;
        case TLS1_AD_BAD_CERTIFICATE_HASH_VALUE:
            str = "BH";
            break;
        case TLS1_AD_UNKNOWN_PSK_IDENTITY:
            str = "UP";
            break;
        default:
            str = "UK";
            break;
    }

    return str;
}

/*
 * SSL_alert_desc_string - get alert description long string
 *
 * @param value - alert value
 *
 * @return alert description long string
 */
const char* SSL_alert_desc_string_long(int value)
{
    const char *str;

    switch (value & 0xff)
    {
        case SSL3_AD_CLOSE_NOTIFY:
            str = "close notify";
            break;
        case SSL3_AD_UNEXPECTED_MESSAGE:
            str = "unexpected_message";
            break;
        case SSL3_AD_BAD_RECORD_MAC:
            str = "bad record mac";
            break;
        case SSL3_AD_DECOMPRESSION_FAILURE:
            str = "decompression failure";
            break;
        case SSL3_AD_HANDSHAKE_FAILURE:
            str = "handshake failure";
            break;
        case SSL3_AD_NO_CERTIFICATE:
            str = "no certificate";
            break;
        case SSL3_AD_BAD_CERTIFICATE:
            str = "bad certificate";
            break;
        case SSL3_AD_UNSUPPORTED_CERTIFICATE:
            str = "unsupported certificate";
            break;
        case SSL3_AD_CERTIFICATE_REVOKED:
            str = "certificate revoked";
            break;
        case SSL3_AD_CERTIFICATE_EXPIRED:
            str = "certificate expired";
            break;
        case SSL3_AD_CERTIFICATE_UNKNOWN:
            str = "certificate unknown";
            break;
        case SSL3_AD_ILLEGAL_PARAMETER:
            str = "illegal parameter";
            break;
        case TLS1_AD_DECRYPTION_FAILED:
            str = "decryption failed";
            break;
        case TLS1_AD_RECORD_OVERFLOW:
            str = "record overflow";
            break;
        case TLS1_AD_UNKNOWN_CA:
            str = "unknown CA";
            break;
        case TLS1_AD_ACCESS_DENIED:
            str = "access denied";
            break;
        case TLS1_AD_DECODE_ERROR:
            str = "decode error";
            break;
        case TLS1_AD_DECRYPT_ERROR:
            str = "decrypt error";
            break;
        case TLS1_AD_EXPORT_RESTRICTION:
            str = "export restriction";
            break;
        case TLS1_AD_PROTOCOL_VERSION:
            str = "protocol version";
            break;
        case TLS1_AD_INSUFFICIENT_SECURITY:
            str = "insufficient security";
            break;
        case TLS1_AD_INTERNAL_ERROR:
            str = "internal error";
            break;
        case TLS1_AD_USER_CANCELLED:
            str = "user canceled";
            break;
        case TLS1_AD_NO_RENEGOTIATION:
            str = "no renegotiation";
            break;
        case TLS1_AD_UNSUPPORTED_EXTENSION:
            str = "unsupported extension";
            break;
        case TLS1_AD_CERTIFICATE_UNOBTAINABLE:
            str = "certificate unobtainable";
            break;
        case TLS1_AD_UNRECOGNIZED_NAME:
            str = "unrecognized name";
            break;
        case TLS1_AD_BAD_CERTIFICATE_STATUS_RESPONSE:
            str = "bad certificate status response";
            break;
        case TLS1_AD_BAD_CERTIFICATE_HASH_VALUE:
            str = "bad certificate hash value";
            break;
        case TLS1_AD_UNKNOWN_PSK_IDENTITY:
            str = "unknown PSK identity";
            break;
        default:
            str = "unknown";
            break;
    }

    return str;
}

/*
 * SSL_alert_type_string - get alert type string
 *
 * @param value - alert value
 *
 * @return alert type string
 */
const char *SSL_alert_type_string(int value)
{
    const char *str;

    switch (value >> 8)
    {
    case SSL3_AL_WARNING:
        str = "W";
        break;
    case SSL3_AL_FATAL:
        str = "F";
        break;
    default:
        str = "U";
        break;
    }

    return str;
}

/*
 * SSL_alert_type_string_long - get alert type long string
 *
 * @param value - alert value
 *
 * @return alert type long string
 */
const char *SSL_alert_type_string_long(int value)
{
    const char *str;

    switch (value >> 8)
    {
        case SSL3_AL_WARNING:
            str = "warning";
            break;
        case SSL3_AL_FATAL:
            str = "fatal";
            break;
        default:
            str = "unknown";
            break;
    }

    return str;
}

/*
 * SSL_rstate_string - get the state string where SSL is reading
 *
 * @param ssl - SSL point
 *
 * @return state string
 */
const char *SSL_rstate_string(SSL *ssl)
{
    const char *str;

    SSL_ASSERT(ssl);

    switch (ssl->rlayer.rstate)
    {
        case SSL_ST_READ_HEADER:
            str = "RH";
            break;
        case SSL_ST_READ_BODY:
            str = "RB";
            break;
        case SSL_ST_READ_DONE:
            str = "RD";
            break;
        default:
            str = "unknown";
            break;
    }

    return str;
}

/*
 * SSL_rstate_string_long - get the statement long string where SSL is reading
 *
 * @param ssl - SSL point
 *
 * @return statement long string
 */
const char *SSL_rstate_string_long(SSL *ssl)
{
    const char *str = "unknown";

    SSL_ASSERT(ssl);

    switch (ssl->rlayer.rstate)
    {
        case SSL_ST_READ_HEADER:
            str = "read header";
            break;
        case SSL_ST_READ_BODY:
            str = "read body";
            break;
        case SSL_ST_READ_DONE:
            str = "read done";
            break;
        default:
            break;
    }

    return str;
}

/*
 * SSL_state_string - get SSL statement string
 *
 * @param ssl - SSL point
 *
 * @return SSL statement string
 */
char *SSL_state_string(const SSL *ssl)
{
    char *str = "UNKWN ";

    SSL_ASSERT(ssl);

    if (ossl_statem_in_error(ssl))
        str = "SSLERR";
    else
    {
        switch (SSL_get_state(ssl))
        {
            case TLS_ST_BEFORE:
                str = "PINIT ";
                break;
            case TLS_ST_OK:
                str =  "SSLOK ";
                break;
            case TLS_ST_CW_CLNT_HELLO:
                str = "TWCH";
                break;
            case TLS_ST_CR_SRVR_HELLO:
                str = "TRSH";
                break;
            case TLS_ST_CR_CERT:
                str = "TRSC";
                break;
            case TLS_ST_CR_KEY_EXCH:
                str = "TRSKE";
                break;
            case TLS_ST_CR_CERT_REQ:
                str = "TRCR";
                break;
            case TLS_ST_CR_SRVR_DONE:
                str = "TRSD";
                break;
            case TLS_ST_CW_CERT:
                str = "TWCC";
                break;
            case TLS_ST_CW_KEY_EXCH:
                str = "TWCKE";
                break;
            case TLS_ST_CW_CERT_VRFY:
                str = "TWCV";
                break;
            case TLS_ST_SW_CHANGE:
            case TLS_ST_CW_CHANGE:
                str = "TWCCS";
                break;
            case TLS_ST_SW_FINISHED:
            case TLS_ST_CW_FINISHED:
                str = "TWFIN";
                break;
            case TLS_ST_SR_CHANGE:
            case TLS_ST_CR_CHANGE:
                str = "TRCCS";
                break;
            case TLS_ST_SR_FINISHED:
            case TLS_ST_CR_FINISHED:
                str = "TRFIN";
                break;
            case TLS_ST_SW_HELLO_REQ:
                str = "TWHR";
                break;
            case TLS_ST_SR_CLNT_HELLO:
                str = "TRCH";
                break;
            case TLS_ST_SW_SRVR_HELLO:
                str = "TWSH";
                break;
            case TLS_ST_SW_CERT:
                str = "TWSC";
                break;
            case TLS_ST_SW_KEY_EXCH:
                str = "TWSKE";
                break;
            case TLS_ST_SW_CERT_REQ:
                str = "TWCR";
                break;
            case TLS_ST_SW_SRVR_DONE:
                str = "TWSD";
                break;
            case TLS_ST_SR_CERT:
                str = "TRCC";
                break;
            case TLS_ST_SR_KEY_EXCH:
                str = "TRCKE";
                break;
            case TLS_ST_SR_CERT_VRFY:
                str = "TRCV";
                break;
            case DTLS_ST_CR_HELLO_VERIFY_REQUEST:
                str = "DRCHV";
                break;
            case DTLS_ST_SW_HELLO_VERIFY_REQUEST:
                str = "DWCHV";
                break;
            default:
                break;
        }
    }

    return str;
}

/*
 * SSL_state_string_long - get SSL statement long string
 *
 * @param ssl - SSL point
 *
 * @return SSL statement long string
 */
char *SSL_state_string_long(const SSL *ssl)
{
    char *str = "UNKWN ";

    SSL_ASSERT(ssl);

    if (ossl_statem_in_error(ssl))
        str = "SSLERR";
    else
    {
        switch (SSL_get_state(ssl))
        {
            case TLS_ST_BEFORE:
                str = "before SSL initialization";
                break;
            case TLS_ST_OK:
                str = "SSL negotiation finished successfully";
                break;
            case TLS_ST_CW_CLNT_HELLO:
                str = "SSLv3/TLS write client hello";
                break;
            case TLS_ST_CR_SRVR_HELLO:
                str = "SSLv3/TLS read server hello";
                break;
            case TLS_ST_CR_CERT:
                str = "SSLv3/TLS read server certificate";
                break;
            case TLS_ST_CR_KEY_EXCH:
                str = "SSLv3/TLS read server key exchange";
                break;
            case TLS_ST_CR_CERT_REQ:
                str = "SSLv3/TLS read server certificate request";
                break;
            case TLS_ST_CR_SESSION_TICKET:
                str = "SSLv3/TLS read server session ticket";
                break;
            case TLS_ST_CR_SRVR_DONE:
                str = "SSLv3/TLS read server done";
                break;
            case TLS_ST_CW_CERT:
                str = "SSLv3/TLS write client certificate";
                break;
            case TLS_ST_CW_KEY_EXCH:
                str = "SSLv3/TLS write client key exchange";
                break;
            case TLS_ST_CW_CERT_VRFY:
                str = "SSLv3/TLS write certificate verify";
                break;
            case TLS_ST_CW_CHANGE:
            case TLS_ST_SW_CHANGE:
                str = "SSLv3/TLS write change cipher spec";
                break;
            case TLS_ST_CW_FINISHED:
            case TLS_ST_SW_FINISHED:
                str = "SSLv3/TLS write finished";
                break;
            case TLS_ST_CR_CHANGE:
            case TLS_ST_SR_CHANGE:
                str = "SSLv3/TLS read change cipher spec";
                break;
            case TLS_ST_CR_FINISHED:
            case TLS_ST_SR_FINISHED:
                str = "SSLv3/TLS read finished";
                break;
            case TLS_ST_SR_CLNT_HELLO:
                str = "SSLv3/TLS read client hello";
                break;
            case TLS_ST_SW_HELLO_REQ:
                str = "SSLv3/TLS write hello request";
                break;
            case TLS_ST_SW_SRVR_HELLO:
                str = "SSLv3/TLS write server hello";
                break;
            case TLS_ST_SW_CERT:
                str = "SSLv3/TLS write certificate";
                break;
            case TLS_ST_SW_KEY_EXCH:
                str = "SSLv3/TLS write key exchange";
                break;
            case TLS_ST_SW_CERT_REQ:
                str = "SSLv3/TLS write certificate request";
                break;
            case TLS_ST_SW_SESSION_TICKET:
                str = "SSLv3/TLS write session ticket";
                break;
            case TLS_ST_SW_SRVR_DONE:
                str = "SSLv3/TLS write server done";
                break;
            case TLS_ST_SR_CERT:
                str = "SSLv3/TLS read client certificate";
                break;
            case TLS_ST_SR_KEY_EXCH:
                str = "SSLv3/TLS read client key exchange";
                break;
            case TLS_ST_SR_CERT_VRFY:
                str = "SSLv3/TLS read certificate verify";
                break;
            case DTLS_ST_CR_HELLO_VERIFY_REQUEST:
                str = "DTLS1 read hello verify request";
                break;
            case DTLS_ST_SW_HELLO_VERIFY_REQUEST:
                str = "DTLS1 write hello verify request";
                break;
            default:
                break;
        }
    }

    return str;
}

/*
 * SSL_CTX_set_default_read_buffer_len - set the SSL context read buffer length
 *
 * @param ctx - SSL context point
 * @param len - read buffer length
 *
 * @return none
 */
void SSL_CTX_set_default_read_buffer_len(SSL_CTX *ctx, size_t len)
{
    SSL_ASSERT(ctx);
    SSL_ASSERT(len);

    ctx->read_buffer_len = len;
}

/*
 * SSL_set_default_read_buffer_len - set the SSL read buffer length
 *
 * @param ssl - SSL point
 * @param len - read buffer length
 *
 * @return none
 */
void SSL_set_default_read_buffer_len(SSL *ssl, size_t len)
{
    SSL_ASSERT(ssl);
    SSL_ASSERT(len);

    SSL_METHOD_CALL(set_bufflen, ssl, len);
}

/*
 * SSL_set_info_callback - set the SSL information callback function
 *
 * @param ssl - SSL point
 * @param cb  - information callback function
 *
 * @return none
 */
void SSL_set_info_callback(SSL *ssl, void (*cb) (const SSL *ssl, int type, int val))
{
    SSL_ASSERT(ssl);

    ssl->info_callback = cb;
}

/*
 * SSL_CTX_up_ref - add SSL context reference count by '1'
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : OK
 *         0 : failed
 */
int SSL_CTX_up_ref(SSL_CTX *ctx)
{
    SSL_ASSERT(ctx);

    /* no support multi-thread SSL here */
    ctx->references++;

    return 1;
}

/*
 * SSL_set_security_level - set the SSL security level
 *
 * @param ssl   - SSL point
 * @param level - security level
 *
 * @return none
 */
void SSL_set_security_level(SSL *ssl, int level)
{
    SSL_ASSERT(ssl);

    ssl->cert->sec_level = level;
}

/*
 * SSL_get_security_level - get the SSL security level
 *
 * @param ssl - SSL point
 *
 * @return security level
 */
int SSL_get_security_level(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->cert->sec_level;
}

/*
 * SSL_CTX_get_verify_mode - get the SSL verifying mode of the SSL context
 *
 * @param ctx - SSL context point
 *
 * @return verifying mode
 */
int SSL_CTX_get_verify_mode(const SSL_CTX *ctx)
{
    SSL_ASSERT(ctx);

    return ctx->verify_mode;
}

/*
 * SSL_CTX_set_timeout - set the session timeout time
 *
 * @param ctx - SSL context point
 * @param t   - new session timeout time
 *
 * @return old session timeout time
 */
long SSL_CTX_set_timeout(SSL_CTX *ctx, long t)
{
    long l;

    SSL_ASSERT(ctx);

    l = ctx->session_timeout;
    ctx->session_timeout = t;

    return l;
}

/*
 * SSL_CTX_get_timeout - get the session timeout time
 *
 * @param ctx - SSL context point
 *
 * @return current session timeout time
 */
long SSL_CTX_get_timeout(const SSL_CTX *ctx)
{
    SSL_ASSERT(ctx);

    return ctx->session_timeout;
}

/*
 * SSL_set_read_ahead - set the SSL if we can read as many as data
 *
 * @param ssl - SSL point
 * @param yes - enable the function
 *
 * @return none
 */
void SSL_set_read_ahead(SSL *ssl, int yes)
{
    SSL_ASSERT(ssl);

    ssl->rlayer.read_ahead = yes;
}

/*
 * SSL_set_read_ahead - set the SSL context if we can read as many as data
 *
 * @param ctx - SSL context point
 * @param yes - enable the function
 *
 * @return none
 */
void SSL_CTX_set_read_ahead(SSL_CTX *ctx, int yes)
{
    SSL_ASSERT(ctx);

    ctx->read_ahead = yes;
}

/*
 * SSL_set_read_ahead - get the SSL ahead signal if we can read as many as data
 *
 * @param ssl - SSL point
 *
 * @return SSL context ahead signal
 */
int SSL_get_read_ahead(const SSL *ssl)
{
    SSL_ASSERT(ssl);

    return ssl->rlayer.read_ahead;
}

/*
 * SSL_set_read_ahead - get the SSL context ahead signal if we can read as many as data
 *
 * @param ctx - SSL context point
 *
 * @return SSL context ahead signal
 */
long SSL_CTX_get_read_ahead(SSL_CTX *ctx)
{
    SSL_ASSERT(ctx);

    return ctx->read_ahead;
}

/*
 * SSL_CTX_get_ciphers - check if the SSL context can read as many as data
 *
 * @param ctx - SSL context point
 *
 * @return
 *         1 : Yes
 *         0 : No
 */
long SSL_CTX_get_default_read_ahead(SSL_CTX *ctx)
{
    SSL_ASSERT(ctx);

    return ctx->read_ahead;
}

/*
 * SSL_set_time - set SSL session time
 *
 * @param ssl - SSL point
 * @param t   - session time
 *
 * @return session time
 */
long SSL_set_time(SSL *ssl, long t)
{
    SSL_ASSERT(ssl);

    ssl->session.time = t;

    return t;
}

/*
 * SSL_set_time - set SSL session timeout time
 *
 * @param ssl - SSL point
 * @param t   - session timeout time
 *
 * @return session timeout time
 */
long SSL_set_timeout(SSL *ssl, long t)
{
    SSL_ASSERT(ssl);

    ssl->session.timeout = t;

    return t;
}

/*
 * SSL_set_verify - set the SSL verifying of the SSL context
 *
 * @param ctx             - SSL point
 * @param mode            - verifying mode
 * @param verify_callback - verifying callback function
 *
 * @return none
 */
void SSL_set_verify(SSL *ssl, int mode, int (*verify_callback)(int, X509_STORE_CTX *))
{
    SSL_ASSERT(ssl);
    SSL_ASSERT(verify_callback);
}
