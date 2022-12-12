/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "esp_mac.h"

#define WAIT_FOR_EVENT(event) \
    TEST_ASSERT_TRUE(xEventGroupWaitBits(s_event_group, event, pdTRUE, pdTRUE, pdMS_TO_TICKS(COMMON_OPERATION_TIMEOUT)) & event);

#define TEST_ASSERT_TRUE(condition) TEST_ASSERT_TRUE_LINE(condition, __LINE__)
#define TEST_ASSERT_TRUE_LINE(condition, line) \
    do {                                       \
        if (!(condition)) {                    \
                ESP_LOGE("test_mqtt_client_broker.c", \
                "Assertion failed in line %d", line); \
                return false;                         \
            } \
    } while(0)


static const int COMMON_OPERATION_TIMEOUT = 10000;
static const int CONNECT_BIT = BIT0;
static const int DISCONNECT_BIT = BIT1;
static const int DATA_BIT = BIT2;

static EventGroupHandle_t s_event_group;

static char* append_mac(const char* string)
{
    uint8_t mac[6];
    char *id_string = NULL;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    asprintf(&id_string, "%s_%02x%02X%02X", string, mac[3], mac[4], mac[5]);
    return id_string;
}

static void mqtt_data_handler_qos(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    if (event_id == MQTT_EVENT_DATA) {
        esp_mqtt_event_handle_t event = event_data;
        int * qos  = handler_args;
        *qos = event->qos;
        xEventGroupSetBits(s_event_group, DATA_BIT);
    }
}

static void mqtt_data_handler_lwt(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    if (event_id == MQTT_EVENT_DATA) {
        esp_mqtt_event_handle_t event = event_data;
        ESP_LOGI("mqtt-lwt", "MQTT_EVENT_DATA");
        ESP_LOGI("mqtt-lwt", "TOPIC=%.*s", event->topic_len, event->topic);
        ESP_LOGI("mqtt-lwt", "DATA=%.*s", event->data_len, event->data);
        if (strncmp(event->data, "no-lwt", event->data_len) == 0) {
            // no lwt, just to indicate the test has finished
            xEventGroupSetBits(s_event_group, DATA_BIT);
        } else {
            // count up any potential lwt message
            int * count  = handler_args;
            *count = *count + 1;
            ESP_LOGE("mqtt-lwt", "count=%d", *count);
        }
    }
}

static void mqtt_data_handler_subscribe(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    if (event_id == MQTT_EVENT_SUBSCRIBED) {
        esp_mqtt_event_handle_t event = event_data;
        ESP_LOGI("mqtt-subscribe", "MQTT_EVENT_SUBSCRIBED, data size=%d", event->data_len);
        int * sub_payload  = handler_args;
        if (event->data_len == 1) {
            ESP_LOGI("mqtt-subscribe", "DATA=%d", *(uint8_t*)event->data);
            *sub_payload = *(uint8_t*)event->data;
        }
        xEventGroupSetBits(s_event_group, DATA_BIT);
    }
}


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(s_event_group, CONNECT_BIT);
            break;

        case MQTT_EVENT_DISCONNECTED:
            xEventGroupSetBits(s_event_group, DISCONNECT_BIT);
            break;
        default:
            break;
    }
}

bool mqtt_connect_disconnect(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = CONFIG_MQTT_TEST_BROKER_URI,
            .network.disable_auto_reconnect = true,
    };
    s_event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_TRUE(NULL != client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt_client_start(client));
    WAIT_FOR_EVENT(CONNECT_BIT);
    esp_mqtt_client_disconnect(client);
    WAIT_FOR_EVENT(DISCONNECT_BIT);
    esp_mqtt_client_reconnect(client);
    WAIT_FOR_EVENT(CONNECT_BIT);
    esp_mqtt_client_destroy(client);
    vEventGroupDelete(s_event_group);
    return true;
}

