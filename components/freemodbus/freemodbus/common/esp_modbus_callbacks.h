/*
 * SPDX-FileCopyrightText: 2016-2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// Stack callback functions prototypes

#ifndef _ESP_MODBUS_CALLBACKS_H_
#define _ESP_MODBUS_CALLBACKS_H_

#include "mb.h"
#include "mb_m.h"

typedef eMBErrorCode (*reg_input_cb)(UCHAR*, USHORT, USHORT);
typedef eMBErrorCode (*reg_holding_cb)(UCHAR*, USHORT, USHORT, eMBRegisterMode);
typedef eMBErrorCode (*reg_coils_cb)(UCHAR*, USHORT, USHORT, eMBRegisterMode);
typedef eMBErrorCode (*reg_discrete_cb)(UCHAR*, USHORT, USHORT);

#endif /* _ESP_MODBUS_CALLBACKS_H_ */
