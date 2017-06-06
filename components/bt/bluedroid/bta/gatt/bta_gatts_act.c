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
 *  This file contains the GATT Server action functions for the state
 *  machine.
 *
 ******************************************************************************/


#include "bt_target.h"

#if defined(GATTS_INCLUDED) && (GATTS_INCLUDED == TRUE)

#include "utl.h"
#include "gki.h"
#include "bta_sys.h"
#include "bta_gatts_int.h"
#include "bta_gatts_co.h"
#include "btm_ble_api.h"
#include <string.h>

static void bta_gatts_nv_save_cback(BOOLEAN is_saved, tGATTS_HNDL_RANGE *p_hndl_range);
static BOOLEAN bta_gatts_nv_srv_chg_cback(tGATTS_SRV_CHG_CMD cmd, tGATTS_SRV_CHG_REQ *p_req,
        tGATTS_SRV_CHG_RSP *p_rsp);

static void bta_gatts_conn_cback (tGATT_IF gatt_if, BD_ADDR bda, UINT16 conn_id,
                                  BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                  tGATT_TRANSPORT transport);
static void bta_gatts_send_request_cback (UINT16 conn_id,
        UINT32 trans_id,
        tGATTS_REQ_TYPE req_type, tGATTS_DATA *p_data);
static void bta_gatts_cong_cback (UINT16 conn_id, BOOLEAN congested);

static tGATT_CBACK bta_gatts_cback = {
    bta_gatts_conn_cback,
    NULL,
    NULL,
    NULL,
    bta_gatts_send_request_cback,
    NULL,
    bta_gatts_cong_cback
};

tGATT_APPL_INFO bta_gatts_nv_cback = {
    bta_gatts_nv_save_cback,
    bta_gatts_nv_srv_chg_cback
};

/*******************************************************************************
**
** Function         bta_gatts_nv_save_cback
**
** Description      NV save callback function.
**
** Parameter        is_add: true is to add a handle range; otherwise is to delete.
** Returns          none.
**
*******************************************************************************/
static void bta_gatts_nv_save_cback(BOOLEAN is_add, tGATTS_HNDL_RANGE *p_hndl_range)
{
    bta_gatts_co_update_handle_range(is_add, (tBTA_GATTS_HNDL_RANGE *)p_hndl_range);
}


/*******************************************************************************
**
** Function         bta_gatts_nv_srv_chg_cback
**
** Description      NV save callback function.
**
** Parameter        is_add: true is to add a handle range; otherwise is to delete.
** Returns          none.
**
*******************************************************************************/
static BOOLEAN bta_gatts_nv_srv_chg_cback(tGATTS_SRV_CHG_CMD cmd,
        tGATTS_SRV_CHG_REQ *p_req, tGATTS_SRV_CHG_RSP *p_rsp)
{
    return bta_gatts_co_srv_chg((tBTA_GATTS_SRV_CHG_CMD) cmd,
                                (tBTA_GATTS_SRV_CHG_REQ *) p_req,
                                (tBTA_GATTS_SRV_CHG_RSP *) p_rsp);
}


