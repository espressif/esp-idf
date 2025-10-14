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
#include "driver/uhci.h"

#define DATA_LENGTH 1024
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

static void disp_buf(uint8_t *buf, int len)
{
    int i;
    for (i = 0; i < len; i++) {
        printf("%02x ", buf[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

IRAM_ATTR static bool s_uhci_rx_event_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    uhci_context_t *ctx = (uhci_context_t *)user_ctx;
    BaseType_t xTaskWoken = 0;
    uhci_event_t evt = 0;
    if (edata->flags.totally_received) {
        evt = UHCI_EVT_EOF;
        ctx->receive_size += edata->recv_size;
        memcpy(ctx->p_receive_data, edata->data, edata->recv_size);
    } else {
        evt = UHCI_EVT_PARTIAL_DATA;
        ctx->receive_size += edata->recv_size;
        memcpy(ctx->p_receive_data, edata->data, edata->recv_size);
        ctx->p_receive_data += edata->recv_size;
    }

    xQueueSendFromISR(ctx->uhci_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void uhci_receive_test(void *arg)
{
    uhci_controller_handle_t uhci_ctrl = ((uhci_controller_handle_t *)arg)[0];
    SemaphoreHandle_t exit_sema = ((SemaphoreHandle_t *)arg)[1];

    uhci_context_t *ctx = heap_caps_calloc(1, sizeof(uhci_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(ctx);
    ctx->uhci_queue = xQueueCreate(15, sizeof(uhci_event_t));
    assert(ctx->uhci_queue);

    uint8_t *receive_data = heap_caps_calloc(1, DATA_LENGTH, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(receive_data);
    uint8_t *pdata = heap_caps_calloc(1, DATA_LENGTH / 4, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(pdata);
    ctx->p_receive_data = receive_data;

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_event_cbs,
    };
    TEST_ESP_OK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, ctx));
    TEST_ESP_OK(uhci_receive(uhci_ctrl, pdata, DATA_LENGTH / 4));

    uhci_event_t evt;
    while (1) {
        if (xQueueReceive(ctx->uhci_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == UHCI_EVT_EOF) {
                disp_buf(receive_data, ctx->receive_size);
                for (int i = 0; i < ctx->receive_size; i++) {
                    TEST_ASSERT(receive_data[i] == (uint8_t)i);
                }
                printf("Received size: %d\n", ctx->receive_size);
                break;
            }
        }
    }

    vQueueDelete(ctx->uhci_queue);
    free(pdata);
    free(receive_data);
    free(ctx);
    xSemaphoreGive(exit_sema);
    vTaskDelete(NULL);
}

TEST_CASE("UHCI write and receive with idle eof", "[uhci]")
{
    uart_config_t uart_config = {
        .baud_rate = 5 * 1000 * 1000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_XTAL,
    };
    TEST_ESP_OK(uart_param_config(EX_UART_NUM, &uart_config));
    // Connect TX and RX together for testing self send-receive
    TEST_ESP_OK(uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_TX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 30,
        .max_receive_internal_mem = 10 * 1024,
        .max_transmit_size = 10 * 1024,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    void *args[] = { uhci_ctrl, exit_sema };
    xTaskCreate(uhci_receive_test, "uhci_receive_test", 4096 * 2, args, 5, NULL);

    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    xSemaphoreTake(exit_sema, portMAX_DELAY);
    vTaskDelay(2);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    vSemaphoreDelete(exit_sema);
}

TEST_CASE("UHCI write and receive with length eof", "[uhci]")
{
    uart_config_t uart_config = {
        .baud_rate = 5 * 1000 * 1000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_XTAL,
    };
    TEST_ESP_OK(uart_param_config(EX_UART_NUM, &uart_config));
    // Connect TX and RX together for testing self send-receive
    TEST_ESP_OK(uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_TX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 30,
        .max_receive_internal_mem = 10 * 1024,
        .max_transmit_size = 10 * 1024,
        .dma_burst_size = 32,
        .max_packet_receive = 100,
        .rx_eof_flags.length_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    void *args[] = { uhci_ctrl, exit_sema };
    xTaskCreate(uhci_receive_test, "uhci_receive_test", 4096 * 2, args, 5, NULL);

    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    xSemaphoreTake(exit_sema, portMAX_DELAY);
    vTaskDelay(2);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    vSemaphoreDelete(exit_sema);
}

#if CONFIG_SPIRAM
#if SOC_AHB_GDMA_SUPPORT_PSRAM
static void uhci_receive_test_in_psram(void *arg)
{
    uhci_controller_handle_t uhci_ctrl = ((uhci_controller_handle_t *)arg)[0];
    SemaphoreHandle_t exit_sema = ((SemaphoreHandle_t *)arg)[1];

    uhci_context_t *ctx = heap_caps_calloc(1, sizeof(uhci_context_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(ctx);
    ctx->uhci_queue = xQueueCreate(15, sizeof(uhci_event_t));
    assert(ctx->uhci_queue);

    uint8_t *receive_data = heap_caps_calloc(1, DATA_LENGTH, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(receive_data);
    uint8_t *pdata = heap_caps_calloc(1, DATA_LENGTH / 4, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    assert(pdata);
    ctx->p_receive_data = receive_data;

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_event_cbs,
    };
    TEST_ESP_OK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, ctx));
    TEST_ESP_OK(uhci_receive(uhci_ctrl, pdata, DATA_LENGTH / 4));

    uhci_event_t evt;
    while (1) {
        if (xQueueReceive(ctx->uhci_queue, &evt, portMAX_DELAY) == pdTRUE) {
            if (evt == UHCI_EVT_EOF) {
                disp_buf(receive_data, ctx->receive_size);
                for (int i = 0; i < DATA_LENGTH; i++) {
                    TEST_ASSERT(receive_data[i] == (uint8_t)i);
                }
                printf("Received size: %d\n", ctx->receive_size);
                break;
            }
        }
    }

    vQueueDelete(ctx->uhci_queue);
    free(pdata);
    free(receive_data);
    free(ctx);
    xSemaphoreGive(exit_sema);
    vTaskDelete(NULL);
}

TEST_CASE("UHCI write and receive in psram", "[uhci]")
{
    uart_config_t uart_config = {
        .baud_rate = 5 * 1000 * 1000,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_XTAL,
    };
    TEST_ESP_OK(uart_param_config(EX_UART_NUM, &uart_config));
    // Connect TX and RX together for testing self send-receive
    TEST_ESP_OK(uart_set_pin(EX_UART_NUM, UART_TX_IO, UART_TX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 30,
        .max_receive_internal_mem = 10 * 1024,
        .max_transmit_size = 10 * 1024,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    void *args[] = { uhci_ctrl, exit_sema };
    xTaskCreate(uhci_receive_test_in_psram, "uhci_receive_test_in_psram", 4096 * 2, args, 5, NULL);

    uint8_t *data_wr = heap_caps_calloc(1, DATA_LENGTH, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    xSemaphoreTake(exit_sema, portMAX_DELAY);
    vTaskDelay(2);
    free(data_wr);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    vSemaphoreDelete(exit_sema);
}
#endif
#endif // CONFIG_SPIRAM
