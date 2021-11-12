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
 *  This is the implementation of the API for GATT module of BTA.
 *
 ******************************************************************************/

#include "common/bt_target.h"
#include "osi/allocator.h"

#if defined(GATTC_INCLUDED) && (GATTC_INCLUDED == TRUE)

#include <string.h>
#include "bta/bta_sys.h"
#include "bta/bta_gatt_api.h"
#include "bta_gattc_int.h"
#include "stack/l2c_api.h"
/*****************************************************************************
**  Constants
*****************************************************************************/

static const tBTA_SYS_REG bta_gattc_reg = {
    bta_gattc_hdl_event,
    BTA_GATTC_Disable
};


/*******************************************************************************
**
** Function         BTA_GATTC_Disable
**
** Description      This function is called to disable GATTC module
**
** Parameters       None.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_Disable(void)
{
    BT_HDR  *p_buf;

    if (bta_sys_is_register(BTA_ID_GATTC) == FALSE) {
        APPL_TRACE_WARNING("GATTC Module not enabled/already disabled\n");
        return;
    }
    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_GATTC_API_DISABLE_EVT;
        bta_sys_sendmsg(p_buf);
    }
    bta_sys_deregister(BTA_ID_GATTC);

}

/*******************************************************************************
**
** Function         BTA_GATTC_AppRegister
**
** Description      This function is called to register application callbacks
**                    with BTA GATTC module.
**
** Parameters       p_app_uuid - application UUID
**                  p_client_cb - pointer to the application callback function.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_AppRegister(tBT_UUID *p_app_uuid, tBTA_GATTC_CBACK *p_client_cb)
{
    tBTA_GATTC_API_REG  *p_buf;

    if (bta_sys_is_register(BTA_ID_GATTC) == FALSE) {
        bta_sys_register(BTA_ID_GATTC, &bta_gattc_reg);
    }

    if ((p_buf = (tBTA_GATTC_API_REG *) osi_malloc(sizeof(tBTA_GATTC_API_REG))) != NULL) {
        p_buf->hdr.event    = BTA_GATTC_API_REG_EVT;
        if (p_app_uuid != NULL) {
            memcpy(&p_buf->app_uuid, p_app_uuid, sizeof(tBT_UUID));
        }
        p_buf->p_cback      = p_client_cb;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_AppDeregister
**
** Description      This function is called to deregister an application
**                  from BTA GATTC module.
**
** Parameters       client_if - client interface identifier.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_AppDeregister(tBTA_GATTC_IF client_if)
{
    tBTA_GATTC_API_DEREG  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_DEREG *) osi_malloc(sizeof(tBTA_GATTC_API_DEREG))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_DEREG_EVT;
        p_buf->client_if = client_if;
        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_Open
**
** Description      Open a direct connection or add a background auto connection
**                  bd address
**
** Parameters       client_if: server interface.
**                  remote_bda: remote device BD address.
**                  remote_addr_type: remote device BD address type.
**                  is_direct: direct connection or background auto connection
**                  transport: Transport to be used for GATT connection (BREDR/LE)
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_Open(tBTA_GATTC_IF client_if, BD_ADDR remote_bda, tBTA_ADDR_TYPE remote_addr_type,
                    BOOLEAN is_direct, tBTA_GATT_TRANSPORT transport)
{
    tBTA_GATTC_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_OPEN *) osi_malloc(sizeof(tBTA_GATTC_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_OPEN_EVT;

        p_buf->client_if = client_if;
        p_buf->is_direct = is_direct;
        p_buf->transport = transport;
        p_buf->remote_addr_type = remote_addr_type;
        memcpy(p_buf->remote_bda, remote_bda, BD_ADDR_LEN);


        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_CancelOpen
**
** Description      Cancel a direct open connection or remove a background auto connection
**                  bd address
**
** Parameters       client_if: server interface.
**                  remote_bda: remote device BD address.
**                  is_direct: direct connection or background auto connection
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_CancelOpen(tBTA_GATTC_IF client_if, BD_ADDR remote_bda, BOOLEAN is_direct)
{
    //If the registration callback is NULL, return
    if(bta_sys_is_register(BTA_ID_GATTC) == FALSE) {
        return;
    }

    tBTA_GATTC_API_CANCEL_OPEN  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_CANCEL_OPEN *) osi_malloc(sizeof(tBTA_GATTC_API_CANCEL_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_CANCEL_OPEN_EVT;

        p_buf->client_if = client_if;
        p_buf->is_direct = is_direct;
        memcpy(p_buf->remote_bda, remote_bda, BD_ADDR_LEN);

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_Close
**
** Description      Close a connection to a GATT server.
**
** Parameters       conn_id: connection ID to be closed.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_Close(UINT16 conn_id)
{
    BT_HDR  *p_buf;

    if ((p_buf = (BT_HDR *) osi_malloc(sizeof(BT_HDR))) != NULL) {
        p_buf->event = BTA_GATTC_API_CLOSE_EVT;

        p_buf->layer_specific = conn_id;

        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTC_ConfigureMTU
**
** Description      Configure the MTU size in the GATT channel. This can be done
**                  only once per connection.
**
** Parameters       conn_id: connection ID.
**                  mtu: desired MTU size to use.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_ConfigureMTU (UINT16 conn_id)
{
    tBTA_GATTC_API_CFG_MTU  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_CFG_MTU *) osi_malloc(sizeof(tBTA_GATTC_API_CFG_MTU))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_CFG_MTU_EVT;
        p_buf->hdr.layer_specific = conn_id;

        bta_sys_sendmsg(p_buf);
    }
    return;
}
/*******************************************************************************
**
** Function         BTA_GATTC_ServiceSearchRequest
**
** Description      This function is called to request a GATT service discovery
**                    on a GATT server. This function report service search result
**                  by a callback event, and followed by a service search complete
**                  event.
**
** Parameters       conn_id: connection ID.
**                  p_srvc_uuid: a UUID of the service application is interested in.
**                              If Null, discover for all services.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_ServiceSearchRequest (UINT16 conn_id, tBT_UUID *p_srvc_uuid)
{
    tBTA_GATTC_API_SEARCH  *p_buf;
    UINT16  len = sizeof(tBTA_GATTC_API_SEARCH) + sizeof(tBT_UUID);

    if ((p_buf = (tBTA_GATTC_API_SEARCH *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, len);

        p_buf->hdr.event = BTA_GATTC_API_SEARCH_EVT;
        p_buf->hdr.layer_specific = conn_id;

        if (p_srvc_uuid) {
            p_buf->p_srvc_uuid = (tBT_UUID *)(p_buf + 1);
            memcpy(p_buf->p_srvc_uuid, p_srvc_uuid, sizeof(tBT_UUID));
        } else {
            p_buf->p_srvc_uuid = NULL;
        }

        bta_sys_sendmsg(p_buf);
    }
    return;
}


/*******************************************************************************
**
** Function         BTA_GATTC_GetServices
**
** Description      This function is called to find the services on the given server.
**
** Parameters       conn_id: connection ID which identify the server.
**
** Returns          returns list_t of tBTA_GATTC_SERVICE or NULL.
**
*******************************************************************************/
const list_t* BTA_GATTC_GetServices(UINT16 conn_id) 
{
    return bta_gattc_get_services(conn_id);
}

