/*******************************************************************************
 *
 * Copyright(c) 2015,2016 Intel Corporation.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of Intel Corporation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************/

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void cdc_acm_device;
extern cdc_acm_device *uart_acm_dev;

#define ACM_BYTES_PER_TX  64

//ACM statuses are negative to distinguish from USB_DC_* status codes
#define ACM_STATUS_LINESTATE_CHANGED -1
#define ACM_STATUS_LINECODING_CHANGED -2
#define ACM_STATUS_TX -3
#define ACM_STATUS_RX -4

typedef void(*uart_irq_callback_t)(cdc_acm_device *dev, int status);

/**
 * @brief Get amount of received characters in buffer
 *
 * @returns character count
 */

int cdc_acm_rx_fifo_cnt(cdc_acm_device *dev);


/*
 * @brief Poll the device for input.
 *
 * @return -ENOTSUP Since underlying USB device controller always uses
 * interrupts, polled mode UART APIs are not implemented for the UART interface
 * exported by CDC ACM driver. Apps should use fifo_read API instead.
 */

int cdc_acm_poll_in(cdc_acm_device *dev, unsigned char *c);

/*
 * @brief Output a character in polled mode.
 *
 * The UART poll method for USB UART is simulated by waiting till
 * we get the next BULK In upcall from the USB device controller or 100 ms.
 *
 * @return the same character which is sent
 */
unsigned char cdc_acm_poll_out(cdc_acm_device *dev, unsigned char c);

/**
 * @brief Fill FIFO with data
 *
 * @param dev     CDC ACM device struct.
 * @param tx_data Data to transmit.
 * @param len     Number of bytes to send.
 *
 * @return Number of bytes sent.
 */
int cdc_acm_fifo_fill(cdc_acm_device *dev, const uint8_t *tx_data, int len);

/**
 * @brief Read data from FIFO
 *
 * @param dev     CDC ACM device struct.
 * @param rx_data Pointer to data container.
 * @param size    Container size.
 *
 * @return Number of bytes read.
 */
int cdc_acm_fifo_read(cdc_acm_device *dev, uint8_t *rx_data, const int size);

/**
 * @brief Enable TX interrupt
 *
 * @param dev CDC ACM device struct.
 *
 * @return N/A.
 */
void cdc_acm_irq_tx_enable(cdc_acm_device *dev);

/**
 * @brief Disable TX interrupt
 *
 * @param dev CDC ACM device struct.
 *
 * @return N/A.
 */
void cdc_acm_irq_tx_disable(cdc_acm_device *dev);

/**
 * @brief Check if Tx IRQ has been raised
 *
 * @param dev CDC ACM device struct.
 *
 * @return 1 if a Tx IRQ is pending, 0 otherwise.
 */
int cdc_acm_irq_tx_ready(cdc_acm_device *dev);

/**
 * @brief Enable RX interrupt
 *
 * @param dev CDC ACM device struct.
 *
 * @return N/A
 */
void cdc_acm_irq_rx_enable(cdc_acm_device *dev);

/**
 * @brief Disable RX interrupt
 *
 * @param dev CDC ACM device struct.
 *
 * @return N/A.
 */
void cdc_acm_irq_rx_disable(cdc_acm_device *dev);

/**
 * @brief Enable line state interrupt
 *
 * @param dev CDC ACM device struct.
 *
 * @return N/A.
 */
void cdc_acm_irq_state_enable(cdc_acm_device *dev);

/**
 * @brief Disable line state interrupt
 *
 * @param dev CDC ACM device struct.
 *
 * @return N/A.
 */
void cdc_acm_irq_state_disable(cdc_acm_device *dev);


/**
 * @brief Check if Rx IRQ has been raised
 *
 * @param dev CDC ACM device struct.
 *
 * @return 1 if an IRQ is ready, 0 otherwise.
 */
int cdc_acm_irq_rx_ready(cdc_acm_device *dev);

/**
 * @brief Check if Tx or Rx IRQ is pending
 *
 * @param dev CDC ACM device struct.
 *
 * @return 1 if a Tx or Rx IRQ is pending, 0 otherwise.
 */
int cdc_acm_irq_is_pending(cdc_acm_device *dev);

/**
 * @brief Set the callback function pointer for IRQ.
 *
 * @param dev CDC ACM device struct.
 * @param cb  Callback function pointer.
 *
 * @return N/A
 */
void cdc_acm_irq_callback_set(cdc_acm_device *dev, uart_irq_callback_t cb);

/**
 * @brief Manipulate line control for UART.
 *
 * @param dev CDC ACM device struct
 * @param ctrl The line control to be manipulated
 * @param val Value to set the line control
 *
 * @return 0 if successful, failed otherwise.
 */
int cdc_acm_line_ctrl_set(cdc_acm_device *dev, uint32_t ctrl, uint32_t val);

/**
 * @brief Manipulate line control for UART.
 *
 * @param dev CDC ACM device struct
 * @param ctrl The line control to be manipulated
 * @param val Value to set the line control
 *
 * @return 0 if successful, failed otherwise.
 */
int cdc_acm_line_ctrl_get(cdc_acm_device *dev, uint32_t ctrl, uint32_t *val);


/**
 * @brief Initialize UART channel
 *
 * This routine is called to reset the chip in a quiescent state.
 * It is assumed that this function is called only once per UART.
 *
 * @param mem_chunk Memory chunk to use for internal use
 * @param mem_chunk_size Size of the memory chunk in bytes
 *
 * @return dev or NULL
 */
cdc_acm_device *cdc_acm_init(void *mem_chunk, int mem_chunk_size);


/** Common line controls for UART.*/
#define LINE_CTRL_BAUD_RATE (1 << 0)
#define LINE_CTRL_RTS       (1 << 1)
#define LINE_CTRL_DTR       (1 << 2)
#define LINE_CTRL_DCD       (1 << 3)
#define LINE_CTRL_DSR       (1 << 4)

/* Common communication errors for UART.*/

/** @brief Overrun error */
#define UART_ERROR_OVERRUN  (1 << 0)

/** @brief Parity error */
#define UART_ERROR_PARITY   (1 << 1)

/** @brief Framing error */
#define UART_ERROR_FRAMING  (1 << 2)

/**
 * @brief Break interrupt error:
 *
 * A break interrupt was received. This happens when the serial input is
 * held at a logic '0' state for longer than the sum of start time + data bits
 * + parity + stop bits.
 */
#define UART_ERROR_BREAK    (1 << 3)

#ifdef __cplusplus
}
#endif