/*******************************************************************************
**
** Function         bta_gatts_enable
**
** Description      enable BTA GATTS module.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_enable(tBTA_GATTS_CB *p_cb)
{
    UINT8 index = 0;
    tBTA_GATTS_HNDL_RANGE handle_range;

    if (p_cb->enabled) {
        APPL_TRACE_DEBUG("GATTS already enabled.");
    } else {
        memset(p_cb, 0, sizeof(tBTA_GATTS_CB));

        p_cb->enabled = TRUE;

        while ( bta_gatts_co_load_handle_range(index, &handle_range)) {
            GATTS_AddHandleRange((tGATTS_HNDL_RANGE *)&handle_range);
            memset(&handle_range, 0, sizeof(tGATTS_HNDL_RANGE));
            index++;
        }

        APPL_TRACE_DEBUG("bta_gatts_enable: num of handle range added=%d", index);

        if (!GATTS_NVRegister(&bta_gatts_nv_cback)) {
            APPL_TRACE_ERROR("BTA GATTS NV register failed.");
        }
    }
}

/*******************************************************************************
**
** Function         bta_gatts_api_disable
**
** Description      disable BTA GATTS module.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_api_disable(tBTA_GATTS_CB *p_cb)
{
    UINT8 i;

    if (p_cb->enabled) {
        for (i = 0; i < BTA_GATTS_MAX_APP_NUM; i ++) {
            if (p_cb->rcb[i].in_use) {
                GATT_Deregister(p_cb->rcb[i].gatt_if);
            }
        }
        memset(p_cb, 0, sizeof(tBTA_GATTS_CB));
    } else {
        APPL_TRACE_ERROR("GATTS not enabled");
    }
}

/*******************************************************************************
**
** Function         bta_gatts_register
**
** Description      register an application.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_register(tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_INT_START_IF  *p_buf;
    tBTA_GATTS               cb_data;
    tBTA_GATT_STATUS         status = BTA_GATT_OK;
    UINT8                    i, first_unuse = 0xff;

    if (p_cb->enabled == FALSE) {
        bta_gatts_enable(p_cb);
    }

    for (i = 0; i < BTA_GATTS_MAX_APP_NUM; i ++) {
        if (p_cb->rcb[i].in_use) {
            if (bta_gatts_uuid_compare(p_cb->rcb[i].app_uuid, p_msg->api_reg.app_uuid)) {
                APPL_TRACE_ERROR("application already registered.\n");
                status = BTA_GATT_DUP_REG;
                break;
            }
        }
    }

    if (status == BTA_GATT_OK) {
        for (i = 0; i < BTA_GATTS_MAX_APP_NUM; i ++) {
            if (first_unuse == 0xff && !p_cb->rcb[i].in_use) {
                first_unuse = i;
                break;
            }
        }

        cb_data.reg_oper.server_if = BTA_GATTS_INVALID_IF;
// btla-specific ++
        memcpy(&cb_data.reg_oper.uuid, &p_msg->api_reg.app_uuid, sizeof(tBT_UUID));
// btla-specific --
        if (first_unuse != 0xff) {
            APPL_TRACE_VERBOSE("register application first_unuse rcb_idx = %d", first_unuse);

            p_cb->rcb[first_unuse].in_use = TRUE;
            p_cb->rcb[first_unuse].p_cback = p_msg->api_reg.p_cback;
            memcpy(&p_cb->rcb[first_unuse].app_uuid, &p_msg->api_reg.app_uuid, sizeof(tBT_UUID));
            cb_data.reg_oper.server_if      =
                p_cb->rcb[first_unuse].gatt_if  =
                    GATT_Register(&p_msg->api_reg.app_uuid, &bta_gatts_cback);
            if ( !p_cb->rcb[first_unuse].gatt_if) {
                status = BTA_GATT_NO_RESOURCES;
            } else {
                if ((p_buf =
                            (tBTA_GATTS_INT_START_IF *) GKI_getbuf(sizeof(tBTA_GATTS_INT_START_IF))) != NULL) {
                    p_buf->hdr.event    = BTA_GATTS_INT_START_IF_EVT;
                    p_buf->server_if    = p_cb->rcb[first_unuse].gatt_if;

                    bta_sys_sendmsg(p_buf);
                } else {
                    status = BTA_GATT_NO_RESOURCES;
                    memset( &p_cb->rcb[first_unuse], 0 , sizeof(tBTA_GATTS_RCB));
                }
            }
        } else {
            status = BTA_GATT_NO_RESOURCES;
        }

    }
    cb_data.reg_oper.status = status;
    if (p_msg->api_reg.p_cback) {
        (*p_msg->api_reg.p_cback)(BTA_GATTS_REG_EVT, &cb_data);
    }
}


/*******************************************************************************
**
** Function         bta_gatts_start_if
**
** Description      start an application interface.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_start_if(tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    UNUSED(p_cb);

    if (bta_gatts_find_app_rcb_by_app_if(p_msg->int_start_if.server_if)) {
        GATT_StartIf(p_msg->int_start_if.server_if);
    } else {
        APPL_TRACE_ERROR("Unable to start app.: Unknown interface =%d",
                         p_msg->int_start_if.server_if );
    }
}
/*******************************************************************************
**
** Function         bta_gatts_deregister
**
** Description      deregister an application.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_deregister(tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    tBTA_GATTS_CBACK    *p_cback = NULL;
    UINT8               i;
    tBTA_GATTS          cb_data;

    cb_data.reg_oper.server_if = p_msg->api_dereg.server_if;
    cb_data.reg_oper.status = status;

    for (i = 0; i < BTA_GATTS_MAX_APP_NUM; i ++) {
        if (p_cb->rcb[i].in_use && p_cb->rcb[i].gatt_if == p_msg->api_dereg.server_if) {
            p_cback = p_cb->rcb[i].p_cback;
            status = BTA_GATT_OK;

            /* deregister the app */
            GATT_Deregister(p_cb->rcb[i].gatt_if);

            /* reset cb */
            memset(&p_cb->rcb[i], 0, sizeof(tBTA_GATTS_RCB));
            cb_data.reg_oper.status = status;
            break;
        }
    }

    if (p_cback) {
        (*p_cback)(BTA_GATTS_DEREG_EVT, &cb_data);
    } else {
        APPL_TRACE_ERROR("application not registered.");
    }
}
/*******************************************************************************
**
** Function         bta_gatts_create_srvc
**
** Description      action function to create a service.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_create_srvc(tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    UINT8               rcb_idx;
    tBTA_GATTS          cb_data;
    UINT8               srvc_idx;
    UINT16              service_id = 0;

    cb_data.create.status = BTA_GATT_ERROR;

    rcb_idx = bta_gatts_find_app_rcb_idx_by_app_if(p_cb, p_msg->api_create_svc.server_if);

    APPL_TRACE_DEBUG("create service rcb_idx = %d", rcb_idx);

    if (rcb_idx != BTA_GATTS_INVALID_APP) {
        if ((srvc_idx = bta_gatts_alloc_srvc_cb(p_cb, rcb_idx)) != BTA_GATTS_INVALID_APP) {
            /* create the service now */
            service_id = GATTS_CreateService (p_cb->rcb[rcb_idx].gatt_if,
                                              &p_msg->api_create_svc.service_uuid,
                                              p_msg->api_create_svc.inst,
                                              p_msg->api_create_svc.num_handle,
                                              p_msg->api_create_svc.is_pri);

            if (service_id != 0) {
                memcpy(&p_cb->srvc_cb[srvc_idx].service_uuid,
                       &p_msg->api_create_svc.service_uuid, sizeof(tBT_UUID));
                p_cb->srvc_cb[srvc_idx].service_id   = service_id;
                p_cb->srvc_cb[srvc_idx].inst_num     = p_msg->api_create_svc.inst;
                p_cb->srvc_cb[srvc_idx].idx          = srvc_idx;

                cb_data.create.status      = BTA_GATT_OK;
                cb_data.create.service_id  = service_id;
// btla-specific ++
                cb_data.create.is_primary  = p_msg->api_create_svc.is_pri;
// btla-specific --
                cb_data.create.server_if   = p_cb->rcb[rcb_idx].gatt_if;
            } else {
                cb_data.status  = BTA_GATT_ERROR;
                memset(&p_cb->srvc_cb[srvc_idx], 0, sizeof(tBTA_GATTS_SRVC_CB));
                APPL_TRACE_ERROR("service creation failed.");
            }
