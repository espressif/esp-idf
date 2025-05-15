/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_sleep.h"
#include "soc/uart_pins.h"
#include "driver/uart.h"
#include "driver/uart_wakeup.h"
#include "sdkconfig.h"

#define EXAMPLE_UART_NUM        0
/* Notice that ESP32 has to use the iomux input to configure uart as wakeup source
 * Please use 'UxRXD_GPIO_NUM' as uart rx pin. No limitation to the other target */
#define EXAMPLE_UART_TX_IO_NUM  U0TXD_GPIO_NUM
#define EXAMPLE_UART_RX_IO_NUM  U0RXD_GPIO_NUM

#define EXAMPLE_UART_WAKEUP_EDGE_THRESHOLD   3
#define EXAMPLE_UART_WAKEUP_FIFO_THRESHOLD   8
#define EXAMPLE_UART_WAKEUP_CHARS_SEQ        "ok"
#define EXAMPLE_UART_WAKEUP_CHARS_SEQ_LEN    SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN

#define EXAMPLE_READ_BUF_SIZE   1024
#define EXAMPLE_UART_BUF_SIZE   (EXAMPLE_READ_BUF_SIZE * 2)

static const char *TAG = "uart_wakeup";

static QueueHandle_t uart_evt_que = NULL;

static void uart_wakeup_task(void *arg)
{
    uart_event_t event;
    if (uart_evt_que == NULL) {
        ESP_LOGE(TAG, "uart_evt_que is NULL");
        abort();
    }

    uint8_t* dtmp = (uint8_t*) malloc(EXAMPLE_READ_BUF_SIZE);
    assert(dtmp);

    while(1) {
        // Waiting for UART event.
        if(xQueueReceive(uart_evt_que, (void * )&event, (TickType_t)portMAX_DELAY)) {
            ESP_LOGI(TAG, "uart%d recved event:%d", EXAMPLE_UART_NUM, event.type);
            switch(event.type) {
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(EXAMPLE_UART_NUM, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]:");
                    uart_write_bytes(EXAMPLE_UART_NUM, (const char *)dtmp, event.size);
                    break;
                // Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EXAMPLE_UART_NUM);
                    xQueueReset(uart_evt_que);
                    break;
                // Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(EXAMPLE_UART_NUM);
                    xQueueReset(uart_evt_que);
                    break;
                // Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                // Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                // Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                // ESP32 can wakeup by uart but there is no wake up interrupt
#if SOC_UART_SUPPORT_WAKEUP_INT
                // Event of waking up by UART
                case UART_WAKEUP:
                    ESP_LOGI(TAG, "uart wakeup");
                    break;
#endif
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    vTaskDelete(NULL);
}

static void uart_initialization(void)
{
    uart_config_t uart_cfg = {
        .baud_rate  = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
#if SOC_UART_SUPPORT_REF_TICK
        .source_clk = UART_SCLK_REF_TICK,
#elif SOC_UART_SUPPORT_XTAL_CLK
        .source_clk = UART_SCLK_XTAL
#endif
    };
    //Install UART driver, and get the queue.
    ESP_ERROR_CHECK(uart_driver_install(EXAMPLE_UART_NUM, EXAMPLE_UART_BUF_SIZE, EXAMPLE_UART_BUF_SIZE, 20, &uart_evt_que, 0));
    if (EXAMPLE_UART_NUM == CONFIG_ESP_CONSOLE_UART_NUM) {
        /* temp fix for uart garbled output, can be removed when IDF-5683 done */
        ESP_ERROR_CHECK(uart_wait_tx_idle_polling(EXAMPLE_UART_NUM));
    }
    ESP_ERROR_CHECK(uart_param_config(EXAMPLE_UART_NUM, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(EXAMPLE_UART_NUM, EXAMPLE_UART_TX_IO_NUM, EXAMPLE_UART_RX_IO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

static void uart_wakeup_config(void)
{
    uart_wakeup_cfg_t uart_wakeup_cfg = {};
    uint8_t wakeup_mode = CONFIG_EXAMPLE_UART_WAKEUP_MODE_SELCTED;
    switch (wakeup_mode) {
    /* UART will wakeup the chip up from light sleep if the edges that RX pin received reaches the threshold */
#if SOC_UART_WAKEUP_SUPPORT_ACTIVE_THRESH_MODE
    case UART_WK_MODE_ACTIVE_THRESH:
        uart_wakeup_cfg.wakeup_mode = UART_WK_MODE_ACTIVE_THRESH;
        uart_wakeup_cfg.rx_edge_threshold = EXAMPLE_UART_WAKEUP_EDGE_THRESHOLD;
        break;
#endif
    /* UART will wakeup the chip up from light sleep if the number of chars that RX FIFO received reaches the threshold */
#if SOC_UART_WAKEUP_SUPPORT_FIFO_THRESH_MODE
    case UART_WK_MODE_FIFO_THRESH:
        uart_wakeup_cfg.wakeup_mode = UART_WK_MODE_FIFO_THRESH;
        uart_wakeup_cfg.rx_fifo_threshold = EXAMPLE_UART_WAKEUP_FIFO_THRESHOLD;
        break;
#endif
    /* UART will wakeup the chip up from light sleep if RX FIFO receives a start bit */
#if SOC_UART_WAKEUP_SUPPORT_START_BIT_MODE
    case UART_WK_MODE_START_BIT:
        uart_wakeup_cfg.wakeup_mode = UART_WK_MODE_START_BIT;
        break;
#endif
    /* UART will wakeup the chip up from light sleep if the chars sequence that RX FIFO received matches the predefined value */
#if SOC_UART_WAKEUP_SUPPORT_CHAR_SEQ_MODE
    case UART_WK_MODE_CHAR_SEQ:
        uart_wakeup_cfg.wakeup_mode = UART_WK_MODE_CHAR_SEQ;
        // uart wakeup chars len need less than SOC_UART_WAKEUP_CHARS_SEQ_MAX_LEN
        strncpy(uart_wakeup_cfg.wake_chars_seq, EXAMPLE_UART_WAKEUP_CHARS_SEQ, EXAMPLE_UART_WAKEUP_CHARS_SEQ_LEN);
        break;
#endif
    default:
        ESP_LOGE(TAG, "Unknown UART wakeup mode");
        ESP_ERROR_CHECK(ESP_FAIL);
        break;
    }

    ESP_ERROR_CHECK(uart_wakeup_setup(EXAMPLE_UART_NUM, &uart_wakeup_cfg));
    ESP_ERROR_CHECK(esp_sleep_enable_uart_wakeup(EXAMPLE_UART_NUM));
}

esp_err_t example_register_uart_wakeup(void)
{
    /* Initialize console uart */
    uart_initialization();
    /* Enable wakeup from uart */
    uart_wakeup_config();

    xTaskCreate(uart_wakeup_task, "uart_wakeup_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "uart wakeup source is ready");

    return ESP_OK;
}
