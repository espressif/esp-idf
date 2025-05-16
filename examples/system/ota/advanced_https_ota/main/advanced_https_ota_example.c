/* Advanced HTTPS OTA example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_ota_ops.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"


#ifdef CONFIG_EXAMPLE_USE_CERT_BUNDLE
#include "esp_crt_bundle.h"
#endif

#if CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
#include "esp_efuse.h"
#endif

#if CONFIG_EXAMPLE_CONNECT_WIFI
#include "esp_wifi.h"
#endif

#if CONFIG_BT_BLE_ENABLED || CONFIG_BT_NIMBLE_ENABLED
#include "ble_api.h"
#endif

static const char *TAG = "advanced_https_ota_example";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

#define OTA_URL_SIZE 256

#ifdef CONFIG_EXAMPLE_ENABLE_OTA_RESUMPTION

#define NVS_NAMESPACE_OTA_RESUMPTION  "ota_resumption"
#define NVS_KEY_OTA_WR_LENGTH  "nvs_ota_wr_len"
#define NVS_KEY_SAVED_URL  "nvs_ota_url"

static esp_err_t example_ota_res_get_ota_written_len_from_nvs(const nvs_handle_t nvs_ota_resumption_handle, const char *client_ota_url, uint32_t *nvs_ota_wr_len)
{
    esp_err_t err;
    char saved_url[OTA_URL_SIZE] = {0};
    size_t url_len = sizeof(saved_url);

    *nvs_ota_wr_len = 0;

    // Retrieve the saved URL from NVS
    err = nvs_get_str(nvs_ota_resumption_handle, NVS_KEY_SAVED_URL, saved_url, &url_len);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGD(TAG, "Saved URL is not initialized yet!");
        return err;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading saved URL (%s)", esp_err_to_name(err));
        return err;
    }

    // Compare the current URL with the saved URL
    if (strcmp(client_ota_url, saved_url) != 0) {
        ESP_LOGD(TAG, "URLs do not match. Restarting OTA from beginning.");
        return ESP_ERR_INVALID_STATE;
    }

    // Fetch the saved write length only if URLs match
    uint16_t saved_wr_len_kb = 0;
    err = nvs_get_u16(nvs_ota_resumption_handle, NVS_KEY_OTA_WR_LENGTH, &saved_wr_len_kb);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGD(TAG, "The write length is not initialized yet!");
        *nvs_ota_wr_len = 0;
        return err;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error reading OTA write length (%s)", esp_err_to_name(err));
        return err;
    }

    // Convert the saved value back to bytes
    *nvs_ota_wr_len = saved_wr_len_kb * 1024;

    return ESP_OK;
}

static esp_err_t example_ota_res_save_ota_cfg_to_nvs(const nvs_handle_t nvs_ota_resumption_handle, int nvs_ota_wr_len,  const char *client_ota_url)
{
    // Convert the write length to kilobytes to optimize NVS space utilization
    uint16_t wr_len_kb = nvs_ota_wr_len / 1024;

    // Save the current OTA write length to NVS
    ESP_RETURN_ON_ERROR(nvs_set_u16(nvs_ota_resumption_handle, NVS_KEY_OTA_WR_LENGTH, wr_len_kb), TAG, "Failed to set OTA write length");

    // Save the URL only if the OTA write length is non-zero and the URL is not already saved
    if (nvs_ota_wr_len) {
        char saved_url[OTA_URL_SIZE] = {0};
        size_t url_len = sizeof(saved_url);

        esp_err_t err = nvs_get_str(nvs_ota_resumption_handle, NVS_KEY_SAVED_URL, saved_url, &url_len);
        if (err == ESP_ERR_NVS_NOT_FOUND || strcmp(saved_url, client_ota_url) != 0) {
            // URL not saved or changed; save it now
            ESP_RETURN_ON_ERROR(nvs_set_str(nvs_ota_resumption_handle, NVS_KEY_SAVED_URL, client_ota_url), TAG, "Failed to set URL in NVS");
        } else if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error reading OTA URL");
            return err;
        }
    }

    ESP_RETURN_ON_ERROR(nvs_commit(nvs_ota_resumption_handle), TAG, "Failed to commit NVS");
    ESP_LOGD(TAG, "Saving state in NVS. Total image written so far : %d KB", wr_len_kb);
    return ESP_OK;
}

static esp_err_t example_ota_res_cleanup_ota_cfg_from_nvs(nvs_handle_t handle) {
    esp_err_t ret;

    // Erase all keys in the NVS handle and commit changes
    ESP_GOTO_ON_ERROR(nvs_erase_all(handle), err, TAG, "Error in erasing NVS");
    ESP_GOTO_ON_ERROR(nvs_commit(handle), err, TAG, "Error in committing NVS");
    ret = ESP_OK;
err:
    nvs_close(handle);
    return ret;
}

#endif

/* Event handler for catching system events */
static void event_handler(void* arg, esp_event_base_t event_base,
                          int32_t event_id, void* event_data)
{
    if (event_base == ESP_HTTPS_OTA_EVENT) {
        switch (event_id) {
            case ESP_HTTPS_OTA_START:
                ESP_LOGI(TAG, "OTA started");
                break;
            case ESP_HTTPS_OTA_CONNECTED:
                ESP_LOGI(TAG, "Connected to server");
                break;
            case ESP_HTTPS_OTA_GET_IMG_DESC:
                ESP_LOGI(TAG, "Reading Image Description");
                break;
            case ESP_HTTPS_OTA_VERIFY_CHIP_ID:
                ESP_LOGI(TAG, "Verifying chip id of new image: %d", *(esp_chip_id_t *)event_data);
                break;
            case ESP_HTTPS_OTA_VERIFY_CHIP_REVISION:
                ESP_LOGI(TAG, "Verifying chip revision of new image: %d", *(esp_chip_id_t *)event_data);
                break;
            case ESP_HTTPS_OTA_DECRYPT_CB:
                ESP_LOGI(TAG, "Callback to decrypt function");
                break;
            case ESP_HTTPS_OTA_WRITE_FLASH:
                ESP_LOGD(TAG, "Writing to flash: %d written", *(int *)event_data);
                break;
            case ESP_HTTPS_OTA_UPDATE_BOOT_PARTITION:
                ESP_LOGI(TAG, "Boot partition updated. Next Partition: %d", *(esp_partition_subtype_t *)event_data);
                break;
            case ESP_HTTPS_OTA_FINISH:
                ESP_LOGI(TAG, "OTA finish");
                break;
            case ESP_HTTPS_OTA_ABORT:
                ESP_LOGI(TAG, "OTA abort");
                break;
        }
    }
}

