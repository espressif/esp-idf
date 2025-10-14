/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "driver/uhci.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_err.h"
#include "esp_check.h"

static const char *TAG = "uhci-example";

#define EXAMPLE_UART_NUM CONFIG_UART_PORT_NUM
#define EXAMPLE_UART_BAUD_RATE CONFIG_UART_BAUD_RATE
#define EXAMPLE_UART_RX_IO CONFIG_UART_RX_IO
#define UART_DMA_OTA_BUFFER_SIZE (10 * 1024)

typedef enum {
    UHCI_EVT_PARTIAL_DATA,
    UHCI_EVT_EOF,
} uhci_event_t;

typedef struct {
    QueueHandle_t uhci_queue;
    size_t receive_size;
    uint8_t *ota_data1;
    uint8_t *ota_data2;
    bool use_ota_data1;
} ota_example_context_t;

static bool s_uhci_rx_event_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    ota_example_context_t *ctx = (ota_example_context_t *)user_ctx;
    BaseType_t xTaskWoken = 0;
    uhci_event_t evt = 0;

    if (edata->flags.totally_received) {
        evt = UHCI_EVT_EOF;
    } else {
        evt = UHCI_EVT_PARTIAL_DATA;
    }

    // Choose the buffer to store received data
    ctx->receive_size = edata->recv_size;
    if (ctx->use_ota_data1) {
        ctx->ota_data1 = edata->data;
    } else {
        ctx->ota_data2 = edata->data;
    }

    // Toggle the buffer for the next receive
    ctx->use_ota_data1 = !ctx->use_ota_data1;

    xQueueSendFromISR(ctx->uhci_queue, &evt, &xTaskWoken);
    return xTaskWoken;
}

static void perform_ota_update(uhci_controller_handle_t uhci_ctrl, ota_example_context_t *ctx)
{
    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        ESP_LOGE(TAG, "No OTA partition found");
        return;
    }

    esp_ota_handle_t ota_handle;
    ESP_ERROR_CHECK(esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle));
    ESP_LOGI(TAG, "OTA process started");

    uhci_event_t evt;
    uint32_t received_size = 0;
    uint8_t *pdata = heap_caps_calloc(1, UART_DMA_OTA_BUFFER_SIZE, MALLOC_CAP_DEFAULT);
    assert(pdata);
    ESP_ERROR_CHECK(uhci_receive(uhci_ctrl, pdata, UART_DMA_OTA_BUFFER_SIZE));
    while (1) {
        if (xQueueReceive(ctx->uhci_queue, &evt, portMAX_DELAY) == pdTRUE) {
            uint8_t *data_to_write = ctx->use_ota_data1 ? ctx->ota_data2 : ctx->ota_data1;
            ESP_ERROR_CHECK(esp_ota_write(ota_handle, data_to_write, ctx->receive_size));
            received_size += ctx->receive_size;
            if (evt == UHCI_EVT_EOF) {
                break;
            }

        }
    }
    free(pdata);

    ESP_LOGI(TAG, "Total received size: %ld", received_size);
    ESP_ERROR_CHECK(esp_ota_end(ota_handle));
    ESP_ERROR_CHECK(esp_ota_set_boot_partition(ota_partition));
}

void app_main(void)
{
    uart_config_t uart_config = {
        .baud_rate = EXAMPLE_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_param_config(EXAMPLE_UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(EXAMPLE_UART_NUM, -1, EXAMPLE_UART_RX_IO, -1, -1));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = EXAMPLE_UART_NUM,
        .tx_trans_queue_depth = 1,
        .max_receive_internal_mem = UART_DMA_OTA_BUFFER_SIZE,
        .max_transmit_size = UART_DMA_OTA_BUFFER_SIZE,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1, // receive finishes when rx line turns idle.
    };

    uhci_controller_handle_t uhci_ctrl;
    ESP_ERROR_CHECK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    ESP_LOGI(TAG, "UHCI initialized, baud rate is %d, rx pin is %d", uart_config.baud_rate, EXAMPLE_UART_RX_IO);

    ota_example_context_t *ctx = calloc(1, sizeof(ota_example_context_t));
    assert(ctx);

    ctx->uhci_queue = xQueueCreate(2, sizeof(uhci_event_t));
    assert(ctx->uhci_queue);

    ctx->use_ota_data1 = true;  // Start with ota_data1

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_event_cbs,
    };

    ESP_ERROR_CHECK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, ctx));

    perform_ota_update(uhci_ctrl, ctx);

    ESP_ERROR_CHECK(uhci_del_controller(uhci_ctrl));

    free(ctx);
    ESP_LOGI(TAG, "OTA update successful. Rebooting...");
    esp_restart();
}
