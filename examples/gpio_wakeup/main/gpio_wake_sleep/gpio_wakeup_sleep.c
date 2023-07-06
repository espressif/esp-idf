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
//EXAMPLE_
#define QUEUE_LEN                     (5)
#define WAKE_SLEEP_TASK_STACK_SIZE    (4096)

#define WAKE_SLEEP_TASK_PRIORITY      (10)

#define ESP_INTR_FLAG_DEFAULT (0)

typedef struct {
    software_state_t old_sys_state;
    //event
} queue_items_t;

//尽量少用全局变量，改称函数传参

//软件状态：时刻与硬件状态保持一致
software_state_t sys_state;
//pm lock handle
esp_pm_lock_handle_t light_sleep_handle; 
QueueHandle_t gpio_evt_queue = NULL;//声明队列
//task handle

static const char *TAG = "gpio_wakeup_sleep";

/* 中断服务程序有三个任务：
1. 改变中断触发方式
2. 更改软件状态
3. 通知task工作 */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    //1. 首先就要改变触发方式
    //描述原因：为什么diable intr，为什么修改intr type

    gpio_intr_disable(GPIO_WAKEUP_SLEEP_NUM);
    gpio_set_intr_type(GPIO_WAKEUP_SLEEP_NUM, sys_state == ACTIVE_STATE ? TO_ACTIVE_INTR_TYPE : TO_SLEEP_INTR_TYPE);
    //intr中尽量不要有打印
    esp_rom_printf("GPIO[%d] intr, val: %d, software state: %s\n", GPIO_WAKEUP_SLEEP_NUM, gpio_get_level(GPIO_WAKEUP_SLEEP_NUM), 
                    sys_state ? "active" : "sleep");
    
    queue_items_t isr_arg;
    switch (sys_state)
    {
        //to sleep
    case ACTIVE_STATE:
        //2. 更改软件状态
        MARK_SOFTWARE_STATE(LIGHT_SLEEP_STATE);

        //设置一个output引脚，每次反转电平，因为打印跟不上，无法用打印debug

        //3.通知task工作
        isr_arg.old_sys_state = ACTIVE_STATE;
        xQueueSendFromISR(gpio_evt_queue, &isr_arg, NULL);//将触发中断的引脚号放入队列

        esp_rom_printf("isr: system is active, ready to sleep\n");
        break;

        //to active
    case LIGHT_SLEEP_STATE:
        //2. 更改软件状态
        MARK_SOFTWARE_STATE(ACTIVE_STATE);
        
        //3.通知task工作
        isr_arg.old_sys_state = LIGHT_SLEEP_STATE;
        xQueueSendFromISR(gpio_evt_queue, &isr_arg, NULL);//将触发中断的引脚号放入队列

        esp_rom_printf("isr: system is sleep, ready to active\n");
        break;
    
    default:
        break;
    }
}

//开启的一个task: acquire lock or release lock
static void gpio_task_example(void* arg)
{
    queue_items_t queue_item;
    while (xQueueReceive(gpio_evt_queue, &queue_item, portMAX_DELAY)) {
        // printf("GPIO[%d] intr, val: %d\n", GPIO_WAKEUP_SLEEP_NUM, gpio_get_level(GPIO_WAKEUP_SLEEP_NUM));

        if (queue_item.old_sys_state == ACTIVE_STATE) {
            // to sleep
            gpio_intr_enable(GPIO_WAKEUP_SLEEP_NUM);
            ESP_ERROR_CHECK(esp_pm_lock_release(light_sleep_handle));
            ESP_LOGI(TAG, "release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
        } else {
            // to active
            ESP_ERROR_CHECK(esp_pm_lock_acquire(light_sleep_handle));
            gpio_intr_enable(GPIO_WAKEUP_SLEEP_NUM);
            ESP_LOGI(TAG, "acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);
        }
    }
}

void example_wait_gpio_inactive(void)
{
    //init时，先等待IO口变为low level
    if (gpio_get_level(GPIO_WAKEUP_SLEEP_NUM) != GPIO_WAKEUP_LEVEL) {
        esp_rom_printf("Waiting for GPIO%d to go low...\n", GPIO_WAKEUP_SLEEP_NUM);
    }
    while (gpio_get_level(GPIO_WAKEUP_SLEEP_NUM) != GPIO_WAKEUP_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "gpio is ready");
}

esp_err_t example_register_gpio_wakeup_sleep(void)
{
    /* Initialize GPIO */
    gpio_config_t config = {
        .pin_bit_mask = BIT64(GPIO_WAKEUP_SLEEP_NUM),
        //set as input mode
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = false,//根据唤醒方式决定上下拉
        .pull_down_en = true
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "Initialize GPIO%d failed", GPIO_WAKEUP_SLEEP_NUM);

    /* Make sure the GPIO is inactive and it won't trigger wakeup immediately */
    example_wait_gpio_inactive();

    //create a queue to handle gpio event from isr（中断服务程序）
    // item len的大小需要评估，频繁切换的时候，要预留  需要测试板
    // item size大小后面可能需要修改，放到结构体中，方便后期修改
    gpio_evt_queue = xQueueCreate(QUEUE_LEN, sizeof(queue_items_t));

    //返回值判定：如果queue create失败，就不继续

    //start gpio task  创建一个task，并开始运行
    //stack导出去 可配置，如果用户在task中malloc stack，那么2048就不够用了
    //task的优先级：有多种task时，优先级也会有影响运行效果，一般设置成10
    xTaskCreate(gpio_task_example, "gpio_task_example", WAKE_SLEEP_TASK_STACK_SIZE, NULL, WAKE_SLEEP_TASK_PRIORITY, NULL);
    //返回值判定：如果task create失败，就不继续
    ESP_LOGI(TAG, "gpio task is ready");

    //enable gpio wake
    ESP_RETURN_ON_ERROR(esp_sleep_enable_gpio_wakeup(), TAG, "Enable GPIO%d wakeup failed", GPIO_WAKEUP_SLEEP_NUM);
    ESP_LOGI(TAG, "enable gpio wakeup");

    //low level唤醒
    gpio_wakeup_enable(GPIO_WAKEUP_SLEEP_NUM, GPIO_WAKEUP_LEVEL == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    //set gpio intr type
    gpio_set_intr_type(GPIO_WAKEUP_SLEEP_NUM, TO_SLEEP_INTR_TYPE);

    //install gpio isr service
    ESP_RETURN_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), TAG, "install isr failed");

    //风险：intr已经enable，但是lock还没有create，应调整顺序

    //hook isr handler for specific gpio pin 
    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(GPIO_WAKEUP_SLEEP_NUM, gpio_isr_handler, NULL), TAG, "add isr for specific gpio pin failed");
    ESP_LOGI(TAG, "gpio interrupt is ready");

    return ESP_OK;
}