/*******************************************************************************
**
** Function         BTA_GATTC_GetCharacteristic
**
** Description      This function is called to find the characteristic on the given server.
**
** Parameters       conn_id - connection ID which identify the server.
**                  handle - characteristic handle
**
** Returns          returns pointer to tBTA_GATTC_CHARACTERISTIC or NULL.
**
*******************************************************************************/
const tBTA_GATTC_CHARACTERISTIC* BTA_GATTC_GetCharacteristic(UINT16 conn_id, UINT16 handle) 
{
    return bta_gattc_get_characteristic(conn_id, handle);
}

/*******************************************************************************
**
** Function         BTA_GATTC_GetDescriptor
**
** Description      This function is called to find the characteristic on the given server.
**
** Parameters       conn_id - connection ID which identify the server.
**                  handle - descriptor handle
**
** Returns          returns pointer to tBTA_GATTC_DESCRIPTOR or NULL.
**
*******************************************************************************/
const tBTA_GATTC_DESCRIPTOR* BTA_GATTC_GetDescriptor(UINT16 conn_id, UINT16 handle)
{
    return bta_gattc_get_descriptor(conn_id, handle);
}

void BTA_GATTC_GetServiceWithUUID(UINT16 conn_id, tBT_UUID *svc_uuid,
                                  btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_service_with_uuid(conn_id, svc_uuid, db, count);
}

