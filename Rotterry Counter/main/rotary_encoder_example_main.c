#include "driver/gpio.h"
#include "driver/timer.h"

#define CAPTURE_PIN GPIO_NUM_4

// Variables to store the captured times
volatile uint32_t last_capture_time = 0;
volatile uint32_t pulse_width = 0;

void IRAM_ATTR capture_isr_handler(void *arg) {
    // Capture the current time from the hardware timer
    uint32_t current_time = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, TIMER_0);
    
    // Calculate the pulse width or time interval
    pulse_width = current_time - last_capture_time;
    
    // Store the current time for the next capture
    last_capture_time = current_time;
}

void setup_input_capture() {
    // Configure GPIO interrupt
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE,  // Falling edge trigger
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << CAPTURE_PIN),
    };
    gpio_config(&io_conf);
    
    // Install GPIO ISR handler
    gpio_install_isr_service(0);
    gpio_isr_handler_add(CAPTURE_PIN, capture_isr_handler, NULL);
    
    // Initialize hardware timer
    timer_config_t config = {
        .divider = 80,  // Timer clock source: 1 MHz (80 MHz / 80)
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .auto_reload = TIMER_AUTORELOAD_DIS,
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_start(TIMER_GROUP_0, TIMER_0);
}
