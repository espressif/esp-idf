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
#ifndef _SHA512_ALT_H_
#define _SHA512_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA512_ALT)
#include "hwcrypto/sha.h"

typedef esp_sha_context  mbedtls_sha512_context;

#define mbedtls_sha512_init     esp_sha512_init
#define mbedtls_sha512_clone    esp_sha512_clone
#define mbedtls_sha512_starts   esp_sha512_start
#define mbedtls_sha512_update   esp_sha512_update
#define mbedtls_sha512_finish   esp_sha512_finish
#define mbedtls_sha512_free     esp_sha512_free
#define mbedtls_sha512_process(...)

#endif

#ifdef __cplusplus
}
#endif

#endif
