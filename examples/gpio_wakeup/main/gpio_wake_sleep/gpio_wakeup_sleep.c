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
#include "driver/rtc_io.h"
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include "gpio_wakeup_example.h"

/* IO pin for wake-up and sleep */
#define DEFAULT_GPIO_WAKEUP_SLEEP_NUM   	  (CONFIG_EXAMPLE_GPIO_WAKEUP_SLEEP_NUM)

/* Set wakeup mode, 1 for wakeup with pulse, 0 for wakeup with level */
#if CONFIG_EXAMPLE_USE_PULSE_WAKEUP
#define DEFAULT_USE_PULSE_WAKEUP              (1)
#else
#define DEFAULT_USE_PULSE_WAKEUP              (0)
#endif

/* Setting the level for wakeup, 1 for wakeup with high level, 0 for wakeup with low level */
#if CONFIG_EXAMPLE_GPIO_WAKEUP_HIGH_LEVEL
#define DEFAULT_GPIO_WAKEUP_LEVEL       	  (1)
#else
#define DEFAULT_GPIO_WAKEUP_LEVEL       	  (0)
#endif

/* Whether to use internal pull up and pull down, 1 means use internal pu pd, 0 means no internal pu pd */
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

/* Types of interrupt for lock acquisition and lock release in level wake-up mode */
#if DEFAULT_GPIO_WAKEUP_LEVEL
#define TO_ACTIVE_INTR_TYPE			  		  (GPIO_INTR_HIGH_LEVEL)
#define TO_SLEEP_INTR_TYPE					  (GPIO_INTR_LOW_LEVEL)
#else
#define TO_ACTIVE_INTR_TYPE			  		  (GPIO_INTR_LOW_LEVEL)
#define TO_SLEEP_INTR_TYPE					  (GPIO_INTR_HIGH_LEVEL)
#endif

/* Whether the digital peripheral is powered down, 1 for digital peripheral power down, 0 for no power down */
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
#define DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP    (1)
#else
#define DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP    (0)
#endif

#define ESP_INTR_FLAG_DEFAULT   (0)

/* Types of events that task can handle */
typedef enum {
    LEVEL_WAKEUP_GPIO_INIT_EVENT = 0,
    PULSE_WAKEUP_GPIO_INIT_EVENT,
    LEVEL_WAKEUP_RTC_GPIO_INIT_EVENT,
    PULSE_WAKEUP_RTC_GPIO_INIT_EVENT,
    INSTALL_GPIO_ISR_EVENT,
    CHECK_LEVEL_EVENT,
	ACQUIRE_PM_LOCK_EVENT,
	RELEASE_PM_LOCK_EVENT
} task_event_t;

/* message passed to task by interrupt function */
typedef struct {
    task_event_t event;
} msg_t;


static const char *TAG = "gpio_wakeup_sleep";

/* Wait for the io level to reach the specified state */
void example_wait_gpio_inactive(gpio_num_t gpio, bool level)
{
    if (gpio_get_level(gpio) != level) {
        esp_rom_printf("Waiting for [GPIO%d] to go %s...\n", gpio, level ? "high" : "low");
    }
    while (gpio_get_level(gpio) != level) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "gpio is ready");
}

/* interrupt function have three tasks：
1. disable interrupt
2. modify interrupt trigger type if it is a level wake-up method
3. notify task to work */
static void IRAM_ATTR gpio_isr_handler(void* args)
{
    gpio_ws_t* object = (gpio_ws_t*)args; 
    msg_t send_task_msg;

    // The first step is to disable intr
    // Documentation describes why: why diable intr, why modify intr type
    ESP_ERROR_CHECK( gpio_intr_disable(object->gpio) );
#if !DEFAULT_USE_PULSE_WAKEUP
    // If it's level wakeup mode, there's also changing the interrupt trigger method
    ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, 
                object->hold_lock_state == HOLD_LOCK_STATE ? TO_ACTIVE_INTR_TYPE : TO_SLEEP_INTR_TYPE) );
#endif

    // Send event to task
#if !DEFAULT_USE_PULSE_WAKEUP
    // Direct acquire lock or release lock in level wakeup mode
    send_task_msg.event = (object->hold_lock_state == HOLD_LOCK_STATE) ? RELEASE_PM_LOCK_EVENT : ACQUIRE_PM_LOCK_EVENT;
