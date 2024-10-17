#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/queue.h"

#define TIMER_DIVIDER 80   // Set clock divider to 80 (1 MHz timer frequency)
#define TIMER_SCALE   (80000000 / TIMER_DIVIDER)  // Convert counter value to seconds
#define TIMER_MAX_COUNT (5)  // Maximum counter value before triggering the interrupt
#define LED_PIN GPIO_NUM_2  // Define LED pin

// Define a speed factor for controlling increment speed (higher means slower)
#define TIMER_INCREMENT_SPEED 4  // Set to 1 for normal speed, >1 for slower, <1 for faster


static const char *TAG = "Counter_EXAMPLE";
static QueueHandle_t timer_queue;  // Queue for timer events

typedef struct {
    int flag;  // Custom event flag to signal timer expiration
} timer_event_t;

// Timer interrupt service routine (ISR)
void IRAM_ATTR timer_group0_isr(void *param) {
    timer_event_t evt = { .flag = 1 };
//  ESP_LOGI(TAG, "Interrupt generated from timer");
    // Clear the interrupt flag
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    // Send event to the queue
    xQueueSendFromISR(timer_queue, &evt, NULL);

    // Turn off the LED when the counter reaches 200 (2 seconds)
    gpio_set_level(LED_PIN, 1);  // Set GPIO high to turn off LED

    // Disable the timer after reaching the max count
    timer_pause(TIMER_GROUP_0, TIMER_0);
}

void setup_timer() {
    // Configure LED pin as output
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 0);  // Initially turn on the LED

    // Timer configuration
    timer_config_t config = {
        .divider = TIMER_DIVIDER * TIMER_INCREMENT_SPEED,  // Modify the divider with speed factor
        .counter_dir = TIMER_COUNT_UP,  // Set to count up
        .counter_en = TIMER_PAUSE,  // The timer is initially paused
        .alarm_en = TIMER_ALARM_EN,  // Enable alarm
        .auto_reload = TIMER_AUTORELOAD_DIS  // Disable auto-reload, one-shot timer
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    // Set the timer's initial counter value (start from 0)
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

    // Set the alarm value (in ticks), multiply by speed factor for custom speed
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, TIMER_MAX_COUNT * TIMER_SCALE * TIMER_INCREMENT_SPEED);

    // Enable interrupt on alarm event
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, (void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);

    // Start the timer
    timer_start(TIMER_GROUP_0, TIMER_0);
}


void app_main(void) {
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));  // Create a queue to handle timer events
    setup_timer();

    // Loop to display the up counter value
    uint64_t timer_counter_value;
    double time_elapsed;
    timer_event_t evt;

    while (1) {
        // Get the current timer counter value
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_counter_value);

        // Convert the counter value to seconds
        time_elapsed = (double) timer_counter_value / TIMER_SCALE;

        // Display the elapsed time
        ESP_LOGI(TAG, "Counter value1: %.f", time_elapsed);

        // Check if the timer has reached 200 using the queue
        if (xQueueReceive(timer_queue, &evt, 0)) {
            if (evt.flag == 1) {
                ESP_LOGI(TAG, "Counter reached 5 seconds, LED turned OFF!");
                break;  // Exit loop after handling event
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

