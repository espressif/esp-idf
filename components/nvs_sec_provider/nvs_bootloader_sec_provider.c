/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "nvs_sec_provider.h"
#include "esp_rom_crc.h"
#include "private/nvs_sec_provider_private.h"

#if SOC_HMAC_SUPPORTED
#include "rom/efuse.h"
#include "rom/hmac.h"
#endif  // SOC_HMAC_SUPPORTED

static __attribute__((unused)) const char *TAG = "nvs_bootloader_sec_provider";

static __attribute__((unused)) nvs_sec_scheme_t sec_scheme;

#if CONFIG_NVS_SEC_KEY_PROTECT_USING_FLASH_ENC

static nvs_sec_config_flash_enc_t nvs_sec_config_flash_enc_scheme_data;

static esp_err_t nvs_bootloader_read_security_cfg(const esp_partition_t* partition, nvs_sec_cfg_t* cfg)
{
    if (cfg == NULL || partition == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    uint32_t crc_read, crc_calc;

    esp_err_t err = esp_partition_read(partition, 0, cfg->eky, NVS_KEY_SIZE);
    if (err != ESP_OK) {
        return err;
    }

    err = esp_partition_read(partition, NVS_KEY_SIZE, cfg->tky, NVS_KEY_SIZE);
    if (err != ESP_OK) {
        return err;
    }

    err = esp_partition_read(partition, 2 * NVS_KEY_SIZE, &crc_read, 4);
    if (err != ESP_OK) {
        return err;
    }

    crc_calc = esp_rom_crc32_le(0xffffffff, cfg->eky, NVS_KEY_SIZE);
    crc_calc = esp_rom_crc32_le(crc_calc, cfg->tky, NVS_KEY_SIZE);

    if (crc_calc != crc_read) {
        return ESP_ERR_NVS_CORRUPT_KEY_PART;
    }

    return ESP_OK;
}

static esp_err_t read_security_cfg_flash_enc(const void* sec_scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (sec_scheme_cfg == NULL || cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_config_flash_enc_t *scheme_cfg_flash_enc = (nvs_sec_config_flash_enc_t *)sec_scheme_cfg;
    return nvs_bootloader_read_security_cfg(scheme_cfg_flash_enc->nvs_keys_part, cfg);
}

esp_err_t nvs_sec_provider_register_flash_enc(const nvs_sec_config_flash_enc_t *sec_scheme_cfg, nvs_sec_scheme_t **sec_scheme_handle_out)
{
    if (sec_scheme_cfg == NULL || sec_scheme_handle_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bzero(&sec_scheme, sizeof(nvs_sec_scheme_t));

    sec_scheme.scheme_id = NVS_SEC_SCHEME_FLASH_ENC;
    sec_scheme.nvs_flash_read_cfg = &read_security_cfg_flash_enc;

    bzero(&nvs_sec_config_flash_enc_scheme_data, sizeof(nvs_sec_config_flash_enc_t));
    sec_scheme.scheme_data = &nvs_sec_config_flash_enc_scheme_data;

    memcpy(sec_scheme.scheme_data, (void *)sec_scheme_cfg, sizeof(nvs_sec_config_flash_enc_t));

    *sec_scheme_handle_out = &sec_scheme;
    return ESP_OK;
}
#endif

#if SOC_HMAC_SUPPORTED

static nvs_sec_config_hmac_t nvs_sec_config_hmac_scheme_data;

#if CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID < 0
#error "NVS Encryption (HMAC): Configured eFuse block (CONFIG_NVS_SEC_HMAC_EFUSE_KEY_ID) out of range!"
#endif

static esp_err_t nvs_bootloader_hmac_calculate(ets_efuse_block_t hmac_key,
                                            const void *message,
                                            size_t message_len,
                                            uint8_t *hmac)
{
    ets_hmac_enable();

    int hmac_ret = ets_hmac_calculate_message(hmac_key, message, message_len, hmac);

    ets_hmac_disable();

    if (hmac_ret != 0) {
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

static esp_err_t compute_nvs_keys_with_hmac(ets_efuse_block_t hmac_key, nvs_sec_cfg_t* cfg)
{
    uint32_t ekey_seed[8] = {[0 ... 7] = EKEY_SEED};
    uint32_t tkey_seed[8] = {[0 ... 7] = TKEY_SEED};

    esp_err_t err = nvs_bootloader_hmac_calculate(hmac_key, ekey_seed, sizeof(ekey_seed), (uint8_t *)cfg->eky);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to calculate seed HMAC: [0x%02X]", err);
        return err;
    }

    err = nvs_bootloader_hmac_calculate(hmac_key, tkey_seed, sizeof(tkey_seed), (uint8_t *)cfg->tky);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Failed to calculate seed HMAC: [0x%02X]", err);
        return err;
    }

    /* NOTE: If the XTS E-key and T-key are the same, we have a hash collision */
    if (memcmp(cfg->eky, cfg->tky, NVS_KEY_SIZE) == 0) {
        ESP_LOGD(TAG, "The XTS-AES E-key and T-key are the same, we have a hash collision");
        return ESP_FAIL;
    }

    return ESP_OK;
}

static bool is_hmac_key_burnt_in_efuse(ets_efuse_block_t hmac_key_blk)
{

    ets_efuse_purpose_t hmac_efuse_blk_purpose = ets_efuse_get_key_purpose(hmac_key_blk);

    if (hmac_efuse_blk_purpose == ETS_EFUSE_KEY_PURPOSE_HMAC_UP) {
        return true;
    }

    return false;
}


static ets_efuse_block_t convert_key_type(hmac_key_id_t key_id) {
    return (ets_efuse_block_t)(ETS_EFUSE_BLOCK_KEY0 + (ets_efuse_block_t) key_id);
}

static esp_err_t read_security_cfg_hmac(const void* scheme_cfg, nvs_sec_cfg_t* cfg)
{
    if (cfg == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    nvs_sec_config_hmac_t *scheme_cfg_hmac = (nvs_sec_config_hmac_t *)scheme_cfg;

    if (scheme_cfg_hmac->hmac_key_id >= HMAC_KEY_MAX) {
        ESP_LOGD(TAG, "Invalid HMAC key ID received!");
        return ESP_ERR_INVALID_ARG;
    }

    ets_efuse_block_t hmac_key = convert_key_type(scheme_cfg_hmac->hmac_key_id);

    if (!is_hmac_key_burnt_in_efuse(hmac_key)) {
        ESP_LOGD(TAG, "Could not find HMAC key in configured eFuse block!");
        return ESP_ERR_NVS_SEC_HMAC_KEY_NOT_FOUND;
    }

    if (compute_nvs_keys_with_hmac(hmac_key, cfg) != ESP_OK) {
        ESP_LOGD(TAG, "Failed to derive the encryption keys using HMAC!");
        return ESP_ERR_NVS_SEC_HMAC_XTS_KEYS_DERIV_FAILED;
    }

    return ESP_OK;
}

esp_err_t nvs_sec_provider_register_hmac(const nvs_sec_config_hmac_t *sec_scheme_cfg, nvs_sec_scheme_t **sec_scheme_handle_out)
{
    if (sec_scheme_cfg == NULL || sec_scheme_handle_out == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    bzero(&sec_scheme, sizeof(nvs_sec_scheme_t));

    sec_scheme.scheme_id = NVS_SEC_SCHEME_HMAC;
    sec_scheme.nvs_flash_read_cfg = &read_security_cfg_hmac;

    bzero(&nvs_sec_config_hmac_scheme_data, sizeof(nvs_sec_config_hmac_t));
    sec_scheme.scheme_data = &nvs_sec_config_hmac_scheme_data;

    memcpy(sec_scheme.scheme_data, (void *)sec_scheme_cfg, sizeof(nvs_sec_config_hmac_t));

    *sec_scheme_handle_out = &sec_scheme;
    return ESP_OK;
}

#endif  // SOC_HMAC_SUPPORTED
