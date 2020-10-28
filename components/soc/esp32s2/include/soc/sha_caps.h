// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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

/* Max amount of bytes in a single DMA operation is 4095,
   for SHA this means that the biggest safe amount of bytes is
   31 blocks of 128 bytes = 3968
*/
#define SOC_SHA_DMA_MAX_BUFFER_SIZE     (3968)
#define SOC_SHA_SUPPORT_DMA             (1)

/* ESP32 style SHA engine, where multiple states can be stored in parallel */
#define SOC_SHA_SUPPORT_PARALLEL_ENG    (0)

/* The SHA engine is able to resume hashing from a user */
#define SOC_SHA_SUPPORT_RESUME          (1)

/* Has "crypto DMA", which is shared with AES */
#define SOC_SHA_CRYPTO_DMA              (1)

/* Has a centralized DMA, which is shared with all peripherals */
#define SOC_SHA_GENERAL_DMA             (0)

/* Supported HW algorithms */
#define SOC_SHA_SUPPORT_SHA1            (1)
#define SOC_SHA_SUPPORT_SHA224          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA384          (1)
#define SOC_SHA_SUPPORT_SHA256          (1)
#define SOC_SHA_SUPPORT_SHA512          (1)
#define SOC_SHA_SUPPORT_SHA512_224      (1)
#define SOC_SHA_SUPPORT_SHA512_256      (1)
#define SOC_SHA_SUPPORT_SHA512_T        (1)


#ifdef __cplusplus
}
#endif
