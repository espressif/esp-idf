/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "soc/uart_periph.h"
#include "esp_rom_gpio.h"
#include "driver/gpio.h"
#include "hal/gpio_hal.h"
#include "sdkconfig.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"
#include <string.h>

#define DEFAULT_UART_CHANNEL    (0)
#define CONSOLE_UART_CHANNEL    (1 - DEFAULT_UART_CHANNEL)
#define DEFAULT_UART_RX_PIN     (3)
#define DEFAULT_UART_TX_PIN     (2)
#define CONSOLE_UART_RX_PIN     (4)
#define CONSOLE_UART_TX_PIN     (5)

#define UARTS_BAUD_RATE         (115200)
#define TASK_STACK_SIZE         (2048)
#define READ_BUF_SIZE           (1024)

/* Message printed by the "consoletest" command.
 * It will also be used by the default UART to check the reply of the second
 * UART. As end of line characters are not standard here (\n, \r\n, \r...),
 * let's not include it in this string. */
const char test_message[] = "This is an example string, if you can read this, the example is a success!";

/**
 * @brief This function connects default UART TX to console UART RX and default
 * UART RX to console UART TX. The purpose is to send commands to the console
 * and get the reply directly by reading RX FIFO.
 */
static void connect_uarts(void)
{
    esp_rom_gpio_connect_out_signal(DEFAULT_UART_RX_PIN, UART_PERIPH_SIGNAL(1, SOC_UART_TX_PIN_IDX), false, false);
    esp_rom_gpio_connect_in_signal(DEFAULT_UART_RX_PIN, UART_PERIPH_SIGNAL(0, SOC_UART_RX_PIN_IDX), false);

    esp_rom_gpio_connect_out_signal(DEFAULT_UART_TX_PIN, UART_PERIPH_SIGNAL(0, SOC_UART_TX_PIN_IDX), false, false);
    esp_rom_gpio_connect_in_signal(DEFAULT_UART_TX_PIN, UART_PERIPH_SIGNAL(1, SOC_UART_RX_PIN_IDX), false);
}

/**
 * @brief Disconnect default UART from the console UART, this is used once
 * testing is finished, it will let us print messages on the UART without
 * sending them back to the console UART. Else, we would get an infinite
 * loop.
 */
static void disconnect_uarts(void)
{
    esp_rom_gpio_connect_out_signal(CONSOLE_UART_TX_PIN, UART_PERIPH_SIGNAL(1, SOC_UART_TX_PIN_IDX), false, false);
    esp_rom_gpio_connect_in_signal(CONSOLE_UART_RX_PIN, UART_PERIPH_SIGNAL(1, SOC_UART_RX_PIN_IDX), false);
}

/**
 * @brief Configure and install the default UART, then, connect it to the
 * console UART.
 */
static void configure_uarts(void)
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = UARTS_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(DEFAULT_UART_CHANNEL, READ_BUF_SIZE * 2, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(DEFAULT_UART_CHANNEL, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(DEFAULT_UART_CHANNEL, DEFAULT_UART_TX_PIN, DEFAULT_UART_RX_PIN,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    connect_uarts();
}

/**
 * @brief Function called when command `consoletest` will be invoked.
 * It will simply print `test_message` defined above.
 */
static int console_test(int argc, char **argv)
{
    printf("%s\n", test_message);
    return 0;
}

/**
 * @brief Function executed in another task then main one (as the one main
 * executes REPL console).
 * It will send "consoletest" command to the console UART and then read back
 * the response on RX.
 * The response shall contain the test_message string.
 */
static void send_commands(void* arg)
{
    static char data[READ_BUF_SIZE];
    char command[] = "consoletest\n";
    int len = 0;
    void* substring = NULL;

    /* Discard the first messages sent by the console. */
    do {
        len = uart_read_bytes(DEFAULT_UART_CHANNEL, data, READ_BUF_SIZE, 100 / portTICK_PERIOD_MS);
    } while (len == 0);

    if (len == -1) {
        goto end;
    }
    /* Send the command `consoletest` to the console UART. */
    len = uart_write_bytes(DEFAULT_UART_CHANNEL, command, sizeof(command));
    if (len == -1) {
        goto end;
    }

    /* Get the answer back from the console, give it some delay. */
    do {
        len = uart_read_bytes(DEFAULT_UART_CHANNEL, data, READ_BUF_SIZE - 1, 250 / portTICK_PERIOD_MS);
    } while (len == 0);

    if (len == -1) {
        goto end;
    }

    /**
     * Check whether we can find test_message in the received message. Before
     * that, we need to add a NULL character to terminate the string.
     */
    data[len] = 0;
    substring = strcasestr(data, test_message);

end:
    /* This is a must to not send anything to the console anymore! */
    disconnect_uarts();
    printf("Result: %s\n", substring == NULL ? "Failure" : "Success");
    vTaskDelete(NULL);
}

void app_main(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "repl >";
    const esp_console_cmd_t cmd = {
        .command = "consoletest",
        .help = "Test console by sending a message",
        .func = &console_test,
    };
    esp_console_dev_uart_config_t uart_config = {
        .channel = CONSOLE_UART_CHANNEL,
        .baud_rate = UARTS_BAUD_RATE,
        .tx_gpio_num = CONSOLE_UART_TX_PIN,
        .rx_gpio_num = CONSOLE_UART_RX_PIN,
    };
    /**
     * As we don't have a real serial terminal, (we just use default UART to
     * send and receive commands, ) we won't handle any escape sequence, so the
     * easiest thing to do is set the console to "dumb" mode. */
    linenoiseSetDumbMode(1);

    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));
    configure_uarts();

    ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));

    /* Create a task for sending and receiving commands to and from the second UART. */
    xTaskCreate(send_commands, "send_commands_task", TASK_STACK_SIZE, NULL, 10, NULL);

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
