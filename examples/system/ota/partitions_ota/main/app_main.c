/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* OTA partitions example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "protocol_examples_common.h"
#include "string.h"
#ifdef CONFIG_EXAMPLE_USE_CERT_BUNDLE
#include "esp_crt_bundle.h"
#endif
#include "esp_flash.h"
#include "esp_flash_partitions.h"
#include "esp_partition.h"

#include "nvs.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include <sys/socket.h>
#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

#include "partition_utils.h"

#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
/* The interface name value can refer to if_desc in esp_netif_defaults.h */
#if CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF_ETH
static const char *bind_interface_name = EXAMPLE_NETIF_DESC_ETH;
#elif CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF_STA
static const char *bind_interface_name = EXAMPLE_NETIF_DESC_STA;
#endif
#endif

static const char *TAG = "ota_example";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

#define OTA_URL_SIZE 256

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
    case HTTP_EVENT_ERROR:
        ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

static esp_err_t register_partition(size_t offset, size_t size, const char *label, esp_partition_type_t type, esp_partition_subtype_t subtype, const esp_partition_t **p_partition)
{
    // If the partition table contains this partition, then use it, otherwise register it.
    *p_partition = esp_partition_find_first(type, subtype, NULL);
    if ((*p_partition) == NULL) {
        esp_err_t error = esp_partition_register_external(NULL, offset, size, label, type, subtype, p_partition);
        if (error != ESP_OK) {
            ESP_LOGE(TAG, "Failed to register %s partition (err=0x%x)", "PrimaryBTLDR", error);
            return error;
        }
    }
    ESP_LOGI(TAG, "Use <%s> partition (0x%08" PRIx32 ")", (*p_partition)->label, (*p_partition)->address);
    return ESP_OK;
}

static esp_err_t ota_update_partitions(esp_https_ota_config_t *ota_config)
{
    esp_err_t ret = ESP_ERR_NOT_SUPPORTED;
    if (strstr(ota_config->http_config->url, "partitions_ota.bin") != NULL) {
        ret = esp_https_ota(ota_config);

    } else if (strstr(ota_config->http_config->url, "bootloader.bin") != NULL) {
        const esp_partition_t *primary_bootloader;
        ESP_ERROR_CHECK(register_partition(ESP_PRIMARY_BOOTLOADER_OFFSET, ESP_BOOTLOADER_SIZE, "PrimaryBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_PRIMARY, &primary_bootloader));
        const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL); // free app ota partition will be used for downloading a new image
#if CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE
        // Check if the passive OTA app partition is not needed for rollback before using it for other partitions.
        // The same can be done for partition table and storage updates.
        esp_ota_img_states_t ota_state;
        ESP_ERROR_CHECK(esp_ota_get_state_partition(ota_partition, &ota_state));
        if (ota_state == ESP_OTA_IMG_VALID) {
            ESP_LOGW(TAG, "Passive OTA app partition <%s> contains a valid app image eligible for rollback.", ota_partition->label);
            uint32_t ota_bootloader_offset;
            ESP_ERROR_CHECK(partition_utils_find_unallocated(NULL, ESP_BOOTLOADER_SIZE, ESP_PARTITION_TABLE_OFFSET + ESP_PARTITION_TABLE_SIZE, &ota_bootloader_offset, NULL));
            ESP_ERROR_CHECK(register_partition(ota_bootloader_offset, ESP_BOOTLOADER_SIZE, "OtaBTLDR", ESP_PARTITION_TYPE_BOOTLOADER, ESP_PARTITION_SUBTYPE_BOOTLOADER_OTA, &ota_partition));
            ESP_LOGW(TAG, "To avoid overwriting the passive app partition, using the unallocated space on the flash to create a temporary OTA bootloader partition <%s>", ota_partition->label);
        }
#endif
        ota_config->partition.staging = ota_partition;
        ota_config->partition.final = primary_bootloader;
        ret = esp_https_ota(ota_config);
        if (ret == ESP_OK) {
            ESP_LOGW(TAG, "Ensure stable power supply! Loss of power at this stage leads to a chip bricking");
            ESP_LOGI(TAG, "Copy from <%s> staging partition to <%s>...", ota_partition->label, primary_bootloader->label);
            ret = esp_partition_copy(primary_bootloader, 0, ota_partition, 0, primary_bootloader->size);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to copy partition to Primary bootloader (err=0x%x). Bootloader likely corrupted. Device will not be able to boot again!", ret);
            }
            // If the primary_bootloader already exists in the partition table on flash, it will not be deregistered, and the function will return an error.
            esp_partition_deregister_external(primary_bootloader);
        }

    } else if (strstr(ota_config->http_config->url, "partition-table.bin") != NULL) {
        const esp_partition_t *primary_partition_table;
        ESP_ERROR_CHECK(register_partition(ESP_PRIMARY_PARTITION_TABLE_OFFSET, ESP_PARTITION_TABLE_SIZE, "PrimaryPrtTable", ESP_PARTITION_TYPE_PARTITION_TABLE, ESP_PARTITION_SUBTYPE_PARTITION_TABLE_PRIMARY, &primary_partition_table));
        const esp_partition_t *free_app_ota_partition = esp_ota_get_next_update_partition(NULL); // free app ota partition will be used for downloading a new image
        ota_config->partition.staging = free_app_ota_partition;
        ota_config->partition.final = primary_partition_table;
        ota_config->partition.finalize_with_copy = false; // After the download is complete, do not copy the received image to the final partition automatically (it is false by default)
        ret = esp_https_ota(ota_config);
        if (ret == ESP_OK) {
            ESP_LOGW(TAG, "Ensure stable power supply! Loss of power at this stage leads to a chip bricking.");
            ESP_LOGI(TAG, "Copy from <%s> staging partition to <%s>...", free_app_ota_partition->label, primary_partition_table->label);
            ret = esp_partition_copy(primary_partition_table, 0, free_app_ota_partition, 0, primary_partition_table->size);
            if (ret != ESP_OK) {
                ESP_LOGE(TAG, "Failed to copy partition to Primary partition table (err=0x%x). Partition table likely corrupted. Device will not be able to boot again!", ret);
            }
            // If the primary_partition_table already exists in the partition table on flash, it will not be deregistered, and the function will return an error.
            esp_partition_deregister_external(primary_partition_table);
        }

    } else if (strstr(ota_config->http_config->url, "storage.bin") != NULL) {
        ota_config->partition.staging = NULL; // free app ota partition will be selected and used for downloading a new image
        ota_config->partition.final = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "storage");
        assert(ota_config->partition.final != NULL);
        ota_config->partition.finalize_with_copy = true; // After the download is complete, copy the received image to the final partition automatically
        ret = esp_https_ota(ota_config);
        char text[16];
        ESP_ERROR_CHECK(esp_partition_read(ota_config->partition.final, 0, text, sizeof(text)));
        ESP_LOG_BUFFER_CHAR(TAG, text, sizeof(text));
        assert(memcmp("7296406769363431", text, sizeof(text)) == 0);

    } else {
        ESP_LOGE(TAG, "Unable to load this file (%s). The final partition is unknown.", ota_config->http_config->url);
    }
    return ret;
}

