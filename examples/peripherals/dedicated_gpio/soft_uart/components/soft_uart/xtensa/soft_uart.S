/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
    #include "sdkconfig.h"

    #if CONFIG_IDF_TARGET_ESP32S2

    .macro WRITE_GPIO_OUT mask value
        wr_mask_gpio_out \value, \mask
    .endm

    .macro READ_GPIO_IN reg
        get_gpio_in \reg
    .endm

    #else // CONFIG_IDF_TARGET_ESP32S3

    .macro WRITE_GPIO_OUT mask value
        ee.wr_mask_gpio_out \value, \mask
    .endm

    .macro READ_GPIO_IN reg
        ee.get_gpio_in \reg
    .endm

    #endif

    .section .text

    /**
    * @brief Send bytes on the emulated UART.
    *
    * @param tx_buffer (a2) Buffer to send on the TX line. Guaranteed not NULL by the caller.
    * @param tx_size (a3) Size of tx_buffer. Guaranteed not 0 by the caller.
    * @param tx_bit (a4) Offset of TX I/O in the dedicated GPIO register.
    * @param baudrate (a5) CPU clock cycles taken by each bit.
    *
    * The C signature of this routine would be:
    * void emulate_uart_send(const uint8_t* tx, uint32_t tx_size, uint32_t tx_bit, uint32_t baudrate_delay);
    */
    .global emulate_uart_send
    .type emulate_uart_send, @function
emulate_uart_send:
    entry a1, 16
    /* Convert tx_bit into a bitmask */
    ssl a4
    movi a4, 1
    sll a4, a4
    /* Set the line to high */
    movi a7, 0xff
    WRITE_GPIO_OUT a4, a7
    mov a10, a5
    /* By calling delay here, we guarantee that in the following code, the next register window will
     * be free, as such, there won't be any window overflow */
    call8 uart_delay
uart_send_loop:
    /* Start bit, clear/reset TX bit */
    movi a7, 0
    WRITE_GPIO_OUT a4, a7
    /* Wait for the given amount of CPU cycles */
    mov a10, a5
    call8 uart_delay
    /* Load the next byte and send it on the line */
    l8ui a6, a2, 0
    /* Use the upper bits of a6 to keep track of the remaining bits to send */
    movi a7, 0xff00
    or a6, a6, a7
uart_send_next_bit:
    /* Take the LSB of a6 */
    mov a7, a4
    bbci a6, 0, uart_send_bit_0
    j uart_send_bit_end
uart_send_bit_0:
    movi a7, 0
uart_send_bit_end:
    WRITE_GPIO_OUT a4, a7
    srli a6, a6, 1
    /* Check if we still have bits to send */
    movi a7, 0x100
    /* Wait for the given amount of CPU cycles */
    mov a10, a5
    call8 uart_delay
    bge a6, a7, uart_send_next_bit
    /* Increment the tx_buffer and continue */
    addi a2, a2, 1
    addi a3, a3, -1
    /* Stop bit, set TX bit */
    WRITE_GPIO_OUT a4, a4
    mov a10, a5
    call8 uart_delay
    bnez a3, uart_send_loop
    retw

    /* Register a2 contains the number of cycles to wait */
    .align 4
uart_delay:
    entry a1, 16
    rsr.ccount a3
    add a2, a2, a3
uart_delay_loop:
    rsr.ccount a3
    blt a3, a2, uart_delay_loop
    retw

    /**
    * @brief Receive bytes from the emulated UART.
    *
    * @param rx_buffer (a2) Buffer to store the received bytes in. Guaranteed not NULL by the caller.
    * @param rx_size (a3) Size of rx_buffer. Guaranteed not 0 by the caller.
    * @param rx_bit (a4) Offset of RX I/O in the dedicated GPIO register.
    * @param baudrate (a5) CPU clock cycles taken by each bit.
    *
    * The C signature of this routine would be:
    * void emulate_uart_receive(uint8_t *rx_buffer, uint32_t tx_size, uint32_t rx_bit, uint32_t baudrate_delay);
    */
    .global emulate_uart_receive
    .type emulate_uart_receive, @function
emulate_uart_receive:
    entry a1, 16

read_next_byte:
    /* a6 contains the current bit being received */
    movi a6, 0x1
    /* Wait for the start bit (0) */
wait_start_bit:
    READ_GPIO_IN a7
    bbs a7, a4, wait_start_bit
    /* a7 will now store the final byte */
    movi a7, 0
    /* Wait 1.5 baudrate cycle, this will let us read the next bit in the middle on its period */
    srli a10, a5, 1
    call8 uart_delay
read_next_bit:
    mov a10, a5
    call8 uart_delay
    /* Read the RX line and store the bit */
    READ_GPIO_IN a8
    bbc a8, a4, read_not_set_bit
    /* Write the bit 1 in the final byte */
    or a7, a7, a6
read_not_set_bit:
    slli a6, a6, 1
    movi a8, 0x100
    bne a6, a8, read_next_bit
    /* Save the received bit in the buffer */
    s8i a7, a2, 0
    addi a2, a2, 1
    /* Wait a baudrate period to make sure stop bit is being sent */
    mov a10, a5
    call8 uart_delay
    /* Check if we have received all the characters */
    addi a3, a3, -1
    bnez a3, read_next_byte
    retw
