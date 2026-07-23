/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <sys/param.h>
#include "unity.h"
#include "test_utils.h"
#include "unity_test_utils_cache.h"
#include "esp_rom_sys.h"
#include "driver/uart.h"
#include "driver/uhci.h"
#include "hal/gdma_periph.h"
#include "hal/uart_ll.h"

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

TEST_CASE("UHCI controller install-uninstall test", "[uhci]")
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

TEST_CASE("UHCI receive/transmit reject invalid buffer size", "[uhci]")
{
    uhci_controller_config_t uhci_cfg = {
        .uart_port = EX_UART_NUM,
        .tx_trans_queue_depth = 3,
        .max_receive_internal_mem = 2 * 1024,
        .max_transmit_size = 2 * 1024,
        .max_transmit_buffer_count = 2,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    const size_t oversize = 5 * 1024;
    uint8_t *big_buf = heap_caps_calloc(1, oversize, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(big_buf);

    // -------- uhci_receive --------

    // buffer_size == 0 must be rejected.
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_receive(uhci_ctrl, big_buf, 0));

    // Excessively large buffers must be rejected.
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_receive(uhci_ctrl, big_buf, oversize));

    // -------- uhci_transmit / uhci_multi_buffer_transmit --------

    // write_size == 0 must be rejected.
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_transmit(uhci_ctrl, big_buf, 0));

    // A single write_size exceeding max_transmit_size must be rejected.
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_transmit(uhci_ctrl, big_buf, oversize));

    // Same limit applies to the combined size of uhci_multi_buffer_transmit segments, even
    // though each individual segment is within max_transmit_size.
    uhci_transmit_buffer_info_t buf_info[2] = {
        { .write_buffer = big_buf, .buffer_size = uhci_cfg.max_transmit_size },
        { .write_buffer = big_buf, .buffer_size = uhci_cfg.max_transmit_size },
    };
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_multi_buffer_transmit(uhci_ctrl, buf_info, 2));

    // A NULL or zero-size segment inside the array must also be rejected.
    buf_info[1].write_buffer = NULL;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_multi_buffer_transmit(uhci_ctrl, buf_info, 2));
    buf_info[1].write_buffer = big_buf;
    buf_info[1].buffer_size = 0;
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_multi_buffer_transmit(uhci_ctrl, buf_info, 2));

    free(big_buf);

    // The controller must still be in a clean, deletable state after all the rejected
    // calls above, which confirms the RX/TX FSMs were correctly rolled back on error.
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
    void **args = (void **)arg;
    uhci_controller_handle_t uhci_ctrl = (uhci_controller_handle_t)args[0];
    SemaphoreHandle_t exit_sema = (SemaphoreHandle_t)args[1];
    int trans_count = *(int *)args[2];

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

    uhci_event_t evt;

    for (int i = 0; i < trans_count; i++) {
        TEST_ESP_OK(uhci_receive(uhci_ctrl, pdata, DATA_LENGTH / 4));
        while (1) {
            if (xQueueReceive(ctx->uhci_queue, &evt, portMAX_DELAY) == pdTRUE) {
                if (evt == UHCI_EVT_EOF) {
                    disp_buf(receive_data, ctx->receive_size);
                    for (int i = 0; i < ctx->receive_size; i++) {
                        TEST_ASSERT_EQUAL(receive_data[i], (uint8_t)i);
                    }
                    printf("Received size: %d\n", ctx->receive_size);
                    break;
                }
            }
        }

        ctx->receive_size = 0;
        ctx->p_receive_data = receive_data;
        memset(receive_data, 0, DATA_LENGTH);
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
    // Tying TX to RX through the GPIO matrix can latch a spurious byte into the RX FIFO. Let the
    // line settle then drop it, otherwise it prepends a bogus 0x00 to the received data.
    vTaskDelay(pdMS_TO_TICKS(20));
    uart_ll_rxfifo_rst(UART_LL_GET_HW(EX_UART_NUM));

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

    int trans_count = 2;
    void *args[] = { uhci_ctrl, exit_sema, &trans_count };
    xTaskCreate(uhci_receive_test, "uhci_receive_test", 4096 * 2, args, 5, NULL);

    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, 10));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    xSemaphoreTake(exit_sema, portMAX_DELAY);
    vTaskDelay(2);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    vSemaphoreDelete(exit_sema);
}

TEST_CASE("UHCI write and receive with length eof", "[uhci]")
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
    // Tying TX to RX through the GPIO matrix can latch a spurious byte into the RX FIFO. Let the
    // line settle then drop it, otherwise it prepends a bogus 0x00 to the received data.
    vTaskDelay(pdMS_TO_TICKS(20));
    uart_ll_rxfifo_rst(UART_LL_GET_HW(EX_UART_NUM));

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

    int trans_count = 1;
    void *args[] = { uhci_ctrl, exit_sema, &trans_count };
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

