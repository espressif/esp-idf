/*
 * SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The Hardware Support layer for Key manager
#include <assert.h>
#include <string.h>
#include "esp_key_mgr.h"
#include "esp_crypto_periph_clk.h"
#include "esp_crypto_lock.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_rom_crc.h"
#include "esp_efuse.h"
#include "hal/key_mgr_types.h"
#include "hal/key_mgr_hal.h"
#include "hal/huk_types.h"
#include "hal/huk_hal.h"
#include "rom/key_mgr.h"

#if SOC_KEY_MANAGER_SUPPORTED
static const char *TAG = "esp_key_mgr";

ESP_STATIC_ASSERT(sizeof(esp_key_mgr_key_recovery_info_t) == sizeof(struct huk_key_block), "Size of esp_key_mgr_key_recovery_info_t should match huk_key_block (from ROM)");
ESP_STATIC_ASSERT(sizeof(esp_key_mgr_key_info_t) == sizeof(struct key_info), "Size of esp_key_mgr_key_info_t should match key_info (from ROM)");
ESP_STATIC_ASSERT(sizeof(esp_key_mgr_huk_info_t) == sizeof(struct huk_info), "Size of esp_key_mgr_huk_info_t should match huk_info (from ROM)");

#if !NON_OS_BUILD
#include <sys/lock.h>

static _lock_t s_key_mgr_ecdsa_key_lock;
static _lock_t s_key_mgr_xts_aes_key_lock;
static _lock_t s_key_mgr_hmac_key_lock;
static _lock_t s_key_mgr_ds_key_lock;
static _lock_t s_key_mgr_psram_key_lock;

static void esp_key_mgr_acquire_key_lock(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        _lock_acquire(&s_key_mgr_ecdsa_key_lock);
        break;
    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        _lock_acquire(&s_key_mgr_xts_aes_key_lock);
        break;
    case ESP_KEY_MGR_HMAC_KEY:
        _lock_acquire(&s_key_mgr_hmac_key_lock);
        break;
    case ESP_KEY_MGR_DS_KEY:
        _lock_acquire(&s_key_mgr_ds_key_lock);
        break;
    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        _lock_acquire(&s_key_mgr_psram_key_lock);
        break;
    default:
        ESP_LOGE(TAG, "Invalid key type");
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
    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        _lock_release(&s_key_mgr_xts_aes_key_lock);
        break;
    case ESP_KEY_MGR_HMAC_KEY:
        _lock_release(&s_key_mgr_hmac_key_lock);
        break;
    case ESP_KEY_MGR_DS_KEY:
        _lock_release(&s_key_mgr_ds_key_lock);
        break;
    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        _lock_release(&s_key_mgr_psram_key_lock);
        break;
    default:
        ESP_LOGE(TAG, "Invalid key type");
        break;
    }
    ESP_LOGV(TAG, "Key lock released for key type %d", key_type);
}
#else /* !NON_OS_BUILD */
static void esp_key_mgr_acquire_key_lock(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
    case ESP_KEY_MGR_HMAC_KEY:
    case ESP_KEY_MGR_DS_KEY:
    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        break;
    default:
        ESP_LOGE(TAG, "Invalid key type");
        break;
    }
    ESP_LOGV(TAG, "Key lock acquired for key type %d", key_type);
}

static void esp_key_mgr_release_key_lock(esp_key_mgr_key_type_t key_type)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
    case ESP_KEY_MGR_HMAC_KEY:
    case ESP_KEY_MGR_DS_KEY:
    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        break;
    default:
        ESP_LOGE(TAG, "Invalid key type");
        break;
    }
    ESP_LOGV(TAG, "Key lock released for key type %d", key_type);
}
#endif /* NON_OS_BUILD */

static void esp_key_mgr_acquire_hardware(bool deployment_mode)
{
    if (deployment_mode) {
        // We only need explicit locks in the deployment mode
        esp_crypto_ecc_lock_acquire();
        esp_crypto_sha_aes_lock_acquire();
        esp_crypto_key_manager_lock_acquire();
    }
    // Reset the Key Manager Clock
    esp_crypto_key_mgr_enable_periph_clk(true);
}

