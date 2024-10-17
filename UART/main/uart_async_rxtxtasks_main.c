// #include <stdio.h>
// #include <string.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/uart.h"
// #include "esp_log.h"

// #define UART_NUM UART_NUM_0          // UART Number
// #define BUF_SIZE (1024)              // Buffer size for UART
// #define TXD_PIN 6
// #define RXD_PIN 7                // RX Pin (GPIO 7)
// #define DEFAULT_STR "Hello from ESP32-C3!\n" // Default string to send

// void uart_init() {
//     // Configure UART parameters
//     uart_config_t uart_config = {
//         .baud_rate = 115200,
//         .data_bits = UART_DATA_8_BITS,
//         .parity = UART_PARITY_DISABLE,
//         .stop_bits = UART_STOP_BITS_1,
//         .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
//     };
    
//     // Install UART driver
//     uart_driver_install(UART_NUM, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL, 0);
//     uart_param_config(UART_NUM, &uart_config);
//     uart_set_pin(UART_NUM, TXD_PIN,  RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
// }

// void uart_task(void *arg) {
//     // Send default string
//     uart_write_bytes(UART_NUM, DEFAULT_STR, strlen(DEFAULT_STR));

//     // Buffer for receiving data
//     uint8_t *data = (uint8_t *) malloc(BUF_SIZE + 1);
//     while (1) {
//         // Read data from UART
//         int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 1000 / portTICK_PERIOD_MS);
//         if (len > 0) {
//             data[len] = '\0'; // Null-terminate the string
//             printf("Received: %s\n", data);
//         }
//         vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
//     }
//     free(data);
// }

// void app_main(void) {
//     // Initialize UART
//     uart_init();

//     // Create UART task
//     xTaskCreate(uart_task, "uart_task", 2048, NULL, 10, NULL);
// }





#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"

#define TXD_PIN (GPIO_NUM_7)
#define RXD_PIN (GPIO_NUM_6)
#define BUF_SIZE (1024)


void init(void)
{
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits =  UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT
    };

    uart_driver_install(UART_NUM_0, BUF_SIZE * 2, 0,0, NULL,0);
    uart_param_config(UART_NUM_0,&uart_config);
}

void send_data(const char *data)
{
    uart_write_bytes(UART_NUM_0, data, strlen(data));
}

void recive_data(void )
{
     uint8_t* data = (uint8_t*)malloc(BUF_SIZE + 1);
    // reading data from the uart..

    int rx_length = uart_read_bytes(UART_NUM_0, data, BUF_SIZE, 1000 / portTICK_PERIOD_MS);
    if(rx_length > 0)
    {
        data[rx_length]=0;
        ESP_LOGI("RECEIVE", "Read %d bytes: '%s'", rx_length, data);
        send_data("Echo: ");
        send_data((const char *)data);
        send_data("\n");
    }      
}

void app_main(void)
{
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    init();

    while(1)
    {
        send_data("hello from esp32..:\n");

        recive_data();
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

}