/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_bluedroid_hci.h"
#include "uart_driver.h"

#define TAG "UART_HCI"

#define UART_NO       (1)
#define UART_BUF_SZ   (1024)

#define UART_TX_PIN   (5)
#define UART_RX_PIN   (18)
#define UART_RTS_PIN  (19)
#define UART_CTS_PIN  (23)

enum {
    UART_RX_TYPE = 0,
    UART_RX_LEN,
    UART_RX_DATA,
};

enum {
    DATA_TYPE_COMMAND = 1,
    DATA_TYPE_ACL     = 2,
    DATA_TYPE_SCO     = 3,
    DATA_TYPE_EVENT   = 4
};

TaskHandle_t s_rx_task_hdl;
esp_bluedroid_hci_driver_callbacks_t s_callback = { 0 };

static void IRAM_ATTR hci_uart_rx_task(void *arg)
{
    uint8_t buf[1026];
    int len_now_read = -1;
    uint32_t len_to_read = 1;
    uint32_t len_total_read = 0;
    uint8_t rx_st = UART_RX_TYPE;

    while (1) {
        len_now_read = uart_read_bytes(UART_NO, &buf[len_total_read], len_to_read, portMAX_DELAY);
        assert(len_now_read == len_to_read);
        len_total_read += len_now_read;

        switch (rx_st) {
        case UART_RX_TYPE: {
            assert(buf[0] >= DATA_TYPE_ACL && buf[0] <= DATA_TYPE_EVENT);
            if (buf[0] == DATA_TYPE_ACL) {
                len_to_read = 4;
            } else if (buf[0] == DATA_TYPE_SCO) {
                len_to_read = 3;
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
            } else if (buf[0] == DATA_TYPE_SCO) {
                len_to_read = buf[3];
            } else if (buf[0] == DATA_TYPE_EVENT) {
                len_to_read = buf[2];
            } else {
                assert(0);
            }
            rx_st = UART_RX_DATA;
        }
        break;

        case UART_RX_DATA: {
            if (s_callback.notify_host_recv) {
                s_callback.notify_host_recv(buf, len_total_read);
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

bool hci_check_send_available(void)
{
    return true;
}

esp_err_t hci_register_host_callback(const esp_bluedroid_hci_driver_callbacks_t *callback)
{
    s_callback.notify_host_send_available = callback->notify_host_send_available;
    s_callback.notify_host_recv = callback->notify_host_recv;

    return ESP_OK;
}

void hci_uart_open(void)
{
    uart_config_t uart_config = {
        .baud_rate  = CONFIG_EXAMPLE_HCI_UART_BAUDRATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_CTS_RTS,
        .source_clk = UART_SCLK_DEFAULT,
        .rx_flow_ctrl_thresh = UART_HW_FIFO_LEN(UART_NO) - 1,
    };

    int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_NO, UART_BUF_SZ * 2, UART_BUF_SZ * 2, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NO, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NO, UART_TX_PIN, UART_RX_PIN, UART_RTS_PIN, UART_CTS_PIN));

    xTaskCreate(hci_uart_rx_task, "hci_uart_rx_task", 2048, NULL, 12, &s_rx_task_hdl);
}

void hci_uart_close(void)
{
    if (s_rx_task_hdl) {
        vTaskDelete(s_rx_task_hdl);
    }
    uart_driver_delete(UART_NO);
    memset(&s_callback, 0, sizeof(esp_bluedroid_hci_driver_callbacks_t));
}
