/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "soc/uart_pins.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#define EXAMPLE_UART_NUM        0
/* Notice that ESP32 has to use the iomux input to configure uart as wakeup source
 * Please use 'UxRXD_GPIO_NUM' as uart rx pin. No limitation to the other target */
#define EXAMPLE_UART_TX_IO_NUM  U0TXD_GPIO_NUM
#define EXAMPLE_UART_RX_IO_NUM  U0RXD_GPIO_NUM

#define EXAMPLE_UART_WAKEUP_THRESHOLD   3

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

static esp_err_t uart_initialization(void)
{
    uart_config_t uart_cfg = {
        .baud_rate  = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
        .data_bits  = UART_DATA_8_BITS,
        .parity     = UART_PARITY_DISABLE,
        .stop_bits  = UART_STOP_BITS_1,
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    //Install UART driver, and get the queue.
    ESP_RETURN_ON_ERROR(uart_driver_install(EXAMPLE_UART_NUM, EXAMPLE_UART_BUF_SIZE, EXAMPLE_UART_BUF_SIZE, 20, &uart_evt_que, 0),
                        TAG, "Install uart failed");
    if (EXAMPLE_UART_NUM == CONFIG_ESP_CONSOLE_UART_NUM) {
        /* temp fix for uart garbled output, can be removed when IDF-5683 done */
        ESP_RETURN_ON_ERROR(uart_wait_tx_idle_polling(EXAMPLE_UART_NUM), TAG, "Wait uart tx done failed");
    }
    ESP_RETURN_ON_ERROR(uart_param_config(EXAMPLE_UART_NUM, &uart_cfg), TAG, "Configure uart param failed");
    ESP_RETURN_ON_ERROR(uart_set_pin(EXAMPLE_UART_NUM, EXAMPLE_UART_TX_IO_NUM, EXAMPLE_UART_RX_IO_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE),
                        TAG, "Configure uart gpio pins failed");
    return ESP_OK;
}

static esp_err_t uart_wakeup_config(void)
{
    /* UART will wakeup the chip up from light sleep if the edges that RX pin received has reached the threshold
     * Besides, the Rx pin need extra configuration to enable it can work during light sleep */
    ESP_RETURN_ON_ERROR(gpio_sleep_set_direction(EXAMPLE_UART_RX_IO_NUM, GPIO_MODE_INPUT), TAG, "Set uart sleep gpio failed");
    ESP_RETURN_ON_ERROR(gpio_sleep_set_pull_mode(EXAMPLE_UART_RX_IO_NUM, GPIO_PULLUP_ONLY), TAG, "Set uart sleep gpio failed");
    ESP_RETURN_ON_ERROR(uart_set_wakeup_threshold(EXAMPLE_UART_NUM, EXAMPLE_UART_WAKEUP_THRESHOLD),
                        TAG, "Set uart wakeup threshold failed");
    /* Only uart0 and uart1 (if has) support to be configured as wakeup source */
    ESP_RETURN_ON_ERROR(esp_sleep_enable_uart_wakeup(EXAMPLE_UART_NUM),
                        TAG, "Configure uart as wakeup source failed");
    return ESP_OK;
}

esp_err_t example_register_uart_wakeup(void)
{
    /* Initialize uart1 */
    ESP_RETURN_ON_ERROR(uart_initialization(), TAG, "Initialize uart%d failed", EXAMPLE_UART_NUM);
    /* Enable wakeup from uart */
    ESP_RETURN_ON_ERROR(uart_wakeup_config(), TAG, "Configure uart as wakeup source failed");

    xTaskCreate(uart_wakeup_task, "uart_wakeup_task", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "uart wakeup source is ready");
    return ESP_OK;
}
