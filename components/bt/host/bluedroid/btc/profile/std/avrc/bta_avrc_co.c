/******************************************************************************
 *
 *  This is the AVRC call-out function implementation for  BTC.
 *
 ******************************************************************************/
// Copyright 2015-2018 Espressif Systems (Shanghai) PTE LTD
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

/*****************************************************************************
 *
 *  Filename:      bta_avrc_co.c
 *
 *  Description:   Bluetooth AVRC implementation
 *
 *****************************************************************************/

#include <stdint.h>
#include "common/bt_target.h"
#include "bta/bta_sys.h"
#include "bta/bta_av_api.h"
#include "btc_avrc.h"

#if BTC_AV_INCLUDED
/*******************************************************************************
 **
 ** Function         bta_avrc_co_cmd_allowed
 **
 ** Description      Check if local AVRCP TG configuration supports a specific
 **                  PASSTHROUGH command with the given operation_id
 **
 ** Returns          TRUE if operation_id is supported, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN bta_avrc_co_cmd_allowed(tBTA_AV_RC rc_id)
{
    if (rc_id >= BTA_AV_VENDOR) {
        return FALSE;
    }
    const uint16_t *rc_cmd = btc_avrc_tg_get_supported_command();
    if (rc_cmd[rc_id >> 4] & ((uint16_t)1 << (rc_id & 0x0F))) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/*******************************************************************************
 **
 ** Function         bta_avrc_co_rn_evt_cap
 **
 ** Description      get the event notifcation capabilities on AVRCP target
 **
 ** Returns          number of event_ids supported
 **
 *******************************************************************************/
UINT8 bta_avrc_co_rn_evt_cap(UINT8 *event_ids)
{
    if (event_ids == 0) {
        return 0;
    }

    UINT16 event_bits = btc_avrc_tg_get_rn_supported_evt();
    UINT8 count = 0;
    for (UINT8 i = 0; i < 16; ++i, event_bits >>= 1) {
        if (event_bits & 0x01) {
            event_ids[count++] = i;
        }
    }
    return count;
}

/*******************************************************************************
 **
 ** Function         bta_avrc_co_evt_supported
 **
 ** Description      Check if local AVRCP TG configuration supports the given
 **                  event_id
 **
 ** Returns          TRUE if operation_id is supported, FALSE otherwise
 **
 *******************************************************************************/
BOOLEAN bta_avrc_co_rn_evt_supported(UINT8 event_id)
{
    return btc_avrc_tg_rn_evt_supported(event_id) ?
           TRUE : FALSE;
}

/* the call out functions for AVRC */
tBTA_AVRC_CO_FUNCTS bta_avrc_cos = {
    bta_avrc_co_cmd_allowed,
    bta_avrc_co_rn_evt_cap,
    bta_avrc_co_rn_evt_supported,
};

#endif /* #if BTC_AV_INCLUDED */
