/*
 * SPDX-FileCopyrightText: 2018-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef _ESP_ASIO_CONFIG_H_
#define _ESP_ASIO_CONFIG_H_

//
// Enabling exceptions only when they are enabled in menuconfig
//
# include <sdkconfig.h>
# ifndef CONFIG_COMPILER_CXX_EXCEPTIONS
#  define ASIO_NO_EXCEPTIONS
# endif   // CONFIG_COMPILER_CXX_EXCEPTIONS

# ifndef CONFIG_COMPILER_RTTI
#  define ASIO_NO_TYPEID
# endif   // CONFIG_COMPILER_RTTI

//
// Supress OpenSSL deprecation warning, when building ASIO
//
#define ESP_OPENSSL_SUPPRESS_LEGACY_WARNING

//
// LWIP compatibility inet and address macros/functions
//
# define LWIP_COMPAT_SOCKET_INET 1
# define LWIP_COMPAT_SOCKET_ADDR 1

//
// Specific ASIO feature flags
//
# define ASIO_DISABLE_SERIAL_PORT
# define ASIO_SEPARATE_COMPILATION
# define ASIO_STANDALONE
# define ASIO_HAS_PTHREADS

# ifdef CONFIG_ASIO_USE_ESP_OPENSSL
#  define ASIO_USE_ESP_OPENSSL
#  define OPENSSL_NO_ENGINE
# elif CONFIG_ASIO_USE_ESP_WOLFSSL
#  define ASIO_USE_WOLFSSL
# endif   // CONFIG_ASIO_USE_ESP_OPENSSL

#endif // _ESP_ASIO_CONFIG_H_
