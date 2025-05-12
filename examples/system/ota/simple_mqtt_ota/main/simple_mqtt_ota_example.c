/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "mqtt_client.h"
#include <string.h>
#include "protocol_examples_common.h"

#define MQTT_BROKER_URI CONFIG_MQTT_BROKER_URI
#define TOPIC_FIRMWARE CONFIG_MQTT_TOPIC_FIRMWARE
#define TOPIC_START_UPDATE CONFIG_MQTT_TOPIC_START_UPDATE
#define TOPIC_DONE CONFIG_MQTT_TOPIC_DONE

#define HASH_LEN 32

static const char *TAG = "simple_mqtt_ota_example";
size_t total_bytes_written = 0;

// Global OTA handle
esp_ota_handle_t ota_handle = 0;
bool ota_in_progress = false;
bool ota_data_written = false;
esp_mqtt_client_handle_t mqtt_client = NULL;
uint64_t timeoutotainit = 0;

/**
 * @brief Start the OTA update process by selecting the update partition and
 * initializing OTA handle.
 *
 * This function ensures OTA is not already in progress before beginning.
 *
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t start_ota(void)

{
    if (ota_in_progress)
    {
        ESP_LOGW(TAG, "OTA already in progress, ignoring start command.");
        return ESP_OK;
    }
    // Get the next available OTA partition (where the firmware will be written)
    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);

    if (update_partition == NULL)
    {
        ESP_LOGE(TAG, "Failed to get update partition");
        return ESP_ERR_NOT_FOUND;
    }

    // Start OTA (this opens the OTA partition for writing)
    esp_err_t err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to start OTA: %s", esp_err_to_name(err));
        return err;
    }

    ota_in_progress = true;
    ota_data_written = false;
    ESP_LOGI(TAG, "OTA started. Ready to receive firmware.");
    return ESP_OK;
}

/**
 * @brief Finalize the OTA process and reboot if successful.
 *
 * Verifies that data has been written and completes the OTA process. If successful,
 * sets the boot partition and restarts the device. Sends MQTT feedback on the result.
 *
 * @return esp_err_t ESP_OK on success, error code otherwise.
 */
esp_err_t finalize_ota(void)

{
    int msg_id = 0;
    char buff[60];
    if (!ota_in_progress || !ota_data_written)
    {
        ESP_LOGE(TAG, "OTA not properly started or no data written");
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_START_UPDATE, "ota not properly started", 0, 1, 0);
        ESP_LOGD(TAG, "sent publish successful, msg_id=%d", msg_id);
        return ESP_FAIL;
    }

    esp_err_t err = esp_ota_end(ota_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_end failed: %s, bytes written: %d", esp_err_to_name(err),total_bytes_written);
        snprintf(buff,sizeof(buff),"esp_ota_end failed: %s", esp_err_to_name(err));
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_START_UPDATE, "OTA failed", 0, 1, 0);
        ESP_LOGD(TAG, "sent publish successful, msg_id=%d", msg_id);
        return err;
    }

    const esp_partition_t *update_partition = esp_ota_get_next_update_partition(NULL);
    err = esp_ota_set_boot_partition(update_partition);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_ota_set_boot_partition failed: %s", esp_err_to_name(err));
        snprintf(buff,sizeof(buff),"esp_ota_set_boot_partition failed: %s, bytes written: %d", esp_err_to_name(err),total_bytes_written);
        msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_START_UPDATE, "OTA set_boot fails", 0, 1, 0);
        ESP_LOGD(TAG, "sent publish successful, msg_id=%d", msg_id);
        return err;
    }
    uint16_t otaTimeDelta = ((esp_timer_get_time() - timeoutotainit) / 1000000);
    snprintf(buff,sizeof(buff),"OTA successful time: %ds, bytes written: %d",otaTimeDelta,total_bytes_written);
    msg_id = esp_mqtt_client_publish(mqtt_client, TOPIC_START_UPDATE, buff, 0, 1, 0);
    ESP_LOGD(TAG, "sent publish successful, msg_id=%d", msg_id);
    vTaskDelay(500);
    ESP_LOGI(TAG, "OTA update successful. Rebooting...");
    esp_restart();
    return ESP_OK;
}

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

