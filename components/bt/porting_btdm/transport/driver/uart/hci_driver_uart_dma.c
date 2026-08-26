/*
 * SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/ringbuf.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/uart.h"
#include "driver/uhci.h"
#include "esp_hci_transport.h"
#include "esp_hci_internal.h"
#include "ble_user_cfg.h"
#include "common/hci_driver_h4.h"
#include "common/hci_driver_util.h"
#include "common/hci_driver_mem.h"
#include "hci_driver_uart.h"

/**
 * HCI UART DMA transport on top of the UHCI driver (driver/uhci.h).
 *
 * RX  (uhci_start_receive_continuous)
 * TX  (uhci_multi_buffer_transmit)
 */

/**
 * @brief Enumeration of HCI transport transmission states.
 */
typedef enum {
    HCI_TRANS_TX_IDLE,    /*!< No UHCI TX transaction in flight; task may dequeue the next packet. */
    HCI_TRANS_TX_BUSY,    /*!< uhci_multi_buffer_transmit() has been submitted; wait for on_tx_trans_done. */
} hci_trans_tx_state_t;

/**
 * @brief Runtime context for the UHCI-based HCI UART DMA transport.
 */
typedef struct {
    TaskHandle_t task_handler;                        /*!< Process task: starts TX when idle and feeds RX bytes to H4. */
    hci_driver_uart_params_config_t *hci_uart_params; /*!< UART port / pins / baud used by uhci_controller_config_t. */
    SemaphoreHandle_t process_sem;                    /*!< Wakes the process task (TX enqueue, TX done, RX event). */
    struct hci_h4_sm *h4_sm;                          /*!< H4 state machine that reassembles HCI packets from the byte stream. */
    hci_driver_forward_fn *forward_cb;                /*!< Host-bound callback invoked when H4 completes one packet. */
    uhci_controller_handle_t uhci_ctrl;               /*!< Handle returned by uhci_new_controller(). */
    uint8_t *rx_dma_ring;                             /*!< Storage for uhci_start_receive_continuous(); valid until uhci_stop_receive(). */
    size_t rx_dma_ring_size;                          /*!< Size of rx_dma_ring, passed as buffer_size to continuous RX. */
    RingbufHandle_t rx_copy_ringbuf;                  /*!< ISR copies DMA-ring slices here; the process task drains it. */
    uhci_transmit_buffer_info_t *tx_segments;         /*!< Scratch array of UHCI TX segments for one multi-buffer transaction. */
    volatile hci_trans_tx_state_t hci_tx_state;       /*!< Only one HCI packet is in flight (tx-list entries cannot be mixed). */
    volatile bool rx_copy_overflow;                   /*!< Set in ISR when rx_copy_ringbuf cannot accept a DMA slice. */
} hci_driver_uart_dma_env_t;

/* Max UHCI TX segments in one uhci_multi_buffer_transmit(); maps to max_transmit_buffer_count. */
#define HCI_TX_MAX_SEGMENT_COUNT            (20)

#if UC_BT_CTRL_BR_EDR_IS_ENABLE
/* BR/EDR or dual-mode: Classic ACL packet budget (H4 type + payload). */
#define HCI_TX_MAX_SIZE                     (1024)
#define HCI_RX_PKT_BUDGET                   (1024 + HCI_TRANSPORT_CMD_SZ)
#else
/* BLE only: H4 type byte + one LE ACL payload; RX also covers a command-sized packet. */
#define HCI_TX_MAX_SIZE                     (DEFAULT_BT_LE_ACL_BUF_SIZE + 1)
#define HCI_RX_PKT_BUDGET                   (DEFAULT_BT_LE_ACL_BUF_SIZE + HCI_TRANSPORT_CMD_SZ)
#endif

#define HCI_RX_DMA_RING_SIZE                ((HCI_RX_PKT_BUDGET) > 4096 ? (HCI_RX_PKT_BUDGET) : 4096)
/*
 * uhci_controller_config_t.max_receive_internal_mem decides how many RX DMA descriptors
 * UHCI allocates (node_count = size / DMA_DESCRIPTOR_BUFFER_MAX_SIZE). Keep this large
 * enough for at least two nodes so continuous RX can ping-pong instead of overwriting
 * a single node while the callback still copies. This is not the allocated DMA ring size.
 */
#define HCI_UHCI_RX_DESC_MEM                (UC_BT_CTRL_HCI_TRANS_RX_MEM_NUM * HCI_RX_DMA_RING_SIZE)
#define HCI_RX_COPY_RINGBUF_SIZE            HCI_RX_DMA_RING_SIZE

