/*
 * SPDX-FileCopyrightText: 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
