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

//---最终应该设计成在menuconfig中可配置的
#define QUEUE_LEN                     5
#define WAKE_SLEEP_TASK_STACK_SIZE    2048
#define WAKE_SLEEP_TASK_PRIORITY      10
#define PULL_UP_EN                    false
#define PULL_DOWN_EN                  true


#define ESP_INTR_FLAG_DEFAULT 0

typedef struct{
    uint32_t gpio_num;
}queue_items_t;

static const char *TAG = "gpio_wakeup_sleep";
QueueHandle_t gpio_evt_queue = NULL;//声明队列

//中断服务程序
static void IRAM_ATTR gpio_isr_handler(void* arg)
{

    queue_items_t isr_arg = 
    {
        .gpio_num = GPIO_WAKEUP_SLEEP_NUM
    };
    xQueueSendFromISR(gpio_evt_queue, &isr_arg, NULL);//将触发中断的引脚号放入队列

    // /* Enable wake up from GPIO */
    // // low level wake up
    // ESP_RETURN_ON_ERROR(gpio_wakeup_enable(GPIO_WAKEUP_NUM, GPIO_WAKEUP_LEVEL == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL),
    //                     TAG, "Enable gpio wakeup failed");
    // ESP_RETURN_ON_ERROR(esp_sleep_enable_gpio_wakeup(), TAG, "Configure gpio as wakeup source failed");

    // //设置在light sleep时，io口保持high level
    // ESP_RETURN_ON_ERROR(gpio_sleep_set_pull_mode(GPIO_WAKEUP_SLEEP_NUM, GPIO_PULLUP_ONLY));

    // //改为下降沿触发中断
    // gpio_intr_disable(GPIO_WAKEUP_NUM);
    // gpio_set_intr_type(GPIO_WAKEUP_NUM, GPIO_INTR_NEGEDGE);
    // gpio_intr_enable(GPIO_WAKEUP_NUM);
}

//另开启的一个task
static void gpio_task_example(void* arg)
{
    queue_items_t queue_item;
    for(;;) {//循环，从队列中取数据
        if(xQueueReceive(gpio_evt_queue, &queue_item, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", queue_item.gpio_num, gpio_get_level(queue_item.gpio_num));

            // handle lock
            //ESP_ERROR_CHECK(esp_pm_lock_acquire(light_sleep_handle));
            //ESP_LOGI(TAG, "gpio wake up success, and handle no_light_sleep lock success!");
        }
    }
}

void example_wait_gpio_inactive(void)
{
    //init时，先等待IO口变为low level
    if(gpio_get_level(GPIO_WAKEUP_SLEEP_NUM) == GPIO_SLEEP_LEVEL)
        printf("Waiting for GPIO%d to go low...\n", GPIO_WAKEUP_SLEEP_NUM);
    while (gpio_get_level(GPIO_WAKEUP_SLEEP_NUM) == GPIO_SLEEP_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

esp_err_t example_register_gpio_wakeup_sleep(void)
{
    /* Initialize GPIO */
    gpio_config_t config = {
        .pin_bit_mask = BIT64(GPIO_WAKEUP_SLEEP_NUM),
        //set as input mode
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = PULL_UP_EN,
        .pull_down_en = PULL_DOWN_EN,
        .intr_type = TO_SLEEP_INTR_TYPE //上电后，应设置high level intr
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "Initialize GPIO%d failed", GPIO_WAKEUP_SLEEP_NUM);

    /* Make sure the GPIO is inactive and it won't trigger wakeup immediately */
    example_wait_gpio_inactive();
    // system sctive时，io口应保持low level
    ESP_RETURN_ON_ERROR(gpio_hold_en(GPIO_WAKEUP_SLEEP_NUM), TAG, "enable gpio%d hold failed", GPIO_WAKEUP_SLEEP_NUM);
    ESP_LOGI(TAG, "gpio is ready");

    //create a queue to handle gpio event from isr（中断服务程序）
    // item len的大小需要评估，频繁切换的时候，要预留  需要测试板
    // item size大小后面可能需要修改，放到结构体中，方便后期修改
    gpio_evt_queue = xQueueCreate(QUEUE_LEN, sizeof(queue_items_t));

    //start gpio task  创建一个task，并开始运行
    //stack导出去 可配置，如果用户在task中malloc stack，那么2048就不够用了
    //task的优先级：有多种task时，优先级也会有影响运行效果，一般设置成10
    xTaskCreate(gpio_task_example, "gpio_task_example", WAKE_SLEEP_TASK_STACK_SIZE, NULL, WAKE_SLEEP_TASK_PRIORITY, NULL);
    ESP_LOGI(TAG, "gpio task is ready");

    //install gpio isr service
    ESP_RETURN_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), TAG, "install isr failed");
    //hook isr handler for specific gpio pin 
    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(GPIO_WAKEUP_SLEEP_NUM, gpio_isr_handler, NULL), TAG, "add isr for specific gpio pin failed");
    ESP_LOGI(TAG, "gpio interrupt is ready");

    return ESP_OK;
}