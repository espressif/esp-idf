/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_heap_trace.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO
   to blink or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

void blink_task2(void *pvParameter)
{
    TaskHandle_t task = (TaskHandle_t)pvParameter;

    while(1) {
        if (xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) != pdPASS) {
            printf("Failed to wait for main task!\n");
            return;
        }
        void *a = malloc(65);
        printf("Alloced 65 bytes @ %p\n", a);
        void *b = malloc(97);
        printf("Alloced 97 bytes @ %p\n", b);
        free(a);
        printf("Freed @ %p\n", a);
        b = malloc(11);
        printf("Alloced 11 bytes @ %p\n", b);
        b = malloc(24);
        printf("Alloced 24 bytes @ %p\n", b);
        free(b);
        printf("Freed @ %p\n", b);
        if (xTaskNotify(task, 0, eNoAction) != pdPASS) {
            printf("Failed to notify main task!\n");
            return;
        }
    }
}

void blink_task(void *pvParameter)
{
    TaskHandle_t task = 0;

    if(heap_trace_init_tohost() != ESP_OK) {
        printf("Failed to init heap trace!\n");
        return;
    }
    xTaskCreatePinnedToCore(&blink_task2, "blink_task2", 2048, xTaskGetCurrentTaskHandle(), 5, &task, 1);

    while(1) {
        heap_trace_start(HEAP_TRACE_ALL);
        if (xTaskNotify(task, 0, eNoAction) != pdPASS) {
            printf("Failed to notify slave task!\n");
            return;
        }
        void *a = malloc(64);
        printf("Alloced 64 bytes @ %p\n", a);
        void *b = malloc(96);
        printf("Alloced 96 bytes @ %p\n", b);
        free(a);
        printf("Freed @ %p\n", a);
        b = malloc(10);
        printf("Alloced 10 bytes @ %p\n", b);
        b = malloc(23);
        printf("Alloced 23 bytes @ %p\n", b);
        free(b);
        printf("Freed @ %p\n", b);
        if (xTaskNotifyWait(0, 0, NULL, portMAX_DELAY) != pdPASS) {
            printf("Failed to wait for slave task!\n");
            return;
        }
        heap_trace_stop();
    }
}

void app_main(void)
{
    xTaskCreatePinnedToCore(&blink_task, "blink_task", 2048, NULL, 5, NULL, 0);
}
