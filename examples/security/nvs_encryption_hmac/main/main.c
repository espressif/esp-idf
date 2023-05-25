/*
 * NVS Encryption with HMAC-based encryption key protection scheme example
 *
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>

#include "esp_flash.h"
#include "esp_partition.h"
#include "esp_hmac.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "nvs_sec_provider.h"

#define CUSTOM_NVS_PART_LABEL       "custom_nvs"
#define CUSTOM_NVS_PART_NAMESPACE   "storage"
#define CUSTOM_NVS_PART_DUMP_SIZE   (512 + 16)

static const char* str_val = "Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
                             "Fusce quis risus justo.\n"
                             "Suspendisse egestas in nisi sit amet auctor.\n"
                             "Pellentesque rhoncus dictum sodales.\n"
                             "In justo erat, viverra at interdum eget, interdum vel dui.\n";

static const char* TAG = "example";

static esp_err_t example_custom_nvs_part_init(const char *label)
{
    esp_err_t ret = ESP_FAIL;
#if defined(CONFIG_NVS_ENCRYPTION) && defined(CONFIG_NVS_SEC_KEY_PROTECT_USING_HMAC)
    nvs_sec_cfg_t cfg = {};
    nvs_sec_scheme_t *sec_scheme_handle = NULL;

    nvs_sec_config_hmac_t sec_scheme_cfg = NVS_SEC_PROVIDER_CFG_HMAC_DEFAULT();

    ret = nvs_sec_provider_register_hmac(&sec_scheme_cfg, &sec_scheme_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = nvs_flash_read_security_cfg_v2(sec_scheme_handle, &cfg);
    if (ret != ESP_OK) {
        /* We shall not generate keys here as that must have been done in default NVS partition initialization case */
        ESP_LOGE(TAG, "Failed to read NVS security cfg: [0x%02X] (%s)", ret, esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_flash_secure_init_partition(label, &cfg);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "NVS partition \"%s\" is encrypted.", label);
    }
    memset(&cfg, 0x00, sizeof(nvs_sec_cfg_t));
#else
    ret = nvs_flash_init_partition(label);
#endif
    return ret;
}

static esp_err_t example_custom_nvs_part_write(const char *label, const char *namespace)
{
    nvs_handle_t my_handle;
    esp_err_t err = ESP_FAIL;

    // Open
    err = nvs_open_from_partition(label, namespace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Write
    err = nvs_set_u8(my_handle, "u8_key", UINT8_MAX);
    if (err != ESP_OK) goto exit;

    err = nvs_set_i8(my_handle, "i8_key", INT8_MIN);
    if (err != ESP_OK) goto exit;

    err = nvs_set_u16(my_handle, "u16_key", UINT16_MAX);
    if (err != ESP_OK) goto exit;

    err = nvs_set_u32(my_handle, "u32_key", UINT32_MAX);
    if (err != ESP_OK) goto exit;

    err = nvs_set_i32(my_handle, "i32_key", INT32_MIN);
    if (err != ESP_OK) goto exit;

    err = nvs_set_str(my_handle, "str_key", str_val);
    if (err != ESP_OK) goto exit;

    // Commit
    err = nvs_commit(my_handle);
    if (err != ESP_OK) goto exit;

exit:
    // Close
    nvs_close(my_handle);
    return err;
}

static esp_err_t example_custom_nvs_part_read(const char *label, const char *namespace)
{
    nvs_handle_t my_handle;
    esp_err_t err = ESP_FAIL;

    // Open
    err = nvs_open_from_partition(label, namespace, NVS_READWRITE, &my_handle);
    if (err != ESP_OK) return err;

    // Write
    uint8_t u8_val = 0;
    err = nvs_get_u8(my_handle, "u8_key", &u8_val);
    if (err != ESP_OK) goto exit;
    ESP_LOGI(TAG, "Key: u8_key | Val: %" PRIu8, u8_val);

    int8_t i8_val = 0;
    err = nvs_get_i8(my_handle, "i8_key", &i8_val);
    if (err != ESP_OK) goto exit;
    ESP_LOGI(TAG, "Key: i8_key | Val: %" PRIi8, i8_val);

    uint16_t u16_val = 0;
    err = nvs_get_u16(my_handle, "u16_key", &u16_val);
    if (err != ESP_OK) goto exit;
    ESP_LOGI(TAG, "Key: u16_key | Val: %" PRIu16, u16_val);

    uint32_t u32_val = 0;
    err = nvs_get_u32(my_handle, "u32_key", &u32_val);
    if (err != ESP_OK) goto exit;
    ESP_LOGI(TAG, "Key: u32_key | Val: %" PRIu32, u32_val);

    int32_t i32_val = 0;
    err = nvs_get_i32(my_handle, "i32_key", &i32_val);
    if (err != ESP_OK) goto exit;
    ESP_LOGI(TAG, "Key: i32_key | Val: %" PRIi32, i32_val);

    size_t str_val_len = 0;
    err = nvs_get_str(my_handle, "str_key", NULL, &str_val_len);
    if (err != ESP_OK) goto exit;

    char* str_key_val = malloc(str_val_len);
    assert(str_val);

    err = nvs_get_str(my_handle, "str_key", str_key_val, &str_val_len);
    if (err != ESP_OK) goto cleanup;
    ESP_LOGI(TAG, "Key: str_key | Val: %s", str_key_val);

cleanup:
    free(str_key_val);
exit:
    // Close
    nvs_close(my_handle);
    return err;
}

void dump_custom_nvs_partition(const char *label, size_t len)
{
    const esp_partition_t* partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, label);
    assert(partition);

    uint8_t *read_data = calloc(len, sizeof(uint8_t));
    assert(read_data != NULL);

    ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, len));
    ESP_LOG_BUFFER_HEXDUMP(label, read_data, len, ESP_LOG_INFO);

    free(read_data);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initialising the default NVS partition");

    /* Initialising the default NVS partition */
    ESP_ERROR_CHECK(nvs_flash_init());

    /* Erasing the custom NVS partition */
    ESP_ERROR_CHECK(nvs_flash_erase_partition(CUSTOM_NVS_PART_LABEL));

    ESP_LOGI(TAG, "Initialising the custom NVS partition");
    /* Initialize the custom NVS partition with encryption enabled */
    esp_err_t ret = example_custom_nvs_part_init(CUSTOM_NVS_PART_LABEL);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase_partition(CUSTOM_NVS_PART_LABEL));
        ret = example_custom_nvs_part_init(CUSTOM_NVS_PART_LABEL);
    }
    ESP_ERROR_CHECK(ret);

    ret = example_custom_nvs_part_write(CUSTOM_NVS_PART_LABEL, CUSTOM_NVS_PART_NAMESPACE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write NVS partition (%s | %s): %s", CUSTOM_NVS_PART_LABEL, CUSTOM_NVS_PART_NAMESPACE, esp_err_to_name(ret));
    };

    ret = example_custom_nvs_part_read(CUSTOM_NVS_PART_LABEL, CUSTOM_NVS_PART_NAMESPACE);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read NVS partition (%s | %s): %s", CUSTOM_NVS_PART_LABEL, CUSTOM_NVS_PART_NAMESPACE, esp_err_to_name(ret));
    };

    dump_custom_nvs_partition(CUSTOM_NVS_PART_LABEL, CUSTOM_NVS_PART_DUMP_SIZE);
}
