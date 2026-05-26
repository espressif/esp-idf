/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <string.h>

#include "sdkconfig.h"

#if CONFIG_BT_ENABLED && CONFIG_BT_CTRL_HCI_MODE_UART_H4 && CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "esp_attr.h"
#include "esp_bt.h"
#include "esp_err.h"
#include "esp_heap_caps.h"
#include "esp_log.h"
#include "esp_task.h"
#include "driver/uart.h"
#include "driver/uhci.h"

#include "hci_uart_h4.h"

#define BT_LOG_TAG                      "BT_HCI_UART"

#define BTDM_HCI_UART_RX_THRS           (120)
#define BTDM_HCI_UHCI_MAX_TX_BUF_SIZE   (512)
/*
 * Continuous RX ring. uhci_controller_config_t.max_receive_internal_mem decides how many RX
 * DMA descriptors UHCI allocates, and uhci_start_receive_continuous() splits the ring across
 * them. Keep it large enough for at least two nodes, so the DMA can fill one node while the
 * ISR still copies the previous one instead of overwriting it.
 */
#define BTDM_HCI_UHCI_RX_DMA_RING_SIZE  (4096)
/* BYTEBUF has wrap/management overhead; keep this larger than one DMA ring burst. */
#define BTDM_HCI_UHCI_RX_RINGBUF_SIZE   (BTDM_HCI_UHCI_RX_DMA_RING_SIZE * 2)
#define BTDM_HCI_UART_TL_TASK_STACK_TX          (3072)
#define BTDM_HCI_UART_TL_TASK_STACK_RX          (4096)
#define BTDM_HCI_UART_TL_TASK_EXIT_POLL_MS      (1)
#define BTDM_HCI_UART_TL_TASK_EXIT_TIMEOUT_MS   (1000)
#define BTDM_HCI_UART_TL_UHCI_DEL_RETRIES       (20)
#define BTDM_HCI_UART_TL_UHCI_DEL_RETRY_MS      (10)

struct btdm_hci_uart_txrxchannel {
    esp_bt_hci_tl_callback_t callback;
    void *arg;
    uint8_t *buf;
    uint32_t size;
};

struct btdm_hci_uart_env {
    struct btdm_hci_uart_txrxchannel tx;
    struct btdm_hci_uart_txrxchannel rx;
    uhci_controller_handle_t uhci_handle;
    TaskHandle_t tx_task;
    TaskHandle_t rx_task;
    SemaphoreHandle_t rx_process_sema;
    SemaphoreHandle_t tx_process_sema;
    RingbufHandle_t ringbufhandle;
    uint8_t *rx_dma_ring;             /*!< Storage for uhci_start_receive_continuous(); valid until uhci_del_controller(). */
    volatile bool rx_copy_overflow;   /*!< Set in ISR when ringbuf cannot accept a DMA slice. */
    volatile bool tl_stopping;
    volatile bool tx_task_exited;
    volatile bool rx_task_exited;
    bool tl_installed;
};

static struct btdm_hci_uart_env s_btdm_hci_uart_env;

/* Set around UHCI callbacks so GiveFromISR yield is returned to the UHCI ISR, not taken here. */
static volatile bool s_in_uhci_cb;
static volatile bool s_uhci_cb_need_yield;

static void btdm_hci_uart_tl_cleanup(void);

/**
 * @brief End the RX DMA session so the UHCI RX ISR can no longer run.
 *
 * ESP_ERR_INVALID_STATE means a receive is concurrently being armed, in which case the
 * stop must be retried, otherwise the DMA keeps running after this returns.
 */
static void btdm_hci_uart_tl_stop_rx(void)
{
    if (!s_btdm_hci_uart_env.uhci_handle) {
        return;
    }

    for (int retry = 0; retry < 10; retry++) {
        esp_err_t ret = uhci_stop_receive(s_btdm_hci_uart_env.uhci_handle);
        if (ret == ESP_OK) {
            return;
        }
        if (ret != ESP_ERR_INVALID_STATE) {
            ESP_LOGW(BT_LOG_TAG, "HCI UART rx stop failed: %s", esp_err_to_name(ret));
            return;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    ESP_LOGE(BT_LOG_TAG, "HCI UART rx stop timeout");
}

static int btdm_hci_uart_tl_init(void);
static void btdm_hci_uart_tl_deinit(void);
static void btdm_hci_uart_tl_recv_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg);
static void btdm_hci_uart_tl_send_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg);
static void btdm_hci_uart_tl_flow_on(void);
static bool btdm_hci_uart_tl_flow_off(void);
static void btdm_hci_uart_tl_finish_transfers(void);

