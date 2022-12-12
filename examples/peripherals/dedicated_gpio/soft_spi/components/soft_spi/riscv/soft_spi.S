/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */
    #include "sdkconfig.h"

    /* RISC-V fast GPIO special registers, taken from "hal/dedic_gpio_cpu_ll.h" */
    #define CSR_GPIO_IN_USER    0x804
    #define CSR_GPIO_OUT_USER   0x805

    .section .text

    /**
     * @brief Routine to emulate SPI thanks to (fast) GPIOS.
     * All Clock, MOSI, Chip Select and MISO must have been assigned beforehand to a bit in the GPIO_OUT_USER
     *
     * @param a0 tx Bytes to send on MOSI
     * @param a1 rx Buffer to store the bytes received on MISO
     * @param a2 size Size of the transaction
     * @param a3 Offset of Clock I/O in the dedicated GPIO register
     * @param a4 Offset of MOSI I/O in the dedicated GPIO register
     * @param a5 Offset of CS I/O in the dedicated GPIO register
     * @param a6 Offset of MISO I/O in the dedicated GPIO register
     *
     * The C signature of this routine would be:
     * void asm_emulate_spi(const uint8_t* tx, uint8_t* rx, uint32_t size,
     *                      uint32_t clock_bit, uint32_t mosi_bit, uint32_t cs_bit, uint32_t miso_bit);
     */
    .global asm_emulate_spi_shift_byte
    .type asm_emulate_spi_shift_byte, @function
asm_emulate_spi_shift_byte:
    /* Convert the offset/bit to a mask for the clock and chip select */
    li t0, 1
    sll a5, t0, a5
    sll a3, t0, a3
    /* Move a0 (TX) to t6 as we will use a0 as a parameter and a return value for subroutines */
    mv t6, a0
    /* Start by asserting chip select (bit in a5), setting it to 0 */
    csrrc zero, CSR_GPIO_OUT_USER, a5
    /* Reading the bytes one by one, this is less efficient but simpler. */
spi_process_byte:
    lb a0, (t6)
    /* Output the byte while reading MISO line. Byte read on MISO is returned in a0.
     * No need to use `call` as the subroutine is only called from here. */
    j spi_send_receive_byte
spi_send_receive_return:
    /* Store the resulted byte in RX buffer if not NULL */
    beqz a1, spi_no_store
    sb a0, (a1)
    /* Point to the next byte in both RX and TX buffers */
    addi a1, a1, 1
spi_no_store:
    addi t6, t6, 1
    /* Decrement the size and continue if not zero */
    addi a2, a2, -1
    bnez a2, spi_process_byte
    /* Wait an additional delay and set Chip select to high again. */
    li t6, ((CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ * 25)/100)
spi_wait:
    add t6, t6, -1
    bnez t6, spi_wait
    csrrs zero, CSR_GPIO_OUT_USER, a5
    ret

    /**
     * In theory, we would need to respect the calling convention but as this routine is private and
     * won't interact with any C function, we don't need to respect it, so we can only use registers,
     * and not the stack.
     * It is possible to optimize the speed even more by unrolling the loop inside the routine. Of course,
     * this will make the routine much bigger.
     *
     * @param a0 Byte to send on the bus
     * @param a3 Bitmask of Clock I/O in the dedicated GPIO register
     * @param a4 Offset of MOSI I/O in the dedicated GPIO register
     * @param a6 Offset of MISO I/O in the dedicated GPIO register
     *
     * The C signature of this routine would be:
     * uint8_t spi_send_receive_byte(uint8_t byte, uint32_t clock_bitmask, uint32_t mosi_bit, uint32_t miso_bit);
     */
spi_send_receive_byte:
    /* Byte received on MISO will be stored in t3 */
    li t3, 0
    /* Save the iterations count in t4 */
    li t4, 0
    /* Maximum iteration count in t5. Pre-define here to make the loop faster. */
    li t5, 8
    /* Save MOSI bitmask in t0 */
    li t0, 1
    sll t0, t0, a4
    /* Byte to send in a0, registers a1, a2, a3, a4, a5, a6 should not be altered.
     * Get the value of a0's the lowest bit, which will be set to MOSI */
    andi t1, a0, 1
    sll t1, t1, a4
spi_send_receive_bit:
    /* Two steps to output the value of a0 lowest bit:
     * - Use csrrs to set the pin, if value is 1
     * - Use csrrc (below) to set the pin, if value is 0
     */
    csrrs zero, CSR_GPIO_OUT_USER, t1
    /* Flip the data bit thanks to MOSI bitmask */
    xor t1, t1, t0
    /* Set Clock pin to 0. Thus, we need to set its bit in t1, use its bitmask for this. */
    xor t1, t1, a3
    csrrc zero, CSR_GPIO_OUT_USER, t1
    /* We have some time here as we need to delay between the clock pulses, while making the time
     * after clock is set to high again smaller. Let's prepare the next bit from a0 and decrement
     * the iteration count */
    srli a0, a0, 1
    andi t1, a0, 1
    sll t1, t1, a4
    /* Add some delay to have a duty cycle approaching the 50% */
    .rept 8
    nop
    .endr
    /* Set Clock to 1, without modifying the other bits */
    csrrs zero, CSR_GPIO_OUT_USER, a3
    /* Sample data coming on the MISO line */
    csrr t2, CSR_GPIO_IN_USER
    /* Extract the incoming bit and put it in bit 0 */
    srl t2, t2, a6
    andi t2, t2, 1
    /* Put it in t3 now, at the right place (t4-th bit) */
    sll t2, t2, t4
    or t3, t3, t2
    /* Check if there as bits remaining */
    addi t4, t4, 1
    bne t4, t5, spi_send_receive_bit
    /* Set return value in a0 */
    mv a0, t3
    j spi_send_receive_return