static const char *TAG = "uart_dma";
static hci_driver_uart_dma_env_t s_hci_driver_uart_dma_env;
static struct hci_h4_sm s_hci_driver_uart_h4_sm;
static portMUX_TYPE s_hci_tx_state_mux = portMUX_INITIALIZER_UNLOCKED;

static int hci_driver_uart_dma_tx_submit(void);

/**
 * @brief Free the DMA ring, software RX ringbuf and TX segment scratch array.
 *
 * Must be called only after uhci_stop_receive() has returned, so the DMA ring
 * is no longer referenced by UHCI.
 */
static void
hci_driver_uart_dma_memory_deinit(void)
{
    if (s_hci_driver_uart_dma_env.rx_copy_ringbuf) {
        vRingbufferDelete(s_hci_driver_uart_dma_env.rx_copy_ringbuf);
        s_hci_driver_uart_dma_env.rx_copy_ringbuf = NULL;
    }

    if (s_hci_driver_uart_dma_env.rx_dma_ring) {
        heap_caps_free(s_hci_driver_uart_dma_env.rx_dma_ring);
        s_hci_driver_uart_dma_env.rx_dma_ring = NULL;
    }

    if (s_hci_driver_uart_dma_env.tx_segments) {
        free(s_hci_driver_uart_dma_env.tx_segments);
        s_hci_driver_uart_dma_env.tx_segments = NULL;
    }
}

/**
 * @brief Allocate UHCI RX/TX working buffers.
 *
 * rx_dma_ring is DMA-capable internal memory required by uhci_start_receive_continuous().
 * tx_segments holds the segment descriptors for uhci_multi_buffer_transmit().
 */
static int
hci_driver_uart_dma_memory_init(void)
{
    s_hci_driver_uart_dma_env.rx_dma_ring_size = HCI_UHCI_RX_DESC_MEM;
    /* DMA + internal: UHCI/GDMA writes here; cache-safe path also expects internal RAM. */
    s_hci_driver_uart_dma_env.rx_dma_ring = heap_caps_calloc(1, s_hci_driver_uart_dma_env.rx_dma_ring_size,
                                                             MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (!s_hci_driver_uart_dma_env.rx_dma_ring) {
        goto init_err;
    }

    /* BYTEBUF so ISR can push arbitrary UHCI slice lengths without pre-sized items. */
    s_hci_driver_uart_dma_env.rx_copy_ringbuf = xRingbufferCreate(HCI_RX_COPY_RINGBUF_SIZE, RINGBUF_TYPE_BYTEBUF);
    if (!s_hci_driver_uart_dma_env.rx_copy_ringbuf) {
        goto init_err;
    }

    s_hci_driver_uart_dma_env.tx_segments = calloc(HCI_TX_MAX_SEGMENT_COUNT, sizeof(uhci_transmit_buffer_info_t));
    if (!s_hci_driver_uart_dma_env.tx_segments) {
        goto init_err;
    }

    return 0;
init_err:
    hci_driver_uart_dma_memory_deinit();
    return -1;
}

static void IRAM_ATTR
hci_driver_uart_dma_txstate_set(hci_trans_tx_state_t tx_state)
{
    portENTER_CRITICAL_SAFE(&s_hci_tx_state_mux);
    s_hci_driver_uart_dma_env.hci_tx_state = tx_state;
    portEXIT_CRITICAL_SAFE(&s_hci_tx_state_mux);
}

/**
 * @brief UHCI on_rx_trans_event callback (ISR context, must be non-blocking).
 *
 * edata->data points into rx_dma_ring and is only guaranteed readable during this
 * callback. Copy the slice out immediately: continuous RX does not stop DMA at EOF,
 * so the same node will be overwritten on wrap-around. Both partial-node and EOF
 * events are forwarded; H4 (not UHCI) decides packet boundaries.
 *
 * @return Whether a higher-priority task was woken (UHCI ISR yield contract).
 */
IRAM_ATTR static bool
hci_driver_uart_dma_rx_event_cb(uhci_controller_handle_t uhci_ctrl, const uhci_rx_event_data_t *edata, void *user_ctx)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    (void)uhci_ctrl;
    (void)user_ctx;

    /* Abnormal EOF: UHCI reports data == NULL and recv_size == 0. Keep the session running. */
    if (!edata->data || edata->recv_size == 0) {
        return false;
    }

    /*
     * xRingbufferSendFromISR copies the DMA-ring slice. If this fails the DMA session
     * still runs (no overrun callback from UHCI); the process task reports HCI sync loss.
     */
    if (xRingbufferSendFromISR(s_hci_driver_uart_dma_env.rx_copy_ringbuf, edata->data, edata->recv_size,
                               &xHigherPriorityTaskWoken) != pdTRUE) {
        s_hci_driver_uart_dma_env.rx_copy_overflow = true;
    }

    xSemaphoreGiveFromISR(s_hci_driver_uart_dma_env.process_sem, &xHigherPriorityTaskWoken);
    return xHigherPriorityTaskWoken == pdTRUE;
}