static esp_bt_hci_tl_t s_btdm_hci_uart_tl_funcs = {
    ._magic = ESP_BT_HCI_TL_MAGIC_VALUE,
    ._version = ESP_BT_HCI_TL_VERSION,
    ._reserved = 0,
    ._open = (void *)btdm_hci_uart_tl_init,
    ._close = (void *)btdm_hci_uart_tl_deinit,
    ._finish_transfers = (void *)btdm_hci_uart_tl_finish_transfers,
    ._recv = (void *)btdm_hci_uart_tl_recv_async,
    ._send = (void *)btdm_hci_uart_tl_send_async,
    ._flow_on = (void *)btdm_hci_uart_tl_flow_on,
    ._flow_off = (void *)btdm_hci_uart_tl_flow_off,
};

esp_bt_hci_tl_t *btdm_hci_uart_tl_get_funcs(void)
{
    return &s_btdm_hci_uart_tl_funcs;
}

static int btdm_hci_uart_tl_init(void)
{
    return 0;
}

static void btdm_hci_uart_tl_deinit(void)
{
    btdm_hci_uart_tl_uninstall();
}

static IRAM_ATTR void btdm_hci_uart_tl_sem_give(SemaphoreHandle_t sem)
{
    if (sem == NULL) {
        return;
    }

    if (xPortInIsrContext()) {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(sem, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken == pdTRUE) {
            if (s_in_uhci_cb) {
                s_uhci_cb_need_yield = true;
            } else {
                portYIELD_FROM_ISR();
            }
        }
    } else {
        xSemaphoreGive(sem);
    }
}

static IRAM_ATTR void btdm_hci_uart_tl_complete_request(struct btdm_hci_uart_txrxchannel *ch, uint8_t status)
{
    esp_bt_hci_tl_callback_t callback = ch->callback;
    void *arg = ch->arg;

    ch->callback = NULL;
    ch->arg = NULL;
    ch->size = 0;
    ch->buf = NULL;

    if (callback == NULL) {
        return;
    }

    callback(arg, status);
    if (!s_btdm_hci_uart_env.tl_stopping) {
        esp_bt_h4tl_eif_io_event_notify(1);
    }
}

static IRAM_ATTR void btdm_hci_uart_tl_recv_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg)
{
    assert(buf != NULL);
    assert(size != 0);
    assert(callback != NULL);

    if (s_btdm_hci_uart_env.tl_stopping || s_btdm_hci_uart_env.rx_process_sema == NULL) {
        /* Do not overwrite an in-flight request; just release this caller. */
        callback(arg, ESP_BT_HCI_TL_STATUS_OK);
        return;
    }

    s_btdm_hci_uart_env.rx.callback = callback;
    s_btdm_hci_uart_env.rx.arg = arg;
    s_btdm_hci_uart_env.rx.size = size;
    s_btdm_hci_uart_env.rx.buf = buf;

    btdm_hci_uart_tl_sem_give(s_btdm_hci_uart_env.rx_process_sema);
}

static IRAM_ATTR void btdm_hci_uart_tl_send_async(uint8_t *buf, uint32_t size, esp_bt_hci_tl_callback_t callback, void *arg)
{
    assert(buf != NULL);
    assert(size != 0);
    assert(callback != NULL);

    if (s_btdm_hci_uart_env.tl_stopping || s_btdm_hci_uart_env.tx_process_sema == NULL) {
        callback(arg, ESP_BT_HCI_TL_STATUS_OK);
        return;
    }

    s_btdm_hci_uart_env.tx.callback = callback;
    s_btdm_hci_uart_env.tx.arg = arg;
    s_btdm_hci_uart_env.tx.size = size;
    s_btdm_hci_uart_env.tx.buf = buf;

    btdm_hci_uart_tl_sem_give(s_btdm_hci_uart_env.tx_process_sema);
}

static void btdm_hci_uart_tl_flow_on(void)
{
}

static bool btdm_hci_uart_tl_flow_off(void)
{
    return true;
}

