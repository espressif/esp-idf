
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define LED_PIN GPIO_NUM_2  // In some ESP32 boards have an inbuilt LED

volatile int interruptCounter = 0;  // For counting interrupts
int totalInterruptCounter = 0;       // Total interrupt counting
int LED_STATE = 0;

portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() {  // Interrupt function
    portENTER_CRITICAL_ISR(&timerMux);
    interruptCounter++;
    portEXIT_CRITICAL_ISR(&timerMux);
}

void app_main() {
    // Initialize the LED pin
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Set up the timer
    const esp_timer_create_args_t timer_args = {
        .callback = &onTimer,
        .name = "timer"
    };
    esp_timer_handle_t timer;
    esp_timer_create(&timer_args, &timer);
    
    // Start the timer with a period of 1 second
    esp_timer_start_periodic(timer, 1000000);  // 1 second in microseconds

    while (1) {
        if (interruptCounter > 0) {
            portENTER_CRITICAL(&timerMux);
            interruptCounter--;
            portEXIT_CRITICAL(&timerMux);

            totalInterruptCounter++;  // Count total interrupts

            LED_STATE = !LED_STATE;   // Toggle logic
            gpio_set_level(LED_PIN, LED_STATE);  // Toggle LED
            printf("An interrupt has occurred. Total number: %d\n", totalInterruptCounter);
        }
        vTaskDelay(pdMS_TO_TICKS(10));  // Small delay to prevent busy-waiting
    }
}


/*
#include <stdio.h>
#include <time.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gpio.h"

// Define the GPIO pin for the internal LED (GPIO_NUM_2 for built-in LED on some boards)
#define LED_PIN GPIO_NUM_2

static const char *TAG = "RTC_ALARM";

// Set your desired alarm time here (24-hour format)
#define ALARM_HOUR 0  // 4 PM
#define ALARM_MINUTE 1 // 00 minutes

// Function to initialize the LED
void init_led(void) {
    //gpio_pad_select_gpio(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
}

// Function to check if it's time to wake the LED
bool check_alarm_time(struct tm *timeinfo) {
    // Check if the current time matches the alarm time
    if (timeinfo->tm_hour == ALARM_HOUR && timeinfo->tm_min == ALARM_MINUTE) {
        return true;
    }
    return false;
}

// Timer callback function that checks the current time and triggers the LED
void periodic_timer_callback(void* arg) {
    time_t now;
    struct tm timeinfo;
    
    time(&now);
    localtime_r(&now, &timeinfo);

    ESP_LOGI(TAG, "Current time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    // Check if the current time matches the alarm time
    if (check_alarm_time(&timeinfo)) {
        ESP_LOGI(TAG, "Alarm triggered! Turning on the LED.");
        gpio_set_level(LED_PIN, 1);  // Turn on the LED
    } else {
        gpio_set_level(LED_PIN, 0);  // Turn off the LED
    }
}

void app_main(void) {
    init_led();  // Initialize the LED GPIO
    
    // Configure time (RTC or NTP sync can be done here if needed)
    time_t now;
    struct tm timeinfo;

    // Simulate setting the time manually if there's no NTP sync
    time(&now);
    localtime_r(&now, &timeinfo);

    // Log the starting time
    ESP_LOGI(TAG, "Starting time: %02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

    // Create a periodic timer to check the current time every second
    const esp_timer_create_args_t periodic_timer_args = {
        .callback = &periodic_timer_callback,
        .name = "periodic_timer"
    };

    esp_timer_handle_t periodic_timer;
    esp_timer_create(&periodic_timer_args, &periodic_timer);
    esp_timer_start_periodic(periodic_timer, 1000000);  // Check every 1 second (1000000 us)
}
  */

/*
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "esp_log.h"

#define LED_PIN GPIO_NUM_2 // Define your LED pin here
static const char *TAG = "Timer Interrupt";

volatile int interruptCounter = 0; // Counter for interrupts
int totalInterruptCounter = 0;      // Total interrupt count
int LED_STATE = 0;

// Timer interrupt handler
void IRAM_ATTR timer_isr(void* arg) {
    interruptCounter++;
}

// Timer initialization
void init_timer() {
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_START,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = true,
        .divider = 80  // Timer runs at 1 MHz
    };

    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000); // Set alarm to 1 second
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
   // timer_start(TIMER_GROUP_0, TIMER_0);

    // Install ISR service
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_isr, NULL, 0, NULL);
}

void app_main() {
    // Configure LED GPIO
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    // Initialize the timer
    init_timer();

    while (1) {
        if (interruptCounter > 0) {
            interruptCounter--;
            totalInterruptCounter++; // Count total interrupts

            LED_STATE = !LED_STATE;  // Toggle LED state
            gpio_set_level(LED_PIN, LED_STATE); // Write the new state to the LED
            
            ESP_LOGI(TAG, "An interrupt has occurred. Total number: %d", totalInterruptCounter);
        }

        // Optional: Add a small delay to reduce CPU usage
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    }

*/
/*
#include <stdio.h>
#include <time.h>
#include "esp_system.h"
#include <sys/time.h>
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//static const char *TAG = "Calendar";

// Function to set time manually
void set_time()
{
    struct timeval tv;
    struct tm timeinfo;
    // Set desired date and time (YYYY-MM-DD HH:MM:SS)
    timeinfo.tm_year = 2023 - 1900; // Year since 1900
    timeinfo.tm_mon = 9 - 1;        // Month (0-11)
    timeinfo.tm_mday = 23;          // Day of the month (1-31)
    timeinfo.tm_hour = 10;          // Hour (0-23)
    timeinfo.tm_min = 43;           // Minutes (0-59)
    timeinfo.tm_sec = 45;           // Seconds (0-59)

    // Convert to timeval
    tv.tv_sec = mktime(&timeinfo);
    tv.tv_usec = 0;

    // Set the time
    settimeofday(&tv, NULL);
    printf("Time set manually!\n");
}

// Interrupt-like function to be called when seconds == 5
void on_second_five()
{
    printf("Interrupt: Seconds is now 5!\n");
}

// Function to obtain and print the current time
void print_time()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    struct tm timeinfo;
    localtime_r(&now.tv_sec, &timeinfo);

    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    printf("Current time: %s\n", strftime_buf);
    
    // Check if the seconds part is 5
    if (timeinfo.tm_sec == 5) {
        on_second_five(); // Call the interrupt-like function
    }
}

void app_main()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    set_time();

    while (1)
    {
        // Wait for 1 second
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // Obtain and print the current time
        print_time();
    }
}
*/