/**
 * @brief UHCI on_tx_trans_done callback (ISR context).
 *
 * For a multi-buffer transaction, edata->buffer only points at the first segment
 * and is treated as a transaction id, not as sent_size bytes of contiguous memory.
 * The tx-list entry is recycled on the next dequeue after last_frame, so the
 * process task must run again now that the buffers may be freed.
 */
IRAM_ATTR static bool
hci_driver_uart_dma_tx_done_cb(uhci_controller_handle_t uhci_ctrl, const uhci_tx_done_event_data_t *edata, void *user_ctx)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    (void)uhci_ctrl;
    (void)edata;
    (void)user_ctx;

    hci_driver_uart_dma_txstate_set(HCI_TRANS_TX_IDLE);
    xSemaphoreGiveFromISR(s_hci_driver_uart_dma_env.process_sem, &xHigherPriorityTaskWoken);
    return xHigherPriorityTaskWoken == pdTRUE;
}

/**
 * @brief Dequeue one HCI packet and submit it with uhci_multi_buffer_transmit().
 *
 * tx_list_dequeue() is one-entry-at-a-time: the first call yields the 1-byte H4
 * type, later calls yield payload fragments, last_frame marks the end of that
 * packet. All fragments stay valid until on_tx_trans_done (the next dequeue
 * after last_frame frees the event buffer / mbuf).
 *
 * @return 0 if a transaction was queued, -1 if the TX list is empty.
 */
static int
hci_driver_uart_dma_tx_submit(void)
{
    void *data;
    bool last_frame = false;
    uint32_t tx_len;
    size_t seg_count = 0;
    esp_err_t err;

    while (seg_count < HCI_TX_MAX_SEGMENT_COUNT) {
        tx_len = hci_driver_util_tx_list_dequeue(0xffffff, &data, &last_frame);
        if (!tx_len) {
            break;
        }

        /* Each fragment is a UHCI TX segment; UHCI concatenates them on the UART wire. */
        s_hci_driver_uart_dma_env.tx_segments[seg_count].write_buffer = data;
        s_hci_driver_uart_dma_env.tx_segments[seg_count].buffer_size = tx_len;
        seg_count++;
        if (last_frame) {
            break;
        }
    }

    if (seg_count == 0) {
        return -1;
    }

    if (!last_frame) {
        /* array_size is capped by max_transmit_buffer_count; a gap before the rest may look like idle EOF on the peer. */
        ESP_LOGW(TAG, "HCI TX packet exceeds max_transmit_buffer_count (%d), sending partial packet",
                 HCI_TX_MAX_SEGMENT_COUNT);
    }

    /* Mark BUSY first so a completion ISR cannot be observed as still-idle. */
    hci_driver_uart_dma_txstate_set(HCI_TRANS_TX_BUSY);
    err = uhci_multi_buffer_transmit(s_hci_driver_uart_dma_env.uhci_ctrl,
                                     s_hci_driver_uart_dma_env.tx_segments, seg_count);
    /* Transmit must not fail; assert directly. */
    assert(err == ESP_OK);

    return 0;
}

/**
 * @brief Create the UHCI controller, register ISR callbacks and start continuous RX.
 *
 * UART pins/baud must already be programmed (uart_param_config / uart_set_pin).
 * UHCI and BT HCI share the same hardware; this driver is the exclusive UHCI user.
 */