static esp_err_t btdm_hci_uart_tl_wait_tx_done(void)
{
    if (!s_btdm_hci_uart_env.uhci_handle) {
        return ESP_OK;
    }

    esp_err_t ret = uhci_wait_all_tx_transaction_done(s_btdm_hci_uart_env.uhci_handle, 1000);
    if (ret != ESP_OK) {
        ESP_LOGW(BT_LOG_TAG, "HCI UART wait tx done failed: %s", esp_err_to_name(ret));
    }
    return ret;
}

static void btdm_hci_uart_tl_finish_transfers(void)
{
    (void)btdm_hci_uart_tl_wait_tx_done();
}

static IRAM_ATTR bool btdm_hci_uhci_rx_event_cb_stub(uhci_controller_handle_t uhci_ctrl,
                                                     const uhci_rx_event_data_t *edata, void *user_ctx)
{
    /* Teardown stub: no task is woken, so do not request a yield from the UHCI ISR. */
    return false;
}

static IRAM_ATTR bool btdm_hci_uhci_tx_done_cb_stub(uhci_controller_handle_t uhci_ctrl,
                                                      const uhci_tx_done_event_data_t *edata, void *user_ctx)
{
    /* Teardown stub: no task is woken, so do not request a yield from the UHCI ISR. */
    return false;
}

static void btdm_hci_uart_tl_detach_uhci_callbacks(void)
{
    if (!s_btdm_hci_uart_env.uhci_handle) {
        return;
    }

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = btdm_hci_uhci_rx_event_cb_stub,
        .on_tx_trans_done = btdm_hci_uhci_tx_done_cb_stub,
    };
    esp_err_t ret = uhci_register_event_callbacks(s_btdm_hci_uart_env.uhci_handle, &uhci_cbs, NULL);
    if (ret != ESP_OK) {
        ESP_LOGW(BT_LOG_TAG, "HCI UART detach UHCI callbacks failed: %s", esp_err_to_name(ret));
    }
}

static IRAM_ATTR bool btdm_hci_uhci_tx_done_cb(uhci_controller_handle_t uhci_ctrl,
                                               const uhci_tx_done_event_data_t *edata, void *user_ctx)
{
    (void)uhci_ctrl;
    (void)user_ctx;
    (void)edata;

    s_in_uhci_cb = true;
    s_uhci_cb_need_yield = false;

    if (s_btdm_hci_uart_env.tx.callback) {
        btdm_hci_uart_tl_complete_request(&s_btdm_hci_uart_env.tx, ESP_BT_HCI_TL_STATUS_OK);
    }

    bool need_yield = s_uhci_cb_need_yield;
    s_in_uhci_cb = false;
    return need_yield;
}

/**
 * @brief UHCI on_rx_trans_event callback (ISR context, must be non-blocking).
 *
 * edata->data points into rx_dma_ring and is only guaranteed readable during this callback:
 * continuous RX does not stop the DMA at EOF, so the same node is overwritten on wrap-around.
 * Copy the slice out immediately and let the rx task hand the bytes to the controller. Both
 * partial-node and EOF events carry data; the H4 layer decides packet boundaries.
 */
static IRAM_ATTR bool btdm_hci_uhci_rx_event_cb(uhci_controller_handle_t uhci_ctrl,
                                                const uhci_rx_event_data_t *edata, void *user_ctx)
{
    (void)uhci_ctrl;
    (void)user_ctx;

    /* Teardown has already stopped the RX session; the ringbuffer has no consumer left. */
    if (s_btdm_hci_uart_env.tl_stopping || s_btdm_hci_uart_env.ringbufhandle == NULL) {
        return false;
    }

    /* Abnormal EOF: UHCI reports data == NULL and recv_size == 0. Keep the session running. */
    if (!edata->data || edata->recv_size == 0) {
        return false;
    }

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    /*
     * xRingbufferSendFromISR copies the DMA buffer slice. If this fails (ringbuffer full
     * under burst traffic, delayed scheduling, or cache disabled during flash ops), drop
     * the slice and keep the UHCI session running. assert() in this ISR would panic.
     */
    if (xRingbufferSendFromISR(s_btdm_hci_uart_env.ringbufhandle, edata->data, edata->recv_size,
                               &xHigherPriorityTaskWoken) != pdTRUE) {
        s_btdm_hci_uart_env.rx_copy_overflow = true;
    }

    return (xHigherPriorityTaskWoken == pdTRUE);
}

