/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "soc/uart_pins.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "uart_wakeup_sleep.h"

/* Set the UART used for the UART wakeup */
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

#define EXAMPLE_READ_BUF_SIZE   1024
#define EXAMPLE_UART_BUF_SIZE   (EXAMPLE_READ_BUF_SIZE * 2)

static const char *TAG = "uart_wakeup_sleep";

/* to_sleep_timer callback */
static void to_sleep_timer_callback(void* args)
{
    uart_wakeup_object_t* object = (uart_wakeup_object_t*)args;
    // Determine whether the current uart task holds a lock to prevent repeated release lock
    if(object->hold_lock_state == HOLD_LOCK_STATE) {
        // Release lock
        ESP_ERROR_CHECK( esp_pm_lock_release(object->pm_lock) );

        // Change Software Status
        object->hold_lock_state = NO_LOCK_STATE;

        ESP_LOGI(TAG, "Release %s lock finished, system may sleep", PM_LOCK_TYPE_TO_STRING);
    } else {
        // The current uart task does not hold a lock
        ESP_LOGW(TAG, "The lock has been released");
    }
}

/* Tasks for handling events */
static void uart_event_task(void *args)
{
    uart_wakeup_object_t* object = (uart_wakeup_object_t*)args;
    uart_event_t event;
    if (object->evt_queue == NULL) {
        ESP_LOGE(TAG, "%s:%d %s::%s uart event queue is NULL!", __FILE__, __LINE__, pcTaskGetName(NULL), __func__);
        abort();
    }
    // esp_rom_printf("uart_event_task: test\n");
    uint8_t* dtmp = (uint8_t*) malloc(EXAMPLE_READ_BUF_SIZE);

    // Waiting for UART event
    while(xQueueReceive(object->evt_queue, (void * )&event, (TickType_t)portMAX_DELAY)) {
        switch(event.type) {
            // Event of uart data
            case UART_DATA:
                // restart to_sleep timer
                // Note: when the timer is not running, restart the timer will result in an error.
                if( esp_timer_is_active(object->to_sleep_timer) ) {
                    // When the to_sleep time is running, restart it.
                    ESP_ERROR_CHECK( esp_timer_restart(object->to_sleep_timer, 10000000) );
                } else {
                    // When the timer is not running, turn on the timer.
                    ESP_ERROR_CHECK( esp_timer_start_once(object->to_sleep_timer, 10000000) );
                }

                // receive uart data
                ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                uart_read_bytes(object->uart_num, dtmp, event.size, portMAX_DELAY);

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
                xQueueReset(object->evt_queue);
                break;
            // Event of UART ring buffer full
            case UART_BUFFER_FULL:
                ESP_LOGI(TAG, "ring buffer full");
                // If buffer full happened, you should consider encreasing your buffer size
                // As an example, we directly flush the rx buffer here in order to read more data.
                uart_flush_input(object->uart_num);
                xQueueReset(object->evt_queue);
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
                // Determine if the current uart task does not hold a lock
                if(object->hold_lock_state == NO_LOCK_STATE) {
                    // Acquire lock
                    ESP_ERROR_CHECK( esp_pm_lock_acquire(object->pm_lock) );

                    // Change Software Status
                    object->hold_lock_state = HOLD_LOCK_STATE;
                    ESP_LOGI(TAG, "Acquire %s lock finished, can to do something...", PM_LOCK_TYPE_TO_STRING);

                    // Start to_sleep timer
                    // Note: When the timer is running, start directly, it will report an error.
                    if( esp_timer_is_active(object->to_sleep_timer) ) {
                        // When the to_sleep time is running, restart it.
                        ESP_ERROR_CHECK( esp_timer_restart(object->to_sleep_timer, 5000000) );
                    } else {
                        // When the timer is not running, turn on the timer.
                        ESP_ERROR_CHECK( esp_timer_start_once(object->to_sleep_timer, 5000000) );
                    }
                } else {
                    // The current uart task already holds a lock
                    ESP_LOGW(TAG, "The lock has been held");
                }
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

/* Initialize to sleep timer */
static esp_err_t sleep_timer_config(uart_wakeup_object_t* args)
{
    uart_wakeup_object_t* object = args;
    /* Initialize sleep timer */
    const esp_timer_create_args_t to_sleep_timer_args = {
            .callback = &to_sleep_timer_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) object,
            .name = "to_sleep_timer"
    };
    ESP_RETURN_ON_ERROR( esp_timer_create(&to_sleep_timer_args, &(object->to_sleep_timer)), 
                            TAG, "Configure sleep timer failed!" );

    // Once the timer is configured, it should be turn on
    ESP_RETURN_ON_ERROR( esp_timer_start_once(object->to_sleep_timer, 5000000), TAG, "Failed to turn on sleep timer");
    return ESP_OK;
}

/* Initialize UART */
static esp_err_t uart_initialization(uart_wakeup_object_t* args)
{
    uart_wakeup_object_t* object = args;
    /* Set Communication Parameters */
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
    ESP_RETURN_ON_ERROR( uart_param_config(object->uart_num, &uart_cfg), TAG, "Configure uart param failed" );

    /* Set Communication Pins */
    ESP_RETURN_ON_ERROR( uart_set_pin(object->uart_num, object->tx_io_num, object->rx_io_num, object->rts_io_num, object->cts_io_num),
                        TAG, "Configure uart gpio pins failed" );

    /* Install UART driver, and get the queue */
    ESP_RETURN_ON_ERROR( uart_driver_install(object->uart_num, EXAMPLE_UART_BUF_SIZE, EXAMPLE_UART_BUF_SIZE, DEFAULT_EVENT_QUEUE_LEN, &(object->evt_queue), 0),
                        TAG, "Install uart failed" );
    if (object->uart_num == CONFIG_ESP_CONSOLE_UART_NUM) {
        /* temp fix for uart garbled output, can be removed when IDF-5683 done */
        ESP_RETURN_ON_ERROR( uart_wait_tx_idle_polling(object->uart_num), TAG, "Wait uart tx done failed" );
    }

    return ESP_OK;
}

/* Configure UART wakeup */
static esp_err_t uart_wakeup_config(uart_wakeup_object_t* args)
{
    uart_wakeup_object_t* object = args;

    /* UART will wakeup the chip up from light sleep if the edges that RX pin received has reached the threshold
     * Besides, the Rx pin need extra configuration to enable it can work during light sleep */

    // Configure the rx pin to work during light sleep
    ESP_RETURN_ON_ERROR( gpio_sleep_set_direction(object->rx_io_num, GPIO_MODE_INPUT), TAG, "Set uart sleep gpio failed" );
    // Set internal pull-up to prevent pin from floating during light sleep.
    ESP_RETURN_ON_ERROR( gpio_sleep_set_pull_mode(object->rx_io_num, GPIO_PULLUP_ONLY), TAG, "Set uart sleep gpio failed" );

    // Set the number of rising edges used for uart wakeup.
    ESP_RETURN_ON_ERROR( uart_set_wakeup_threshold(object->uart_num, object->wakeup_threshold),
                            TAG, "Set uart wakeup threshold failed" );

    // Enable uart wakeup: Only uart0 and uart1 support to be configured as wakeup source
    ESP_RETURN_ON_ERROR( esp_sleep_enable_uart_wakeup(object->uart_num),
                            TAG, "Configure uart as wakeup source failed" );

    return ESP_OK;
}

/* Registered UART wake-up function */
esp_err_t example_register_uart_wakeup_sleep(uart_wakeup_object_t* args)
{
    uart_wakeup_object_t* object = args;

    /* should first pm config, create lock, and then uart wakeup configuration
    (to avoid the problems caused by first configuring wakeup when the lock has not been created) */

    // auto light sleep and light sleep lock init
    ESP_RETURN_ON_ERROR( example_register_power_config(&(object->pm_lock), &(object->hold_lock_state)), 
                            TAG, "pm lock initialization failed!" );

    /* Set Parameters */
    object->evt_queue = NULL;
    object->uart_num = DEFAULT_UART_NUM;
    object->tx_io_num = DEFAULT_UART_TX_IO_NUM;
    object->rx_io_num = DEFAULT_UART_RX_IO_NUM;
    object->cts_io_num = DEFAULT_UART_CTS_IO_NUM;
    object->rts_io_num = DEFAULT_UART_RTS_IO_NUM;
    object->wakeup_threshold = DEFAULT_UART_WAKEUP_THRESHOLD;

    /* Initialize uart */
    ESP_RETURN_ON_ERROR( uart_initialization(object), TAG, "Initialize uart%d failed", object->uart_num );
    /* Enable wakeup from uart */
    ESP_RETURN_ON_ERROR( uart_wakeup_config(object), TAG, "Configure uart as wakeup source failed" );

    /* Create a task for handling events */
    xTaskCreate(uart_event_task, "uart_event_task", DEFAULT_EVENT_TASK_STACK_SIZE, 
                                    (void *)object, DEFAULT_EVENT_TASK_PRIORITY, &(object->event_task));

    ESP_LOGI(TAG, "uart wakeup source is ready");
    
    /* Start sleep timer */
    ESP_RETURN_ON_ERROR( sleep_timer_config(object), TAG, "Configure sleep timer failed" );
    return ESP_OK;
}