static int
hci_driver_uart_dma_uhci_install(void)
{
    uhci_controller_config_t uhci_cfg = {
        .uart_port = s_hci_driver_uart_dma_env.hci_uart_params->hci_uart_port, /* Attach this UART to UHCI. */
        .tx_trans_queue_depth = 2,                    /* One in-flight HCI packet is enough; +1 for slack. */
        .max_transmit_size = HCI_TX_MAX_SIZE,         /* Total bytes of all segments in one transaction. */
        .max_transmit_buffer_count = HCI_TX_MAX_SEGMENT_COUNT, /* Caps uhci_multi_buffer_transmit() array_size. */
        .max_receive_internal_mem = HCI_UHCI_RX_DESC_MEM,      /* Sizes the RX DMA descriptor chain, not the ring. */
        .dma_burst_size = 32,                         /* Power-of-two burst; 0 would disable burst. */
        .rx_eof_flags.idle_eof = 1,                   /* Frame ends when the UART RX line goes idle. */
    };
    uhci_event_callbacks_t uhci_cbs = {
        .on_rx_trans_event = hci_driver_uart_dma_rx_event_cb, /* Partial node and/or frame EOF. */
        .on_tx_trans_done = hci_driver_uart_dma_tx_done_cb,
    };

    ESP_LOGI(TAG, "uart attach uhci");
    if (uhci_new_controller(&uhci_cfg, &s_hci_driver_uart_dma_env.uhci_ctrl) != ESP_OK) {
        return -1;
    }

    /* Register before start_receive_continuous so the first frame is not dropped. */
    if (uhci_register_event_callbacks(s_hci_driver_uart_dma_env.uhci_ctrl, &uhci_cbs, NULL) != ESP_OK) {
        uhci_del_controller(s_hci_driver_uart_dma_env.uhci_ctrl);
        s_hci_driver_uart_dma_env.uhci_ctrl = NULL;
        return -1;
    }

    /* Arm once. Do not call uhci_receive() / start again until uhci_stop_receive(). */
    if (uhci_start_receive_continuous(s_hci_driver_uart_dma_env.uhci_ctrl,
                                      s_hci_driver_uart_dma_env.rx_dma_ring,
                                      s_hci_driver_uart_dma_env.rx_dma_ring_size) != ESP_OK) {
        uhci_del_controller(s_hci_driver_uart_dma_env.uhci_ctrl);
        s_hci_driver_uart_dma_env.uhci_ctrl = NULL;
        return -1;
    }

    return 0;
}

/**
 * @brief Stop continuous RX so UHCI RX callbacks cannot run anymore.
 *
 * Does not delete the controller: the process task may still submit TX until it
 * is torn down. No-op if UHCI was never installed (init error path).
 */
static void
hci_driver_uart_dma_uhci_stop_rx(void)
{
    if (!s_hci_driver_uart_dma_env.uhci_ctrl) {
        return;
    }

    uhci_stop_receive(s_hci_driver_uart_dma_env.uhci_ctrl);
}

/**
 * @brief Wait for in-flight TX and delete the UHCI controller.
 *
 * Call only after the process task is gone so it cannot uhci_multi_buffer_transmit()
 * on a handle that is being deleted. The DMA ring may be freed only after this returns.
 */
static void
hci_driver_uart_dma_uhci_del(void)
{
    if (!s_hci_driver_uart_dma_env.uhci_ctrl) {
        return;
    }

    uhci_wait_all_tx_transaction_done(s_hci_driver_uart_dma_env.uhci_ctrl, 100);
    uhci_del_controller(s_hci_driver_uart_dma_env.uhci_ctrl);
    s_hci_driver_uart_dma_env.uhci_ctrl = NULL;
}

static void
hci_driver_uart_dma_task_delete(void)
{
    if (s_hci_driver_uart_dma_env.task_handler) {
        vTaskDelete(s_hci_driver_uart_dma_env.task_handler);
        s_hci_driver_uart_dma_env.task_handler = NULL;
    }
}

/**
 * @brief Enqueue a controller-to-host HCI packet and wake the process task.
 *
 * The actual UART DMA send happens in hci_driver_uart_dma_tx_submit() when TX is idle.
 */
