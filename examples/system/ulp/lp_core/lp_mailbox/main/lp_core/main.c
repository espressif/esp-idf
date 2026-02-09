/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include "ulp_lp_core_print.h"
#include "ulp_lp_core_utils.h"
#include "ulp_lp_core_mailbox.h"

static lp_mailbox_t mailbox;

#define VALUES_COUNT    10

int main (void)
{
    uint32_t sum = 0;
    /* No configuration for now */
    lp_core_mailbox_init(&mailbox, NULL);

    lp_message_t messages[VALUES_COUNT] = {
        2, 5, 7, 9, 11, 13, 17, 23, 27, 31,
    };

    /* Send the integers we want the HP core to calculate */
    for (int i = 0; i < VALUES_COUNT; i++) {
        esp_err_t ret = lp_core_mailbox_send(mailbox, messages[i], -1);
        if (ret != ESP_OK) {
            lp_core_printf("Error sending message\n");
            break;
        }
    }

    /* Receive the squared of each value, calculate the sum and send it to HP */
    for (int i = 0; i < VALUES_COUNT; i++) {
        esp_err_t ret = lp_core_mailbox_receive(mailbox, &messages[i], -1);
        if (ret != ESP_OK) {
            lp_core_printf("Error receiving message\n");
            break;
        }
        sum += messages[i];
    }

    /* Wait a few milliseconds and send the sum asynchronously, so we are not sure if anyone will receive it */
    ulp_lp_core_delay_us(10000);
    lp_core_mailbox_send_async(mailbox, sum);

    while(1) {
    }

    return 0;
}