/**
 * @brief Last statement of a process task: publish that it left the shared resources.
 *
 * After the flag is set the task must not touch s_btdm_hci_uart_env anymore, so the
 * teardown path is free to delete the UHCI controller, the semaphores and the ringbuffer.
 */
static void btdm_hci_uart_tl_task_exit(volatile bool *exited)
{
    *exited = true;
    vTaskDelete(NULL);
}

/**
 * @brief Unblock whatever the process tasks are waiting on so they observe tl_stopping.
 *
 * The rx task blocks either on its request semaphore or inside xRingbufferReceiveUpTo();
 * a single dummy byte releases the latter and is discarded by the task.
 */
static void btdm_hci_uart_tl_wake_tasks(void)
{
    const uint8_t wake_byte = 0;

    btdm_hci_uart_tl_sem_give(s_btdm_hci_uart_env.tx_process_sema);
    btdm_hci_uart_tl_sem_give(s_btdm_hci_uart_env.rx_process_sema);
    if (s_btdm_hci_uart_env.ringbufhandle) {
        xRingbufferSend(s_btdm_hci_uart_env.ringbufhandle, &wake_byte, sizeof(wake_byte), 0);
    }
}

static void btdm_hci_uart_tl_flush_rx_ringbuf(void)
{
    size_t item_size = 0;
    uint8_t *data = NULL;

    if (s_btdm_hci_uart_env.ringbufhandle == NULL) {
        return;
    }

    while ((data = xRingbufferReceive(s_btdm_hci_uart_env.ringbufhandle, &item_size, 0)) != NULL) {
        vRingbufferReturnItem(s_btdm_hci_uart_env.ringbufhandle, data);
    }
}

static void btdm_hci_uhci_tx_process_task(void *arg)
{
    (void)arg;

    while (!s_btdm_hci_uart_env.tl_stopping) {
        xSemaphoreTake(s_btdm_hci_uart_env.tx_process_sema, portMAX_DELAY);
        if (s_btdm_hci_uart_env.tl_stopping) {
            break;
        }
        if (uhci_transmit(s_btdm_hci_uart_env.uhci_handle, s_btdm_hci_uart_env.tx.buf,
                          s_btdm_hci_uart_env.tx.size) != ESP_OK) {
            ESP_LOGE(BT_LOG_TAG, "HCI UART transmit failed");
            btdm_hci_uart_tl_complete_request(&s_btdm_hci_uart_env.tx, ESP_BT_HCI_TL_STATUS_OK);
        }
    }

    btdm_hci_uart_tl_task_exit(&s_btdm_hci_uart_env.tx_task_exited);
}

static void btdm_hci_uhci_rx_done_process_task(void *arg)
{
    size_t item_size = 0;
    uint8_t *data = NULL;

    (void)arg;

    xSemaphoreTake(s_btdm_hci_uart_env.rx_process_sema, portMAX_DELAY);
    while (!s_btdm_hci_uart_env.tl_stopping) {
        if (s_btdm_hci_uart_env.rx_copy_overflow) {
            ESP_LOGE(BT_LOG_TAG, "RX software ring buffer overflow, HCI stream may lose sync");
            s_btdm_hci_uart_env.rx_copy_overflow = false;
            /* Drop queued fragments so the ISR can copy again instead of staying full. */
            btdm_hci_uart_tl_flush_rx_ringbuf();
        }
        data = xRingbufferReceiveUpTo(s_btdm_hci_uart_env.ringbufhandle, &item_size, portMAX_DELAY, s_btdm_hci_uart_env.rx.size);
        if (s_btdm_hci_uart_env.tl_stopping) {
            /* Whatever was received here is the teardown wake-up byte, not HCI data. */
            if (data) {
                vRingbufferReturnItem(s_btdm_hci_uart_env.ringbufhandle, data);
            }
            break;
        }
        if (data == NULL) {
            /* Nothing was requested (rx.size == 0), wait for the next recv request. */
            if (s_btdm_hci_uart_env.rx_process_sema == NULL) {
                break;
            }
            xSemaphoreTake(s_btdm_hci_uart_env.rx_process_sema, portMAX_DELAY);
            continue;
        }

        memcpy(s_btdm_hci_uart_env.rx.buf, data, item_size);
        vRingbufferReturnItem(s_btdm_hci_uart_env.ringbufhandle, data);
        s_btdm_hci_uart_env.rx.size -= item_size;
        s_btdm_hci_uart_env.rx.buf += item_size;

        /* Packet may arrive as multiple DMA chunks; wait for the rest. */
        if (s_btdm_hci_uart_env.rx.size) {
            continue;
        }

        btdm_hci_uart_tl_complete_request(&s_btdm_hci_uart_env.rx, ESP_BT_HCI_TL_STATUS_OK);

        /* The callback may have triggered the teardown, so re-check before using the semaphore. */
        if (s_btdm_hci_uart_env.tl_stopping) {
            break;
        }
        xSemaphoreTake(s_btdm_hci_uart_env.rx_process_sema, portMAX_DELAY);
    }

    btdm_hci_uart_tl_task_exit(&s_btdm_hci_uart_env.rx_task_exited);
}

