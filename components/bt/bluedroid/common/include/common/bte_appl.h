/******************************************************************************
 *
 *  Copyright (C) 2002-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  This is the interface file for the bte application task
 *
 ******************************************************************************/

#pragma once

typedef struct {
#if ((BLE_INCLUDED == TRUE) && (SMP_INCLUDED == TRUE))
    UINT8   ble_auth_req;
    UINT8   ble_io_cap;
    UINT8   ble_init_key;
    UINT8   ble_resp_key;
    UINT8   ble_max_key_size;
#endif

} tBTE_APPL_CFG;

extern tBTE_APPL_CFG bte_appl_cfg;


typedef struct {
#if ((CLASSIC_BT_INCLUDED == TRUE) && (BT_SSP_INCLUDED == TRUE))
    UINT8   bt_auth_req;
    UINT8   bt_io_cap;
    UINT8   *bt_oob_auth_data;
#endif
} tBTE_BT_APPL_CFG;

extern tBTE_BT_APPL_CFG bte_bt_appl_cfg;