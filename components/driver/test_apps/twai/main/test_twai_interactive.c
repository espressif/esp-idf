/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"
#include "unity.h"
#include "unity_test_utils.h"
#include "driver/twai.h"
#include "soc/soc_caps.h"
#include "esp_log.h"

#define TEST_TWAI_TX_PIN     4
#define TEST_TWAI_RX_PIN     5

#if CONFIG_TWAI_ISR_IN_IRAM
static void IRAM_ATTR test_delay_post_cache_disable(void *args)
{
    esp_rom_delay_us(1000);
}
#endif

TEST_CASE("twai_listen_only", "[twai]")
{
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TEST_TWAI_TX_PIN, TEST_TWAI_RX_PIN, TWAI_MODE_LISTEN_ONLY);
#if CONFIG_TWAI_ISR_IN_IRAM
    g_config.intr_flags |= ESP_INTR_FLAG_IRAM;
#endif
    // listen only mode doesn't need a tx queue
    g_config.tx_queue_len = 0;
    TEST_ESP_OK(twai_driver_install(&g_config, &t_config, &f_config));
    TEST_ESP_OK(twai_start());

#if CONFIG_TWAI_ISR_IN_IRAM
    printf("disable flash cache and check if we can still receive the frame\n");
    for (int i = 0; i < 100; i++) {
        unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    }
#endif

    uint8_t expected_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    twai_message_t rx_msg;
    TEST_ESP_OK(twai_receive(&rx_msg, portMAX_DELAY));

    TEST_ASSERT_EQUAL(0x123, rx_msg.identifier);
    ESP_LOG_BUFFER_HEX("rx", rx_msg.data, rx_msg.data_length_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, rx_msg.data, rx_msg.data_length_code);

    TEST_ESP_OK(twai_stop());
    TEST_ESP_OK(twai_driver_uninstall());
}

TEST_CASE("twai_remote_request", "[twai]")
{
    twai_handle_t bus_handle;
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(TEST_TWAI_TX_PIN, TEST_TWAI_RX_PIN, TWAI_MODE_NORMAL);
#if CONFIG_IDF_TARGET_ESP32C6 || CONFIG_IDF_TARGET_ESP32P4
    g_config.controller_id = 1;
#endif
    TEST_ESP_OK(twai_driver_install_v2(&g_config, &t_config, &f_config, &bus_handle));
    TEST_ESP_OK(twai_start_v2(bus_handle));

    twai_message_t req_msg = {
        .identifier = 0x6688,
        .data_length_code = 8,
        .rtr = true, // remote request
        .extd = true,// extended ID
    };
    TEST_ESP_OK(twai_transmit_v2(bus_handle, &req_msg, portMAX_DELAY));
    ESP_LOGI("TWAI", "send remote frame");

    uint8_t expected_data[8] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
    twai_message_t res_msg;
    TEST_ESP_OK(twai_receive_v2(bus_handle, &res_msg, portMAX_DELAY));
    ESP_LOGI("TWAI", "receive with id %lx\n", res_msg.identifier);
    TEST_ASSERT_EQUAL(0x6688, res_msg.identifier);
    ESP_LOG_BUFFER_HEX("rx", res_msg.data, res_msg.data_length_code);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected_data, res_msg.data, res_msg.data_length_code);

    TEST_ESP_OK(twai_stop_v2(bus_handle));
    TEST_ESP_OK(twai_driver_uninstall_v2(bus_handle));
}