static void uhci_fill_pattern(uint8_t *buf, size_t len, uint8_t start)
{
    for (size_t i = 0; i < len; i++) {
        buf[i] = (uint8_t)(start + i);
    }
}

// ---------------------------------------------------------------------------
// Re-arm uhci_receive() from the RX-done callback (ISR context)
// ---------------------------------------------------------------------------

#define REARM_BURST_SIZE  64
#define REARM_BURST_COUNT 4

typedef struct {
    QueueHandle_t evt_queue;    // carries the index of the just-filled buffer
    uint8_t *bufs[2];           // double buffer, alternately armed
    size_t buf_size;
    int cur;                    // buffer index currently armed
    int done;                   // number of completed receptions
} uhci_rearm_ctx_t;

typedef struct {
    int buf_idx;
    size_t size;
    const uint8_t *data;    // actual DMA data pointer (may be cache-line aligned inside the buffer)
} rearm_evt_t;

// This callback runs in ISR context. On EOF it immediately re-arms reception with the
// other buffer (so RX never idles) and hands the filled buffer to the task for processing.
IRAM_ATTR static bool s_uhci_rx_rearm_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    uhci_rearm_ctx_t *ctx = (uhci_rearm_ctx_t *)user_ctx;
    BaseType_t xTaskWoken = pdFALSE;

    if (edata->flags.totally_received) {
        rearm_evt_t evt = { .buf_idx = ctx->cur, .size = edata->recv_size, .data = edata->data };
        // Re-arm with the alternate buffer from ISR (except after the last expected burst, so the
        // controller can be deleted cleanly), then let the task consume the just-filled one.
        if (++ctx->done < REARM_BURST_COUNT) {
            ctx->cur ^= 1;
            uhci_receive(uhci_ctrl, ctx->bufs[ctx->cur], ctx->buf_size);
        }
        xQueueSendFromISR(ctx->evt_queue, &evt, &xTaskWoken);
    }
    return xTaskWoken == pdTRUE;
}

static void uhci_rearm_receive_test(void *arg)
{
    void **args = (void **)arg;
    uhci_controller_handle_t uhci_ctrl = (uhci_controller_handle_t)args[0];
    SemaphoreHandle_t exit_sema = (SemaphoreHandle_t)args[1];

    uhci_rearm_ctx_t *ctx = heap_caps_calloc(1, sizeof(uhci_rearm_ctx_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    assert(ctx);
    ctx->evt_queue = xQueueCreate(REARM_BURST_COUNT + 2, sizeof(rearm_evt_t));
    assert(ctx->evt_queue);
    ctx->buf_size = DATA_LENGTH / 4;
    for (int i = 0; i < 2; i++) {
        ctx->bufs[i] = heap_caps_calloc(1, ctx->buf_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        assert(ctx->bufs[i]);
    }

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_rearm_cbs,
    };
    TEST_ESP_OK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, ctx));

    // Arm the first buffer from task context; subsequent re-arms happen inside the ISR callback.
    ctx->cur = 0;
    TEST_ESP_OK(uhci_receive(uhci_ctrl, ctx->bufs[0], ctx->buf_size));

    rearm_evt_t evt;
    for (int i = 0; i < REARM_BURST_COUNT; i++) {
        TEST_ASSERT(xQueueReceive(ctx->evt_queue, &evt, portMAX_DELAY) == pdTRUE);
        printf("burst %d filled buffer %d, size %d\n", i, evt.buf_idx, (int)evt.size);
        TEST_ASSERT_EQUAL(REARM_BURST_SIZE, evt.size);
        for (int j = 0; j < evt.size; j++) {
            TEST_ASSERT(evt.data[j] == (uint8_t)j);
        }
    }

    vQueueDelete(ctx->evt_queue);
    for (int i = 0; i < 2; i++) {
        free(ctx->bufs[i]);
    }
    free(ctx);
    xSemaphoreGive(exit_sema);
    vTaskDelete(NULL);
}

