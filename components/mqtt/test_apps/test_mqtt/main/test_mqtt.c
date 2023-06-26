/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 *
 * This test code is in the Public Domain (or CC0 licensed, at your option.)
 *
 * Unless required by applicable law or agreed to in writing, this
 * software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, either express or implied.
 */

#include <sys/time.h>
#include "unity_fixture.h"
#include "unity_fixture_extras.h"
#include "test_utils.h"
#include "memory_checks.h"
#include "mqtt_client.h"
#include "esp_ota_ops.h"
#include "test_mqtt_client_broker.h"
#include "test_mqtt_connection.h"
#include "esp_partition.h"

TEST_GROUP(mqtt);

TEST_SETUP(mqtt){
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
}

TEST_TEAR_DOWN(mqtt){
    test_utils_finish_and_evaluate_leaks(test_utils_get_leak_level(ESP_LEAK_TYPE_WARNING, ESP_COMP_LEAK_ALL),
                                         test_utils_get_leak_level(ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_ALL));
}

TEST(mqtt, init_with_invalid_url)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
            .broker.address.uri = "INVALID",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_EQUAL(NULL, client );
}

TEST(mqtt, init_and_deinit)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
            // no connection takes place, but the uri has to be valid for init() to succeed
            .broker.address.uri = "mqtts://localhost:8883",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
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

TEST(mqtt, enqueue_and_destroy_outbox)
{
    const char * bin_addr = this_bin_addr();

    // Reseting leak detection since this_bin_addr adds to allocated memory.
    test_utils_record_free_mem();
    TEST_ESP_OK(test_utils_set_leak_level(0, ESP_LEAK_TYPE_CRITICAL, ESP_COMP_LEAK_GENERAL));
    const int messages = 20;
    const int size = 2000;
    const esp_mqtt_client_config_t mqtt_cfg = {
            // no connection takes place, but the uri has to be valid for init() to succeed
            .broker.address.uri = "mqtts://localhost:8883",
    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    TEST_ASSERT_NOT_EQUAL(NULL, client );
    int bytes_before = esp_get_free_heap_size();
    for (int i=0; i<messages; ++i) {
        esp_mqtt_client_publish(client, "test", bin_addr, size, 1, 0);
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
TEST(mqtt, broker_tests)
{
    test_case_uses_tcpip();
    connect_test_fixture_setup();

    RUN_MQTT_BROKER_TEST(mqtt_connect_disconnect);
    RUN_MQTT_BROKER_TEST(mqtt_subscribe_publish);
    RUN_MQTT_BROKER_TEST(mqtt_lwt_clean_disconnect);
    RUN_MQTT_BROKER_TEST(mqtt_subscribe_payload);

    connect_test_fixture_teardown();
}
#endif // SOC_EMAC_SUPPORTED


TEST_GROUP_RUNNER(mqtt) {
RUN_TEST_CASE(mqtt, init_with_invalid_url);
RUN_TEST_CASE(mqtt, init_and_deinit);
RUN_TEST_CASE(mqtt, enqueue_and_destroy_outbox);

#if SOC_EMAC_SUPPORTED
RUN_TEST_CASE(mqtt, broker_tests);
#endif // SOC_EMAC_SUPPORTED
}

void app_main(void){
  UNITY_MAIN(mqtt);
}
