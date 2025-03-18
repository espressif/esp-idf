/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "esp_hci_transport.h"
#include "esp_hci_internal.h"
#include "common/hci_driver_h4.h"
#include "common/hci_driver_util.h"
#include "common/hci_driver_mem.h"
#include "hci_driver_uart.h"

static const char *TAG = "hci_uart";

#define CONFIG_BT_LE_HCI_RX_PROC_DATA_LEN       (256)

typedef struct {
    TaskHandle_t tx_task_handler;
    TaskHandle_t rx_task_handler;
    hci_driver_uart_params_config_t *hci_uart_params;
    SemaphoreHandle_t tx_sem;
    QueueHandle_t rx_event_queue;
    uint8_t *rx_data;
    struct hci_h4_sm *h4_sm;
    hci_driver_forward_fn *forward_cb;
} hci_driver_uart_env_t;

static hci_driver_uart_env_t s_hci_driver_uart_env;
static struct hci_h4_sm s_hci_driver_uart_h4_sm;
static uint8_t s_hci_driver_uart_rx_data[CONFIG_BT_LE_HCI_RX_PROC_DATA_LEN];

static int
hci_driver_uart_tx(hci_driver_data_type_t data_type, uint8_t *data, uint32_t length,
                   hci_driver_direction_t dir)
{
    /* By now, this layer is only used by controller. */
    assert(dir == HCI_DRIVER_DIR_C2H);
    ESP_LOGD(TAG, "controller tx len:%d\n", length);

    hci_driver_util_tx_list_enqueue(data_type, data, length);
    xSemaphoreGive(s_hci_driver_uart_env.tx_sem);

    return 0;
}

static int
hci_driver_uart_h4_frame_cb(uint8_t pkt_type, void *data)
{
    hci_driver_forward_fn *forward_cb;

    forward_cb = s_hci_driver_uart_env.forward_cb;
    if (!forward_cb) {
        return -1;
    }
    ESP_LOGD(TAG, "h4 frame\n");
    return forward_cb(pkt_type, data, 0, HCI_DRIVER_DIR_H2C);
}

static void
hci_driver_uart_tx_task(void *p)
{
    void *data;
    bool last_frame;
    uint32_t tx_len;
    uart_port_t port;

    port = s_hci_driver_uart_env.hci_uart_params->hci_uart_port;
    while (true) {
        xSemaphoreTake(s_hci_driver_uart_env.tx_sem, portMAX_DELAY);
        while (true) {
            tx_len = hci_driver_util_tx_list_dequeue(0xffffff, &data, &last_frame);
            if (tx_len == 0) {
                break;
            }
            ESP_LOGD(TAG, "uart tx");
            ESP_LOG_BUFFER_HEXDUMP(TAG, data, tx_len, ESP_LOG_DEBUG);
            uart_write_bytes(port, data, tx_len);
        }
    }
}

static void
hci_driver_uart_rx_task(void *p)
{
    void *data;
    int read_len;
    int ret;
    uart_port_t port;
    uart_event_t uart_event;

    port = s_hci_driver_uart_env.hci_uart_params->hci_uart_port;
    while (true) {
        xQueueReceive(s_hci_driver_uart_env.rx_event_queue, &uart_event, portMAX_DELAY);
        data = s_hci_driver_uart_env.rx_data;
        while (true) {
            read_len = uart_read_bytes(port, data, CONFIG_BT_LE_HCI_RX_PROC_DATA_LEN, 0);
            if (read_len == 0) {
                break;
            }
            ESP_LOGD(TAG, "uart rx");
            ESP_LOG_BUFFER_HEXDUMP(TAG, data, read_len, ESP_LOG_DEBUG);
            ret = hci_h4_sm_rx(s_hci_driver_uart_env.h4_sm, data, read_len);
            if (ret < 0) {
                ESP_LOGE(TAG, "parse rx data error! sm_state:%d\n", s_hci_driver_uart_env.h4_sm->state);
                r_ble_ll_hci_ev_hw_err(ESP_HCI_SYNC_LOSS_ERR);
            }
        }
    }
}