TEST_CASE("UHCI re-arm receive from ISR callback", "[uhci]")
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
    // Tying TX to RX through the GPIO matrix can latch a spurious byte into the RX FIFO. Let the
    // line settle then drop it, otherwise it becomes a bogus 1-byte "frame 0" ahead of the real data.
    vTaskDelay(pdMS_TO_TICKS(20));
    uart_ll_rxfifo_rst(UART_LL_GET_HW(EX_UART_NUM));

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
    xTaskCreate(uhci_rearm_receive_test, "uhci_rearm_receive_test", 4096 * 2, args, 5, NULL);
    // Give the receiver task time to arm the first buffer before transmitting.
    vTaskDelay(100 / portTICK_PERIOD_MS);

    uint8_t data_wr[REARM_BURST_SIZE];
    for (int i = 0; i < REARM_BURST_SIZE; i++) {
        data_wr[i] = i;
    }

    // Each burst is followed by an idle gap so the RX side generates an idle EOF and the
    // ISR callback re-arms reception with the next buffer.
    for (int i = 0; i < REARM_BURST_COUNT; i++) {
        TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, REARM_BURST_SIZE));
        uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    xSemaphoreTake(exit_sema, portMAX_DELAY);
    vTaskDelay(2);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    vSemaphoreDelete(exit_sema);
}

// ---------------------------------------------------------------------------
// Continuous reception: arm once with uhci_start_receive_continuous(), the driver keeps the DMA running
// across EOFs and each frame lands in the next slot of the ring, no re-arm between frames.
// ---------------------------------------------------------------------------
#define CONT_BURST_SIZE  600  // larger than a single RX DMA node, so each frame spans several nodes
#define CONT_BURST_COUNT 6    // > RX DMA node count, so the ring wraps at least once

typedef struct {
    QueueHandle_t done_queue;   // carries the reassembled frame length
    uint8_t *reasm;             // linear reassembly buffer for the current frame
    size_t reasm_len;           // bytes accumulated so far for the current frame
} cont_ctx_t;

// Runs in ISR context. A frame larger than one DMA node arrives as several node-sized "partial"
// events (totally_received == false) followed by the EOF event, so copy every chunk into a linear
// buffer to reassemble the frame in order (also covers frames crossing the ring wrap-around).
IRAM_ATTR static bool s_uhci_rx_continuous_cbs(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    cont_ctx_t *ctx = (cont_ctx_t *)user_ctx;
    BaseType_t xTaskWoken = pdFALSE;

    if (ctx->reasm_len + edata->recv_size <= DATA_LENGTH) {
        memcpy(ctx->reasm + ctx->reasm_len, edata->data, edata->recv_size);
        // Only count bytes actually copied so the reported length stays consistent with the buffer.
        ctx->reasm_len += edata->recv_size;
    }
    if (edata->flags.totally_received) {
        size_t total = ctx->reasm_len;
        ctx->reasm_len = 0;
        xQueueSendFromISR(ctx->done_queue, &total, &xTaskWoken);
    }
    return xTaskWoken == pdTRUE;
}

TEST_CASE("UHCI continuous receive keeps DMA running across frames", "[uhci]")
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
    // Tying TX to RX through the GPIO matrix can latch a spurious byte into the RX FIFO (seen when
    // this test re-runs). Let the line settle then drop it, otherwise it becomes a bogus 1-byte
    // "frame 0" ahead of the real data.
    vTaskDelay(pdMS_TO_TICKS(20));
    uart_ll_rxfifo_rst(UART_LL_GET_HW(EX_UART_NUM));

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

    cont_ctx_t ctx = {0};
    ctx.done_queue = xQueueCreate(CONT_BURST_COUNT + 2, sizeof(size_t));
    TEST_ASSERT_NOT_NULL(ctx.done_queue);
    // Sized to the whole ring so a buggy over-long frame can't overflow it.
    ctx.reasm = heap_caps_calloc(1, DATA_LENGTH, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ctx.reasm);

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = s_uhci_rx_continuous_cbs,
    };
    TEST_ESP_OK(uhci_register_event_callbacks(uhci_ctrl, &uhci_cbs, &ctx));

    // A single ring buffer, split across the RX DMA nodes. Each frame is larger than one node so it
    // spans several, and consecutive frames wrap the ring around.
    uint8_t *ring = heap_caps_calloc(1, DATA_LENGTH, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    TEST_ASSERT_NOT_NULL(ring);

    // Arm continuous reception ONCE. Note: no uhci_receive() call between frames below.
    TEST_ESP_OK(uhci_start_receive_continuous(uhci_ctrl, ring, DATA_LENGTH));

    uint8_t data_wr[CONT_BURST_SIZE];
    for (int i = 0; i < CONT_BURST_COUNT; i++) {
        // Distinct content per frame so we can verify ordering and correctness.
        for (int j = 0; j < CONT_BURST_SIZE; j++) {
            data_wr[j] = (uint8_t)(i + j);
        }
        TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, CONT_BURST_SIZE));
        uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
        // Idle gap so the RX side raises an idle EOF for this frame.
        vTaskDelay(pdMS_TO_TICKS(50));

        // The whole frame must arrive (reassembled from its node chunks) though RX was never re-armed.
        size_t total = 0;
        TEST_ASSERT(xQueueReceive(ctx.done_queue, &total, pdMS_TO_TICKS(1000)) == pdTRUE);
        printf("frame %d received, size %d\n", i, (int)total);
        TEST_ASSERT_EQUAL(CONT_BURST_SIZE, total);
        for (int j = 0; j < CONT_BURST_SIZE; j++) {
            TEST_ASSERT_EQUAL_HEX8((uint8_t)(i + j), ctx.reasm[j]);
        }
    }

    TEST_ESP_OK(uhci_stop_receive(uhci_ctrl));
    vTaskDelay(2);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    free(ring);
    free(ctx.reasm);
    vQueueDelete(ctx.done_queue);
}