// btla-specific ++
            memcpy(&cb_data.create.uuid, &p_msg->api_create_svc.service_uuid, sizeof(tBT_UUID));
            cb_data.create.svc_instance = p_msg->api_create_svc.inst;
// btla-specific --
        }
        if (p_cb->rcb[rcb_idx].p_cback) {
            (* p_cb->rcb[rcb_idx].p_cback)(BTA_GATTS_CREATE_EVT, &cb_data);
        }
    } else { /* application not registered */
        APPL_TRACE_ERROR("Application not registered");
    }
}
/*******************************************************************************
**
** Function         bta_gatts_add_include_srvc
**
** Description      action function to add an included service.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_add_include_srvc(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB  *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    UINT16          attr_id = 0;
    tBTA_GATTS      cb_data;

    attr_id = GATTS_AddIncludeService(p_msg->api_add_incl_srvc.hdr.layer_specific,
                                      p_msg->api_add_incl_srvc.included_service_id);

    cb_data.add_result.server_if = p_rcb->gatt_if;
    cb_data.add_result.service_id = p_msg->api_add_incl_srvc.hdr.layer_specific;
    cb_data.add_result.attr_id = attr_id;

    if (attr_id) {
        cb_data.add_result.status = BTA_GATT_OK;
    } else {
        cb_data.add_result.status = BTA_GATT_ERROR;
    }

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_ADD_INCL_SRVC_EVT, &cb_data);
    }
}
/*******************************************************************************
**
** Function         bta_gatts_add_char
**
** Description      action function to add characteristic.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_add_char(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB  *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    UINT16          attr_id = 0;
    tBTA_GATTS      cb_data;

    tGATT_ATTR_VAL *p_attr_val = NULL; 
    tGATTS_ATTR_CONTROL *p_control = NULL;

    if(p_msg->api_add_char.attr_val.attr_max_len != 0){
        p_attr_val = &p_msg->api_add_char.attr_val;
    }

    if(p_msg->api_add_char.control.auto_rsp != 0){
        p_control = &p_msg->api_add_char.control;
    }


    attr_id = GATTS_AddCharacteristic(p_msg->api_add_char.hdr.layer_specific,
                                      &p_msg->api_add_char.char_uuid,
                                      p_msg->api_add_char.perm,
                                      p_msg->api_add_char.property, p_attr_val, p_control);
    cb_data.add_result.server_if = p_rcb->gatt_if;
    cb_data.add_result.service_id = p_msg->api_add_incl_srvc.hdr.layer_specific;
    cb_data.add_result.attr_id = attr_id;
// btla-specific ++
    memcpy(&cb_data.add_result.char_uuid, &p_msg->api_add_char.char_uuid, sizeof(tBT_UUID));
// btla-specific --

    if (attr_id) {
        cb_data.add_result.status = BTA_GATT_OK;
    } else {
        cb_data.add_result.status = BTA_GATT_ERROR;
    }
    if((p_attr_val != NULL) && (p_attr_val->attr_val != NULL)){
        GKI_freebuf(p_attr_val->attr_val);
    }

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_ADD_CHAR_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gatts_add_char_descr
**
** Description      action function to add characteristic descriptor.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_add_char_descr(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB  *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    UINT16          attr_id = 0;
    tBTA_GATTS      cb_data;
    tGATT_ATTR_VAL *p_attr_val = NULL;
    tGATTS_ATTR_CONTROL *p_control = NULL;

    if (p_msg->api_add_char_descr.attr_val.attr_max_len != 0) {
        p_attr_val = &p_msg->api_add_char_descr.attr_val;
    }

    if (p_msg->api_add_char_descr.control.auto_rsp != 0) {
        p_control = &p_msg->api_add_char_descr.control;
    }
    attr_id = GATTS_AddCharDescriptor(p_msg->api_add_char_descr.hdr.layer_specific,
                                      p_msg->api_add_char_descr.perm,
                                      &p_msg->api_add_char_descr.descr_uuid, p_attr_val,
                                      p_control);

    cb_data.add_result.server_if = p_rcb->gatt_if;
    cb_data.add_result.service_id = p_msg->api_add_incl_srvc.hdr.layer_specific;
    cb_data.add_result.attr_id = attr_id;
// btla-specific ++
    memcpy(&cb_data.add_result.char_uuid, &p_msg->api_add_char_descr.descr_uuid, sizeof(tBT_UUID));
// btla-specific --

    if (attr_id) {
        cb_data.add_result.status = BTA_GATT_OK;
    } else {
        cb_data.add_result.status = BTA_GATT_ERROR;
    }
    if((p_attr_val != NULL) && (p_attr_val->attr_val != NULL)){
        GKI_freebuf(p_attr_val->attr_val);
    }

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_ADD_CHAR_DESCR_EVT, &cb_data);
    }

}

/*******************************************************************************
**
** Function         bta_gatts_add_char_descr
**
** Description      action function to add characteristic descriptor.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_set_attr_value(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB  *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    UINT16          attr_id = 0;
    tBTA_GATTS      cb_data;
    tBTA_GATT_STATUS gatts_status;
    gatts_status = GATTS_SetAttributeValue(p_msg->api_add_char_descr.hdr.layer_specific,
                                           p_msg->api_set_val.length,
                                           p_msg->api_set_val.value);

    cb_data.attr_val.server_if = p_rcb->gatt_if;
    cb_data.attr_val.service_id = p_msg->api_set_val.hdr.layer_specific;
    cb_data.attr_val.attr_id = attr_id;
    cb_data.attr_val.status = gatts_status;

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_SET_ATTR_VAL_EVT, &cb_data);
    }
}

void bta_gatts_get_attr_value(UINT16 attr_handle, UINT16 *length, UINT8 **value)
{
    GATTS_GetAttributeValue(attr_handle, length, value);
}

/*******************************************************************************
**
** Function         bta_gatts_delete_service
**
** Description      action function to delete a service.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_delete_service(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    tBTA_GATTS      cb_data;

    cb_data.srvc_oper.server_if = p_rcb->gatt_if;
    cb_data.srvc_oper.service_id = p_msg->api_add_incl_srvc.hdr.layer_specific;

    if (GATTS_DeleteService(p_rcb->gatt_if,
                            &p_srvc_cb->service_uuid,
                            p_srvc_cb->inst_num)) {
        cb_data.srvc_oper.status = BTA_GATT_OK;
        memset(p_srvc_cb, 0, sizeof(tBTA_GATTS_SRVC_CB));
    } else {
        cb_data.srvc_oper.status = BTA_GATT_ERROR;
    }

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_DELELTE_EVT, &cb_data);
    }

}
/*******************************************************************************
**
** Function         bta_gatts_start_service
**
** Description      action function to start a service.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_start_service(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    tBTA_GATTS      cb_data;

    cb_data.srvc_oper.server_if = p_rcb->gatt_if;
    cb_data.srvc_oper.service_id = p_msg->api_add_incl_srvc.hdr.layer_specific;

    if (GATTS_StartService(p_rcb->gatt_if,
                           p_srvc_cb->service_id,
                           p_msg->api_start.transport) ==  GATT_SUCCESS) {
        APPL_TRACE_DEBUG("bta_gatts_start_service service_id= %d", p_srvc_cb->service_id);
        cb_data.srvc_oper.status = BTA_GATT_OK;
    } else {
        cb_data.srvc_oper.status = BTA_GATT_ERROR;
    }

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_START_EVT, &cb_data);
    }

}
/*******************************************************************************
**
** Function         bta_gatts_stop_service
**
** Description      action function to stop a service.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_stop_service(tBTA_GATTS_SRVC_CB *p_srvc_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB *p_rcb = &bta_gatts_cb.rcb[p_srvc_cb->rcb_idx];
    tBTA_GATTS      cb_data;
    UNUSED(p_msg);

    GATTS_StopService(p_srvc_cb->service_id);
    cb_data.srvc_oper.server_if = p_rcb->gatt_if;
    cb_data.srvc_oper.service_id = p_srvc_cb->service_id;
    cb_data.srvc_oper.status = BTA_GATT_OK;
    APPL_TRACE_ERROR("bta_gatts_stop_service service_id= %d", p_srvc_cb->service_id);

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_STOP_EVT, &cb_data);
    }

}
/*******************************************************************************
**
** Function         bta_gatts_send_rsp
**
** Description      GATTS send response.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_send_rsp (tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    UNUSED(p_cb);

    if (GATTS_SendRsp (p_msg->api_rsp.hdr.layer_specific,
                       p_msg->api_rsp.trans_id,
                       p_msg->api_rsp.status,
                       (tGATTS_RSP *)p_msg->api_rsp.p_rsp) != GATT_SUCCESS) {
        APPL_TRACE_ERROR("Sending response failed\n");
    }

}
/*******************************************************************************
**
** Function         bta_gatts_indicate_handle
**
** Description      GATTS send handle value indication or notification.
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_indicate_handle (tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_SRVC_CB  *p_srvc_cb;
    tBTA_GATTS_RCB      *p_rcb = NULL;
    tBTA_GATT_STATUS    status = BTA_GATT_ILLEGAL_PARAMETER;
    tGATT_IF            gatt_if;
    BD_ADDR             remote_bda;
    tBTA_TRANSPORT transport;
    tBTA_GATTS          cb_data;

    p_srvc_cb = bta_gatts_find_srvc_cb_by_attr_id (p_cb, p_msg->api_indicate.attr_id);

    if (p_srvc_cb ) {
        if (GATT_GetConnectionInfor(p_msg->api_indicate.hdr.layer_specific,
                                    &gatt_if, remote_bda, &transport)) {
            p_rcb = bta_gatts_find_app_rcb_by_app_if(gatt_if);

            if (p_msg->api_indicate.need_confirm)

                status = GATTS_HandleValueIndication (p_msg->api_indicate.hdr.layer_specific,
                                                      p_msg->api_indicate.attr_id,
                                                      p_msg->api_indicate.len,
                                                      p_msg->api_indicate.value);
            else
                status = GATTS_HandleValueNotification (p_msg->api_indicate.hdr.layer_specific,
                                                        p_msg->api_indicate.attr_id,
                                                        p_msg->api_indicate.len,
                                                        p_msg->api_indicate.value);

            /* if over BR_EDR, inform PM for mode change */
            if (transport == BTA_TRANSPORT_BR_EDR) {
                bta_sys_busy(BTA_ID_GATTS, BTA_ALL_APP_ID, remote_bda);
                bta_sys_idle(BTA_ID_GATTS, BTA_ALL_APP_ID, remote_bda);
            }
        } else {
            APPL_TRACE_ERROR("Unknown connection ID: %d fail sending notification",
                             p_msg->api_indicate.hdr.layer_specific);
        }

        if ((status != GATT_SUCCESS || !p_msg->api_indicate.need_confirm) &&
                p_rcb && p_cb->rcb[p_srvc_cb->rcb_idx].p_cback) {
            cb_data.req_data.status = status;
            cb_data.req_data.conn_id = p_msg->api_indicate.hdr.layer_specific;

            (*p_rcb->p_cback)(BTA_GATTS_CONF_EVT, &cb_data);
        }
    } else {
        APPL_TRACE_ERROR("Not an registered servce attribute ID: 0x%04x",
                         p_msg->api_indicate.attr_id);
    }
}


