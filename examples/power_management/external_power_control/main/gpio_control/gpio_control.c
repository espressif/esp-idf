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
#define DEFAULT_GPIO_CONTROL_LEVE                       (1)
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
#define TO_ACQUIRE_LOCK_INTR_TYPE                       (GPIO_INTR_HIGH_LEVEL)
#define TO_RELEASE_LOCK_INTR_TYPE                       (GPIO_INTR_LOW_LEVEL)
#else
#define TO_ACQUIRE_LOCK_INTR_TYPE                       (GPIO_INTR_LOW_LEVEL)
#define TO_RELEASE_LOCK_INTR_TYPE                       (GPIO_INTR_HIGH_LEVEL)
#endif

/* Whether the digital peripheral is powered down, 1 for digital peripheral power down, 0 for no power down */
#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
#define DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP    (1)
#else
#define DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP    (0)
#endif

#define ESP_INTR_FLAG_DEFAULT                           (0)

/**
 * @brief  Types of events that task can handle.
 * 
 */
typedef enum {
    GPIO_INIT_EVENT = 0,    /*!< gpio initialization event */
    CHECK_PULSE_EVENT,      /*!< detect pulse event */
	ACQUIRE_PM_LOCK_EVENT,  /*!< acquire lock event */
	RELEASE_PM_LOCK_EVENT   /*!< release lock event */
} example_gpio_event_t;

/**
 * @brief  Message passed to task by interrupt function.
 * 
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
static void IRAM_ATTR gpio_isr_handler(void* args)
{
    gpio_ctrl_context_t* object = (gpio_ctrl_context_t*)args; 
    example_gpio_msg_t send_msg;

    // The first step is to disable intr
    // Documentation describes why: why diable intr, why modify intr type
    ESP_ERROR_CHECK( gpio_intr_disable(object->gpio) );

    if(object->control_mode == GPIO_LEVEL_CONTROL) {
        // If it's level control mode, there's also changing the interrupt trigger method
        ESP_ERROR_CHECK( gpio_set_intr_type(object->gpio, 
                    object->lock_state ? TO_ACQUIRE_LOCK_INTR_TYPE : TO_RELEASE_LOCK_INTR_TYPE) );
        
        // Direct acquire lock or release lock in level control mode
        send_msg.event = object->lock_state ? RELEASE_PM_LOCK_EVENT : ACQUIRE_PM_LOCK_EVENT;
    } else {
        // In pulse mode, the pulse is detected first
        send_msg.event = CHECK_PULSE_EVENT;
    }

    xQueueSendFromISR(object->event_queue, (void *)&send_msg, NULL);
}

/**
 * @brief  GPIO initialization.
 * 
 * Initialize the gpio according to the control mode.
 * 
 * @param gpio_control  Important information to be used for gpio control
 */
static void example_gpio_initialization( gpio_ctrl_context_t* gpio_control )
{
    gpio_config_t gpio_cfg = {
        .pin_bit_mask = BIT64(gpio_control->gpio),
        .mode = GPIO_MODE_INPUT
    };

    switch (gpio_control->control_mode) {
        case GPIO_LEVEL_CONTROL:
            gpio_cfg.pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? (gpio_control->trigger_ctrl_level ? true : false) : false;
            gpio_cfg.pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? (gpio_control->trigger_ctrl_level ? false : true) : false;
            break;

        case GPIO_PULSE_CONTROL:
            gpio_cfg.pull_up_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!gpio_control->trigger_ctrl_level) ? true : false ) : false;
            gpio_cfg.pull_down_en = DEFAULT_USE_INTERNAL_PU_PD ? ( (!gpio_control->trigger_ctrl_level) ? false : true ) : false;
            break;

        default:
            break;
    }

    ESP_ERROR_CHECK( gpio_config(&gpio_cfg) );
}

/**
 * @brief  Wait for gpio state to stabilize.
 * 
 * @param gpio_control  Important information to be used for gpio control
 */
static void example_wait_gpio_inactive( gpio_ctrl_context_t* gpio_control )
{
    bool wait_level = (gpio_control->control_mode == GPIO_LEVEL_CONTROL) ? gpio_control->trigger_ctrl_level 
                            : !(gpio_control->trigger_ctrl_level);

    if (gpio_get_level(gpio_control->gpio) != wait_level) {
        esp_rom_printf("Waiting for [GPIO%d] to go %s...\n", gpio_control->gpio, wait_level ? "high" : "low");
    }
    while (gpio_get_level(gpio_control->gpio) != wait_level) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    ESP_LOGI(TAG, "gpio is ready");
}

