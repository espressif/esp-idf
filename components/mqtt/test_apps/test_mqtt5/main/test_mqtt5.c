/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/time.h>
#include "unity_fixture.h"
#include "unity_fixture_extras.h"
#include "test_utils.h"
#include "memory_checks.h"
#include "mqtt_client.h"
#include "esp_ota_ops.h"
#include "test_mqtt5_client_broker.h"
#include "test_mqtt_connection.h"
#include "esp_partition.h"


TEST_GROUP(mqtt5);

TEST_SETUP(mqtt5)
{
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(mqtt5)
{
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

static esp_mqtt5_user_property_item_t user_property_arr[3] = {
    {"board", "esp32"},
    {"u", "user"},
    {"p", "password"}
};

TEST(mqtt5, init_with_invalid_url)
{
    const esp_mqtt_client_config_t mqtt5_cfg = {
        .broker.address.uri = "INVALID",
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    TEST_ASSERT_EQUAL(NULL, client );
}

TEST(mqtt5, init_and_deinit)
{
    const esp_mqtt_client_config_t mqtt5_cfg = {
        // no connection takes place, but the uri has to be valid for init() to succeed
        .broker.address.uri = "mqtts://localhost:8883",
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
        .credentials.username = "123",
        .credentials.authentication.password = "456",
        .session.last_will.topic = "/topic/will",
        .session.last_will.msg = "i will leave",
        .session.last_will.msg_len = 12,
        .session.last_will.qos = 1,
        .session.last_will.retain = true,
    };
    esp_mqtt5_connection_property_config_t connect_property = {
        .session_expiry_interval = 10,
        .maximum_packet_size = 1024,
        .receive_maximum = 65535,
        .topic_alias_maximum = 2,
        .request_resp_info = true,
        .request_problem_info = true,
        .will_delay_interval = 10,
        .payload_format_indicator = true,
        .message_expiry_interval = 10,
        .content_type = "json",
        .response_topic = "/test/response",
        .correlation_data = "123456",
        .correlation_data_len = 6,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    esp_mqtt5_client_set_user_property(&connect_property.user_property, user_property_arr, 3);
    esp_mqtt5_client_set_user_property(&connect_property.will_user_property, user_property_arr, 3);
    esp_mqtt5_client_set_connect_property(client, &connect_property);
    esp_mqtt5_client_delete_user_property(connect_property.user_property);
    esp_mqtt5_client_delete_user_property(connect_property.will_user_property);
    TEST_ASSERT_NOT_EQUAL(NULL, client );
    esp_mqtt_client_destroy(client);
}

static const char *this_bin_addr(void)
{
    esp_partition_mmap_handle_t out_handle;
    const void *binary_address;
    const esp_partition_t *partition = esp_ota_get_running_partition();
    esp_partition_mmap(partition, 0, partition->size, ESP_PARTITION_MMAP_DATA, &binary_address, &out_handle);
    return binary_address;
}

TEST(mqtt5, enqueue_and_destroy_outbox)
{
    const char *bin_addr = this_bin_addr();
    // Reseting leak detection since this_bin_addr adds to allocated memory.
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
    const int messages = 20;
    const int size = 2000;
    const esp_mqtt_client_config_t mqtt5_cfg = {
        // no connection takes place, but the uri has to be valid for init() to succeed
        .broker.address.uri = "mqtts://localhost:8883",
        .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    esp_mqtt5_publish_property_config_t publish_property = {
        .payload_format_indicator = 1,
        .message_expiry_interval = 1000,
        .topic_alias = 0,
        .response_topic = "/topic/test/response",
        .correlation_data = "123456",
        .correlation_data_len = 6,
        .content_type = "json",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client );
    int bytes_before = esp_get_free_heap_size();
    for (int i = 0; i < messages; i ++) {
        esp_mqtt5_client_set_user_property(&publish_property.user_property, user_property_arr, 3);
        esp_mqtt5_client_set_publish_property(client, &publish_property);
        esp_mqtt_client_publish(client, "test", bin_addr, size, 1, 0);
        esp_mqtt5_client_delete_user_property(publish_property.user_property);
        publish_property.user_property = NULL;
    }
    int bytes_after = esp_get_free_heap_size();
    // check that outbox allocated all messages on heap
    TEST_ASSERT_GREATER_OR_EQUAL(messages * size, bytes_before - bytes_after);

    esp_mqtt_client_destroy(client);
}

#if SOC_EMAC_SUPPORTED
/**
 * This test cases uses ethernet kit, so build and use it only if EMAC supported
 */
TEST(mqtt5, broker_tests)
{
    test_case_uses_tcpip();
    connect_test_fixture_setup();

    RUN_MQTT5_BROKER_TEST(mqtt5_connect_disconnect);
    RUN_MQTT5_BROKER_TEST(mqtt5_subscribe_publish);
    RUN_MQTT5_BROKER_TEST(mqtt5_lwt_clean_disconnect);
    RUN_MQTT5_BROKER_TEST(mqtt5_subscribe_payload);

    connect_test_fixture_teardown();
}
#endif // SOC_EMAC_SUPPORTED

TEST_GROUP_RUNNER(mqtt5)
{
#if !DISABLED_FOR_TARGETS(ESP32H2)
    RUN_TEST_CASE(mqtt5, init_with_invalid_url);
    RUN_TEST_CASE(mqtt5, init_and_deinit);
    RUN_TEST_CASE(mqtt5, enqueue_and_destroy_outbox);

#if SOC_EMAC_SUPPORTED
    RUN_TEST_CASE(mqtt5, broker_tests);
#endif // SOC_EMAC_SUPPORTED
#endif //!DISABLED_FOR_TARGETS(ESP32H2)
}


void app_main(void)
{
    UNITY_MAIN(mqtt5);
}