void BTA_GATTC_GetAllChar(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle,
                          btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_db_with_opration(conn_id,
                                   GATT_OP_GET_ALL_CHAR,
                                   0,
                                   NULL,
                                   NULL,
                                   NULL,
                                   start_handle,
                                   end_handle,
                                   db,
                                   count);
}

void BTA_GATTC_GetAllDescriptor(UINT16 conn_id, UINT16 char_handle,
                                btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_db_with_opration(conn_id,
                                   GATT_OP_GET_ALL_DESCRI,
                                   char_handle,
                                   NULL,
                                   NULL,
                                   NULL,
                                   0,
                                   0xFFFF,
                                   db,
                                   count);
}

void BTA_GATTC_GetCharByUUID(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle, tBT_UUID char_uuid,
                             btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_db_with_opration(conn_id,
                                   GATT_OP_GET_CHAR_BY_UUID,
                                   0,
                                   NULL,
                                   &char_uuid,
                                   NULL,
                                   start_handle,
                                   end_handle,
                                   db,
                                   count);
}

void BTA_GATTC_GetDescrByUUID(UINT16 conn_id, uint16_t start_handle, uint16_t end_handle,
                              tBT_UUID char_uuid, tBT_UUID descr_uuid,
                              btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_db_with_opration(conn_id,
                                   GATT_OP_GET_DESCRI_BY_UUID,
                                   0,
                                   NULL,
                                   &char_uuid,
                                   &descr_uuid,
                                   start_handle,
                                   end_handle,
                                   db,
                                   count);
}

void BTA_GATTC_GetDescrByCharHandle(UINT16 conn_id, UINT16 char_handle, tBT_UUID descr_uuid,
                                    btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_db_with_opration(conn_id,
                                   GATT_OP_GET_DESCRI_BY_HANDLE,
                                   char_handle,
                                   NULL,
                                   NULL,
                                   &descr_uuid,
                                   0,
                                   0xFFFF,
                                   db,
                                   count);
}

void BTA_GATTC_GetIncludeService(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle,
                                 tBT_UUID *incl_uuid, btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_db_with_opration(conn_id,
                                   GATT_OP_GET_INCLUDE_SVC,
                                   0,
                                   incl_uuid,
                                   NULL,
                                   NULL,
                                   start_handle,
                                   end_handle,
                                   db,
                                   count);
}

void BTA_GATTC_GetDBSize(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle, UINT16 *count)
{
    bta_gattc_get_db_size_handle(conn_id, start_handle, end_handle, count);
}

void BTA_GATTC_GetDBSizeByType(UINT16 conn_id, bt_gatt_db_attribute_type_t type,
                               UINT16 start_handle, UINT16 end_handle, UINT16 char_handle, UINT16 *count)
{
    bta_gattc_get_db_size_with_type_handle(conn_id, type, start_handle, end_handle, char_handle, count);
}