static void esp_key_mgr_release_hardware(bool deployment_mode)
{
    if (deployment_mode) {
        esp_crypto_key_manager_lock_release();
        esp_crypto_sha_aes_lock_release();
        esp_crypto_ecc_lock_release();
    }

    // Reset the Key Manager Clock
    esp_crypto_key_mgr_enable_periph_clk(false);
}

/**
 * @brief Check if a key purpose requires a secondary deployment stage
 *
 * Multi-part keys (256-bit XTS-AES and 384-bit ECDSA) require two deployment stages.
 * This function identifies the primary purposes that need a follow-up secondary deployment.
 *
 * @param purpose Key purpose to check
 * @return true if this purpose requires a secondary deployment, false otherwise
 */
static inline bool is_multi_stage_key_purpose(esp_key_mgr_key_purpose_t purpose)
{
    return (purpose == ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_1 ||
            purpose == ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_1 ||
            purpose == ESP_KEY_MGR_KEY_PURPOSE_ECDSA_384_H);
}

/**
 * @brief Get the secondary key purpose for a given primary purpose
 *
 * @param primary_purpose The primary key purpose
 * @return The corresponding secondary purpose, or ESP_KEY_MGR_KEY_PURPOSE_INVALID if not applicable
 */
static inline esp_key_mgr_key_purpose_t get_secondary_key_purpose(esp_key_mgr_key_purpose_t primary_purpose)
{
    switch (primary_purpose) {
    case ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_1:
        return ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_2;
    case ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_1:
        return ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_2;
    case ESP_KEY_MGR_KEY_PURPOSE_ECDSA_384_H:
        return ESP_KEY_MGR_KEY_PURPOSE_ECDSA_384_L;
    default:
        return ESP_KEY_MGR_KEY_PURPOSE_INVALID;
    }
}

static esp_key_mgr_key_purpose_t get_key_purpose(const esp_key_mgr_key_type_t key_type, const esp_key_mgr_key_len_t key_len)
{
    switch (key_type) {
    case ESP_KEY_MGR_ECDSA_KEY:
        switch (key_len) {
        case ESP_KEY_MGR_ECDSA_LEN_192:
            return ESP_KEY_MGR_KEY_PURPOSE_ECDSA_192;
        case ESP_KEY_MGR_ECDSA_LEN_256:
            return ESP_KEY_MGR_KEY_PURPOSE_ECDSA_256;
        case ESP_KEY_MGR_ECDSA_LEN_384:
            return ESP_KEY_MGR_KEY_PURPOSE_ECDSA_384_H;
        default:
            return ESP_KEY_MGR_KEY_PURPOSE_INVALID;
        }

    case ESP_KEY_MGR_FLASH_XTS_AES_KEY:
        switch (key_len) {
        case ESP_KEY_MGR_XTS_AES_LEN_128:
            return ESP_KEY_MGR_KEY_PURPOSE_FLASH_128;
        case ESP_KEY_MGR_XTS_AES_LEN_256:
            return ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_1;
        default:
            return ESP_KEY_MGR_KEY_PURPOSE_INVALID;
        }

    case ESP_KEY_MGR_HMAC_KEY:
        return ESP_KEY_MGR_KEY_PURPOSE_HMAC;

    case ESP_KEY_MGR_DS_KEY:
        return ESP_KEY_MGR_KEY_PURPOSE_DS;

    case ESP_KEY_MGR_PSRAM_XTS_AES_KEY:
        switch (key_len) {
        case ESP_KEY_MGR_XTS_AES_LEN_128:
            return ESP_KEY_MGR_KEY_PURPOSE_PSRAM_128;
        case ESP_KEY_MGR_XTS_AES_LEN_256:
            return ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_1;
        default:
            return ESP_KEY_MGR_KEY_PURPOSE_INVALID;
        }
    default:
        return ESP_KEY_MGR_KEY_PURPOSE_INVALID;
    }
}

