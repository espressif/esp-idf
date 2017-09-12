/******************************************************************************
 *
 *  Copyright (C) 2010-2012 Broadcom Corporation
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
 *  This is the implementation of the API for GATT server of BTA.
 *
 ******************************************************************************/

#include "bt_target.h"

#if defined(GATTS_INCLUDED) && (GATTS_INCLUDED == TRUE)

#include <string.h>
#include "bta_sys.h"
#include "bta_gatt_api.h"
#include "bta_gatts_int.h"
#include "allocator.h"

/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_gatts_reg = {
    bta_gatts_hdl_event,
    BTA_GATTS_Disable
};

/*******************************************************************************
**
** Function         BTA_GATTS_Disable
**
** Description      This function is called to disable GATTS module
**
** Parameters       None.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTS_Disable(void)
{
    BT_HDR  *p_buf;

    if (bta_sys_is_register(BTA_ID_GATTS) == FALSE) {
        APPL_TRACE_WARNING("GATTS Module not enabled/already disabled");
        return;
    }

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_GATTS_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
    bta_sys_deregister(BTA_ID_GATTS);

}

/*******************************************************************************
**
** Function         BTA_GATTS_AppRegister
**
** Description      This function is called to register application callbacks
**                    with BTA GATTS module.
**
** Parameters       p_app_uuid - applicaiton UUID
**                  p_cback - pointer to the application callback function.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTS_AppRegister(tBT_UUID *p_app_uuid, tBTA_GATTS_CBACK *p_cback)
{
    tBTA_GATTS_API_REG  *p_buf;

    /* register with BTA system manager */
    if (bta_sys_is_register(BTA_ID_GATTS) == FALSE) {
        bta_sys_register(BTA_ID_GATTS, &bta_gatts_reg);
    }

    if ((p_buf = (tBTA_GATTS_API_REG *) osi_malloc(sizeof(tBTA_GATTS_API_REG))) != NULL) {
        p_buf->hdr.event    = BTA_GATTS_API_REG_EVT;

        if (p_app_uuid != NULL) {
            memcpy(&p_buf->app_uuid, p_app_uuid, sizeof(tBT_UUID));
        }
        p_buf->p_cback      = p_cback;

        bta_sys_sendmsg(p_buf);
    }
    return;
}



