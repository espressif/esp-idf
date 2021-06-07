/*
 * SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "soc/uart_periph.h"
#include "soc/gpio_periph.h"
#include "esp_gdbstub_common.h"
#include "sdkconfig.h"
#include "hal/uart_ll.h"
#include "freertos/FreeRTOS.h"
#include "xtensa/config/specreg.h"

#define UART_NUM CONFIG_ESP_CONSOLE_UART_NUM

static uart_dev_t *gdb_uart = NULL;


void esp_gdbstub_target_init(void)
{
    switch (UART_NUM) {
    case 0:
        gdb_uart = &UART0;
        break;
#if SOC_UART_NUM > 1
    case 1:
        gdb_uart = &UART1;
        break;
#endif
#if SOC_UART_NUM > 2
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
    if (gdb_uart == NULL) {
        esp_gdbstub_target_init();
    }
    unsigned char data;
    while (uart_ll_get_rxfifo_len(gdb_uart) == 0) {

    }
    uart_ll_read_rxfifo(gdb_uart, &data, 1);
    return data;
}

void esp_gdbstub_putchar(int c)
{
    if (gdb_uart == NULL) {
        esp_gdbstub_target_init();
    }
    while (uart_ll_get_txfifo_len(gdb_uart) <= 126) {
    }
    uart_ll_write_txfifo(gdb_uart, (uint8_t *)&c, 1);
}

void esp_gdbstub_flush()
{
    // wait until some data in transmition
    while (false == uart_ll_is_tx_idle(gdb_uart))
    {
    }
}

int esp_gdbstub_getfifo()
{
    if (gdb_uart == NULL) {
        esp_gdbstub_target_init();
    }
    int doDebug = 0;

    int fifolen = uart_ll_get_rxfifo_len(gdb_uart);
    while (fifolen != 0) {
        unsigned char data;
        uart_ll_read_rxfifo(gdb_uart, &data, 1);
        if (data == 0x3) {
            doDebug = 1; //Check if any of the chars is Ctrl+C. Throw away rest.
        }
        fifolen--;
    }
    uart_ll_clr_intsts_mask(gdb_uart, UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT);
    return doDebug;
}

int esp_gdbstub_readmem(intptr_t addr)
{
    if (addr < 0x20000000 || addr >= 0x80000000) {
        /* see cpu_configure_region_protection */
        return -1;
    }
    uint32_t val_aligned = *(uint32_t *)(addr & (~3));
    uint32_t shift = (addr & 3) * 8;
    return (val_aligned >> shift) & 0xff;
}

int esp_gdbstub_writemem(unsigned int addr, unsigned char data)
{
    if (addr < 0x20000000 || addr >= 0x80000000) {
        /* see cpu_configure_region_protection */
        return -1;
    }

    int *i = (int *)(addr & (~3));
    if ((addr & 3) == 0) {
        *i = (*i & 0xffffff00) | (data << 0);
    }
    if ((addr & 3) == 1) {
        *i = (*i & 0xffff00ff) | (data << 8);
    }
    if ((addr & 3) == 2) {
        *i = (*i & 0xff00ffff) | (data << 16);
    }
    if ((addr & 3) == 3) {
        *i = (*i & 0x00ffffff) | (data << 24);
    }
    asm volatile("ISYNC\nISYNC\n");

    return 0;
}
