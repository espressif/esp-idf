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


//-----------最终应该设计成在menuconfig中可配置的--------------
#define EXAMPLE_EVENT_QUEUE_LEN          5
#define EXAMPLE_EVENT_TASK_STACK_SIZE    4096
#define EXAMPLE_EVENT_TASK_PRIORITY      10
//-----------最终应该设计成在menuconfig中可配置的--------------

#define ESP_INTR_FLAG_DEFAULT (0)

/* message passed to task by interrupt function */
typedef struct {
    task_event_t pending_evt
} msg_t;


static const char *TAG = "gpio_wakeup_sleep";

/* interrupt function have two tasks：
1. modify interrupt trigger types
2. notify task to work */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    //1. 首先就要改变触发方式
    //文档中描述原因：为什么diable intr，为什么修改intr type
    gpio_intr_disable(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM);
    gpio_ws_t* gpio_ws = (gpio_ws_t*)arg; 
    gpio_set_intr_type(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM, gpio_ws->hold_lock_state == HOLD_LOCK_STATE ? TO_ACTIVE_INTR_TYPE : TO_SLEEP_INTR_TYPE);
    //intr中尽量不要有打印
    esp_rom_printf("GPIO[%d] intr, val: %d, software state: %s\n", EXAMPLE_GPIO_WAKEUP_SLEEP_NUM, gpio_get_level(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM), 
                    gpio_ws->hold_lock_state ? "hold lock" : "no lock");
    
    // gpio isr只是gpio的isr，所以不会再有其他状态，直接使用三目运算符就可以
    msg_t send_task_msg = {
        .pending_evt = (gpio_ws->hold_lock_state == HOLD_LOCK_STATE) ? PM_LOCK_EVENT_RELEASE : PM_LOCK_EVENT_ACQUIRE
    };
    xQueueSendFromISR(gpio_ws->evt_queue, &send_task_msg, NULL);
    
    // 再来一个output pin，便于电流表上观察

    // enable intr?是不是更合适
}

//开启的一个task: acquire lock or release lock
static void example_event_task(void* arg)
{
    gpio_ws_t* gpio_ws = (gpio_ws_t*)arg; 
    msg_t recv_isr_msg;
    while (xQueueReceive(gpio_ws->evt_queue, &recv_isr_msg, portMAX_DELAY)) {
        // printf("GPIO[%d] intr, val: %d\n", EXAMPLE_GPIO_WAKEUP_SLEEP_NUM, gpio_get_level(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM));

        // switch中加一个output pin，便于电流表观察
        
        switch (recv_isr_msg.pending_evt)
        {
        case PM_LOCK_EVENT_RELEASE:// to sleep
            
            ESP_ERROR_CHECK(esp_pm_lock_release(gpio_ws->pm_lock));

            //2. 更改软件状态
            gpio_ws->hold_lock_state = NO_LOCK_STATE;

            gpio_intr_enable(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM);
            ESP_LOGI(TAG, "release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
            break;

        case PM_LOCK_EVENT_ACQUIRE:// to active
            
            ESP_ERROR_CHECK(esp_pm_lock_acquire(gpio_ws->pm_lock));

            //2. 更改软件状态
            gpio_ws->hold_lock_state = HOLD_LOCK_STATE;

            gpio_intr_enable(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM);
            ESP_LOGI(TAG, "acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);
            break;

        default:
            break;
        }
    }
}

void example_wait_gpio_inactive(void)
{
    //init时，先等待IO口变为low level
    if (gpio_get_level(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM) != EXAMPLE_GPIO_WAKEUP_LEVEL) {
        esp_rom_printf("Waiting for GPIO%d to go low...\n", EXAMPLE_GPIO_WAKEUP_SLEEP_NUM);
    }
    while (gpio_get_level(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM) != EXAMPLE_GPIO_WAKEUP_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "gpio is ready");
}

esp_err_t example_register_gpio_wakeup_sleep(gpio_ws_t* arg)
{
    gpio_ws_t* gpio_ws = arg;
    
    /* Initialize GPIO */
    gpio_config_t config = {
        .pin_bit_mask = BIT64(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = EXAMPLE_USE_INTERNAL_PU_PD ? (EXAMPLE_GPIO_WAKEUP_LEVEL ? true : false) : false,
        .pull_down_en = EXAMPLE_USE_INTERNAL_PU_PD ? (EXAMPLE_GPIO_WAKEUP_LEVEL ? false : true) : false
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "Initialize GPIO%d failed", EXAMPLE_GPIO_WAKEUP_SLEEP_NUM);

    /* Make sure the GPIO is inactive and it won't trigger wakeup immediately */
    example_wait_gpio_inactive();

    //create a queue to handle gpio event from isr（中断服务程序）
    // item len的大小需要评估，频繁切换的时候，要预留  需要测试板
    // item size大小后面可能需要修改，放到结构体中，方便后期修改
    gpio_ws->evt_queue = xQueueCreate(EXAMPLE_EVENT_QUEUE_LEN, sizeof(msg_t));
    if((gpio_ws->evt_queue) == NULL) {
        ESP_LOGE(TAG, "create event queue failed");
        return ESP_FAIL;
    }

    //start gpio task  创建一个task，并开始运行
    //stack导出去 可配置，如果用户在task中malloc stack，那么2048就不够用了
    //task的优先级：有多种task时，优先级也会有影响运行效果，一般设置成10
    if(1 != xTaskCreate(example_event_task, "example_event_task", EXAMPLE_EVENT_TASK_STACK_SIZE, gpio_ws, EXAMPLE_EVENT_TASK_PRIORITY, gpio_ws->event_task)) {
        ESP_LOGE(TAG, "create event task failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "gpio task is ready");

    //enable gpio wake up
    ESP_RETURN_ON_ERROR(esp_sleep_enable_gpio_wakeup(), TAG, "Enable GPIO%d wakeup failed", EXAMPLE_GPIO_WAKEUP_SLEEP_NUM);
    ESP_LOGI(TAG, "enable gpio wakeup");

    //先设置low level唤醒，再设置gpio intr type（由于硬件缺陷，导致必须后设置gpio intr type）
    gpio_wakeup_enable(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM, EXAMPLE_GPIO_WAKEUP_LEVEL == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL);
    gpio_set_intr_type(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM, TO_SLEEP_INTR_TYPE);

    //install gpio isr service
    ESP_RETURN_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), TAG, "install isr failed");

    //hook isr handler for specific gpio pin 
    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(EXAMPLE_GPIO_WAKEUP_SLEEP_NUM, gpio_isr_handler, gpio_ws), TAG, "add isr for specific gpio pin failed");
    ESP_LOGI(TAG, "gpio interrupt is ready");

    return ESP_OK;
}