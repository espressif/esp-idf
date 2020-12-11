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

/* padlock.c and aesni.c rely on these values! */
#define ESP_AES_ENCRYPT     1
#define ESP_AES_DECRYPT     0


/* DMA AES working modes*/
typedef enum {
    ESP_AES_BLOCK_MODE_ECB = 0,
    ESP_AES_BLOCK_MODE_CBC,
    ESP_AES_BLOCK_MODE_OFB,
    ESP_AES_BLOCK_MODE_CTR,
    ESP_AES_BLOCK_MODE_CFB8,
    ESP_AES_BLOCK_MODE_CFB128,
    ESP_AES_BLOCK_MODE_GCM,
    ESP_AES_BLOCK_MODE_MAX,
} esp_aes_mode_t;

/* Number of bytes in an AES block */
#define AES_BLOCK_BYTES     (16)
/* Number of words in an AES block */
#define AES_BLOCK_WORDS     (4)
/* Number of bytes in an IV */
#define IV_BYTES            (16)
/* Number of words in an IV */
#define IV_WORDS            (4)
/* Number of bytes in a GCM tag block */
#define TAG_BYTES           (16)
/* Number of words in a GCM tag block */
#define TAG_WORDS           (4)

#define AES_128_KEY_BYTES   (128/8)
#define AES_192_KEY_BYTES   (192/8)
#define AES_256_KEY_BYTES   (256/8)