/**
 * @brief Ask both process tasks to leave their loops and wait until they did.
 *
 * Deleting them from the outside could strand a ringbuffer item or cut a task while it is
 * inside a UHCI call, so force delete is only a bounded fallback if a task never exits.
 * If this teardown runs on a process task, that task cannot exit until we return: do not
 * wait for it and do not vTaskDelete it (it will self-delete via task_exit).
 */
static void btdm_hci_uart_tl_tasks_stop(void)
{
    TaskHandle_t self = xTaskGetCurrentTaskHandle();
    int waited_ms = 0;
    bool tx_is_self;
    bool rx_is_self;

    s_btdm_hci_uart_env.tl_stopping = true;

    while (waited_ms < BTDM_HCI_UART_TL_TASK_EXIT_TIMEOUT_MS) {
        tx_is_self = s_btdm_hci_uart_env.tx_task && (s_btdm_hci_uart_env.tx_task == self);
        rx_is_self = s_btdm_hci_uart_env.rx_task && (s_btdm_hci_uart_env.rx_task == self);
        bool tx_done = !s_btdm_hci_uart_env.tx_task || s_btdm_hci_uart_env.tx_task_exited || tx_is_self;
        bool rx_done = !s_btdm_hci_uart_env.rx_task || s_btdm_hci_uart_env.rx_task_exited || rx_is_self;

        if (tx_done && rx_done) {
            break;
        }

        btdm_hci_uart_tl_wake_tasks();
        vTaskDelay(pdMS_TO_TICKS(BTDM_HCI_UART_TL_TASK_EXIT_POLL_MS));
        waited_ms += BTDM_HCI_UART_TL_TASK_EXIT_POLL_MS;
    }

    tx_is_self = s_btdm_hci_uart_env.tx_task && (s_btdm_hci_uart_env.tx_task == self);
    rx_is_self = s_btdm_hci_uart_env.rx_task && (s_btdm_hci_uart_env.rx_task == self);

    if (s_btdm_hci_uart_env.tx_task) {
        if (tx_is_self) {
            ESP_LOGW(BT_LOG_TAG, "HCI UART deinit from tx process task");
        } else if (!s_btdm_hci_uart_env.tx_task_exited) {
            ESP_LOGE(BT_LOG_TAG, "HCI UART tx process task exit timeout");
            vTaskDelete(s_btdm_hci_uart_env.tx_task);
        }
        s_btdm_hci_uart_env.tx_task = NULL;
    }
    if (s_btdm_hci_uart_env.rx_task) {
        if (rx_is_self) {
            ESP_LOGW(BT_LOG_TAG, "HCI UART deinit from rx process task");
        } else if (!s_btdm_hci_uart_env.rx_task_exited) {
            ESP_LOGE(BT_LOG_TAG, "HCI UART rx process task exit timeout");
            vTaskDelete(s_btdm_hci_uart_env.rx_task);
        }
        s_btdm_hci_uart_env.rx_task = NULL;
    }
}

