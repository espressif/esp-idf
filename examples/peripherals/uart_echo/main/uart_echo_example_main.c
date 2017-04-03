/* Uart Events Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"

/**
 * This is a example exaple which echos any data it receives on UART1 back to the sender, with hardware flow control
 * turned on. It does not use UART driver event queue.
 *
 * - port: UART1
 * - rx buffer: on
 * - tx buffer: off
 * - flow control: on
 * - event queue: off
 * - pin assignment: txd(io4), rxd(io5), rts(18), cts(19)
 */

#define ECHO_TEST_TXD  (4)
#define ECHO_TEST_RXD  (5)
#define ECHO_TEST_RTS  (18)
#define ECHO_TEST_CTS  (19)

#define BUF_SIZE (1024)

//an example of echo test with hardware flow control on UART1
static void echo_task()
{
    const int uart_num = UART_NUM_1;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        .rx_flow_ctrl_thresh = 122,
    };
    //Configure UART1 parameters
    uart_param_config(uart_num, &uart_config);
    //Set UART1 pins(TX: IO4, RX: I05, RTS: IO18, CTS: IO19)
    uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    //Install UART driver (we don't need an event queue here)
    //In this example we don't even use a buffer for sending data.
    uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL, 0);

    uint8_t* data = (uint8_t*) malloc(BUF_SIZE);
    while(1) {
        //Read data from UART
        int len = uart_read_bytes(uart_num, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        //Write data back to UART
        uart_write_bytes(uart_num, (const char*) data, len);
    }
}

void app_main()
{
    //A uart read/write example without event queue;
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