/*******************************************************************************
**
** Function         bta_gatts_open
**
** Description
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_open (tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB      *p_rcb = NULL;
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    UINT16              conn_id;
    tBTA_GATTS_OPEN    open;
    UNUSED(p_cb);

    if ((p_rcb = bta_gatts_find_app_rcb_by_app_if(p_msg->api_open.server_if)) != NULL) {
        /* should always get the connection ID */
        if (GATT_Connect(p_rcb->gatt_if, p_msg->api_open.remote_bda,
                         p_msg->api_open.is_direct, p_msg->api_open.transport)) {
            status = BTA_GATT_OK;

            if (GATT_GetConnIdIfConnected(p_rcb->gatt_if, p_msg->api_open.remote_bda,
                                          &conn_id, p_msg->api_open.transport)) {
                status = BTA_GATT_ALREADY_OPEN;
            }
        }
    } else {
        APPL_TRACE_ERROR("Inavlide server_if=%d", p_msg->api_open.server_if);
    }

    if (p_rcb && p_rcb->p_cback) {
        open.status = status;
        open.server_if = p_msg->api_open.server_if;
        (*p_rcb->p_cback)(BTA_GATTS_OPEN_EVT,  (tBTA_GATTS *)&open);
    }

}
/*******************************************************************************
**
** Function         bta_gatts_cancel_open
**
** Description
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_cancel_open (tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB      *p_rcb;
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    tBTA_GATTS_CANCEL_OPEN   cancel_open;
    UNUSED(p_cb);

    if ((p_rcb = bta_gatts_find_app_rcb_by_app_if(p_msg->api_cancel_open.server_if)) != NULL) {
        if (!GATT_CancelConnect(p_rcb->gatt_if, p_msg->api_cancel_open.remote_bda,
                                p_msg->api_cancel_open.is_direct)) {
            APPL_TRACE_ERROR("bta_gatts_cancel_open failed for open request");
        } else {
            status = BTA_GATT_OK;
        }
    } else {
        APPL_TRACE_ERROR("Inavlide server_if=%d", p_msg->api_cancel_open.server_if);
    }

    if (p_rcb && p_rcb->p_cback) {
        cancel_open.status = status;
        cancel_open.server_if = p_msg->api_cancel_open.server_if;
        (*p_rcb->p_cback)(BTA_GATTS_CANCEL_OPEN_EVT,  (tBTA_GATTS *)&cancel_open);

    }
}
/*******************************************************************************
**
** Function         bta_gatts_close
**
** Description
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_close (tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB     *p_rcb;
    tBTA_GATT_STATUS    status = BTA_GATT_ERROR;
    tGATT_IF            gatt_if;
    BD_ADDR             remote_bda;
    tBTA_GATT_TRANSPORT transport;
    tBTA_GATTS_CLOSE    close;
    UNUSED(p_cb);

    if (GATT_GetConnectionInfor(p_msg->hdr.layer_specific, &gatt_if, remote_bda, &transport)) {
        if (GATT_Disconnect(p_msg->hdr.layer_specific) != GATT_SUCCESS) {
            APPL_TRACE_ERROR("bta_gatts_close fail conn_id=%d", p_msg->hdr.layer_specific);
        } else {
            status = BTA_GATT_OK;
        }

        p_rcb = bta_gatts_find_app_rcb_by_app_if(gatt_if);

        if (p_rcb && p_rcb->p_cback) {
            if (transport == BTA_TRANSPORT_BR_EDR) {
                bta_sys_conn_close( BTA_ID_GATTS , BTA_ALL_APP_ID, remote_bda);
            }

            close.status = status;
            close.conn_id = p_msg->hdr.layer_specific;
            (*p_rcb->p_cback)(BTA_GATTS_CLOSE_EVT,  (tBTA_GATTS *)&close);
        }
    } else {
        APPL_TRACE_ERROR("Unknown connection ID: %d", p_msg->hdr.layer_specific);
    }

}
/*******************************************************************************
**
** Function         bta_gatts_listen
**
** Description      Start or stop listening for LE connection on a GATT server
**
** Returns          none.
**
*******************************************************************************/
void bta_gatts_listen(tBTA_GATTS_CB *p_cb, tBTA_GATTS_DATA *p_msg)
{
    tBTA_GATTS_RCB     *p_rcb = bta_gatts_find_app_rcb_by_app_if(p_msg->api_listen.server_if);
    tBTA_GATTS          cb_data;
    UNUSED(p_cb);

    cb_data.reg_oper.status = BTA_GATT_OK;
    cb_data.reg_oper.server_if = p_msg->api_listen.server_if;

    if (p_rcb == NULL) {
        APPL_TRACE_ERROR("Unknown GATTS application");
        return;
    }

    if (!GATT_Listen(p_msg->api_listen.server_if,
                     p_msg->api_listen.start,
                     p_msg->api_listen.remote_bda)) {
        cb_data.status = BTA_GATT_ERROR;
        APPL_TRACE_ERROR("bta_gatts_listen Listen failed");
    }

    if (p_rcb->p_cback) {
        (*p_rcb->p_cback)(BTA_GATTS_LISTEN_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gatts_request_cback
**
** Description      GATTS attribute request callback.
**
** Returns          none.
**
*******************************************************************************/
static void bta_gatts_send_request_cback (UINT16 conn_id,
        UINT32 trans_id,
        tGATTS_REQ_TYPE req_type, tGATTS_DATA *p_data)
{
    tBTA_GATTS          cb_data;
    tBTA_GATTS_RCB     *p_rcb;
    tGATT_IF            gatt_if;
    tBTA_GATT_TRANSPORT transport;

    memset(&cb_data, 0 , sizeof(tBTA_GATTS));

    if (GATT_GetConnectionInfor(conn_id, &gatt_if, cb_data.req_data.remote_bda, &transport)) {
        p_rcb = bta_gatts_find_app_rcb_by_app_if(gatt_if);

        APPL_TRACE_DEBUG ("bta_gatts_send_request_cback conn_id=%d trans_id=%d req_type=%d",
                          conn_id, trans_id, req_type);

        if (p_rcb && p_rcb->p_cback) {
            /* if over BR_EDR, inform PM for mode change */
            if (transport == BTA_TRANSPORT_BR_EDR) {
                bta_sys_busy(BTA_ID_GATTS, BTA_ALL_APP_ID, cb_data.req_data.remote_bda);
                bta_sys_idle(BTA_ID_GATTS, BTA_ALL_APP_ID, cb_data.req_data.remote_bda);
            }

            cb_data.req_data.conn_id    = conn_id;
            cb_data.req_data.trans_id   = trans_id;
            cb_data.req_data.p_data     = (tBTA_GATTS_REQ_DATA *)p_data;

            (*p_rcb->p_cback)(req_type,  &cb_data);
        } else {
            APPL_TRACE_ERROR("connection request on gatt_if[%d] is not interested", gatt_if);
        }
    } else {
        APPL_TRACE_ERROR("request received on unknown connection ID: %d", conn_id);
    }
}

/*******************************************************************************
**
** Function         bta_gatts_conn_cback
**
** Description      connection callback.
**
** Returns          none.
**
*******************************************************************************/
static void bta_gatts_conn_cback (tGATT_IF gatt_if, BD_ADDR bda, UINT16 conn_id,
                                  BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                  tGATT_TRANSPORT transport)
{
    tBTA_GATTS      cb_data;
    UINT8           evt = connected ? BTA_GATTS_CONNECT_EVT : BTA_GATTS_DISCONNECT_EVT;
    tBTA_GATTS_RCB  *p_reg;

    APPL_TRACE_DEBUG ("bta_gatts_conn_cback gatt_if=%d conn_id=%d connected=%d reason = 0x%04d",
                      gatt_if, conn_id, connected, reason);
    APPL_TRACE_DEBUG("bta_gatts_conn_cback  bda :%02x-%02x-%02x-%02x-%02x-%02x ",
                     bda[0],  bda[1], bda[2],  bda[3], bda[4],  bda[5]);

    bt_bdaddr_t bdaddr;
    bdcpy(bdaddr.address, bda);
    /*
    if (connected)
        btif_debug_conn_state(bdaddr, BTIF_DEBUG_CONNECTED, GATT_CONN_UNKNOWN);
    else
        btif_debug_conn_state(bdaddr, BTIF_DEBUG_DISCONNECTED, reason);
    */
    p_reg = bta_gatts_find_app_rcb_by_app_if(gatt_if);

    if (p_reg && p_reg->p_cback) {
        /* there is no RM for GATT */
        if (transport == BTA_TRANSPORT_BR_EDR) {
            if (connected) {
                bta_sys_conn_open(BTA_ID_GATTS, BTA_ALL_APP_ID, bda);
            } else {
                bta_sys_conn_close( BTA_ID_GATTS , BTA_ALL_APP_ID, bda);
            }
        }

        cb_data.conn.conn_id = conn_id;
        cb_data.conn.server_if = gatt_if;
        cb_data.conn.reason = reason;
        cb_data.conn.transport = transport;
        memcpy(cb_data.conn.remote_bda, bda, BD_ADDR_LEN);
        (*p_reg->p_cback)(evt, &cb_data);
    } else {
        APPL_TRACE_ERROR("bta_gatts_conn_cback server_if=%d not found", gatt_if);
    }
}

/*******************************************************************************
**
** Function         bta_gatts_cong_cback
**
** Description      congestion callback.
**
** Returns          none.
**
*******************************************************************************/
static void bta_gatts_cong_cback (UINT16 conn_id, BOOLEAN congested)
{
    tBTA_GATTS_RCB *p_rcb;
    tGATT_IF gatt_if;
    tBTA_GATT_TRANSPORT transport;
    tBTA_GATTS cb_data;

    if (GATT_GetConnectionInfor(conn_id, &gatt_if, cb_data.req_data.remote_bda, &transport)) {
        p_rcb = bta_gatts_find_app_rcb_by_app_if(gatt_if);

        if (p_rcb && p_rcb->p_cback) {
            cb_data.congest.conn_id = conn_id;
            cb_data.congest.congested = congested;

            (*p_rcb->p_cback)(BTA_GATTS_CONGEST_EVT, &cb_data);
        }
    }
}
#endif /* GATTS_INCLUDED */