static void btdm_hci_uart_tl_sw_resources_deinit(void)
{
    if (s_btdm_hci_uart_env.tx_process_sema) {
        vSemaphoreDelete(s_btdm_hci_uart_env.tx_process_sema);
        s_btdm_hci_uart_env.tx_process_sema = NULL;
    }
    if (s_btdm_hci_uart_env.rx_process_sema) {
        vSemaphoreDelete(s_btdm_hci_uart_env.rx_process_sema);
        s_btdm_hci_uart_env.rx_process_sema = NULL;
    }
    if (s_btdm_hci_uart_env.ringbufhandle) {
        vRingbufferDelete(s_btdm_hci_uart_env.ringbufhandle);
        s_btdm_hci_uart_env.ringbufhandle = NULL;
    }
}

static void btdm_hci_uart_tl_dma_ring_free(void)
{
    /* Only safe once UHCI no longer references the ring (controller deleted, or never armed). */
    if (s_btdm_hci_uart_env.rx_dma_ring) {
        heap_caps_free(s_btdm_hci_uart_env.rx_dma_ring);
        s_btdm_hci_uart_env.rx_dma_ring = NULL;
    }
}

static void btdm_hci_uart_tl_resources_deinit(void)
{
    btdm_hci_uart_tl_sw_resources_deinit();
    if (s_btdm_hci_uart_env.uhci_handle == NULL) {
        btdm_hci_uart_tl_dma_ring_free();
    }
}

static esp_err_t btdm_hci_uart_tl_del_controller(void)
{
    esp_err_t uhci_ret = ESP_OK;

    if (!s_btdm_hci_uart_env.uhci_handle) {
        return ESP_OK;
    }

    for (int retry = 0; retry < BTDM_HCI_UART_TL_UHCI_DEL_RETRIES; retry++) {
        (void)btdm_hci_uart_tl_wait_tx_done();
        uhci_ret = uhci_del_controller(s_btdm_hci_uart_env.uhci_handle);
        if (uhci_ret == ESP_OK) {
            s_btdm_hci_uart_env.uhci_handle = NULL;
            return ESP_OK;
        }
        vTaskDelay(pdMS_TO_TICKS(BTDM_HCI_UART_TL_UHCI_DEL_RETRY_MS));
    }

    ESP_LOGE(BT_LOG_TAG, "HCI UART UHCI delete failed: %s", esp_err_to_name(uhci_ret));
    return uhci_ret;
}

static void btdm_hci_uart_tl_cleanup(void)
{
    /*
     * Tear down in reverse-dependency order:
     * 1. stop the RX DMA session so the UHCI RX ISR stops feeding the ringbuffer;
     * 2. let both process tasks reach their exit point;
     * 3. detach callbacks, drain TX, delete the controller;
     * 4. complete leftover TL requests only when the corresponding DMA is idle;
     * 5. free software buffers always; free the DMA ring only after the controller is gone.
     *
     * tl_installed is cleared even if UHCI delete fails. A leftover controller is tracked
     * by uhci_handle so a later install/uninstall can retry instead of pretending we are up.
     */
    esp_err_t tx_wait_ret;
    esp_err_t del_ret;
    bool tx_idle;

    s_btdm_hci_uart_env.tl_stopping = true;
    btdm_hci_uart_tl_stop_rx();
    btdm_hci_uart_tl_tasks_stop();
    btdm_hci_uart_tl_detach_uhci_callbacks();
    tx_wait_ret = btdm_hci_uart_tl_wait_tx_done();
    del_ret = btdm_hci_uart_tl_del_controller();
    tx_idle = (tx_wait_ret == ESP_OK) || (del_ret == ESP_OK);

    /* RX DMA is already stopped; completing avoids a hung controller recv. */
    btdm_hci_uart_tl_complete_request(&s_btdm_hci_uart_env.rx, ESP_BT_HCI_TL_STATUS_OK);
    memset(&s_btdm_hci_uart_env.rx, 0, sizeof(s_btdm_hci_uart_env.rx));

    if (tx_idle) {
        btdm_hci_uart_tl_complete_request(&s_btdm_hci_uart_env.tx, ESP_BT_HCI_TL_STATUS_OK);
        memset(&s_btdm_hci_uart_env.tx, 0, sizeof(s_btdm_hci_uart_env.tx));
    }

    s_btdm_hci_uart_env.rx_copy_overflow = false;
    s_btdm_hci_uart_env.tl_installed = false;

    if (del_ret == ESP_OK) {
        btdm_hci_uart_tl_resources_deinit();
    } else {
        /* Keep uhci_handle and rx_dma_ring so a later uninstall/install can retry the delete. */
        btdm_hci_uart_tl_sw_resources_deinit();
    }
}

