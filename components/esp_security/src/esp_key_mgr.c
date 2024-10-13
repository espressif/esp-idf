/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The Hardware Support layer for Key manager
#include <string.h>
#include <sys/lock.h>
#include "assert.h"
#include "esp_key_mgr.h"
#include "esp_private/periph_ctrl.h"
#include "esp_crypto_lock.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_random.h"
#include "esp_heap_caps.h"
#include "esp_rom_crc.h"
#include "esp_efuse.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "hal/key_mgr_types.h"
#include "hal/key_mgr_hal.h"
#include "hal/key_mgr_ll.h"
#include "hal/huk_types.h"
#include "hal/huk_hal.h"
#include "rom/key_mgr.h"

#if CONFIG_LOG_DEFAULT_LEVEL_VERBOSE
#include "soc/huk_reg.h"
#include "soc/keymng_reg.h"
#endif

static const char *TAG = "esp_key_mgr";

#define KEY_MANAGER_RCC_ATOMIC() PERIPH_RCC_ATOMIC()

static _lock_t s_key_mgr_ecdsa_key_lock;
static _lock_t s_key_mgr_xts_aes_key_lock;

ESP_STATIC_ASSERT(sizeof(esp_key_mgr_key_recovery_info_t) == sizeof(struct huk_key_block), "Size of esp_key_mgr_key_recovery_info_t should match huk_key_block (from ROM)");

ESP_STATIC_ASSERT(sizeof(esp_key_mgr_key_info_t) == sizeof(struct key_info), "Size of esp_key_mgr_key_info_t should match key_info (from ROM)");

ESP_STATIC_ASSERT(sizeof(esp_key_mgr_huk_info_t) == sizeof(struct huk_info), "Size of esp_key_mgr_huk_info_t should match huk_info (from ROM)");

static void esp_key_mgr_acquire_key_lock(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        _lock_acquire(&s_key_mgr_ecdsa_key_lock);
        break;
    case ESP_KEY_MGR_XTS_AES_128_KEY:
    case ESP_KEY_MGR_XTS_AES_256_KEY:
        _lock_acquire(&s_key_mgr_xts_aes_key_lock);
        break;
    }
    ESP_LOGV(TAG, "Key lock acquired for key type %d", key_type);
}

static void esp_key_mgr_release_key_lock(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        _lock_release(&s_key_mgr_ecdsa_key_lock);
        break;
    case ESP_KEY_MGR_XTS_AES_128_KEY:
    case ESP_KEY_MGR_XTS_AES_256_KEY:
        _lock_release(&s_key_mgr_xts_aes_key_lock);
        break;
    }
    ESP_LOGV(TAG, "Key lock released for key type %d", key_type);
}

static void esp_key_mgr_acquire_hardware(bool deployment_mode)
{
    if (deployment_mode) {
        // We only need explicit locks in the deployment mode
        esp_crypto_ecc_lock_acquire();
        esp_crypto_sha_aes_lock_acquire();
        esp_crypto_key_manager_lock_acquire();
    }
    // Reset the Key Manager Clock
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(true);
        key_mgr_ll_enable_peripheral_clock(true);
        key_mgr_ll_reset_register();
    }
}

static void esp_key_mgr_release_hardware(bool deployment_mode)
{
    if (deployment_mode) {
        esp_crypto_ecc_lock_release();
        esp_crypto_sha_aes_lock_release();
        esp_crypto_key_manager_lock_release();
    }

    // Reset the Key Manager Clock
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_peripheral_clock(false);
        key_mgr_ll_enable_bus_clock(false);
        key_mgr_ll_reset_register();
    }
}

static void key_mgr_wait_for_state(esp_key_mgr_state_t state)
{
    while (key_mgr_hal_get_state() != state) {
        ;
    }
}

typedef struct aes_deploy {
    esp_key_mgr_key_purpose_t key_purpose;
    const uint8_t *k1_encrypted;
    const esp_key_mgr_aes_key_config_t *key_config;
    esp_key_mgr_key_recovery_info_t *key_info;
    bool huk_deployed;
} aes_deploy_config_t;

static void check_huk_risk_level(void)
{
    uint8_t huk_risk_level = huk_hal_get_risk_level();
    if (huk_risk_level > KEY_MGR_HUK_RISK_ALERT_LEVEL) {
        ESP_LOGE(TAG, "HUK Risk level too high (level %d)\n"
                 "It is recommended to immediately regenerate HUK in order"
                 "to avoid permanently losing the deployed keys", huk_risk_level);
    } else {
        ESP_LOGI(TAG, "HUK Risk level - %" PRId8 " within acceptable limit (%" PRIu32 ")", huk_risk_level, (uint32_t)KEY_MGR_HUK_RISK_ALERT_LEVEL);
    }

}

