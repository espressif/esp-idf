/*
 * SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
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

/* Set wakeup mode */
#if CONFIG_EXAMPLE_USE_PULSE_WAKEUP
#define DEFAULT_USE_PULSE_WAKEUP              (1)
#else
#define DEFAULT_USE_PULSE_WAKEUP              (0)
#endif

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
    GPIO_INIT_EVENT_LEVEL_WAKEUP = 0,
    GPIO_INIT_EVENT_PULSE_WAKEUP,
    CHECK_EVENT_LEVEL,
	PM_LOCK_EVENT_ACQUIRE,
	PM_LOCK_EVENT_RELEASE
} task_event_t;

/* message passed to task by interrupt function */
typedef struct {
    task_event_t event;
} msg_t;


static const char *TAG = "gpio_wakeup_sleep";

void example_wait_gpio_inactive(gpio_num_t gpio, bool level)
{
    // When init, wait for the IO port to become given level first.
    if (gpio_get_level(gpio) != level) {
        esp_rom_printf("Waiting for [GPIO%d] to go %s...\n", gpio, level ? "high" : "low");
    }
    while (gpio_get_level(gpio) != level) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "gpio is ready");
}

/* interrupt function have two tasks：
1. modify interrupt trigger types
2. notify task to work */
static void IRAM_ATTR gpio_isr_handler(void* args)
{
    gpio_ws_t* object = (gpio_ws_t*)args; 
    msg_t send_task_msg;

    // 1. The first step is to disable intr
    // Documentation describes why: why diable intr, why modify intr type
    ESP_ERROR_CHECK( gpio_intr_disable(object->gpio) );
#if !DEFAULT_USE_PULSE_WAKEUP
    // If it's a level wakeup, there's also changing the interrupt trigger method
    ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, 
                object->hold_lock_state == HOLD_LOCK_STATE ? TO_ACTIVE_INTR_TYPE : TO_SLEEP_INTR_TYPE) );
#endif

    // 2. Send event to task
#if !DEFAULT_USE_PULSE_WAKEUP
    send_task_msg.event = (object->hold_lock_state == HOLD_LOCK_STATE) ? PM_LOCK_EVENT_RELEASE : PM_LOCK_EVENT_ACQUIRE;
#else
    send_task_msg.event = CHECK_EVENT_LEVEL;
#endif
    xQueueSendFromISR(object->evt_queue, (void *)&send_task_msg, NULL);
    
    // An output pin can be added for easy viewing on the ammeter

    // Is it possible to enable intr here?
}