/*******************************************************************************
**
** Function         BTA_GATTC_GetGattDb
**
** Description      This function is called to get the GATT database.
**
** Parameters       conn_id: connection ID which identify the server.
**                  db: output parameter which will contain the GATT database copy.
**                      Caller is responsible for freeing it.
**                  count: number of elements in database.
**
*******************************************************************************/
void  BTA_GATTC_GetGattDb(UINT16 conn_id, UINT16 start_handle, UINT16 end_handle,
                          btgatt_db_element_t **db, UINT16 *count)
{
    bta_gattc_get_gatt_db(conn_id, start_handle, end_handle, db, count);
}

/*******************************************************************************
**
** Function         BTA_GATTC_ReadCharacteristic
**
** Description      This function is called to read a characteristics value
**
** Parameters       conn_id - connection ID.
**                  handle - characteritic handle to read.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_ReadCharacteristic(UINT16 conn_id, UINT16 handle, tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_READ  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_READ *) osi_malloc(sizeof(tBTA_GATTC_API_READ))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_READ));

        p_buf->hdr.event = BTA_GATTC_API_READ_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->handle = handle;
        p_buf->cmpl_evt = BTA_GATTC_READ_CHAR_EVT;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_ReadCharDescr
**
** Description      This function is called to read a descriptor value.
**
** Parameters       conn_id - connection ID.
**                  handle - descriptor handle to read.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_ReadCharDescr (UINT16 conn_id, UINT16 handle, tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_READ  *p_buf;
    UINT16  len = (UINT16)(sizeof(tBTA_GATT_ID) + sizeof(tBTA_GATTC_API_READ));

    if ((p_buf = (tBTA_GATTC_API_READ *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_READ));

        p_buf->hdr.event = BTA_GATTC_API_READ_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->handle = handle;
        p_buf->cmpl_evt = BTA_GATTC_READ_DESCR_EVT;

        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTC_ReadMultiple
**
** Description      This function is called to read multiple characteristic or
**                  characteristic descriptors.
**
** Parameters       conn_id - connection ID.
**                    p_read_multi - pointer to the read multiple parameter.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_ReadMultiple(UINT16 conn_id, tBTA_GATTC_MULTI *p_read_multi,
                            tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_READ_MULTI  *p_buf;
    //tBTA_GATTC_API_READ_MULTI          *p_value;
    UINT16      len = (UINT16)(sizeof(tBTA_GATTC_API_READ_MULTI));

    if ((p_buf = (tBTA_GATTC_API_READ_MULTI *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, len);

        p_buf->hdr.event = BTA_GATTC_API_READ_MULTI_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->num_attr = p_read_multi->num_attr;
        p_buf->cmpl_evt = BTA_GATTC_READ_MULTIPLE_EVT;
        if (p_buf->num_attr > 0) {
            memcpy(p_buf->handles, p_read_multi->handles, sizeof(UINT16) * p_read_multi->num_attr);
	    }

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_Read_by_type
**
** Description      This function is called to read a attribute value by uuid
**
** Parameters       conn_id - connection ID.
**                  s_handle - start handle.
**                  e_handle - end hanle
**                  uuid - The attribute UUID.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_Read_by_type(UINT16 conn_id, UINT16 s_handle,UINT16 e_handle, tBT_UUID *uuid, tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_READ  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_READ *) osi_malloc(sizeof(tBTA_GATTC_API_READ))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_READ));

        p_buf->hdr.event = BTA_GATTC_API_READ_BY_TYPE_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->s_handle = s_handle;
        p_buf->e_handle = e_handle;
        memcpy(&(p_buf->uuid), uuid, sizeof(tBT_UUID));
        p_buf->cmpl_evt = BTA_GATTC_READ_CHAR_EVT;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_WriteCharValue
**
** Description      This function is called to write characteristic value.
**
** Parameters       conn_id - connection ID.
**                  handle - characteristic handle to write.
**                  write_type - type of write.
**                  len: length of the data to be written.
**                  p_value - the value to be written.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_WriteCharValue ( UINT16 conn_id,
                                UINT16 handle,
                                tBTA_GATTC_WRITE_TYPE  write_type,
                                UINT16 len,
                                UINT8 *p_value,
                                tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_WRITE  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_WRITE *) osi_malloc((UINT16)(sizeof(tBTA_GATTC_API_WRITE) + len))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_WRITE) + len);

        p_buf->hdr.event = BTA_GATTC_API_WRITE_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->handle = handle;
        p_buf->cmpl_evt = BTA_GATTC_WRITE_CHAR_EVT;
        p_buf->write_type = write_type;
        p_buf->len = len;

        if (p_value && len > 0) {
            p_buf->p_value = (UINT8 *)(p_buf + 1);
            memcpy(p_buf->p_value, p_value, len);
        }
        if(write_type == BTA_GATTC_TYPE_WRITE_NO_RSP){
            l2ble_update_att_acl_pkt_num(L2CA_DECREASE_BTC_NUM, NULL);
            l2ble_update_att_acl_pkt_num(L2CA_ADD_BTU_NUM, NULL);
        }
        bta_sys_sendmsg(p_buf);
    }
    return;
}
/*******************************************************************************
**
** Function         BTA_GATTC_WriteCharDescr
**
** Description      This function is called to write descriptor value.
**
** Parameters       conn_id - connection ID
**                  handle - descriptor hadle to write.
**                  write_type - write type.
**                  p_value - the value to be written.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_WriteCharDescr (UINT16 conn_id,
                               UINT16 handle,
                               tBTA_GATTC_WRITE_TYPE  write_type,
                               tBTA_GATT_UNFMT      *p_data,
                               tBTA_GATT_AUTH_REQ auth_req)
{
    size_t len = sizeof(tBTA_GATTC_API_WRITE);
    tBTA_GATTC_API_WRITE *p_buf;
    if (p_data != NULL) {
        len += p_data->len;
    }

    if ((p_buf = (tBTA_GATTC_API_WRITE *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, len);

        p_buf->hdr.event = BTA_GATTC_API_WRITE_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->handle = handle;
        p_buf->cmpl_evt = BTA_GATTC_WRITE_DESCR_EVT;
        p_buf->write_type = write_type;

        if (p_data && p_data->len != 0) {
            p_buf->p_value  = (UINT8 *)(p_buf + 1);
            p_buf->len      = p_data->len;
            /* pack the descr data */
            memcpy(p_buf->p_value, p_data->p_value, p_data->len);
        }
        if(write_type == BTA_GATTC_TYPE_WRITE_NO_RSP){
            l2ble_update_att_acl_pkt_num(L2CA_DECREASE_BTC_NUM, NULL);
            l2ble_update_att_acl_pkt_num(L2CA_ADD_BTU_NUM, NULL);
        }
        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTC_PrepareWrite