static esp_err_t btdm_hci_uhci_rx_prepare(void)
{
    /* Cleared here and not on teardown, so a task that outlived the teardown stays stopped. */
    s_btdm_hci_uart_env.tl_stopping = false;
    s_btdm_hci_uart_env.tx_task_exited = false;
    s_btdm_hci_uart_env.rx_task_exited = false;
    s_btdm_hci_uart_env.rx_copy_overflow = false;

    /* DMA + internal: UHCI/GDMA writes here, and the ISR reads it with the cache disabled. */
    s_btdm_hci_uart_env.rx_dma_ring = heap_caps_calloc(1, BTDM_HCI_UHCI_RX_DMA_RING_SIZE,
                                                       MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (s_btdm_hci_uart_env.rx_dma_ring == NULL) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART rx dma ring alloc failed");
        return ESP_ERR_NO_MEM;
    }

    s_btdm_hci_uart_env.ringbufhandle = xRingbufferCreate(BTDM_HCI_UHCI_RX_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
    if (s_btdm_hci_uart_env.ringbufhandle == NULL) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART ringbuffer create failed");
        return ESP_ERR_NO_MEM;
    }

    s_btdm_hci_uart_env.rx_process_sema = xSemaphoreCreateBinary();
    if (s_btdm_hci_uart_env.rx_process_sema == NULL) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART rx process semaphore create failed");
        return ESP_ERR_NO_MEM;
    }

    s_btdm_hci_uart_env.tx_process_sema = xSemaphoreCreateBinary();
    if (s_btdm_hci_uart_env.tx_process_sema == NULL) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART tx process semaphore create failed");
        return ESP_ERR_NO_MEM;
    }

    if (xTaskCreate(btdm_hci_uhci_tx_process_task, "btdm hci uart tx", BTDM_HCI_UART_TL_TASK_STACK_TX, NULL,
                    ESP_TASK_BT_CONTROLLER_PRIO, &s_btdm_hci_uart_env.tx_task) != pdPASS) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART tx process task create failed");
        s_btdm_hci_uart_env.tx_task = NULL;
        return ESP_ERR_NO_MEM;
    }

    if (xTaskCreate(btdm_hci_uhci_rx_done_process_task, "btdm hci uart rx", BTDM_HCI_UART_TL_TASK_STACK_RX, NULL,
                    ESP_TASK_BT_CONTROLLER_PRIO, &s_btdm_hci_uart_env.rx_task) != pdPASS) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART rx process task create failed");
        s_btdm_hci_uart_env.rx_task = NULL;
        return ESP_ERR_NO_MEM;
    }

    /*
     * Arm once: the DMA keeps running across EOFs, so no byte is lost in the gap between a
     * frame EOF and a re-arm. Do not call uhci_receive() / start again until uhci_stop_receive().
     */
    esp_err_t ret = uhci_start_receive_continuous(s_btdm_hci_uart_env.uhci_handle,
                                                  s_btdm_hci_uart_env.rx_dma_ring,
                                                  BTDM_HCI_UHCI_RX_DMA_RING_SIZE);
    if (ret != ESP_OK) {
        ESP_LOGE(BT_LOG_TAG, "HCI UART rx start failed: %s", esp_err_to_name(ret));
        return ret;
    }
    return ESP_OK;
}

void btdm_hci_uart_tl_uninstall(void)
{
    if (!s_btdm_hci_uart_env.tl_installed && s_btdm_hci_uart_env.uhci_handle == NULL) {
        return;
    }

    btdm_hci_uart_tl_cleanup();
}

