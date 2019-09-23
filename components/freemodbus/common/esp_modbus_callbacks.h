/* Copyright 2018 Espressif Systems (Shanghai) PTE LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
