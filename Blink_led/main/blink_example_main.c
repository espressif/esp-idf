#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#define LED_PIN GPIO_NUM_16
#define SWITCH1 GPIO_NUM_40
#define SWITCH2 GPIO_NUM_41
#define SWITCH3 GPIO_NUM_42

void app_main(void)
{
    // Configure GPIO
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(SWITCH1, GPIO_MODE_INPUT);
    gpio_set_direction(SWITCH2, GPIO_MODE_INPUT);
    gpio_set_direction(SWITCH3, GPIO_MODE_INPUT);

    // while (1)
    {
        // Read switch state
        int switch1_state = gpio_get_level(SWITCH1);

        // Control LED based on switch state
        if (switch1_state == 1)
        {
            gpio_set_level(LED_PIN, 1); // Turn LED on
            printf("LED ON\n");
        }
        else
        {
            gpio_set_level(LED_PIN, 0); // Turn LED off
            printf("LED OFF\n");
        }

        // Print the state of switch1
        printf("switch1_state=%d\n", switch1_state);
        
        // Delay for 1 second
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
