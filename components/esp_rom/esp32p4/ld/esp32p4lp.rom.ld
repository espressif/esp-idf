/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4lp.rom.ld for esp32p4lp
 *
 *
 * Generated from ./target/esp32p4lp/interface-esp32p4lp.yml md5sum 0751c805e21bd23f11b74dcc1f7e8007
 *
 * Compatible with ROM where ECO version equal or greater to 0.
 *
 * THIS FILE WAS AUTOMATICALLY GENERATED. DO NOT EDIT.
 */

/***************************************
 Group common
 ***************************************/

/* Functions */
rtc_get_reset_reason = 0x50100098;
rtc_get_wakeup_cause = 0x5010009c;
ets_printf = 0x501000a0;
ets_install_putc1 = 0x501000a4;
ets_install_uart_printf = 0x501000a8;
ets_delay_us = 0x501000ac;
uart_tx_one_char = 0x501000b0;
uart_tx_flush = 0x501000b4;
uart_tx_wait_idle = 0x501000b8;
uartAttach = 0x501000bc;
Uart_Init = 0x501000c0;
GetUartDevice = 0x501000c4;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
crc32_le = 0x501000c8;
crc16_le = 0x501000cc;
crc8_le = 0x501000d0;
esp_crc8 = 0x501000d4;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
ets_intr_lock = 0x501000d8;
ets_intr_unlock = 0x501000dc;
PROVIDE( intr_handler_set = 0x501000e0 );
ets_isr_attach = 0x501000e4;
ets_isr_mask = 0x501000e8;
ets_isr_unmask = 0x501000ec;