/* Tasks for handling events */
static void example_event_task(void* args)
{
    gpio_ws_t* object = (gpio_ws_t*)args; 
    msg_t recv_isr_msg, send_msg;

    // create a queue to handle event from isr
    object->evt_queue = xQueueCreate(DEFAULT_EVENT_QUEUE_LEN, sizeof(msg_t));
    if(!(object->evt_queue)) {
        ESP_LOGE(TAG, "%s:%d %s::%s create gpio event queue failed!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return;
    }

    // When the task first starts running, it sends a gpio init event to itself to complete initialization.
#if DEFAULT_USE_PULSE_WAKEUP
    send_msg.event = GPIO_INIT_EVENT_PULSE_WAKEUP;
#else
    send_msg.event = GPIO_INIT_EVENT_LEVEL_WAKEUP;
#endif
    if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
        ESP_LOGE(TAG, "%s:%d %s::%s send gpio init event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return;
    }

    while (xQueueReceive(object->evt_queue, (void *)&recv_isr_msg, portMAX_DELAY)) {
        switch (recv_isr_msg.event)
        {
        case GPIO_INIT_EVENT_LEVEL_WAKEUP:// Initialization in level wake-up mode
            /* Initialize GPIO */
            gpio_config_t gpio_config_level = {
                .pin_bit_mask = BIT64(object->gpio),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? (DEFAULT_GPIO_WAKEUP_LEVEL ? true : false) : false,
                .pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? (DEFAULT_GPIO_WAKEUP_LEVEL ? false : true) : false
            };
            ESP_ERROR_CHECK( gpio_config(&gpio_config_level) );

            // Make sure the GPIO is inactive and it won't trigger wakeup immediately
            example_wait_gpio_inactive(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL);

            // Set level wakeup first, then set gpio intr type 
            // (due to a hardware defect that makes it necessary to DEFAULT_GPIO_WAKEUP_SLEEP_NUM gpio intr type later)
            ESP_ERROR_CHECK( gpio_wakeup_enable(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );

            ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, TO_SLEEP_INTR_TYPE) );
            // install gpio isr service
            ESP_ERROR_CHECK( gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT) );
            // hook isr handler for specific gpio pin 
            ESP_ERROR_CHECK( gpio_isr_handler_add(object->gpio, gpio_isr_handler, (void *)object) );
            // enable gpio intr
            ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
            ESP_LOGI(TAG, "gpio interrupt is ready");

            // enable gpio wakeup
            ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
            ESP_LOGI(TAG, "enable gpio wakeup");
            break;

        case GPIO_INIT_EVENT_PULSE_WAKEUP:// Initialization in pulse wake-up mode
            /* Initialize GPIO */
            gpio_config_t gpio_config_pulse = {
                .pin_bit_mask = BIT64(object->gpio),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!DEFAULT_GPIO_WAKEUP_LEVEL) ? true : false ) : false,
                .pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!DEFAULT_GPIO_WAKEUP_LEVEL) ? false : true ) : false
            };
            ESP_ERROR_CHECK( gpio_config(&gpio_config_pulse) );

            // Make sure the GPIO is inactive and it won't trigger wakeup immediately
            example_wait_gpio_inactive( object->gpio, (!DEFAULT_GPIO_WAKEUP_LEVEL) );

            // Set level wakeup first, then set gpio intr type
            ESP_ERROR_CHECK( gpio_wakeup_enable(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );

            ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
            // install gpio isr service
            ESP_ERROR_CHECK( gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT) );
            // hook isr handler for specific gpio pin
            ESP_ERROR_CHECK( gpio_isr_handler_add(object->gpio, gpio_isr_handler, (void *)object) );
            // enable gpio intr
            ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
            ESP_LOGI(TAG, "gpio interrupt is ready");

            // enable gpio wakeup
            ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
            ESP_LOGI(TAG, "enable gpio wakeup");
            break;

        case CHECK_EVENT_LEVEL:// check pulse
            if (gpio_get_level(object->gpio) == (!DEFAULT_GPIO_WAKEUP_LEVEL)) {
                send_msg.event =  (object->hold_lock_state == HOLD_LOCK_STATE) ? PM_LOCK_EVENT_RELEASE : PM_LOCK_EVENT_ACQUIRE;
            } else {
                send_msg.event = CHECK_EVENT_LEVEL;
            }

            if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
                ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                return;
            }
            break;

        case PM_LOCK_EVENT_RELEASE:// to sleep
            ESP_ERROR_CHECK( esp_pm_lock_release(object->pm_lock) );

            // Change Software Status
            object->hold_lock_state = NO_LOCK_STATE;

            // Enable gpio intr
            ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
            ESP_LOGI(TAG, "release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
            break;

        case PM_LOCK_EVENT_ACQUIRE:// to active
            ESP_ERROR_CHECK( esp_pm_lock_acquire(object->pm_lock) );

            // Change Software Status
            object->hold_lock_state = HOLD_LOCK_STATE;

            // Enable gpio intr
            ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
            ESP_LOGI(TAG, "acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);
            break;

        // Other event handlers can be added

        default:
            break;
        }
    }
}

/* Set the basic parameters and create a task to handle the event */
esp_err_t example_register_gpio_wakeup_sleep(gpio_ws_t* args)
{
    gpio_ws_t* object = args;
    object->gpio = DEFAULT_GPIO_WAKEUP_SLEEP_NUM;

    // Create a task for handling events
    if(pdPASS != xTaskCreate(example_event_task, "example_event_task", DEFAULT_EVENT_TASK_STACK_SIZE, 
                                    (void *)object, DEFAULT_EVENT_TASK_PRIORITY, object->event_task)) {
        ESP_LOGE(TAG, "%s:%d %s::%s create event task failed!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "gpio task is ready");

    return ESP_OK;
}