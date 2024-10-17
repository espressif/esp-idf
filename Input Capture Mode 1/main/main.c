#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/timer.h"

#define BOOT_BUTTON_PIN GPIO_NUM_9 // GPIO9 is the boot button
#define TIMER_DIVIDER 80           // Timer clock divider (80MHz / 80 = 1MHz, or 1 tick = 1 microsecond)
#define TIMER_GROUP TIMER_GROUP_0  // Using hardware timer group 0
#define TIMER_INDEX TIMER_0        // Timer 0

QueueHandle_t gpio_evt_queue = NULL;
uint64_t last_timestamp = 0; // Variable to store the previous interrupt time
// frequncy of the timer
float TIMER_PERIOD = 0;
float freq = 0;

// ISR handler for GPIO interrupt
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    uint64_t current_timestamp;
    timer_get_counter_value(TIMER_GROUP, TIMER_INDEX, &current_timestamp); // Get current timer value (timestamp)

    if( last_timestamp >0) // If there is a previous timestamp
    {
        TIMER_PERIOD = current_timestamp - last_timestamp; 
        freq = 1 /(TIMER_PERIOD/1000000); // Calculate the period of the timer in microseconds
    }
    // Send GPIO number and timestamp to the task queue
    
    uint64_t event_data = ((uint64_t)gpio_num << 32) | current_timestamp;
    xQueueSendFromISR(gpio_evt_queue, &event_data, NULL);
}

// Timer configuration
void init_timer()
{
    timer_config_t config = {
        .divider = TIMER_DIVIDER,            // Set clock divider
        .counter_dir = TIMER_COUNT_UP,       // Counting up
        .counter_en = TIMER_PAUSE,           // Do not start yet
        .alarm_en = TIMER_ALARM_DIS,         // Disable alarm
        .auto_reload = TIMER_AUTORELOAD_DIS, // Disable auto-reload
    };
    timer_init(TIMER_GROUP, TIMER_INDEX, &config);
    timer_start(TIMER_GROUP, TIMER_INDEX); // Start the timer
}

// GPIO setup and interrupt configuration
void init_gpio()
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE,            // Interrupt on both edges (press and release)
        .mode = GPIO_MODE_INPUT,                   // Set as input mode
        .pin_bit_mask = (1ULL << BOOT_BUTTON_PIN), // Select GPIO9 (boot button)
        .pull_down_en = GPIO_PULLDOWN_DISABLE,     // Disable pull-down
        .pull_up_en = GPIO_PULLUP_ENABLE,          // Enable pull-up
    };
    gpio_config(&io_conf);

    // Create a queue to handle GPIO events
    gpio_evt_queue = xQueueCreate(10, sizeof(uint64_t));

    // Install GPIO ISR handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BOOT_BUTTON_PIN, gpio_isr_handler, (void *)BOOT_BUTTON_PIN);
}

// Task to handle GPIO events
void gpio_task(void *arg)
{
    uint64_t event_data;
    while (1)
    {
        if (xQueueReceive(gpio_evt_queue, &event_data, portMAX_DELAY))
        {
            uint32_t gpio_num = (uint32_t)(event_data >> 32);
            uint64_t current_timestamp = event_data & 0xFFFFFFFF;

            if (last_timestamp == 0)
            {
                // If this is the first event, just store the timestamp
                printf("First event captured. Waiting for the next...\n");
            }
            else
            {
                // Calculate the difference between current and last timestamp
                uint64_t time_diff_us = current_timestamp - last_timestamp; // Difference in microseconds
                double time_diff_sec = time_diff_us / 1000000.0;            // Convert to seconds

                printf("GPIO %ld event captured. Time since last event: %.2f seconds\n", gpio_num, time_diff_sec);

                // Print the frequency
                printf("Frequency: %f Hz\n", freq);


            }

            // Update the last timestamp
            last_timestamp = current_timestamp;
        }
    }
}

void app_main(void)
{
    init_timer(); // Initialize the timer
    init_gpio();  // Initialize GPIO and ISR

    // Start a task to handle GPIO events
    while (1)
    {
        xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
