/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/key_mgr_types.h"
#include "hal/huk_types.h"

#define KEY_MGR_SW_INIT_KEY_SIZE         32
#define KEY_MGR_ASSIST_INFO_SIZE         64
#define KEY_MGR_KEY_RECOVERY_INFO_SIZE   64
#define KEY_MGR_HUK_INFO_SIZE            64
/* AES deploy mode */
#define KEY_MGR_K2_INFO_SIZE             64
#define KEY_MGR_K1_ENCRYPTED_SIZE        32
#define KEY_MGR_ECDH0_INFO_SIZE          64

typedef struct {
    // TODO - Should we use fixed arrays here instead of pointers ?
    esp_key_mgr_key_type_t key_type;
    uint8_t *huk_info;
    size_t huk_info_size;
    esp_key_mgr_key_purpose_t key_purpose;
    bool use_sw_init_key;
    uint8_t *sw_init_key;
    size_t sw_init_key_size;
    uint8_t k2_info[KEY_MGR_K2_INFO_SIZE];
    uint8_t k1_encrypted[KEY_MGR_K1_ENCRYPTED_SIZE];
} esp_key_mgr_aes_key_config_t;

typedef struct {
    // TODO - Should we use fixed arrays here instead of pointers ?
    esp_key_mgr_key_type_t key_type;
    uint8_t *huk_info;
    size_t huk_info_size;
    esp_key_mgr_key_purpose_t key_purpose;
    bool use_sw_init_key;
    uint8_t *sw_init_key;
    size_t sw_init_key_size;
    uint8_t k1_G[KEY_MGR_ECDH0_INFO_SIZE];
    uint8_t k2_G[KEY_MGR_ECDH0_INFO_SIZE];
} esp_key_mgr_ecdh0_key_config_t;

typedef struct {
    uint8_t huk_recovery_info[KEY_MGR_HUK_INFO_SIZE];
    uint8_t key_recovery_info[KEY_MGR_KEY_RECOVERY_INFO_SIZE];
    esp_key_mgr_key_type_t key_type;
    esp_key_mgr_key_purpose_t key_purpose;
} __attribute__((packed)) esp_key_mgr_key_recovery_info_t;

/**
 * @brief Check if the deployed key is valid or not
 * @input
 *      key_config(input)  AES key configuration
 *      key_info(output)   A writable struct of esp_key_mgr_key_info_t type. The recovery key info for the deplyed key shall be stored here
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_deploy_key_in_aes_mode(esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info);

/**
 * @brief Check if the deployed key is valid or not
 * @input
 *      key_config(input)  AES key configuration
 *      key_info(output)   A writable struct of esp_key_mgr_key_info_t type. The recovery key info for the deplyed key shall be stored here
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_deploy_key_in_aes_mode(esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info);

/*
 * @brief Recover a key from the given key info
 *
 * @input
 *      key_info The key info required to recover the key
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/revevant error code for failure
 */
esp_err_t esp_key_mgr_recover_key(esp_key_mgr_key_recovery_info_t *key_recovery_info);
#ifdef __cplusplus
}
#endif