static bool check_huk_info_validity(const esp_key_mgr_huk_info_t *huk_info)
{
    uint32_t calc_crc = esp_rom_crc32_le(0, huk_info->info, KEY_MGR_HUK_INFO_SIZE);
    if (calc_crc != huk_info->crc) {
        ESP_LOGE(TAG, "Calculated CRC for HUK %" PRIx32 " does not match with %" PRIx32, calc_crc, huk_info->crc);
        return false;
    }
    return true;
}

static bool check_key_info_validity(const esp_key_mgr_key_info_t *key_info)
{
    uint32_t calc_crc = esp_rom_crc32_le(0, key_info->info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    if (calc_crc != key_info->crc) {
        ESP_LOGE(TAG, "Calculated CRC for Key info %" PRIx32 " does not match with %" PRIx32, calc_crc, key_info->crc);
        return false;
    }
    return true;
}

typedef struct {
    bool use_pre_generated_huk_info;
    const esp_key_mgr_huk_info_t *pre_generated_huk_info;
    esp_key_mgr_huk_info_t *huk_recovery_info;
} huk_deploy_config_t;

static esp_err_t deploy_huk(huk_deploy_config_t *config)
{
    esp_err_t esp_ret = ESP_FAIL;
    uint8_t *huk_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_HUK_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!huk_recovery_info) {
        return ESP_ERR_NO_MEM;
    }
    if (config->use_pre_generated_huk_info) {
        // If HUK info is provided then recover the HUK from given info
        check_huk_risk_level();
        if (!check_huk_info_validity(config->pre_generated_huk_info)) {
            ESP_LOGE(TAG, "HUK info is not valid");
            heap_caps_free(huk_recovery_info);
            return ESP_ERR_INVALID_ARG;
        }
        memcpy(huk_recovery_info, config->pre_generated_huk_info->info, KEY_MGR_HUK_INFO_SIZE);
        ESP_LOGI(TAG, "Recovering HUK from given HUK recovery info");
        esp_ret = huk_hal_configure(ESP_HUK_MODE_RECOVERY, huk_recovery_info);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to recover HUK");
            heap_caps_free(huk_recovery_info);
            return esp_ret;
        }
        // Copy the pre generated huk info in the output key recovery info
        memcpy(config->huk_recovery_info->info, huk_recovery_info, KEY_MGR_HUK_INFO_SIZE);
        config->huk_recovery_info->crc = config->pre_generated_huk_info->crc;
    } else {
        // Generate new HUK and corresponding HUK info
        ESP_LOGI(TAG, "Generating new HUK");
        esp_ret = huk_hal_configure(ESP_HUK_MODE_GENERATION, huk_recovery_info);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to generate HUK");
            heap_caps_free(huk_recovery_info);
            return esp_ret;
        }
        memcpy(config->huk_recovery_info->info, huk_recovery_info, KEY_MGR_HUK_INFO_SIZE);
        config->huk_recovery_info->crc = esp_rom_crc32_le(0,  huk_recovery_info, KEY_MGR_HUK_INFO_SIZE);
    }

    if (!key_mgr_hal_is_huk_valid()) {
        ESP_LOGE(TAG, "HUK is invalid");
        heap_caps_free(huk_recovery_info);
        return ESP_FAIL;
    }

    ESP_LOG_BUFFER_HEX_LEVEL("HUK INFO", huk_recovery_info, KEY_MGR_HUK_INFO_SIZE, ESP_LOG_DEBUG);
    // Free the local buffer for huk recovery info
    heap_caps_free(huk_recovery_info);
    return ESP_OK;
}

