/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
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
#include "esp_attr.h"

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
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 2, TWAI_MODE_LISTEN_ONLY);
#if CONFIG_TWAI_ISR_IN_IRAM
    g_config.intr_flags |= ESP_INTR_FLAG_IRAM;
#endif
    // listen only mode doesn't need a tx queue
    g_config.tx_queue_len = 0;
    TEST_ESP_OK(twai_driver_install(&g_config, &t_config, &f_config));
    TEST_ESP_OK(twai_start());

#if CONFIG_TWAI_ISR_IN_IRAM
    printf("disable flash cache and check if we can still receive the frame\r\n");
    for (int i = 0; i < 100; i++) {
        unity_utils_run_cache_disable_stub(test_delay_post_cache_disable, NULL);
    }
#endif

    uint8_t expected_data[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    twai_message_t rx_msg;
    TEST_ESP_OK(twai_receive(&rx_msg, portMAX_DELAY));

    TEST_ASSERT_EQUAL(0x123, rx_msg.identifier);
    for (int i = 0; i < rx_msg.data_length_code; i++) {
        TEST_ASSERT_EQUAL_HEX8(expected_data[i], rx_msg.data[i]);
    }

    TEST_ESP_OK(twai_stop());
    TEST_ESP_OK(twai_driver_uninstall());
}

TEST_CASE("twai_remote_request", "[twai]")
{
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 2, TWAI_MODE_NORMAL);
    TEST_ESP_OK(twai_driver_install(&g_config, &t_config, &f_config));
    TEST_ESP_OK(twai_start());

    twai_message_t req_msg = {
        .identifier = 0x6688,
        .data_length_code = 8,
        .rtr = true, // remote request
        .extd = true,// extended ID
    };
    TEST_ESP_OK(twai_transmit(&req_msg, portMAX_DELAY));

    uint8_t expected_data[8] = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80};
    twai_message_t res_msg;
    TEST_ESP_OK(twai_receive(&res_msg, portMAX_DELAY));
    TEST_ASSERT_EQUAL(0x6688, res_msg.identifier);
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL_HEX8(expected_data[i], res_msg.data[i]);
    }

    TEST_ESP_OK(twai_stop());
    TEST_ESP_OK(twai_driver_uninstall());
}
