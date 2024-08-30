/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_fault.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "nvs_flash.h"
#include "nvs_sec_provider.h"

#include "esp_private/startup_internal.h"
#if SOC_HMAC_SUPPORTED
#include "bootloader_random.h"
#include "esp_random.h"

#include "esp_efuse.h"
#include "esp_efuse_chip.h"
#endif  // SOC_HMAC_SUPPORTED

static __attribute__((unused)) const char *TAG = "nvs_sec_provider";

#if CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC

static esp_err_t generate_keys_flash_enc(const void* sec_scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (sec_scheme_cfg == NULL || cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_config_flash_enc_t *scheme_cfg_flash_enc = (nvs_sec_config_flash_enc_t *)sec_scheme_cfg;
    return nvs_flash_generate_keys(scheme_cfg_flash_enc->nvs_keys_part, cfg);
}

static esp_err_t read_security_cfg_flash_enc(const void* sec_scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (sec_scheme_cfg == NULL || cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_config_flash_enc_t *scheme_cfg_flash_enc = (nvs_sec_config_flash_enc_t *)sec_scheme_cfg;
    return nvs_flash_read_security_cfg(scheme_cfg_flash_enc->nvs_keys_part, cfg);
}

esp_err_t nvs_sec_provider_register_flash_enc(const nvs_sec_config_flash_enc_t *sec_scheme_cfg, nvs_sec_scheme_t **sec_scheme_handle_out)
{
    if (sec_scheme_cfg == NULL || sec_scheme_handle_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_scheme_t *sec_scheme = calloc(1, sizeof(nvs_sec_scheme_t));
    if (sec_scheme == NULL) {
        return ESP_ERR_NO_MEM;
    }

    sec_scheme->scheme_id = NVS_SEC_SCHEME_FLASH_ENC;
    sec_scheme->nvs_flash_key_gen = &generate_keys_flash_enc;
    sec_scheme->nvs_flash_read_cfg = &read_security_cfg_flash_enc;

    sec_scheme->scheme_data = calloc(1, sizeof(nvs_sec_config_flash_enc_t));
    if (sec_scheme->scheme_data == NULL) {
        free(sec_scheme);
        return ESP_ERR_NO_MEM;
    }
    memcpy(sec_scheme->scheme_data, (void *)sec_scheme_cfg, sizeof(nvs_sec_config_flash_enc_t));

    esp_err_t err = nvs_flash_register_security_scheme(sec_scheme);
    if (err != ESP_OK) {
        free(sec_scheme->scheme_data);
        free(sec_scheme);
        return err;
    }

    *sec_scheme_handle_out = sec_scheme;
    return ESP_OK;
}


ESP_SYSTEM_INIT_FN(nvs_sec_provider_register_flash_enc_scheme, SECONDARY, BIT(0), 150)
{
    ESP_EARLY_LOGI(TAG, "NVS Encryption - Registering Flash encryption-based scheme...");

    nvs_sec_config_flash_enc_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_FLASH_ENC_DEFAULT();

     /*
     * This checks partition with subtype nvs_keys from partition table, if NVS Encryption is enabled
     * and "nvs_keys" do not exist in partition table, then execution gets aborted. To fix the problem,
     * please introduce partition with subtype "nvs_keys" in the partition table.
     */

    if (sec_scheme_cfg.nvs_keys_part == NULL) {
        ESP_EARLY_LOGE(TAG, "partition with subtype \"nvs_keys\" not found");
        return ESP_FAIL;
    }

    nvs_sec_scheme_t *sec_scheme_handle_out = NULL;

    return nvs_sec_provider_register_flash_enc(&sec_scheme_cfg, &sec_scheme_handle_out);
}

#endif

#if SOC_HMAC_SUPPORTED

#if CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID > 5
#error "NVS Encryption (HMAC): Configured eFuse block (CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID) out of range!"
#endif

static esp_err_t compute_nvs_keys_with_hmac(hmac_key_id_t hmac_key_id, nvs_sec_cfg_t* cfg)
{
    uint32_t ekey_seed[8] = {[0 ... 7] = 0xAEBE5A5A};
    uint32_t tkey_seed[8] = {[0 ... 7] = 0xCEDEA5A5};

    esp_err_t err = esp_hmac_calculate(hmac_key_id, ekey_seed, sizeof(ekey_seed), (uint8_t *)cfg->eky);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calculate seed HMAC: [0x%02X] (%s)", err, esp_err_to_name(err));
        return err;
    }
    ESP_FAULT_ASSERT(err == ESP_OK);

    err = esp_hmac_calculate(hmac_key_id, tkey_seed, sizeof(tkey_seed), (uint8_t *)cfg->tky);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to calculate seed HMAC: [0x%02X] (%s)", err, esp_err_to_name(err));
        return err;
    }
    ESP_FAULT_ASSERT(err == ESP_OK);

    /* NOTE: If the XTS E-key and T-key are the same, we have a hash collision */
    ESP_FAULT_ASSERT(memcmp(cfg->eky, cfg->tky, NVS_KEY_SIZE) != 0);

    return ESP_OK;
}

static esp_efuse_block_t convert_key_type(hmac_key_id_t key_id) {
    return (esp_efuse_block_t)(EFUSE_BLK_KEY0 + (esp_efuse_block_t) key_id);
}

static bool is_hmac_key_burnt_in_efuse(hmac_key_id_t hmac_key_id)
{
    bool ret = false;

    esp_efuse_block_t hmac_key_blk = convert_key_type(hmac_key_id);

    esp_efuse_purpose_t hmac_efuse_blk_purpose = esp_efuse_get_key_purpose(hmac_key_blk);
    if (hmac_efuse_blk_purpose == ESP_EFUSE_KEY_PURPOSE_HMAC_UP) {
        ret = true;
    }

    return ret;
}

static esp_err_t generate_keys_hmac(const void* scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_config_hmac_t *scheme_cfg_hmac = (nvs_sec_config_hmac_t *)scheme_cfg;

    hmac_key_id_t hmac_key = scheme_cfg_hmac->hmac_key_id;
    if (hmac_key >= HMAC_KEY_MAX) {
        ESP_LOGE(TAG, "Invalid HMAC key ID received!");
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = ESP_FAIL;

    if (!is_hmac_key_burnt_in_efuse(hmac_key)) {
        uint8_t hmac_key_buf[32] = {0};

        bootloader_random_enable();
        esp_fill_random(hmac_key_buf, sizeof(hmac_key_buf));
        bootloader_random_disable();

        esp_efuse_block_t hmac_key_blk = convert_key_type(hmac_key);

        err = esp_efuse_write_key(hmac_key_blk, ESP_EFUSE_KEY_PURPOSE_HMAC_UP, hmac_key_buf, sizeof(hmac_key_buf));
        if (err != ESP_OK) {
            if (err == ESP_ERR_EFUSE_REPEATED_PROG) {
                ESP_LOGE(TAG, "Configured eFuse block for HMAC key already-in-use!");
                return ESP_ERR_NVS_SEC_HMAC_KEY_BLK_ALREADY_USED;
            }
            ESP_LOGE(TAG, "Failed to write the HMAC key to eFuse: [0x%02X] (%s)", err, esp_err_to_name(err));
            return ESP_ERR_NVS_SEC_HMAC_KEY_GENERATION_FAILED;
        }
        memset(hmac_key_buf, 0x00, sizeof(hmac_key_buf));
    } else {
        ESP_LOGW(TAG, "HMAC Key already present in configured eFuse block, using the same for NVS encryption!");
    }

    err = compute_nvs_keys_with_hmac(hmac_key, cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to derive the encryption keys using HMAC!");
        return ESP_ERR_NVS_SEC_HMAC_XTS_KEYS_DERIV_FAILED;
    }

    return ESP_OK;
}

static esp_err_t read_security_cfg_hmac(const void* scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_config_hmac_t *scheme_cfg_hmac = (nvs_sec_config_hmac_t *)scheme_cfg;

    hmac_key_id_t hmac_key = scheme_cfg_hmac->hmac_key_id;
    if (hmac_key >= HMAC_KEY_MAX) {
        ESP_LOGE(TAG, "Invalid HMAC key ID received!");
        return ESP_ERR_INVALID_ARG;
    }

    if (!is_hmac_key_burnt_in_efuse(hmac_key)) {
        ESP_LOGE(TAG, "Could not find HMAC key in configured eFuse block!");
        return ESP_ERR_NVS_SEC_HMAC_KEY_NOT_FOUND;
    }

    esp_err_t err = compute_nvs_keys_with_hmac(hmac_key, cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to derive the encryption keys using HMAC!");
        return ESP_ERR_NVS_SEC_HMAC_XTS_KEYS_DERIV_FAILED;
    }

    return ESP_OK;
}

esp_err_t nvs_sec_provider_register_hmac(const nvs_sec_config_hmac_t *sec_scheme_cfg, nvs_sec_scheme_t **sec_scheme_handle_out)
{
    if (sec_scheme_cfg == NULL || sec_scheme_handle_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_scheme_t *sec_scheme = calloc(1, sizeof(nvs_sec_scheme_t));
    if (sec_scheme == NULL) {
        return ESP_ERR_NO_MEM;
    }

    sec_scheme->scheme_id = NVS_SEC_SCHEME_HMAC;
    sec_scheme->nvs_flash_key_gen = &generate_keys_hmac;
    sec_scheme->nvs_flash_read_cfg = &read_security_cfg_hmac;

    sec_scheme->scheme_data = calloc(1, sizeof(nvs_sec_config_hmac_t));
    if (sec_scheme->scheme_data == NULL) {
        free(sec_scheme);
        return ESP_ERR_NO_MEM;
    }

    memcpy(sec_scheme->scheme_data, (void *)sec_scheme_cfg, sizeof(nvs_sec_config_hmac_t));

    esp_err_t err = nvs_flash_register_security_scheme(sec_scheme);
    if (err != ESP_OK) {
        free(sec_scheme->scheme_data);
        free(sec_scheme);
        return err;
    }

    *sec_scheme_handle_out = sec_scheme;
    return ESP_OK;
}

#if CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC
ESP_SYSTEM_INIT_FN(nvs_sec_provider_register_hmac_scheme, SECONDARY, BIT(0), 151)
{
    ESP_EARLY_LOGI(TAG, "NVS Encryption - Registering HMAC-based scheme...");

    nvs_sec_config_hmac_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_HMAC_DEFAULT();

    nvs_sec_scheme_t *sec_scheme_handle_out = NULL;

    return nvs_sec_provider_register_hmac(&sec_scheme_cfg, &sec_scheme_handle_out);
}
#endif  // CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC

#endif  // SOC_HMAC_SUPPORTED

esp_err_t nvs_sec_provider_deregister(nvs_sec_scheme_t *sec_scheme_handle)
{
    if (sec_scheme_handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (sec_scheme_handle->scheme_data != NULL) {
        free(sec_scheme_handle->scheme_data);
    }

    free(sec_scheme_handle);

    return ESP_OK;
}

void nvs_sec_provider_include_impl(void)
{
    // Linker hook, exists for no other purpose
}
