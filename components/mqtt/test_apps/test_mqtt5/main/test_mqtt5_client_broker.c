/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
                ESP_LOGE("test_mqtt5_client_broker.c", \
                "Assertion failed in line %d", line); \
                return false;                         \
            } \
    } while(0)


static const int COMMON_OPERATION_TIMEOUT = 10000;
static const int CONNECT_BIT = BIT0;
static const int DISCONNECT_BIT = BIT1;
static const int DATA_BIT = BIT2;

static EventGroupHandle_t s_event_group;

static esp_mqtt5_user_property_item_t user_property_arr[3] = {
    {"board", "esp32"},
    {"u", "user"},
    {"p", "password"}
};

static char* append_mac(const char* string)
{
    uint8_t mac[6];
    char *id_string = NULL;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    asprintf(&id_string, "%s_%02x%02X%02X", string, mac[3], mac[4], mac[5]);
    return id_string;
}

static void mqtt5_data_handler_qos(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    if (event_id == MQTT_EVENT_DATA) {
        esp_mqtt_event_handle_t event = event_data;
        int * qos  = handler_args;
        *qos = event->qos;
        xEventGroupSetBits(s_event_group, DATA_BIT);
    }
}

static void mqtt5_data_handler_lwt(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
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
            ESP_LOGE("mqtt5-lwt", "count=%d", *count);
        }
    }
}

static void mqtt5_data_handler_subscribe(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    if (event_id == MQTT_EVENT_SUBSCRIBED) {
        esp_mqtt_event_handle_t event = event_data;
        ESP_LOGI("mqtt5-subscribe", "MQTT_EVENT_SUBSCRIBED, data size=%d", event->data_len);
        int * sub_payload  = handler_args;
        if (event->data_len == 1) {
            ESP_LOGI("mqtt5-subscribe", "DATA=%d", *(uint8_t*)event->data);
            *sub_payload = *(uint8_t*)event->data;
        }
        xEventGroupSetBits(s_event_group, DATA_BIT);
    }
}


static void mqtt5_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
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

bool mqtt5_connect_disconnect(void)
{
    const esp_mqtt_client_config_t mqtt5_cfg = {
            .broker.address.uri = CONFIG_MQTT5_TEST_BROKER_URI,
            .network.disable_auto_reconnect = true,
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    esp_mqtt5_connection_property_config_t connect_property = {
        .session_expiry_interval = 10,
        .maximum_packet_size = 1024,
        .receive_maximum = 65535,
        .topic_alias_maximum = 2,
        .request_resp_info = true,
        .request_problem_info = true,
    };
    esp_mqtt5_disconnect_property_config_t disconnect_property = {
        .session_expiry_interval = 10,
        .disconnect_reason = 0,
    };
    s_event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    TEST_ASSERT_TRUE(NULL != client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_user_property(&connect_property.user_property, user_property_arr, 3));
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_connect_property(client, &connect_property));
    esp_mqtt5_client_delete_user_property(connect_property.user_property);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt_client_start(client));
    WAIT_FOR_EVENT(CONNECT_BIT);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_user_property(&disconnect_property.user_property, user_property_arr, 3));
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_disconnect_property(client, &disconnect_property));
    esp_mqtt5_client_delete_user_property(disconnect_property.user_property);
    esp_mqtt_client_disconnect(client);
    WAIT_FOR_EVENT(DISCONNECT_BIT);
    esp_mqtt_client_reconnect(client);
    WAIT_FOR_EVENT(CONNECT_BIT);
    esp_mqtt_client_destroy(client);
    vEventGroupDelete(s_event_group);
    return true;
}

