// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This API should be used by all components which use the SHA, AES, HMAC and DS crypto hardware on the ESP32S3.
 * Not all of them can be used in parallel because they use the same underlying module.
 * E.g., HMAC uses SHA or DS uses HMAC and AES. See the ESP32S3 Technical Reference Manual for more details.
 *
 * Other unrelated components must not use it.
 */

/**
 * @brief Acquire lock for the SHA and AES cryptography peripheral.
 *
 */
void esp_crypto_sha_aes_lock_acquire(void);

/**
 * @brief Release lock for the SHA and AES cryptography peripheral.
 *
 */
void esp_crypto_sha_aes_lock_release(void);

/**
 * Acquire lock for the MPI/RSA cryptography peripheral
 */
void esp_crypto_mpi_lock_acquire(void);

/**
 * Release lock for the MPI/RSA cryptography peripheral
 */
void esp_crypto_mpi_lock_release(void);

#ifdef __cplusplus
}
#endif
