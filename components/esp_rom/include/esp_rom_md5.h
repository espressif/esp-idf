// Copyright 2010-2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * The MD5 functions calculate a 128-bit cryptographic digest for any number of input bytes.
 */

/**
 * @brief Type defined for MD5 context
 *
 */
typedef struct MD5Context {
    uint32_t buf[4];
    uint32_t bits[2];
    uint8_t in[64];
} md5_context_t;

/**
 * @brief Initialize the MD5 context
 *
 * @param context Context object allocated by user
 */
void esp_rom_md5_init(md5_context_t *context);

/**
 * @brief Running MD5 algorithm over input data
 *
 * @param context MD5 context which has been initialized by `MD5Init`
 * @param buf Input buffer
 * @param len Buffer length
 */
void esp_rom_md5_update(md5_context_t *context, const uint8_t *buf, uint32_t len);

/**
 * @brief Extract the MD5 result, and erase the context
 *
 * @param digest Where to store the 128-bit digest value
 * @param context MD5 context
 */
void esp_rom_md5_final(uint8_t digest[16], md5_context_t *context);

#ifdef __cplusplus
}
#endif
