/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "esp_check.h"
#include "soft_spi.h"

#define READ_COUNT  16

const char* EXAMPLE_TAG = "soft_spi_master";

void app_main(void)
{
    esp_err_t ret = ESP_OK;
    const uint8_t write_buffer[] = "Hello, world! This is a message.\n";
    uint8_t read_buffer[sizeof(write_buffer)] = { 0 };
    soft_spi_bus_t bus = NULL;
    soft_spi_config_t config = {
        .clk_pin = CONFIG_EMULATE_SPI_GPIO_CLOCK,
        .mosi_pin = CONFIG_EMULATE_SPI_GPIO_MOSI,
        .miso_pin = CONFIG_EMULATE_SPI_GPIO_MISO,
        .cs_pin = CONFIG_EMULATE_SPI_GPIO_CS
    };

    /* Initialize and configure the software SPI bus */
    ret = soft_spi_new(&config, &bus);
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error configuring software SPI");

    /* Initiate the transfer. The software SPI is full-duplex, we will send and
     * receive data at the same time. */
    ret = soft_spi_transfer(bus, write_buffer, read_buffer, sizeof(read_buffer));
    ESP_GOTO_ON_ERROR(ret, error, EXAMPLE_TAG, "Error reading from the software SPI");

    printf("SPI transfers succeeded, received bytes: { ");
    for (int i = 0; i < sizeof(read_buffer); i++) {
        printf("0x%02x ", read_buffer[i]);
    }
    printf("}\n");
error:
    if (bus != NULL) {
        soft_spi_del(bus);
    }
    if (ret != ESP_OK) {
        ESP_LOGE(EXAMPLE_TAG, "An error occurred while transferring date on software SPI bus");
    }
}
