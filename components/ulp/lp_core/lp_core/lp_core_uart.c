/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "esp_err.h"
#include "ulp_lp_core_uart.h"
#include "ulp_lp_core_utils.h"
#include "soc/lp_uart_struct.h"
#include "hal/uart_hal.h"

#define LP_UART_ERR_INT_FLAG        (UART_INTR_PARITY_ERR | UART_INTR_FRAM_ERR)
#define LP_UART_TX_INT_FLAG         (UART_INTR_TX_DONE)
#define LP_UART_RX_INT_FLAG         (UART_INTR_RXFIFO_FULL | UART_INTR_RXFIFO_TOUT | UART_INTR_RXFIFO_OVF)
#define LP_UART_TOUT_THRESH_DEFAULT (10U)
#define LP_UART_FULL_THRESH_DEFAULT (10U)

/* LP UART HAL Context */
uart_hal_context_t hal = {
    .dev = (uart_dev_t *)UART_LL_GET_HW(LP_UART_NUM_0),
};

static esp_err_t lp_core_uart_check_timeout(uint32_t intr_mask, int32_t timeout, uint32_t *ticker)
{
    if (timeout > -1) {
        /* If the timeout value is not -1, delay for 1 CPU cycle and keep track of ticks */
        ulp_lp_core_delay_cycles(1);
        *ticker = *ticker + 1;
        if (*ticker >= timeout) {
            /* Disable and clear interrupt bits */
            uart_hal_disable_intr_mask(&hal, intr_mask);
            uart_hal_clr_intsts_mask(&hal, intr_mask);

            return ESP_ERR_TIMEOUT;
        }
    }

    return ESP_OK;
}

int lp_core_uart_tx_chars(uart_port_t lp_uart_num, const void *src, size_t size)
{
    (void)lp_uart_num;
    uint32_t tx_len = 0;

    /* Argument sanity check */
    if (!src) {
        /* Invalid input arguments */
        return -1;
    }

    /* Nothing to do if the length is 0 */
    if (size == 0) {
        return 0;
    }

    /* Write the data to the Tx FIFO */
    uart_hal_write_txfifo(&hal, src, size, &tx_len);

    /* Return the number of bytes written */
    return tx_len;
}

esp_err_t lp_core_uart_write_bytes(uart_port_t lp_uart_num, const void *src, size_t size, int32_t timeout)
{
    (void)lp_uart_num;

    /* Argument sanity check */
    if (!src) {
        /* Invalid input arguments */
        return ESP_ERR_INVALID_ARG;
    }

    /* Nothing to do if the length is 0 */
    if (size == 0) {
        return ESP_OK;
    }

    /* Enable the Tx done interrupt */
    uint32_t intr_mask = LP_UART_TX_INT_FLAG | LP_UART_ERR_INT_FLAG;
    uart_hal_clr_intsts_mask(&hal, intr_mask);
    uart_hal_ena_intr_mask(&hal, intr_mask);

    /* Transmit data */
    uint32_t tx_len;
    uint32_t bytes_sent = 0;
    int32_t remaining_bytes = size;
    esp_err_t ret = ESP_OK;
    uint32_t intr_status = 0;
    uint32_t to = 0;

    while (remaining_bytes > 0) {
        /* Write to the Tx FIFO */
        tx_len = 0;
        uart_hal_write_txfifo(&hal, src + bytes_sent, remaining_bytes, &tx_len);

        if (tx_len) {
            /* We have managed to write some data to the Tx FIFO. Check Tx interrupt status */
            while (1) {
                /* Fetch the interrupt status */
                intr_status = uart_hal_get_intsts_mask(&hal);
                if (intr_status & LP_UART_TX_INT_FLAG) {
                    /* Clear interrupt status and break */
                    uart_hal_clr_intsts_mask(&hal, intr_mask);
                    break;
                } else if ((intr_status & LP_UART_ERR_INT_FLAG)) {
                    /* Transaction error. Abort */
                    return ESP_FAIL;
                }

                /* Check for transaction timeout */
                ret = lp_core_uart_check_timeout(intr_mask, timeout, &to);
                if (ret == ESP_ERR_TIMEOUT) {
                    /* Timeout */
                    uart_hal_disable_intr_mask(&hal, intr_mask);
                    return ret;
                }
            }

            /* Update the byte counters */
            bytes_sent += tx_len;
            remaining_bytes -= tx_len;
        } else {
            /* Tx FIFO does not have empty slots. Check for transaction timeout */
            ret = lp_core_uart_check_timeout(intr_mask, timeout, &to);
            if (ret == ESP_ERR_TIMEOUT) {
                /* Timeout */
                uart_hal_disable_intr_mask(&hal, intr_mask);
                return ret;
            }
        }
    }

    /* Disable the Tx done interrupt */
    uart_hal_disable_intr_mask(&hal, intr_mask);

    return ret;
}

