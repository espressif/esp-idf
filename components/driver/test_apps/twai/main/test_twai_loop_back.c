/*
 * SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/twai.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "esp_private/sleep_cpu.h"
#include "esp_private/esp_sleep_internal.h"
#include "esp_private/esp_pmu.h"

TEST_CASE("driver_life_cycle", "[twai-loop-back]")
{
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_100KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 0, TWAI_MODE_NO_ACK);
    printf("install driver\r\n");
    TEST_ESP_OK(twai_driver_install(&g_config, &t_config, &f_config));
    // can't install the driver multiple times
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_driver_install(&g_config, &t_config, &f_config));

    printf("start driver\r\n");
    TEST_ESP_OK(twai_start());
    // can't start the driver again if it's already in running state
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_start());
    // can't uninstall the driver before stopping it
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, twai_driver_uninstall());

    printf("stop driver\r\n");
    TEST_ESP_OK(twai_stop());
    printf("uninstall driver\r\n");
    TEST_ESP_OK(twai_driver_uninstall());
}

TEST_CASE("twai_bit_timing", "[twai-loop-back]")
{
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 0, TWAI_MODE_NO_ACK);
    twai_timing_config_t t_config = {
        .quanta_resolution_hz = 33333, // invalid resolution
        .tseg_1 = 15,
        .tseg_2 = 4,
        .sjw = 1,
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, twai_driver_install(&g_config, &t_config, &f_config));

    t_config.quanta_resolution_hz = 2000000;
    TEST_ESP_OK(twai_driver_install(&g_config, &t_config, &f_config));
    TEST_ESP_OK(twai_driver_uninstall());
}

TEST_CASE("twai_mode_std_no_ack_25kbps", "[twai-loop-back]")
{
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_25KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // bind the TX and RX to the same GPIO to act like a loopback
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 0, TWAI_MODE_NO_ACK);
    printf("install twai driver\r\n");
    TEST_ESP_OK(twai_driver_install(&g_config, &t_config, &f_config));
    TEST_ESP_OK(twai_start());

    twai_message_t tx_msg = {
        .identifier = 0x123,
        .data_length_code = 8,
        .data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88},
        .self = true, // Transmitted message will also received by the same node
    };
    printf("transmit message\r\n");
    TEST_ESP_OK(twai_transmit(&tx_msg, pdMS_TO_TICKS(1000)));

    printf("receive message\r\n");
    twai_message_t rx_msg;
    TEST_ESP_OK(twai_receive(&rx_msg, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_TRUE(rx_msg.data_length_code == 8);
    for (int i = 0; i < 8; i++) {
        TEST_ASSERT_EQUAL(tx_msg.data[i], rx_msg.data[i]);
    }

    TEST_ESP_OK(twai_stop());
    TEST_ESP_OK(twai_driver_uninstall());
}

TEST_CASE("twai_mode_ext_no_ack_250kbps", "[twai-loop-back]")
{
    twai_handle_t twai_buses[SOC_TWAI_CONTROLLER_NUM] = {0};
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // bind the TX and RX to the same GPIO to act like a loopback
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 0, TWAI_MODE_NO_ACK);
    twai_message_t tx_msg = {
        .identifier = 0x12345,
        .data_length_code = 6,
        .data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
        .self = true, // Transmitted message will also received by the same node
        .extd = true, // Extended Frame Format (29bit ID)
    };

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        g_config.controller_id = i;
        g_config.tx_io = i;
        g_config.rx_io = i;
        printf("install twai driver %d\r\n", g_config.controller_id);
        TEST_ESP_OK(twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_buses[i]));
        TEST_ESP_OK(twai_start_v2(twai_buses[i]));
    }

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        printf("transmit message from %d\r\n", i);
        tx_msg.data[5] = SOC_TWAI_CONTROLLER_NUM - i;
        TEST_ESP_OK(twai_transmit_v2(twai_buses[i], &tx_msg, pdMS_TO_TICKS(1000)));
    }

    twai_message_t rx_msg;
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        printf("receive message from %d\r\n", i);
        TEST_ESP_OK(twai_receive_v2(twai_buses[i], &rx_msg, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_TRUE(rx_msg.data_length_code == 6);
        tx_msg.data[5] = SOC_TWAI_CONTROLLER_NUM - i;
        for (int i = 0; i < 6; i++) {
            TEST_ASSERT_EQUAL(tx_msg.data[i], rx_msg.data[i]);
        }
    }

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        TEST_ESP_OK(twai_stop_v2(twai_buses[i]));
        TEST_ESP_OK(twai_driver_uninstall_v2(twai_buses[i]));
    }
}

static void s_test_sleep_retention(bool allow_pd)
{
    // Prepare a TOP PD sleep
    TEST_ESP_OK(esp_sleep_enable_timer_wakeup(1 * 1000 * 1000));
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(true));
#endif
    esp_sleep_context_t sleep_ctx;
    esp_sleep_set_sleep_context(&sleep_ctx);

    twai_handle_t twai_buses[SOC_TWAI_CONTROLLER_NUM] = {0};
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // bind the TX and RX to the same GPIO to act like a loopback
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(0, 0, TWAI_MODE_NO_ACK);
    g_config.general_flags.sleep_allow_pd = allow_pd;
    twai_message_t tx_msg = {
        .identifier = 0x12345,
        .data_length_code = 6,
        .data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
        .self = true, // Transmitted message will also received by the same node
        .extd = true, // Extended Frame Format (29bit ID)
    };

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        g_config.controller_id = i;
        g_config.tx_io = i;
        g_config.rx_io = i;
        printf("install twai driver %d\r\n", g_config.controller_id);
        TEST_ESP_OK(twai_driver_install_v2(&g_config, &t_config, &f_config, &twai_buses[i]));
    }

    printf("Going into sleep...\n");
    TEST_ESP_OK(esp_light_sleep_start());
    printf("Waked up!\n");

    // check if the sleep happened as expected
    TEST_ASSERT_EQUAL(0, sleep_ctx.sleep_request_result);
#if SOC_TWAI_SUPPORT_SLEEP_RETENTION
    // check if the power domain also is powered down
    TEST_ASSERT_EQUAL(allow_pd ? PMU_SLEEP_PD_TOP : 0, (sleep_ctx.sleep_flags) & PMU_SLEEP_PD_TOP);
#endif

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        TEST_ESP_OK(twai_start_v2(twai_buses[i]));
    }

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        printf("transmit message from %d\r\n", i);
        tx_msg.data[5] = SOC_TWAI_CONTROLLER_NUM - i;
        TEST_ESP_OK(twai_transmit_v2(twai_buses[i], &tx_msg, pdMS_TO_TICKS(1000)));
    }

    twai_message_t rx_msg;
    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        printf("receive message from %d\r\n", i);
        TEST_ESP_OK(twai_receive_v2(twai_buses[i], &rx_msg, pdMS_TO_TICKS(1000)));
        TEST_ASSERT_TRUE(rx_msg.data_length_code == 6);
        tx_msg.data[5] = SOC_TWAI_CONTROLLER_NUM - i;
        for (int i = 0; i < 6; i++) {
            TEST_ASSERT_EQUAL(tx_msg.data[i], rx_msg.data[i]);
        }
    }

    for (int i = 0; i < SOC_TWAI_CONTROLLER_NUM; i++) {
        TEST_ESP_OK(twai_stop_v2(twai_buses[i]));
        TEST_ESP_OK(twai_driver_uninstall_v2(twai_buses[i]));
    }

    esp_sleep_set_sleep_context(NULL);
#if ESP_SLEEP_POWER_DOWN_CPU
    TEST_ESP_OK(sleep_cpu_configure(false));
#endif
}

#if SOC_TWAI_SUPPORT_SLEEP_RETENTION
TEST_CASE("twai_mode_ext_no_ack_250kbps with sleep retention (allow pd)", "[twai-loop-back]")
{
    s_test_sleep_retention(true);
}
#endif

TEST_CASE("twai_mode_ext_no_ack_250kbps with sleep retention (no pd)", "[twai-loop-back]")
{
    s_test_sleep_retention(false);
}
