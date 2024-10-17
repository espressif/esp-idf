#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"


#define UART_NUM UART_NUM_1
#define BUF_SIZE (1024)

void uart_init() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE); // Adjust pins as needed
}
static void IRAM_ATTR uart_intr_handler(void *arg) {
    uint8_t data[BUF_SIZE];
    int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 100 /  portTICK_PERIOD_MS);
    if (len > 0) {
        // Process received data
        data[len] = '\0'; // Null-terminate for string operations
        ESP_LOGI("UART", "Received: %s", data);
    }
}

void app_main() {
    uart_init();
    // Install UART ISR
    uart_enable_rx_intr(UART_NUM);
      uart_isr_free(UART_NUM);
     uart_isr_register(UART_NUM, uart_intr_handler, NULL, 0, NULL);
    
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
