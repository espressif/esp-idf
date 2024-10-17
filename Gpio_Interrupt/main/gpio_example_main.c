#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "driver/gpio.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

volatile int cnt = 0;
volatile int flag = 0, flag1 = 0;
volatile int led_state = 0;
uint32_t gpio_num;

// Define GPIO pins
#define GPIO_OUTPUT_IO_0 18 // Output GPIO pin
#define GPIO_OUTPUT_IO_1 19 // Output GPIO pin
#define GPIO_INPUT_IO_0 4   // Input GPIO pin for interrupt
#define GPIO_INPUT_IO_1 5   // Input GPIO pin for interrupt

// Interrupt Service Routine (ISR) for GPIO
void IRAM_ATTR gpio_isr_handler(void *arg)
{
    gpio_num=(void *)arg;
}

void app_main(void)
{
    // Configure output GPIOs
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;                                          // Disable interrupt for output pins
    io_conf.mode = GPIO_MODE_OUTPUT;                                                // Set as output mode
    io_conf.pin_bit_mask = (1ULL << GPIO_OUTPUT_IO_0) | (1ULL << GPIO_OUTPUT_IO_1); // Pin mask
    io_conf.pull_down_en = 0;                                                       // Disable pull-down
    io_conf.pull_up_en = 0;                                                         // Disable pull-up
    gpio_config(&io_conf);

    // Configure input GPIOs with interrupt
    io_conf.intr_type = GPIO_INTR_NEGEDGE;                                        // Trigger interrupt on falling edge
    io_conf.mode = GPIO_MODE_INPUT;                                               // Set as input mode
    io_conf.pin_bit_mask = (1ULL << GPIO_INPUT_IO_0) | (1ULL << GPIO_INPUT_IO_1); // Pin mask
    io_conf.pull_up_en = 1;                                                       // Enable pull-up
    gpio_config(&io_conf);

    // Install GPIO ISR service
    gpio_install_isr_service(0);
    // Hook ISR handler for input GPIOs
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void *)GPIO_INPUT_IO_0);
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void *)GPIO_INPUT_IO_1);

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

    while (1)
    {
        if (gpio_num==GPIO_INPUT_IO_0)
        {
            gpio_num = (void *)GPIO_INPUT_IO_0;
            cnt++;
            if (led_state == 0)
            {
                gpio_set_level(GPIO_OUTPUT_IO_0, 1);
            }
            else
            {
                gpio_set_level(GPIO_OUTPUT_IO_0, 0);
            }
            led_state = !led_state;
            printf("Interrupt generated..:\n");

            printf("GPIO[%ld] interrupt tri0000ggered\n", gpio_num);
            printf("cnt2: %d\n", cnt);
            flag = 0;
        }
        if (gpio_num==GPIO_INPUT_IO_1)
        {
            gpio_num = (void *)GPIO_INPUT_IO_1;
            cnt++;
            if (led_state == 0)
            {
                gpio_set_level(GPIO_OUTPUT_IO_1, 1);
            }
            else
            {
                gpio_set_level(GPIO_OUTPUT_IO_1, 0);
            }
            led_state = !led_state;
            printf("Interrupt generated..:\n");

            printf("GPIO[%ld] interrupt triggered\n", gpio_num);
            printf("cnt2: %d\n", cnt);
            flag = 0;
        }
        // gpio_set_level(GPIO_OUTPUT_IO_1, (cnt + 1) % 2);

        // printf("flag: %d\n", flag++);
        // printf("cnt: %d\n", cnt);
        // gpio_set_level(GPIO_OUTPUT_IO_0, 0);
        // Simple delay for demonstration (without FreeRTOS)
        gpio_num=0;
        vTaskDelay(100);
    }
}