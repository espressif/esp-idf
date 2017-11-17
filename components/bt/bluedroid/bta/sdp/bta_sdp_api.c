/******************************************************************************
 *
 *  Copyright (C) 2014 The Android Open Source Project
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
 *  This is the implementation of the API for SDP search subsystem
 *
 ******************************************************************************/

#include "bt_target.h"
#include "bta_api.h"
#include "bta_sys.h"
#include "bta_sdp_api.h"
#include "bta_sdp_int.h"
#include <string.h>
#include "allocator.h"
#include "sdp_api.h"

#if defined(BTA_SDP_INCLUDED) && (BTA_SDP_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_sdp_reg = {
    bta_sdp_sm_execute,
    NULL
};

/*******************************************************************************
**
** Function         BTA_SdpEnable
**
** Description      Enable the SDP search I/F service. When the enable
**                  operation is complete the callback function will be
**                  called with a BTA_SDP_ENABLE_EVT. This function must
**                  be called before other functions in the SDP search API are
**                  called.
**
** Returns          BTA_SDP_SUCCESS if successful.
**                  BTA_SDP_FAIL if internal failure.
**
*******************************************************************************/
tBTA_SDP_STATUS BTA_SdpEnable(tBTA_SDP_DM_CBACK *p_cback)
{
    tBTA_SDP_STATUS status = BTA_SDP_FAILURE;
    tBTA_SDP_API_ENABLE  *p_buf;

    APPL_TRACE_API("%s\n", __FUNCTION__);
    if (p_cback && FALSE == bta_sys_is_register(BTA_ID_SDP)) {
        memset(&bta_sdp_cb, 0, sizeof(tBTA_SDP_CB));

        /* register with BTA system manager */
        bta_sys_register(BTA_ID_SDP, &bta_sdp_reg);

        if (p_cback &&
                (p_buf = (tBTA_SDP_API_ENABLE *) osi_malloc(sizeof(tBTA_SDP_API_ENABLE))) != NULL) {
            p_buf->hdr.event = BTA_SDP_API_ENABLE_EVT;
            p_buf->p_cback = p_cback;
            bta_sys_sendmsg(p_buf);
            status = BTA_SDP_SUCCESS;
        }
    }
    return (status);
}

/*******************************************************************************
**
** Function         BTA_SdpSearch
**
** Description      This function performs service discovery for a specific service
**                  on given peer device. When the operation is completed
**                  the tBTA_SDP_DM_CBACK callback function will be  called with
**                  a BTA_SDP_SEARCH_COMPLETE_EVT.
**
** Returns          BTA_SDP_SUCCESS, if the request is being processed.
**                  BTA_SDP_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_SDP_STATUS BTA_SdpSearch(BD_ADDR bd_addr, tSDP_UUID *uuid)
{
    tBTA_SDP_STATUS ret = BTA_SDP_FAILURE;
    tBTA_SDP_API_SEARCH *p_msg;

    APPL_TRACE_API("%s\n", __FUNCTION__);
    if ((p_msg = (tBTA_SDP_API_SEARCH *)osi_malloc(sizeof(tBTA_SDP_API_SEARCH))) != NULL) {
        p_msg->hdr.event = BTA_SDP_API_SEARCH_EVT;
        bdcpy(p_msg->bd_addr, bd_addr);
        //p_msg->uuid = uuid;
        memcpy(&(p_msg->uuid), uuid, sizeof(tSDP_UUID));
        bta_sys_sendmsg(p_msg);
        ret = BTA_SDP_SUCCESS;
    }

    return (ret);
}

/*******************************************************************************
**
** Function         BTA_SdpCreateRecordByUser
**
** Description      This function is used to request a callback to create a SDP
**                  record. The registered callback will be called with event
**                  BTA_SDP_CREATE_RECORD_USER_EVT.
**
** Returns          BTA_SDP_SUCCESS, if the request is being processed.
**                  BTA_SDP_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_SDP_STATUS BTA_SdpCreateRecordByUser(void *user_data)
{
    tBTA_SDP_STATUS ret = BTA_SDP_FAILURE;
    tBTA_SDP_API_RECORD_USER *p_msg;

    APPL_TRACE_API("%s\n", __FUNCTION__);
    if ((p_msg = (tBTA_SDP_API_RECORD_USER *)osi_malloc(sizeof(tBTA_SDP_API_RECORD_USER))) != NULL) {
        p_msg->hdr.event = BTA_SDP_API_CREATE_RECORD_USER_EVT;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        ret = BTA_SDP_SUCCESS;
    }

    return (ret);
}

/*******************************************************************************
**
** Function         BTA_SdpRemoveRecordByUser
**
** Description      This function is used to request a callback to remove a SDP
**                  record. The registered callback will be called with event
**                  BTA_SDP_REMOVE_RECORD_USER_EVT.
**
** Returns          BTA_SDP_SUCCESS, if the request is being processed.
**                  BTA_SDP_FAILURE, otherwise.
**
*******************************************************************************/
tBTA_SDP_STATUS BTA_SdpRemoveRecordByUser(void *user_data)
{
    tBTA_SDP_STATUS ret = BTA_SDP_FAILURE;
    tBTA_SDP_API_RECORD_USER *p_msg;

    APPL_TRACE_API("%s\n", __FUNCTION__);
    if ((p_msg = (tBTA_SDP_API_RECORD_USER *)osi_malloc(sizeof(tBTA_SDP_API_RECORD_USER))) != NULL) {
        p_msg->hdr.event = BTA_SDP_API_REMOVE_RECORD_USER_EVT;
        p_msg->user_data = user_data;
        bta_sys_sendmsg(p_msg);
        ret = BTA_SDP_SUCCESS;
    }

    return (ret);
}


#endif /* #if defined(BTA_SDP_INCLUDED) && (BTA_SDP_INCLUDED == TRUE) */
