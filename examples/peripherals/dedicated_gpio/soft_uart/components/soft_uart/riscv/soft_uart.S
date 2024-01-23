/*
 * SPDX-FileCopyrightText: 2010-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
    #include "sdkconfig.h"

    /* RISC-V fast GPIO special registers, taken from "hal/dedic_gpio_cpu_ll.h" */
    #define CSR_GPIO_IN_USER    0x804
    #define CSR_GPIO_OUT_USER   0x805
    /* Special register for machine cycle count */
    #define CSR_PCCR_MACHINE    0x7e2

    .section .text

    /**
    * @brief Send bytes on the emulated UART.
    *
    * @param tx_buffer (a0) Buffer to send on the TX line. Guaranteed not NULL by the caller.
    * @param tx_size (a1) Size of tx_buffer. Guaranteed not 0 by the caller.
    * @param tx_bit (a2) Offset of TX I/O in the dedicated GPIO register.
    * @param baudrate (a3) CPU clock cycles taken by each bit.
    *
    * The C signature of this routine would be:
    * void emulate_uart_send(const uint8_t* tx, uint32_t tx_size, uint32_t tx_bit, uint32_t baudrate_delay);
    */
    .global emulate_uart_send
    .type emulate_uart_send, @function
emulate_uart_send:
    /* "Convert" tx_bit to an actual mask. Thus, use 1 << tx_bit instead.
     * rx_bit is not modified as we need the bit offset controlling the RX I/O and not a bit mask. */
    li t0, 1
    sll a2, t0, a2
    /* Save return address in a4 */
    mv a4, ra
    /* Reading the characters 4 by 4 would be much faster, but in our case, we don't need
     * the process to be fast as the bottleneck is the UART speed */
uart_read_next:
    lb t0, (a0)
    /* Output the next character on the TX line */
    call uart_send_byte
    /* Go to the next character and repeat */
    addi a0, a0, 1
    addi a1, a1, -1
    /* If we don't have more bytes to send, return */
    bnez a1, uart_read_next
uart_ret:
    /* Restore the return address */
    mv ra, a4
    ret

    /**
     * In theory, we would need to respect the calling convention and receive the parameter
     * in a0, but as this routine is private and won't interact with any C function, we don't need
     * to respect it, so we can only use registers, and not the stack.
     *
     * The C signature of this routine would be:
     * void uart_send_byte(uint8_t byte, uint32_t tx_bitmask, uint32_t baudrate_delay);
     */
uart_send_byte:
    /* a0, a1, a3, a4 are used by the caller.
     * Parameters:
     *  t0 - Character to send
     *  a2 - Bit mask of GPIO_OUT_USER controlling TX
     *  a3 - Delay to wait between each bit
     */
    mv t1, ra
    /* Setup t3 to as we will send all 8 bits of the parameter (t0) */
    li t3, 8
    /* Start bit, clear/reset TX bit */
    csrrc zero, CSR_GPIO_OUT_USER, a2
    /* Wait a bit, depends on the baudrate configured */
    call uart_delay
uart_send_byte_loop:
    /* Get the lowest bit of t0 (parameter), store the result in t2 */
    andi t2, t0, 1
    /* We could avoid using a branch, but writing a 0 or 1 would have different timings.
     * Using branches, we can arrange the code to have roughly the same timings in both cases. */
    beqz t2, uart_send_bit_zero
    /* The following will set the GPIO pointed by the lowest bit to 1 */
    csrrs zero, CSR_GPIO_OUT_USER, a2
    j uart_send_bit_after
uart_send_bit_zero:
    /* If the bit was 0, we have to "clear" the GPIO */
    csrrc zero, CSR_GPIO_OUT_USER, a2
