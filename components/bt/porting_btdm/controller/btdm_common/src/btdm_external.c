/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>
#include <stdio.h>
#include "btdm_external.h"

extern const int8_t *bt_bb_get_tx_pwr_table(uint8_t *length);
extern const int8_t *bt_bb_tx_pwr_table_get(uint8_t *length, uint8_t modem_cfg);

const int8_t *
wr_btdm_external_bb_get_tx_pwr_table(uint8_t *length, uint8_t modem_cfg)
{
    assert(length != NULL);
    assert (modem_cfg <= 2);
    // TODO: replace with bt_bb_tx_pwr_table_get when all the targets(h4, s31, etc) support this API
    if (modem_cfg == 2) { // BREDR
        return bt_bb_tx_pwr_table_get(length, modem_cfg);
    } else {
        return bt_bb_get_tx_pwr_table(length);
    }
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