**
** Description      This function is called to prepare write a characteristic value.
**
** Parameters       conn_id - connection ID.
**                    p_char_id - GATT characteritic ID of the service.
**                  offset - offset of the write value.
**                  len: length of the data to be written.
**                  p_value - the value to be written.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_PrepareWrite  (UINT16 conn_id, UINT16 handle,
                              UINT16 offset, UINT16 len, UINT8 *p_value,
                              tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_WRITE  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_WRITE *) osi_malloc((UINT16)(sizeof(tBTA_GATTC_API_WRITE) + len))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_WRITE) + len);

        p_buf->hdr.event = BTA_GATTC_API_WRITE_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->handle = handle;

        p_buf->write_type = BTA_GATTC_WRITE_PREPARE;
        p_buf->offset   = offset;
        p_buf->len = len;

        if (p_value && len > 0) {
            p_buf->p_value = (UINT8 *)(p_buf + 1);
            memcpy(p_buf->p_value, p_value, len);
        }

        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTC_PrepareWriteCharDescr
**
** Description      This function is called to prepare write a characteristic descriptor value.
**
** Parameters       conn_id - connection ID.
**                  p_char_descr_id - GATT characteritic descriptor ID of the service.
**                  offset - offset of the write value.
**                  len: length of the data to be written.
**                  p_value - the value to be written.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_PrepareWriteCharDescr  (UINT16 conn_id, UINT16 handle,
                                       UINT16 offset,tBTA_GATT_UNFMT *p_data,
                                       tBTA_GATT_AUTH_REQ auth_req)
{
    tBTA_GATTC_API_WRITE  *p_buf;
    UINT16  len = sizeof(tBTA_GATTC_API_WRITE);

    if (p_data != NULL) {
        len += p_data->len;
    }

    if ((p_buf = (tBTA_GATTC_API_WRITE *) osi_malloc(len)) != NULL) {
        memset(p_buf, 0, len);

        p_buf->hdr.event = BTA_GATTC_API_WRITE_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->auth_req = auth_req;
        p_buf->handle = handle;
        p_buf->write_type = BTA_GATTC_WRITE_PREPARE;
        p_buf->offset = offset;

        if (p_data && p_data->len != 0) {
            p_buf->len      = p_data->len;
            p_buf->p_value = (UINT8 *)(p_buf + 1);
            /* pack the descr data */
            memcpy(p_buf->p_value, p_data->p_value, p_data->len);
        }

        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTC_ExecuteWrite
**
** Description      This function is called to execute write a prepare write sequence.
**
** Parameters       conn_id - connection ID.
**                    is_execute - execute or cancel.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_ExecuteWrite  (UINT16 conn_id, BOOLEAN is_execute)
{
    tBTA_GATTC_API_EXEC  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_EXEC *) osi_malloc((UINT16)sizeof(tBTA_GATTC_API_EXEC))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_EXEC));
        p_buf->hdr.event = BTA_GATTC_API_EXEC_EVT;
        p_buf->hdr.layer_specific = conn_id;

        p_buf->is_execute = is_execute;

        bta_sys_sendmsg(p_buf);
    }
    return;

}
/*******************************************************************************
**
** Function         BTA_GATTC_SendIndConfirm
**
** Description      This function is called to send handle value confirmation.
**
** Parameters       conn_id - connection ID.
**                    p_char_id - characteristic ID to confirm.
**
** Returns          None
**
*******************************************************************************/
void BTA_GATTC_SendIndConfirm (UINT16 conn_id, UINT16 handle)
{
    tBTA_GATTC_API_CONFIRM  *p_buf;

    APPL_TRACE_API("BTA_GATTC_SendIndConfirm conn_id=%d handle =0x%x",
                   conn_id, handle);

    if ((p_buf = (tBTA_GATTC_API_CONFIRM *) osi_malloc(sizeof(tBTA_GATTC_API_CONFIRM))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_API_CONFIRM));
        p_buf->hdr.event = BTA_GATTC_API_CONFIRM_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->handle = handle;

        bta_sys_sendmsg(p_buf);
    }
    return;

}

