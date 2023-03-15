/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "esp_log.h"

/**
 * @brief MQTT client-broker tests are not implemented as separate test cases
 * due to time consuming connection setup/teardown.
 * This utility macro is used to run functional cases as MQTT tests
 * and evaluate as separate assertions in one "mqtt broker tests" test case.
 */
#define RUN_MQTT_BROKER_TEST(test_name) \
    do {                                \
        ESP_LOGI("mqtt_test", "Running test:" #test_name "()");                       \
        TEST_ASSERT_TRUE_MESSAGE(test_name(), "Mqtt test failed: " #test_name "() "); \
        ESP_LOGI("mqtt_test", "Test:" #test_name "() passed ");                       \
    } while(0)


/**
 * @brief This module contains mqtt test cases interacting the client with a (real) broker
 */

/**
 * @brief The client subscribes and publishes on the same topic
 * and verifies the received published qos in the event
 */
bool mqtt_subscribe_publish(void);

/**
 * @brief The client connects, disconnects and reconnects.
 * Tests basic client state transitions
 */
bool mqtt_connect_disconnect(void);

/**
 * @brief Two clients with defined lwt connect and subscribe to lwt topic.
 * This test verifies that no lwt is send when each of the client disconnects.
 * (we expect a clean disconnection, so no last-will being sent)
 */
bool mqtt_lwt_clean_disconnect(void);

/**
 * @brief The client subscribes to a topic with certain qos
 * and verifies the qos in SUBACK message from the broker.
 */
bool mqtt_subscribe_payload(void);
