/* Blink Example with covergae info

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_gcov.h"
#include "sdkconfig.h"
#include "esp_log.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO
   to blink, or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

static const char *TAG = "example";

void blink_dummy_func(void);
void some_dummy_func(void);

#if !CONFIG_APPTRACE_DEST_JTAG
#include "soc/uart_pins.h"
#include "esp_app_trace.h"
/* Override default uart config to use console pins as a uart channel */
esp_apptrace_config_t esp_apptrace_get_user_params(void)
{
    esp_apptrace_config_t config = APPTRACE_UART_CONFIG_DEFAULT();
    config.dest_cfg.uart.uart_num = 0;
    config.dest_cfg.uart.tx_pin_num = U0TXD_GPIO_NUM;
    config.dest_cfg.uart.rx_pin_num = U0RXD_GPIO_NUM;
    return config;
}
#endif

static void blink_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Ready for OpenOCD connection");

    // The first two iterations GCOV data are dumped using call to esp_gcov_dump() and OOCD's "esp32 gcov dump" command.
    // After that they can be dumped using OOCD's "esp32 gcov" command only.
    int dump_gcov_after = -2;
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        blink_dummy_func();
        some_dummy_func();
        if (dump_gcov_after++ < 0) {
            // Dump gcov data
            printf("Ready to dump GCOV data...\n");
            esp_gcov_dump();
            printf("GCOV data have been dumped.\n");
        }
    }
}

void app_main(void)
{
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
}
