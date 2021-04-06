/**
 * \file md5_alt.h
 *
 * \brief MD5 block cipher
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *
 */
#ifndef MD5_ALT_H
#define MD5_ALT_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(MBEDTLS_MD5_ALT)
#include "md/esp_md.h"

#define mbedtls_md5_init                        esp_md5_init
#define mbedtls_md5_update                      esp_md5_update
#define mbedtls_md5_finish                      esp_md5_finish
#define mbedtls_md5_starts_ret                  esp_md5_init_ret
#define mbedtls_md5_update_ret                  esp_md5_update_ret
#define mbedtls_md5_finish_ret                  esp_md5_finish_ret

#define mbedtls_md5_free                        esp_md5_free
#define mbedtls_md5_clone                       esp_md5_clone
#define mbedtls_internal_md5_process            esp_md5_process

#endif /* MBEDTLS_MD5_ALT */

#ifdef __cplusplus
}
#endif

#endif
