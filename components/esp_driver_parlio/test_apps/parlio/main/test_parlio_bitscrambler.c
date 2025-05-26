/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "unity.h"
#include "driver/parlio_tx.h"
#include "driver/parlio_rx.h"
#include "driver/parlio_bitscrambler.h"
#include "driver/bitscrambler.h"
#include "driver/gpio.h"
#include "hal/parlio_ll.h"
#include "soc/soc_caps.h"
#include "esp_attr.h"
#include "test_board.h"

BITSCRAMBLER_PROGRAM(bitscrambler_program_test_tx_LSB_to_MSB, "test_parlio_tx_LSB_to_MSB");
BITSCRAMBLER_PROGRAM(bitscrambler_program_test_tx_multiply, "test_parlio_tx_multiply");

TEST_PARLIO_CALLBACK_ATTR
static bool test_parlio_rx_done_callback(parlio_rx_unit_handle_t rx_unit, const parlio_rx_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_wakeup = pdFALSE;
    TaskHandle_t task = (TaskHandle_t)user_ctx;
    vTaskNotifyGiveFromISR(task, &high_task_wakeup);
    return high_task_wakeup == pdTRUE;
}

#define TEST_PAYLOAD_SIZE  64
static void test_parlio_bitscrambler(void)
{
    parlio_tx_unit_handle_t tx_unit = NULL;
    parlio_tx_unit_config_t tx_config = {
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .data_width = 4,
        .clk_in_gpio_num = -1,  // use internal clock source
        .valid_gpio_num = TEST_VALID_GPIO,
        .clk_out_gpio_num = TEST_CLK_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
        },
        .output_clk_freq_hz = 1 * 1000 * 1000,
        .trans_queue_depth = 8,
        .max_transfer_size = 128,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
    };

    parlio_rx_unit_handle_t rx_unit = NULL;
    parlio_rx_unit_config_t rx_config = {
        .trans_queue_depth = 10,
        .max_recv_size = 1024,
        .data_width = 4,
        .clk_src = PARLIO_CLK_SRC_DEFAULT,
        .ext_clk_freq_hz =  0,
        .clk_in_gpio_num =  -1,
        .exp_clk_freq_hz = 1 * 1000 * 1000,
        .clk_out_gpio_num = -1,
        .valid_gpio_num = TEST_VALID_GPIO,
        .data_gpio_nums = {
            TEST_DATA0_GPIO,
            TEST_DATA1_GPIO,
            TEST_DATA2_GPIO,
            TEST_DATA3_GPIO,
        },
        .flags = {
            .clk_gate_en = false,
        }
    };

    printf("install parlio unit\r\n");
    TEST_ESP_OK(parlio_new_tx_unit(&tx_config, &tx_unit));
    TEST_ESP_OK(parlio_new_rx_unit(&rx_config, &rx_unit));

    printf("decorate tx unit with bitscrambler\r\n");
    TEST_ESP_OK(parlio_tx_unit_decorate_bitscrambler(tx_unit));

    parlio_transmit_config_t transmit_config = {
        .idle_value = 0x00,
        .bitscrambler_program = bitscrambler_program_test_tx_LSB_to_MSB,
    };
    uint8_t tx_payload[TEST_PAYLOAD_SIZE] = {0};
    for (int i = 0; i < TEST_PAYLOAD_SIZE; i++) {
        tx_payload[i] = i;
    }

    parlio_rx_delimiter_handle_t deli = NULL;
    parlio_rx_level_delimiter_config_t lvl_deli_cfg = {
        .valid_sig_line_id = PARLIO_RX_UNIT_MAX_DATA_WIDTH - 1,
        .sample_edge = PARLIO_SAMPLE_EDGE_POS,
        .bit_pack_order = PARLIO_BIT_PACK_ORDER_MSB, // data in LSB mode is adjusted by bitscrambler, so we use MSB mode for receiving
        .eof_data_len = TEST_PAYLOAD_SIZE,
        .timeout_ticks = 0,
        .flags = {
            .active_low_en = 0,
        },
    };
    TEST_ESP_OK(parlio_new_rx_level_delimiter(&lvl_deli_cfg, &deli));
    printf("register receive_done event callback\r\n");
    parlio_rx_event_callbacks_t rx_cbs = {
        .on_receive_done = test_parlio_rx_done_callback,
    };
    TEST_ESP_OK(parlio_rx_unit_register_event_callbacks(rx_unit, &rx_cbs, xTaskGetCurrentTaskHandle()));

    parlio_receive_config_t recv_config = {
        .delimiter = deli,
        .flags.partial_rx_en = false,
    };
    __attribute__((aligned(TEST_PAYLOAD_SIZE))) uint8_t rx_payload[TEST_PAYLOAD_SIZE] = {0};

    // Tx in LSB mode
    // Tx Bitscrambler turn LSB to MSB
    // Rx in MSB mode
    printf("enable parlio and transmit\r\n");
    TEST_ESP_OK(parlio_tx_unit_enable(tx_unit));
    TEST_ESP_OK(parlio_rx_unit_enable(rx_unit, 1));
    TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, rx_payload, TEST_PAYLOAD_SIZE, &recv_config));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, tx_payload, TEST_PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));

    for (int i = 0; i < TEST_PAYLOAD_SIZE; i++) {
        printf("%.3d ", (rx_payload[i]));
        TEST_ASSERT_EQUAL(tx_payload[i], rx_payload[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    TEST_ESP_OK(parlio_del_rx_delimiter(deli));
    // we don't use bitscrambler to adjust the bit order in the following tests, so we keep LSB order to receive the original data
    lvl_deli_cfg.bit_pack_order = PARLIO_BIT_PACK_ORDER_LSB;
    TEST_ESP_OK(parlio_new_rx_level_delimiter(&lvl_deli_cfg, &deli));

    // not use bitscrambler
    transmit_config.bitscrambler_program = NULL;

    TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, rx_payload, TEST_PAYLOAD_SIZE, &recv_config));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, tx_payload, TEST_PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));

    for (int i = 0; i < TEST_PAYLOAD_SIZE; i++) {
        printf("%.3d ", (rx_payload[i]));
        TEST_ASSERT_EQUAL(tx_payload[i], rx_payload[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    // use another bitscrambler program
    // Tx Bitscrambler multiply the tx payload by 2
    transmit_config.bitscrambler_program = bitscrambler_program_test_tx_multiply;

    TEST_ESP_OK(parlio_rx_unit_receive(rx_unit, rx_payload, TEST_PAYLOAD_SIZE, &recv_config));
    TEST_ESP_OK(parlio_tx_unit_transmit(tx_unit, tx_payload, TEST_PAYLOAD_SIZE * sizeof(uint8_t) * 8, &transmit_config));

    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));

    for (int i = 0; i < TEST_PAYLOAD_SIZE; i++) {
        printf("%.3d ", (rx_payload[i]));
        TEST_ASSERT_EQUAL(tx_payload[i] * 2, rx_payload[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    TEST_ESP_OK(parlio_tx_unit_disable(tx_unit));
    TEST_ASSERT_EQUAL(ESP_ERR_INVALID_STATE, parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(parlio_tx_unit_undecorate_bitscrambler(tx_unit));
    TEST_ESP_OK(parlio_del_tx_unit(tx_unit));
    TEST_ESP_OK(parlio_rx_unit_disable(rx_unit));
    TEST_ESP_OK(parlio_del_rx_delimiter(deli));
    TEST_ESP_OK(parlio_del_rx_unit(rx_unit));
}

TEST_CASE("parlio_tx_bitscrambler_test", "[parlio_bitscrambler]")
{
    test_parlio_bitscrambler();
}