void key_mgr_wait_for_state(esp_key_mgr_state_t state)
{
    while (key_mgr_hal_get_state() != state) {
        ;
    }
}

static void check_huk_risk_level(void)
{
    uint8_t huk_risk_level = huk_hal_get_risk_level();
    if (huk_risk_level > KEY_MGR_HUK_RISK_ALERT_LEVEL) {
        ESP_LOGE(TAG, "HUK Risk level too high (level %d)\n"
                 "It is recommended to immediately regenerate HUK in order"
                 "to avoid permanently losing the deployed keys", huk_risk_level);
    } else {
        ESP_LOGD(TAG, "HUK Risk level - %d within acceptable limit (%d)", huk_risk_level, (int) KEY_MGR_HUK_RISK_ALERT_LEVEL);
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

static esp_err_t configure_huk(esp_huk_mode_t huk_mode, uint8_t *huk_info)
{
    esp_err_t ret = huk_hal_configure(huk_mode, huk_info);
    if (ret != ESP_OK) {
        return ret;
    }

#if SOC_HUK_MEM_NEEDS_RECHARGE
    if (!key_mgr_hal_is_huk_valid()) {
        huk_hal_recharge_huk_memory();
        ret = huk_hal_configure(huk_mode, huk_info);
        if (ret != ESP_OK) {
            return ret;
        }
    }
#endif

    if (!key_mgr_hal_is_huk_valid()) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

static esp_err_t deploy_huk(huk_deploy_config_t *config)
{
    esp_err_t esp_ret = ESP_FAIL;

    if (config->use_pre_generated_huk_info) {
        ESP_LOGD(TAG, "Using pre-generated HUK info");

        // If HUK info is provided then recover the HUK from given info
        check_huk_risk_level();

        if (!check_huk_info_validity(config->pre_generated_huk_info)) {
            ESP_LOGE(TAG, "HUK info is not valid");
            return ESP_ERR_INVALID_ARG;
        }

        ESP_LOGD(TAG, "Recovering HUK from given HUK recovery info");

        esp_ret = configure_huk(ESP_HUK_MODE_RECOVERY, (uint8_t *) config->pre_generated_huk_info->info);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to recover HUK");
            return esp_ret;
        }

        // Copy the pre generated huk info in the output key recovery info
        memcpy(config->huk_recovery_info->info, config->pre_generated_huk_info->info, KEY_MGR_HUK_INFO_SIZE);
        config->huk_recovery_info->crc = config->pre_generated_huk_info->crc;
    } else {
        // Generate new HUK and corresponding HUK info
        ESP_LOGD(TAG, "Generating new HUK");

        esp_ret = configure_huk(ESP_HUK_MODE_GENERATION, config->huk_recovery_info->info);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to generate HUK");
            memset(config->huk_recovery_info->info, 0, KEY_MGR_HUK_INFO_SIZE);
            return esp_ret;
        }

        config->huk_recovery_info->crc = esp_rom_crc32_le(0,  config->huk_recovery_info->info, KEY_MGR_HUK_INFO_SIZE);
    }

    return ESP_OK;
}

typedef struct aes_deploy {
    esp_key_mgr_key_purpose_t key_purpose;
    const uint8_t *k1_encrypted;
    const esp_key_mgr_aes_key_config_t *key_config;
    esp_key_mgr_key_recovery_info_t *key_info;
    bool huk_deployed;
} aes_deploy_config_t;

static esp_err_t key_mgr_deploy_key_aes_mode(aes_deploy_config_t *config)
{
    esp_err_t esp_ret = ESP_FAIL;
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);

    if ((!key_mgr_hal_is_huk_valid()) || (!config->huk_deployed)) {
        // For purpose ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_2 or ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_2 this part shall be already executed
        huk_deploy_config_t huk_deploy_config = {
            .use_pre_generated_huk_info = config->key_config->use_pre_generated_huk_info,
            .pre_generated_huk_info = &config->key_config->huk_info,
            .huk_recovery_info = &config->key_info->huk_info,
        };

        esp_ret = deploy_huk(&huk_deploy_config);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }

        ESP_LOGD(TAG, "HUK deployed successfully");
    }

    uint8_t key_recovery_info_index = is_multi_stage_key_purpose(config->key_purpose) ? 0 : 1;

    uint8_t *key_recovery_info = config->key_info->key_info[key_recovery_info_index].info;

    // STEP 1: Init Step
    // Set mode
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_AES);

    // Set key purpose
    key_mgr_hal_set_key_purpose(config->key_purpose);

    // Set key length for XTS-AES key
    esp_key_mgr_key_type_t key_type = config->key_config->key_type;
    esp_key_mgr_key_len_t key_len = config->key_config->key_len;

    if (key_type == ESP_KEY_MGR_FLASH_XTS_AES_KEY || key_type == ESP_KEY_MGR_PSRAM_XTS_AES_KEY) {
        key_mgr_hal_set_xts_aes_key_len(key_type, key_len);
    }

    if (config->key_config->use_pre_generated_sw_init_key) {
        key_mgr_hal_use_sw_init_key();
    } else if (!esp_efuse_find_purpose(ESP_EFUSE_KEY_PURPOSE_KM_INIT_KEY, NULL)) {
        ESP_LOGE(TAG, "Could not find key with purpose KM_INIT_KEY");
        return ESP_FAIL;
    }

    key_mgr_hal_start();

    // Step 2: Load phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);

    if (config->key_config->use_pre_generated_sw_init_key) {
        key_mgr_hal_write_sw_init_key(config->key_config->sw_init_key, KEY_MGR_SW_INIT_KEY_SIZE);
    }

    ESP_LOGD(TAG, "Writing Information into Key Manager Registers");
    key_mgr_hal_write_assist_info(config->key_config->k2_info, KEY_MGR_K2_INFO_SIZE);

    key_mgr_hal_write_public_info(config->k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);

    key_mgr_hal_continue();

    // Step 3: Gain phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);

    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);

    // Check if key deployment validation should be skipped for this purpose
    // Primary purposes in multi-stage deployments skip validation after the first stage
    // because the key is not yet completely deployed.
    if (!is_multi_stage_key_purpose(config->key_purpose)) {
        if (!key_mgr_hal_is_key_deployment_valid(key_type, key_len)) {
            ESP_LOGE(TAG, "Key deployment is not valid");
            return ESP_FAIL;
        }
    }
    ESP_LOGD(TAG, "Key deployment valid");

    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);

    config->key_info->key_info[key_recovery_info_index].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    config->key_info->key_type = key_type;
    config->key_info->key_len = key_len;
    config->key_info->key_deployment_mode = ESP_KEY_MGR_KEYGEN_MODE_AES;
    config->key_info->magic = KEY_HUK_SECTOR_MAGIC;

    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_aes_mode(const esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    if (key_config == NULL || key_recovery_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Key deployment in AES mode");

    aes_deploy_config_t aes_deploy_config = {
        .key_config = key_config,
        .key_info = key_recovery_info,
        .k1_encrypted = key_config->k1_encrypted[0],
    };

    aes_deploy_config.key_purpose = get_key_purpose(key_config->key_type, key_config->key_len);
    if (aes_deploy_config.key_purpose == ESP_KEY_MGR_KEY_PURPOSE_INVALID) {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_hardware(true);

    esp_err_t esp_ret = key_mgr_deploy_key_aes_mode(&aes_deploy_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Key deployment in AES mode failed");
        goto cleanup;
    }

    aes_deploy_config.huk_deployed = true;

    if (is_multi_stage_key_purpose(aes_deploy_config.key_purpose)) {
        aes_deploy_config.key_purpose = get_secondary_key_purpose(aes_deploy_config.key_purpose);
        aes_deploy_config.k1_encrypted = key_config->k1_encrypted[1];
        esp_ret = key_mgr_deploy_key_aes_mode(&aes_deploy_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Key deployment in AES mode failed");
            goto cleanup;
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_config->key_type, ESP_KEY_MGR_USE_OWN_KEY);

cleanup:
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
        esp_err_t esp_ret = configure_huk(ESP_HUK_MODE_RECOVERY, config->key_recovery_info->huk_info.info);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to recover HUK");
            return esp_ret;
        }

        ESP_LOGD(TAG, "HUK recovered successfully");
        config->huk_recovered = true;
    }

    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_RECOVER);

    // Set XTS-AES key length
    esp_key_mgr_key_type_t key_type = config->key_recovery_info->key_type;
    esp_key_mgr_key_len_t key_len = config->key_recovery_info->key_len;

    if (key_type == ESP_KEY_MGR_FLASH_XTS_AES_KEY || key_type == ESP_KEY_MGR_PSRAM_XTS_AES_KEY) {
        key_mgr_hal_set_xts_aes_key_len(key_type, key_len);
    }

    key_mgr_hal_set_key_purpose(config->key_purpose);

    key_mgr_hal_start();

    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);

    uint8_t key_recovery_info_index = is_multi_stage_key_purpose(config->key_purpose) ? 0 : 1;

    if (!check_key_info_validity(&config->key_recovery_info->key_info[key_recovery_info_index])) {
        ESP_LOGE(TAG, "Key info not valid");
        return ESP_FAIL;
    }

    key_mgr_hal_write_assist_info(config->key_recovery_info->key_info[key_recovery_info_index].info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);

    // Check if key deployment validation should be skipped for this purpose
    // Primary purposes in multi-stage deployments skip validation after the first stage
    // because the key is not yet completely deployed.
    if (!is_multi_stage_key_purpose(config->key_purpose)) {
        if (!key_mgr_hal_is_key_deployment_valid(key_type, key_len)) {
            ESP_LOGD(TAG, "Key deployment is not valid");
            return ESP_FAIL;
        }
    }
    ESP_LOGD(TAG, "Key Recovery valid");

    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    return ESP_OK;
}