static esp_err_t validate_image_header(esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGI(TAG, "Running firmware version: %s", running_app_info.version);
    }

#ifndef CONFIG_EXAMPLE_SKIP_VERSION_CHECK
    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0) {
        ESP_LOGW(TAG, "Current running version is the same as a new. We will not continue the update.");
        return ESP_FAIL;
    }
#endif

#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    /**
     * Secure version check from firmware image header prevents subsequent download and flash write of
     * entire firmware image. However this is optional because it is also taken care in API
     * esp_https_ota_finish at the end of OTA update procedure.
     */
    const uint32_t hw_sec_version = esp_efuse_read_secure_version();
    if (new_app_info->secure_version < hw_sec_version) {
        ESP_LOGW(TAG, "New firmware security version is less than eFuse programmed, %"PRIu32" < %"PRIu32, new_app_info->secure_version, hw_sec_version);
        return ESP_FAIL;
    }
#endif

    return ESP_OK;
}

static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client)
{
    esp_err_t err = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
    return err;
}

void advanced_ota_example_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Starting Advanced OTA example");

    esp_err_t err;
    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL,
#ifdef CONFIG_EXAMPLE_USE_CERT_BUNDLE
        .crt_bundle_attach = esp_crt_bundle_attach,
#else
        .cert_pem = (char *)server_cert_pem_start,
#endif
        .timeout_ms = CONFIG_EXAMPLE_OTA_RECV_TIMEOUT,
        .keep_alive_enable = true,
#ifdef CONFIG_EXAMPLE_ENABLE_PARTIAL_HTTP_DOWNLOAD
        .save_client_session = true,
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

#ifdef CONFIG_EXAMPLE_ENABLE_OTA_RESUMPTION
    nvs_handle_t nvs_ota_resumption_handle;
    err = nvs_open(NVS_NAMESPACE_OTA_RESUMPTION, NVS_READWRITE, &nvs_ota_resumption_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        vTaskDelete(NULL);
    }

    uint32_t ota_wr_len = 0; // Variable to hold the written length
    err = example_ota_res_get_ota_written_len_from_nvs(nvs_ota_resumption_handle, config.url, &ota_wr_len);
    if (err != ESP_OK) {
        ESP_LOGD(TAG, "Starting OTA from beginning");
    } else {
        ESP_LOGD(TAG, "OTA write length fetched successfully");
    }
