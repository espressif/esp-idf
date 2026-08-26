/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_uart.h"
#include "ulp_lp_core_utils.h"

#define LP_UART_PORT_NUM    LP_UART_NUM_0

int main (void)
{
    esp_err_t ret;
    uint8_t data[1024] = {0};
    int len = 0;

    while (1) {
        /* Read data from the LP_UART */
        len = lp_core_uart_read_bytes(LP_UART_PORT_NUM, data, (sizeof(data) - 1), 10);
        if (len > 0) {
            /* Write data back to the LP_UART */
            ret = lp_core_uart_write_bytes(LP_UART_PORT_NUM, (const char *)data, len, 500);
            if (ret != ESP_OK) {
                /* Error in writing. Bail */
                continue;
            }
        }
    }

    return 0;
}