#else
    // In pulse mode, the pulse is detected first
    send_task_msg.event = CHECK_LEVEL_EVENT;
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

    /* When the task first starts running, it sends a gpio init event to itself to complete initialization */
    // If the digital peripheral is power down and the soc supports the rtcio wakeup function, then initialize the rtcio
#if DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
#if DEFAULT_USE_PULSE_WAKEUP
    send_msg.event = PULSE_WAKEUP_RTC_GPIO_INIT_EVENT;
#else
    send_msg.event = LEVEL_WAKEUP_RTC_GPIO_INIT_EVENT;
#endif
    // If the digital peripheral not power down or the soc not support rtcio wakeup, then initialize the gpio
#else
#if DEFAULT_USE_PULSE_WAKEUP
    send_msg.event = PULSE_WAKEUP_GPIO_INIT_EVENT;
#else
    send_msg.event = LEVEL_WAKEUP_GPIO_INIT_EVENT;
#endif
#endif
    if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
        ESP_LOGE(TAG, "%s:%d %s::%s send gpio init event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return;
    }

    while (xQueueReceive(object->evt_queue, (void *)&recv_isr_msg, portMAX_DELAY)) {
        switch (recv_isr_msg.event)
        {
        // Initialization in level wake-up mode
        case LEVEL_WAKEUP_GPIO_INIT_EVENT:
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

            /* Set level wakeup first, then set gpio intr type */
            // (due to a hardware defect that makes it necessary to DEFAULT_GPIO_WAKEUP_SLEEP_NUM gpio intr type later)
            // Digital peripheral powers down and does not support rtcio wakeup, then enable ext1
#if DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && (!SOC_RTCIO_INPUT_OUTPUT_SUPPORTED)
            ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(BIT64(object->gpio), DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    ESP_EXT1_WAKEUP_ANY_LOW : ESP_EXT1_WAKEUP_ANY_HIGH) );
#else
            // If the digital peripheral is not powered down, then enable gpio wakeup
            ESP_ERROR_CHECK( gpio_wakeup_enable(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
#endif
            // Set gpio interrupt type
            ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, TO_SLEEP_INTR_TYPE) );
            ESP_LOGI(TAG, "%d: LEVEL_WAKEUP_GPIO_INIT_EVENT finished", __LINE__);

            // Then the final step of the initialization process is completed: install gpio isr
            send_msg.event = INSTALL_GPIO_ISR_EVENT;
            if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
                ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                return;
            }
            break;

        // Initialization in pulse wake-up mode
        case PULSE_WAKEUP_GPIO_INIT_EVENT:
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

            /* Set level wakeup first, then set gpio intr type */
            // Digital peripheral powers down and does not support rtcio wakeup, then enable ext1
#if DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP && CONFIG_IDF_TARGET_ESP32H2
            ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(BIT64(object->gpio), DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    ESP_EXT1_WAKEUP_ANY_LOW : ESP_EXT1_WAKEUP_ANY_HIGH) );
