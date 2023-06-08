/* efuse example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include "esp_efuse_custom_table.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "sdkconfig.h"

static const char* TAG = "example";


typedef struct {
    uint8_t module_version;        /*!< Module version: length 8 bits */
    uint8_t device_role;           /*!< Device role: length 3 bits */
    uint8_t setting_1;             /*!< Setting 1: length 6 bits */
    uint8_t setting_2;             /*!< Setting 2: length 5 bits */
    size_t custom_secure_version;  /*!< Custom secure version: length 16 bits */
    uint16_t reserv;               /*!< Reserv */
} device_desc_t;


static void print_device_desc(device_desc_t *desc)
{
    ESP_LOGI(TAG, "module_version = %d", desc->module_version);
    if (desc->device_role == 0) {
        ESP_LOGI(TAG, "device_role = None");
    } else if (desc->device_role == 1) {
        ESP_LOGI(TAG, "device_role = Master");
    } else if (desc->device_role == 2) {
        ESP_LOGI(TAG, "device_role = Slave");
    } else {
        ESP_LOGI(TAG, "device_role = Not supported");
    }
    ESP_LOGI(TAG, "setting_1 = %d", desc->setting_1);
    ESP_LOGI(TAG, "setting_2 = %d", desc->setting_2);
    ESP_LOGI(TAG, "custom_secure_version = %d", desc->custom_secure_version);
}


static void read_device_desc_efuse_fields(device_desc_t *desc)
{
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_MODULE_VERSION, &desc->module_version, 8));
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_DEVICE_ROLE, &desc->device_role, 3));
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_SETTING_1, &desc->setting_1, 6));
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_SETTING_2, &desc->setting_2, 5));
    ESP_ERROR_CHECK(esp_efuse_read_field_cnt(ESP_EFUSE_CUSTOM_SECURE_VERSION, &desc->custom_secure_version));
    print_device_desc(desc);
}


static void read_efuse_fields(device_desc_t *desc)
{
    ESP_LOGI(TAG, "read efuse fields");

    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_efuse_read_field_blob(ESP_EFUSE_MAC_FACTORY, &mac, sizeof(mac) * 8));
    ESP_LOGI(TAG, "1. read MAC address: %02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    size_t secure_version = 0;
    ESP_ERROR_CHECK(esp_efuse_read_field_cnt(ESP_EFUSE_SECURE_VERSION, &secure_version));
    ESP_LOGI(TAG, "2. read secure_version: %d", secure_version);

    ESP_LOGI(TAG, "3. read custom fields");
    read_device_desc_efuse_fields(desc);
}


#if defined(CONFIG_EFUSE_VIRTUAL) || defined(CONFIG_EXAMPLE_TEST_RUN_USING_QEMU)
static void write_efuse_fields(device_desc_t *desc, esp_efuse_coding_scheme_t coding_scheme)
{
#if CONFIG_IDF_TARGET_ESP32
    const esp_efuse_coding_scheme_t coding_scheme_for_batch_mode = EFUSE_CODING_SCHEME_3_4;
#else
    const esp_efuse_coding_scheme_t coding_scheme_for_batch_mode = EFUSE_CODING_SCHEME_RS;
#endif

    ESP_LOGI(TAG, "write custom efuse fields");
    if (coding_scheme == coding_scheme_for_batch_mode) {
        ESP_LOGI(TAG, "In the case of 3/4 or RS coding scheme, you cannot write efuse fields separately");
        ESP_LOGI(TAG, "You should use the batch mode of writing fields for this");
        ESP_ERROR_CHECK(esp_efuse_batch_write_begin());
    }

    ESP_ERROR_CHECK(esp_efuse_write_field_blob(ESP_EFUSE_MODULE_VERSION, &desc->module_version, 8));
    ESP_ERROR_CHECK(esp_efuse_write_field_blob(ESP_EFUSE_DEVICE_ROLE, &desc->device_role, 3));
    ESP_ERROR_CHECK(esp_efuse_write_field_blob(ESP_EFUSE_SETTING_1, &desc->setting_1, 6));
    ESP_ERROR_CHECK(esp_efuse_write_field_blob(ESP_EFUSE_SETTING_2, &desc->setting_2, 5));
    ESP_ERROR_CHECK(esp_efuse_write_field_cnt(ESP_EFUSE_CUSTOM_SECURE_VERSION, desc->custom_secure_version));

    if (coding_scheme == coding_scheme_for_batch_mode) {
        ESP_ERROR_CHECK(esp_efuse_batch_write_commit());
    }
}
#endif // defined(CONFIG_EFUSE_VIRTUAL) || defined(CONFIG_EXAMPLE_TEST_RUN_USING_QEMU)