esp_err_t btdm_hci_uart_tl_install(void)
{
    uart_config_t uart_config = {
        .baud_rate = CONFIG_BT_CTRL_HCI_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
#if CONFIG_BT_CTRL_HCI_UART_DTM_MODE
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#elif CONFIG_BT_CTRL_HCI_UART_FLOW_CTRL_EN
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
#else
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#endif
        .rx_flow_ctrl_thresh = BTDM_HCI_UART_RX_THRS,
        .source_clk = UART_SCLK_DEFAULT,
    };

    if (s_btdm_hci_uart_env.tl_installed) {
        return ESP_OK;
    }

    /* A previous uninstall may have failed to delete UHCI; retry before creating another. */
    if (s_btdm_hci_uart_env.uhci_handle) {
        btdm_hci_uart_tl_cleanup();
        if (s_btdm_hci_uart_env.uhci_handle) {
            return ESP_ERR_INVALID_STATE;
        }
    }

    ESP_ERROR_CHECK(uart_param_config(CONFIG_BT_CTRL_HCI_UART_PORT, &uart_config));
#if CONFIG_BT_CTRL_HCI_UART_FLOW_CTRL_EN && !CONFIG_BT_CTRL_HCI_UART_DTM_MODE
    const int hci_uart_rts_pin = CONFIG_BT_CTRL_HCI_UART_RTS_PIN;
    const int hci_uart_cts_pin = CONFIG_BT_CTRL_HCI_UART_CTS_PIN;
#else
    const int hci_uart_rts_pin = UART_PIN_NO_CHANGE;
    const int hci_uart_cts_pin = UART_PIN_NO_CHANGE;
#endif
    ESP_ERROR_CHECK(uart_set_pin(CONFIG_BT_CTRL_HCI_UART_PORT, CONFIG_BT_CTRL_HCI_UART_TX_PIN,
                                 CONFIG_BT_CTRL_HCI_UART_RX_PIN, hci_uart_rts_pin,
                                 hci_uart_cts_pin));

    uhci_controller_config_t uhci_cfg = {
        .uart_port = CONFIG_BT_CTRL_HCI_UART_PORT,
        .tx_trans_queue_depth = 1,
        /* Sizes the RX DMA descriptor chain that the continuous-RX ring is split across. */
        .max_receive_internal_mem = BTDM_HCI_UHCI_RX_DMA_RING_SIZE,
        .max_transmit_size = BTDM_HCI_UHCI_MAX_TX_BUF_SIZE,
        .dma_burst_size = 32,
        .rx_eof_flags.idle_eof = 1,
    };

    ESP_ERROR_CHECK(uhci_new_controller(&uhci_cfg, &s_btdm_hci_uart_env.uhci_handle));

    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = btdm_hci_uhci_rx_event_cb,
        .on_tx_trans_done = btdm_hci_uhci_tx_done_cb,
    };

    ESP_ERROR_CHECK(uhci_register_event_callbacks(s_btdm_hci_uart_env.uhci_handle, &uhci_cbs, NULL));

    esp_err_t ret = btdm_hci_uhci_rx_prepare();
    if (ret != ESP_OK) {
        btdm_hci_uart_tl_cleanup();
        return ret;
    }

    s_btdm_hci_uart_env.tl_installed = true;

    ESP_LOGI(BT_LOG_TAG, "HCI messages can be communicated over UART%d:\n"
             "--PINs: TxD %d, RxD %d, RTS %d, CTS %d\n"
             "--Baudrate: %d", CONFIG_BT_CTRL_HCI_UART_PORT,
             CONFIG_BT_CTRL_HCI_UART_TX_PIN, CONFIG_BT_CTRL_HCI_UART_RX_PIN,
             hci_uart_rts_pin, hci_uart_cts_pin,
             CONFIG_BT_CTRL_HCI_UART_BAUDRATE);
    return ESP_OK;
}

esp_err_t esp_bt_hci_uart_reconfig_pin(int tx_pin, int rx_pin)
{
    uart_config_t uart_config = {
        .baud_rate = CONFIG_BT_CTRL_HCI_UART_BAUDRATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = BTDM_HCI_UART_RX_THRS,
        .source_clk = UART_SCLK_DEFAULT,
    };
    esp_err_t ret;

    ret = uart_param_config(CONFIG_BT_CTRL_HCI_UART_PORT, &uart_config);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = uart_set_pin(CONFIG_BT_CTRL_HCI_UART_PORT, tx_pin, rx_pin, -1, -1);
    if (ret != ESP_OK) {
        return ret;
    }

    ESP_LOGI(BT_LOG_TAG, "HCI UART%d reconfig pins: TxD %d, RxD %d", CONFIG_BT_CTRL_HCI_UART_PORT, tx_pin, rx_pin);
    return ESP_OK;
}

#endif /* CONFIG_BT_ENABLED && CONFIG_BT_CTRL_HCI_MODE_UART_H4 && CONFIG_BT_CTRL_HCI_UART_INIT_BY_CONTROLLER */
