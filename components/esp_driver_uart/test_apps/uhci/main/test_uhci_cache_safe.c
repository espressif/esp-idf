/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Cache-safe UHCI test. Built only when CONFIG_UHCI_ISR_CACHE_SAFE is set (see main/CMakeLists.txt),
// so uhci_receive() can be called from the RX-done callback while the flash cache is disabled.

#include <assert.h>
#include "unity.h"
#include "test_utils.h"
#include "unity_test_utils_cache.h"
#include "esp_attr.h"
#include "esp_rom_sys.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/uhci.h"

#define DATA_LENGTH 1024
#define EX_UART_NUM 1
#define UART_TX_IO 2

#define CACHE_SAFE_BURST_SIZE 64

typedef struct {
    TaskHandle_t task_to_notify;
    uint8_t *bufs[2];       // double buffer, alternately armed
    size_t buf_size;
    int cur;                // buffer index currently armed
    volatile bool stop_rearm; // stop re-arming reception from the ISR
    volatile size_t recv_size;
    const uint8_t *recv_data;
} uhci_cache_safe_ctx_t;

// Runs in ISR context with the cache disabled. Re-arm reception with the alternate buffer
// (this is the uhci_receive() call under test) and notify the task.
IRAM_ATTR static bool s_uhci_rx_cache_safe_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    uhci_cache_safe_ctx_t *ctx = (uhci_cache_safe_ctx_t *)user_ctx;
    BaseType_t xTaskWoken = pdFALSE;

    if (edata->flags.totally_received) {
        ctx->recv_size = edata->recv_size;
        ctx->recv_data = edata->data;
        if (!ctx->stop_rearm) {
            ctx->cur ^= 1;
            uhci_receive(uhci_ctrl, ctx->bufs[ctx->cur], ctx->buf_size);
        }
        vTaskNotifyGiveFromISR(ctx->task_to_notify, &xTaskWoken);
    }
    return xTaskWoken == pdTRUE;
}

// Holds the cache disabled long enough for the primed transmission to loop back, so the RX
// idle-EOF interrupt fires (and re-arms reception) entirely within this window.
IRAM_ATTR static void s_uhci_hold_cache_disabled(void *args)
{
    esp_rom_delay_us(5000);
}

TEST_CASE("UHCI receive from ISR works with cache disabled", "[uhci]")
{
    uart_config_t uart_config = {
        .baud_rate = 2 * 1000 * 1000,
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
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    uhci_cache_safe_ctx_t ctx = {
        .task_to_notify = xTaskGetCurrentTaskHandle(),
        .buf_size = DATA_LENGTH / 4,
        .cur = 0,
    };
    for (int i = 0; i < 2; i++) {
        ctx.bufs[i] = heap_caps_calloc(1, ctx.buf_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        assert(ctx.bufs[i]);
    }

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_cache_safe_cbs,
    };
    TEST_ESP_OK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, &ctx));

    uint8_t data_wr[CACHE_SAFE_BURST_SIZE];
    for (int i = 0; i < CACHE_SAFE_BURST_SIZE; i++) {
        data_wr[i] = i;
    }

    // Arm reception, then start a transmission that will loop back. Disable the cache right away:
    // the transmit finishes and the RX idle-EOF interrupt fires within the cache-disabled window,
    // so uhci_receive() runs from the ISR while the cache is off.
    TEST_ESP_OK(uhci_receive(uhci_ctrl, ctx.bufs[0], ctx.buf_size));
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, CACHE_SAFE_BURST_SIZE));
    unity_utils_run_cache_disable_stub(s_uhci_hold_cache_disabled, NULL);

    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(CACHE_SAFE_BURST_SIZE, ctx.recv_size);
    for (int i = 0; i < CACHE_SAFE_BURST_SIZE; i++) {
        TEST_ASSERT_EQUAL(data_wr[i], ctx.recv_data[i]);
    }

    // A second receive was re-armed from the ISR. Stop re-arming and feed it once more so it
    // finishes naturally, then wait for it to complete before deleting the controller.
    ctx.stop_rearm = true;
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, CACHE_SAFE_BURST_SIZE));
    TEST_ASSERT_NOT_EQUAL(0, ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1000)));
    TEST_ASSERT_EQUAL(CACHE_SAFE_BURST_SIZE, ctx.recv_size);
    for (int i = 0; i < CACHE_SAFE_BURST_SIZE; i++) {
        TEST_ASSERT_EQUAL(data_wr[i], ctx.recv_data[i]);
    }

    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    for (int i = 0; i < 2; i++) {
        free(ctx.bufs[i]);
    }
}
