//
// SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
//
// SPDX-License-Identifier: BSL-1.0
//

#pragma once

//
// Supply OpenSSL macros and flags for asio-ssl header files
//
#define OPENSSL_VERSION_NUMBER 0x10100001L

#define SSL_R_SHORT_READ 219
#define SSL_OP_ALL 0
#define SSL_OP_SINGLE_DH_USE 0
#define SSL_OP_NO_COMPRESSION 0

#define SSL_OP_NO_SSLv2					0x01000000L
#define SSL_OP_NO_SSLv3					0x02000000L
#define SSL_OP_NO_TLSv1					0x04000000L

#define SSL_VERIFY_NONE                 0x00
#define SSL_VERIFY_PEER                 0x01
#define SSL_VERIFY_FAIL_IF_NO_PEER_CERT 0x02
#define SSL_VERIFY_CLIENT_ONCE          0x04

//
// Implement asio-ssl layer with these three classes in asio::ssl::mbedtls
//
namespace asio {
namespace ssl {
namespace mbedtls {

class engine;
class bio;
class shared_ctx;
} } } // namespace asio::ssl::mbedtls

//
// Supply OpenSSL types as aliases to mbedtls classes
//
using X509_STORE_CTX=void;
using BIO=asio::ssl::mbedtls::bio;
using SSL_CTX=asio::ssl::mbedtls::shared_ctx;
using SSL=asio::ssl::mbedtls::engine;
