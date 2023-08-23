/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include "esp_check.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/projdefs.h"
#include "gpio_control.h"

/* IO pin for gpio control */
#define DEFAULT_GPIO_CONTROL_NUM                        (CONFIG_EXAMPLE_GPIO_CONTROL_NUM)

/* Set control mode, 1 for control with pulse, 0 for control with level */
#if CONFIG_EXAMPLE_USE_PULSE_CONTROL
#define DEFAULT_USE_PULSE_CONTROL                       (1)
#else
#define DEFAULT_USE_PULSE_CONTROL                       (0)
#endif

/* Setting the level for control, 1 for control with high level, 0 for control with low level */
#if CONFIG_EXAMPLE_GPIO_CONTROL_HIGH_LEVEL
#define DEFAULT_GPIO_CONTROL_LEVEL                      (1)
#else
#define DEFAULT_GPIO_CONTROL_LEVEL                      (0)
#endif

/* Whether to use internal pull up and pull down, 1 means use internal pu pd, 0 means no internal pu pd */
#if CONFIG_EXAMPLE_USE_INTERNAL_PU_PD
#define DEFAULT_USE_INTERNAL_PU_PD                      (1)
#else
#define DEFAULT_USE_INTERNAL_PU_PD                      (0)
#endif

/* Types of interrupt for lock acquisition and lock release in level control mode */
#if DEFAULT_GPIO_CONTROL_LEVEL
#define ACQUIRE_LOCK_INTR_TYPE                          (GPIO_INTR_HIGH_LEVEL)
#define RELEASE_LOCK_INTR_TYPE                          (GPIO_INTR_LOW_LEVEL)
#else
#define ACQUIRE_LOCK_INTR_TYPE                          (GPIO_INTR_LOW_LEVEL)
#define RELEASE_LOCK_INTR_TYPE                          (GPIO_INTR_HIGH_LEVEL)
#endif

#define ESP_INTR_FLAG_DEFAULT                           (0)

/**
 * @brief  Types of event that task can handle.
 */
typedef enum {
    GPIO_EVENT_INIT = 0,        /*!< gpio initialization event */
    GPIO_EVENT_CHECK_PULSE,     /*!< detect pulse event */
    GPIO_EVENT_ACQUIRE_PM_LOCK, /*!< acquire lock event */
    GPIO_EVENT_RELEASE_PM_LOCK  /*!< release lock event */
} example_gpio_event_t;

/**
 * @brief  Message passed to task by interrupt function.
 */
typedef struct {
    example_gpio_event_t event;
} example_gpio_msg_t;

static const char *TAG = "gpio_control";

/**
 * @brief  The gpio interrupt service program.
 *
 * interrupt service program have three tasks：
 * 1. disable interrupt
 * 2. modify interrupt trigger type if it is a level wake-up method
 * 3. notify task to work
 *
 * @param args  The parameter passed in when creating the task, in this case of type 'gpio_ctrl_context_t'.
 */
static void IRAM_ATTR gpio_isr_handler(void *args)
{
    gpio_ctrl_context_t *object = (gpio_ctrl_context_t *)args;
    example_gpio_msg_t message;

    // The first step is to disable intr
    ESP_ERROR_CHECK( gpio_intr_disable(object->gpio) );
    if (object->control_mode == GPIO_LEVEL_CONTROL) {
        // If it's level control mode, there's also changing the interrupt trigger method
        ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio,
                    object->lock_state ? ACQUIRE_LOCK_INTR_TYPE : RELEASE_LOCK_INTR_TYPE) );

        // Direct acquire lock or release lock in level control mode
        message.event = object->lock_state ? GPIO_EVENT_RELEASE_PM_LOCK : GPIO_EVENT_ACQUIRE_PM_LOCK;
    } else {
        // In pulse mode, the pulse is detected first
        message.event = GPIO_EVENT_CHECK_PULSE;
    }
    xQueueSendFromISR(object->event_queue, (void *)&message, NULL);
}

/**
 * @brief  GPIO initialization.
 *
 * Initialize the gpio according to the control mode.
 *
 * @param gpio_context  Important information to be used for gpio control
 */
static void example_gpio_initialization(gpio_ctrl_context_t *gpio_context)
{
    gpio_config_t config = {
        .pin_bit_mask = BIT64(gpio_context->gpio),
        .mode = GPIO_MODE_INPUT
    };
    switch (gpio_context->control_mode) {
        case GPIO_LEVEL_CONTROL:
            config.pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? (gpio_context->control_level ? true : false) : false;
            config.pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? (gpio_context->control_level ? false : true) : false;
            break;

        case GPIO_PULSE_CONTROL:
            config.pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!gpio_context->control_level) ? true : false ) : false;
            config.pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!gpio_context->control_level) ? false : true ) : false;
            break;
        default: break;
    }
    ESP_ERROR_CHECK( gpio_config(&config) );
}

