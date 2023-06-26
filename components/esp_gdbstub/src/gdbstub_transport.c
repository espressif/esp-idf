/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_gdbstub.h"
#include "esp_gdbstub_common.h"
#include "sdkconfig.h"
#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
#include "hal/usb_serial_jtag_ll.h"
#else
#include "hal/uart_ll.h"
#endif

#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

int esp_gdbstub_getchar(void)
{
    uint8_t c;
    // retry the read until we succeed
    while (usb_serial_jtag_ll_read_rxfifo(&c, 1) == 0) {
        ;
    }
    return c;
}

void esp_gdbstub_putchar(int c)
{
    uint8_t cc = c;
    // retry the write until we succeed
    while (usb_serial_jtag_ll_write_txfifo(&cc, 1) < 1) {
        ;
    }
}

void esp_gdbstub_flush(void)
{
    usb_serial_jtag_ll_txfifo_flush();
}

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
int esp_gdbstub_getfifo(void)
{
    return 0; // TODO: IDF-7264
}
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME

#else // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG

static uart_dev_t *gdb_uart = NULL;

static inline void esp_gdbstub_uart_init(void)
{
    if (gdb_uart != NULL) {
        return;
    }
    switch (CONFIG_ESP_CONSOLE_UART_NUM) {
    case 0:
        gdb_uart = &UART0;
        break;
#if SOC_UART_HP_NUM > 1
    case 1:
        gdb_uart = &UART1;
        break;
#endif
#if SOC_UART_HP_NUM > 2
    case 2:
        gdb_uart = &UART2;
        break;
#endif
    default:
        gdb_uart = &UART0;
        break;
    }
}

int esp_gdbstub_getchar(void)
{
    esp_gdbstub_uart_init();
    unsigned char data;
    while (uart_ll_get_rxfifo_len(gdb_uart) == 0) {
        ;
    }
    uart_ll_read_rxfifo(gdb_uart, &data, 1);
    return data;
}

void esp_gdbstub_putchar(int c)
{
    esp_gdbstub_uart_init();
    while (uart_ll_get_txfifo_len(gdb_uart) <= 126) {
        ;
    }
    uart_ll_write_txfifo(gdb_uart, (uint8_t *)&c, 1);
}

void esp_gdbstub_flush(void)
{
    esp_gdbstub_uart_init();
    // wait until some data in transmition
    while (false == uart_ll_is_tx_idle(gdb_uart)) {
        ;
    }
}

#ifdef CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
int esp_gdbstub_getfifo(void)
{
    esp_gdbstub_uart_init();
    int doDebug = 0;

    int fifolen = uart_ll_get_rxfifo_len(gdb_uart);
    while (fifolen != 0) {
        unsigned char data;
        uart_ll_read_rxfifo(gdb_uart, &data, 1);
        if (data == 0x3) {
            doDebug = 1; // Check if any of the chars is Ctrl+C. Throw away rest.
        }
        fifolen--;
    }
    uart_ll_clr_intsts_mask(gdb_uart, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
    return doDebug;
}
#endif // CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME
#endif // CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
