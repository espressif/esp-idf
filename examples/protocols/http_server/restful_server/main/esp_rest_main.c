/* HTTP Restful API Server Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "driver/sdmmc_host.h"
#include "driver/gpio.h"
#include "esp_vfs_semihost.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mdns.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"

#define MDNS_INSTANCE "esp home web server"

static const char *TAG = "example";

esp_err_t start_rest_server(const char *base_path);

static void initialise_mdns(void)
{
    mdns_init();
    mdns_hostname_set(CONFIG_EXAMPLE_MDNS_HOST_NAME);
    mdns_instance_name_set(MDNS_INSTANCE);

    mdns_txt_item_t serviceTxtData[] = {
        {"board", "esp32"},
        {"path", "/"}
    };

    ESP_ERROR_CHECK(mdns_service_add("ESP32-WebServer", "_http", "_tcp", 80, serviceTxtData,
                                     sizeof(serviceTxtData) / sizeof(serviceTxtData[0])));
}

#if CONFIG_EXAMPLE_WEB_DEPLOY_SEMIHOST
esp_err_t init_fs(void)
{
    esp_err_t ret = esp_vfs_semihost_register(CONFIG_EXAMPLE_WEB_MOUNT_POINT, CONFIG_EXAMPLE_HOST_PATH_TO_MOUNT);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register semihost driver (%s)!", esp_err_to_name(ret));
        return ESP_FAIL;
    }
    return ESP_OK;
}
#endif

#if CONFIG_EXAMPLE_WEB_DEPLOY_SD
esp_err_t init_fs(void)
{
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    gpio_set_pull_mode(15, GPIO_PULLUP_ONLY); // CMD
    gpio_set_pull_mode(2, GPIO_PULLUP_ONLY);  // D0
    gpio_set_pull_mode(4, GPIO_PULLUP_ONLY);  // D1
    gpio_set_pull_mode(12, GPIO_PULLUP_ONLY); // D2
    gpio_set_pull_mode(13, GPIO_PULLUP_ONLY); // D3

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 4,
        .allocation_unit_size = 16 * 1024
    };

    sdmmc_card_t *card;
    esp_err_t ret = esp_vfs_fat_sdmmc_mount(CONFIG_EXAMPLE_WEB_MOUNT_POINT, &host, &slot_config, &mount_config, &card);
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }
    /* print card info if mount successfully */
    sdmmc_card_print_info(stdout, card);
    return ESP_OK;
}
#endif

#if CONFIG_EXAMPLE_WEB_DEPLOY_SF
esp_err_t init_fs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_EXAMPLE_WEB_MOUNT_POINT,
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ESP_FAIL;
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ESP_OK;
}
#endif

void app_main(void)
{
    ESP_ERROR_CHECK(nvs_flash_init());
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    initialise_mdns();

    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(init_fs());
    ESP_ERROR_CHECK(start_rest_server(CONFIG_EXAMPLE_WEB_MOUNT_POINT));
}
