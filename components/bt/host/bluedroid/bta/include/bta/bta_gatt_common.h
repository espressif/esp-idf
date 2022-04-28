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

#include "stack/bt_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

extern void BTA_GATT_SetLocalMTU(uint16_t mtu);

extern uint16_t BTA_GATT_GetLocalMTU(void);

#ifdef __cplusplus
}
#endif
