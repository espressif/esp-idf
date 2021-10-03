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
#include "esp_efuse.h"
#include "esp_secure_boot.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
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
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU cores\n", CONFIG_IDF_TARGET, chip_info.cores);

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
}

#define DIGEST_LEN 32

static void example_secure_boot_status(void)
{
    ets_secure_boot_key_digests_t trusted_keys = { 0};

    ESP_LOGI(TAG, "Checking for Secure Boot..");
    if(esp_secure_boot_enabled()) {
        ESP_LOGI(TAG, "Secure Boot is enabled");
        ESP_ERROR_CHECK( esp_secure_boot_read_key_digests(&trusted_keys) );

        unsigned total = 0;
        for (int i = 0; i < MAX_KEY_DIGESTS; i++) {
            ESP_LOGI(TAG, "Key slot %d:", i);
            if (trusted_keys.key_digests[i]) {
                ESP_LOG_BUFFER_HEXDUMP("trusted key", trusted_keys.key_digests[i], DIGEST_LEN, ESP_LOG_INFO);
                total++;
            }
        }
        ESP_LOGI(TAG, "Total %d trusted public keys", total);
    } else {
        ESP_LOGI(TAG, "Secure Boot not enabled. Enable Secure Boot in menuconfig, build & flash again.");
    }
}
