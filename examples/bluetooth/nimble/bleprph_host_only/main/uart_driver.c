/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "uart_driver.h"
#include "nimble/hci_common.h"
#include "host/ble_hs_mbuf.h"

#define TAG "UART_HCI"

#define UART_NO                  (1)
#define UART_BUF_SZ              (1024)

#define UART_TX_PIN              (CONFIG_EXAMPLE_HCI_UART_TX_PIN)
#define UART_RX_PIN              (CONFIG_EXAMPLE_HCI_UART_RX_PIN)
#define UART_RTS_PIN             (CONFIG_EXAMPLE_HCI_UART_RTS_PIN)
#define UART_CTS_PIN             (CONFIG_EXAMPLE_HCI_UART_CTS_PIN)
#define HCI_H4_ACL               (0x02)
#define HCI_H4_CMD               (0x01)
#define HCI_H4_EVT               (0x04)
#define BLE_HCI_EVENT_HDR_LEN    (2)
#define BLE_HCI_CMD_HDR_LEN      (3)

enum {
    UART_RX_TYPE = 0,
    UART_RX_LEN,
    UART_RX_DATA,
};

enum {
    DATA_TYPE_COMMAND = 1,
    DATA_TYPE_ACL     = 2,
    DATA_TYPE_EVENT   = 4
};

TaskHandle_t s_rx_task_hdl;
static volatile bool s_shutdown_flag = false;

static void IRAM_ATTR hci_uart_rx_task(void *arg)
{
    uint8_t buf[1026];
    int len_now_read = -1;
    uint32_t len_to_read = 1;
    uint32_t len_total_read = 0;
    uint8_t rx_st = UART_RX_TYPE;

    while (!s_shutdown_flag) {
        // Use timeout instead of portMAX_DELAY to allow periodic shutdown flag check
        len_now_read = uart_read_bytes(UART_NO, &buf[len_total_read], len_to_read, pdMS_TO_TICKS(100));

        // If timeout occurred, continue loop to check shutdown flag again
        if (len_now_read == 0) {
            continue;
        }

        len_total_read += len_now_read;

        switch (rx_st) {
        case UART_RX_TYPE: {
            assert(buf[0] >= DATA_TYPE_ACL && buf[0] <= DATA_TYPE_EVENT);
            if (buf[0] == DATA_TYPE_ACL) {
                len_to_read = 4;
            } else if (buf[0] == DATA_TYPE_EVENT) {
                len_to_read = 2;
            } else {
                assert(0);
            }
            rx_st = UART_RX_LEN;
        }
        break;

        case UART_RX_LEN: {
            if (buf[0] == DATA_TYPE_ACL) {
                len_to_read = buf[3] | (buf[4] << 8);
            } else if (buf[0] == DATA_TYPE_EVENT) {
                len_to_read = buf[2];
            } else {
                assert(0);
            }
            rx_st = UART_RX_DATA;
        }
        break;

        case UART_RX_DATA: {
            uint8_t *data = buf;
            int rc;

            if (data[0] == HCI_H4_EVT) {
                uint8_t *evbuf;
                int totlen;

                totlen = BLE_HCI_EVENT_HDR_LEN + data[2];
                assert(totlen <= UINT8_MAX + BLE_HCI_EVENT_HDR_LEN);

                if (totlen > MYNEWT_VAL(BLE_TRANSPORT_EVT_SIZE)) {
                    ESP_LOGE(TAG, "Received HCI data length at host (%d)"
                             "exceeds maximum configured HCI event buffer size (%d).",
                             totlen, MYNEWT_VAL(BLE_TRANSPORT_EVT_SIZE));
                    break;
                }

                if (data[1] == BLE_HCI_EVCODE_HW_ERROR) {
                    assert(0);
                }

                /* Allocate LE Advertising Report Event from lo pool only */
                if ((data[1] == BLE_HCI_EVCODE_LE_META) &&
                        (data[3] == BLE_HCI_LE_SUBEV_ADV_RPT || data[3] == BLE_HCI_LE_SUBEV_EXT_ADV_RPT)) {
                    evbuf = ble_transport_alloc_evt(1);
                    /* Skip advertising report if we're out of memory */
                    if (!evbuf) {
                        ESP_LOGE(TAG, "No buffers");
                        break;
                    }
                } else {
                    evbuf = ble_transport_alloc_evt(0);
                    assert(evbuf != NULL);
                }

                memset(evbuf, 0, sizeof * evbuf);
                memcpy(evbuf, &data[1], totlen);

                rc = ble_transport_to_hs_evt(evbuf);
                assert(rc == 0);
            } else if (data[0] == HCI_H4_ACL) {
                struct os_mbuf *m = NULL;
                m = ble_transport_alloc_acl_from_ll();
                if (!m) {
                    ESP_LOGE(TAG, "No buffers");
                    rx_st = UART_RX_TYPE;
                    len_to_read = 1;
                    len_total_read = 0;
                    break;
                }

                if ((rc = os_mbuf_append(m, &data[1], len_total_read - 1)) != 0) {
                    ESP_LOGE(TAG, "%s failed to os_mbuf_append; rc = %d", __func__, rc);
                    os_mbuf_free_chain(m);
                    return;
                }

                ble_transport_to_hs_acl(m);
            }

            rx_st = UART_RX_TYPE;
            len_to_read = 1;
            len_total_read = 0;
        }
        break;

        default: {
            assert(0);
            break;
        }
        }

    }
        vTaskDelete(NULL);
}

