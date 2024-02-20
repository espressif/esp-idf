/* MQTT publish test

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include <freertos/event_groups.h>
#include "esp_system.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "sdkconfig.h"
#include "publish_connect_test.h"

static const char *TAG = "publish_test";

static EventGroupHandle_t mqtt_event_group;
const static int CONNECTED_BIT = BIT0;

#if CONFIG_EXAMPLE_BROKER_CERTIFICATE_OVERRIDDEN == 1
static const uint8_t mqtt_eclipseprojects_io_pem_start[]  = "-----BEGIN CERTIFICATE-----\n" CONFIG_EXAMPLE_BROKER_CERTIFICATE_OVERRIDE "\n-----END CERTIFICATE-----";
#else
extern const uint8_t mqtt_eclipseprojects_io_pem_start[]   asm("_binary_mqtt_eclipseprojects_io_pem_start");
#endif
extern const uint8_t mqtt_eclipseprojects_io_pem_end[]   asm("_binary_mqtt_eclipseprojects_io_pem_end");

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    publish_context_t * test_data = handler_args;
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
        msg_id = esp_mqtt_client_subscribe(client, CONFIG_EXAMPLE_SUBSCRIBE_TOPIC, test_data->qos);
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
        ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "ID=%d, total_len=%d, data_len=%d, current_data_offset=%d", event->msg_id, event->total_data_len, event->data_len, event->current_data_offset);
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
        memcpy(test_data->received_data + event->current_data_offset, event->data, event->data_len);
        if (actual_len == event->total_data_len) {
            if (0 == memcmp(test_data->received_data, test_data->expected, test_data->expected_size)) {
                memset(test_data->received_data, 0, test_data->expected_size);
                test_data->nr_of_msg_received ++;
                if (test_data->nr_of_msg_received == test_data->nr_of_msg_expected) {
                    ESP_LOGI(TAG, "Correct pattern received exactly x times");
                    ESP_LOGI(TAG, "Test finished correctly!");
                }
            } else {
                ESP_LOGE(TAG, "FAILED!");
                abort();
            }
        }
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}



void test_init(void)
{
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
}

void pattern_setup(publish_context_t * test_data)
{
    int pattern_size = strlen(test_data->pattern);
    free(test_data->expected);
    free(test_data->received_data);
    test_data->nr_of_msg_received = 0;
    test_data->expected_size = (size_t)(pattern_size) * test_data->pattern_repetitions;
    test_data->expected = malloc(test_data->expected_size);
    test_data->received_data = malloc(test_data->expected_size);
    for (int i = 0; i < test_data->pattern_repetitions; i++) {
        memcpy(test_data->expected + (ptrdiff_t)(i * pattern_size), test_data->pattern, pattern_size);
    }
    ESP_LOGI(TAG, "EXPECTED STRING %.*s, SIZE:%d", test_data->expected_size, test_data->expected, test_data->expected_size);
}

static void configure_client(command_context_t * ctx, const char *transport)
{
    publish_context_t * test_data = ctx->data;
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


    if (selected_transport != test_data->selected_transport) {
        test_data->selected_transport = selected_transport;
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
        esp_mqtt_set_config(ctx->mqtt_client, &config);
    }
}

void publish_init_flags(void) {
    mqtt_event_group = xEventGroupCreate();
}

void publish_setup(command_context_t * ctx, char const * const transport) {
    xEventGroupClearBits(mqtt_event_group, CONNECTED_BIT);
    publish_context_t * data = (publish_context_t*)ctx->data;
    pattern_setup(data);
    configure_client(ctx, transport);
    esp_mqtt_client_register_event(ctx->mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, data);
}

void publish_teardown(command_context_t * ctx)
{
    esp_mqtt_client_unregister_event(ctx->mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler);
}

void publish_test(command_context_t * ctx, int expect_to_publish, int qos, bool enqueue)
{
    publish_context_t * data = (publish_context_t*)ctx->data;
    data->nr_of_msg_expected = expect_to_publish;
    ESP_LOGI(TAG, "PATTERN:%s REPEATED:%d PUBLISHED:%d", data->pattern, data->pattern_repetitions, data->nr_of_msg_expected);

    xEventGroupWaitBits(mqtt_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
    for (int i = 0; i < data->nr_of_msg_expected; i++) {
        int msg_id;
        if (enqueue) {
            msg_id = esp_mqtt_client_enqueue(ctx->mqtt_client, CONFIG_EXAMPLE_PUBLISH_TOPIC, data->expected, data->expected_size, qos, 0, true);
        } else {
            msg_id = esp_mqtt_client_publish(ctx->mqtt_client, CONFIG_EXAMPLE_PUBLISH_TOPIC, data->expected, data->expected_size, qos, 0);
            if(msg_id < 0) {
                ESP_LOGE(TAG, "Failed to publish");
                break;
            }
        }
        ESP_LOGD(TAG, "Publishing msg_id=%d", msg_id);
    }
}
