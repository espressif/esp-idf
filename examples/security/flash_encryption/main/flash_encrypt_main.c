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
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse_table.h"

void app_main()
{
    uint32_t flash_crypt_cnt = 0;
    esp_flash_enc_mode_t mode;

    printf("\nSample program to check Flash Encryption\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    esp_efuse_read_field_blob(ESP_EFUSE_FLASH_CRYPT_CNT, &flash_crypt_cnt, 7);

    if (esp_flash_encryption_enabled()) {
        printf("Flash encryption feature is enabled\n");
        mode = esp_get_flash_encryption_mode();

        if (mode == ESP_FLASH_ENC_MODE_DEVELOPMENT) {
            printf("Flash encryption mode is DEVELOPMENT\n");
            /* If odd bits set flash encryption is enabled else disabled */
            if (__builtin_parity(flash_crypt_cnt) == 1) {
                printf("Flash in encrypted mode with flash_crypt_cnt = %d\n", flash_crypt_cnt);
            }
            else {
                printf("Flash in plaintext mode with flash_crypt_cnt = %d\n", flash_crypt_cnt);
            }
        } else {
          printf("Flash encryption mode is RELEASE\n");
        }
    }
    else {
        printf("FLASH_CRYPT_CNT eFuse value is %d\n", flash_crypt_cnt);
        printf("Flash encryption feature is disabled\n\n");
    }

    printf("Halting...\n");
    while(1) {        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }    
}
