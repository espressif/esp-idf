/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once
#include "esp_err.h"
#include "soc/soc_caps.h"

#if SOC_KEY_MANAGER_SUPPORTED

#ifdef __cplusplus
extern "C" {
#endif

#include "hal/key_mgr_types.h"
#include "hal/huk_types.h"
#include "esp_attr.h"

#define KEY_MGR_SW_INIT_KEY_SIZE         32
#define KEY_MGR_ASSIST_INFO_SIZE         64
#define KEY_MGR_KEY_RECOVERY_INFO_SIZE   64

#define KEY_MGR_HUK_INFO_SIZE            HUK_INFO_SIZE
#define KEY_MGR_HUK_RISK_ALERT_LEVEL     HUK_RISK_ALERT_LEVEL

/* AES deploy mode */
#define KEY_MGR_K2_INFO_SIZE             64
#define KEY_MGR_K1_ENCRYPTED_SIZE        32
#define KEY_MGR_ECDH0_INFO_SIZE          64
#define KEY_MGR_PLAINTEXT_KEY_SIZE       32

typedef struct {
    esp_key_mgr_key_type_t key_type;
    bool use_pre_generated_huk_info;
    bool use_pre_generated_sw_init_key;
    WORD_ALIGNED_ATTR esp_key_mgr_huk_info_t huk_info;
    WORD_ALIGNED_ATTR uint8_t sw_init_key[KEY_MGR_SW_INIT_KEY_SIZE];
    WORD_ALIGNED_ATTR uint8_t k2_info[KEY_MGR_K2_INFO_SIZE];
    WORD_ALIGNED_ATTR uint8_t k1_encrypted[2][KEY_MGR_K1_ENCRYPTED_SIZE];
} esp_key_mgr_aes_key_config_t;

typedef struct {
    esp_key_mgr_key_type_t key_type;
    bool use_pre_generated_huk_info;
    WORD_ALIGNED_ATTR esp_key_mgr_huk_info_t huk_info;
    WORD_ALIGNED_ATTR uint8_t k1_G[2][KEY_MGR_ECDH0_INFO_SIZE];
} esp_key_mgr_ecdh0_key_config_t;

typedef struct {
    esp_key_mgr_key_type_t key_type;
    bool use_pre_generated_huk_info;
    WORD_ALIGNED_ATTR esp_key_mgr_huk_info_t huk_info;
} esp_key_mgr_random_key_config_t;

typedef struct {
    esp_key_mgr_key_type_t key_type;
    WORD_ALIGNED_ATTR uint8_t k2_G[2][KEY_MGR_ECDH0_INFO_SIZE];
} esp_key_mgr_ecdh0_info_t;

/**
 * @brief Deploy key in AES deployment mode
 * @input
 *      key_config(input)  AES key configuration
 *      key_info(output)   A writable struct of esp_key_mgr_key_info_t type.
 *                          The recovery information for the the deployed key shall be stored here
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_deploy_key_in_aes_mode(const esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info);

/**
 * @brief Deploy key in ECDH0 deployment mode
 * @input
 *      key_config(input)  ECDH0 key configuration
 *      key_info(output)   A writable struct of esp_key_mgr_key_info_t type. The recovery key info for the deployed key shall be stored here
 *      ecdh0_key_info     A writable struct of esp_key_mgr_ecdh0_info_t. The ecdh0 info to recover the actual key shall be stored here.
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_deploy_key_in_ecdh0_mode(const esp_key_mgr_ecdh0_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info, esp_key_mgr_ecdh0_info_t *ecdh0_key_info);

/**
 * @brief Deploy key in Random deployment mode
 * @input
 *      key_config(input)  Random key configuration
 *      key_info(output)   A writable struct of esp_key_mgr_key_info_t type. The recovery key info for the deployed key shall be stored here
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_deploy_key_in_random_mode(const esp_key_mgr_random_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info);

/*
 * @brief Recover and Activate a key from the given key info
 *
 * @note
 *      Once a key of particular type is activated through Key Manager,
 *      then a different key of the same type cannot be activated at the same time.
 *      This key must be deactivated first through a call to esp_key_mgr_deactivate_key()
 *      before activating other key of the same type
 * @input
 *      key_info The key info required to recover the key
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_activate_key(esp_key_mgr_key_recovery_info_t *key_recovery_info);

/*
 * @brief De-activate a key from the given key info
 *      The key which is de-activated can no longer be used for any operation
 * @input
 *      key_info The key info required to recover the key
 * @return
 *      ESP_OK for success
 *      ESP_FAIL/relevant error code for failure
 */
esp_err_t esp_key_mgr_deactivate_key(esp_key_mgr_key_type_t key_type);

#ifdef __cplusplus
}
#endif
#endif
