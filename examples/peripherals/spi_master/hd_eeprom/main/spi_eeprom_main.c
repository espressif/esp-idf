/* SPI Master Half Duplex EEPROM example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "spi_eeprom.h"

/*
 This code demonstrates how to use the SPI master half duplex mode to read/write a AT932C46D EEPROM (8-bit mode).
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////// Please update the following configuration according to your HardWare spec /////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if CONFIG_IDF_TARGET_ESP32
#  if CONFIG_EXAMPLE_USE_SPI1_PINS
#   define EEPROM_HOST      SPI1_HOST
// Use default pins, same as the flash chip.
#   define PIN_NUM_MISO     7
#   define PIN_NUM_MOSI     8
#   define PIN_NUM_CLK      6
#  else
#   define EEPROM_HOST      HSPI_HOST
#   define PIN_NUM_MISO     18
#   define PIN_NUM_MOSI     23
#   define PIN_NUM_CLK      19
#  endif
#  define PIN_NUM_CS        13
#else
#  define EEPROM_HOST       SPI2_HOST
#  define PIN_NUM_MISO      13
#  define PIN_NUM_MOSI      12
#  define PIN_NUM_CLK       11
#  define PIN_NUM_CS        10
#endif

static const char TAG[] = "main";

void app_main(void)
{
    esp_err_t ret;
#ifndef CONFIG_EXAMPLE_USE_SPI1_PINS
    ESP_LOGI(TAG, "Initializing bus SPI%d...", EEPROM_HOST + 1);
    spi_bus_config_t buscfg = {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };
    //Initialize the SPI bus
    ret = spi_bus_initialize(EEPROM_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);
#else
    ESP_LOGI(TAG, "Attach to main flash bus...");
#endif

    eeprom_config_t eeprom_config = {
        .cs_io = PIN_NUM_CS,
        .host = EEPROM_HOST,
        .miso_io = PIN_NUM_MISO,
    };
#ifdef CONFIG_EXAMPLE_INTR_USED
    eeprom_config.intr_used = true;
    gpio_install_isr_service(0);
#endif

    eeprom_handle_t eeprom_handle;

    ESP_LOGI(TAG, "Initializing device...");
    ret = spi_eeprom_init(&eeprom_config, &eeprom_handle);
    ESP_ERROR_CHECK(ret);

    ret = spi_eeprom_write_enable(eeprom_handle);
    ESP_ERROR_CHECK(ret);

    const char test_str[] = "Hello World!";
    ESP_LOGI(TAG, "Write: %s", test_str);
    for (int i = 0; i < sizeof(test_str); i++) {
        // No need for this EEPROM to erase before write.
        ret = spi_eeprom_write(eeprom_handle, i, test_str[i]);
        ESP_ERROR_CHECK(ret);
    }

    uint8_t test_buf[32] = "";
    for (int i = 0; i < sizeof(test_str); i++) {
        ret = spi_eeprom_read(eeprom_handle, i, &test_buf[i]);
        ESP_ERROR_CHECK(ret);
    }
    ESP_LOGI(TAG, "Read: %s", test_buf);
    ESP_LOGI(TAG, "Example finished.");

    while (1) {
        // Add your main loop handling code here.
        vTaskDelay(100);
    }
}
