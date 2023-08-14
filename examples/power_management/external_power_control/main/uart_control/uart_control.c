/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "esp_check.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "soc/uart_pins.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "uart_control.h"

/* Set the UART used for the UART control */
#define DEFAULT_UART_NUM                (CONFIG_EXAMPLE_UART_NUM)

/* Set the tx pin of the uart */
#define DEFAULT_UART_TX_IO_NUM          (CONFIG_EXAMPLE_UART_TX_IO_NUM)

/* Set the rx pin of the uart */
#define DEFAULT_UART_RX_IO_NUM          (CONFIG_EXAMPLE_UART_RX_IO_NUM)

/* Set the cts pin of the uart */
#define DEFAULT_UART_CTS_IO_NUM         (CONFIG_EXAMPLE_UART_CTS_IO_NUM)

/* Set the rts pin of the uart */
#define DEFAULT_UART_RTS_IO_NUM         (CONFIG_EXAMPLE_UART_RTS_IO_NUM)

/* Set the number of rising edges used for uart wakeup */
#define DEFAULT_UART_WAKEUP_THRESHOLD   (CONFIG_EXAMPLE_UART_WAKEUP_THRESHOLD)

/* Set the timeout for the sleep timer */
#define DEFAULT_SLEEP_TIMEOUT_US        (CONFIG_EXAMPLE_SLEEP_TIMEOUT_US)


#define EXAMPLE_READ_BUF_SIZE           1024
#define EXAMPLE_UART_BUF_SIZE           (EXAMPLE_READ_BUF_SIZE * 2)


static const char *TAG = "uart_control";


/**
 * @brief  sleep_timer callback.
 * 
 * The callback's responsibility is to release the lock and record that the lock has been released.
 * 
 * @param args  The parameter passed in when creating the timer, in this case a parameter of type 'uart_ctrl_context_t'.
 */
static void sleep_timer_callback(void* args)
{
    uart_ctrl_context_t* object = (uart_ctrl_context_t*)args;
    // Release lock
    ESP_ERROR_CHECK( esp_pm_lock_release(object->pm_lock) );

    // Record that the power management lock has been released
    object->lock_state = RELEASED_LOCK;

    ESP_LOGI(TAG, "Release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
}

/**
 * @brief  Initializes the sleep timer.
 * 
 * Create a timer and turn on one-time timing.
 * 
 * @param uart_control  Important information to be used for uart control
 * 
 * @return
 *  - ESP_OK on success
 */
static esp_err_t sleep_timer_config(uart_ctrl_context_t* uart_control)
{
    /* Initialize sleep timer */
    const esp_timer_create_args_t sleep_timer_args = {
            .callback = &sleep_timer_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) uart_control,
            .name = "sleep_timer"
    };
    ESP_RETURN_ON_ERROR( esp_timer_create(&sleep_timer_args, &(uart_control->sleep_timer)), 
                            TAG, "Configure sleep timer failed!" );

    // Once the timer is configured, it should be turn on
    ESP_RETURN_ON_ERROR( esp_timer_start_once(uart_control->sleep_timer, uart_control->sleep_timeout_us), TAG, "Failed to turn on sleep timer");
    return ESP_OK;
}

/**
 * @brief  The task that handles the uart event.
 * 
 * Since the example uses a timer to realize the 
 * function of releasing the lock, the task configures 
 * the timer first, and then handles the uart event.
 * 
 * @param args  The parameter passed in when creating the task, in this case of type 'uart_ctrl_context_t'.
 */