TEST_CASE("UHCI single buffer and multi buffer transmit interleaved", "[uhci]")
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
        .max_transmit_buffer_count = 3,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    uhci_controller_handle_t uhci_ctrl;
    SemaphoreHandle_t exit_sema = xSemaphoreCreateBinary();
    TEST_ESP_OK(uhci_new_controller(&uhci_cfg, &uhci_ctrl));

    // 4 transactions in total: single buffer, multi buffer (3 segments), single buffer, multi buffer (2 segments)
    int trans_count = 4;
    void *args[] = { uhci_ctrl, exit_sema, &trans_count };
    xTaskCreate(uhci_receive_test, "uhci_receive_test", 4096 * 2, args, 5, NULL);

    uint8_t data_wr[DATA_LENGTH];
    for (int i = 0; i < DATA_LENGTH; i++) {
        data_wr[i] = i;
    }

    // 1) plain single buffer transmit
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, DATA_LENGTH));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    // Idle gap so RX side sees each transaction as a separate idle-eof event
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // 2) multi buffer transmit with 3 discontinuous segments
    size_t seg_sizes_a[3] = {128, 64, 108};
    uint8_t *segs_a[3];
    uhci_transmit_buffer_info_t buf_info_a[3];
    size_t offset = 0;
    for (int i = 0; i < 3; i++) {
        segs_a[i] = heap_caps_calloc(1, seg_sizes_a[i], MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        assert(segs_a[i]);
        uhci_fill_pattern(segs_a[i], seg_sizes_a[i], (uint8_t)offset);
        buf_info_a[i].write_buffer = segs_a[i];
        buf_info_a[i].buffer_size = seg_sizes_a[i];
        offset += seg_sizes_a[i];
    }
    TEST_ESP_OK(uhci_multi_buffer_transmit(uhci_ctrl, buf_info_a, 3));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    for (int i = 0; i < 3; i++) {
        free(segs_a[i]);
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // 3) plain single buffer transmit again
    TEST_ESP_OK(uhci_transmit(uhci_ctrl, data_wr, 200));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // 4) multi buffer transmit with 2 discontinuous segments
    size_t seg_sizes_b[2] = {256, 96};
    uint8_t *segs_b[2];
    uhci_transmit_buffer_info_t buf_info_b[2];
    offset = 0;
    for (int i = 0; i < 2; i++) {
        segs_b[i] = heap_caps_calloc(1, seg_sizes_b[i], MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        assert(segs_b[i]);
        uhci_fill_pattern(segs_b[i], seg_sizes_b[i], (uint8_t)offset);
        buf_info_b[i].write_buffer = segs_b[i];
        buf_info_b[i].buffer_size = seg_sizes_b[i];
        offset += seg_sizes_b[i];
    }
    TEST_ESP_OK(uhci_multi_buffer_transmit(uhci_ctrl, buf_info_b, 2));
    uhci_wait_all_tx_transaction_done(uhci_ctrl, portMAX_DELAY);
    for (int i = 0; i < 2; i++) {
        free(segs_b[i]);
    }

    // 5) exceeding max_transmit_buffer_count must be rejected with ESP_ERR_INVALID_ARG,
    // without consuming a transaction descriptor or touching any buffer.
    uhci_transmit_buffer_info_t buf_info_over[4] = {0};
    TEST_ESP_ERR(ESP_ERR_INVALID_ARG, uhci_multi_buffer_transmit(uhci_ctrl, buf_info_over, 4));

    xSemaphoreTake(exit_sema, portMAX_DELAY);
    vTaskDelay(2);
    TEST_ESP_OK(uhci_del_controller(uhci_ctrl));
    vSemaphoreDelete(exit_sema);
}

#if CONFIG_SPIRAM
#if GDMA_LL_GET(AHB_PSRAM_CAPABLE)
static void uhci_receive_test_in_psram(void *arg)
{
    void **args = (void **)arg;
    uhci_controller_handle_t uhci_ctrl = (uhci_controller_handle_t)args[0];
    SemaphoreHandle_t exit_sema = (SemaphoreHandle_t)args[1];

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
