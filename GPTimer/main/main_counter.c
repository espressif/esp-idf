#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include "freertos/task.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/queue.h"
#include <stddef.h>

#define TIMER_DIVIDER 80   // Hardware timer clock divider (80 MHz / 80 = 1 MHz)
#define TIMER_SCALE    (80000000 / TIMER_DIVIDER)  // Convert counter value to seconds
#define TIMER_INTERVAL_SEC (5)  // Set the timer for 10 seconds
#define LED_PIN GPIO_NUM_2  // Define LED pin, GPIO 2 is commonly used

static const char *TAG = "Counter_EXAMPLE";
static QueueHandle_t timer_queue;  // Queue for timer events

typedef struct {
    int flag;  // Custom event flag to signal timer expiration
} timer_event_t;

// Timer interrupt service routine (ISR)
void IRAM_ATTR timer_group0_isr(void *param) {
    timer_event_t evt = { .flag = 1 };

    // Turn on the LED
    gpio_set_level(LED_PIN, 0);  // Set GPIO high to turn on LED

    // Clear the interrupt flag
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    // Send event to the queueimer_set_counter_value(TIMER_GROUP_0, TIMER_0, TIMER_INTERVAL_SEC * TIMER_SCALE);

    xQueueSendFromISR(timer_queue, &evt, NULL);

    // Disable timer after one-shot expiration
    timer_pause(TIMER_GROUP_0, TIMER_0);
}

void setup_timer() {
    // Configure LED pin as output
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(LED_PIN, 1);  // Initially turn off the LED

    // Timer configuration
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_DOWN,  // Set to count down
        .counter_en = TIMER_PAUSE,  // The timer is initially paused
        .alarm_en = TIMER_ALARM_EN,  // Enable alarm
        .auto_reload = TIMER_AUTORELOAD_DIS  // Disable auto-reload, one-shot timer
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    // Set the timer's counter value (in ticks)
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, TIMER_INTERVAL_SEC * TIMER_SCALE);

    // Enable interrupt on alarm event
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, (void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);

    // Start the timer
    timer_start(TIMER_GROUP_0, TIMER_0);
   
}

void app_main(void) {
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));  // Create a queue to handle timer events
    setup_timer();

    // Loop to display the down counter value
    uint64_t timer_counter_value;
    double time_remaining;
    timer_event_t evt;

    while (1) { 
        // Get the current timer counter value
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &timer_counter_value);

        // Convert the counter value to seconds
        time_remaining = (double) timer_counter_value / TIMER_SCALE;
        

        // Display the remaining time
        ESP_LOGI(TAG, "Counter remaining: %.f", time_remaining);

        // Add a delay to avoid flooding the output (e.g., 100ms)
        vTaskDelay((1000)/portTICK_PERIOD_MS);

        // Check if the timer has expired using the queue
        if (xQueueReceive(timer_queue, &evt, 0)) {
            if (evt.flag == 1) {
                ESP_LOGI(TAG, "Counter reached  '0' ");
                ESP_LOGI(TAG, "Timer expired, LED turned oFF!");
                break;
            }  
        }
    }
}