/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "hci_uart.h"
#include "esp_log.h"
#include "esp_attr.h"

#ifdef CONFIG_BT_LE_HCI_INTERFACE_USE_UART

static const char *TAG = "hci_uart";

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

#define HCI_UART_TX_PIN CONFIG_BT_LE_HCI_UART_TX_PIN
#define HCI_UART_RX_PIN CONFIG_BT_LE_HCI_UART_RX_PIN


#ifdef CONFIG_BT_LE_HCI_UART_FLOWCTRL
#define HCI_UART_FLOWCTRL UART_HW_FLOWCTRL_CTS_RTS
#define HCI_UART_RTS_PIN CONFIG_BT_LE_HCI_UART_RTS_PIN
#define HCI_UART_CTS_PIN CONFIG_BT_LE_HCI_UART_CTS_PIN
#else
#define HCI_UART_FLOWCTRL UART_HW_FLOWCTRL_DISABLE
#define HCI_UART_RTS_PIN (-1)
#define HCI_UART_CTS_PIN (-1)
#endif


typedef struct {
    bool uart_opened;
    uart_port_t port;
    uart_config_t cfg;
    QueueHandle_t evt_queue;
    TaskHandle_t rx_task_handler;
    hci_uart_tx_char tx_char;
    hci_uart_tx_done tx_done;
    hci_uart_rx_char rx_char;
    void *u_func_arg;

} hci_uart_t;

static hci_uart_t hci_uart;

static void IRAM_ATTR hci_uart_rx_task(void *pvParameters)
{
    uart_event_t event;
    uint8_t *dtmp = (uint8_t *) malloc(RD_BUF_SIZE);
    while (hci_uart.uart_opened) {
        //Waiting for UART event.
        if (xQueueReceive(hci_uart.evt_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGD(TAG, "uart[%d] event:", hci_uart.port);
            switch (event.type) {
            //Event of UART receving data
            /*We'd better handler data event fast, there would be much more data events than
            other types of events. If we take too much time on data event, the queue might
            be full.*/
            case UART_DATA:
                // ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(hci_uart.port, dtmp, event.size, portMAX_DELAY);
                for (int i = 0 ; i < event.size; i++) {
                    hci_uart.rx_char(hci_uart.u_func_arg, dtmp[i]);
                }
                break;
            //Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                uart_flush_input(hci_uart.port);
                xQueueReset(hci_uart.evt_queue);
                break;
            //Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                uart_flush_input(hci_uart.port);
                xQueueReset(hci_uart.evt_queue);
                break;
            //Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            //Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            //Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            //Others
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    hci_uart.rx_task_handler = NULL;
    vTaskDelete(NULL);
}

int hci_uart_config(int port_num, int32_t baud_rate, uint8_t data_bits, uint8_t stop_bits,
                    uart_parity_t parity, uart_hw_flowcontrol_t flow_ctl)
{
    uart_config_t uart_cfg = {
        .baud_rate = baud_rate,
        .data_bits = data_bits,
        .parity    = parity,
        .stop_bits = stop_bits,
        .flow_ctrl = HCI_UART_FLOWCTRL,
        .source_clk = UART_SCLK_DEFAULT,
        .rx_flow_ctrl_thresh = UART_HW_FIFO_LEN(port_num) - 1,
    };
    hci_uart.port = port_num;
    hci_uart.cfg = uart_cfg;

    int intr_alloc_flags = 0;
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;

    printf("set uart pin tx:%d, rx:%d.\n", HCI_UART_TX_PIN, HCI_UART_RX_PIN);
    printf("set rts:%d, cts:%d.\n", HCI_UART_RTS_PIN, HCI_UART_CTS_PIN);
    printf("set baud_rate:%d.\n", baud_rate);

    ESP_ERROR_CHECK(uart_driver_delete(port_num));
    ESP_ERROR_CHECK(uart_driver_install(port_num, BUF_SIZE * 2, BUF_SIZE * 2, 20, &hci_uart.evt_queue, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(port_num, &hci_uart.cfg));
    ESP_ERROR_CHECK(uart_set_pin(port_num, HCI_UART_TX_PIN, HCI_UART_RX_PIN, HCI_UART_RTS_PIN, HCI_UART_CTS_PIN));

    hci_uart.uart_opened = true;

    //Create a task to handler UART event from ISR
    xTaskCreate(hci_uart_rx_task, "hci_uart_rx_task", 2048, NULL, 12, &hci_uart.rx_task_handler);
    return 0;
}

void IRAM_ATTR hci_uart_start_tx(int port_num)
{
    int data;
    uint8_t u8_data = 0;
    while (1) {
        data = hci_uart.tx_char(hci_uart.u_func_arg);
        if (data >= 0) {
            u8_data = data;
            uart_write_bytes(port_num, (char *)&u8_data, 1);
        } else {
            break;
        }
    }
    if (hci_uart.tx_done) {
        hci_uart.tx_done(hci_uart.u_func_arg);
    }
}

int hci_uart_init_cbs(int port_num, hci_uart_tx_char tx_func,
                      hci_uart_tx_done tx_done, hci_uart_rx_char rx_func, void *arg)
{
    hci_uart.tx_char = tx_func;
    hci_uart.rx_char = rx_func;
    hci_uart.tx_done = tx_done;
    hci_uart.u_func_arg = arg;
    return 0;
}

int hci_uart_close(int port_num)
{
    uart_event_t uart_event;
    uart_event.type = UART_BREAK;
    hci_uart.uart_opened = false;
    // Stop uart rx task
    if (hci_uart.rx_task_handler != NULL) {
        xQueueSend(hci_uart.evt_queue, (void *)&uart_event, 1000);
        ESP_LOGW(TAG, "Waiting for uart task finish...");
    }
    while (hci_uart.rx_task_handler != NULL);

    uart_driver_delete(port_num);
    ESP_LOGI(TAG, "hci uart close success.");
    return 0;
}

int hci_uart_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin)
{
    int port_num = hci_uart.port;
    int32_t baud_rate = hci_uart.cfg.baud_rate;
    uint8_t data_bits = hci_uart.cfg.data_bits;
    uint8_t stop_bits = hci_uart.cfg.stop_bits;
    uart_parity_t parity = hci_uart.cfg.parity;
    uart_hw_flowcontrol_t flow_ctl = hci_uart.cfg.flow_ctrl;
    hci_uart_close(port_num);
    hci_uart_config(port_num, baud_rate, data_bits, stop_bits, parity, flow_ctl);
    ESP_ERROR_CHECK(uart_set_pin(port_num, tx_pin, rx_pin, rts_pin, cts_pin));
    return 0;
}
#endif //CONFIG_BT_LE_HCI_INTERFACE_USE_UART
