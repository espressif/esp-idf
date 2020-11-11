/******************************************************************************
 *
 *  Copyright (C) 2014  Broadcom Corporation
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

#include <string.h>
#include "common/bt_target.h"

#if (BLE_INCLUDED == TRUE)
#include "stack/bt_types.h"
#include "stack/hcimsgs.h"
#include "stack/btu.h"
#include "btm_int.h"
//#include "bt_utils.h"
#include "stack/hcidefs.h"
#include "stack/btm_ble_api.h"

tBTM_BLE_ENERGY_INFO_CB ble_energy_info_cb;

/*******************************************************************************
**
** Function         btm_ble_cont_energy_cmpl_cback
**
** Description      Controller VSC complete callback
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
void btm_ble_cont_energy_cmpl_cback (tBTM_VSC_CMPL *p_params)
{
    UINT8  *p = p_params->p_param_buf;
    UINT16  len = p_params->param_len;
    UINT8  status = 0;
    UINT32 total_tx_time = 0, total_rx_time = 0, total_idle_time = 0, total_energy_used = 0;

    if (len < 17) {
        BTM_TRACE_ERROR("wrong length for btm_ble_cont_energy_cmpl_cback");
        return;
    }

    STREAM_TO_UINT8(status, p);
    STREAM_TO_UINT32(total_tx_time, p);
    STREAM_TO_UINT32(total_rx_time, p);
    STREAM_TO_UINT32(total_idle_time, p);
    STREAM_TO_UINT32(total_energy_used, p);

    BTM_TRACE_DEBUG("energy_info status=%d,tx_t=%u, rx_t=%u, ener_used=%u, idle_t=%u",
                    status, total_tx_time, total_rx_time, total_energy_used, total_idle_time);

    if (NULL != ble_energy_info_cb.p_ener_cback) {
        ble_energy_info_cb.p_ener_cback(total_tx_time, total_rx_time, total_idle_time,
                                        total_energy_used, status);
    }

    return;
}

/*******************************************************************************
**
** Function         BTM_BleGetEnergyInfo
**
** Description      This function obtains the energy info
**
** Parameters      p_ener_cback - Callback pointer
**
** Returns          status
**
*******************************************************************************/
tBTM_STATUS BTM_BleGetEnergyInfo(tBTM_BLE_ENERGY_INFO_CBACK *p_ener_cback)
{
    tBTM_STATUS status = BTM_ILLEGAL_VALUE;
    tBTM_BLE_VSC_CB cmn_ble_vsc_cb;

    BTM_BleGetVendorCapabilities(&cmn_ble_vsc_cb);

    BTM_TRACE_EVENT("BTM_BleGetEnergyInfo\n");

    if (0 == cmn_ble_vsc_cb.energy_support) {
        BTM_TRACE_ERROR("Controller does not support get energy info\n");
        return BTM_ERR_PROCESSING;
    }

    ble_energy_info_cb.p_ener_cback = p_ener_cback;
    if ((status = BTM_VendorSpecificCommand (HCI_BLE_ENERGY_INFO_OCF, 0, NULL,
                  btm_ble_cont_energy_cmpl_cback)) != BTM_CMD_STARTED) {
        BTM_TRACE_ERROR("BTM_BleGetEnergyInfo status: %d", status);
        return BTM_ILLEGAL_VALUE;
    }

    return status;
}

#endif
