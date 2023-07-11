/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/projdefs.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include "gpio_wakeup_example.h"

/* gpio pin for wake-up and sleep */
#define DEFAULT_GPIO_WAKEUP_SLEEP_NUM   	  (CONFIG_EXAMPLE_GPIO_WAKEUP_SLEEP_NUM)

/* Setting the level for wakeup */
#if CONFIG_EXAMPLE_GPIO_WAKEUP_HIGH_LEVEL
#define DEFAULT_GPIO_WAKEUP_LEVEL       	  (1)
#else
#define DEFAULT_GPIO_WAKEUP_LEVEL       	  (0)
#endif

/* whether to use internal pull up and pull down */
#if CONFIG_EXAMPLE_USE_INTERNAL_PU_PD
#define DEFAULT_USE_INTERNAL_PU_PD			  (1)
#else
#define DEFAULT_USE_INTERNAL_PU_PD			  (0)
#endif

/* Set the length of the event queue */
#define DEFAULT_EVENT_QUEUE_LEN               (CONFIG_EXAMPLE_EVENT_QUEUE_LEN)

/* Set the task's stack size */
#define DEFAULT_EVENT_TASK_STACK_SIZE         (CONFIG_EXAMPLE_EVENT_TASK_STACK_SIZE)

/* Set the priority of a task */
#define DEFAULT_EVENT_TASK_PRIORITY           (CONFIG_EXAMPLE_EVENT_TASK_PRIORITY)


/* acquire lock and release lock interrupt trigger type */
#if DEFAULT_GPIO_WAKEUP_LEVEL
#define TO_ACTIVE_INTR_TYPE			  		  (GPIO_INTR_HIGH_LEVEL)
#define TO_SLEEP_INTR_TYPE					  (GPIO_INTR_LOW_LEVEL)
#else
#define TO_ACTIVE_INTR_TYPE			  		  (GPIO_INTR_LOW_LEVEL)
#define TO_SLEEP_INTR_TYPE					  (GPIO_INTR_HIGH_LEVEL)
#endif


#define ESP_INTR_FLAG_DEFAULT                 (0)

/* Types of events that task can handle */
typedef enum {
	PM_LOCK_EVENT_ACQUIRE = 0,
	PM_LOCK_EVENT_RELEASE = 1
} task_event_t;

/* message passed to task by interrupt function */
typedef struct {
    task_event_t event;
} msg_t;


static const char *TAG = "gpio_wakeup_sleep";

/* interrupt function have two tasks：
1. modify interrupt trigger types
2. notify task to work */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    gpio_ws_t* gpio_ws = (gpio_ws_t*)arg; 
    // 1. The first thing to do is to change the trigger
    // Documentation describes why: why diable intr, why modify intr type
    ESP_ERROR_CHECK(gpio_intr_disable(DEFAULT_GPIO_WAKEUP_SLEEP_NUM));
    ESP_ERROR_CHECK(gpio_set_intr_type(DEFAULT_GPIO_WAKEUP_SLEEP_NUM, 
                gpio_ws->hold_lock_state == HOLD_LOCK_STATE ? TO_ACTIVE_INTR_TYPE : TO_SLEEP_INTR_TYPE));
    
    msg_t send_task_msg = {
        .event = (gpio_ws->hold_lock_state == HOLD_LOCK_STATE) ? PM_LOCK_EVENT_RELEASE : PM_LOCK_EVENT_ACQUIRE
    };
    xQueueSendFromISR(gpio_ws->evt_queue, &send_task_msg, NULL);
    
    // An output pin can be added for easy viewing on the ammeter

    // Is it possible to enable intr here?
}