/**
 * @brief  Wait for gpio state to stabilize.
 *
 * @param gpio_context  Important information to be used for gpio control
 */
static void example_wait_gpio_inactive(gpio_ctrl_context_t *gpio_context)
{
    bool wait_level = (gpio_context->control_mode == GPIO_LEVEL_CONTROL) ? gpio_context->control_level
                            : !(gpio_context->control_level);

    if (gpio_get_level(gpio_context->gpio) != wait_level) {
        printf("Waiting for [GPIO%d] to go %s...\n", gpio_context->gpio, wait_level ? "high" : "low");
    }
    while (gpio_get_level(gpio_context->gpio) != wait_level) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
 * @brief  Enable rtc gpio wakeup.
 *
 * First initialize rtc gpio according to the control mode, then enable rtc gpio wakeup.
 *
 * @param gpio_context  Important information to be used for gpio control
 */
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
static void example_rtc_gpio_wakeup_enable(gpio_ctrl_context_t *gpio_context)
{
    // Initialize LPIO
    ESP_ERROR_CHECK( rtc_gpio_init(gpio_context->gpio) );
    ESP_ERROR_CHECK( rtc_gpio_set_direction(gpio_context->gpio, RTC_GPIO_MODE_INPUT_ONLY) );
#if DEFAULT_USE_INTERNAL_PU_PD
    switch (gpio_context->control_mode) {
        case GPIO_LEVEL_CONTROL:
            if (gpio_context->control_level) {
                ESP_ERROR_CHECK( rtc_gpio_pullup_en(gpio_context->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(gpio_context->gpio) );
            } else {
                ESP_ERROR_CHECK( rtc_gpio_pullup_dis(gpio_context->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_en(gpio_context->gpio) );
            }
            break;

        case GPIO_PULSE_CONTROL:
            if (gpio_context->control_level) {
                ESP_ERROR_CHECK( rtc_gpio_pullup_dis(gpio_context->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_en(gpio_context->gpio) );
            } else {
                ESP_ERROR_CHECK( rtc_gpio_pullup_en(gpio_context->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(gpio_context->gpio) );
            }
            break;
        default: break;
    }
#else
    ESP_ERROR_CHECK( rtc_gpio_pullup_dis(gpio_context->gpio) );
    ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(gpio_context->gpio) );
#endif

    // Enable wake up from LPIO wakeup
    ESP_ERROR_CHECK( rtc_gpio_wakeup_enable(gpio_context->gpio, gpio_context->control_level == 0 ?
                            GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
}
#endif

/**
 * @brief  Enable wakeup.
 *
 * Enable rtc gpio wakeup if the digital peripheral is powered down
 * and the target supports rtc input and output; enable ext1 wakeup
 * if the target does not support rtc inputs and outputs. If the
 * digital peripheral is not powered down, enable gpio wakeup.
 *
 * @param gpio_context  Important information to be used for gpio control
 */
static void example_gpio_wakeup_enable(gpio_ctrl_context_t *gpio_context)
{
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
    example_rtc_gpio_wakeup_enable(gpio_context);
#else
    ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(BIT64(gpio_context->gpio), gpio_context->control_level == 0 ?
                                    ESP_EXT1_WAKEUP_ANY_LOW : ESP_EXT1_WAKEUP_ANY_HIGH) );
#endif
#else
    ESP_ERROR_CHECK( gpio_wakeup_enable(gpio_context->gpio, gpio_context->control_level == 0 ? GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
#endif
}

/**
 * @brief  Set the gpio interrupt type.
 *
 * Different interrupt types should be set for different
 * control modes. For level control mode, it should be set
 * to non-control level; for pulse control mode, it should
 * be set to control level.
 *
 * @param gpio_context  Important information to be used for gpio control
 */
static void example_gpio_set_intr_type(gpio_ctrl_context_t *gpio_context)
{
    switch (gpio_context->control_mode) {
        case GPIO_LEVEL_CONTROL:
            ESP_ERROR_CHECK( gpio_set_intr_type(gpio_context->gpio, RELEASE_LOCK_INTR_TYPE) );
            break;

        case GPIO_PULSE_CONTROL:
            ESP_ERROR_CHECK( gpio_set_intr_type(gpio_context->gpio, gpio_context->control_level == 0 ?
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
            break;
        default: break;
    }
}

/**
 * @brief  Enable gpio interrupt.
 *
 * @param gpio_context  Important information to be used for gpio control
 */
static void example_gpio_intr_enable(gpio_ctrl_context_t *gpio_context)
{
    ESP_ERROR_CHECK( gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT) );
    ESP_ERROR_CHECK( gpio_isr_handler_add(gpio_context->gpio, gpio_isr_handler, (void *)gpio_context) );
    ESP_ERROR_CHECK( gpio_intr_enable(gpio_context->gpio) );
}

/**
 * @brief  Task to handle gpio events.
 *
 * The gpio initialization section also uses a publish-subscribe mechanism.
 * After creating an event queue in task, put a gpio initialization event
 * directly into the queue.
 * The gpio initialization is divided into 5 steps:
 * 1. gpio initialization
 * 2. wait for gpio state to stabilize
 * 3. enable wakeup
 * 4. set gpio interrupt
 * 5. gpio interrupt enable
 *
 * @param args  The parameter passed in when creating the task, in this case of type 'gpio_ctrl_context_t'.
 */
static void gpio_event_task(void *args)
{
    gpio_ctrl_context_t *object = (gpio_ctrl_context_t *)args;
    example_gpio_msg_t recv_msg, send_msg;

    object->event_queue = xQueueCreate(DEFAULT_EVENT_QUEUE_LEN, sizeof(example_gpio_msg_t));
    if (!(object->event_queue)) {
        ESP_LOGE(TAG, "create gpio event queue failed!");
        abort();
    }

    /* When the task first starts running, it sends a gpio init event to itself to complete initialization */
    send_msg.event = GPIO_EVENT_INIT;
    if (pdFALSE == xQueueSend(object->event_queue, (void *)&send_msg, 0)) {
        ESP_LOGE(TAG, "send gpio init event failed!");
        abort();
    }

    while (xQueueReceive(object->event_queue, (void *)&recv_msg, (TickType_t)portMAX_DELAY)) {
        switch (recv_msg.event) {
            case GPIO_EVENT_INIT:
                example_gpio_initialization(object);
                example_wait_gpio_inactive(object);
                example_gpio_wakeup_enable(object);
                example_gpio_set_intr_type(object);
                example_gpio_intr_enable(object);
                ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
                ESP_LOGI(TAG, "GPIO control initialization complete");
                break;

            case GPIO_EVENT_CHECK_PULSE:
                if (gpio_get_level(object->gpio) != object->control_level) {
                    send_msg.event = object->lock_state ? GPIO_EVENT_RELEASE_PM_LOCK : GPIO_EVENT_ACQUIRE_PM_LOCK;
                } else {
                    send_msg.event = GPIO_EVENT_CHECK_PULSE;
                }

                if (pdFALSE == xQueueSend(object->event_queue, (void *)&send_msg, 0)) {
                    ESP_LOGE(TAG, "send event failed!");
                }
                break;

            case GPIO_EVENT_RELEASE_PM_LOCK:
                ESP_ERROR_CHECK( esp_pm_lock_release(object->pm_lock) );
                object->lock_state = RELEASED_LOCK;
                ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
                ESP_LOGI(TAG, "Released lock at [%p], system may sleep", object->pm_lock);
                break;

            case GPIO_EVENT_ACQUIRE_PM_LOCK:
                ESP_ERROR_CHECK( esp_pm_lock_acquire(object->pm_lock) );
                object->lock_state = ACQUIRED_LOCK;
                ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
                ESP_LOGI(TAG, "Acquired lock at [%p], system remains active", object->pm_lock);
                break;
            default: break;
        }
    }
}

/**
 * @brief  Register external power control - gpio control.
 *
 * To avoid problems caused by configuring wakeups when
 * locks have not yet been created, power management
 * configure should be done first. Then set some parameters.
 * Finally create a task for handling gpio events.
 *
 * @param gpio_context  Important information to be used for gpio control
 *
 * @return
 *  - ESP_OK on success
 */
esp_err_t example_register_gpio_control(gpio_ctrl_context_t *gpio_context)
{
    ESP_RETURN_ON_ERROR(example_register_power_config(&(gpio_context->pm_lock), &(gpio_context->lock_state)), TAG, "PM lock initialization failed!");

    gpio_context->gpio = DEFAULT_GPIO_CONTROL_NUM;
#if DEFAULT_USE_PULSE_CONTROL
    gpio_context->control_mode = GPIO_PULSE_CONTROL;
#else
    gpio_context->control_mode = GPIO_LEVEL_CONTROL;
#endif
    gpio_context->control_level = DEFAULT_GPIO_CONTROL_LEVEL;

    if(pdPASS != xTaskCreate(gpio_event_task, "gpio_event_task", DEFAULT_EVENT_TASK_STACK_SIZE,
                                    (void *)gpio_context, DEFAULT_EVENT_TASK_PRIORITY, &(gpio_context->task_handle))) {
        ESP_LOGE(TAG, "create event task failed!");
        return ESP_FAIL;
    }
    return ESP_OK;
}
