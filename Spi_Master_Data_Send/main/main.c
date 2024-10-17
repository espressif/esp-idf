#include <stdio.h>
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include <string.h> // Include for strlen

#define SPI_HOST SPI2_HOST // Use SPI2_HOST for ESP32-S3
#define PIN_NUM_MISO 10
#define PIN_NUM_MOSI 9
#define PIN_NUM_SCK 8
#define PIN_NUM_CS 5

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
        .mode = 0,                   // SPI mode 0
        .spics_io_num = PIN_NUM_CS,
        .flags = 0,
        .queue_size = 100,
    };

    spi_device_handle_t handle;
    ret = spi_bus_add_device(SPI_HOST, &devcfg, &handle);
    assert(ret == ESP_OK);

    const char *dataToSend = "hello from master"; // Example data to send
     size_t dataLength = strlen(dataToSend) * 8; // Length in bits
     //  size_t dataLength = strlen(dataToSend); // Length in bits

    while (1) {
        spi_transaction_t trans = {
            .length = dataLength,    // Transaction length in bits
            .tx_buffer = dataToSend, // Pointer to the string
            .rx_buffer = receive_buff, // Buffer to store received data
        };

        // Transmit the data
        ret = spi_device_transmit(handle, &trans);
        assert(ret == ESP_OK);

        // Null-terminate the received data to print it as a string
        receive_buff[strlen(dataToSend) ] = '\0'; // Ensure it's null-terminated
        printf("Received: %s\n", receive_buff);

        vTaskDelay(1000 / portTICK_PERIOD_MS); // Wait for a second
    }
}
 