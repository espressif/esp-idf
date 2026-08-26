/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <string.h>
#include "esp_netif.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/uart_vfs.h"
#include "esp_console.h"
#include "linenoise/linenoise.h"

//
// Internal functions declaration referenced in io object
//
static esp_err_t netsuite_io_transmit(void *h, void *buffer, size_t len);
static esp_err_t netsuite_io_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf);
static esp_err_t netsuite_io_attach(esp_netif_t * esp_netif, void * args);

/**
 * @brief IO object netif related configuration with data-path function callbacks
 * and pointer to the IO object instance (unused as this is a singleton)
 */
const esp_netif_driver_ifconfig_t c_driver_ifconfig = {
        .driver_free_rx_buffer = NULL,
        .transmit = netsuite_io_transmit,
        .transmit_wrap = netsuite_io_transmit_wrap,
        .handle = "netsuite-io-object" // this IO object is a singleton, its handle uses as a name
};

/**
 * @brief IO object base structure used to point to internal attach function
 */
const esp_netif_driver_base_t s_driver_base = {
        .post_attach =  netsuite_io_attach
};

/**
 * @brief Transmit function called from esp_netif to output network stack data
 *
 * Note: This API has to conform to esp-netif transmit prototype
 *
 * @param h Opaque pointer representing the io driver (unused, const string in this case)
 * @param data data buffer
 * @param length length of data to send
 *
 * @return ESP_OK on success
 */
static esp_err_t netsuite_io_transmit(void *h, void *buffer, size_t len)
{
    /* output the packet to stdout */
    char *data = buffer;
    printf("\nPacketOut:[");
    for (size_t i=0; i<len; i++) {
        printf("%02x", *data++);
    }
    printf("]\n");
    return ESP_OK;
}

/**
 * @brief Transmit wrapper that is typically used for buffer handling and optimization.
 * Here just wraps the netsuite_io_transmit().
 *
 * @note The netstack_buf could be a ref-counted network stack buffer and might be used
 * by the lower layers directly if an additional handling is practical.
 * See docs on `esp_wifi_internal_tx_by_ref()` in components/esp_wifi/include/esp_private/wifi.h
 */
static esp_err_t netsuite_io_transmit_wrap(void *h, void *buffer, size_t len, void *netstack_buf)
{
    return netsuite_io_transmit(h, buffer, len);
}

/**
 * @brief Post attach adapter for netsuite i/o
 *
 * Used to exchange internal callbacks and context between esp-netif and the I/O object.
 * In case of netsuite I/O, it only updates the driver config with internal callbacks and
 * its instance pointer (const string in this case)
 *
 * @param esp_netif handle to esp-netif object
 * @param args pointer to netsuite IO
 *
 * @return ESP_OK on success
 */
static esp_err_t netsuite_io_attach(esp_netif_t * esp_netif, void * args)
{
    ESP_ERROR_CHECK(esp_netif_set_driver_config(esp_netif, &c_driver_ifconfig));
    return ESP_OK;
}

/**
 * @brief Process line read from serial input, character by character
 *
 * Converts from hex string to byte stream, so it can be processed
 * in test network interface
 *
 * @param line
 * @param packet
 *
 * @return size of packet
 */
static size_t process_line(char* line, char* packet)
{
    size_t count = 0;
    size_t i;

    for (i=0; i< strlen(line); i++) {
        char c = line[i];
        // accept both separators between bytes
        if (c == '-' || c == ' ') {
            ++count;
            // Processing numeric characters
        } else if (c >= '0' && c <= '9') {
            packet[count] *= 16;
            packet[count] += c - '0';
            // Processing alpha-numeric hex characters
        } else if (c >= 'a' && c <= 'f') {
            packet[count] *= 16;
            packet[count] += c - 'a' + 10;
        }
    }

    if (i>0 && strlen(line)>0) {
        count++;
    }
    return count;
}

/**
 * Created (initializes) the i/o object and returns handle ready to be attached to the esp-netif
 */
void * netsuite_io_new(void)
{
    // Initialize VFS & UART so we can use std::cout/cin
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    /* Install UART driver for interrupt-driven reads and writes */
    ESP_ERROR_CHECK( uart_driver_install( (uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM,
                                          256, 0, 0, NULL, 0) );
    /* Tell VFS to use UART driver */
    uart_vfs_dev_use_driver(CONFIG_ESP_CONSOLE_UART_NUM);
    uart_vfs_dev_port_set_rx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CR);
    /* Move the caret to the beginning of the next line on '\n' */
    uart_vfs_dev_port_set_tx_line_endings(CONFIG_ESP_CONSOLE_UART_NUM, ESP_LINE_ENDINGS_CRLF);
    linenoiseSetDumbMode(1);
    return (void *)&s_driver_base;
}

/**
 * I/O receive function
 */
ssize_t netsuite_io_get_packet(char *packet, size_t max_len)
{
    size_t size;

    /* read packet from stdin */
    char* line = linenoise("");
    if (!line) {
        return -1;
    }

    /* convert to binary */
    size = process_line(line, packet);

    if (size > max_len) {
        return -1;
    }

    linenoiseFree(line);

    return size;
}
