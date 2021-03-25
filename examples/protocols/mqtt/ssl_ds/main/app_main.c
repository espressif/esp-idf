/* MQTT Mutual Authentication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "rsa_sign_alt.h"

/* pre_prov - name of partition containing encrypted prv key parameters ( It is set as such to synchronize it with the pre provisioning service */
#define NVS_PARTITION_NAME  "pre_prov"
/* esp_ds_ns - namespace used for defining values in esp_ds_nvs */
#define NVS_NAMESPACE       "esp_ds_ns"
/* esp_ds_key_id - efuse key block id where 256 bit key is stored, which will be read by
     * DS module to perform DS operation */
#define NVS_EFUSE_KEY_ID    "esp_ds_key_id"
/* esp_ds_rsa_len - length of RSA private key (in bits) which is encrypted */
#define NVS_RSA_LEN         "esp_ds_rsa_len"
/* following entries denote key(ASCII string) for particular value in key-value pair of esp_ds_nvs (which are defined in esp_ds_ns) */
/* ciphertext_c - encrypted RSA private key, see ESP32-S2 Techincal Reference Manual for more details */
#define NVS_CIPHER_C        "esp_ds_c"
/* initialization vector (iv) - 256 bit value used to encrypt RSA private key (to generate ciphertext_c) */
#define NVS_IV              "esp_ds_iv"
static const char *TAG = "MQTTS_EXAMPLE";

extern const uint8_t client_cert_pem_start[] asm("_binary_client_crt_start");
extern const uint8_t client_cert_pem_end[] asm("_binary_client_crt_end");
extern const uint8_t server_cert_pem_start[] asm("_binary_mosquitto_org_crt_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_mosquitto_org_crt_end");

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    // your_context_t *context = event->context;
    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
        ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
    return ESP_OK;
}

void *esp_read_ds_data_from_nvs(void)
{
    esp_ds_data_ctx_t *ds_data_ctx;
    ds_data_ctx = (esp_ds_data_ctx_t *)malloc(sizeof(esp_ds_data_ctx_t));
    if (ds_data_ctx == NULL) {
        ESP_LOGE(TAG, "Error in allocating memory for esp_ds_data_context");
        goto exit;
    }

    ds_data_ctx->esp_ds_data = (esp_ds_data_t *)calloc(1, sizeof(esp_ds_data_t));
    if (ds_data_ctx->esp_ds_data == NULL) {
        ESP_LOGE(TAG, "Could not allocate memory for DS data handle ");
        goto exit;
    }

    nvs_handle_t esp_ds_nvs_handle;
    esp_err_t esp_ret;
    esp_ret = nvs_flash_init_partition(NVS_PARTITION_NAME);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in esp_ds_nvs partition init,\nreturned %02x (%s)", esp_ret, esp_err_to_name(esp_ret));
        goto exit;
    }

    esp_ret = nvs_open_from_partition(NVS_PARTITION_NAME, NVS_NAMESPACE,
                                      NVS_READONLY, &esp_ds_nvs_handle);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in esp_ds_nvs partition open,\nreturned %02x (%s)", esp_ret, esp_err_to_name(esp_ret));
        goto exit;
    }

    esp_ret = nvs_get_u8(esp_ds_nvs_handle, NVS_EFUSE_KEY_ID, &ds_data_ctx->efuse_key_id);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in efuse_key_id value from nvs,\nreturned %02x (%s)", esp_ret, esp_err_to_name(esp_ret));
        goto exit;
    }

    esp_ret = nvs_get_u16(esp_ds_nvs_handle, NVS_RSA_LEN, &ds_data_ctx->rsa_length_bits);
    if (esp_ret != ESP_OK) {
        ESP_LOGE(TAG, "Error in reading rsa key length value from nvs,\nreturned %02x (%s)", esp_ret, esp_err_to_name(esp_ret));
        goto exit;
    }

    size_t blob_length = ESP_DS_C_LEN;
    esp_ret = nvs_get_blob(esp_ds_nvs_handle, NVS_CIPHER_C, (void *)(ds_data_ctx->esp_ds_data->c), &blob_length);
    if ((esp_ret != ESP_OK) || (blob_length != ESP_DS_C_LEN)) {
        ESP_LOGE(TAG, "Error in reading ciphertext_c value from nvs,bytes_read = %d,\nreturned %02x (%s)", blob_length, esp_ret, esp_err_to_name(esp_ret));
        goto exit;
    }

    blob_length = ESP_DS_IV_LEN;
    esp_ret = nvs_get_blob(esp_ds_nvs_handle, NVS_IV, (void *)(ds_data_ctx->esp_ds_data->iv), &blob_length);
    if ((esp_ret != ESP_OK) || (blob_length != ESP_DS_IV_LEN)) {
        ESP_LOGE(TAG, "Error in reading initialization vector value from nvs,bytes_read = %d,\nreturned %02x (%s)", blob_length, esp_ret, esp_err_to_name(esp_ret));
        goto exit;
    }

    return (void *)ds_data_ctx;
exit:
    if (ds_data_ctx != NULL) {
        free(ds_data_ctx->esp_ds_data);
    }
    free(ds_data_ctx);
    return NULL;
}

static void mqtt_app_start(void)
{

    /* The context is used by the DS peripheral, should not be freed */
    void *ds_data = esp_read_ds_data_from_nvs();
    if (ds_data == NULL) {
        ESP_LOGE(TAG, "Error in reading DS data from NVS");
        vTaskDelete(NULL);
    }
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://test.mosquitto.org:8884",
        .event_handle = mqtt_event_handler,
        .cert_pem =  (const char *)server_cert_pem_start,
        .client_cert_pem = (const char *)client_cert_pem_start,
        .client_key_pem = NULL,
        .ds_data = ds_data,
    };

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT", ESP_LOG_VERBOSE);
    esp_log_level_set("OUTBOX", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    mqtt_app_start();
}
