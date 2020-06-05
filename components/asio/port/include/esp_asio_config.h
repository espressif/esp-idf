// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
