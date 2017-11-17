/* UART Events Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

static const char *TAG = "uart_events";

/**
 * This example shows how to use the UART driver to handle special UART events.
 *
 * It also reads data from UART0 directly, and echoes it to console.
 *
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: on
 * - Pin assignment: TxD (default), RxD (default)
 */

#define EX_UART_NUM UART_NUM_0

#define BUF_SIZE (1024)
static QueueHandle_t uart0_queue;

static void uart_event_task(void *pvParameters)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t *dtmp = (uint8_t *) malloc(BUF_SIZE);
    while (1) {
        /* Waiting for UART event.
           If it happens then print out information what is it */
        if (xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            ESP_LOGI(TAG, "uart[%d] event:", EX_UART_NUM);
            switch (event.type) {
            case UART_DATA:
                /* Event of UART receiving data
                 * We'd better handler data event fast, there would be much more data events
                 * than other types of events.
                 * If we take too much time on data event, the queue might be full.
                 * In this example, we don't process data in event, but read data outside.
                 */
                uart_get_buffered_data_len(EX_UART_NUM, &buffered_size);
                ESP_LOGI(TAG, "data, len: %d; buffered len: %d", event.size, buffered_size);
                break;
            case UART_FIFO_OVF:
                ESP_LOGE(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // We can read data out out the buffer, or directly flush the Rx buffer.
                uart_flush(EX_UART_NUM);
                break;
            case UART_BUFFER_FULL:
                ESP_LOGE(TAG, "ring buffer full");
                // If buffer full happened, you should consider increasing your buffer size
                // We can read data out out the buffer, or directly flush the Rx buffer.
                uart_flush(EX_UART_NUM);
                break;
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break detected");
                break;
            case UART_PARITY_ERR:
                ESP_LOGE(TAG, "uart parity error");
                break;
            case UART_FRAME_ERR:
                ESP_LOGE(TAG, "uart frame error");
                break;
            case UART_PATTERN_DET:
                ESP_LOGI(TAG, "uart pattern detected");
                break;
            default:
                ESP_LOGE(TAG, "not serviced uart event type: %d\n", event.type);
                break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

void app_main()
{
    esp_log_level_set(TAG, ESP_LOG_INFO);

    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(EX_UART_NUM, &uart_config);
    // Set UART pins using UART0 default pins i.e. no changes
    uart_set_pin(EX_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(EX_UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart0_queue, 0);

    // Set uart pattern detection function
    uart_enable_pattern_det_intr(EX_UART_NUM, '+', 3, 10000, 10, 10);

    // Create a task to handle uart event from ISR
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);

    // Reserve a buffer and process incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    while (1) {
        int len = uart_read_bytes(EX_UART_NUM, data, BUF_SIZE, 100 / portTICK_RATE_MS);
        if (len > 0) {
            ESP_LOGI(TAG, "uart read : %d", len);
            uart_write_bytes(EX_UART_NUM, (const char *)data, len);
        }
    }
}