uart_send_bit_after:
    call uart_delay
    /* Shift the parameter right */
    srli t0, t0, 1
    /* Decrement the loop index and continue if needed */
    addi t3, t3, -1
    bnez t3, uart_send_byte_loop
    /* Stop bit, set bit to 1 */
    csrrs zero, CSR_GPIO_OUT_USER, a2
    call uart_delay
    /* Restore return address before returning */
    mv ra, t1
    ret

    /**
    * @brief Receive bytes from the emulated UART.
    *
    * @param rx_buffer (a0) Buffer to store the received bytes in. Guaranteed not NULL by the caller.
    * @param rx_size (a1) Size of rx_buffer. Guaranteed not 0 by the caller.
    * @param rx_bit (a2) Offset of RX I/O in the dedicated GPIO register.
    * @param baudrate (a3) CPU clock cycles taken by each bit.
    *
    * The C signature of this routine would be:
    * void emulate_uart_receive(uint8_t *rx_buffer, uint32_t tx_size, uint32_t rx_bit, uint32_t baudrate_delay);
    */
    .global emulate_uart_receive
    .type emulate_uart_receive, @function
emulate_uart_receive:
    /* Save return address in a4 */
    mv a4, ra
uart_receive_iterate:
    /* Receive characters on RX line now */
    call uart_receive_char
    /* Character received in a5. Store it in the buffer */
    sb a5, (a0)
    addi a0, a0, 1
    /* Decrement the size */
    addi a1, a1, -1
    /* Iterate until we don't have space in the buffer */
    bnez a1, uart_receive_iterate
    /* Restore the return address */
    mv ra, a4
    ret

    /* Routine to receive a character from the RX line and return it in a0.
     * For the same reasons as above, we can use temporary registers.
     *
     * The C signature of this routine would be:
     * uint8_t uart_receive_char(uint32_t rx_bit, uint32_t baudrate_delay);
     */
uart_receive_char:
    /* a0, a1, a3, a4 are used by the caller.
     * Parameters:
     *  a2 - Bit offset of GPIO_OUT_USER controlling RX. For example, 0 if RX is mapped to BIT(0).
     *  a3 - Delay (CPU cycles) to wait between each bit.
     */
    mv t1, ra
    li a5, 0
uart_receive_wait:
    /* Wait for the start bit. The input GPIO is bound to the lowest bit of CSR_GPIO_IN_USER */
    csrr t0, CSR_GPIO_IN_USER
    sra t0, t0, a2
    andi t0, t0, 1
    /* Check that the input pin is 0 (start bit) */
    bnez t0, uart_receive_wait
    /* t2 will go from 0 to 7 as we will receive 8 bits */
    li t2, 0
    /* Start bit arrived, wait a bit:
     * Wait half a UART-bit period here, the rest when we enter the loop, this will let us
     * sample the bits in the middle of the period */
    srli t6, a3, 1
    call uart_delay_t6
uart_receive_next_bit:
    call uart_delay
    /* Read the next bit of RX */
    csrr t0, CSR_GPIO_IN_USER
    sra t0, t0, a2
    andi t0, t0, 1
    /* Add the bit we've just received to a5 */
    sll t0, t0, t2
    add a5, a5, t0
    /* Check if we have received all the bits */
    addi t2, t2, 1
    li t0, 8
    bne t0, t2, uart_receive_next_bit
    /* We have received all the bits, we have to wait for the stop bit, in theory.
     * In practice, just wait and return */
    call uart_delay
    /* Restore return address that was saved in t1 */
    mv ra, t1
    ret


    /* Routine to wait few microseconds. The delay depends on the baudrate configured. */
uart_delay:
     /* Default baudrate to wait in a3 register */
    mv t6, a3

    /* Specify a delay, in machine cycles, to wait */
uart_delay_t6:
    /* t4, t5, t6 are available.
     * Use t4 to store the "end" point to wait.
     * Use t5 to get the current machine cycle. */
    csrr t4, CSR_PCCR_MACHINE
    /* In a real use case, we would need to check for a potential overflow,
     * In this example, there should be any issue. */
    add t4, t4, t6
uart_delay_loop:
    csrr t5, CSR_PCCR_MACHINE
    bltu t5, t4, uart_delay_loop
    ret
