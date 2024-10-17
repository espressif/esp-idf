#include <stdio.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gptimer.h"   // New general-purpose timer API
#include <string.h>           // Include for strlen

#define SPI_HOST SPI2_HOST // Use SPI2_HOST for ESP32-S3
#define PIN_NUM_MISO 10
#define PIN_NUM_MOSI 9
#define PIN_NUM_SCK 8
#define PIN_NUM_CS 5

#define TIMER_INTERVAL_SEC 1  // Timer interval (in seconds)

TaskHandle_t xTaskHandle;

// Timer ISR handler (called every time the timer interrupt occurs)
bool IRAM_ATTR timer_isr_callback(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_ctx) {
    // Notify the task that the timer interrupt has occurred
    BaseType_t high_task_awoken = pdFALSE;
    vTaskNotifyGiveFromISR(xTaskHandle, &high_task_awoken);

    // Return true to clear the alarm event
    return high_task_awoken == pdTRUE;
}

// Task that handles SPI communication when notified by the timer interrupt
void spi_task(void* arg) {
    // SPI device handle
    spi_device_handle_t handle = (spi_device_handle_t)arg;
    uint8_t receive_buff[100];
    const char* dataToSend = "hello from master";  // Example data to send
    size_t dataLength = strlen(dataToSend) * 8;    // Data length in bits

    while (1) {
        // Wait for notification from the timer interrupt
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // SPI transaction structure
        spi_transaction_t trans = {
            .length = dataLength,     // Transaction length in bits
            .tx_buffer = dataToSend,  // Data to transmit
            .rx_buffer = receive_buff // Buffer to receive data
        };

        // Transmit the data
        esp_err_t ret = spi_device_transmit(handle, &trans);
        assert(ret == ESP_OK);

        // Null-terminate the received data to print it as a string
        receive_buff[strlen(dataToSend)] = '\0'; // Ensure it's null-terminated
        printf("Received: %s\n", receive_buff);
    }
}

// Function to initialize the GPTimer and set up the interrupt
void init_timer() {
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,      // Use default clock source
        .direction = GPTIMER_COUNT_UP,           // Count up
        .resolution_hz = 1000000,                // 1 MHz resolution (1 tick = 1us)
    };

    // Create the GPTimer
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    // Configure the timer alarm
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = TIMER_INTERVAL_SEC * 1000000, // Set alarm interval (in microseconds)
        .reload_count = 0,                          // Start counting from zero after each alarm
        .flags.auto_reload_on_alarm = true          // Auto-reload on alarm event
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

    // Register the ISR callback for timer alarms
    gptimer_event_callbacks_t cbs = {
        .on_alarm = timer_isr_callback,            // Set the callback for timer interrupts
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

    // **NEW**: Enable the timer before starting it
    ESP_ERROR_CHECK(gptimer_enable(gptimer));

    // Start the timer
    ESP_ERROR_CHECK(gptimer_start(gptimer));
}

void app_main(void) {
    // Initialize the SPI bus
    uint8_t receive_buff[100];
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = sizeof(receive_buff),
    };
    esp_err_t ret = spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);

    // Configure the SPI device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1 * 1000, // Set clock speed to 1 kHz
        .mode = 0,                  // SPI mode 0
        .spics_io_num = PIN_NUM_CS,
        .flags = 0,
        .queue_size = 100,
    };

    spi_device_handle_t handle;
    ret = spi_bus_add_device(SPI_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);

    // Create the SPI task and pass the SPI device handle
    xTaskCreate(spi_task, "spi_task", 2048, (void*)handle, 10, &xTaskHandle);

    // Initialize the GPTimer and set up the interrupt
    init_timer();
}
