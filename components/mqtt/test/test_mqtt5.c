/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "unity.h"
#include "test_utils.h"
#include "memory_checks.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "sdkconfig.h"
#include "test_mqtt5_client_broker.h"
#include "test_mqtt_connection.h"
#include "esp_mac.h"
#include "esp_partition.h"

static esp_mqtt5_user_property_item_t user_property_arr[3] = {
    {"board", "esp32"},
    {"u", "user"},
    {"p", "password"}
};

static void test_leak_setup(const char * file, long line)
{
    uint8_t mac[6];
    struct timeval te;
    gettimeofday(&te, NULL); // get current time
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    printf("%s:%ld: time=%ld.%lds, mac:" MACSTR "\n", file, line, te.tv_sec, te.tv_usec, MAC2STR(mac));
    test_utils_record_free_mem();
}

TEST_CASE("mqtt5 init with invalid url", "[mqtt5][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
    const esp_mqtt_client_config_t mqtt5_cfg = {
            .broker.address.uri = "INVALID",
            .session.protocol_ver = MQTT_PROTOCOL_V_5,
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt5_cfg);
    TEST_ASSERT_EQUAL(NULL, client );
}

TEST_CASE("mqtt5 init and deinit", "[mqtt5][leaks=0]")
{
    test_leak_setup(__FILE__, __LINE__);
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

static const char* this_bin_addr(void)
{
    esp_partition_mmap_handle_t out_handle;
    const void *binary_address;
    const esp_partition_t* partition = esp_ota_get_running_partition();
    esp_partition_mmap(partition, 0, partition->size, ESP_PARTITION_MMAP_DATA, &binary_address, &out_handle);
    return binary_address;
}

TEST_CASE("mqtt5 enqueue and destroy outbox", "[mqtt5][leaks=0]")
{
    const char * bin_addr = this_bin_addr();
    test_leak_setup(__FILE__, __LINE__);
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
    TEST_ASSERT_GREATER_OR_EQUAL(messages*size, bytes_before - bytes_after);

    esp_mqtt_client_destroy(client);
}

#if SOC_EMAC_SUPPORTED
/**
 * This test cases uses ethernet kit, so build and use it only if EMAC supported
 */
TEST_CASE("mqtt5 broker tests", "[mqtt5][test_env=UT_T2_Ethernet]")
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
