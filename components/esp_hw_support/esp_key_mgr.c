/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
// The Hardware Support layer for Key manager
#include "hal/key_mgr_types.h"
#include "hal/key_mgr_hal.h"
#include "hal/huk_types.h"
#include "hal/huk_hal.h"
#include "esp_key_mgr.h"
#include "esp_private/periph_ctrl.h"
#include "hal/key_mgr_ll.h"
#include "esp_log.h"
#include "esp_err.h"
#include "assert.h"
#include "string.h"
#include "esp_heap_caps.h"

#define KEY_MANAGER_RCC_ATOMIC() PERIPH_RCC_ATOMIC()

static const char *TAG = "esp_key_mgr";
static void key_mgr_wait_for_state(esp_key_mgr_state_t state)
{
    while (key_mgr_hal_get_state() != state) {
        ;
    }
}

esp_err_t esp_key_mgr_deploy_key_in_aes_mode(esp_key_mgr_aes_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info)
{
    ESP_LOGI(TAG, "Key Deployment");
    // Reset the Key Manager Clock
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(true);
        key_mgr_ll_reset_register();
    }

    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    uint8_t *huk_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_HUK_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!huk_recovery_info) {
        return ESP_ERR_NO_MEM;
    }
    uint8_t *key_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_KEY_RECOVERY_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!key_recovery_info) {
        return ESP_ERR_NO_MEM;
    }

    if (key_config->huk_info && key_config->huk_info_size) {
        if (key_config->huk_info_size != KEY_MGR_HUK_INFO_SIZE) {
            ESP_LOGE(TAG, "Invalid HUK info given");
            return ESP_ERR_INVALID_ARG;
        }
        ESP_LOGI(TAG, "Recovering key from given HUK recovery info");
        // If HUK info is provided then recover the HUK from given info
        huk_hal_configure(ESP_HUK_MODE_RECOVERY, key_config->huk_info);
    } else {
        // Generate new HUK and corresponding HUK info
        ESP_LOGI(TAG, "Generating new HUK");
        huk_hal_configure(ESP_HUK_MODE_GENERATION, huk_recovery_info);
    }

    ESP_LOGI(TAG, "HUK generated successfully");
    // Configure deployment mode to AES
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_AES);

    // Set key purpose (XTS/ECDSA)
    key_mgr_hal_set_key_purpose(key_config->key_purpose);

    if (key_config->use_sw_init_key) {
        key_mgr_hal_use_sw_init_key();
    }

    key_mgr_hal_start();
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    if (key_config->use_sw_init_key) {
        assert(key_config->sw_init_key_size == KEY_MGR_SW_INIT_KEY_SIZE);
        key_mgr_hal_write_sw_init_key(key_config->sw_init_key, key_config->sw_init_key_size);
    }
    ESP_LOGI(TAG, "Writing Information into Key Manager Registers");
    key_mgr_hal_write_assist_info(key_config->k2_info, KEY_MGR_K2_INFO_SIZE);
    key_mgr_hal_write_public_info(key_config->k1_encrypted, KEY_MGR_K1_ENCRYPTED_SIZE);
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    ESP_LOG_BUFFER_HEX_LEVEL("HUK INFO", huk_recovery_info, KEY_MGR_HUK_INFO_SIZE, ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEX_LEVEL("KEY_MGR KEY INFO", key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_INFO);

    //TODO - check if HUK is valid just after it is generated
    if (!key_mgr_hal_is_huk_valid()) {
        ESP_LOGE(TAG, "HUK is invalid");
        // TODO - define error code
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "HUK deplpoyed is Valid");

    if (!key_mgr_hal_is_key_deployment_valid(key_config->key_type)) {
        ESP_LOGE(TAG, "Key deployment is not valid");
        // Todo - Define respective error code;
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Key deployment valid");
    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    //memcpy(&key_info->huk_recovery_info[0], huk_recovery_info, KEY_MGR_HUK_INFO_SIZE);
    //memcpy(&key_info->key_recovery_info[0], key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    key_info->key_purpose = key_config->key_purpose;
    heap_caps_free(key_recovery_info);
    heap_caps_free(huk_recovery_info);
    key_mgr_hal_set_key_usage(ESP_KEY_MGR_XTS_KEY, ESP_KEY_MGR_USE_OWN_KEY);
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(false);
        key_mgr_ll_reset_register();
    }
    return ESP_OK;
}

esp_err_t esp_key_mgr_recover_key(esp_key_mgr_key_recovery_info_t *key_recovery_info)
{
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(true);
        key_mgr_ll_reset_register();
    }

    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    huk_hal_configure(ESP_HUK_MODE_RECOVERY, key_recovery_info->huk_recovery_info);
    if (key_mgr_hal_is_huk_valid()) {
        ESP_LOGD(TAG, "HUK is invalid");
        // TODO - define error code
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "purpose = %d", key_recovery_info->key_purpose);
    key_mgr_hal_set_key_purpose(key_recovery_info->key_purpose);
    key_mgr_hal_start();
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    key_mgr_hal_write_assist_info(key_recovery_info->huk_recovery_info, KEY_MGR_HUK_INFO_SIZE);
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    if (!key_mgr_hal_is_key_deployment_valid(key_recovery_info->key_type)) {
        ESP_LOGD(TAG, "Key deployment is not valid");
        // Todo - Define respective error code;
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Key deployment valid");
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(false);
        key_mgr_ll_reset_register();
    }
    return ESP_OK;
}