void ota_example_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting OTA example task");
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
    esp_netif_t *netif = get_example_netif_from_desc(bind_interface_name);
    if (netif == NULL) {
        ESP_LOGE(TAG, "Can't find netif from interface description");
        abort();
    }
    struct ifreq ifr;
    esp_netif_get_netif_impl_name(netif, ifr.ifr_name);
    ESP_LOGI(TAG, "Bind interface name is %s", ifr.ifr_name);
#endif
    esp_http_client_config_t config = {
        .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
#ifdef CONFIG_EXAMPLE_USE_CERT_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach,
#else
        .cert_pem = (char *)server_cert_pem_start,
#endif /* CONFIG_EXAMPLE_USE_CERT_BUNDLE */
        .event_handler = _http_event_handler,
        .keep_alive_enable = true,
#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_BIND_IF
        .if_name = &ifr,
#endif
    };

#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL_FROM_STDIN
    char url_buf[OTA_URL_SIZE];
    if (strcmp(config.url, "FROM_STDIN") == 0) {
        example_configure_stdin_stdout();
        fgets(url_buf, OTA_URL_SIZE, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        config.url = url_buf;
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong firmware upgrade image url");
        abort();
    }
#endif

#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
#endif

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
    };
    ESP_LOGI(TAG, "Attempting to download update from %s", config.url);
    esp_err_t ret = ota_update_partitions(&ota_config);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "OTA Succeed, Rebooting...");
        esp_restart();
    } else {
        ESP_LOGE(TAG, "Firmware upgrade failed");
    }
    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "OTA example app_main start");
    // Initialize NVS.
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // 1.OTA app partition table has a smaller NVS partition size than the non-OTA
        // partition table. This size mismatch may cause NVS initialization to fail.
        // 2.NVS partition contains data in new format and cannot be recognized by this version of code.
        // If this happens, we erase NVS partition and initialize NVS again.
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

#if CONFIG_EXAMPLE_CONNECT_WIFI
    /* Ensure to disable any WiFi power save mode, this allows best throughput
     * and hence timings for overall OTA operation.
     */
    esp_wifi_set_ps(WIFI_PS_NONE);
#endif // CONFIG_EXAMPLE_CONNECT_WIFI

    xTaskCreate(&ota_example_task, "ota_example_task", 8192, NULL, 5, NULL);
}