/*******************************************************************************
**
** Function         BTA_GATTC_RegisterForNotifications
**
** Description      This function is called to register for notification of a service.
**
** Parameters       client_if - client interface.
**                  bda - target GATT server.
**                  handle - GATT characteristic handle.
**
** Returns          OK if registration succeed, otherwise failed.
**
*******************************************************************************/
tBTA_GATT_STATUS BTA_GATTC_RegisterForNotifications (tBTA_GATTC_IF client_if,
                                                     BD_ADDR bda, UINT16 handle)
{
    tBTA_GATTC_RCB      *p_clreg;
    tBTA_GATT_STATUS    status = BTA_GATT_ILLEGAL_PARAMETER;
    UINT8               i;

    if (!handle)
    {
        APPL_TRACE_ERROR("deregistration failed, handle is 0");
        return status;
    }

    if ((p_clreg = bta_gattc_cl_get_regcb(client_if)) != NULL) {
        for (i = 0; i < BTA_GATTC_NOTIF_REG_MAX; i ++) {
            if ( p_clreg->notif_reg[i].in_use &&
                    !memcmp(p_clreg->notif_reg[i].remote_bda, bda, BD_ADDR_LEN) &&
                  p_clreg->notif_reg[i].handle == handle) {
                APPL_TRACE_DEBUG("notification already registered");
                status = BTA_GATT_OK;
                break;
            }
        }
        if (status != BTA_GATT_OK) {
            for (i = 0; i < BTA_GATTC_NOTIF_REG_MAX; i ++) {
                if (!p_clreg->notif_reg[i].in_use) {
                    memset((void *)&p_clreg->notif_reg[i], 0, sizeof(tBTA_GATTC_NOTIF_REG));

                    p_clreg->notif_reg[i].in_use = TRUE;
                    memcpy(p_clreg->notif_reg[i].remote_bda, bda, BD_ADDR_LEN);

                    p_clreg->notif_reg[i].handle = handle;
                    status = BTA_GATT_OK;
                    break;
                }
            }
            if (i == BTA_GATTC_NOTIF_REG_MAX) {
                status = BTA_GATT_NO_RESOURCES;
                APPL_TRACE_ERROR("Max Notification Reached, registration failed.");
            }
        }
    } else {
        APPL_TRACE_ERROR("Client_if: %d Not Registered", client_if);
    }

    return status;
}