bool mqtt5_subscribe_publish(void)
{
    const esp_mqtt_client_config_t mqtt5_cfg = {
            .broker.address.uri = CONFIG_MQTT5_TEST_BROKER_URI,
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    esp_mqtt5_publish_property_config_t publish_property = {
        .payload_format_indicator = 1,
        .message_expiry_interval = 1000,
        .topic_alias = 1,
        .response_topic = "/topic/test/response",
        .correlation_data = "123456",
        .correlation_data_len = 6,
        .content_type = "json",
    };
    esp_mqtt5_subscribe_property_config_t subscribe_property = {
        .subscribe_id = 25555,
        .no_local_flag = false,
        .retain_as_published_flag = true,
        .retain_handle = 0,
    };
    char* topic = append_mac("topic");
    TEST_ASSERT_TRUE(NULL != topic);
    s_event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    TEST_ASSERT_TRUE(NULL != client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt_client_start(client));
    WAIT_FOR_EVENT(CONNECT_BIT);
    int qos = -1;
    esp_mqtt_client_register_event(client, MQTT_EVENT_DATA, mqtt5_data_handler_qos, &qos);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_subscribe_property(client, &subscribe_property));
    TEST_ASSERT_TRUE(esp_mqtt_client_subscribe(client, topic, 2) != -1);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_publish_property(client, &publish_property));
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

bool mqtt5_lwt_clean_disconnect(void)
{
    char* lwt = append_mac("lwt");
    TEST_ASSERT_TRUE(lwt);
    const esp_mqtt_client_config_t mqtt5_cfg1 = {
            .broker.address.uri = CONFIG_MQTT5_TEST_BROKER_URI,
            .credentials.set_null_client_id = true,
            .session.last_will.topic = lwt,
            .session.last_will.msg = "lwt_msg",
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    const esp_mqtt_client_config_t mqtt5_cfg2 = {
            .broker.address.uri = CONFIG_MQTT5_TEST_BROKER_URI,
            .credentials.set_null_client_id = true,
            .session.last_will.topic = lwt,
            .session.last_will.msg = "lwt_msg",
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    esp_mqtt5_connection_property_config_t connect_property = {
        .will_delay_interval = 10,
        .payload_format_indicator = true,
        .message_expiry_interval = 10,
        .content_type = "json",
        .response_topic = "/test/response",
        .correlation_data = "123456",
        .correlation_data_len = 6,
    };
    s_event_group = xEventGroupCreate();

    esp_mqtt_client_handle_t client1 = esp_mqtt_client_init(&mqtt5_cfg1);
    esp_mqtt_client_handle_t client2 = esp_mqtt_client_init(&mqtt5_cfg2);
    TEST_ASSERT_TRUE(NULL != client1 && NULL != client2 );
    esp_mqtt_client_register_event(client1, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    esp_mqtt_client_register_event(client2, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_connect_property(client1, &connect_property));
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt5_client_set_connect_property(client2, &connect_property));
    TEST_ASSERT_TRUE(esp_mqtt_client_start(client1) == ESP_OK);
    WAIT_FOR_EVENT(CONNECT_BIT);
    TEST_ASSERT_TRUE(esp_mqtt_client_start(client2) == ESP_OK);
    WAIT_FOR_EVENT(CONNECT_BIT);
    int counter = 0;
    esp_mqtt_client_register_event(client1, MQTT_EVENT_DATA, mqtt5_data_handler_lwt, &counter);
    esp_mqtt_client_register_event(client2, MQTT_EVENT_DATA, mqtt5_data_handler_lwt, &counter);
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

bool mqtt5_subscribe_payload(void)
{
    const esp_mqtt_client_config_t mqtt5_cfg = {
            .broker.address.uri = CONFIG_MQTT5_TEST_BROKER_URI,
            .network.disable_auto_reconnect = true,
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    char* topic = append_mac("topic");
    TEST_ASSERT_TRUE(NULL != topic);
    s_event_group = xEventGroupCreate();
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    TEST_ASSERT_TRUE(NULL != client );
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt5_event_handler, NULL);
    TEST_ASSERT_TRUE(ESP_OK == esp_mqtt_client_start(client));
    WAIT_FOR_EVENT(CONNECT_BIT);
    int qos_payload = -1;
    esp_mqtt_client_register_event(client, MQTT_EVENT_SUBSCRIBED, mqtt5_data_handler_subscribe, &qos_payload);
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
