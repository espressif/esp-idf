/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "driver/spi_slave.h"
#include "driver/gpio.h"

#include "ble_spi_slave.h"

/*
SPI receiver (slave) example.

This example is supposed to work together with a NimBLE module acting as SPI master.
It uses the standard SPI pins (MISO, MOSI, SCLK, CS) to transmit logs over in a
half-duplex fashion, that is, the master puts data on the MOSI pin, while the MISO pins
remain inactive.
*/

//Dummy callback called after a transaction is queued and ready for pickup by master.
static void my_post_setup_cb(spi_slave_transaction_t *trans)
{
    return;
}

//Dummy callback called after transaction is sent/received.
static void my_post_trans_cb(spi_slave_transaction_t *trans)
{
    return;
}

//Main application
void app_main(void)
{
    int n = 0;
    esp_err_t ret;
    spi_slave_transaction_t t = {0};

    //Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = -1,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    //Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 3,
        .flags = 0,
        .post_setup_cb = my_post_setup_cb,
        .post_trans_cb = my_post_trans_cb
    };

    //Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    /**
     * The default drive capability on esp32 is GPIO_DRIVE_CAP_2 (~20 mA).
     * When connecting master devices that uses a source/sink current lower or higher than GPIO_DRIVER_CAP_DEFAULT.
     * Using a drive strength that does not match the requirements of the connected device can cause issues
     * such as unreliable switching, or damage to the GPIO pin or external device.
     *
     * - GPIO_DRIVE_CAP_0: ~5 mA
     * - GPIO_DRIVE_CAP_1: ~10 mA
     * - GPIO_DRIVE_CAP_2: ~20 mA
     * - GPIO_DRIVE_CAP_3: ~40 mA

    gpio_set_drive_capability(GPIO_MOSI, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(GPIO_SCLK, GPIO_DRIVE_CAP_3);
    gpio_set_drive_capability(GPIO_CS, GPIO_DRIVE_CAP_3);

    **/

    //Initialize SPI slave interface
    ret = spi_slave_initialize(RCV_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
    assert(ret == ESP_OK);

    // For SPI DMA, buffers should be word-aligned.
    WORD_ALIGNED_ATTR uint8_t recvbuf[1024];

    while (1) {
        //Clear receive buffer, set send buffer to something sane
        memset(recvbuf, 0x00, 1024);

        //Set up a transaction of 1024 bytes to receive
        t.length = 1024 * 8;
        t.tx_buffer = NULL;
        t.rx_buffer = recvbuf;

        /* This call enables the SPI slave interface to receive to the recvbuf. The transaction is
         * initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
         * by pulling CS low and pulsing the clock etc.
         */
        ret = spi_slave_transmit(RCV_HOST, &t, portMAX_DELAY);

        /* Get the actual number of bytes received */
        int rcv_bytes = t.trans_len / 8;
        if (rcv_bytes == 0) {
            rcv_bytes = 1024; // Fallback if hardware didn't report exact length
        }

        /* Print the binary buffer in safe 32-byte chunks */
        for (int i = 0; i < rcv_bytes; i++) {
            // Print the raw byte (ignoring null-terminators)
            printf("%02X ", recvbuf[i]);

            // Break the line to prevent ESP-IDF terminal truncation
            if ((i + 1) % 32 == 0 || i == rcv_bytes - 1) {
                printf("\n");
            }
        }

        n++;
    }

}
