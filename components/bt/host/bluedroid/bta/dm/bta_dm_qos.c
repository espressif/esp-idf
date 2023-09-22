/******************************************************************************
 *
 *  Copyright (C) 2003-2012 Broadcom Corporation
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
 *  This file contains the action functions for QoS state
 *  machine.
 *
 ******************************************************************************/

#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta_dm_int.h"
#include "stack/btm_api.h"
#include "osi/allocator.h"

#if (BTA_DM_QOS_INCLUDED == TRUE)

void bta_dm_set_qos(tBTA_DM_MSG *p_data)
{
    FLOW_SPEC p_flow = {
        .qos_flags = 0,                             /* TBD */
        .service_type = NO_TRAFFIC,                 /* service_type */
        .token_rate = 0,                            /* bytes/second */
        .token_bucket_size = 0,                     /* bytes */
        .peak_bandwidth = 0,                        /* bytes/second */
        .latency = 625 * p_data->qos_set.t_poll,    /* microseconds */
        .delay_variation = 0xFFFFFFFF               /* microseconds */
    };

    tBTM_STATUS status = BTM_SetQoS (p_data->qos_set.bd_addr, &p_flow, p_data->qos_set.p_cb);

    if(status != BTM_CMD_STARTED) {
        APPL_TRACE_ERROR("%s ERROR: 0x%x\n", __func__, status);
    }
}


void BTA_DmSetQos(BD_ADDR bd_addr, UINT32 t_poll, tBTM_CMPL_CB *p_cb)
{
    tBTA_DM_API_QOS_SET *p_msg;

    if ((p_msg = (tBTA_DM_API_QOS_SET *) osi_malloc(sizeof(tBTA_DM_API_QOS_SET))) != NULL) {
        p_msg->hdr.event = BTA_DM_API_QOS_SET_EVT;

        bdcpy(p_msg->bd_addr, bd_addr);
        p_msg->t_poll = t_poll;
        p_msg->p_cb = p_cb;

        bta_sys_sendmsg(p_msg);
    }
}
#endif /// (BTA_DM_QOS_INCLUDED == TRUE)