/*******************************************************************************
**
** Function         BTA_GATTC_DeregisterForNotifications
**
** Description      This function is called to de-register for notification of a service.
**
** Parameters       client_if - client interface.
**                  remote_bda - target GATT server.
**                  handle - GATT characteristic handle.
**
** Returns          OK if deregistration succeed, otherwise failed.
**
*******************************************************************************/
tBTA_GATT_STATUS BTA_GATTC_DeregisterForNotifications (tBTA_GATTC_IF client_if,
                                                       BD_ADDR bda, UINT16 handle)
{
    if (!handle) {
        APPL_TRACE_ERROR("%s: deregistration failed, handle is 0", __func__);
        return BTA_GATT_ILLEGAL_PARAMETER;
    }

    tBTA_GATTC_RCB *p_clreg = bta_gattc_cl_get_regcb(client_if);
    if (p_clreg == NULL) {
        APPL_TRACE_ERROR("%s client_if: %d not registered bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
            __func__, client_if, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
        return BTA_GATT_ILLEGAL_PARAMETER;
    }

    for (int i = 0; i < BTA_GATTC_NOTIF_REG_MAX; i ++) {
        if (p_clreg->notif_reg[i].in_use &&
            !memcmp(p_clreg->notif_reg[i].remote_bda, bda, BD_ADDR_LEN) &&
            p_clreg->notif_reg[i].handle == handle) {
            APPL_TRACE_DEBUG("%s deregistered bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
                __func__, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
            memset(&p_clreg->notif_reg[i], 0, sizeof(tBTA_GATTC_NOTIF_REG));
            return BTA_GATT_OK;
        }
    }

    APPL_TRACE_ERROR("%s registration not found bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
        __func__, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);
    return BTA_GATT_ERROR;
}

/*******************************************************************************
**
** Function         BTA_GATTC_Refresh
**
** Description      Refresh the server cache of the remote device
**
** Parameters       remote_bda: remote device BD address.
**                  erase_flash: delete cache from nvs flash
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_Refresh(BD_ADDR remote_bda, bool erase_flash)
{
#if(GATTC_CACHE_NVS == TRUE)
    if(erase_flash) {
        /* used to reset cache in application */
        bta_gattc_cache_reset(remote_bda);
    }
#endif
    //If the registration callback is NULL, return
    if(bta_sys_is_register(BTA_ID_GATTC) == FALSE) {
        return;
    }
    tBTA_GATTC_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_OPEN *) osi_malloc(sizeof(tBTA_GATTC_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_REFRESH_EVT;
        memcpy(p_buf->remote_bda, remote_bda, BD_ADDR_LEN);

        bta_sys_sendmsg(p_buf);
    }
    return;
}

