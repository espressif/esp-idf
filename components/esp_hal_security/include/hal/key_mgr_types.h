/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_attr.h"

#define KEY_MGR_ASSIST_INFO_LEN    64

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief State of Key Manager: idle, load, gain or busy.
 */
typedef enum {
    ESP_KEY_MGR_STATE_IDLE = 0,     /* Key Manager is idle */
    ESP_KEY_MGR_STATE_LOAD = 1,     /* Key Manager is ready to receive input */
    ESP_KEY_MGR_STATE_GAIN = 2,     /* Key Manager is ready to provide output */
    ESP_KEY_MGR_STATE_BUSY = 3,     /* Key Manager is busy */
} esp_key_mgr_state_t;

/**
 * @brief Length of the deployed key (XTS-AES, ECDSA)
 */
typedef enum {
    ESP_KEY_MGR_ECDSA_LEN_192 = 0,      /* ecdsa key is 192 bit */
    ESP_KEY_MGR_ECDSA_LEN_256,          /* ecdsa key is 256 bit */
    ESP_KEY_MGR_ECDSA_LEN_384,          /* ecdsa key is 384 bit */
    ESP_KEY_MGR_XTS_AES_LEN_128,        /* xts-aes key is 128 bit */
    ESP_KEY_MGR_XTS_AES_LEN_256,        /* xts-aes key is 512 bit, please note that xts-aes algorithm is XTS_AES_256  */
} esp_key_mgr_key_len_t;

/**
 * @brief Type of the key
 */
typedef enum {
    ESP_KEY_MGR_ECDSA_KEY = 0,      /* ECDSA key */
    ESP_KEY_MGR_FLASH_XTS_AES_KEY,  /* XTS-AES key */
    ESP_KEY_MGR_HMAC_KEY,           /* HMAC key */
    ESP_KEY_MGR_DS_KEY,             /* Digital signature key */
    ESP_KEY_MGR_PSRAM_XTS_AES_KEY,  /* PSRAM XTS-AES key */
} esp_key_mgr_key_type_t;

/**
 * @brief Key Manager key usage type
 */
typedef enum {
    ESP_KEY_MGR_USE_OWN_KEY = 0, /* Use key from the key manager */
    ESP_KEY_MGR_USE_EFUSE_KEY,   /* Use key from the eFuse */
    ESP_KEY_MGR_USAGE_INVALID,
} esp_key_mgr_key_usage_t;

/**
 * @brief Key Purpose to be set for a particular key in the Key Manager
 */
typedef enum {
    ESP_KEY_MGR_KEY_PURPOSE_INVALID = 0,
    ESP_KEY_MGR_KEY_PURPOSE_ECDSA_192 = 1,      /* ECDSA 192-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_ECDSA_256 = 2,      /* ECDSA 256-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_1 = 3,    /* First half of flash 256-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1 = ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_1,
    ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_2 = 4,    /* Second half of flash 256-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2 = ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_2,
    ESP_KEY_MGR_KEY_PURPOSE_FLASH_128 = 5,      /* Flash 128-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_128 = ESP_KEY_MGR_KEY_PURPOSE_FLASH_128,
    ESP_KEY_MGR_KEY_PURPOSE_HMAC = 6,           /* HMAC key */
    ESP_KEY_MGR_KEY_PURPOSE_DS = 7,             /* Digital signature key */
    ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_1 = 8,    /* First half of PSRAM 256-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_2 = 9,    /* Second half of PSRAM 256-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_PSRAM_128 = 10,     /* PSRAM 128-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_ECDSA_384_L = 11,   /* Lower half of ECDSA 384-bit key */
    ESP_KEY_MGR_KEY_PURPOSE_ECDSA_384_H = 12,   /* Higher half of ECDSA 384-bit key */
} esp_key_mgr_key_purpose_t;

/**
 * @brief Key Manager Generator mode
 */
typedef enum {
    ESP_KEY_MGR_KEYGEN_MODE_RANDOM = 0,
    ESP_KEY_MGR_KEYGEN_MODE_AES = 1,
    ESP_KEY_MGR_KEYGEN_MODE_ECDH0 = 2,
    ESP_KEY_MGR_KEYGEN_MODE_ECDH1 = 3,
    ESP_KEY_MGR_KEYGEN_MODE_RECOVER = 4,
    ESP_KEY_MGR_KEYGEN_MODE_EXPORT = 5,
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

/**
 * @brief Force use key manager key type
 * @note This is used to force the key manager to use a specific key type.
 */
typedef enum {
    ESP_KEY_MGR_FORCE_USE_KM_ECDSA_KEY = 0,
    ESP_KEY_MGR_FORCE_USE_KM_XTS_AES_KEY = 1,
    ESP_KEY_MGR_FORCE_USE_KM_HMAC_KEY = 2,
    ESP_KEY_MGR_FORCE_USE_KM_DS_KEY = 3,
} esp_key_mgr_force_use_km_key_t;

/**
 * @brief HUK info structure, stores HUK recovery information
 */
typedef struct PACKED_ATTR {
#define HUK_INFO_LEN 660
    uint8_t info[HUK_INFO_LEN];    /*!< HUK info data */
    uint32_t crc;                   /*!< CRC of the HUK info */
} esp_key_mgr_huk_info_t;

/**
 * @brief Key info structure, stores key recovery information (512 bits)
 */
typedef struct PACKED_ATTR {
#define KEY_INFO_LEN 64
    uint8_t info[KEY_INFO_LEN];    /*!< Key info data */
    uint32_t crc;                  /*!< CRC of the key info */
} esp_key_mgr_key_info_t;

/**
 * @brief Key recovery info structure containing all data needed to recover a deployed key
 */
typedef struct WORD_ALIGNED_ATTR PACKED_ATTR {
#define KEY_HUK_SECTOR_MAGIC 0xDEA5CE5A
    uint32_t magic;                         /*!< Magic number for validation */
    uint32_t version;                       /*!< Version for backward compatibility */
    uint8_t key_type;                       /*!< Type of the deployed key */
    uint8_t key_len;                        /*!< Length of the deployed key */
    uint8_t key_deployment_mode;            /*!< Deployment mode used for the key */
    uint8_t reserved[13];                   /*!< Reserved for future use */
    esp_key_mgr_huk_info_t huk_info;        /*!< HUK recovery info */
    esp_key_mgr_key_info_t key_info[2];     /*!< Key info (up to 2 entries for XTS-AES-256) */
} esp_key_mgr_key_recovery_info_t;

#ifdef __cplusplus
}
#endif