static int
hci_driver_uart_dma_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                       hci_driver_direction_t dir)
{
    int rc;
    uint8_t data_source;

    data_source = 0;
    ESP_LOGD(TAG, "dma tx:");
    ESP_LOG_BUFFER_HEXDUMP(TAG, data, length, ESP_LOG_DEBUG);
    if (dir == HCI_DRIVER_DIR_BTDMC2H) {
        if (data_type == HCI_DRIVER_TYPE_EVT) {
            data_source = HCI_DRIVER_BTDM_EVT;
        }
    } else if (dir == HCI_DRIVER_DIR_LEC2H) {
        if (data_type == HCI_DRIVER_TYPE_ACL) {
            data_source = HCI_DRIVER_LE_ACL;
        } else if (data_type == HCI_DRIVER_TYPE_EVT) {
            data_source = HCI_DRIVER_LE_EVT;
        } else if (data_type == HCI_DRIVER_TYPE_ISO) {
            data_source = HCI_DRIVER_LE_ISO;
        }
    }
#if UC_BT_CTRL_BR_EDR_IS_ENABLE
    else if (dir == HCI_DRIVER_DIR_BREDRC2H) {
        if (data_type == HCI_DRIVER_TYPE_ACL) {
            data_source = HCI_DRIVER_BREDR_ACL;
        } else if (data_type == HCI_DRIVER_TYPE_SYNC) {
            data_source = HCI_DRIVER_BREDR_SYNC;
        } else if (data_type == HCI_DRIVER_TYPE_EVT) {
            data_source = HCI_DRIVER_BREDR_EVT;
        }
    }
#endif // UC_BT_CTRL_BR_EDR_IS_ENABLE
    else {
        assert(0);
    }

    rc = hci_driver_util_tx_list_enqueue(data_type, data, length, data_source);
    if (rc < 0) {
        ESP_LOGE(TAG, "tx data enqueue failed!\n");
        return rc;
    }

    xSemaphoreGive(s_hci_driver_uart_dma_env.process_sem);
    return rc;
}

/**
 * @brief H4 has assembled one host-bound HCI packet; forward it upward.
 */
static int
hci_driver_uart_dma_h4_frame_cb(uint8_t pkt_type, void *data, int pkt_len, uint8_t data_source)
{
    hci_driver_forward_fn *forward_cb = s_hci_driver_uart_dma_env.forward_cb;

    if (!forward_cb) {
        ESP_LOGE(TAG, "rx cb is NULL\n");
        return -1;
    }
    ESP_LOGD(TAG, "h4 frame\n");
    return forward_cb(pkt_type, data, pkt_len, HCI_DRIVER_DIR_H2C, data_source);
}

/**
 * @brief Drain the software RX ringbuf into the H4 state machine.
 *
 * Timeout is 0: the process task is already woken by process_sem. Chunks may be
 * one DMA node or a short EOF tail; H4 concatenates them into HCI packets.
 */
static void
hci_driver_uart_dma_process_rx(void)
{
    size_t item_size;
    uint8_t *rx_data;
    int ret;

    if (s_hci_driver_uart_dma_env.rx_copy_overflow) {
        ESP_LOGE(TAG, "RX software ring buffer overflow, HCI stream may lose sync");
        s_hci_driver_uart_dma_env.rx_copy_overflow = false;
#if UC_BT_CTRL_BLE_IS_ENABLE
        r_ble_ll_hci_ev_hw_err(ESP_HCI_SYNC_LOSS_ERR);
#endif // #if UC_BT_CTRL_BLE_IS_ENABLE
    }

    while ((rx_data = xRingbufferReceive(s_hci_driver_uart_dma_env.rx_copy_ringbuf, &item_size, 0)) != NULL) {
        ESP_LOGD(TAG, "uart rx");
        ESP_LOG_BUFFER_HEXDUMP(TAG, rx_data, item_size, ESP_LOG_DEBUG);
        ret = hci_h4_sm_rx(s_hci_driver_uart_dma_env.h4_sm, rx_data, (uint16_t)item_size);
        /* Return the item before parsing the next slice so the ringbuf can accept more ISR copies. */
        vRingbufferReturnItem(s_hci_driver_uart_dma_env.rx_copy_ringbuf, rx_data);
        if (ret < 0) {
            ESP_LOGW(TAG, "parse rx data error!\n");
#if UC_BT_CTRL_BLE_IS_ENABLE
            r_ble_ll_hci_ev_hw_err(ESP_HCI_SYNC_LOSS_ERR);
#endif // #if UC_BT_CTRL_BLE_IS_ENABLE
        }
    }
}

/**
 * @brief Serialized TX kick + RX parse.
 *
 * Woken by: controller TX enqueue, UHCI TX done, or UHCI RX event. A binary
 * semaphore is enough because both sides are polled every wake-up.
 */
static void
hci_driver_uart_dma_process_task(void *p)
{
    (void)p;

    while (true) {
        xSemaphoreTake(s_hci_driver_uart_dma_env.process_sem, portMAX_DELAY);
        ESP_LOGD(TAG, "task run:%d\n", s_hci_driver_uart_dma_env.hci_tx_state);

        /* Do not start another uhci_multi_buffer_transmit() while one is in flight. */
        if (s_hci_driver_uart_dma_env.hci_tx_state == HCI_TRANS_TX_IDLE) {
            hci_driver_uart_dma_tx_submit();
        }

        hci_driver_uart_dma_process_rx();
    }
}

