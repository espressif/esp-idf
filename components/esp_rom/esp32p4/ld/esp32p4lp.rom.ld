/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/* ROM function interface esp32p4lp.rom.ld for esp32p4lp
 *
 *
 * Generated from ./target/esp32p4lp/interface-esp32p4lp.yml md5sum f827caf806e9f4faec384ffbde44b380
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
pmu_enable_unhold_pads = 0x501000a0;
ets_printf = 0x501000a4;
ets_install_putc1 = 0x501000a8;
ets_install_uart_printf = 0x501000ac;
ets_delay_us = 0x501000b0;
uart_tx_one_char = 0x501000b4;
uart_tx_flush = 0x501000b8;
uart_tx_wait_idle = 0x501000bc;
uartAttach = 0x501000c0;
Uart_Init = 0x501000c4;
GetUartDevice = 0x501000c8;


/***************************************
 Group gpio
 ***************************************/

/* Functions */
gpio_set_output_level = 0x501000cc;
gpio_get_input_level = 0x501000d0;
gpio_matrix_in = 0x501000d4;
gpio_matrix_out = 0x501000d8;
gpio_bypass_matrix_in = 0x501000dc;
gpio_output_disable = 0x501000e0;
gpio_output_enable = 0x501000e4;
gpio_pad_input_disable = 0x501000e8;
gpio_pad_input_enable = 0x501000ec;
gpio_pad_pulldown = 0x501000f0;
gpio_pad_pullup = 0x501000f4;
gpio_pad_select_gpio = 0x501000f8;
gpio_pad_set_drv = 0x501000fc;
gpio_pad_unhold = 0x50100100;
gpio_pad_hold = 0x50100104;
gpio_lppad_select_mux = 0x50100108;


/***************************************
 Group crypto
 ***************************************/

/* Functions */
crc32_le = 0x5010010c;
crc16_le = 0x50100110;
crc8_le = 0x50100114;
esp_crc8 = 0x50100118;


/***************************************
 Group interrupts
 ***************************************/

/* Functions */
ets_intr_lock = 0x5010011c;
ets_intr_unlock = 0x50100120;
PROVIDE( intr_handler_set = 0x50100124 );
ets_isr_attach = 0x50100128;
ets_isr_mask = 0x5010012c;
ets_isr_unmask = 0x50100130;