esp_err_t esp_key_mgr_deploy_key_in_ecdh0_mode(esp_key_mgr_ecdh0_key_config_t *key_config, esp_key_mgr_key_recovery_info_t *key_info)
{
    ESP_LOGI(TAG, "Key Deployment");
    // Reset the Key Manager Clock
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(true);
        key_mgr_ll_reset_register();
    }

    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    uint8_t *huk_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_HUK_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!huk_recovery_info) {
        return ESP_ERR_NO_MEM;
    }
    uint8_t *key_recovery_info = (uint8_t *) heap_caps_calloc(1, KEY_MGR_KEY_RECOVERY_INFO_SIZE, MALLOC_CAP_INTERNAL);
    if (!key_recovery_info) {
        return ESP_ERR_NO_MEM;
    }

    if (key_config->huk_info && key_config->huk_info_size) {
        if (key_config->huk_info_size != KEY_MGR_HUK_INFO_SIZE) {
            ESP_LOGE(TAG, "Invalid HUK info given");
            return ESP_ERR_INVALID_ARG;
        }
        ESP_LOGI(TAG, "Recovering key from given HUK recovery info");
        // If HUK info is provided then recover the HUK from given info
        huk_hal_configure(ESP_HUK_MODE_RECOVERY, key_config->huk_info);
    } else {
        // Generate new HUK and corresponding HUK info
        ESP_LOGI(TAG, "Generating new HUK");
        huk_hal_configure(ESP_HUK_MODE_GENERATION, huk_recovery_info);
    }

    ESP_LOGI(TAG, "HUK generated successfully");
    // Configure deployment mode to AES
    key_mgr_hal_set_key_generator_mode(ESP_KEY_MGR_KEYGEN_MODE_AES);

    // Set key purpose (XTS/ECDSA)
    key_mgr_hal_set_key_purpose(key_config->key_purpose);

    if (key_config->use_sw_init_key) {
        key_mgr_hal_use_sw_init_key();
    }

    key_mgr_hal_start();
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_LOAD);
    if (key_config->use_sw_init_key) {
        assert(key_config->sw_init_key_size == KEY_MGR_SW_INIT_KEY_SIZE);
        key_mgr_hal_write_sw_init_key(key_config->sw_init_key, key_config->sw_init_key_size);
    }
    ESP_LOGI(TAG, "Writing Information into Key Manager Registers");
    key_mgr_hal_write_public_info(key_config->k1_G, KEY_MGR_ECDH0_INFO_SIZE);
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_GAIN);
    key_mgr_hal_read_public_info(key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    key_mgr_hal_read_assist_info(key_config->k2_G);
    ESP_LOG_BUFFER_HEX_LEVEL("HUK INFO", huk_recovery_info, KEY_MGR_HUK_INFO_SIZE, ESP_LOG_INFO);
    ESP_LOG_BUFFER_HEX_LEVEL("KEY_MGR KEY INFO", key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE, ESP_LOG_INFO);

    //TODO - check if HUK is valid just after it is generated
    if (!key_mgr_hal_is_huk_valid()) {
        ESP_LOGE(TAG, "HUK is invalid");
        // TODO - define error code
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "HUK deplpoyed is Valid");

    if (!key_mgr_hal_is_key_deployment_valid(key_config->key_type)) {
        ESP_LOGE(TAG, "Key deployment is not valid");
        // Todo - Define respective error code;
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Key deployment valid");
    // Wait till Key Manager deployment is complete
    key_mgr_hal_continue();
    key_mgr_wait_for_state(ESP_KEY_MGR_STATE_IDLE);
    //memcpy(&key_info->huk_recovery_info[0], huk_recovery_info, KEY_MGR_HUK_INFO_SIZE);
    //memcpy(&key_info->key_recovery_info[0], key_recovery_info, KEY_MGR_KEY_RECOVERY_INFO_SIZE);
    key_info->key_purpose = key_config->key_purpose;
    heap_caps_free(key_recovery_info);
    heap_caps_free(huk_recovery_info);
    ESP_LOGI(TAG, "\nKey deployment complete\n");
    //key_mgr_hal_set_key_usage(ESP_KEY_MGR_XTS_KEY, ESP_KEY_MGR_USE_OWN_KEY);
    KEY_MANAGER_RCC_ATOMIC() {
        key_mgr_ll_enable_bus_clock(false);
        key_mgr_ll_reset_register();
    }
    return ESP_OK;
}
