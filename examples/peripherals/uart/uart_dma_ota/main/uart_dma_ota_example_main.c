/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/ringbuf.h"
#include "driver/uart.h"
#include "driver/uhci.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_heap_caps.h"

static const char *TAG = "uhci-example";

#define EXAMPLE_UART_NUM CONFIG_UART_PORT_NUM
#define EXAMPLE_UART_BAUD_RATE CONFIG_UART_BAUD_RATE
#define EXAMPLE_UART_RX_IO CONFIG_UART_RX_IO
#define UART_DMA_OTA_BUFFER_SIZE (10 * 1024)
#define UART_DMA_OTA_RINGBUF_SIZE (10 * 1024)

typedef struct {
    RingbufHandle_t ringbuf;
    volatile bool rx_eof;
    volatile bool rx_overflow;
} ota_rx_context_t;

static bool s_uhci_rx_event_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    ota_rx_context_t *ctx = (ota_rx_context_t *)user_ctx;
    BaseType_t xTaskWoken = pdFALSE;

    if (xRingbufferSendFromISR(ctx->ringbuf, edata->data, edata->recv_size, &xTaskWoken) != pdTRUE) {
        ctx->rx_overflow = true;
    }
    if (edata->flags.totally_received) {
        ctx->rx_eof = true;
    }
    return xTaskWoken == pdTRUE;
}

static bool rx_ringbuf_is_empty(RingbufHandle_t ringbuf)
{
    UBaseType_t items_waiting = 0;
    vRingbufferGetInfo(ringbuf, NULL, NULL, NULL, NULL, &items_waiting);
    return items_waiting == 0;
}

static void perform_ota_update(uhci_controller_handle_t uhci_ctrl, ota_rx_context_t *ctx)
{
    const esp_partition_t *ota_partition = esp_ota_get_next_update_partition(NULL);
    if (!ota_partition) {
        ESP_LOGE(TAG, "No OTA partition found");
        return;
    }

    esp_ota_handle_t ota_handle;
    ESP_ERROR_CHECK(esp_ota_begin(ota_partition, OTA_SIZE_UNKNOWN, &ota_handle));
    ESP_LOGI(TAG, "OTA process started");

    uint8_t *pdata = heap_caps_calloc(1, UART_DMA_OTA_BUFFER_SIZE, MALLOC_CAP_DEFAULT);
    assert(pdata);
    ESP_ERROR_CHECK(uhci_receive(uhci_ctrl, pdata, UART_DMA_OTA_BUFFER_SIZE));

    size_t total_received_size = 0;
    while (1) {
        size_t item_size = 0;
        uint8_t *data = xRingbufferReceive(ctx->ringbuf, &item_size, pdMS_TO_TICKS(1000));
        if (data) {
            ESP_ERROR_CHECK(esp_ota_write(ota_handle, data, item_size));
            vRingbufferReturnItem(ctx->ringbuf, data);
            total_received_size += item_size;
        }

        if (ctx->rx_overflow) {
            ESP_LOGE(TAG, "RX ring buffer overflow, please reduce the baud rate or increase the ring buffer size");
            abort();
        }

        if (ctx->rx_eof && rx_ringbuf_is_empty(ctx->ringbuf)) {
            break;
        }
    }
    free(pdata);

    ESP_LOGI(TAG, "Total received size: %zu", total_received_size);
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

    ota_rx_context_t ctx = {
        .ringbuf = xRingbufferCreate(UART_DMA_OTA_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF),
        .rx_eof = false,
        .rx_overflow = false,
    };
    assert(ctx.ringbuf);

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_event_cbs,
    };

    ESP_ERROR_CHECK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, &ctx));

    perform_ota_update(uhci_ctrl, &ctx);

    ESP_ERROR_CHECK(uhci_del_controller(uhci_ctrl));

    vRingbufferDelete(ctx.ringbuf);
    ESP_LOGI(TAG, "OTA update successful. Rebooting...");
    esp_restart();
}
