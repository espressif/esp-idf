/**
 * \brief AES block cipher, ESP-IDF hardware accelerated version
 * Based on mbedTLS FIPS-197 compliant version.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE Ltd
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
 *  Internal API
 */

#pragma once


#include "aes/esp_aes.h"
#include "aes/esp_aes_gcm.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool valid_key_length(const esp_aes_context *ctx);


/**
 * @brief           Run a AES-GCM conversion using DMA
 *
 * @param ctx       Aes context
 * @param input     Pointer to input data
 * @param output    Pointer to output data
 * @param len       Length of the input data
 * @param aad_desc  GCM additional data DMA descriptor
 * @param aad_len   GCM additional data length
 * @return int      -1 on error
 */
int esp_aes_process_dma_gcm(esp_aes_context *ctx, const unsigned char *input, unsigned char *output, size_t len, lldesc_t *aad_desc, size_t aad_len);


#ifdef __cplusplus
}
#endif
