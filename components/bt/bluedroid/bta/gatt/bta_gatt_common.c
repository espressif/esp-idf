/******************************************************************************
* Copyright 2015-2017 Espressif Systems (Shanghai) PTE LTD
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at

*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
 ******************************************************************************/

/******************************************************************************
 *
 * This file contains the action functions for gatts and gattc.
 *
 *
 ******************************************************************************/

#include "bta_gatt_common.h"
#include "gatt_int.h"

void BTA_GATT_SetLocalMTU(uint16_t mtu)
{
    gatt_set_local_mtu(mtu);
}