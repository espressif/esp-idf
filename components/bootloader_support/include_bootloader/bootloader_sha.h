// Copyright 2017 Espressif Systems (Shanghai) PTE LTD
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
#pragma once

/* Provide a SHA256 API for bootloader_support code,
   that can be used from bootloader or app code.

   This header is available to source code in the bootloader & bootloader_support components only.
   Use mbedTLS APIs or include esp32/sha.h to calculate SHA256 in IDF apps.
*/

#include <stdint.h>
#include <stdlib.h>
#include "esp_err.h"

typedef void *bootloader_sha256_handle_t;

bootloader_sha256_handle_t bootloader_sha256_start();

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len);

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest);

/**
 * @brief Converts an array to a printable string.
 *
 * This function is useful for printing SHA-256 digest.
 * \code{c}
 * // Example of using. image_hash will be printed
 * #define HASH_LEN 32 // SHA-256 digest length
 * ...
 * char hash_print[HASH_LEN * 2 + 1];
 * hash_print[HASH_LEN * 2] = 0;
 * bootloader_sha256_hex_to_str(hash_print, image_hash, HASH_LEN);
 * ESP_LOGI(TAG, %s", hash_print);
 * \endcode

 * @param[out] out_str       Output string
 * @param[in]  in_array_hex  Pointer to input array
 * @param[in]  len           Length of input array
 *
 * @return   ESP_OK: Successful
 *           ESP_ERR_INVALID_ARG: Error in the passed arguments
 */
esp_err_t bootloader_sha256_hex_to_str(char *out_str, const uint8_t *in_array_hex, size_t len);
