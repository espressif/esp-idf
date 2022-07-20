/* MQTT publish test

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "sdkconfig.h"

static const char *TAG = "PUBLISH_TEST";

static EventGroupHandle_t mqtt_event_group;
const static int CONNECTED_BIT = BIT0;

static esp_mqtt_client_handle_t mqtt_client = NULL;

static char *expected_data = NULL;
static char *actual_data = NULL;
static size_t expected_size = 0;
static size_t expected_published = 0;
static size_t actual_published = 0;
static int qos_test = 0;

#if CONFIG_EXAMPLE_BROKER_CERTIFICATE_OVERRIDDEN == 1
static const uint8_t mqtt_eclipseprojects_io_pem_start[]  = "-----BEGIN CERTIFICATE-----\n" CONFIG_EXAMPLE_BROKER_CERTIFICATE_OVERRIDE "\n-----END CERTIFICATE-----";
#else
extern const uint8_t mqtt_eclipseprojects_io_pem_start[]   asm("_binary_mqtt_eclipseprojects_io_pem_start");
#endif
extern const uint8_t mqtt_eclipseprojects_io_pem_end[]   asm("_binary_mqtt_eclipseprojects_io_pem_end");

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    static int msg_id = 0;
    static int actual_len = 0;
    switch (event->event_id) {
    case MQTT_EVENT_BEFORE_CONNECT:
        break;
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xEventGroupSetBits(mqtt_event_group, CONNECTED_BIT);
        msg_id = esp_mqtt_client_subscribe(client, CONFIG_EXAMPLE_SUBSCRIBE_TOPIC, qos_test);
        ESP_LOGI(TAG, "sent subscribe successful %s , msg_id=%d", CONFIG_EXAMPLE_SUBSCRIBE_TOPIC, msg_id);

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
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        printf("ID=%d, total_len=%d, data_len=%d, current_data_offset=%d\n", event->msg_id, event->total_data_len, event->data_len, event->current_data_offset);
        if (event->topic) {
            actual_len = event->data_len;
            msg_id = event->msg_id;
        } else {
            actual_len += event->data_len;
            // check consistency with msg_id across multiple data events for single msg
            if (msg_id != event->msg_id) {
                ESP_LOGI(TAG, "Wrong msg_id in chunked message %d != %d", msg_id, event->msg_id);
                abort();
            }
        }
        memcpy(actual_data + event->current_data_offset, event->data, event->data_len);
        if (actual_len == event->total_data_len) {
            if (0 == memcmp(actual_data, expected_data, expected_size)) {
                printf("OK!");
                memset(actual_data, 0, expected_size);
                actual_published ++;
                if (actual_published == expected_published) {
                    printf("Correct pattern received exactly x times\n");
                    ESP_LOGI(TAG, "Test finished correctly!");
                }
            } else {
                printf("FAILED!");
                abort();
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

typedef enum {NONE, TCP, SSL, WS, WSS} transport_t;
static transport_t current_transport;


void test_init(void)
{
    mqtt_event_group = xEventGroupCreate();
    esp_mqtt_client_config_t config = {0};
    mqtt_client = esp_mqtt_client_init(&config);
    current_transport = NONE;
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
}

void pattern_setup(char *pattern, int repeat)
{
    int pattern_size = strlen(pattern);
    free(expected_data);
    free(actual_data);
    actual_published = 0;
    expected_size = pattern_size * repeat;
    expected_data = malloc(expected_size);
    actual_data = malloc(expected_size);
    for (int i = 0; i < repeat; i++) {
        memcpy(expected_data + i * pattern_size, pattern, pattern_size);
    }
    printf("EXPECTED STRING %.*s, SIZE:%d\n", expected_size, expected_data, expected_size);
}

static void configure_client(char *transport)
{
    ESP_LOGI(TAG, "Configuration");
    transport_t selected_transport;
    if (0 == strcmp(transport, "tcp")) {
        selected_transport = TCP;
    } else if (0 == strcmp(transport, "ssl")) {
        selected_transport = SSL;
    } else if (0 == strcmp(transport, "ws")) {
        selected_transport = WS;
    } else if (0 == strcmp(transport, "wss")) {
        selected_transport = WSS;
    } else {
        ESP_LOGE(TAG, "Unexpected transport %s", transport);
        abort();
    }


    if (selected_transport != current_transport) {
        esp_mqtt_client_config_t config = {0};
        switch (selected_transport) {
        case NONE:
            break;
        case TCP:
            ESP_LOGI(TAG, "[TCP transport] Startup..");
            config.broker.address.uri = CONFIG_EXAMPLE_BROKER_TCP_URI;
            break;
        case SSL:
            ESP_LOGI(TAG, "[SSL transport] Startup..");
            config.broker.address.uri = CONFIG_EXAMPLE_BROKER_SSL_URI;
            break;
        case WS:
            ESP_LOGI(TAG, "[WS transport] Startup..");
            config.broker.address.uri = CONFIG_EXAMPLE_BROKER_WS_URI;
            break;
        case WSS:
            ESP_LOGI(TAG, "[WSS transport] Startup..");
            config.broker.address.uri = CONFIG_EXAMPLE_BROKER_WSS_URI;
            break;
        }
        if (selected_transport == SSL || selected_transport == WSS) {
            ESP_LOGI(TAG, "Set certificate");
            config.broker.verification.certificate = (const char *)mqtt_eclipseprojects_io_pem_start;
        }
        esp_mqtt_set_config(mqtt_client, &config);

    }

}
void publish_test(const char *line)
{
    char pattern[32];
    char transport[32];
    int repeat = 0;
    int enqueue = 0;

    static bool is_test_init = false;
    if (!is_test_init) {
        test_init();
        is_test_init = true;
    } else {
        esp_mqtt_client_stop(mqtt_client);
    }

    sscanf(line, "%s %s %d %d %d %d", transport, pattern, &repeat, &expected_published, &qos_test, &enqueue);
    ESP_LOGI(TAG, "PATTERN:%s REPEATED:%d PUBLISHED:%d\n", pattern, repeat, expected_published);
    pattern_setup(pattern, repeat);
    xEventGroupClearBits(mqtt_event_group, CONNECTED_BIT);
    configure_client(transport);
    esp_mqtt_client_start(mqtt_client);

    ESP_LOGI(TAG, "Note free memory: %d bytes", esp_get_free_heap_size());

    xEventGroupWaitBits(mqtt_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    for (int i = 0; i < expected_published; i++) {
        int msg_id;
        if (enqueue) {
            msg_id = esp_mqtt_client_enqueue(mqtt_client, CONFIG_EXAMPLE_PUBLISH_TOPIC, expected_data, expected_size, qos_test, 0, true);
        } else {
            msg_id = esp_mqtt_client_publish(mqtt_client, CONFIG_EXAMPLE_PUBLISH_TOPIC, expected_data, expected_size, qos_test, 0);
        }
        ESP_LOGI(TAG, "[%d] Publishing...", msg_id);
    }
}