esp_err_t esp_key_mgr_activate_key(esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    if (key_recovery_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_key_type_t key_type = key_recovery_info->key_type;

    ESP_LOGD(TAG, "Activating key of type %d", key_type);

    key_recovery_config_t key_recovery_config = {
        .key_recovery_info = key_recovery_info,
    };

    key_recovery_config.key_purpose = get_key_purpose(key_type, key_recovery_info->key_len);
    if (key_recovery_config.key_purpose == ESP_KEY_MGR_KEY_PURPOSE_INVALID) {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_key_lock(key_type);

    esp_key_mgr_acquire_hardware(false);

    esp_err_t esp_ret = key_mgr_recover_key(&key_recovery_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to recover key");
        esp_key_mgr_release_key_lock(key_type);
        goto cleanup;
    }

    if (is_multi_stage_key_purpose(key_recovery_config.key_purpose)) {
        key_recovery_config.key_purpose = get_secondary_key_purpose(key_recovery_config.key_purpose);
        esp_ret = key_mgr_recover_key(&key_recovery_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to recover key");
            esp_key_mgr_release_key_lock(key_type);
            goto cleanup;
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_type, ESP_KEY_MGR_USE_OWN_KEY);

    ESP_LOGD(TAG, "Key activation for type %d successful", key_type);
    return ESP_OK;

cleanup:
    ESP_LOGE(TAG, "Key activation failed");
    esp_key_mgr_release_hardware(false);
    return esp_ret;
}

esp_err_t esp_key_mgr_deactivate_key(esp_key_mgr_key_type_t key_type)
{
    esp_key_mgr_release_key_lock(key_type);
    esp_key_mgr_release_hardware(false);
    ESP_LOGD(TAG, "Key deactivation successful for type %d", key_type);
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
        // For purpose ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_2 or ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_2 this part shall be already executed
        huk_deploy_config_t huk_deploy_config = {
            .use_pre_generated_huk_info = config->key_config->use_pre_generated_huk_info,
            .pre_generated_huk_info = &config->key_config->huk_info,
            .huk_recovery_info = &config->key_info->huk_info,
        };

        esp_ret = deploy_huk(&huk_deploy_config);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }

        ESP_LOGD(TAG, "HUK deployed successfully");
    }

    uint8_t key_recovery_info_index = is_multi_stage_key_purpose(config->key_purpose) ? 0 : 1;

    uint8_t *key_recovery_info = config->key_info->key_info[key_recovery_info_index].info;

    // Step 1 : Initialization
    // Configure deployment mode to ECDH0
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_ECDH0);

    // Set key purpose
    key_mgr_hal_set_key_purpose(config->key_purpose);

    // Set XTS-AES key length
    esp_key_mgr_key_type_t key_type = config->key_config->key_type;
    esp_key_mgr_key_len_t key_len = config->key_config->key_len;

    if (key_type == ESP_KEY_MGR_FLASH_XTS_AES_KEY || key_type == ESP_KEY_MGR_PSRAM_XTS_AES_KEY) {
        key_mgr_hal_set_xts_aes_key_len(key_type, key_len);
    }

    key_mgr_hal_start();

    // Step 2: Load phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);

    ESP_LOGD(TAG, "Writing Information into Key Manager Registers");
    key_mgr_hal_write_public_info(config->k1_G, KEY_MGR_ECDH0_INFO_SIZE);

    key_mgr_hal_continue();

    // Step 3: Gain phase
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);

    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    key_mgr_hal_read_assist_info(config->ecdh0_key_info);

    // Check if key deployment validation should be skipped for this purpose
    // Primary purposes in multi-stage deployments skip validation after the first stage
    // because the key is not yet completely deployed.
    if (!is_multi_stage_key_purpose(config->key_purpose)) {
        if (!key_mgr_hal_is_key_deployment_valid(key_type, key_len)) {
            ESP_LOGE(TAG, "Key deployment is not valid");
            return ESP_FAIL;
        }
    }
    ESP_LOGD(TAG, "Key deployment valid");

    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);

    config->key_info->key_info[key_recovery_info_index].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    config->key_info->key_type = key_type;
    config->key_info->key_len = key_len;
    config->key_info->key_deployment_mode = ESP_KEY_MGR_KEYGEN_MODE_ECDH0;
    config->key_info->magic = KEY_HUK_SECTOR_MAGIC;

    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_ecdh0_mode(const esp_key_mgr_ecdh0_key_config_t *key_config,
                                               esp_key_mgr_key_recovery_info_t *key_info, esp_key_mgr_ecdh0_info_t *ecdh0_key_info)
{
    if (key_config == NULL || key_info == NULL || ecdh0_key_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Key Deployment in ECDH0 mode");

    ecdh0_deploy_config_t ecdh0_deploy_config = {
        .key_config = key_config,
        .key_info = key_info,
        .k1_G = key_config->k1_G[0],
        .ecdh0_key_info = ecdh0_key_info->k2_G[0],
    };

    ecdh0_deploy_config.key_purpose = get_key_purpose(key_config->key_type, key_config->key_len);
    if (ecdh0_deploy_config.key_purpose == ESP_KEY_MGR_KEY_PURPOSE_INVALID) {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_hardware(true);

    esp_err_t esp_ret = key_mgr_deploy_key_ecdh0_mode(&ecdh0_deploy_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Key deployment in ECDH0 mode failed");
        goto cleanup;
    }

    ecdh0_deploy_config.huk_deployed = true;

    if (is_multi_stage_key_purpose(ecdh0_deploy_config.key_purpose)) {
        ecdh0_deploy_config.key_purpose = get_secondary_key_purpose(ecdh0_deploy_config.key_purpose);
        ecdh0_deploy_config.k1_G = key_config->k1_G[1];
        ecdh0_deploy_config.ecdh0_key_info = ecdh0_key_info->k2_G[1];
        esp_ret = key_mgr_deploy_key_ecdh0_mode(&ecdh0_deploy_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Key deployment in ECDH0 mode failed");
            goto cleanup;
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_config->key_type, ESP_KEY_MGR_USE_OWN_KEY);

cleanup:
    esp_key_mgr_release_hardware(true);
    return esp_ret;
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
        // For purpose ESP_KEY_MGR_KEY_PURPOSE_FLASH_256_2 or ESP_KEY_MGR_KEY_PURPOSE_PSRAM_256_2 this part shall be already executed
        huk_deploy_config_t huk_deploy_config = {
            .use_pre_generated_huk_info = config->key_config->use_pre_generated_huk_info,
            .pre_generated_huk_info = &config->key_config->huk_info,
            .huk_recovery_info = &config->key_info->huk_info,
        };

        esp_ret = deploy_huk(&huk_deploy_config);
        if (esp_ret != ESP_OK) {
            return esp_ret;
        }
        ESP_LOGD(TAG, "HUK deployed successfully");
    }

    uint8_t key_recovery_info_index = is_multi_stage_key_purpose(config->key_purpose) ? 0 : 1;

    uint8_t *key_recovery_info = config->key_info->key_info[key_recovery_info_index].info;

    // Configure deployment mode to RANDOM
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_RANDOM);

    // Set key purpose
    key_mgr_hal_set_key_purpose(config->key_purpose);

    // Set XTS-AES key length
    esp_key_mgr_key_type_t key_type = config->key_config->key_type;
    esp_key_mgr_key_len_t key_len = config->key_config->key_len;

    if (key_type == ESP_KEY_MGR_FLASH_XTS_AES_KEY || key_type == ESP_KEY_MGR_PSRAM_XTS_AES_KEY) {
        key_mgr_hal_set_xts_aes_key_len(key_type, key_len);
    }

    key_mgr_hal_start();

    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);

    key_mgr_hal_continue();

    // No configuration for Random deploy mode
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);

    // Check if key deployment validation should be skipped for this purpose
    // Primary purposes in multi-stage deployments skip validation after the first stage
    // because the key is not yet completely deployed.
    if (!is_multi_stage_key_purpose(config->key_purpose)) {
        if (!key_mgr_hal_is_key_deployment_valid(key_type, key_len)) {
            ESP_LOGE(TAG, "Key deployment is not valid");
            return ESP_FAIL;
        }
    }
    ESP_LOGD(TAG, "Key deployment valid");

    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);

    config->key_info->key_info[key_recovery_info_index].crc = esp_rom_crc32_le(0, key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    config->key_info->key_type = key_type;
    config->key_info->key_len = key_len;
    config->key_info->key_deployment_mode = ESP_KEY_MGR_KEYGEN_MODE_RANDOM;
    config->key_info->magic = KEY_HUK_SECTOR_MAGIC;

    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_random_mode(const esp_key_mgr_random_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    if (key_config == NULL || key_recovery_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    ESP_LOGD(TAG, "Key deployment in Random mode");

    random_deploy_config_t random_deploy_config = {
        .key_config = key_config,
        .key_info = key_recovery_info,
    };

    random_deploy_config.key_purpose = get_key_purpose(key_config->key_type, key_config->key_len);
    if (random_deploy_config.key_purpose == ESP_KEY_MGR_KEY_PURPOSE_INVALID) {
        ESP_LOGE(TAG, "Invalid key type");
        return ESP_ERR_INVALID_ARG;
    }

    esp_key_mgr_acquire_hardware(true);

    esp_err_t esp_ret = key_mgr_deploy_key_random_mode(&random_deploy_config);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Key deployment in Random mode failed");
        goto cleanup;
    }

    random_deploy_config.huk_deployed = true;

    if (is_multi_stage_key_purpose(random_deploy_config.key_purpose)) {
        random_deploy_config.key_purpose = get_secondary_key_purpose(random_deploy_config.key_purpose);
        esp_ret = key_mgr_deploy_key_random_mode(&random_deploy_config);
        if (esp_ret != ESP_OK) {
            ESP_LOGE(TAG, "Key deployment in Random mode failed");
            goto cleanup;
        }
    }

    // Set the Key Manager Static Register to use own key for the respective key type
    key_mgr_hal_set_key_usage(key_config->key_type, ESP_KEY_MGR_USE_OWN_KEY);

cleanup:
    esp_key_mgr_release_hardware(true);
    return esp_ret;
}
#endif
