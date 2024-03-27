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
#include "soft_uart.h"

#define ERR_CHECK_OR_GOTO(ret, label) do { if((ret) != ESP_OK ) goto label; } while (0)

/* Forward declaration of static functions */
void IRAM_ATTR emulate_uart_send(const uint8_t* tx_msg, uint32_t tx_size, uint32_t tx_bit, uint32_t baudrate);
void IRAM_ATTR emulate_uart_receive(uint8_t* rx_msg, uint32_t rx_size, uint32_t rx_bit, uint32_t baudrate);

static uint32_t baudrate_to_cycles(soft_uart_baudrate_t baudrate);

/* Mutex required to enter critical sections */
static portMUX_TYPE g_lock = portMUX_INITIALIZER_UNLOCKED;

const char* __attribute__((used)) SOFT_UART_TAG = "soft_uart";

/***** Public API implementation *****/
struct soft_uart_port_impl_t {
    uint32_t tx_bit;
    uint32_t rx_bit;
    uint32_t cycles;
    dedic_gpio_bundle_handle_t tx_bundle;
    dedic_gpio_bundle_handle_t rx_bundle;
};

esp_err_t soft_uart_new(soft_uart_config_t *config, soft_uart_port_t *port)
{
    esp_err_t ret;
    struct soft_uart_port_impl_t *port_impl = NULL;

    /* Check the parameters */
    ESP_GOTO_ON_FALSE(config != NULL && port != NULL, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG,
                      "Parameters must not be NULL");
    ESP_GOTO_ON_FALSE(config->baudrate < SOFT_UART_BAUD_END, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG,
                      "Invalid baudrate");

    int tx = config->tx_pin;
    int rx = config->rx_pin;

    /* In order to prevent the receiver to get garbage while we configure the GPIOs, pull the pins up to
     * reflect a UART idle state. */
    ret = gpio_set_pull_mode(tx, GPIO_PULLUP_ENABLE);
    ERR_CHECK_OR_GOTO(ret, error);
    ret = gpio_set_pull_mode(rx, GPIO_PULLUP_ENABLE);
    ERR_CHECK_OR_GOTO(ret, error);
    ret = gpio_set_direction(tx, GPIO_MODE_OUTPUT);
    ERR_CHECK_OR_GOTO(ret, error);
    ret = gpio_set_direction(rx, GPIO_MODE_INPUT);
    ERR_CHECK_OR_GOTO(ret, error);

    /**
     * Before actually calling any assembly routine, we need to configure the GPIOs
     * We can do this in C. Using dedic_gpio API will do this for us, it will route
     * the instruction-controlled signals to the GPIO pads thanks to the GPIO matrix.
     *
     * Use one GPIO as output, for TX, and one as input, for RX.
     * Create the configuration for each.
     */
    dedic_gpio_bundle_config_t tx_config = {
        .gpio_array = &tx,
        .array_size = 1,
        .flags = {
            .out_en = 1
        }
    };
    dedic_gpio_bundle_config_t rx_config = {
        .gpio_array = &rx,
        .array_size = 1,
        .flags = {
            .in_en = 1
        }
    };

    /* Allocate the master port structure now that we need it */
    port_impl = malloc(sizeof(struct soft_uart_port_impl_t));
    ESP_GOTO_ON_FALSE(port_impl != NULL, ESP_ERR_NO_MEM, error, SOFT_UART_TAG, "No more memory available in the system");

    /* Initialize the dedicated GPIO bundles */
    ret = dedic_gpio_new_bundle(&tx_config, &port_impl->tx_bundle);
    ERR_CHECK_OR_GOTO(ret, error);
    ret = dedic_gpio_new_bundle(&rx_config, &port_impl->rx_bundle);
    ERR_CHECK_OR_GOTO(ret, error_rx);

    /**
     * Before executing the assembly routine, get the offset of TX/RX in the dedicated GPIO registers
     */
    ret = dedic_gpio_get_out_offset(port_impl->tx_bundle, &port_impl->tx_bit);
    ERR_CHECK_OR_GOTO(ret, error_offset);
    ret = dedic_gpio_get_out_offset(port_impl->rx_bundle, &port_impl->rx_bit);
    ERR_CHECK_OR_GOTO(ret, error_offset);

    port_impl->cycles = baudrate_to_cycles(config->baudrate);

    *port = port_impl;

    return ret;

error_offset:
    dedic_gpio_del_bundle(port_impl->rx_bundle);
error_rx:
    dedic_gpio_del_bundle(port_impl->tx_bundle);
error:
    if (port_impl != NULL) {
        free(port_impl);
    }
    return ret;
}

esp_err_t soft_uart_del(soft_uart_port_t port)
{
    esp_err_t ret;
    ESP_GOTO_ON_FALSE(port != NULL, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Bus must not be NULL");

    dedic_gpio_del_bundle(port->tx_bundle);
    dedic_gpio_del_bundle(port->rx_bundle);

    free(port);
error:
    return ret;
}

esp_err_t soft_uart_send(soft_uart_port_t port, const uint8_t* write_buffer, size_t write_size)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(port != NULL, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Bus must not be NULL");
    ESP_GOTO_ON_FALSE(write_buffer != NULL, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Buffer must not be NULL");
    ESP_GOTO_ON_FALSE(write_size != 0, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Buffer size must not be 0");

    portENTER_CRITICAL(&g_lock);
    emulate_uart_send(write_buffer, write_size, port->tx_bit, port->cycles);
    portEXIT_CRITICAL(&g_lock);

error:
    return ret;
}

esp_err_t soft_uart_receive(soft_uart_port_t port, uint8_t* read_buffer, size_t read_size)
{
    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_FALSE(port != NULL, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Bus must not be NULL");
    ESP_GOTO_ON_FALSE(read_buffer != NULL, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Buffer must not be NULL");
    ESP_GOTO_ON_FALSE(read_size != 0, ESP_ERR_INVALID_ARG, error, SOFT_UART_TAG, "Buffer size must not be 0");

    portENTER_CRITICAL(&g_lock);
    emulate_uart_receive(read_buffer, read_size, port->rx_bit, port->cycles);
    portEXIT_CRITICAL(&g_lock);

error:
    return ret;
}

/***** Private helpers *****/

static uint32_t baudrate_to_cycles(soft_uart_baudrate_t baudrate)
{
    /**
     * Calculate the delay to wait between each bit depending on the UART baudrate and the CPU frequency.
     * For each delay, subtract a small amount of clock cycles which compensate for the instructions
     * used to prepare the next bits (loop, shifts, logic...).
     */
    switch (baudrate) {
    case SOFT_UART_115200: // 115200, 8.63us per bit
        return ((CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 863) / 100 - 20);
    case SOFT_UART_230400: // 4.34us per bit
        return ((CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 434) / 100 - 24);
    case SOFT_UART_460800: // 2.17us per bit
        return ((CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 217) / 100 - 20);
    case SOFT_UART_921600: // 1.085us per bit
        return ((CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 108) / 100 - 23);
    default:
        assert(false);
        return 0;
    }
}