/*******************************************************************************
**
** Function         BTA_GATTS_AppDeregister
**
** Description      De-register with GATT Server.
**
** Parameters       app_id: applicatino ID.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_AppDeregister(tBTA_GATTS_IF server_if)
{
    tBTA_GATTS_API_DEREG  *p_buf;

    if ((p_buf = (tBTA_GATTS_API_DEREG *) osi_malloc(sizeof(tBTA_GATTS_API_DEREG))) != NULL) {
        p_buf->hdr.event    = BTA_GATTS_API_DEREG_EVT;
        p_buf->server_if    = server_if;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTS_CreateService
**
** Description      Create a service. When service creation is done, a callback
**                  event BTA_GATTS_CREATE_SRVC_EVT is called to report status
**                  and service ID to the profile. The service ID obtained in
**                  the callback function needs to be used when adding included
**                  service and characteristics/descriptors into the service.
**
** Parameters       app_id: Profile ID this service is belonged to.
**                  p_service_uuid: service UUID.
**                  inst: instance ID number of this service.
**                  num_handle: numble of handle requessted for this service.
**                  is_primary: is this service a primary one or not.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_CreateService(tBTA_GATTS_IF server_if, tBT_UUID *p_service_uuid, UINT8 inst,
                             UINT16 num_handle, BOOLEAN is_primary)
{
    tBTA_GATTS_API_CREATE_SRVC  *p_buf;

    if ((p_buf = (tBTA_GATTS_API_CREATE_SRVC *) osi_malloc(sizeof(tBTA_GATTS_API_CREATE_SRVC))) != NULL) {
        p_buf->hdr.event = BTA_GATTS_API_CREATE_SRVC_EVT;

        p_buf->server_if = server_if;
        p_buf->inst = inst;
        memcpy(&p_buf->service_uuid, p_service_uuid, sizeof(tBT_UUID));
        p_buf->num_handle = num_handle;
        p_buf->is_pri = is_primary;

        bta_sys_sendmsg(p_buf);
    }
    return;
}
/*******************************************************************************
**
** Function         BTA_GATTS_AddIncludeService
**
** Description      This function is called to add an included service. After included
**                  service is included, a callback event BTA_GATTS_ADD_INCL_SRVC_EVT
**                  is reported the included service ID.
**
** Parameters       service_id: service ID to which this included service is to
**                              be added.
**                  included_service_id: the service ID to be included.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_AddIncludeService(UINT16 service_id, UINT16 included_service_id)
{
    tBTA_GATTS_API_ADD_INCL_SRVC  *p_buf;

    if ((p_buf =
                (tBTA_GATTS_API_ADD_INCL_SRVC *) osi_malloc(sizeof(tBTA_GATTS_API_ADD_INCL_SRVC)))
            != NULL) {
        p_buf->hdr.event = BTA_GATTS_API_ADD_INCL_SRVC_EVT;

        p_buf->hdr.layer_specific = service_id;
        p_buf->included_service_id = included_service_id;

        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTS_AddCharacteristic
**
** Description      This function is called to add a characteristic into a service.
**
** Parameters       service_id: service ID to which this included service is to
**                              be added.
**                  p_char_uuid : Characteristic UUID.
**                  perm      : Characteristic value declaration attribute permission.
**                  property  : Characteristic Properties
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTS_AddCharacteristic (UINT16 service_id,  tBT_UUID  *p_char_uuid,
                                  tBTA_GATT_PERM perm, tBTA_GATT_CHAR_PROP property, tGATT_ATTR_VAL *attr_val, 
                                  tBTA_GATTS_ATTR_CONTROL *control)
{
    tBTA_GATTS_API_ADD_CHAR  *p_buf;
    UINT16 len = 0;
    if(attr_val != NULL){
        len = attr_val->attr_len;
    }
    if ((p_buf = (tBTA_GATTS_API_ADD_CHAR *) osi_malloc(sizeof(tBTA_GATTS_API_ADD_CHAR))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTS_API_ADD_CHAR));

        p_buf->hdr.event = BTA_GATTS_API_ADD_CHAR_EVT;
        p_buf->hdr.layer_specific = service_id;
        p_buf->perm = perm;
        p_buf->property = property;
        if(control !=NULL){
            p_buf->control.auto_rsp = control->auto_rsp;
        }
        if(attr_val != NULL){
            APPL_TRACE_DEBUG("!!!!!!attr_val->attr_len = %x\n",attr_val->attr_len);
            APPL_TRACE_DEBUG("!!!!!!!attr_val->attr_max_len = %x\n",attr_val->attr_max_len);
            p_buf->attr_val.attr_len = attr_val->attr_len;
            p_buf->attr_val.attr_max_len = attr_val->attr_max_len;
            p_buf->attr_val.attr_val = (uint8_t *)osi_malloc(len);
            if(p_buf->attr_val.attr_val != NULL){
                memcpy(p_buf->attr_val.attr_val, attr_val->attr_val, attr_val->attr_len);
            }
        }

        if (p_char_uuid) {
            memcpy(&p_buf->char_uuid, p_char_uuid, sizeof(tBT_UUID));
        }
        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTS_AddCharDescriptor
**
** Description      This function is called to add characteristic descriptor. When
**                  it's done, a callback event BTA_GATTS_ADD_DESCR_EVT is called
**                  to report the status and an ID number for this descriptor.
**
** Parameters       service_id: service ID to which this charatceristic descriptor is to
**                              be added.
**                  perm: descriptor access permission.
**                  p_descr_uuid: descriptor UUID.
**
** Returns          returns status.
**
*******************************************************************************/
void BTA_GATTS_AddCharDescriptor (UINT16 service_id,
                                  tBTA_GATT_PERM perm,
                                  tBT_UUID   *p_descr_uuid, tBTA_GATT_ATTR_VAL *attr_val, 
                                  tBTA_GATTS_ATTR_CONTROL *control)
{
    tBTA_GATTS_API_ADD_DESCR  *p_buf;
    UINT16  value_len = 0;

    if ((p_buf = (tBTA_GATTS_API_ADD_DESCR *) osi_malloc(sizeof(tBTA_GATTS_API_ADD_DESCR))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTS_API_ADD_DESCR));

        p_buf->hdr.event = BTA_GATTS_API_ADD_DESCR_EVT;
        p_buf->hdr.layer_specific = service_id;
        p_buf->perm = perm;

        if(control != NULL){
            p_buf->control.auto_rsp = control->auto_rsp;
        }

        if (p_descr_uuid) {
            memcpy(&p_buf->descr_uuid, p_descr_uuid, sizeof(tBT_UUID));
        }

        if(attr_val != NULL){
            p_buf->attr_val.attr_len = attr_val->attr_len;
            p_buf->attr_val.attr_max_len = attr_val->attr_max_len;
            value_len =  attr_val->attr_len;
            if (value_len != 0){
                p_buf->attr_val.attr_val = (uint8_t*)osi_malloc(value_len);
                if(p_buf->attr_val.attr_val != NULL){
                    memcpy(p_buf->attr_val.attr_val, attr_val->attr_val, value_len);
                }
                else{
                    APPL_TRACE_ERROR("Allocate fail for %s\n", __func__);

                }
            }
        }

        bta_sys_sendmsg(p_buf);
    }
    return;

}

