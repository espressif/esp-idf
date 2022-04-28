/*
 * SPDX-FileCopyrightText: 2015-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 *
 * This file contains the action functions for gatts and gattc.
 *
 *
 ******************************************************************************/

#include "bta/bta_gatt_common.h"
#include "gatt_int.h"

void BTA_GATT_SetLocalMTU(uint16_t mtu)
{
    gatt_set_local_mtu(mtu);
}

uint16_t BTA_GATT_GetLocalMTU(void)
{
    return gatt_get_local_mtu();
}