#else
            // If the digital peripheral is not powered down, then enable gpio wakeup
            ESP_ERROR_CHECK( gpio_wakeup_enable(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
#endif
            // Set gpio interrupt type
            ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
            ESP_LOGI(TAG, "%d: PULSE_WAKEUP_GPIO_INIT_EVENT finished", __LINE__);

            // Then the final step of the initialization process is completed: install gpio isr
            send_msg.event = INSTALL_GPIO_ISR_EVENT;
            if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
                ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                return;
            }
            break;
/* Only soc that support the rtcio input output function can initialize rtcio, 
    otherwise it will result in compilation errors, eg: esp32-h2 */
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
        case LEVEL_WAKEUP_RTC_GPIO_INIT_EVENT:
            /* Initialize GPIO */
            gpio_config_t rtc_gpio_config_level = {
                .pin_bit_mask = BIT64(object->gpio),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? (DEFAULT_GPIO_WAKEUP_LEVEL ? true : false) : false,
                .pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? (DEFAULT_GPIO_WAKEUP_LEVEL ? false : true) : false
            };
            ESP_ERROR_CHECK( gpio_config(&rtc_gpio_config_level) );

            // Make sure the GPIO is inactive and it won't trigger wakeup immediately
            example_wait_gpio_inactive(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL);

            /* Initialize LPIO */
            ESP_ERROR_CHECK( rtc_gpio_init(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_set_direction(object->gpio, RTC_GPIO_MODE_INPUT_ONLY) );
#if !DEFAULT_USE_INTERNAL_PU_PD
            ESP_ERROR_CHECK( rtc_gpio_pullup_dis(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(object->gpio) );
#elif DEFAULT_GPIO_WAKEUP_LEVEL
            ESP_ERROR_CHECK( rtc_gpio_pullup_en(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(object->gpio) );
#else
            ESP_ERROR_CHECK( rtc_gpio_pullup_dis(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_pulldown_en(object->gpio) );
#endif
            /* Enable wakeup from LPIO wakeup */
            ESP_ERROR_CHECK( rtc_gpio_wakeup_enable(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );

            // Set gpio interrupt type
            ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, TO_SLEEP_INTR_TYPE) );
            ESP_LOGI(TAG, "%d: LEVEL_WAKEUP_RTC_GPIO_INIT_EVENT finished", __LINE__);

            // Then the final step of the initialization process is completed: install gpio isr
            send_msg.event = INSTALL_GPIO_ISR_EVENT;
            if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
                ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                return;
            }
            break;

        case PULSE_WAKEUP_RTC_GPIO_INIT_EVENT:
            /* Initialize GPIO */
            gpio_config_t rtc_gpio_config_pulse = {
                .pin_bit_mask = BIT64(object->gpio),
                .mode = GPIO_MODE_INPUT,
                .pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!DEFAULT_GPIO_WAKEUP_LEVEL) ? true : false ) : false,
                .pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!DEFAULT_GPIO_WAKEUP_LEVEL) ? false : true ) : false
            };
            ESP_ERROR_CHECK( gpio_config(&rtc_gpio_config_pulse) );

            // Make sure the GPIO is inactive and it won't trigger wakeup immediately
            example_wait_gpio_inactive( object->gpio, (!DEFAULT_GPIO_WAKEUP_LEVEL) );

            /* Initialize LPIO */
            ESP_ERROR_CHECK( rtc_gpio_init(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_set_direction(object->gpio, RTC_GPIO_MODE_INPUT_ONLY) );
#if !DEFAULT_USE_INTERNAL_PU_PD
            ESP_ERROR_CHECK( rtc_gpio_pullup_dis(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(object->gpio) );
#elif !DEFAULT_GPIO_WAKEUP_LEVEL
            ESP_ERROR_CHECK( rtc_gpio_pullup_en(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(object->gpio) );
#else
            ESP_ERROR_CHECK( rtc_gpio_pullup_dis(object->gpio) );
            ESP_ERROR_CHECK( rtc_gpio_pulldown_en(object->gpio) );
#endif
            /* Enable wake up from LPIO wakeup */
            ESP_ERROR_CHECK( rtc_gpio_wakeup_enable(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );

            // Set gpio interrupt type
            ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, DEFAULT_GPIO_WAKEUP_LEVEL == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
            ESP_LOGI(TAG, "%d: PULSE_WAKEUP_RTC_GPIO_INIT_EVENT finished", __LINE__);

            // Then the final step of the initialization process is completed: install gpio isr
            send_msg.event = INSTALL_GPIO_ISR_EVENT;
            if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
                ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                return;
            }
            break;
#endif
        case INSTALL_GPIO_ISR_EVENT:
            // install gpio isr service
            ESP_ERROR_CHECK( gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT) );
            // hook isr handler for specific gpio pin
            ESP_ERROR_CHECK( gpio_isr_handler_add(object->gpio, gpio_isr_handler, (void *)object) );
            // enable gpio intr
            ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
            ESP_LOGI(TAG, "gpio interrupt is ready");

            // enable gpio wakeup
            ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
            ESP_LOGI(TAG, "%d: Enable io wakeup!", __LINE__);
            break;

        // check pulse
        case CHECK_LEVEL_EVENT:
            if (gpio_get_level(object->gpio) == (!DEFAULT_GPIO_WAKEUP_LEVEL)) {
                send_msg.event =  (object->hold_lock_state == HOLD_LOCK_STATE) ? RELEASE_PM_LOCK_EVENT : ACQUIRE_PM_LOCK_EVENT;
            } else {
                send_msg.event = CHECK_LEVEL_EVENT;
            }

            if (pdFALSE == xQueueSend(object->evt_queue, (void *)&send_msg, 0)) {
                ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                return;
            }
            break;

        // to sleep
        case RELEASE_PM_LOCK_EVENT:
            ESP_ERROR_CHECK( esp_pm_lock_release(object->pm_lock) );

            // Change Software Status
            object->hold_lock_state = NO_LOCK_STATE;

            // Enable gpio intr
            ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
            ESP_LOGI(TAG, "release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
            break;

        // to active
        case ACQUIRE_PM_LOCK_EVENT:
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