/* Tasks for handling events */
static void example_event_task(void* arg)
{
    gpio_ws_t* gpio_ws = (gpio_ws_t*)arg; 
    msg_t recv_isr_msg;
    while (xQueueReceive(gpio_ws->evt_queue, &recv_isr_msg, portMAX_DELAY)) {
        switch (recv_isr_msg.event)
        {
        case PM_LOCK_EVENT_RELEASE:// to sleep
            
            ESP_ERROR_CHECK(esp_pm_lock_release(gpio_ws->pm_lock));

            //2. Change Software Status
            gpio_ws->hold_lock_state = NO_LOCK_STATE;

            ESP_ERROR_CHECK(gpio_intr_enable(DEFAULT_GPIO_WAKEUP_SLEEP_NUM));
            ESP_LOGI(TAG, "release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
            break;

        case PM_LOCK_EVENT_ACQUIRE:// to active
            
            ESP_ERROR_CHECK(esp_pm_lock_acquire(gpio_ws->pm_lock));

            //2. Change Software Status
            gpio_ws->hold_lock_state = HOLD_LOCK_STATE;

            ESP_ERROR_CHECK(gpio_intr_enable(DEFAULT_GPIO_WAKEUP_SLEEP_NUM));
            ESP_LOGI(TAG, "acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);
            break;

        // Other event handlers can be added

        default:
            break;
        }
    }
}

void example_wait_gpio_inactive(void)
{
    // When init, wait for the IO port to become low level first.
    if (gpio_get_level(DEFAULT_GPIO_WAKEUP_SLEEP_NUM) != DEFAULT_GPIO_WAKEUP_LEVEL) {
        esp_rom_printf("Waiting for GPIO%d to go low...\n", DEFAULT_GPIO_WAKEUP_SLEEP_NUM);
    }
    while (gpio_get_level(DEFAULT_GPIO_WAKEUP_SLEEP_NUM) != DEFAULT_GPIO_WAKEUP_LEVEL) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "gpio is ready");
}

/* Configure gpio wakeup and gpio interrupts */
esp_err_t example_register_gpio_wakeup_sleep(gpio_ws_t* arg)
{
    gpio_ws_t* gpio_ws = arg;
    
    /* Initialize GPIO */
    gpio_config_t config = {
        .pin_bit_mask = BIT64(DEFAULT_GPIO_WAKEUP_SLEEP_NUM),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? (DEFAULT_GPIO_WAKEUP_LEVEL ? true : false) : false,
        .pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? (DEFAULT_GPIO_WAKEUP_LEVEL ? false : true) : false
    };
    ESP_RETURN_ON_ERROR(gpio_config(&config), TAG, "Initialize GPIO%d failed", DEFAULT_GPIO_WAKEUP_SLEEP_NUM);

    // Make sure the GPIO is inactive and it won't trigger wakeup immediately
    example_wait_gpio_inactive();

    // create a queue to handle event from isr
    gpio_ws->evt_queue = xQueueCreate(DEFAULT_EVENT_QUEUE_LEN, sizeof(msg_t));
    if((gpio_ws->evt_queue) == NULL) {
        ESP_LOGE(TAG, "create event queue failed");
        return ESP_FAIL;
    }

    // Create a task for handling events
    if(pdPASS != xTaskCreate(example_event_task, "example_event_task", DEFAULT_EVENT_TASK_STACK_SIZE, gpio_ws, DEFAULT_EVENT_TASK_PRIORITY, gpio_ws->event_task)) {
        // release queue
        vQueueDelete(gpio_ws->evt_queue);
        gpio_ws->evt_queue = NULL;
        ESP_LOGE(TAG, "create event task failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "gpio task is ready");

    // enable gpio wake-up
    ESP_RETURN_ON_ERROR(esp_sleep_enable_gpio_wakeup(), TAG, "Enable GPIO%d wakeup failed", DEFAULT_GPIO_WAKEUP_SLEEP_NUM);
    ESP_LOGI(TAG, "enable gpio wakeup");

    // Set low level wakeup first, then set gpio intr type 
    // (due to a hardware defect that makes it necessary to DEFAULT_GPIO_WAKEUP_SLEEP_NUM gpio intr type later)
    ESP_RETURN_ON_ERROR(gpio_wakeup_enable(DEFAULT_GPIO_WAKEUP_SLEEP_NUM, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                            GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL), TAG, "set gpio[%d] wake-up level failed", DEFAULT_GPIO_WAKEUP_SLEEP_NUM);
    ESP_RETURN_ON_ERROR(gpio_set_intr_type(DEFAULT_GPIO_WAKEUP_SLEEP_NUM, TO_SLEEP_INTR_TYPE), 
                            TAG, "set gpio[%d] intr failed", DEFAULT_GPIO_WAKEUP_SLEEP_NUM);

    // install gpio isr service
    ESP_RETURN_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), TAG, "install isr failed");

    // hook isr handler for specific gpio pin 
    ESP_RETURN_ON_ERROR(gpio_isr_handler_add(DEFAULT_GPIO_WAKEUP_SLEEP_NUM, gpio_isr_handler, gpio_ws), TAG, "add isr for specific gpio pin failed");
    ESP_LOGI(TAG, "gpio interrupt is ready");

    return ESP_OK;
}