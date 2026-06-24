/*
 * SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Flash encryption Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/efuse_reg.h"
#include "esp_efuse.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_partition.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse_table.h"
#include "esp_secure_boot.h"
#include "nvs_flash.h"
#include "nvs_sec_provider.h"
#include "inttypes.h"

static const char* TAG = "security_features_app";

#define CUSTOM_NVS_PART_NAME "custom_nvs"

extern const uint8_t secure_jtag_token_start[] asm("_binary_secure_jtag_token_bin_start");
extern const uint8_t secure_jtag_token_end[] asm("_binary_secure_jtag_token_bin_end");

#define SECURE_JTAG_TOKEN_LENGTH 32

static void example_read_write_flash(void)
{
    const esp_partition_t* partition = esp_partition_find_first(
                                           ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
    assert(partition);

    printf("Erasing partition \"%s\" (0x%" PRIx32 " bytes)\n", partition->label, partition->size);

    ESP_ERROR_CHECK(esp_partition_erase_range(partition, 0, partition->size));

    /* Generate the data which will be written */
    const size_t data_size = 32;
    uint8_t plaintext_data[data_size];
    for (uint8_t i = 0; i < data_size; ++i) {
        plaintext_data[i] = i;
    }

    printf("Writing data with esp_partition_write:\n");
    ESP_LOG_BUFFER_HEXDUMP(TAG, plaintext_data, data_size, ESP_LOG_INFO);
    ESP_ERROR_CHECK(esp_partition_write(partition, 0, plaintext_data, data_size));

    uint8_t read_data[data_size];
    printf("Reading with esp_partition_read:\n");
    ESP_ERROR_CHECK(esp_partition_read(partition, 0, read_data, data_size));
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_data, data_size, ESP_LOG_INFO);

    printf("Reading with esp_flash_read:\n");
    ESP_ERROR_CHECK(esp_flash_read(NULL, read_data, partition->address, data_size));
    ESP_LOG_BUFFER_HEXDUMP(TAG, read_data, data_size, ESP_LOG_INFO);
}

static esp_err_t example_custom_nvs_part_init(const char *name)
{
#if CONFIG_NVS_ENCRYPTION
    esp_err_t ret = ESP_FAIL;
    nvs_sec_cfg_t cfg = {};
    nvs_sec_scheme_t *sec_scheme_handle = nvs_flash_get_default_security_scheme();

    ret = nvs_flash_read_security_cfg_v2(sec_scheme_handle, &cfg);
    if (ret != ESP_OK) {
        /* We shall not generate keys here as that must have been done in default NVS partition initialization case */
        ESP_LOGE(TAG, "Failed to read NVS security cfg: [0x%02X] (%s)", ret, esp_err_to_name(ret));
        return ret;
    }

    ret = nvs_flash_secure_init_partition(name, &cfg);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "NVS partition \"%s\" is encrypted.", name);
    }
    return ret;
#else
    return nvs_flash_init_partition(name);
#endif
}

void app_main(void)
{
    bool restart_required = false;
    bool sb_release_mode = esp_secure_boot_cfg_verify_release_mode();
    if (!sb_release_mode) {
        restart_required = true;
        ESP_LOGI(TAG, "Secure Boot V2 not in Release mode\nSetting Release mode...");
        esp_efuse_batch_write_begin();
        if (esp_secure_boot_enable_secure_features() == ESP_OK) {
            esp_efuse_batch_write_commit();
            sb_release_mode = esp_secure_boot_cfg_verify_release_mode();
        } else {
            esp_efuse_batch_write_cancel();
        }

        if (!sb_release_mode) {
            ESP_LOGE(TAG, "Failed to set Secure Boot V2 to Release mode");
        }
    }

    bool fe_release_mode = esp_flash_encryption_cfg_verify_release_mode();
    if (!fe_release_mode) {
        restart_required = true;
        ESP_LOGI(TAG, "Flash encryption not in Release mode\nSetting Release mode...");
        esp_flash_encryption_set_release_mode();
        fe_release_mode = esp_flash_encryption_cfg_verify_release_mode();
        if (!fe_release_mode) {
            ESP_LOGE(TAG, "Failed to set Flash Encryption to Release mode");
        }
    }

    if (restart_required) {
        ESP_LOGI(TAG, "Restarting now");
        esp_restart();
    }

    if (fe_release_mode) {
        ESP_LOGI(TAG, "Flash Encryption is enabled in Release Mode");
    } else {
        ESP_LOGE(TAG, "Flash Encryption is not enabled in Release mode");
    }

    if (sb_release_mode) {
        ESP_LOGI(TAG, "Secure Boot is enabled in Release Mode");
    } else {
        ESP_LOGE(TAG, "Secure Boot is not enabled in Release mode");
    }

    example_read_write_flash();
    /* Initialize the default NVS partition */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    /* Initialize the custom NVS partition */
    ret = example_custom_nvs_part_init(CUSTOM_NVS_PART_NAME);
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase_partition(CUSTOM_NVS_PART_NAME));
        ret = example_custom_nvs_part_init(CUSTOM_NVS_PART_NAME);
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "JTAG Status: Not enabled");
    ESP_LOGI(TAG, "Enabling in ..");
    for (int i = 0; i < 5; i++) {
        ESP_LOGI(TAG, "%d...", i);
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, "Enabling JTAG");
    size_t secure_jtag_token_length = strlen((const char *)secure_jtag_token_start);
    if (secure_jtag_token_length != SECURE_JTAG_TOKEN_LENGTH) {
        ESP_LOGE(TAG, "Invalid JTAG token length %d, should be %d", secure_jtag_token_length, SECURE_JTAG_TOKEN_LENGTH);
    }
    ret = esp_hmac_jtag_enable(CONFIG_EXAMPLE_JTAG_REENABLE_EFUSE_BLOCK, secure_jtag_token_start);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable JTAG");
    }

}
