/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/parlio_tx.h"
#include "driver/gpio.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "test_board.h"

TEST_CASE("parallel_tx_unit_install_uninstall", "[parlio_tx]")
{
    printf("install tx units exhaustively\r\n");
    parlio_tx_unit_handle_t units[SOC_PARLIO_GROUPS * SOC_PARLIO_TX_UNITS_PER_GROUP];
    int k = 0;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = SOC_PARLIO_TX_UNIT_MAX_DATA_WIDTH,
        .clk_in_gpio_num = -1, // clock source from internal
        .clk_out_gpio_num = 0,
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 64,
        .valid_gpio_num = -1,
    };
    for (int i = 0; i < SOC_PARLIO_GROUPS; i++) {
        for (int j = 0; j < SOC_PARLIO_TX_UNITS_PER_GROUP; j++) {
            TEST_ESP_OK(parlio_new_tx_unit(&config, &units[k++]));
        }
    }
    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, parlio_new_tx_unit(&config, &units[0]));

    for (int i = 0; i < k; i++) {
        TEST_ESP_OK(parlio_del_tx_unit(units[i]));
    }

    printf("install tx unit with valid signal and external core clock\r\n");
    // clock from external
    config.clk_in_gpio_num = 2;
    // failed because of invalid clock source frequency
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, parlio_new_tx_unit(&config, &units[0]));
    config.input_clk_src_freq_hz = 1000000;

    config.valid_gpio_num = 0;
    // failed because of data line conflict with valid signal
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, parlio_new_tx_unit(&config, &units[0]));

    config.data_width = 4;
    TEST_ESP_OK(parlio_new_tx_unit(&config, &units[0]));
    TEST_ESP_OK(parlio_tx_unit_enable(units[0]));
    // delete unit before it's disabled is not allowed
    TEST_ESP_ERR(ESP_ERR_INVALID_STATE, parlio_del_tx_unit(units[0]));
    TEST_ESP_OK(parlio_tx_unit_disable(units[0]));
    TEST_ESP_OK(parlio_del_tx_unit(units[0]));
}

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_tx_done_callback(parlio_tx_unit_handle_t tx_unit, const parlio_tx_done_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup = pdFALSE;
    TaskHandle_t task = (TaskHandle_t)user_ctx;
    vTaskNotifyGiveFromISR(task, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

TEST_CASE("parallel_tx_unit_trans_done_event", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 8,
        .max_transfer_size = 128,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("register trans_done event callback\r\n");
    parlio_tx_event_callbacks_t cbs = {
        .on_trans_done = test_parlio_tx_done_callback,
    };
    TEST_ESP_OK(parlio_tx_unit_register_event_callbacks(tx_unit, &cbs, xTaskGetCurrentTaskHandle()));

    printf("send packets and check event is fired\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    __attribute__((aligned(64))) uint8_t payload[64] = {0};
    for (int i = 0; i < 64; i++) {
        payload[i] = i;
    }
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 64 * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, portMAX_DELAY));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 64 * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, portMAX_DELAY));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
};

TEST_CASE("parallel_tx_unit_enable_disable", "[parlio_tx]")
{
    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 64,
        .max_transfer_size = 256,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packets for multiple times\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    __attribute__((aligned(64))) uint8_t payload[128] = {0};
    for (int i = 0; i < 128; i++) {
        payload[i] = i;
    }
    for (int j = 0; j < 64; j++) {
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, 128 * sizeof(uint8_t) * 8, &transmit_config));
    }

    printf("disable the transaction in the middle\r\n");
    while (parlio_tx_unit_disable(tx_unit) != ESP_OK) {
        esp_rom_delay_us(1000);
    }
    vTaskDelay(pdMS_TO_TICKS(100));

    printf("resume the transaction and pending packets should continue\r\n");
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
}

TEST_CASE("parallel_tx_unit_idle_value", "[parlio_tx]")
{
    printf("init a gpio to read parlio_tx output\r\n");
    gpio_config_t test_gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(TEST_DATA0_GPIO),
    };
    TEST_ESP_OK(gpio_config(&test_gpio_conf));

    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 8,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = -1,   // don't generate valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
            TEST_DATA4_GPIO,
            TEST_DATA5_GPIO,
            TEST_DATA6_GPIO,
            TEST_DATA7_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 64,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packet with different idle_value\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    uint32_t size = 64;
    __attribute__((aligned(64))) uint8_t payload[size];
    for (int i = 0; i < size; i++) {
        payload[i] = i;
    }
    for (int j = 0; j < 16; j++) {
        transmit_config.idle_value = j;
        TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, sizeof(payload) * 8, &transmit_config));
        TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, 100));
        TEST_ASSERT_EQUAL(j & 0x01, gpio_get_level(TEST_DATA0_GPIO));
    }

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(gpio_reset_pin(TEST_DATA0_GPIO));
}

#if SOC_PARLIO_TX_CLK_SUPPORT_GATING
TEST_CASE("parallel_tx_clock_gating", "[paralio_tx]")
{
    printf("init a gpio to read parlio_tx clk output\r\n");
    gpio_config_t test_gpio_conf = {
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = BIT64(TEST_CLK_GPIO),
    };
    TEST_ESP_OK(gpio_config(&test_gpio_conf));

    printf("install parlio tx unit\r\n");
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 2,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_DATA7_GPIO, // generate the valid signal
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 4,
        .max_transfer_size = 64,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .flags.clk_gate_en = true, // enable clock gating, controlled by the level of TEST_DATA7_GPIO
    };
    TEST_ESP_OK(parlio_new_tx_unit(&config, &tx_unit));
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));

    printf("send packets and see if the clock is gated when there's no transaction on line\r\n");
    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
    };
    uint32_t size = 64;
    __attribute__((aligned(64))) uint8_t payload[size];
    for (int i = 0; i < size; i++) {
        payload[i] = 0x1B; // 8'b00011011, in PARLIO_BIT_PACK_ORDER_MSB, you should see 2'b00, 2'b01, 2'b10, 2'b11 on the data line
    }
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, size * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    // check if the level on the clock line is low
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, payload, size * sizeof(uint8_t) * 8, &transmit_config));
    TEST_ESP_OK(parlio_tx_unit_wait_all_done(tx_unit, -1));
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));
    TEST_ASSERT_EQUAL(0, gpio_get_level(TEST_CLK_GPIO));

    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(gpio_reset_pin(TEST_CLK_GPIO));
}
#endif // SOC_PARLIO_TX_CLK_SUPPORT_GATING