static esp_err_t key_mgr_deploy_key_aes_mode(aes_deploy_config_t *config)
{
    esp_err_t esp_ret = ESP_FAIL;
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    if ((!key_mgr_hal_is_huk_valid()) || (!config->huk_deployed)) {
        // For purpose ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2 this part shall be already executed
        huk_deploy_config_t huk_deploy_config = {};
        huk_deploy_config.use_pre_generated_huk_info = config->key_config->use_pre_generated_huk_info;
        huk_deploy_config.pre_generated_huk_info = &config->key_config->huk_info;
        huk_deploy_config.huk_recovery_info = &config->key_info->huk_info;
        esp_ret = deploy_huk(&huk_deploy_config);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
        ESP_LOGI(TAG, "HUK deployed successfully");
    }

    // STEP 1: Init Step
    // Set mode
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_AES);

    uint8_t *key_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_KEY_RECOVERY_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!key_recovery_info) {
        return ESP_ERR_NO_MEM;
    }
    // Set key purpose (XTS/ECDSA)
    ESP_LOGD(TAG, "Key purpose = %d", config->key_purpose);
    key_mgr_hal_set_key_purpose(config->key_purpose);

    // Set key length for XTS-AES key
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) config->key_config->key_type;

    if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_256);
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_512);
    }

    if (config->key_config->use_pre_generated_sw_init_key) {
        key_mgr_hal_use_sw_init_key();
    } else {
        if (!esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_KM_INIT_KEY, NULL)) {
            ESP_LOGE(TAG, "Could not find key with purpose KM_INIT_KEY");
            heap_caps_free(key_recovery_info);
            return ESP_FAIL;
        }
    }

    key_mgr_hal_start();
    key_mgr_hal_continue();
    // Step 2: Load phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    if (config->key_config->use_pre_generated_sw_init_key) {
        key_mgr_hal_write_sw_init_key(config->key_config->sw_init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    }
    ESP_LOG_BUFFER_HEX_LEVEL("SW_INIT_KEY", config->key_config->sw_init_key, KEY_MGR_SW_INIT_KEY_SIZE, ESP_LOG_DEBUG);

    ESP_LOGD(TAG, "Writing Information into Key Manager Registers");
    key_mgr_hal_write_assist_info(config->key_config->k2_info, KEY_MGR_K2_INFO_SIZE);
    ESP_LOG_BUFFER_HEX_LEVEL("K2_INFO", config->key_config->k2_info, KEY_MGR_K2_INFO_SIZE, ESP_LOG_DEBUG);
    key_mgr_hal_write_public_info(config->k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    ESP_LOG_BUFFER_HEX_LEVEL("K1_ENCRYPTED", config->k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE, ESP_LOG_DEBUG);
    key_mgr_hal_continue();
    // Step 3: Gain phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    ESP_LOG_BUFFER_HEX_LEVEL("KEY_RECOVERY_INFO", key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_DEBUG);

    if (config->key_purpose != ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1) {
        if (!key_mgr_hal_is_key_deployment_valid(config->key_config->key_type)) {
            ESP_LOGE(TAG, "Key deployment is not valid");
            heap_caps_free(key_recovery_info);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Key deployment valid");
    }
    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    if (config->key_purpose == ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2) {
        memcpy(config->key_info->key_info[1].info, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        config->key_info->key_info[1].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);

    } else {
        memcpy(config->key_info->key_info[0].info, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        config->key_info->key_info[0].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    }
    heap_caps_free(key_recovery_info);
    config->key_info->key_type = config->key_config->key_type;
    config->key_info->magic = KEY_HUK_SECTOR_MAGIC;

    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_aes_mode(const esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    if (key_config == NULL || key_recovery_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Key deployment in AES mode");

    aes_deploy_config_t aes_deploy_config = {};
    aes_deploy_config.key_config = key_config;
    aes_deploy_config.key_info = key_recovery_info;
    aes_deploy_config.k1_encrypted = key_config->k1_encrypted[0];

    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) key_config->key_type;
    if (key_type == ESP_KEY_MGR_ECDSA_KEY) {
        aes_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_ECDSA;
    } else if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        aes_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_128;
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        aes_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1;
    } else {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_hardware(true);

    esp_err_t esp_ret = key_mgr_deploy_key_aes_mode(&aes_deploy_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Key deployment in AES mode failed");
    }
    aes_deploy_config.huk_deployed = true;

    if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        aes_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2;
        aes_deploy_config.k1_encrypted = key_config->k1_encrypted[1];
        esp_ret = key_mgr_deploy_key_aes_mode(&aes_deploy_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Key deployment in AES mode failed");
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_type, ESP_KEY_MGR_USE_OWN_KEY);

    esp_key_mgr_release_hardware(true);
    return esp_ret;
}

typedef struct key_recovery_config {
    esp_key_mgr_key_purpose_t key_purpose;
    esp_key_mgr_key_recovery_info_t *key_recovery_info;
    bool huk_recovered;
} key_recovery_config_t;

static esp_err_t key_mgr_recover_key(key_recovery_config_t *config)
{
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    if (!check_huk_info_validity(&config->key_recovery_info->huk_info)) {
        ESP_LOGE(TAG, "HUK info is not valid");
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGD(TAG, "HUK info valid");

    if ((!key_mgr_hal_is_huk_valid()) || (!config->huk_recovered)) {
        check_huk_risk_level();
        esp_err_t esp_ret = huk_hal_configure(ESP_HUK_MODE_RECOVERY, config->key_recovery_info->huk_info.info);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to recover HUK");
            return ESP_FAIL;
        }
        if (!key_mgr_hal_is_huk_valid()) {
            ESP_LOGE(TAG, "HUK is invalid");
            // TODO - define error code
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "HUK recovered successfully");
        ESP_LOG_BUFFER_HEX_LEVEL("HUK INFO", config->key_recovery_info->huk_info.info, KEY_MGR_HUK_INFO_SIZE, ESP_LOG_DEBUG);
        config->huk_recovered = true;
    }

    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_RECOVER);

    // Set AES-XTS key len
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) config->key_recovery_info->key_type;
    if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_256);
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_512);
    }

    key_mgr_hal_set_key_purpose(config->key_purpose);
    key_mgr_hal_start();
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    if (config->key_purpose == ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2) {
        if (!check_key_info_validity(&config->key_recovery_info->key_info[1])) {
            ESP_LOGE(TAG, "Key info not valid");
            return ESP_FAIL;
        }
        key_mgr_hal_write_assist_info(config->key_recovery_info->key_info[1].info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        ESP_LOG_BUFFER_HEX_LEVEL("RECOVERY_INFO[1]", config->key_recovery_info->key_info[0].info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_DEBUG);
    } else {
        if (!check_key_info_validity(&config->key_recovery_info->key_info[0])) {
            ESP_LOGE(TAG, "Key info not valid");
            return ESP_FAIL;
        }
        key_mgr_hal_write_assist_info(config->key_recovery_info->key_info[0].info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        ESP_LOG_BUFFER_HEX_LEVEL("RECOVERY_INFO[0]", config->key_recovery_info->key_info[0].info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_DEBUG);
    }

    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    if (config->key_purpose != ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1) {
        if (!key_mgr_hal_is_key_deployment_valid(config->key_recovery_info->key_type)) {
            ESP_LOGD(TAG, "Key deployment is not valid");
            return ESP_FAIL;
        }
        ESP_LOGD(TAG, "Key Recovery valid");
    }
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    return ESP_OK;
}

esp_err_t esp_key_mgr_activate_key(esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    if (key_recovery_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_key_purpose_t key_purpose;
    ESP_LOGD(TAG, "Activating key of type %d", key_recovery_info->key_type);
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) key_recovery_info->key_type;
    if (key_type == ESP_KEY_MGR_ECDSA_KEY) {
        key_purpose = ESP_KEY_MGR_KEY_PURPOSE_ECDSA;
    } else if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_128;
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1;
    } else {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t esp_ret = ESP_FAIL;
    esp_key_mgr_acquire_key_lock(key_type);
    key_recovery_config_t key_recovery_config = {};
    key_recovery_config.key_recovery_info = key_recovery_info;
    key_recovery_config.key_purpose = key_purpose;

    esp_key_mgr_acquire_hardware(false);

    esp_ret = key_mgr_recover_key(&key_recovery_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to recover key");
        goto cleanup;
    }

    if (key_recovery_info->key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_recovery_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2;
        esp_ret = key_mgr_recover_key(&key_recovery_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to recover key");
            goto cleanup;
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_recovery_info->key_type, ESP_KEY_MGR_USE_OWN_KEY);
    ESP_LOGI(TAG, "Key activation for type %d successful", key_recovery_info->key_type);
    return ESP_OK;

cleanup:
    ESP_LOGI(TAG, "Key activation failed");
    esp_key_mgr_release_hardware(false);
    return esp_ret;
}

esp_err_t esp_key_mgr_deactivate_key(esp_key_mgr_key_type_t key_type)
{
    ESP_LOGD(TAG, "Deactivating key of type %d", key_type);
    esp_key_mgr_release_key_lock(key_type);
    esp_key_mgr_release_hardware(false);
    ESP_LOGI(TAG, "Key deactivation successful");
    return ESP_OK;
}

typedef struct ecdh0_config {
    esp_key_mgr_key_purpose_t key_purpose;
    const uint8_t *k1_G;
    const esp_key_mgr_ecdh0_key_config_t *key_config;
    esp_key_mgr_key_recovery_info_t *key_info;
    uint8_t *ecdh0_key_info;
    bool huk_deployed;
} ecdh0_deploy_config_t;

static esp_err_t key_mgr_deploy_key_ecdh0_mode(ecdh0_deploy_config_t *config)
{
    esp_err_t esp_ret = ESP_FAIL;
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);

    if ((!key_mgr_hal_is_huk_valid()) || (!config->huk_deployed)) {
        // For purpose ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2 this part shall be already executed
        huk_deploy_config_t huk_deploy_config;
        huk_deploy_config.use_pre_generated_huk_info = config->key_config->use_pre_generated_huk_info;
        huk_deploy_config.pre_generated_huk_info = &config->key_config->huk_info;
        huk_deploy_config.huk_recovery_info = &config->key_info->huk_info;
        esp_ret = deploy_huk(&huk_deploy_config);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
        ESP_LOGI(TAG, "HUK deployed successfully");
    }

    uint8_t *key_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_KEY_RECOVERY_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!key_recovery_info) {
        return ESP_ERR_NO_MEM;
    }

    // Step 1 : Initialization
    // Configure deployment mode to ECDH0
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_ECDH0);

    // Set AES-XTS key len
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) config->key_config->key_type;
    if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_256);
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_512);
    }

    // Set key purpose (XTS/ECDSA)
    key_mgr_hal_set_key_purpose(config->key_purpose);
    key_mgr_hal_start();
    key_mgr_hal_continue();

    // Step 2: Load phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    ESP_LOGD(TAG, "Writing Information into Key Manager Registers");
    key_mgr_hal_write_public_info(config->k1_G, KEY_MGR_ECDH0_INFO_SIZE);
    key_mgr_hal_continue();

    // Step 3: Gain phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    key_mgr_hal_read_assist_info(config->ecdh0_key_info);
    ESP_LOG_BUFFER_HEX_LEVEL("KEY_MGR KEY INFO", key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_DEBUG);

    ESP_LOGI(TAG, "HUK deplpoyed is Valid");

    if (config->key_purpose != ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1) {
        if (!key_mgr_hal_is_key_deployment_valid(config->key_config->key_type)) {
            ESP_LOGE(TAG, "Key deployment is not valid");
            heap_caps_free(key_recovery_info);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Key deployment valid");
    }
    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    if (config->key_purpose == ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2) {
        memcpy(config->key_info->key_info[1].info, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        config->key_info->key_info[1].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);

    } else {
        memcpy(config->key_info->key_info[0].info, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        config->key_info->key_info[0].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    }

    config->key_info->key_type = config->key_config->key_type;
    config->key_info->magic = KEY_HUK_SECTOR_MAGIC;

    heap_caps_free(key_recovery_info);
    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_ecdh0_mode(const esp_key_mgr_ecdh0_key_config_t *key_config,
                                               esp_key_mgr_key_recovery_info_t *key_info, esp_key_mgr_ecdh0_info_t *ecdh0_key_info)
{
    if (key_config == NULL || key_info == NULL || ecdh0_key_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }
    ESP_LOGI(TAG, "Key Deployment in ECDH0 mode");
    esp_key_mgr_key_purpose_t key_purpose;
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) key_config->key_type;

    ecdh0_deploy_config_t ecdh0_deploy_config = {};
    ecdh0_deploy_config.key_config = key_config;
    ecdh0_deploy_config.key_info = key_info;
    ecdh0_deploy_config.k1_G = key_config->k1_G[0];

    if (key_type == ESP_KEY_MGR_ECDSA_KEY) {
        ecdh0_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_ECDSA;
        ecdh0_deploy_config.ecdh0_key_info = ecdh0_key_info->k2_G[0];
    } else if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        ecdh0_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_128;
        ecdh0_deploy_config.ecdh0_key_info = ecdh0_key_info->k2_G[0];
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        ecdh0_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1;
        ecdh0_deploy_config.ecdh0_key_info = ecdh0_key_info->k2_G[0];
    } else {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_hardware(true);

    esp_err_t esp_ret = key_mgr_deploy_key_ecdh0_mode(&ecdh0_deploy_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to deploy key in ECDH0 mode");
    }
    ecdh0_deploy_config.huk_deployed = true;

    if (key_config->key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2;
        ecdh0_deploy_config.key_purpose = key_purpose;
        ecdh0_deploy_config.k1_G = key_config->k1_G[1];

        ecdh0_deploy_config.ecdh0_key_info = ecdh0_key_info->k2_G[1];
        esp_ret = key_mgr_deploy_key_ecdh0_mode(&ecdh0_deploy_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to deploy key in ECDH0 mode");
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_type, ESP_KEY_MGR_USE_OWN_KEY);

    esp_key_mgr_release_hardware(true);
    return ESP_OK;
}

typedef struct random_deploy {
    esp_key_mgr_key_purpose_t key_purpose;
    const esp_key_mgr_random_key_config_t *key_config;
    esp_key_mgr_key_recovery_info_t *key_info;
    bool huk_deployed;
} random_deploy_config_t;

static esp_err_t key_mgr_deploy_key_random_mode(random_deploy_config_t *config)
{
    esp_err_t esp_ret = ESP_FAIL;
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    if ((!key_mgr_hal_is_huk_valid()) || (!config->huk_deployed)) {
        // For purpose ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2 this part shall be already executed
        huk_deploy_config_t huk_deploy_config = {};
        huk_deploy_config.use_pre_generated_huk_info = config->key_config->use_pre_generated_huk_info;
        huk_deploy_config.pre_generated_huk_info = &config->key_config->huk_info;
        huk_deploy_config.huk_recovery_info = &config->key_info->huk_info;
        esp_ret = deploy_huk(&huk_deploy_config);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
        ESP_LOGI(TAG, "HUK deployed successfully");
    }

    // Configure deployment mode to RANDOM
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_RANDOM);

    // Set AES-XTS key len
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) config->key_config->key_type;
    if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_256);
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        key_mgr_hal_set_xts_aes_key_len(ESP_KEY_MGR_XTS_AES_LEN_512);
    }

    uint8_t *key_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_KEY_RECOVERY_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!key_recovery_info) {
        return ESP_ERR_NO_MEM;
    }

    // Set key purpose (XTS/ECDSA)
    key_mgr_hal_set_key_purpose(config->key_purpose);

    key_mgr_hal_start();
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    key_mgr_hal_continue();
    // No configuration for Random deploy mode
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    ESP_LOG_BUFFER_HEX_LEVEL("KEY_MGR KEY INFO", key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_DEBUG);

    if (config->key_purpose != ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1) {
        if (!key_mgr_hal_is_key_deployment_valid(config->key_config->key_type)) {
            ESP_LOGE(TAG, "Key deployment is not valid");
            heap_caps_free(key_recovery_info);
            return ESP_FAIL;
        }
        ESP_LOGI(TAG, "Key deployment valid");
    }

    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    if (config->key_purpose == ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2) {
        memcpy(config->key_info->key_info[1].info, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        config->key_info->key_info[1].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);

    } else {
        memcpy(config->key_info->key_info[0].info, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
        config->key_info->key_info[0].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    }
    heap_caps_free(key_recovery_info);
    config->key_info->key_type = config->key_config->key_type;
    config->key_info->magic = KEY_HUK_SECTOR_MAGIC;

    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_random_mode(const esp_key_mgr_random_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    if (key_config == NULL || key_recovery_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGI(TAG, "Key deployment in Random mode");

    random_deploy_config_t random_deploy_config = {};
    random_deploy_config.key_config = key_config;
    random_deploy_config.key_info = key_recovery_info;
    esp_key_mgr_key_type_t key_type = (esp_key_mgr_key_type_t) key_config->key_type;

    if (key_type == ESP_KEY_MGR_ECDSA_KEY) {
        random_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_ECDSA;
    } else if (key_type == ESP_KEY_MGR_XTS_AES_128_KEY) {
        random_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_128;
    } else if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        random_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_1;
    } else {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_hardware(true);

    esp_err_t esp_ret = key_mgr_deploy_key_random_mode(&random_deploy_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Key deployment in Random mode failed");
        return ESP_FAIL;
    }
    random_deploy_config.huk_deployed = true;

    if (key_type == ESP_KEY_MGR_XTS_AES_256_KEY) {
        random_deploy_config.key_purpose = ESP_KEY_MGR_KEY_PURPOSE_XTS_AES_256_2;
        esp_ret = key_mgr_deploy_key_random_mode(&random_deploy_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Key deployment in Random mode failed");
            return ESP_FAIL;
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_config->key_type, ESP_KEY_MGR_USE_OWN_KEY);

    esp_key_mgr_release_hardware(true);
    return esp_ret;
}