int lp_core_uart_read_bytes(uart_port_t lp_uart_num, void *buf, size_t size, int32_t timeout)
{
    (void)lp_uart_num;

    /* Argument sanity check */
    if (!buf) {
        /* Invalid input arguments */
        return -1;
    }

    /* Nothing to do if the length is 0 */
    if (size == 0) {
        return 0;
    }

    /* Set the Rx interrupt thresholds */
    uart_hal_set_rx_timeout(&hal, LP_UART_TOUT_THRESH_DEFAULT);
    uart_hal_set_rxfifo_full_thr(&hal, LP_UART_FULL_THRESH_DEFAULT);

    /* Enable the Rx interrupts */
    uint32_t intr_mask = LP_UART_RX_INT_FLAG | LP_UART_ERR_INT_FLAG;
    uart_hal_clr_intsts_mask(&hal, intr_mask);
    uart_hal_ena_intr_mask(&hal, intr_mask);

    /* Receive data */
    int rx_len = 0;
    uint32_t bytes_rcvd = 0;
    int32_t remaining_bytes = size;
    esp_err_t ret = ESP_OK;
    uint32_t intr_status = 0;
    uint32_t to = 0;

    while (remaining_bytes > 0) {
        /* Read from the Rx FIFO
         * We set rx_len to -1 to read all bytes in the Rx FIFO
         */
        rx_len = -1;
        uart_hal_read_rxfifo(&hal, (uint8_t *)(buf + bytes_rcvd), &rx_len);

        if (rx_len) {
            /* We have some data to read from the Rx FIFO. Check Rx interrupt status */
            intr_status = uart_hal_get_intsts_mask(&hal);
            if ((intr_status & UART_INTR_RXFIFO_FULL) ||
                    (intr_status & UART_INTR_RXFIFO_TOUT)) {
                /* This is expected. Clear interrupt status and break */
                uart_hal_clr_intsts_mask(&hal, intr_mask);
                break;
            } else if ((intr_status & UART_INTR_RXFIFO_OVF)) {
                /* We reset the Rx FIFO if it overflows */
                uart_hal_clr_intsts_mask(&hal, intr_mask);
                uart_hal_rxfifo_rst(&hal);
                break;
            } else if ((intr_status & LP_UART_ERR_INT_FLAG)) {
                /* Transaction error. Abort */
                uart_hal_clr_intsts_mask(&hal, intr_mask);
                uart_hal_disable_intr_mask(&hal, intr_mask);
                return -1;
            }

            /* Update the byte counters */
            bytes_rcvd += rx_len;
            remaining_bytes -= rx_len;
        } else {
            /* We have no data to read from the Rx FIFO. Check for transaction timeout */
            ret = lp_core_uart_check_timeout(intr_mask, timeout, &to);
            if (ret == ESP_ERR_TIMEOUT) {
                break;
            }
        }
    }

    /* Disable the Rx interrupts */
    uart_hal_disable_intr_mask(&hal, intr_mask);

    /* Return the number of bytes received */
    return bytes_rcvd;
}