static int
hci_driver_uart_task_create(void)
{
    /* !TODO: Set the core id by menuconfig */
    xTaskCreatePinnedToCore(hci_driver_uart_tx_task, "hci_driver_uart_tx_task",
                            CONFIG_BT_LE_HCI_TRANS_TASK_STACK_SIZE, NULL,
                            ESP_TASK_BT_CONTROLLER_PRIO, &s_hci_driver_uart_env.tx_task_handler,
                            0);
    assert(s_hci_driver_uart_env.tx_task_handler);

    xTaskCreatePinnedToCore(hci_driver_uart_rx_task, "hci_driver_uart_rx_task",
                            CONFIG_BT_LE_HCI_TRANS_TASK_STACK_SIZE, NULL,
                            ESP_TASK_BT_CONTROLLER_PRIO, &s_hci_driver_uart_env.rx_task_handler,
                            0);
    assert(s_hci_driver_uart_env.rx_task_handler);

    ESP_LOGI(TAG, "hci transport task create successfully, prio:%d, stack size: %ld",
             ESP_TASK_BT_CONTROLLER_PRIO, CONFIG_BT_LE_HCI_TRANS_TASK_STACK_SIZE);

    return 0;
}

static void
hci_driver_uart_task_delete(void)
{
    if (s_hci_driver_uart_env.tx_task_handler) {
        vTaskDelete(s_hci_driver_uart_env.tx_task_handler);
        s_hci_driver_uart_env.tx_task_handler = NULL;
    }

    if (s_hci_driver_uart_env.rx_task_handler) {
        vTaskDelete(s_hci_driver_uart_env.rx_task_handler);
        s_hci_driver_uart_env.rx_task_handler = NULL;
    }
}

static void
hci_driver_uart_deinit(void)
{
    hci_driver_uart_task_delete();

    ESP_ERROR_CHECK(uart_driver_delete(s_hci_driver_uart_env.hci_uart_params->hci_uart_port));

    if (s_hci_driver_uart_env.tx_sem) {
        vSemaphoreDelete(s_hci_driver_uart_env.tx_sem);
    }

    hci_driver_util_deinit();
    memset(&s_hci_driver_uart_env, 0, sizeof(hci_driver_uart_env_t));
}

static int
hci_driver_uart_init(hci_driver_forward_fn *cb)
{
    int rc;
    memset(&s_hci_driver_uart_env, 0, sizeof(hci_driver_uart_env_t));

    s_hci_driver_uart_env.h4_sm = &s_hci_driver_uart_h4_sm;
    hci_h4_sm_init(s_hci_driver_uart_env.h4_sm, &s_hci_driver_mem_alloc, &s_hci_driver_mem_free, hci_driver_uart_h4_frame_cb);

    rc = hci_driver_util_init();
    if (rc) {
        goto error;
    }

    s_hci_driver_uart_env.tx_sem = xSemaphoreCreateBinary();
    if (!s_hci_driver_uart_env.tx_sem) {
        goto error;
    }

    s_hci_driver_uart_env.rx_data = s_hci_driver_uart_rx_data;
    s_hci_driver_uart_env.forward_cb = cb;
    s_hci_driver_uart_env.hci_uart_params = hci_driver_uart_config_param_get();
    hci_driver_uart_config(s_hci_driver_uart_env.hci_uart_params);
    /* Currently, the queue size is set to 1. It will be considered as semaphore. */
    ESP_ERROR_CHECK(uart_driver_install(s_hci_driver_uart_env.hci_uart_params->hci_uart_port,
                                        CONFIG_BT_LE_HCI_UART_RX_BUFFER_SIZE,
                                        CONFIG_BT_LE_HCI_UART_TX_BUFFER_SIZE,
                                        1, &s_hci_driver_uart_env.rx_event_queue,
                                        0));

    rc = hci_driver_uart_task_create();
    if (rc) {
        goto error;
    }

    return 0;

error:
    hci_driver_uart_deinit();
    return rc;
}

int
hci_driver_uart_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin)
{
    int rc;

    hci_driver_uart_task_delete();
    hci_driver_uart_pin_update(tx_pin, rx_pin, cts_pin, rts_pin);
    /* Currently, the queue size is set to 1. It will be considered as semaphore. */
    ESP_ERROR_CHECK(uart_driver_install(s_hci_driver_uart_env.hci_uart_params->hci_uart_port,
                                        CONFIG_BT_LE_HCI_UART_RX_BUFFER_SIZE,
                                        CONFIG_BT_LE_HCI_UART_TX_BUFFER_SIZE,
                                        1, &s_hci_driver_uart_env.rx_event_queue,
                                        0));
    rc = hci_driver_uart_task_create();
    if (rc) {
        hci_driver_uart_task_delete();
        return -2;
    }

    return 0;
}

hci_driver_ops_t hci_driver_uart_ops = {
    .hci_driver_tx = hci_driver_uart_tx,
    .hci_driver_init = hci_driver_uart_init,
    .hci_driver_deinit = hci_driver_uart_deinit,
};