void hci_uart_send(uint8_t *buf, uint16_t len)
{
    uint8_t *p = buf;
    int len_write = 0;

    while (len) {
        len_write = uart_write_bytes(UART_NO, p, len);
        assert(len_write > 0);
        len -= len_write;
        p += len_write;
    }
}


void
ble_transport_ll_init(void)
{

}

void
ble_transport_ll_deinit(void)
{
    hci_uart_close();
}

int
ble_transport_to_ll_acl_impl(struct os_mbuf *om)
{
    uint8_t buf[OS_MBUF_PKTLEN(om) + 1];
    int rc;
    buf[0] = HCI_H4_ACL;
    rc = ble_hs_mbuf_to_flat(om, buf + 1, OS_MBUF_PKTLEN(om), NULL);
    if(rc) {
        ESP_LOGE(TAG, "Error copying data %d", rc);
        os_mbuf_free_chain(om);
        return rc;
    }
    hci_uart_send(buf, OS_MBUF_PKTLEN(om) + 1);
    os_mbuf_free_chain(om);
    return 0;
}

int
ble_transport_to_ll_cmd_impl(void *buf)
{
    int len = 3 + ((uint8_t *)buf)[2] + 1;
    uint8_t data[258];
    data[0] = HCI_H4_CMD;
    memcpy(data + 1, buf, len - 1);
    hci_uart_send(data, len);
    ble_transport_free(buf);
    return 0;
}

void hci_uart_open(void)
{
    uart_config_t uart_config = {
        .baud_rate  = CONFIG_EXAMPLE_HCI_UART_BAUDRATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = CONFIG_EXAMPLE_HCI_UART_FLOW_CTRL,
        .source_clk = UART_SCLK_DEFAULT,
    };

    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    s_shutdown_flag = false;  // Reset shutdown flag on open
    ESP_ERROR_CHECK(uart_driver_install(UART_NO, UART_BUF_SZ * 2, UART_BUF_SZ * 2, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NO, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NO, UART_TX_PIN, UART_RX_PIN, -1, -1));

    xTaskCreate(hci_uart_rx_task, "hci_uart_rx_task", 2048, NULL, 12, &s_rx_task_hdl);
}

void hci_uart_close(void)
{
    if (s_rx_task_hdl) {

        s_shutdown_flag = true;

        int wait_count = 0;
        const int max_wait_count = 5;
        TaskHandle_t task_handle = s_rx_task_hdl;

        while (wait_count < max_wait_count) {
            vTaskDelay(pdMS_TO_TICKS(100));
            wait_count++;

        }


        if (s_rx_task_hdl == task_handle) {
            vTaskDelete(s_rx_task_hdl);
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        s_rx_task_hdl = NULL;
    }

    uart_driver_delete(UART_NO);
}
