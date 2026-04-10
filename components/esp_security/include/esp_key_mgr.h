/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
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

#define KEY_MGR_HUK_INFO_SIZE            HUK_INFO_LEN
#define KEY_MGR_HUK_RISK_ALERT_LEVEL     HUK_RISK_ALERT_LEVEL

#define KEY_MGR_KEY_INFO_SIZE            KEY_INFO_LEN

/* AES deploy mode */
#define KEY_MGR_K2_INFO_SIZE             64
#define KEY_MGR_K1_ENCRYPTED_SIZE        32
#define KEY_MGR_ECDH0_INFO_SIZE          64
#define KEY_MGR_PLAINTEXT_KEY_SIZE       32

/**
 * @brief Configuration for deploying a key in AES mode
 */
typedef struct {
    esp_key_mgr_key_type_t key_type;                                    /*!< Type of key to deploy */
    esp_key_mgr_key_len_t key_len;                                      /*!< Length of the key */
    bool use_pre_generated_huk_info;                                    /*!< Use pre-generated HUK info if true */
    bool use_pre_generated_sw_init_key;                                 /*!< Use pre-generated software init key if true */
    WORD_ALIGNED_ATTR esp_key_mgr_huk_info_t huk_info;                  /*!< HUK recovery info */
    WORD_ALIGNED_ATTR uint8_t sw_init_key[KEY_MGR_SW_INIT_KEY_SIZE];    /*!< Software init key */
    WORD_ALIGNED_ATTR uint8_t k2_info[KEY_MGR_K2_INFO_SIZE];            /*!< K2 info for AES deployment */
    WORD_ALIGNED_ATTR uint8_t k1_encrypted[2][KEY_MGR_K1_ENCRYPTED_SIZE]; /*!< Encrypted K1 key data */
} esp_key_mgr_aes_key_config_t;

/**
 * @brief Configuration for deploying a key in ECDH0 mode
 */
typedef struct {
    esp_key_mgr_key_type_t key_type;                                    /*!< Type of key to deploy */
    esp_key_mgr_key_len_t key_len;                                      /*!< Length of the key */
    bool use_pre_generated_huk_info;                                    /*!< Use pre-generated HUK info if true */
    WORD_ALIGNED_ATTR esp_key_mgr_huk_info_t huk_info;                  /*!< HUK recovery info */
    WORD_ALIGNED_ATTR uint8_t k1_G[2][KEY_MGR_ECDH0_INFO_SIZE];        /*!< K1*G points for ECDH0 deployment */
} esp_key_mgr_ecdh0_key_config_t;

/**
 * @brief Configuration for deploying a key in Random mode
 */
typedef struct {
    esp_key_mgr_key_type_t key_type;                                    /*!< Type of key to deploy */
    esp_key_mgr_key_len_t key_len;                                      /*!< Length of the key */
    bool use_pre_generated_huk_info;                                    /*!< Use pre-generated HUK info if true */
    WORD_ALIGNED_ATTR esp_key_mgr_huk_info_t huk_info;                  /*!< HUK recovery info */
} esp_key_mgr_random_key_config_t;

/**
 * @brief ECDH0 key info generated during ECDH0 deployment
 */
typedef struct {
    esp_key_mgr_key_type_t key_type;                                    /*!< Type of key */
    esp_key_mgr_key_len_t key_len;                                      /*!< Length of the key */
    WORD_ALIGNED_ATTR uint8_t k2_G[2][KEY_MGR_ECDH0_INFO_SIZE];        /*!< K2*G points from ECDH0 deployment */
} esp_key_mgr_ecdh0_info_t;

/**
 * @brief Wait for the Key Manager to reach the given state
 *
 * @param state The state to wait for
 */
void key_mgr_wait_for_state(esp_key_mgr_state_t state);

/**
 * @brief Deploy key in AES deployment mode
 *
 * @param[in]  key_config  AES key configuration
 * @param[out] key_info    A writable struct of esp_key_mgr_key_recovery_info_t type.
 *                         The recovery information for the deployed key shall be stored here.
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL or relevant error code on failure
 */
esp_err_t esp_key_mgr_deploy_key_in_aes_mode(const esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info);

/**
 * @brief Deploy key in ECDH0 deployment mode
 *
 * @param[in]  key_config      ECDH0 key configuration
 * @param[out] key_info        A writable struct of esp_key_mgr_key_recovery_info_t type.
 *                             The recovery key info for the deployed key shall be stored here.
 * @param[out] ecdh0_key_info  A writable struct of esp_key_mgr_ecdh0_info_t type.
 *                             The ECDH0 info to recover the actual key shall be stored here.
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL or relevant error code on failure
 */
esp_err_t esp_key_mgr_deploy_key_in_ecdh0_mode(const esp_key_mgr_ecdh0_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info, esp_key_mgr_ecdh0_info_t *ecdh0_key_info);

/**
 * @brief Deploy key in Random deployment mode
 *
 * @param[in]  key_config  Random key configuration
 * @param[out] key_info    A writable struct of esp_key_mgr_key_recovery_info_t type.
 *                         The recovery key info for the deployed key shall be stored here.
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL or relevant error code on failure
 */
esp_err_t esp_key_mgr_deploy_key_in_random_mode(const esp_key_mgr_random_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info);

/**
 * @brief Recover and Activate a key from the given key info
 *
 * @note Once a key of particular type is activated through Key Manager,
 *       then a different key of the same type cannot be activated at the same time.
 *       This key must be deactivated first through a call to esp_key_mgr_deactivate_key()
 *       before activating other key of the same type.
 *
 * @param[in] key_recovery_info The key recovery info required to recover the key
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL or relevant error code on failure
 */
esp_err_t esp_key_mgr_activate_key(esp_key_mgr_key_recovery_info_t *key_recovery_info);

/**
 * @brief De-activate a key of the given type
 *
 * The key which is de-activated can no longer be used for any operation.
 *
 * @param[in] key_type The type of the key to deactivate
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL or relevant error code on failure
 */
esp_err_t esp_key_mgr_deactivate_key(esp_key_mgr_key_type_t key_type);

#ifdef __cplusplus
}
#endif
#endif