#endif

    esp_https_ota_config_t ota_config = {
        .http_config = &config,
        .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
#ifdef CONFIG_EXAMPLE_ENABLE_PARTIAL_HTTP_DOWNLOAD
        .partial_http_download = true,
        .max_http_request_size = CONFIG_EXAMPLE_HTTP_REQUEST_SIZE,
#endif
#ifdef CONFIG_EXAMPLE_ENABLE_OTA_RESUMPTION
        .ota_resumption = true,
        .ota_image_bytes_written = ota_wr_len,
#endif
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
        vTaskDelete(NULL);
    }

    esp_app_desc_t app_desc = {};
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_https_ota_get_img_desc failed");
        goto ota_end;
    }
    err = validate_image_header(&app_desc);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "image header verification failed");
        goto ota_end;
    }

    while (1) {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
        // esp_https_ota_perform returns after every read operation which gives user the ability to
        // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
        // data read so far.
        const size_t len = esp_https_ota_get_image_len_read(https_ota_handle);
        ESP_LOGD(TAG, "Image bytes read: %d", len);
#ifdef CONFIG_EXAMPLE_ENABLE_OTA_RESUMPTION
        err = example_ota_res_save_ota_cfg_to_nvs(nvs_ota_resumption_handle, len, config.url);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to save OTA config to NVS (%s).", esp_err_to_name(err));
        }
#endif
    }

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true) {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data was not received.");
    } else {
#ifdef CONFIG_EXAMPLE_ENABLE_OTA_RESUMPTION
        err = example_ota_res_cleanup_ota_cfg_from_nvs(nvs_ota_resumption_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Failed to clean up OTA config from NVS (%s).", esp_err_to_name(err));
        }
#endif
        ota_finish_err = esp_https_ota_finish(https_ota_handle);
        if ((err == ESP_OK) && (ota_finish_err == ESP_OK)) {
            ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
        } else {
            if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED) {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            }
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota_finish_err);
            vTaskDelete(NULL);
        }
    }

ota_end:
    esp_https_ota_abort(https_ota_handle);
    ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed");
    vTaskDelete(NULL);
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
    ESP_ERROR_CHECK( err );

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(esp_event_handler_register(ESP_HTTPS_OTA_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL));
    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
    */
    ESP_ERROR_CHECK(example_connect());

#if defined(CONFIG_BOOTLOADER_APP_ROLLBACK_ENABLE)
    /**
     * We are treating successful WiFi connection as a checkpoint to cancel rollback
     * process and mark newly updated firmware image as active. For production cases,
     * please tune the checkpoint behavior per end application requirement.
     */
    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_ota_img_states_t ota_state;
    if (esp_ota_get_state_partition(running, &ota_state) == ESP_OK) {
        if (ota_state == ESP_OTA_IMG_PENDING_VERIFY) {
            if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
                ESP_LOGI(TAG, "App is valid, rollback cancelled successfully");
            } else {
                ESP_LOGE(TAG, "Failed to cancel rollback");
            }
        }
    }
#endif

#if CONFIG_EXAMPLE_CONNECT_WIFI
#if !CONFIG_BT_ENABLED
    /* Ensure to disable any WiFi power save mode, this allows best throughput
     * and hence timings for overall OTA operation.
     */
    esp_wifi_set_ps(WIFI_PS_NONE);
#else
    /* WIFI_PS_MIN_MODEM is the default mode for WiFi Power saving. When both
     * WiFi and Bluetooth are running, WiFI modem has to go down, hence we
     * need WIFI_PS_MIN_MODEM. And as WiFi modem goes down, OTA download time
     * increases.
     */
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
#endif // CONFIG_BT_ENABLED
#endif // CONFIG_EXAMPLE_CONNECT_WIFI

#if CONFIG_BT_CONTROLLER_ENABLED && (CONFIG_BT_BLE_ENABLED || CONFIG_BT_NIMBLE_ENABLED)
    ESP_ERROR_CHECK(esp_ble_helper_init());
#endif

    xTaskCreate(&advanced_ota_example_task, "advanced_ota_example_task", 1024 * 8, NULL, 5, NULL);
}