/*******************************************************************************
 **
 ** Function         BTA_GATTS_DeleteService
 **
 ** Description      This function is called to delete a service. When this is done,
 **                  a callback event BTA_GATTS_DELETE_EVT is report with the status.
 **
 ** Parameters       service_id: service_id to be deleted.
 **
** Returns          returns none.
**
*******************************************************************************/
void  BTA_GATTS_DeleteService(UINT16 service_id)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_GATTS_API_DEL_SRVC_EVT;

        p_buf->layer_specific = service_id;

        bta_sys_sendmsg(p_buf);
    }
    return;

}

/*******************************************************************************
**
** Function         BTA_GATTS_StartService
**
** Description      This function is called to start a service.
**
** Parameters       service_id: the service ID to be started.
**                  sup_transport: supported trasnport.
**
** Returns          None.
**
*******************************************************************************/
void  BTA_GATTS_StartService(UINT16 service_id, tBTA_GATT_TRANSPORT sup_transport)
{
    tBTA_GATTS_API_START  *p_buf;

    if ((p_buf = (tBTA_GATTS_API_START *) osi_malloc(sizeof(tBTA_GATTS_API_START))) != NULL) {
        p_buf->hdr.event = BTA_GATTS_API_START_SRVC_EVT;

        p_buf->hdr.layer_specific = service_id;
        p_buf->transport = sup_transport;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTS_StopService
**
** Description      This function is called to stop a service.
**
** Parameters       service_id - service to be topped.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTS_StopService(UINT16 service_id)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_GATTS_API_STOP_SRVC_EVT;

        p_buf->layer_specific = service_id;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTS_HandleValueIndication
**
** Description      This function is called to read a characteristics descriptor.
**
** Parameters       bda - remote device bd address to indicate.
**                  attr_id - attribute ID to indicate.
**                  data_len - indicate data length.
**                  p_data: data to indicate.
**                  need_confirm - if this indication expects a confirmation or not.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTS_HandleValueIndication (UINT16 conn_id, UINT16 attr_id, UINT16 data_len,
                                      UINT8 *p_data, BOOLEAN need_confirm)
{
    tBTA_GATTS_API_INDICATION  *p_buf;
    UINT16  len = sizeof(tBTA_GATTS_API_INDICATION);

    if ((p_buf = (tBTA_GATTS_API_INDICATION *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, len);

        p_buf->hdr.event = BTA_GATTS_API_INDICATION_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->attr_id = attr_id;
        p_buf->need_confirm = need_confirm;

        if (data_len > 0 && p_data != NULL) {
            p_buf->len = data_len;
            memcpy(p_buf->value, p_data, data_len);

        }
        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTS_SendRsp
**
** Description      This function is called to send a response to a request.
**
** Parameters       conn_id - connection identifier.
**                  trans_id - transaction ID.
**                  status - response status
**                  p_msg - response data.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTS_SendRsp (UINT16 conn_id, UINT32 trans_id,
                        tBTA_GATT_STATUS status, tBTA_GATTS_RSP *p_msg)
{
    tBTA_GATTS_API_RSP  *p_buf;
    UINT16  len = sizeof(tBTA_GATTS_API_RSP) + sizeof(tBTA_GATTS_RSP);

    if ((p_buf = (tBTA_GATTS_API_RSP *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, len);

        p_buf->hdr.event    = BTA_GATTS_API_RSP_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->trans_id = trans_id;
        p_buf->status = status;

        if (p_msg != NULL) {
            p_buf->p_rsp = (tBTA_GATTS_RSP *)(p_buf + 1);
            memcpy(p_buf->p_rsp, p_msg, sizeof(tBTA_GATTS_RSP));
        }

        bta_sys_sendmsg(p_buf);
    }
    return;

}


void BTA_SetAttributeValue(UINT16 attr_handle, UINT16 length, UINT8 *value)
{
    tBTA_GATTS_API_SET_ATTR_VAL *p_buf;
    if((p_buf = (tBTA_GATTS_API_SET_ATTR_VAL *)osi_malloc(
                    sizeof(tBTA_GATTS_API_SET_ATTR_VAL))) != NULL){
        p_buf->hdr.event = BTA_GATTS_API_SET_ATTR_VAL_EVT;
        p_buf->hdr.layer_specific = attr_handle;
        p_buf->length = length;
        if(value != NULL){
            if((p_buf->value = (UINT8 *)osi_malloc(length)) != NULL){
                memcpy(p_buf->value, value, length);
            }
        }

        bta_sys_sendmsg(p_buf);
    }

}

tBTA_GATT_STATUS BTA_GetAttributeValue(UINT16 attr_handle, UINT16 *length, UINT8 **value)
{
    return bta_gatts_get_attr_value(attr_handle, length, value);
}

/*******************************************************************************
**
** Function         BTA_GATTS_Open
**
** Description      Open a direct open connection or add a background auto connection
**                  bd address
**
** Parameters       server_if: server interface.
**                  remote_bda: remote device BD address.
**                  is_direct: direct connection or background auto connection
**                  transport : Transport on which GATT connection to be opened (BR/EDR or LE)
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_Open(tBTA_GATTS_IF server_if, BD_ADDR remote_bda, BOOLEAN is_direct,
                    tBTA_GATT_TRANSPORT transport)
{
    tBTA_GATTS_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_GATTS_API_OPEN *) osi_malloc(sizeof(tBTA_GATTS_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTS_API_OPEN_EVT;
        p_buf->server_if = server_if;
        p_buf->is_direct = is_direct;
        p_buf->transport = transport;
        memcpy(p_buf->remote_bda, remote_bda, BD_ADDR_LEN);

        bta_sys_sendmsg(p_buf);
    }
    return;
}


/*******************************************************************************
**
** Function         BTA_GATTS_CancelOpen
**
** Description      Cancel a direct open connection or remove a background auto connection
**                  bd address
**
** Parameters       server_if: server interface.
**                  remote_bda: remote device BD address.
**                  is_direct: direct connection or background auto connection
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_CancelOpen(tBTA_GATTS_IF server_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
    tBTA_GATTS_API_CANCEL_OPEN  *p_buf;

    if ((p_buf = (tBTA_GATTS_API_CANCEL_OPEN *) osi_malloc(sizeof(tBTA_GATTS_API_CANCEL_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTS_API_CANCEL_OPEN_EVT;
        p_buf->server_if = server_if;
        p_buf->is_direct = is_direct;
        memcpy(p_buf->remote_bda, remote_bda, BD_ADDR_LEN);
        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTS_Close
**
** Description      Close a connection  a remote device.
**
** Parameters       conn_id: connection ID to be closed.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_Close(UINT16 conn_id)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_GATTS_API_CLOSE_EVT;
        p_buf->layer_specific = conn_id;
        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTS_Listen
**
** Description      Start advertisement to listen for connection request for a
**                  GATT server
**
** Parameters       server_if: server interface.
**                  start: to start or stop listening for connection
**                  remote_bda: remote device BD address, if listen to all device
**                              use NULL.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTS_Listen(tBTA_GATTS_IF server_if, BOOLEAN start, BD_ADDR_PTR target_bda)
{
    tBTA_GATTS_API_LISTEN  *p_buf;

    if ((p_buf = (tBTA_GATTS_API_LISTEN *) osi_malloc((UINT16)(sizeof(tBTA_GATTS_API_LISTEN) + BD_ADDR_LEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTS_API_LISTEN_EVT;

        p_buf->server_if    = server_if;
        p_buf->start        = start;

        if (target_bda) {
            p_buf->remote_bda = (UINT8 *)(p_buf + 1);
            memcpy(p_buf->remote_bda, target_bda, BD_ADDR_LEN);
        } else {
            p_buf->remote_bda = NULL;
        }

        bta_sys_sendmsg(p_buf);
    }
    return;
}

#endif /* BTA_GATT_INCLUDED */
