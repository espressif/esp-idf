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
#ifndef _SHA1_ALT_H_
#define _SHA1_ALT_H_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_SHA1_ALT)

#include "hwcrypto/sha.h"

typedef esp_sha_context mbedtls_sha1_context;

#define mbedtls_sha1_init       esp_sha1_init
#define mbedtls_sha1_starts     esp_sha1_start
#define mbedtls_sha1_clone      esp_sha1_clone
#define mbedtls_sha1_update     esp_sha1_update
#define mbedtls_sha1_finish     esp_sha1_finish
#define mbedtls_sha1_free       esp_sha1_free
#define mbedtls_sha1_process(...)

#endif

#ifdef __cplusplus
}
#endif

#endif

