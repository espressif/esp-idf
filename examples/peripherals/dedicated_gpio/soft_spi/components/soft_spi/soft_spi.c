/*
 * SPDX-FileCopyrightText: 2010-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "driver/dedic_gpio.h"
#include "driver/gpio.h"
#include "soft_spi.h"

#define ERR_CHECK_OR_GOTO(ret, label) do { if((ret) != ESP_OK ) goto label; } while (0)

/* Forward declaration of static functions */
/**
 * @brief Send data on the emulated SPI MOSI while reading at the same time from MISO
 *        Fast GPIOs are used for the emulation, 4 I/Os are required.
 *
 * @param tx Buffer containing the bytes to send.
 * @param rx Buffer to store the received bytes in.
 * @param size Size of both buffers.
 */
void IRAM_ATTR asm_emulate_spi_shift_byte(const uint8_t* tx, uint8_t* rx, uint32_t size,
                                          uint32_t clock_bit, uint32_t mosi_bit, uint32_t cs_bit,
                                          uint32_t miso_bit);

/* Mutex required to enter critical sections */
static portMUX_TYPE g_lock = portMUX_INITIALIZER_UNLOCKED;

const char* __attribute__((used)) SOFT_SPI_TAG = "soft_spi";

struct soft_spi_bus_impl_t {
    uint32_t clk_bit;
    uint32_t mosi_bit;
    uint32_t miso_bit;
    uint32_t cs_bit;
    dedic_gpio_bundle_handle_t out_bundle;
    dedic_gpio_bundle_handle_t in_bundle;
};

esp_err_t soft_spi_new(soft_spi_config_t *config, soft_spi_bus_t *bus)
{
    esp_err_t ret;
    struct soft_spi_bus_impl_t *bus_impl = NULL;

    /* Check the parameters */
    ESP_GOTO_ON_FALSE(config != NULL && bus != NULL, ESP_ERR_INVALID_ARG, error, SOFT_SPI_TAG,
                      "Parameters must not be NULL");

    const int clk = config->clk_pin;
    const int mosi = config->mosi_pin;
    const int miso = config->miso_pin;
    const int cs = config->cs_pin;

    /**
     * Create a dedicated GPIO bundle configuration for the SPI output pins: Clock, MOSI, and CS, in this order.
     * The order will define the position of each I/O in the bundle, which is important when writing to it and
     * reading from it.
     */
    int output_array[] = { clk, mosi, cs };
    dedic_gpio_bundle_config_t output_config = {
        .gpio_array = output_array,
        .array_size = 3,
        .flags = {
            .out_en = 1
        }
    };
    /**
     * Same goes for the input pin: MISO.
     */
    int input_array[] = { miso };
    dedic_gpio_bundle_config_t input_config = {
        .gpio_array = input_array,
        .array_size = 1,
        .flags = {
            .in_en = 1
        }
    };

    /* Allocate the master bus structure now that we need it. */
    bus_impl = malloc(sizeof(struct soft_spi_bus_impl_t));
    ESP_GOTO_ON_FALSE(bus_impl != NULL, ESP_ERR_NO_MEM, error, SOFT_SPI_TAG, "No more memory available in the system");

    /* Initialize the dedicated GPIO bundles with the configurations we made above. */
    ret = dedic_gpio_new_bundle(&output_config, &bus_impl->out_bundle);
    ERR_CHECK_OR_GOTO(ret, error_output);
    ret = dedic_gpio_new_bundle(&input_config, &bus_impl->in_bundle);
    ERR_CHECK_OR_GOTO(ret, error_input);

    /**
     * Before executing the assembly routine, get the offset (bit) of each of the pins in the dedicated GPIO
     * registers. For example, if CLK pin's offset is 0, then bit 0 of the special register will be assigned
     * to it.
     */
    ret = dedic_gpio_get_out_offset(bus_impl->out_bundle, &bus_impl->clk_bit);
    ERR_CHECK_OR_GOTO(ret, error_offset);
    ret = dedic_gpio_get_in_offset(bus_impl->in_bundle, &bus_impl->miso_bit);
    ERR_CHECK_OR_GOTO(ret, error_offset);
    /* MOSI and CS immediately follow the Clock in the special register. */
    bus_impl->mosi_bit = bus_impl->clk_bit + 1;
    bus_impl->cs_bit = bus_impl->clk_bit + 2;

    /* Return the bus to the user */
    *bus = bus_impl;

    return ret;

error_offset:
    dedic_gpio_del_bundle(bus_impl->in_bundle);
error_input:
    dedic_gpio_del_bundle(bus_impl->out_bundle);
error_output:
    if (bus_impl != NULL) {
        free(bus_impl);
    }
error:
    return ret;
}

esp_err_t soft_spi_del(soft_spi_bus_t bus)
{
    esp_err_t ret;
    ESP_GOTO_ON_FALSE(bus != NULL, ESP_ERR_INVALID_ARG, error, SOFT_SPI_TAG, "Bus must not be NULL");

    dedic_gpio_del_bundle(bus->out_bundle);
    dedic_gpio_del_bundle(bus->in_bundle);

    free(bus);
error:
    return ret;
}

esp_err_t soft_spi_transfer(soft_spi_bus_t bus, const uint8_t* write_buffer, uint8_t* read_buffer, size_t buf_size)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(bus != NULL, ESP_ERR_INVALID_ARG, error, SOFT_SPI_TAG, "Bus must not be NULL");
    ESP_GOTO_ON_FALSE(write_buffer != NULL, ESP_ERR_INVALID_ARG, error, SOFT_SPI_TAG, "Write buffer must not be NULL");
    ESP_GOTO_ON_FALSE(buf_size != 0, ESP_ERR_INVALID_ARG, error, SOFT_SPI_TAG, "Buffer size must not be 0");

    portENTER_CRITICAL(&g_lock);
    asm_emulate_spi_shift_byte(write_buffer, read_buffer, buf_size,
                               /* Provide the offset in special dedicated GPIO register for Clock, MOSI and Clock
                                * respectively */
                               bus->clk_bit, bus->mosi_bit, bus->cs_bit, bus->miso_bit);
    portEXIT_CRITICAL(&g_lock);

error:
    return ret;
}