void BTA_GATTC_CacheAssoc(tBTA_GATTC_IF client_if, BD_ADDR src_addr, BD_ADDR assoc_addr, BOOLEAN is_assoc)
{
    tBTA_GATTC_API_CACHE_ASSOC *p_buf;

    if ((p_buf = (tBTA_GATTC_API_CACHE_ASSOC *)osi_malloc(sizeof(tBTA_GATTC_API_CACHE_ASSOC))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_CACHE_ASSOC_EVT;
        p_buf->is_assoc = is_assoc;
        p_buf->client_if  = client_if;
        memcpy(p_buf->src_addr, src_addr, sizeof(BD_ADDR));
        memcpy(p_buf->assoc_addr, assoc_addr, sizeof(BD_ADDR));

        bta_sys_sendmsg(p_buf);
        
    }
    return;
}

void BTA_GATTC_CacheGetAddrList(tBTA_GATTC_IF client_if)
{
    tBTA_GATTC_API_GET_ADDR *p_buf;
    if ((p_buf = (tBTA_GATTC_API_GET_ADDR *)osi_malloc(sizeof(tBTA_GATTC_API_GET_ADDR))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_CACHE_GET_ADDR_LIST_EVT;
        p_buf->client_if = client_if;

        bta_sys_sendmsg(p_buf);
    }
    return;
}

/*******************************************************************************
**
** Function         BTA_GATTC_Clean
**
** Description      Clean the server cache of the remote device
**
** Parameters       remote_bda: remote device BD address.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_Clean(BD_ADDR remote_bda)
{
#if(GATTC_CACHE_NVS == TRUE)
    /* used to reset cache in application */
    bta_gattc_cache_reset(remote_bda);
#endif

    tBTA_GATTC_API_OPEN  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_OPEN *) osi_malloc(sizeof(tBTA_GATTC_API_OPEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_CACHE_CLEAN_EVT;
        memcpy(p_buf->remote_bda, remote_bda, BD_ADDR_LEN);

        bta_sys_sendmsg(p_buf);
    }
    return;
}
/*******************************************************************************
**
** Function         BTA_GATTC_Listen
**
** Description      Start advertisement to listen for connection request for a GATT
**                  client application.
**
** Parameters       client_if: server interface.
**                  start: to start or stop listening for connection
**                  remote_bda: remote device BD address, if listen to all device
**                              use NULL.
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_Listen(tBTA_GATTC_IF client_if, BOOLEAN start, BD_ADDR_PTR target_bda)
{
    tBTA_GATTC_API_LISTEN  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_LISTEN *) osi_malloc((UINT16)(sizeof(tBTA_GATTC_API_LISTEN) + BD_ADDR_LEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_LISTEN_EVT;

        p_buf->client_if = client_if;
        p_buf->start = start;
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

/*******************************************************************************
**
** Function         BTA_GATTC_Broadcast
**
** Description      Start broadcasting (non-connectable advertisements)
**
** Parameters       client_if: client interface.
**                  start: to start or stop listening for connection
**
** Returns          void
**
*******************************************************************************/
void BTA_GATTC_Broadcast(tBTA_GATTC_IF client_if, BOOLEAN start)
{
    tBTA_GATTC_API_LISTEN  *p_buf;

    if ((p_buf = (tBTA_GATTC_API_LISTEN *) osi_malloc((UINT16)(sizeof(tBTA_GATTC_API_LISTEN) + BD_ADDR_LEN))) != NULL) {
        p_buf->hdr.event = BTA_GATTC_API_BROADCAST_EVT;
        p_buf->client_if = client_if;
        p_buf->start = start;
        bta_sys_sendmsg(p_buf);
    }
    return;
}

#endif /* defined(GATTC_INCLUDED) && (GATTC_INCLUDED == TRUE) */

