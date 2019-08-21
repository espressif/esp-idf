// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


/******************************************************************************
 *
 *  This is the interface file for BT GAP call-out functions.
 *
 ******************************************************************************/
#ifndef BTA_GAP_BT_CO_H
#define BTA_GAP_BT_CO_H

#if (BTC_GAP_BT_INCLUDED == TRUE)

extern void btc_gap_bt_config_eir_cmpl_callback (uint8_t status, uint8_t eir_type_num, uint8_t *eir_type);

#endif /// (BTC_GAP_BT_INCLUDED == TRUE)
#endif /// BTA_GAP_BT_CO_H