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

#ifndef _SSL_PORT_H_
#define _SSL_PORT_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "platform/ssl_opt.h"

#ifdef ESP32_IDF_PLATFORM

#include "esp_types.h"
#include "esp_log.h"

void *ssl_mem_zalloc(size_t size);
void *ssl_mem_malloc(size_t size);
void ssl_mem_free(void *p);

void* ssl_memcpy(void *to, const void *from, size_t size);
size_t ssl_strlen(const char *src);

void ssl_speed_up_enter(void);
void ssl_speed_up_exit(void);

#define SSL_PRINT_LOG(fmt, ...) ESP_LOGD("openssl", fmt, ##__VA_ARGS__)
#define SSL_ERROR_LOG(fmt, ...) ESP_LOGE("openssl", fmt, ##__VA_ARGS__)
#define SSL_LOCAL_LOG(fmt, ...) ESP_LOGD("openssl", fmt, ##__VA_ARGS__)

#elif defined(SSL_PLATFORM_USER_INCLUDE)

SSL_PLATFORM_USER_INCLUDE

#endif

#endif
