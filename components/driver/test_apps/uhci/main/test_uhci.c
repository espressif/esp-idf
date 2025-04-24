/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"
#include "driver/uart.h"
#include "esp_private/uhci.h"

#define DATA_LENGTH 5024
#define EX_UART_NUM 1
#define UART_TX_IO 2
#define UART_RX_IO 3

TEST_CASE("UHCI driver memory leaking check", "[uhci]")
{
    uart_config_t uart_config = {
        .baud_rate = 1 * 1000 * 1000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    //UART parameter config
    TEST_ESP_OK(uart_param_config(EX_UART_NUM, &uart_config));
    TEST_ESP_OK(uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_RX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 30,
        .max_receive_internal_mem = 10 * 1024,
        .max_transmit_size = 10 * 1024,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;

    int size = esp_get_free_heap_size();
    for (int i = 0; i < 5; i++) {
        TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));
        vTaskDelay(10 / portTICK_PERIOD_MS);
        TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    }

    TEST_ASSERT_INT_WITHIN(300, size, esp_get_free_heap_size());
}

TEST_CASE("UHCI controller install-uninstall test", "[i2c]")
{
    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 30,
        .max_receive_internal_mem = 10 * 1024,
        .max_transmit_size = 10 * 1024,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    uhci_controller_handle_t uhci_ctrl2;

    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    TEST_ESP_ERR(ESP_ERR_NOT_FOUND, uhci_new_controller(&uhci_cfg, &uhci_ctrl2));

    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
}

typedef enum {
    UHCI_EVT_PARTIAL_DATA,
    UHCI_EVT_EOF,
} uhci_event_t;

typedef struct {
    QueueHandle_t uhci_queue;
    size_t receive_size;
    uint8_t *p_receive_data;
} uhci_context_t;

TEST_CASE("UHCI write and receive with idle eof", "[uhci]")
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_XTAL,
    };
    TEST_ESP_OK(uart_param_config(EX_UART_NUM, &uart_config));
    // Connect TX and RX together for testing self send-receive
    TEST_ESP_OK(uart_set_pin(EX_UART_NUM, 2, 5, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 30,
        .max_receive_internal_mem = 10 * 1024,
        .max_transmit_size = 10 * 1024,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    uint8_t *data_wr = heap_caps_calloc(1, DATA_LENGTH, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    free(data_wr);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
}