/**
 * @brief Event handler registered to receive MQTT events
 *
 * This function processes MQTT events like connection, disconnection, data received, and errors.
 * It responds to subscribed topics to manage the OTA update lifecycle.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    int msg_id = 0;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT connected to broker: %s",MQTT_BROKER_URI);
        msg_id = esp_mqtt_client_subscribe(mqtt_client, TOPIC_START_UPDATE, 0);
        ESP_LOGI(TAG, "Subscribed to topic '%s', msg_id=%d", TOPIC_START_UPDATE, msg_id);
        msg_id = esp_mqtt_client_subscribe(mqtt_client, TOPIC_DONE, 0);
        ESP_LOGI(TAG, "Subscribed to topic '%s', msg_id=%d", TOPIC_DONE, msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT received data on topic %.*s", event->topic_len, event->topic);

        if (event->topic_len == strlen(TOPIC_START_UPDATE) &&
            strncmp(event->topic, TOPIC_START_UPDATE, event->topic_len) == 0)
        {
            timeoutotainit = esp_timer_get_time();
            ESP_LOGI(TAG, "Received firmware update command. Starting OTA...");
            msg_id = esp_mqtt_client_subscribe(mqtt_client, TOPIC_FIRMWARE, 0);
            ESP_LOGI(TAG, "Subscribed to topic '%s', msg_id=%d", TOPIC_FIRMWARE, msg_id);
            esp_err_t err = start_ota();
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "Failed to start OTA");
                return;
            }
        }

        else if (event->topic_len == strlen(TOPIC_FIRMWARE) &&
                 strncmp(event->topic, TOPIC_FIRMWARE, event->topic_len) == 0)
        {
            if (!ota_in_progress)
            {
                ESP_LOGE(TAG, "OTA not started yet. Ignoring firmware chunk.");
                return;
            }

            esp_err_t err = esp_ota_write(ota_handle, event->data, event->data_len);
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "OTA write failed: %s", esp_err_to_name(err));
                return;
            }
            ota_data_written = true;
            total_bytes_written += event->data_len;
            ESP_LOGD(TAG, "Received chunk: %d bytes", event->data_len);
            ESP_LOGI(TAG, "Bytes received: %d bytes", total_bytes_written);
        }

        else if (event->topic_len == strlen(TOPIC_DONE) &&
                 strncmp(event->topic, TOPIC_DONE, event->topic_len) == 0)
        {
            ESP_LOGI(TAG, "Received OTA DONE message, total size written: %d bytes",total_bytes_written);
            esp_err_t err = finalize_ota();
            if (err != ESP_OK)
            {
                ESP_LOGE(TAG, "OTA finalization failed: %s", esp_err_to_name(err));
            }
        }
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/**
 * @brief Start the MQTT client and register event handlers.
 *
 * Initializes MQTT using the configured broker URI and starts the client.
 */
void mqtt_app_start(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
    };
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_client));
}

/**
 * @brief Print SHA-256 hash of a firmware image.
 *
 * Formats and logs a hash using the provided label.
 *
 * @param image_hash Pointer to the 32-byte hash.
 * @param label Description of the hash being printed.
 */
static void print_sha256(const uint8_t *image_hash, const char *label)
{
    char hash_print[HASH_LEN * 2 + 1];
    hash_print[HASH_LEN * 2] = 0;
    for (int i = 0; i < HASH_LEN; ++i) {
        sprintf(&hash_print[i * 2], "%02x", image_hash[i]);
    }
    ESP_LOGI(TAG, "%s %s", label, hash_print);
}

/**
 * @brief Compute and log SHA-256 hashes of the bootloader and current firmware.
 */
static void get_sha256_of_partitions(void)
{
    uint8_t sha_256[HASH_LEN] = { 0 };
    esp_partition_t partition;

    // get sha256 digest for bootloader
    partition.address   = ESP_BOOTLOADER_OFFSET;
    partition.size      = ESP_PARTITION_TABLE_OFFSET;
    partition.type      = ESP_PARTITION_TYPE_APP;
    esp_partition_get_sha256(&partition, sha_256);
    print_sha256(sha_256, "SHA-256 for bootloader: ");

    // get sha256 digest for running partition
    esp_partition_get_sha256(esp_ota_get_running_partition(), sha_256);
    print_sha256(sha_256, "SHA-256 for current firmware: ");
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    ESP_LOGI(TAG, "Starting MQTT OTA example...");

    const esp_partition_t *running = esp_ota_get_running_partition();
    if (running != NULL)
    {
        ESP_LOGI(TAG, "Running partition type: %d subtype: %d at offset 0x%x",
                 running->type, running->subtype, running->address);
    }
    else
    {
        ESP_LOGE(TAG, "Failed to get running partition");
    }
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    get_sha256_of_partitions();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
    mqtt_app_start();
}
