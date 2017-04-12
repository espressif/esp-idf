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
 *****************************************************************************/

/******************************************************************************
 *
 *  This file contains the GATT server main functions and state machine.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE)

#include <string.h>

#include "bta_gatts_int.h"
#include "gki.h"

/* type for service building action functions */
typedef void (*tBTA_GATTS_SRVC_ACT)(tBTA_GATTS_SRVC_CB *p_rcb, tBTA_GATTS_DATA *p_data);

/* service building action function list */
const tBTA_GATTS_SRVC_ACT bta_gatts_srvc_build_act[] = {
    bta_gatts_add_include_srvc,
    bta_gatts_add_char,
    bta_gatts_add_char_descr,
    bta_gatts_delete_service,
    bta_gatts_start_service,
    bta_gatts_stop_service,
};

/* GATTS control block */
#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_GATTS_CB  bta_gatts_cb;
#endif

/*******************************************************************************
**
** Function         bta_gatts_hdl_event
**
** Description      BTA GATT server main event handling function.
**
**
** Returns          void
**
*******************************************************************************/
BOOLEAN bta_gatts_hdl_event(BT_HDR *p_msg)
{
    tBTA_GATTS_CB *p_cb = &bta_gatts_cb;
    tBTA_GATTS_SRVC_CB *p_srvc_cb = NULL;

    switch (p_msg->event) {
    case BTA_GATTS_API_DISABLE_EVT:
        bta_gatts_api_disable(p_cb);
        break;

    case BTA_GATTS_API_REG_EVT:
        bta_gatts_register(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_INT_START_IF_EVT:
        bta_gatts_start_if(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_DEREG_EVT:
        bta_gatts_deregister(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_CREATE_SRVC_EVT:
        bta_gatts_create_srvc(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_INDICATION_EVT:
        bta_gatts_indicate_handle(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_OPEN_EVT:
        bta_gatts_open(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_CANCEL_OPEN_EVT:
        bta_gatts_cancel_open(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_CLOSE_EVT:
        bta_gatts_close(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;

    case BTA_GATTS_API_RSP_EVT:
        bta_gatts_send_rsp(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;
    case BTA_GATTS_API_SET_ATTR_VAL_EVT:{
        UINT16 attr_id = ((tBTA_GATTS_DATA *) p_msg)->api_set_val.hdr.layer_specific;
        p_srvc_cb = bta_gatts_find_srvc_cb_by_attr_id(p_cb, attr_id);
        bta_gatts_set_attr_value(p_srvc_cb, (tBTA_GATTS_DATA *) p_msg);
        break;
    }
    case BTA_GATTS_API_LISTEN_EVT:
        bta_gatts_listen(p_cb, (tBTA_GATTS_DATA *) p_msg);
        break;
    case BTA_GATTS_API_ADD_INCL_SRVC_EVT:
    case BTA_GATTS_API_ADD_CHAR_EVT:
    case BTA_GATTS_API_ADD_DESCR_EVT:
    case BTA_GATTS_API_DEL_SRVC_EVT:
    case BTA_GATTS_API_START_SRVC_EVT:
    case BTA_GATTS_API_STOP_SRVC_EVT:
        p_srvc_cb = bta_gatts_find_srvc_cb_by_srvc_id(p_cb,
                    ((tBTA_GATTS_DATA *)p_msg)->api_add_incl_srvc.hdr.layer_specific);

        if (p_srvc_cb != NULL) {
            bta_gatts_srvc_build_act[p_msg->event - BTA_GATTS_API_ADD_INCL_SRVC_EVT](p_srvc_cb, (tBTA_GATTS_DATA *) p_msg);
        } else {
            APPL_TRACE_ERROR("service not created\n");
        }
        break;
    default:
        break;
    }


    return (TRUE);
}

#endif /* BTA_GATT_INCLUDED */
