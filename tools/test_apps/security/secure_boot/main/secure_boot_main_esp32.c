/* Flash encryption Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/efuse_reg.h"
#include "hal/efuse_ll.h"
#include "esp_efuse.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_efuse_table.h"
#include <string.h>

static void example_print_chip_info(void);
static void example_secure_boot_status(void);

#define TAG "example_secure_boot"

void app_main(void)
{
    printf("\nExample to check Secure Boot status\n");

    example_print_chip_info();
    example_secure_boot_status();
}


static void example_print_chip_info(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
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

    printf("%dMB %s flash\n", flash_size / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

#define DIGEST_LEN 32

static void example_secure_boot_status(void)
{
#if CONFIG_ESP32_REV_MIN_FULL >= 300
    uint8_t efuse_trusted_digest[DIGEST_LEN] = {0}, i;
    ESP_LOGI(TAG, "Checking for secure boot v2..");
    if(efuse_ll_get_secure_boot_v2_en()) {
        ESP_LOGI(TAG, "ABS_DONE_1 is set. Secure Boot V2 enabled");
        esp_err_t err = esp_efuse_read_block(EFUSE_BLK2, &efuse_trusted_digest, 0, DIGEST_LEN * 8);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error (0x%x) while reading eFuse BLK2", err);
        }
        ESP_LOGI(TAG,  "Reading the public key digest from BLK2.");
        for (i = 0; i < DIGEST_LEN; i++) {
            ESP_LOGI(TAG, "%02x \t", efuse_trusted_digest[i]);
        }
        return;
    } else {
        ESP_LOGI(TAG, "Secure boot v2 not enabled. Enable Secure Boot V2 in menuconfig, build & flash again.");
    }
#endif

    ESP_LOGI(TAG, "Checking for secure boot v1..");
    if (efuse_ll_get_secure_boot_v1_en()) {
    ESP_LOGI(TAG, "ABS_DONE_0 is set. Secure Boot V1 enabled");
#if CONFIG_ESP32_REV_MIN_FULL >= 300
        ESP_LOGW(TAG, "This chip version supports Secure Boot V2. It is recommended to use Secure Boot V2.");
#endif
        ESP_LOGI(TAG, "Checking the integrityof the key in BLK2..");
        if (!esp_efuse_get_key_dis_read(EFUSE_BLK2)) {
            ESP_LOGE(TAG, "Key is not read protected. Refusing to blow secure boot efuse.");
            return;
        }
        if (!esp_efuse_get_key_dis_write(EFUSE_BLK2)) {
            ESP_LOGE(TAG, "Key is not write protected. Refusing to blow secure boot efuse.");
            return;
        }
        ESP_LOGI(TAG, "Key is read/write protected in eFuse.");
        return;
    } else {
        ESP_LOGI(TAG, "Secure Boot V1 not enabled. Enable Secure Boot in menuconfig, build & flash again.");
    }
}