bool mqtt_subscribe_publish(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = CONFIG_MQTT_TEST_BROKER_URI,
    };
    char* topic = append_mac("topic");
    TEST_ASSERT_TRUE(NULL != topic);
    s_event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_TRUE(NULL != client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt_client_start(client));
    WAIT_FOR_EVENT(CONNECT_BIT);
    int qos = -1;
    esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt_data_handler_qos, &qos);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client, topic, 2) != -1);
    TEST_ASSERT_TRUE(esp_mqtt_client_publish(client, topic, "message", 0, 2, 0) != -1);
    WAIT_FOR_EVENT(DATA_BIT);
    TEST_ASSERT_TRUE(qos == 2);
    TEST_ASSERT_TRUE(esp_mqtt_client_publish(client, topic, "message", 0, 1, 0) != -1);
    WAIT_FOR_EVENT(DATA_BIT);
    TEST_ASSERT_TRUE(qos == 1);
    esp_mqtt_client_destroy(client);
    vEventGroupDelete(s_event_group);
    free(topic);
    return true;
}

bool mqtt_lwt_clean_disconnect(void)
{
    char* lwt = append_mac("lwt");
    TEST_ASSERT_TRUE(lwt);
    const esp_mqtt_client_config_t mqtt_cfg1 = {
            .broker.address.uri = CONFIG_MQTT_TEST_BROKER_URI,
            .credentials.set_null_client_id = true,
            .session.last_will.topic = lwt,
            .session.last_will.msg = "lwt_msg"
    };
    const esp_mqtt_client_config_t mqtt_cfg2 = {
            .broker.address.uri = CONFIG_MQTT_TEST_BROKER_URI,
            .credentials.set_null_client_id = true,
            .session.last_will.topic = lwt,
            .session.last_will.msg = "lwt_msg"
    };
    s_event_group = xEventGroupCreate();

    esp_mqtt_client_handle_t client1 = esp_mqtt_client_init(&mqtt_cfg1);
    esp_mqtt_client_handle_t client2 = esp_mqtt_client_init(&mqtt_cfg2);
    TEST_ASSERT_TRUE(NULL != client1 && NULL != client2 );
    esp_mqtt_client_register_event(client1, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_register_event(client2, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    TEST_ASSERT_TRUE(esp_mqtt_client_start(client1) == ESP_OK);
    WAIT_FOR_EVENT(CONNECT_BIT);
    TEST_ASSERT_TRUE(esp_mqtt_client_start(client2) == ESP_OK);
    WAIT_FOR_EVENT(CONNECT_BIT);
    int counter = 0;
    esp_mqtt_client_register_event(client1, MQTT_EVENT_DATA, mqtt_data_handler_lwt, &counter);
    esp_mqtt_client_register_event(client2, MQTT_EVENT_DATA, mqtt_data_handler_lwt, &counter);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client1, lwt, 0) != -1);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client2, lwt, 0) != -1);
    esp_mqtt_client_disconnect(client1);
    WAIT_FOR_EVENT(DISCONNECT_BIT);
    esp_mqtt_client_reconnect(client1);
    WAIT_FOR_EVENT(CONNECT_BIT);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client1, lwt, 0) != -1);
    esp_mqtt_client_stop(client2);
    esp_mqtt_client_start(client2);
    WAIT_FOR_EVENT(CONNECT_BIT);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client2, lwt, 0) != -1);
    TEST_ASSERT_TRUE(esp_mqtt_client_publish(client1, lwt, "no-lwt", 0, 0, 0) != -1);
    WAIT_FOR_EVENT(DATA_BIT);
    TEST_ASSERT_TRUE(counter == 0);
    esp_mqtt_client_destroy(client1);
    esp_mqtt_client_destroy(client2);
    vEventGroupDelete(s_event_group);
    free(lwt);
    return true;
}

bool mqtt_subscribe_payload(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = CONFIG_MQTT_TEST_BROKER_URI,
            .network.disable_auto_reconnect = true,
    };
    char* topic = append_mac("topic");
    TEST_ASSERT_TRUE(NULL != topic);
    s_event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_TRUE(NULL != client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt_client_start(client));
    WAIT_FOR_EVENT(CONNECT_BIT);
    int qos_payload = -1;
    esp_mqtt_client_register_event(client, MQTT_EVENT_SUBSCRIBED, mqtt_data_handler_subscribe, &qos_payload);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client, topic, 2) != -1);
    WAIT_FOR_EVENT(DATA_BIT);
    TEST_ASSERT_TRUE(qos_payload == 2);
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client, topic, 0) != -1);
    WAIT_FOR_EVENT(DATA_BIT);
    TEST_ASSERT_TRUE(qos_payload == 0);
    esp_mqtt_client_destroy(client);
    vEventGroupDelete(s_event_group);
    free(topic);
    return true;
}
