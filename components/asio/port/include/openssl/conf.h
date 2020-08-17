// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _ESP_ASIO_OPENSSL_CONF_H
#define _ESP_ASIO_OPENSSL_CONF_H
#include "esp_asio_config.h"
#include "openssl/esp_asio_openssl_stubs.h"

#if defined(ASIO_USE_WOLFSSL)
// SSLv3 Methods not present in current wolfSSL library
#define OPENSSL_NO_SSL3
#include_next "openssl/conf.h"
#endif // ASIO_USE_WOLFSSL

#endif // _ESP_ASIO_OPENSSL_CONF_H
