/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define KEY_MGR_ASSIST_INFO_LEN    64

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief State of Key Manager: idle, load, gain or busy.
 */
typedef enum {
    ESP_KEY_MGR_STATE_IDLE = 0, /* Key Manager is idle */
    ESP_KEY_MGR_STATE_LOAD,     /* Key Manager is read to recieve input */
    ESP_KEY_MGR_STATE_GAIN,     /* Key Manager is ready to provide output */
    ESP_KEY_MGR_STATE_BUSY      /* Key Manager is busy */
} esp_key_mgr_state_t;

/**
 * @brief Length of the XTS AES key
 */
typedef enum {
    ESP_KEY_MGR_XTS_AES_LEN_512 = 0, /* xts-aes key is 512 bit, please note that xts-aes algorithm is XTS_AES_256  */
    ESP_KEY_MGR_XTS_AES_LEN_256,     /* xts-aes key is 256 bit, please note that xts-aes algorithm is XTS_AES_128*/
} esp_key_mgr_xts_aes_key_len_t;

/**
 * @brief Type of the key: ECDSA, XTS
 */
typedef enum {
    ESP_KEY_MGR_ECDSA_KEY = 1, /* ECDSA key */
    ESP_KEY_MGR_XTS_KEY,       /* XTS AES key */
} esp_key_mgr_key_type_t;

/*
 * @brief Key Manager key usage type
 */
typedef enum {
    ESP_KEY_MGR_USE_OWN_KEY = 0, /* Use key from the key manager */
    ESP_KEY_MGR_USE_EFUSE_KEY, /* Use key from the eFuse */
} esp_key_mgr_key_usage_t;

/**
 * @brief Key Purpose to be set for a particular key in the Key Manager
 */
typedef enum {
    ESP_KEY_MGR_KEY_PURPOSE_ECDSA = 1,
    ESP_KEY_MGR_KEY_PURPOSE_XTS_256_1, /* First half of the XTS AES 256 bit key */
    ESP_KEY_MGR_KEY_PURPOSE_XTS_256_2, /* Second half of the XTS AES 256 bit key */
    ESP_KEY_MGR_KEY_PURPOSE_XTS_128    /* XTS AES 128 bit key */
} esp_key_mgr_key_purpose_t;

/**
 * @brief Key Manager Generator mode
 */
typedef enum {
    ESP_KEY_MGR_KEYGEN_MODE_RANDOM = 0,
    ESP_KEY_MGR_KEYGEN_MODE_AES,
    ESP_KEY_MGR_KEYGEN_MODE_ECDH0,
    ESP_KEY_MGR_KEYGEN_MODE_ECDH1,
    ESP_KEY_MGR_KEYGEN_MODE_RECOVER,
    ESP_KEY_MGR_KEYGEN_MODE_EXPORT,
} esp_key_mgr_key_generator_mode_t;

/**
 * @brief
 * Key Manager interrupt types
 */
typedef enum {
    ESP_KEY_MGR_INT_PREP_DONE = 0x01,
    ESP_KEY_MGR_INT_PROC_DONE,
    ESP_KEY_MGR_INT_POST_DONE,
} esp_key_mgr_interrupt_type_t;

#ifdef __cplusplus
}
#endif

#endif