static void uart_event_task(void *args)
{
    uart_ctrl_context_t* object = (uart_ctrl_context_t*)args;
    uart_event_t event;

    if (object->event_queue == NULL) {
        ESP_LOGE(TAG, "%s:%d %s::%s uart event queue is NULL!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        abort();
    }

    uint8_t* dtmp = (uint8_t*) malloc(EXAMPLE_READ_BUF_SIZE);

    /* Start sleep timer */
    ESP_ERROR_CHECK( sleep_timer_config(object) );

    /* Waiting for UART event */
    while(xQueueReceive(object->event_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
        switch(event.type) {
            // Event of uart data
            case UART_DATA:
                /* When the power management lock of the uart_control has been released, the uart should pause its work, 
                and then come uart data, which is incorrect data. (See README.md for details) */
                if( !(object->lock_state) ) {
                    ESP_LOGE(TAG, "%s:%d %s::%s uart receives untimely data!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
                    abort();
                }
                // restart sleep timer
                ESP_ERROR_CHECK( esp_timer_restart(object->sleep_timer, object->sleep_timeout_us) );

                // receive uart data
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(object->uart_num, dtmp, event.size, (TickType_t)portMAX_DELAY);

                // Retransmit the received data
                ESP_LOGI(TAG, "[DATA EVT]:");
                uart_write_bytes(object->uart_num, (const char *)dtmp, event.size);
                break;
            // Event of HW FIFO overflow detected
            case UART_FIFO_OVF:
                ESP_LOGI(TAG, "hw fifo overflow");
                // If fifo overflow happened, you should consider adding flow control for your application.
                // The ISR has already reset the rx FIFO,
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(object->uart_num);
                xQueueReset(object->event_queue);
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(object->uart_num);
                xQueueReset(object->event_queue);
                break;
            // Event of UART RX break detected
            case UART_BREAK:
                ESP_LOGI(TAG, "uart rx break");
                break;
            // Event of UART parity check error
            case UART_PARITY_ERR:
                ESP_LOGI(TAG, "uart parity error");
                break;
            // Event of UART frame error
            case UART_FRAME_ERR:
                ESP_LOGI(TAG, "uart frame error");
                break;
            // ESP32 can wakeup by uart but there is no wake up interrupt
#if SOC_UART_SUPPORT_WAKEUP_INT
            // Event of wakeup by UART
            case UART_WAKEUP:
                // Acquire lock
                ESP_ERROR_CHECK( esp_pm_lock_acquire(object->pm_lock) );

                // Change Software Status
                object->lock_state = ACQUIRED_LOCK;
                ESP_LOGI(TAG, "Acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);

                // Start sleep timer
                ESP_ERROR_CHECK( esp_timer_start_once(object->sleep_timer, object->sleep_timeout_us) );
                break;
#endif
            default:
                ESP_LOGI(TAG, "uart event type: %d", event.type);
                break;
        }
    }
    free(dtmp);
    vTaskDelete(NULL);
}

/**
 * @brief  Initialize uart.
 * 
 * Take special care when setting the uart clock source! 
 * Because the DFS function will change the frequency, 
 * which will cause the uart baud rate to be wrong, 
 * resulting in garbled output. 
 * Configure the basic flow of uart:
 * 1.Set communication parameters
 * 2.Set communication pins
 * 3.Install Drivers
 * 
 * @param uart_control  Important information to be used for uart control
 * 
 * @return
 *  - ESP_OK on success
 */
static esp_err_t uart_initialization(uart_ctrl_context_t* uart_control)
{
    /* Set communication parameters */
    uart_config_t uart_cfg = {
        // Set baud rate
        .baud_rate  = CONFIG_ESP_CONSOLE_UART_BAUDRATE,
        // Set data size
        .data_bits  = UART_DATA_8_BITS,
        // Set parity mode
        .parity     = UART_PARITY_DISABLE,
        // Set stop bits
        .stop_bits  = UART_STOP_BITS_1,
        // Set HW flow control mode (cts/rts)
        .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
        // Set clock source
        /* Note: there is a conflict between the uart clock source and the DFS function, 
        so it is very important to choose the right clock source (detailed description in the README.md) */
#if !CONFIG_IDF_TARGET_ESP32S2
        .source_clk = SOC_MOD_CLK_XTAL,
#else
        // s2 uart not support xtal clock source
        .source_clk = SOC_MOD_CLK_REF_TICK,
#endif
    };
    ESP_RETURN_ON_ERROR( uart_param_config(uart_control->uart_num, &uart_cfg), TAG, "Configure uart param failed" );

    /* Set communication pins */
    ESP_RETURN_ON_ERROR( uart_set_pin(uart_control->uart_num, uart_control->tx_io_num, uart_control->rx_io_num, 
                        uart_control->rts_io_num, uart_control->cts_io_num), TAG, "Configure uart gpio pins failed" );

    /* Install uart driver, and get the queue */
    ESP_RETURN_ON_ERROR( uart_driver_install(uart_control->uart_num, EXAMPLE_UART_BUF_SIZE, EXAMPLE_UART_BUF_SIZE, 
                                DEFAULT_EVENT_QUEUE_LEN, &(uart_control->event_queue), 0), TAG, "Install uart failed" );

    return ESP_OK;
}

/**
 * @brief  Configure uart wakeup.
 * 
 * UART will wakeup the chip up from light sleep 
 * if the edges that RX pin received has reached 
 * the threshold. Besides, the Rx pin need extra 
 * configuration to enable it can work during light 
 * sleep (The rx pin should be held high).
 * 
 * @param uart_num              The uart to be configured, typically uart0 or uart1.
 * @param rx_io_num             The rx pin number of uart.
 * @param wakeup_threshold      Threshold for the pos-edge used for wakeup.
 * 
 * @return
 *  - ESP_OK on success
 */
static esp_err_t uart_wakeup_config(int uart_num, int rx_io_num, int wakeup_threshold)
{
    /* UART will wakeup the chip up from light sleep if the edges that RX pin received has reached the threshold
     * Besides, the Rx pin need extra configuration to enable it can work during light sleep */

    // Configure the rx pin to work during light sleep
    ESP_RETURN_ON_ERROR( gpio_sleep_set_direction(rx_io_num, GPIO_MODE_INPUT), TAG, "Set uart sleep gpio failed" );
    // Set internal pull-up to prevent pin from floating during light sleep. The rx pin should be held high.
    ESP_RETURN_ON_ERROR( gpio_sleep_set_pull_mode(rx_io_num, GPIO_PULLUP_ONLY), TAG, "Set uart sleep gpio failed" );

    // Set the number of rising edges used for uart wakeup.
    ESP_RETURN_ON_ERROR( uart_set_wakeup_threshold(uart_num, wakeup_threshold),
                            TAG, "Set uart wakeup threshold failed" );

    // Enable uart wakeup: Only uart0 and uart1 support to be configured as wakeup source
    ESP_RETURN_ON_ERROR( esp_sleep_enable_uart_wakeup(uart_num),
                            TAG, "Configure uart as wakeup source failed" );

    return ESP_OK;
}

/**
 * @brief  Register external power control - uart control.
 * 
 * To avoid problems caused by configuring wakeups when 
 * locks have not yet been created, power management 
 * configure should be done first. Then initialize the 
 * uart according to the set parameters. when the uart 
 * is successfully initialized, then configure the uart 
 * to wake up. Once the initial setup is complete, a task 
 * is created to handle uart events.
 * 
 * @param uart_control  Important information to be used for uart control
 * 
 * @return
 *  - ESP_OK on success
 */
esp_err_t example_register_uart_control(uart_ctrl_context_t* uart_control)
{
    // power management config
    ESP_RETURN_ON_ERROR( example_register_power_config(&(uart_control->pm_lock), &(uart_control->lock_state)), 
                            TAG, "pm lock initialization failed!" );

    /* Set Parameters */
    uart_control->event_queue = NULL;
    uart_control->uart_num = DEFAULT_UART_NUM;
    uart_control->tx_io_num = DEFAULT_UART_TX_IO_NUM;
    uart_control->rx_io_num = DEFAULT_UART_RX_IO_NUM;
    uart_control->cts_io_num = DEFAULT_UART_CTS_IO_NUM;
    uart_control->rts_io_num = DEFAULT_UART_RTS_IO_NUM;
    uart_control->wakeup_threshold = DEFAULT_UART_WAKEUP_THRESHOLD;
    uart_control->sleep_timeout_us = DEFAULT_SLEEP_TIMEOUT_US;

    /* Initialize uart */
    ESP_RETURN_ON_ERROR( uart_initialization(uart_control), TAG, "Initialize uart%d failed", uart_control->uart_num );

    /* Enable wakeup from uart */
    ESP_RETURN_ON_ERROR( uart_wakeup_config(uart_control->uart_num, uart_control->rx_io_num, uart_control->wakeup_threshold), 
                                TAG, "Configure uart as wakeup source failed" );

    /* Create a task for handling events */
    xTaskCreate(uart_event_task, "uart_event_task", DEFAULT_EVENT_TASK_STACK_SIZE, 
                                    (void *)uart_control, DEFAULT_EVENT_TASK_PRIORITY, &(uart_control->event_task));

    ESP_LOGI(TAG, "uart wakeup source is ready");
    return ESP_OK;
}