static esp_efuse_coding_scheme_t get_coding_scheme(void)
{
    // The coding scheme is used for EFUSE_BLK1, EFUSE_BLK2 and EFUSE_BLK3.
    // We use EFUSE_BLK3 (custom block) to verify it.
    esp_efuse_coding_scheme_t coding_scheme = esp_efuse_get_coding_scheme(EFUSE_BLK3);
    if (coding_scheme == EFUSE_CODING_SCHEME_NONE) {
        ESP_LOGI(TAG, "Coding Scheme NONE");
#if CONFIG_IDF_TARGET_ESP32
    } else if (coding_scheme == EFUSE_CODING_SCHEME_3_4) {
        ESP_LOGI(TAG, "Coding Scheme 3/4");
    } else {
        ESP_LOGI(TAG, "Coding Scheme REPEAT");
    }
#else
    } else if (coding_scheme == EFUSE_CODING_SCHEME_RS) {
        ESP_LOGI(TAG, "Coding Scheme RS (Reed-Solomon coding)");
    }
#endif
    return coding_scheme;
}


void app_main(void)
{
    ESP_LOGI(TAG, "Start eFuse example");

#ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
    if (esp_flash_encryption_cfg_verify_release_mode()) {
        ESP_LOGI(TAG, "Flash Encryption is in RELEASE mode");
    } else {
        ESP_LOGW(TAG, "Flash Encryption is NOT in RELEASE mode");
    }
#endif
#ifdef CONFIG_SECURE_BOOT
    if (esp_secure_boot_cfg_verify_release_mode()) {
        ESP_LOGI(TAG, "Secure Boot is in RELEASE mode");
    } else {
        ESP_LOGW(TAG, "Secure Boot is NOT in RELEASE mode");
    }
#endif

    esp_efuse_coding_scheme_t coding_scheme = get_coding_scheme();
    (void) coding_scheme;

    device_desc_t device_desc = { 0 };
    read_efuse_fields(&device_desc);

#if !CONFIG_EXAMPLE_TEST_RUN_USING_QEMU
    ESP_LOGW(TAG, "This example does not burn any efuse in reality only virtually");
#endif


#if CONFIG_IDF_TARGET_ESP32C2
    if (esp_secure_boot_enabled() || esp_flash_encryption_enabled()) {
        ESP_LOGW(TAG, "BLOCK3 is used for secure boot or/and flash encryption");
        ESP_LOGW(TAG, "eFuses from the custom eFuse table can not be used as they are placed in BLOCK3");
        ESP_LOGI(TAG, "Done");
        return;
    }
#endif

#if defined(CONFIG_EFUSE_VIRTUAL) || defined(CONFIG_EXAMPLE_TEST_RUN_USING_QEMU)
#if !CONFIG_EXAMPLE_TEST_RUN_USING_QEMU
    ESP_LOGW(TAG, "Write operations in efuse fields are performed virtually");
#endif
    if (device_desc.device_role == 0) {
        device_desc.module_version = 1;
        device_desc.device_role = 2;
        device_desc.setting_1 = 3;
        device_desc.setting_2 = 4;
        device_desc.custom_secure_version = 5;
        write_efuse_fields(&device_desc, coding_scheme);
        read_device_desc_efuse_fields(&device_desc);
    }
#else
    ESP_LOGW(TAG, "The part of the code that writes efuse fields is disabled");
#endif

    ESP_LOGI(TAG, "Done");
}
