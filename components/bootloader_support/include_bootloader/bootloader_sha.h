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
   Use mbedTLS APIs or include hwcrypto/sha.h to calculate SHA256 in IDF apps.
*/

#include <stdint.h>
#include <stdlib.h>

typedef void *bootloader_sha256_handle_t;

bootloader_sha256_handle_t bootloader_sha256_start();

void bootloader_sha256_data(bootloader_sha256_handle_t handle, const void *data, size_t data_len);

void bootloader_sha256_finish(bootloader_sha256_handle_t handle, uint8_t *digest);