/**
 * @brief  Enable rtc gpio wakeup. 
 * 
 * First initialize rtc gpio according to the control mode, then enable rtc gpio wakeup.
 * 
 * @param gpio_control  Important information to be used for gpio control
 */
static void example_rtc_gpio_wakeup_enable( gpio_ctrl_context_t* gpio_control )
{
    /* Initialize LPIO */
    ESP_ERROR_CHECK( rtc_gpio_init(gpio_control->gpio) );
    ESP_ERROR_CHECK( rtc_gpio_set_direction(gpio_control->gpio, RTC_GPIO_MODE_INPUT_ONLY) );
#if !DEFAULT_USE_INTERNAL_PU_PD
    ESP_ERROR_CHECK( rtc_gpio_pullup_dis(gpio_control->gpio) );
    ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(gpio_control->gpio) );
#else
    switch (gpio_control->control_mode) {
        case GPIO_LEVEL_CONTROL:
            if( gpio_control->trigger_ctrl_level ) {
                ESP_ERROR_CHECK( rtc_gpio_pullup_en(gpio_control->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(gpio_control->gpio) );
            } else {
                ESP_ERROR_CHECK( rtc_gpio_pullup_dis(gpio_control->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_en(gpio_control->gpio) );
            }
            break;

        case GPIO_PULSE_CONTROL:
            if( gpio_control->trigger_ctrl_level ) {
                ESP_ERROR_CHECK( rtc_gpio_pullup_dis(gpio_control->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_en(gpio_control->gpio) );
            } else {
                ESP_ERROR_CHECK( rtc_gpio_pullup_en(gpio_control->gpio) );
                ESP_ERROR_CHECK( rtc_gpio_pulldown_dis(gpio_control->gpio) );
            }
            break;

        default:
            break;
    }
#endif

    /* Enable wake up from LPIO wakeup */
    ESP_ERROR_CHECK( rtc_gpio_wakeup_enable(gpio_control->gpio, gpio_control->trigger_ctrl_level == 0 ? 
                            GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
    ESP_LOGI(TAG, "%d: Enable rtc gpio wakeup!", __LINE__);
}

/**
 * @brief  Enable wakeup.
 * 
 * Enable rtc gpio wakeup if the digital peripheral is powered down 
 * and the target supports rtc input and output; enable ext1 wakeup 
 * if the target does not support rtc inputs and outputs. If the 
 * digital peripheral is not powered down, enable gpio wakeup.
 * 
 * @param gpio_control  Important information to be used for gpio control
 */
static void example_wakeup_enable( gpio_ctrl_context_t* gpio_control )
{
#if DEFAULT_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
    example_rtc_gpio_wakeup_enable( gpio_control );
#else
    ESP_ERROR_CHECK( esp_sleep_enable_ext1_wakeup(BIT64(gpio_control->gpio), gpio_control->trigger_ctrl_level == 0 ? 
                                    ESP_EXT1_WAKEUP_ANY_LOW : ESP_EXT1_WAKEUP_ANY_HIGH) );
#endif
#else
    ESP_ERROR_CHECK( gpio_wakeup_enable(gpio_control->gpio, gpio_control->trigger_ctrl_level == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
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
 * @param gpio_control  Important information to be used for gpio control
 */
static void example_gpio_set_intr_type( gpio_ctrl_context_t* gpio_control )
{
    // Set gpio interrupt type
    switch (gpio_control->control_mode) {
        case GPIO_LEVEL_CONTROL:
            ESP_ERROR_CHECK( gpio_set_intr_type(gpio_control->gpio, TO_RELEASE_LOCK_INTR_TYPE) );
            break;

        case GPIO_PULSE_CONTROL:
            ESP_ERROR_CHECK( gpio_set_intr_type(gpio_control->gpio, gpio_control->trigger_ctrl_level == 0 ? 
                                    GPIO_INTR_LOW_LEVEL : GPIO_INTR_HIGH_LEVEL) );
            break;

        default:
            break;
    }
}

/**
 * @brief  Enable gpio interrupt.
 * 
 * @param gpio_control  Important information to be used for gpio control
 */
static void example_gpio_intr_enable( gpio_ctrl_context_t* gpio_control )
{
    // install gpio isr service
    ESP_ERROR_CHECK( gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT) );
    // hook isr handler for specific gpio pin
    ESP_ERROR_CHECK( gpio_isr_handler_add(gpio_control->gpio, gpio_isr_handler, (void *)gpio_control) );
    // enable gpio intr
    ESP_ERROR_CHECK( gpio_intr_enable(gpio_control->gpio) );
    ESP_LOGI(TAG, "gpio interrupt is ready");

    // enable gpio wakeup
    ESP_ERROR_CHECK( esp_sleep_enable_gpio_wakeup() );
    ESP_LOGI(TAG, "%d: Enable io wakeup!", __LINE__);
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
static void gpio_event_task(void* args)
{
    gpio_ctrl_context_t* object = (gpio_ctrl_context_t*)args; 
    example_gpio_msg_t recv_msg, send_msg;

    /* create a queue to handle event from isr */ 
    object->event_queue = xQueueCreate(DEFAULT_EVENT_QUEUE_LEN, sizeof(example_gpio_msg_t));
    if(!(object->event_queue)) {
        ESP_LOGE(TAG, "%s:%d %s::%s create gpio event queue failed!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return;
    }

    /* When the task first starts running, it sends a gpio init event to itself to complete initialization */
    send_msg.event = GPIO_INIT_EVENT;
    if (pdFALSE == xQueueSend(object->event_queue, (void *)&send_msg, 0)) {
        ESP_LOGE(TAG, "%s:%d %s::%s send gpio init event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return;
    }

    while (xQueueReceive(object->event_queue, (void *)&recv_msg, portMAX_DELAY)) {
        switch (recv_msg.event) {
            // Initialization
            case GPIO_INIT_EVENT:
                // 1.initialize gpio
                example_gpio_initialization( object );

                // 2.wait for gpio state to stabilize 
                example_wait_gpio_inactive( object );

                // 3.enable wakeup
                example_wakeup_enable( object );

                // 4.set gpio interrupt
                example_gpio_set_intr_type( object );

                // 5.gpio interrupt enable
                example_gpio_intr_enable( object );
                break;

            // check pulse
            case CHECK_PULSE_EVENT:
                if (gpio_get_level(object->gpio) == !object->trigger_ctrl_level) {
                    send_msg.event =  object->lock_state ? RELEASE_PM_LOCK_EVENT : ACQUIRE_PM_LOCK_EVENT;
                } else {
                    send_msg.event = CHECK_PULSE_EVENT;
                }

                if (pdFALSE == xQueueSend(object->event_queue, (void *)&send_msg, 0)) {
                    ESP_LOGE(TAG, "%s:%d %s::%s send event failed!",__FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                    return;
                }
                break;

            // to release power management lock
            case RELEASE_PM_LOCK_EVENT:
                ESP_ERROR_CHECK( esp_pm_lock_release(object->pm_lock) );

                // Change Software Status
                object->lock_state = RELEASED_LOCK;

                // Enable gpio intr
                ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
                ESP_LOGI(TAG, "Release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
                break;

            // to acquire power management lock
            case ACQUIRE_PM_LOCK_EVENT:
                ESP_ERROR_CHECK( esp_pm_lock_acquire(object->pm_lock) );

                // Change Software Status
                object->lock_state = ACQUIRED_LOCK;

                // Enable gpio intr
                ESP_ERROR_CHECK( gpio_intr_enable(object->gpio) );
                ESP_LOGI(TAG, "Acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);
                break;

            // Other event handlers can be added

            default:
                break;
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
 * @param gpio_control  Important information to be used for gpio control
 * 
 * @return
 *  - ESP_OK on success
 */
esp_err_t example_register_gpio_control(gpio_ctrl_context_t* gpio_control)
{
    // power management config
    ESP_RETURN_ON_ERROR( example_register_power_config(&(gpio_control->pm_lock), &(gpio_control->lock_state)), 
                            TAG, "PM lock initialization failed!");
    
    gpio_control->gpio = DEFAULT_GPIO_CONTROL_NUM;
#if DEFAULT_USE_PULSE_CONTROL
    gpio_control->control_mode = GPIO_PULSE_CONTROL;
#else
    gpio_control->control_mode = GPIO_LEVEL_CONTROL;
#endif
    gpio_control->trigger_ctrl_level = DEFAULT_GPIO_CONTROL_LEVEL;

    // Create a task for handling events
    if(pdPASS != xTaskCreate(gpio_event_task, "gpio_event_task", DEFAULT_EVENT_TASK_STACK_SIZE, 
                                    (void *)gpio_control, DEFAULT_EVENT_TASK_PRIORITY, &(gpio_control->event_task))) {
        ESP_LOGE(TAG, "%s:%d %s::%s create event task failed!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "gpio task is ready");

    return ESP_OK;
}