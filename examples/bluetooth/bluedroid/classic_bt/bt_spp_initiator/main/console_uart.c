/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "console_uart.h"
#include "app_spp_msg_prs.h"

#define CONSOLE_UART_NUM    UART_NUM_0

static QueueHandle_t uart_queue;
static spp_msg_prs_cb_t spp_msg_parser;

static const uart_config_t uart_cfg = {
    .baud_rate = 115200,   //1.5M
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .rx_flow_ctrl_thresh = 127,
    .source_clk = UART_SCLK_DEFAULT,
};

extern void spp_msg_args_parser(char *buf, int len);

void spp_msg_handler(char *buf, int len)
{
    ESP_LOGE(TAG_CNSL, "Command [%s]", buf);
    spp_msg_args_parser(buf, len);
}

static void console_uart_task(void *pvParameters)
{
    int len;
    uart_event_t event;
    spp_msg_prs_cb_t *parser = &spp_msg_parser;
    spp_msg_parser_reset_state(parser);
    spp_msg_parser_register_callback(parser, spp_msg_handler);
    spp_msg_show_usage();
#define TMP_BUF_LEN 128
    uint8_t tmp_buf[128] = {0};

    for (;;) {
        //Waiting for UART event.
        if (xQueueReceive(uart_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
            switch (event.type) {
                //Event of UART receving data
                case UART_DATA:
                {
                    len = uart_read_bytes(CONSOLE_UART_NUM, tmp_buf, TMP_BUF_LEN, 0);
                    for (int i = 0; i < len; i++) {
                        spp_msg_parse(tmp_buf[i], parser);
                    }
                    break;
                }
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                {
                    ESP_LOGI(TAG_CNSL, "hw fifo overflow");
                    break;
                }
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                {
                    ESP_LOGI(TAG_CNSL, "ring buffer full");
                    break;
                }
                //Event of UART RX break detected
                case UART_BREAK:
                {
                    ESP_LOGI(TAG_CNSL, "uart rx break");
                    break;
                }
                //Event of UART parity check error
                case UART_PARITY_ERR:
                {
                    ESP_LOGI(TAG_CNSL, "uart parity error");
                    break;
                }
                //Event of UART frame error
                case UART_FRAME_ERR:
                {
                    ESP_LOGI(TAG_CNSL, "uart frame error");
                    break;
                }
                //Others
                default:
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}


esp_err_t console_uart_init(void)
{
    esp_err_t ret;

    ret =  uart_param_config(CONSOLE_UART_NUM, &uart_cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG_CNSL, "Uart %d initialize err %04x", CONSOLE_UART_NUM, ret);
        return ret;
    }

    uart_set_pin(CONSOLE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(CONSOLE_UART_NUM, 1024, 1024, 8, &uart_queue, 0);
    xTaskCreate(console_uart_task, "uTask", 2048, NULL, 8, NULL);

    return ESP_OK;
}
