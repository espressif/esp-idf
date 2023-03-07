/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <inttypes.h>
#include "driver/uart.h"

/**
 * Function prototype for UART driver to ask for more data to send.
 * Returns -1 if no more data is available for TX.
 * Driver must call this with interrupts disabled.
 */
typedef int (*hci_uart_tx_char)(void *arg);

/**
 * Function prototype for UART driver to report that transmission is
 * complete. This should be called when transmission of last byte is
 * finished.
 * Driver must call this with interrupts disabled.
 */
typedef void (*hci_uart_tx_done)(void *arg);

/**
 * Function prototype for UART driver to report incoming byte of data.
 * Returns -1 if data was dropped.
 * Driver must call this with interrupts disabled.
 */
typedef int (*hci_uart_rx_char)(void *arg, uint8_t byte);


/**
 * Initializes given uart. Mapping of logical UART number to physical
 * UART/GPIO pins is in BSP.
 */
int hci_uart_init_cbs(int uart, hci_uart_tx_char tx_func,
                      hci_uart_tx_done tx_done, hci_uart_rx_char rx_func, void *arg);


/**
 * Applies given configuration to UART.
 *
 * @param port_num The UART number to configure
 * @param speed The baudrate in bps to configure
 * @param databits The number of databits to send per byte
 * @param stopbits The number of stop bits to send
 * @param parity The UART parity
 * @param flow_ctl Flow control settings on the UART
 *
 * @return 0 on success, non-zero error code on failure
 */
int hci_uart_config(int port_num, int32_t baud_rate, uint8_t data_bits, uint8_t stop_bits,
                    uart_parity_t parity, uart_hw_flowcontrol_t flow_ctl);

/**
 * Close UART port. Can call hal_uart_config() with different settings after
 * calling this.
 *
 * @param port_num The UART number to close
 */
int hci_uart_close(int port_num);

/**
 * More data queued for transmission. UART driver will start asking for that
 * data.
 *
 * @param port_num The UART number to start TX on
 */
void hci_uart_start_tx(int port_num);

/**
 * Upper layers have consumed some data, and are now ready to receive more.
 * This is meaningful after uart_rx_char callback has returned -1 telling
 * that no more data can be accepted.
 *
 * @param port_num The UART number to begin RX on
 */
void hci_uart_start_rx(int port_num);

/**
 * @brief reconfig hci uart pin
 *
 * @param tx_pin  The Tx pin
 * @param rx_pin  The Rx pin
 * @param cts_pin The CTS pin
 * @param rts_pin The RTS pin
 * @return int  0 on success, non-zero error code on failure
 */
int hci_uart_reconfig_pin(int tx_pin, int rx_pin, int cts_pin, int rts_pin);

#ifdef __cplusplus
}
#endif
