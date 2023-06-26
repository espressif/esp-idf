/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "Mockrmt_encoder.h"
#include "Mockrmt_common.h"
#include "Mockrmt_tx.h"
#include "Mockrmt_rx.h"

void app_main(void)
{
    rmt_channel_handle_t channel = 0;

    /*Test that mock functions exist*/
    rmt_new_bytes_encoder(NULL, NULL);
    rmt_new_rx_channel(NULL, NULL);
    rmt_del_channel(channel);
    rmt_new_tx_channel(NULL, NULL);
}
