/******************************************************************************
 *
 * Copyright (C) 2014 Samsung System LSI
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

/************************************************************************************
 *
 *  Filename:      btif_sdp.c
 *  Description:   SDP Bluetooth Interface.
 *                 Implements the generic message handling and search functionality.
 *                 References btif_sdp_server.c for SDP record creation.
 *
 ***********************************************************************************/

// #include <hardware/bluetooth.h>
#include "bt_sdp.h"
#include <stdlib.h>
#include <string.h>

#define LOG_TAG "BTIF_SDP"
#include "btif_common.h"
#include "btif_util.h"
// #include "btif_profile_queue.h"
#include "bta_api.h"
#include "bta_sdp_api.h"

/*****************************************************************************
**  Functions implemented in sdp_server.c
******************************************************************************/
bt_status_t sdp_server_init();
void sdp_server_cleanup();
bt_status_t create_sdp_record(bluetooth_sdp_record *records, int* record_handles);
bt_status_t remove_sdp_record(int record_handle);
void on_create_record_event(int handle);
void on_remove_record_event(int handle);

// Utility functions:
int get_sdp_records_size(bluetooth_sdp_record* in_record, int count);
void copy_sdp_records(bluetooth_sdp_record* in_records,
        bluetooth_sdp_record* out_records, int count);


/*****************************************************************************
**  Static variables
******************************************************************************/

static btsdp_callbacks_t *bt_sdp_callbacks = NULL;

static void btif_sdp_search_comp_evt(UINT16 event, char *p_param)
{
    tBTA_SDP_SEARCH_COMP *evt_data = (tBTA_SDP_SEARCH_COMP*) p_param;
    bt_bdaddr_t addr;
    BTIF_TRACE_DEBUG("%s:  event = %d\n", __FUNCTION__, event);

    if (event != BTA_SDP_SEARCH_COMP_EVT)
        return;

    bdcpy(addr.address, evt_data->remote_addr);

    HAL_CBACK(bt_sdp_callbacks, sdp_search_cb, evt_data->status,
            &addr, (uint8_t*)(evt_data->uuid.uu.uuid128),
            evt_data->record_count, evt_data->records);
}

static void sdp_search_comp_copy_cb(UINT16 event, char *p_dest, char *p_src)
{
    tBTA_SDP_SEARCH_COMP *p_dest_data =  (tBTA_SDP_SEARCH_COMP *) p_dest;
    tBTA_SDP_SEARCH_COMP *p_src_data =  (tBTA_SDP_SEARCH_COMP *) p_src;

    if (!p_src)
        return;

    if (event != BTA_SDP_SEARCH_COMP_EVT)
        return;

    memcpy(p_dest_data, p_src_data, sizeof(tBTA_SDP_SEARCH_COMP));

    copy_sdp_records(p_src_data->records, p_dest_data->records, p_src_data->record_count);
}

static void sdp_dm_cback(tBTA_SDP_EVT event, tBTA_SDP *p_data, void *user_data)
{
    switch (event)
    {
        case BTA_SDP_SEARCH_COMP_EVT:
        {
            int size = sizeof(tBTA_SDP);
            size += get_sdp_records_size(p_data->sdp_search_comp.records,
                    p_data->sdp_search_comp.record_count);

            /* need to deep copy the record content */
            btif_transfer_context(btif_sdp_search_comp_evt, event,
                                    (char*)p_data, size, sdp_search_comp_copy_cb);
            break;
        }
        case BTA_SDP_CREATE_RECORD_USER_EVT:
        {
            on_create_record_event((int)user_data);
            break;
        }
        case BTA_SDP_REMOVE_RECORD_USER_EVT:
        {
            on_remove_record_event((int)user_data);
            break;
        }
        default:
            break;
    }
}

bt_status_t API_BT_SdpInit(btsdp_callbacks_t *callbacks)
{
    BTIF_TRACE_DEBUG("Sdp Search %s\n", __FUNCTION__);

    bt_sdp_callbacks = callbacks;
    sdp_server_init();

    btif_enable_service(BTA_SDP_SERVICE_ID);

    return BT_STATUS_SUCCESS;
}

bt_status_t API_BT_SdpDeinit(void)
{
    BTIF_TRACE_DEBUG("Sdp Search %s\n", __FUNCTION__);

    bt_sdp_callbacks = NULL;
    sdp_server_cleanup();
    btif_disable_service(BTA_SDP_SERVICE_ID);

    return BT_STATUS_SUCCESS;
}


bt_status_t API_BT_SdpSearch(bt_bdaddr_t *bd_addr,  const uint8_t* uuid)
{
    bdstr_t bdstr;
    tSDP_UUID sdp_uuid;
    sdp_uuid.len = 16;
    memcpy(sdp_uuid.uu.uuid128, uuid, sizeof(sdp_uuid.uu.uuid128));

    BTA_SdpSearch(bd_addr->address, &sdp_uuid);

    return BT_STATUS_SUCCESS;
}

/*******************************************************************************
**
** Function         btif_sdp_execute_service
**
** Description      Initializes/Shuts down the service
**
** Returns          BT_STATUS_SUCCESS on success, BT_STATUS_FAIL otherwise
**
*******************************************************************************/
bt_status_t btif_sdp_execute_service(BOOLEAN b_enable)
{
    BTIF_TRACE_DEBUG("%s enable:%d\n", __FUNCTION__, b_enable);

     if (b_enable)
     {
         BTA_SdpEnable(sdp_dm_cback);
     }
     else
     {
         /* This is called on BT disable so no need to extra cleanup */
     }
     return BT_STATUS_SUCCESS;
}

