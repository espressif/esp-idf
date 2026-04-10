/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include "btdm_external.h"

extern const int8_t *bt_bb_get_tx_pwr_table(uint8_t *length);

const int8_t *
wr_btdm_external_bb_get_tx_pwr_table(uint8_t *length)
{
    assert(length != NULL);
    return bt_bb_get_tx_pwr_table(length);
}

/*
 ***************************************************************************************************
 * BTDM External Function Initialization
 ***************************************************************************************************
 */
int
btdm_external_init(void)
{
    return 0;
}

void
btdm_external_deinit(void)
{

}
