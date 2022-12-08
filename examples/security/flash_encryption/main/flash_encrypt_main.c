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
#include "nvs_flash.h"

static void example_print_chip_info(void);
static void example_print_flash_encryption_status(void);
static void example_read_write_flash(void);

#define CUSTOM_NVS_PART_NAME "custom_nvs"

static const char* TAG = "example";

#if CONFIG_IDF_TARGET_ESP32
#define TARGET_CRYPT_CNT_EFUSE  ESP_EFUSE_FLASH_CRYPT_CNT
#define TARGET_CRYPT_CNT_WIDTH  7
#else
#define TARGET_CRYPT_CNT_EFUSE ESP_EFUSE_SPI_BOOT_CRYPT_CNT
#define TARGET_CRYPT_CNT_WIDTH  3
#endif

static esp_err_t example_custom_nvs_part_init(const char *name)
{
#if CONFIG_NVS_ENCRYPTION
    esp_err_t ret = ESP_FAIL;
    const esp_partition_t *key_part = esp_partition_find_first(
                                          ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS_KEYS, NULL);
    if (key_part == NULL) {
        ESP_LOGE(TAG, "CONFIG_NVS_ENCRYPTION is enabled, but no partition with subtype nvs_keys found in the partition table.");
        return ret;
    }

    nvs_sec_cfg_t cfg = {};
    ret = nvs_flash_read_security_cfg(key_part, &cfg);
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
    printf("\nExample to check Flash Encryption status\n");

    example_print_chip_info();
    example_print_flash_encryption_status();
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
}

static void example_print_chip_info(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }
    printf("%" PRIu32 "MB %s flash\n", flash_size / (1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}


static void example_print_flash_encryption_status(void)
{
    uint32_t flash_crypt_cnt = 0;
    esp_efuse_read_field_blob(TARGET_CRYPT_CNT_EFUSE, &flash_crypt_cnt, TARGET_CRYPT_CNT_WIDTH);
    printf("FLASH_CRYPT_CNT eFuse value is %" PRIu32 "\n", flash_crypt_cnt);

    esp_flash_enc_mode_t mode = esp_get_flash_encryption_mode();
    if (mode == ESP_FLASH_ENC_MODE_DISABLED) {
        printf("Flash encryption feature is disabled\n");
    } else {
        printf("Flash encryption feature is enabled in %s mode\n",
            mode == ESP_FLASH_ENC_MODE_DEVELOPMENT ? "DEVELOPMENT" : "RELEASE");
    }
}


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
