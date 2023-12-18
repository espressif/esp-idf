/* UART Select Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/unistd.h>
#include <sys/select.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart_vfs.h"
#include "driver/uart.h"

static const char* TAG = "uart_select_example";

static void uart_select_task(void *arg)
{
    if (uart_driver_install(UART_NUM_0, 2 * 1024, 0, 0, NULL, 0) != ESP_OK) {
        ESP_LOGE(TAG, "Driver installation failed");
        vTaskDelete(NULL);
    }

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    uart_param_config(UART_NUM_0, &uart_config);

    while (1) {
        int fd;

        if ((fd = open("/dev/uart/0", O_RDWR)) == -1) {
            ESP_LOGE(TAG, "Cannot open UART");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
            continue;
        }

        // We have a driver now installed so set up the read/write functions to use driver also.
        uart_vfs_dev_use_driver(0);

        while (1) {
            int s;
            fd_set rfds;
            struct timeval tv = {
                .tv_sec = 5,
                .tv_usec = 0,
            };

            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);

            s = select(fd + 1, &rfds, NULL, NULL, &tv);

            if (s < 0) {
                ESP_LOGE(TAG, "Select failed: errno %d", errno);
                break;
            } else if (s == 0) {
                ESP_LOGI(TAG, "Timeout has been reached and nothing has been received");
            } else {
                if (FD_ISSET(fd, &rfds)) {
                    char buf;
                    if (read(fd, &buf, 1) > 0) {
                        ESP_LOGI(TAG, "Received: %c", buf);
                        // Note: Only one character was read even the buffer contains more. The other characters will
                        // be read one-by-one by subsequent calls to select() which will then return immediately
                        // without timeout.
                    } else {
                        ESP_LOGE(TAG, "UART read error");
                        break;
                    }
                } else {
                    ESP_LOGE(TAG, "No FD has been set in select()");
                    break;
                }
            }
        }

        close(fd);
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    xTaskCreate(uart_select_task, "uart_select_task", 4 * 1024, NULL, 5, NULL);
}