static int
hci_driver_uart_dma_task_create(void)
{
    /* !TODO: Set the core id by menuconfig */
    xTaskCreatePinnedToCore(hci_driver_uart_dma_process_task, "hci_driver_uart_dma_process_task",
                            UC_BT_CTRL_HCI_TRANS_TASK_STACK_SIZE, NULL,
                            ESP_TASK_BT_CONTROLLER_PRIO, &s_hci_driver_uart_dma_env.task_handler,
                            0);
    assert(s_hci_driver_uart_dma_env.task_handler);

    ESP_LOGI(TAG, "hci transport task create successfully, prio:%d, stack size: %ld",
             ESP_TASK_BT_CONTROLLER_PRIO, UC_BT_CTRL_HCI_TRANS_TASK_STACK_SIZE);

    return 0;
}

/**
 * @brief Tear down in reverse-dependency order, with UHCI RX stopped before the task.
 *
 * 1. uhci_stop_receive: RX ISR must not write a ringbuf / give a sem we are about to free.
 * 2. Delete the process task: it must not call UHCI TX APIs during del_controller.
 * 3. Wait TX + uhci_del_controller: DMA ring is then unused.
 * 4. UART, working buffers, process_sem, TX list.
 */
static void
hci_driver_uart_dma_deinit(void)
{
    hci_driver_uart_dma_uhci_stop_rx();
    hci_driver_uart_dma_task_delete();
    hci_driver_uart_dma_uhci_del();

    if (s_hci_driver_uart_dma_env.hci_uart_params) {
        /* uart_param_config/set_pin do not uart_driver_install(); delete may be a no-op. */
        (void)uart_driver_delete(s_hci_driver_uart_dma_env.hci_uart_params->hci_uart_port);
    }

    hci_driver_uart_dma_memory_deinit();
    if (s_hci_driver_uart_dma_env.process_sem) {
        vSemaphoreDelete(s_hci_driver_uart_dma_env.process_sem);
        s_hci_driver_uart_dma_env.process_sem = NULL;
    }

    hci_driver_util_deinit();
    memset(&s_hci_driver_uart_dma_env, 0, sizeof(hci_driver_uart_dma_env_t));
}

/**
 * @brief Bring up H4, TX list, UART, UHCI continuous RX and the process task.
 */
static int
hci_driver_uart_dma_init(hci_driver_forward_fn *cb)
{
    int rc;

    memset(&s_hci_driver_uart_dma_env, 0, sizeof(hci_driver_uart_dma_env_t));

    s_hci_driver_uart_dma_env.h4_sm = &s_hci_driver_uart_h4_sm;
    hci_h4_sm_init(s_hci_driver_uart_dma_env.h4_sm, &s_hci_driver_mem_alloc, &s_hci_driver_mem_free,
                   hci_driver_uart_dma_h4_frame_cb);

    rc = hci_driver_util_init();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_dma_env.process_sem = xSemaphoreCreateBinary();
    if (!s_hci_driver_uart_dma_env.process_sem) {
        goto error;
    }

    rc = hci_driver_uart_dma_memory_init();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_dma_env.forward_cb = cb;
    s_hci_driver_uart_dma_env.hci_uart_params = hci_driver_uart_config_param_get();
    /* UART must be parameterized before uhci_new_controller() attaches the port. */
    hci_driver_uart_config(s_hci_driver_uart_dma_env.hci_uart_params);

    rc = hci_driver_uart_dma_uhci_install();
    if (rc) {
        goto error;
    }

    rc = hci_driver_uart_dma_task_create();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_dma_env.hci_tx_state = HCI_TRANS_TX_IDLE;
    return 0;

error:
    hci_driver_uart_dma_deinit();
    return rc;
}

int
hci_driver_uart_dma_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin)
{
    /* UHCI stays attached to the same UART port; only the GPIO mapping changes. */
    return hci_driver_uart_pin_update(tx_pin, rx_pin, cts_pin, rts_pin);
}

hci_driver_ops_t hci_driver_uart_dma_ops = {
    .hci_driver_tx = hci_driver_uart_dma_tx,
    .hci_driver_init = hci_driver_uart_dma_init,
    .hci_driver_deinit = hci_driver_uart_dma_deinit,
};
