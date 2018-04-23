/*
 *  @Copyright 2018 Jasbir Matharu
 *  RS485 half duplex data direction control echo example using UART.
 *
 *  This example code is in the Public Domain (or CC0 licensed, at your option.)
 *
 *  Unless required by applicable law or agreed to in writing, this
 *  software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

/**
 * This example was tested with a SP3485 RS485 breakout board (i.e SparkFun Transceiver Breakout) 
 * which host a RTS pin for data direction control. In this example any data the UART receives is 
 * sent back to the sender by toggling the RTS to set the direction. 
 * It was also tested with a XY-017 RS485 to TTL Module (Auto Direction Transceiver) with the RTS 
 * pin disabled and uart_set_rts commented out.
 *
 * - Port: UART1
 * - Pin assignment: see defines below
 */

#define ECHO_TEST_TXD  (GPIO_NUM_4)
#define ECHO_TEST_RXD  (GPIO_NUM_5)
// Set ECHO_TEST_RTS to 'UART_PIN_NO_CHANGE' if using a Auto Direction Transceiver
#define ECHO_TEST_RTS  (18)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

// Enable these to dump the contents received
#define DEBUG_RX_DUMP 1
#define DEBUG_RX_DUMP_HEX 0

static void echo_task()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_EVEN,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    // For RS485 disable tx/rx loopback and enable tx while rx is busy. 
    uart_set_rs485(UART_NUM_1,true,false,true);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
       
    while (1) {
        // Enabe RX by toggling RTS to high
        uart_set_rts(UART_NUM_1,1);
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 500 / portTICK_RATE_MS);
        // Reset RTS to low for TX
        uart_set_rts(UART_NUM_1,0);
        if (len >0) {
#if DEBUG_RX_DUMP == 1
            int i=0;
            for (i=0;i<len;i++) {
#if DEBUG_RX_DUMP_HEX == 1
                printf("%02X ",*(data+i));
#else 
                printf("%c",*(data+i));
#endif // DEBUG_RX_DUMP_HEX 
            }
            printf(" %d \r\n",len);
#endif // DEBUG_RX_DUMP

            // Write data back to the UART
            uart_write_bytes(UART_NUM_1, (const char *) data, len);
            // Wait for tx fifo empty as we are in half duplex mode 
            uart_wait_tx_done(UART_NUM_1,500 / portTICK_RATE_MS);
        }
    }
}

void app_main()
{
    xTaskCreate(echo_task, "rs485_echo_task", 2048, NULL, 10, NULL);
}
