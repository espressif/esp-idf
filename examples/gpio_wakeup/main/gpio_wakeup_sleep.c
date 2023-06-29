/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include "gpio_wakeup_example.h"

/* Most development boards have "boot" button attached to GPIO0.
 * You can also change this to another pin.
 */
#if CONFIG_IDF_TARGET_ESP32C3 || CONFIG_IDF_TARGET_ESP32C2 || CONFIG_IDF_TARGET_ESP32H2 \
    || CONFIG_IDF_TARGET_ESP32C6
#define BOOT_BUTTON_NUM         9
#else
#define BOOT_BUTTON_NUM         0
#endif

/* Use boot button as gpio input */
#define GPIO_WAKEUP_NUM         BOOT_BUTTON_NUM
/* "Boot" button is active low */
#define GPIO_WAKEUP_LEVEL       0

#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "gpio_wakeup_sleep";
QueueHandle_t gpio_evt_queue = NULL;//声明队列

//中断服务程序
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    //禁止gpio interrupt
    gpio_intr_disable(GPIO_WAKEUP_NUM);
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);//将触发中断的引脚号放入队列

    //开启中断
    gpio_intr_enable(GPIO_WAKEUP_NUM);
}

//另开启的一个task
static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for(;;) {//循环，从队列中取数据
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));

            // handle lock
            ESP_ERROR_CHECK(esp_pm_lock_acquire(light_sleep_handle));
            ESP_LOGI(TAG, "gpio wake up success, and handle no_light_sleep lock success!");
        }
    }
}

void example_wait_gpio_inactive(void)
{
    if(gpio_get_level(GPIO_WAKEUP_NUM) == GPIO_WAKEUP_LEVEL)
        printf("Waiting for GPIO%d to go high...\n", GPIO_WAKEUP_NUM);
    while (gpio_get_level(GPIO_WAKEUP_NUM) == GPIO_WAKEUP_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t example_register_gpio_wakeup(void)
{
    /* Initialize GPIO */
    gpio_config_t config = {
            .pin_bit_mask = BIT64(GPIO_WAKEUP_NUM),
            .mode = GPIO_MODE_INPUT,
            .pull_down_en = false,
            .pull_up_en = false,
            .intr_type = GPIO_INTR_LOW_LEVEL  //enable interrupt
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "Initialize GPIO%d failed", GPIO_WAKEUP_NUM);

    /* Enable wake up from GPIO */
    // low level wake up
    ESP_RETURN_ON_ERROR(gpio_wakeup_enable(GPIO_WAKEUP_NUM, GPIO_WAKEUP_LEVEL == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL),
                        TAG, "Enable gpio wakeup failed");
    ESP_RETURN_ON_ERROR(esp_sleep_enable_gpio_wakeup(), TAG, "Configure gpio as wakeup source failed");

    /* Make sure the GPIO is inactive and it won't trigger wakeup immediately */
    example_wait_gpio_inactive();
    ESP_LOGI(TAG, "gpio wakeup source is ready");

    /* Initialize ESP_PM_NO_LIGHT_SLEEP lock */
    esp_pm_lock_type_t light_sleep_lock = ESP_PM_NO_LIGHT_SLEEP;
    ESP_RETURN_ON_ERROR(esp_pm_lock_create(light_sleep_lock, 0, "contorl_light_sleep", &light_sleep_handle), TAG, "create NO_LIGHT_SLEEP lock failed");
    ESP_LOGI(TAG, "create no_light_sleep lock success!");

    //create a queue to handle gpio event from isr（中断服务程序）
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));  //创建队列，用于存放isr存入的数据
    //start gpio task  创建一个task，并开始运行
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //install gpio isr service
    ESP_RETURN_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), TAG, "install isr failed");
    //hook isr handler for specific gpio pin
    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(GPIO_WAKEUP_NUM, gpio_isr_handler, (void*) GPIO_WAKEUP_NUM), TAG, "add isr for specific gpio pin failed");
    ESP_LOGI(TAG, "gpio interrupt is ready");

    return ESP_OK;
}
