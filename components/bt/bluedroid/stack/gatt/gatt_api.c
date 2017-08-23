/******************************************************************************
 *
 *  Copyright (C) 1999-2012 Broadcom Corporation
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
 *  this file contains GATT interface functions
 *
 ******************************************************************************/
#include "bt_target.h"


#if defined(BTA_GATT_INCLUDED) && (BTA_GATT_INCLUDED == TRUE)

#include "allocator.h"
#include <string.h>
#include "gatt_api.h"
#include "gatt_int.h"
#include "l2c_api.h"
#include "btm_int.h"
#include "sdpdefs.h"
#include "sdp_api.h"

/*******************************************************************************
**
** Function         GATT_SetTraceLevel
**
** Description      This function sets the trace level.  If called with
**                  a value of 0xFF, it simply returns the current trace level.
**
**                  Input Parameters:
**                      level:  The level to set the GATT tracing to:
**                      0xff-returns the current setting.
**                      0-turns off tracing.
**                      >= 1-Errors.
**                      >= 2-Warnings.
**                      >= 3-APIs.
**                      >= 4-Events.
**                      >= 5-Debug.
**
** Returns          The new or current trace level
**
*******************************************************************************/
UINT8 GATT_SetTraceLevel (UINT8 new_level)
{
    if (new_level != 0xFF) {
        gatt_cb.trace_level = new_level;
    }

    return (gatt_cb.trace_level);
}


#if (GATTS_INCLUDED == TRUE)
/*****************************************************************************
**
**                  GATT SERVER API
**
******************************************************************************/
/*******************************************************************************
**
** Function         GATTS_AddHandleRange
**
** Description      This function add the allocated handles range for the specifed
**                  application UUID, service UUID and service instance
**
** Parameter        p_hndl_range:   pointer to allocated handles information
**
** Returns          TRUE if handle range is added sucessfully; otherwise FALSE.
**
*******************************************************************************/

BOOLEAN GATTS_AddHandleRange(tGATTS_HNDL_RANGE *p_hndl_range)
{
    tGATT_HDL_LIST_ELEM *p_buf;
    BOOLEAN status = FALSE;

    if ((p_buf = gatt_alloc_hdl_buffer()) != NULL) {
        p_buf->asgn_range = *p_hndl_range;
        status  = gatt_add_an_item_to_list(&gatt_cb.hdl_list_info, p_buf);
    }
    return status;
}


/*******************************************************************************
**
** Function         GATTS_NVRegister
**
** Description      Application manager calls this function to register for
**                  NV save callback function.  There can be one and only one
**                  NV save callback function.
**
** Parameter        p_cb_info : callback informaiton
**
** Returns          TRUE if registered OK, else FALSE
**
*******************************************************************************/
BOOLEAN  GATTS_NVRegister (tGATT_APPL_INFO *p_cb_info)
{
    BOOLEAN status = FALSE;
    if (p_cb_info) {
        gatt_cb.cb_info = *p_cb_info;
        status = TRUE;
        gatt_init_srv_chg();
    }

    return status;
}

/*******************************************************************************
**
** Function         GATTS_CreateService
**
** Description      This function is called to reserve a block of handles for a service.
**
**                  *** It should be called only once per service instance  ***
**
** Parameter        gatt_if       : application if
**                  p_svc_uuid    : service UUID
**                  svc_inst      : instance of the service inside the application
**                  num_handles   : number of handles needed by the service.
**                  is_pri        : is a primary service or not.
**
** Returns          service handle if sucessful, otherwise 0.
**
*******************************************************************************/
UINT16 GATTS_CreateService (tGATT_IF gatt_if, tBT_UUID *p_svc_uuid,
                            UINT16 svc_inst, UINT16 num_handles, BOOLEAN is_pri)
{

    tGATT_HDL_LIST_INFO     *p_list_info = &gatt_cb.hdl_list_info;
    tGATT_HDL_LIST_ELEM     *p_list = NULL;
    UINT16                  s_hdl = 0;
    BOOLEAN                 save_hdl = FALSE;
    tGATTS_PENDING_NEW_SRV_START      *p_buf = NULL;
    tGATT_REG              *p_reg = gatt_get_regcb(gatt_if);
    tBT_UUID     *p_app_uuid128;


    GATT_TRACE_API ("GATTS_CreateService\n" );

    if (p_reg == NULL) {
        GATT_TRACE_ERROR ("Inavlid gatt_if=%d\n", gatt_if);
        return (0);
    }

    p_app_uuid128 = &p_reg->app_uuid128;

    if ((p_list = gatt_find_hdl_buffer_by_app_id(p_app_uuid128, p_svc_uuid, svc_inst)) != NULL) {
        s_hdl = p_list->asgn_range.s_handle;
        GATT_TRACE_DEBUG ("Service already been created!!\n");
    } else {
        if ( (p_svc_uuid->len == LEN_UUID_16) && (p_svc_uuid->uu.uuid16 == UUID_SERVCLASS_GATT_SERVER)) {
            s_hdl =  gatt_cb.hdl_cfg.gatt_start_hdl;
        } else if ((p_svc_uuid->len == LEN_UUID_16) && (p_svc_uuid->uu.uuid16 == UUID_SERVCLASS_GAP_SERVER)) {
            s_hdl = gatt_cb.hdl_cfg.gap_start_hdl;
        } else {
            p_list = p_list_info->p_first;

            if (p_list) {
                s_hdl = p_list->asgn_range.e_handle + 1;
            }

            if (s_hdl < gatt_cb.hdl_cfg.app_start_hdl) {

                s_hdl = gatt_cb.hdl_cfg.app_start_hdl;
            }
            save_hdl = TRUE;
        }

        /* check for space */
        if (num_handles > (0xFFFF - s_hdl + 1)) {
            GATT_TRACE_ERROR ("GATTS_ReserveHandles: no handles, s_hdl: %u  needed: %u\n", s_hdl, num_handles);
            return (0);
        }

        if ( (p_list = gatt_alloc_hdl_buffer()) == NULL) {
            /* No free entry */
            GATT_TRACE_ERROR ("GATTS_ReserveHandles: no free handle blocks\n");
            return (0);
        }

        p_list->asgn_range.app_uuid128 = *p_app_uuid128;
        p_list->asgn_range.svc_uuid    = *p_svc_uuid;
        p_list->asgn_range.svc_inst    = svc_inst;
        p_list->asgn_range.s_handle    = s_hdl;
        p_list->asgn_range.e_handle    = s_hdl + num_handles - 1;
        p_list->asgn_range.is_primary  = is_pri;

        gatt_add_an_item_to_list(p_list_info, p_list);

        if (save_hdl) {
            if (gatt_cb.cb_info.p_nv_save_callback) {
                (*gatt_cb.cb_info.p_nv_save_callback)(TRUE, &p_list->asgn_range);
            }
            /* add a pending new  service change item to the list */
            if ( (p_buf = gatt_add_pending_new_srv_start(&p_list->asgn_range)) == NULL) {
                /* No free entry */
                GATT_TRACE_ERROR ("gatt_add_pending_new_srv_start: no free blocks\n");

                if (p_list) {
                    gatt_remove_an_item_from_list(p_list_info, p_list);
                    gatt_free_attr_value_buffer(p_list);
                    gatt_free_hdl_buffer(p_list);
                }
                return (0);
            }

            GATT_TRACE_DEBUG ("Add a new srv chg item\n");
        }
    }

    if (!gatts_init_service_db(&p_list->svc_db, p_svc_uuid, is_pri, s_hdl , num_handles)) {
        GATT_TRACE_ERROR ("GATTS_ReserveHandles: service DB initialization failed\n");
        if (p_list) {
            gatt_remove_an_item_from_list(p_list_info, p_list);
            gatt_free_attr_value_buffer(p_list);
            gatt_free_hdl_buffer(p_list);
        }

        if (p_buf) {
            osi_free(fixed_queue_try_remove_from_queue(gatt_cb.pending_new_srv_start_q, p_buf));
        }
        return (0);
    }

    return (s_hdl);
}

/*******************************************************************************
**
** Function         GATTS_AddIncludeService
**
** Description      This function is called to add an included service.
**
** Parameter        service_handle : To which service this included service is added to.
**                  include_svc_handle    : included service handle.
**
** Returns          included service attribute handle. If 0, add included service
**                  fail.
**
*******************************************************************************/
UINT16 GATTS_AddIncludeService (UINT16 service_handle, UINT16 include_svc_handle)

{
    tGATT_HDL_LIST_ELEM  *p_decl, *p_incl_decl;

    if ((p_decl = gatt_find_hdl_buffer_by_handle(service_handle)) == NULL) {
        GATT_TRACE_DEBUG("Service not created");
        return 0;
    }
    if ((p_incl_decl = gatt_find_hdl_buffer_by_handle(include_svc_handle)) == NULL) {
        GATT_TRACE_DEBUG("Included Service not created");
        return 0;
    }

    return gatts_add_included_service(&p_decl->svc_db,
                                      p_incl_decl->asgn_range.s_handle,
                                      p_incl_decl->asgn_range.e_handle,
                                      p_incl_decl->asgn_range.svc_uuid);
}
/*******************************************************************************
**
** Function         GATTS_AddCharacteristic
**
** Description      This function is called to add a characteristic into a service.
**                  It will add a characteristic declaration and characteristic
**                  value declaration into the service database identified by the
**                  service handle.
**
** Parameter        service_handle : To which service this included service is added to.
**                  char_uuid : Characteristic UUID.
**                  perm      : Characteristic value declaration attribute permission.
**                  property  : Characteristic Properties
**
** Returns          Characteristic value declaration attribute handle. 0 if failed.
**
*******************************************************************************/
UINT16 GATTS_AddCharacteristic (UINT16 service_handle, tBT_UUID *p_char_uuid,
                                tGATT_PERM perm, tGATT_CHAR_PROP property, 
                                tGATT_ATTR_VAL *attr_val, tGATTS_ATTR_CONTROL *control)
{
    tGATT_HDL_LIST_ELEM  *p_decl;

    if ((p_decl = gatt_find_hdl_buffer_by_handle(service_handle)) == NULL) {
        GATT_TRACE_DEBUG("Service not created\n");
        return 0;
    }
    /* data validity checking */
    if (  ((property & GATT_CHAR_PROP_BIT_AUTH) && !(perm & GATT_WRITE_SIGNED_PERM)) ||
            ((perm & GATT_WRITE_SIGNED_PERM) && !(property & GATT_CHAR_PROP_BIT_AUTH)) ) {
        GATT_TRACE_DEBUG("Invalid configuration property=0x%x perm=0x%x\n ", property, perm);
        return 0;
    }

    return gatts_add_characteristic(&p_decl->svc_db,
                                    perm,
                                    property,
                                    p_char_uuid, 
                                    attr_val, control);
}
/*******************************************************************************
**
** Function         GATTS_AddCharDescriptor
**
** Description      This function is called to add a characteristic descriptor
**                  into a service database. Add descriptor should follow add char
**                  to which it belongs, and next add char should be done only
**                  after all add descriptors for the previous char.
**
** Parameter        service_handle  : To which service this characteristic descriptor
**                                    is added to.
**                  perm            : Characteristic value declaration attribute
**                                    permission.
**                  p_descr_uuid    : Characteristic descriptor UUID
**
** Returns         Characteristic descriptor attribute handle. 0 if add
**                 characteristic descriptor failed.
**
*******************************************************************************/
UINT16 GATTS_AddCharDescriptor (UINT16 service_handle,
                                tGATT_PERM perm,
                                tBT_UUID   *p_descr_uuid, tGATT_ATTR_VAL *attr_val, tGATTS_ATTR_CONTROL *control)
{
    tGATT_HDL_LIST_ELEM  *p_decl;

    if ((p_decl = gatt_find_hdl_buffer_by_handle(service_handle)) == NULL) {
        GATT_TRACE_DEBUG("Service not created");
        return 0;
    }
    if (p_descr_uuid == NULL ||
            (p_descr_uuid->len != LEN_UUID_128 && p_descr_uuid->len !=  LEN_UUID_16
             && p_descr_uuid->len !=  LEN_UUID_32)) {
        GATT_TRACE_DEBUG("Illegal parameter");
        return 0;
    }

    return gatts_add_char_descr(&p_decl->svc_db,
                                perm,
                                p_descr_uuid,
                                attr_val, control);

}
/*******************************************************************************
**
** Function         GATTS_DeleteService
**
** Description      This function is called to delete a service.
**
** Parameter        gatt_if       : application interface
**                  p_svc_uuid    : service UUID
**                  svc_inst      : instance of the service inside the application
**
** Returns          TRUE if operation succeed, FALSE if handle block was not found.
**
*******************************************************************************/
BOOLEAN GATTS_DeleteService (tGATT_IF gatt_if, tBT_UUID *p_svc_uuid, UINT16 svc_inst)
{

    tGATT_HDL_LIST_INFO             *p_list_info = &gatt_cb.hdl_list_info;
    tGATT_HDL_LIST_ELEM             *p_list = NULL;
    UINT8                           i_sreg;
    tGATTS_PENDING_NEW_SRV_START    *p_buf;
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    tBT_UUID *p_app_uuid128;

    GATT_TRACE_DEBUG ("GATTS_DeleteService");

    if (p_reg == NULL) {
        GATT_TRACE_ERROR ("Applicaiton not foud");
        return (FALSE);
    }
    p_app_uuid128 = &p_reg->app_uuid128;

    if ((p_list = gatt_find_hdl_buffer_by_app_id(p_app_uuid128, p_svc_uuid, svc_inst)) == NULL) {
        GATT_TRACE_ERROR ("No Service found");
        return (FALSE);
    }

    if ( (p_buf = gatt_sr_is_new_srv_chg(&p_list->asgn_range.app_uuid128,
                                         &p_list->asgn_range.svc_uuid,
                                         p_list->asgn_range.svc_inst)) != NULL) {
        GATT_TRACE_DEBUG ("Delete a new service changed item - the service has not yet started");
        osi_free(fixed_queue_try_remove_from_queue(gatt_cb.pending_new_srv_start_q, p_buf));
    } else {
        gatt_proc_srv_chg();
    }

    if ((i_sreg = gatt_sr_find_i_rcb_by_app_id (p_app_uuid128,
                  p_svc_uuid,
                  svc_inst)) != GATT_MAX_SR_PROFILES) {
        GATTS_StopService(gatt_cb.sr_reg[i_sreg].s_hdl);
    }

    GATT_TRACE_DEBUG ("released handles s_hdl=%u e_hdl=%u",
                      p_list->asgn_range.s_handle , p_list->asgn_range.e_handle  );

    if ( (p_list->asgn_range.s_handle >= gatt_cb.hdl_cfg.app_start_hdl)
            && gatt_cb.cb_info.p_nv_save_callback) {
        (*gatt_cb.cb_info.p_nv_save_callback)(FALSE, &p_list->asgn_range);
    }

    gatt_remove_an_item_from_list(p_list_info, p_list);
    gatt_free_attr_value_buffer(p_list);
    gatt_free_hdl_buffer(p_list);

    return (TRUE);
}

/*******************************************************************************
**
** Function         GATTS_StartService
**
** Description      This function is called to start a service with GATT
**
** Parameter        gatt_if : service handle.
**                  p_cback       : application service callback functions.
**                  sup_transport : supported transport(s) for this primary service
**
** return           GATT_SUCCESS if sucessfully started; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS GATTS_StartService (tGATT_IF gatt_if, UINT16 service_handle,
                                 tGATT_TRANSPORT sup_transport)
{
    tGATT_SR_REG            *p_sreg;
    tGATT_HDL_LIST_ELEM      *p_list = NULL;
    UINT8                    i_sreg;
#if (SDP_INCLUDED == TRUE)
    tBT_UUID                *p_uuid;
#endif  ///SDP_INCLUDED == TRUE
    tGATT_REG              *p_reg = gatt_get_regcb(gatt_if);

    tGATTS_PENDING_NEW_SRV_START *p_buf;

    GATT_TRACE_API ("GATTS_StartService");

    if (p_reg == NULL) {
        /* Not found  */
        GATT_TRACE_ERROR ("Applicaiton not found ");
        return GATT_NOT_FOUND;
    }

    if ((p_list = gatt_find_hdl_buffer_by_handle(service_handle)) == NULL) {
        /* Not found  */
        GATT_TRACE_ERROR ("no service found");
        return GATT_NOT_FOUND;
    }

    if (gatt_sr_find_i_rcb_by_app_id (&p_list->asgn_range.app_uuid128,
                                      &p_list->asgn_range.svc_uuid,
                                      p_list->asgn_range.svc_inst) != GATT_MAX_SR_PROFILES) {
        GATT_TRACE_ERROR ("Duplicate Service start - Service already started");
        return GATT_SERVICE_STARTED;
    }

    /*this is a new application servoce start */
    if ((i_sreg = gatt_sr_alloc_rcb(p_list)) ==  GATT_MAX_SR_PROFILES) {
        GATT_TRACE_ERROR ("GATTS_StartService: no free server registration block");
        return GATT_NO_RESOURCES;
    }

    p_sreg = &gatt_cb.sr_reg[i_sreg];
    p_sreg->gatt_if = gatt_if;

    switch (sup_transport) {
    case GATT_TRANSPORT_BR_EDR:
    case GATT_TRANSPORT_LE_BR_EDR:
        if (p_sreg->type == GATT_UUID_PRI_SERVICE) {
#if (SDP_INCLUDED == TRUE)
            p_uuid = gatts_get_service_uuid (p_sreg->p_db);
            p_sreg->sdp_handle = gatt_add_sdp_record(p_uuid, p_sreg->s_hdl, p_sreg->e_hdl);
#endif  ///SDP_INCLUDED == TRUE
        }
        break;
    default:
        break;
    }

    gatts_update_srv_list_elem(i_sreg, p_sreg->s_hdl,
                               p_list->asgn_range.is_primary);

    gatt_add_a_srv_to_list(&gatt_cb.srv_list_info, &gatt_cb.srv_list[i_sreg]);

    GATT_TRACE_DEBUG ("allocated i_sreg=%d\n", i_sreg);

    GATT_TRACE_DEBUG ("s_hdl=%d e_hdl=%d type=0x%x svc_inst=%d sdp_hdl=0x%x\n",
                      p_sreg->s_hdl, p_sreg->e_hdl,
                      p_sreg->type,  p_sreg->service_instance,
                      p_sreg->sdp_handle);


    if ( (p_buf = gatt_sr_is_new_srv_chg(&p_list->asgn_range.app_uuid128,
                                         &p_list->asgn_range.svc_uuid,
                                         p_list->asgn_range.svc_inst)) != NULL) {
        gatt_proc_srv_chg();
        /* remove the new service element after the srv changed processing is completed*/

        osi_free(fixed_queue_try_remove_from_queue(gatt_cb.pending_new_srv_start_q, p_buf));
    }
    return GATT_SUCCESS;
}

/*******************************************************************************
**
** Function         GATTS_StopService
**
** Description      This function is called to stop a service
**
** Parameter         service_handle : this is the start handle of a service
**
** Returns          None.
**
*******************************************************************************/
void GATTS_StopService (UINT16 service_handle)
{
    UINT8           ii = gatt_sr_find_i_rcb_by_handle(service_handle);

    GATT_TRACE_API("GATTS_StopService %u", service_handle);

    /* Index 0 is reserved for GATT, and is never stopped */
    if ( (ii > 0) && (ii < GATT_MAX_SR_PROFILES) && (gatt_cb.sr_reg[ii].in_use) ) {
#if(SDP_INCLUDED == TRUE)
        if (gatt_cb.sr_reg[ii].sdp_handle) {
            SDP_DeleteRecord(gatt_cb.sr_reg[ii].sdp_handle);
        }
#endif  ///SDP_INCLUDED == TRUE
        gatt_remove_a_srv_from_list(&gatt_cb.srv_list_info, &gatt_cb.srv_list[ii]);
        gatt_cb.srv_list[ii].in_use = FALSE;
        memset (&gatt_cb.sr_reg[ii], 0, sizeof(tGATT_SR_REG));
    } else {
        GATT_TRACE_ERROR("GATTS_StopService service_handle: %u is not in use", service_handle);
    }
}
/*******************************************************************************
**
** Function         GATTs_HandleValueIndication
**
** Description      This function sends a handle value indication to a client.
**
** Parameter        conn_id: connection identifier.
**                  attr_handle: Attribute handle of this handle value indication.
**                  val_len: Length of the indicated attribute value.
**                  p_val: Pointer to the indicated attribute value data.
**
** Returns          GATT_SUCCESS if sucessfully sent or queued; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS GATTS_HandleValueIndication (UINT16 conn_id,  UINT16 attr_handle, UINT16 val_len, UINT8 *p_val)
{
    tGATT_STATUS    cmd_status = GATT_NO_RESOURCES;

    tGATT_VALUE      indication;
    BT_HDR          *p_msg;
    tGATT_VALUE     *p_buf;
    tGATT_IF         gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);


    GATT_TRACE_API ("GATTS_HandleValueIndication");
    if ( (p_reg == NULL) || (p_tcb == NULL)) {
        GATT_TRACE_ERROR ("GATTS_HandleValueIndication Unknown  conn_id: %u ", conn_id);
        return (tGATT_STATUS) GATT_INVALID_CONN_ID;
    }

    if (! GATT_HANDLE_IS_VALID (attr_handle)) {
        return GATT_ILLEGAL_PARAMETER;
    }

    indication.conn_id  = conn_id;
    indication.handle   = attr_handle;
    indication.len      = val_len;
    memcpy (indication.value, p_val, val_len);
    indication.auth_req = GATT_AUTH_REQ_NONE;

    if (GATT_HANDLE_IS_VALID(p_tcb->indicate_handle)) {
        GATT_TRACE_DEBUG ("Add a pending indication");
        if ((p_buf = gatt_add_pending_ind(p_tcb, &indication)) != NULL) {
            cmd_status = GATT_SUCCESS;
        } else {
            cmd_status = GATT_NO_RESOURCES;
        }
    } else {

        if ( (p_msg = attp_build_sr_msg (p_tcb, GATT_HANDLE_VALUE_IND, (tGATT_SR_MSG *)&indication)) != NULL) {
            cmd_status = attp_send_sr_msg (p_tcb, p_msg);

            if (cmd_status == GATT_SUCCESS || cmd_status == GATT_CONGESTED) {
                p_tcb->indicate_handle = indication.handle;
                gatt_start_conf_timer(p_tcb);
            }
        }
    }
    return cmd_status;
}

/*******************************************************************************
**
** Function         GATTS_HandleValueNotification
**
** Description      This function sends a handle value notification to a client.
**
** Parameter        conn_id: connection identifier.
**                  attr_handle: Attribute handle of this handle value indication.
**                  val_len: Length of the indicated attribute value.
**                  p_val: Pointer to the indicated attribute value data.
**
** Returns          GATT_SUCCESS if sucessfully sent; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS GATTS_HandleValueNotification (UINT16 conn_id, UINT16 attr_handle,
        UINT16 val_len, UINT8 *p_val)
{
    tGATT_STATUS    cmd_sent = GATT_ILLEGAL_PARAMETER;
    BT_HDR          *p_buf;
    tGATT_VALUE     notif;
    tGATT_IF         gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);

    GATT_TRACE_API ("GATTS_HandleValueNotification");

    if ( (p_reg == NULL) || (p_tcb == NULL)) {
        GATT_TRACE_ERROR ("GATTS_HandleValueNotification Unknown  conn_id: %u \n", conn_id);
        return (tGATT_STATUS) GATT_INVALID_CONN_ID;
    }

    if (GATT_HANDLE_IS_VALID (attr_handle)) {
        notif.handle    = attr_handle;
        notif.len       = val_len;
        memcpy (notif.value, p_val, val_len);
        notif.auth_req = GATT_AUTH_REQ_NONE;

        if ((p_buf = attp_build_sr_msg (p_tcb, GATT_HANDLE_VALUE_NOTIF, (tGATT_SR_MSG *)&notif))
                != NULL) {
            cmd_sent = attp_send_sr_msg (p_tcb, p_buf);
        } else {
            cmd_sent = GATT_NO_RESOURCES;
        }
    }
    return cmd_sent;
}

/*******************************************************************************
**
** Function         GATTS_SendRsp
**
** Description      This function sends the server response to client.
**
** Parameter        conn_id: connection identifier.
**                  trans_id: transaction id
**                  status: response status
**                  p_msg: pointer to message parameters structure.
**
** Returns          GATT_SUCCESS if sucessfully sent; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS GATTS_SendRsp (UINT16 conn_id,  UINT32 trans_id,
                            tGATT_STATUS status, tGATTS_RSP *p_msg)
{
    tGATT_STATUS cmd_sent = GATT_ILLEGAL_PARAMETER;
    tGATT_IF         gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);

    GATT_TRACE_API ("GATTS_SendRsp: conn_id: %u  trans_id: %u  Status: 0x%04x\n",
                    conn_id, trans_id, status);

    if ( (p_reg == NULL) || (p_tcb == NULL)) {
        GATT_TRACE_ERROR ("GATTS_SendRsp Unknown  conn_id: %u\n", conn_id);
        return (tGATT_STATUS) GATT_INVALID_CONN_ID;
    }

    if (p_tcb->sr_cmd.trans_id != trans_id) {
        GATT_TRACE_ERROR ("GATTS_SendRsp conn_id: %u  waiting for op_code = %02x\n",
                          conn_id, p_tcb->sr_cmd.op_code);

        return (GATT_WRONG_STATE);
    }
    /* Process App response */
    cmd_sent = gatt_sr_process_app_rsp (p_tcb,  gatt_if, trans_id, p_tcb->sr_cmd.op_code, status, p_msg);

    return cmd_sent;
}


/*******************************************************************************
**
** Function         GATTS_SetAttributeValue
**
** Description      This function sends to set the attribute value .
**
** Parameter        attr_handle:the attribute handle
**                  length: the attribute length
**                  value: the value to be set to the attribute in the database
**
** Returns          GATT_SUCCESS if successfully sent; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS GATTS_SetAttributeValue(UINT16 attr_handle, UINT16 length, UINT8 *value)
{
    tGATT_STATUS status;
    tGATT_HDL_LIST_ELEM  *p_decl = NULL;

    GATT_TRACE_DEBUG("GATTS_SetAttributeValue: attr_handle: %u  length: %u \n",
                    attr_handle, length);

    if ((p_decl = gatt_find_hdl_buffer_by_attr_handle(attr_handle)) == NULL) {
        GATT_TRACE_DEBUG("Service not created\n"); 
        return GATT_INVALID_HANDLE;
    }

    status =  gatts_set_attribute_value(&p_decl->svc_db, attr_handle, length, value);
    return status;

}


/*******************************************************************************
**
** Function         GATTS_GetAttributeValue
**
** Description      This function sends to set the attribute value .
**
** Parameter        attr_handle: the attribute handle
**                  length:the attribute value length in the database
**                  value: the attribute value out put
**
** Returns          GATT_SUCCESS if successfully sent; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS GATTS_GetAttributeValue(UINT16 attr_handle, UINT16 *length, UINT8 **value)
{
     tGATT_STATUS status;
     tGATT_HDL_LIST_ELEM  *p_decl;

     GATT_TRACE_DEBUG("GATTS_GetAttributeValue: attr_handle: %u\n",
                    attr_handle);

     if ((p_decl = gatt_find_hdl_buffer_by_attr_handle(attr_handle)) == NULL) {
         GATT_TRACE_ERROR("Service not created\n"); 
         return GATT_INVALID_HANDLE;
     }

     status =  gatts_get_attribute_value(&p_decl->svc_db, attr_handle, length, value);
     return status;
}
#endif  ///GATTS_INCLUDED == TRUE


#if (GATTC_INCLUDED == TRUE)
/*******************************************************************************/
/* GATT Profile Srvr Functions */
/*******************************************************************************/

/*******************************************************************************/
/*                                                                             */
/*                   GATT CLIENT APIs                                          */
/*                                                                             */
/*******************************************************************************/


/*******************************************************************************
**
** Function         GATTC_ConfigureMTU
**
** Description      This function is called to configure the ATT MTU size.
**
** Parameters       conn_id: connection identifier.
**                  mtu    - attribute MTU size..
**
** Returns          GATT_SUCCESS if command started successfully.
**
*******************************************************************************/
tGATT_STATUS GATTC_ConfigureMTU (UINT16 conn_id, UINT16 mtu)
{
    UINT8           ret = GATT_NO_RESOURCES;
    tGATT_IF        gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);

    tGATT_CLCB    *p_clcb;

    GATT_TRACE_API ("GATTC_ConfigureMTU conn_id=%d mtu=%d", conn_id, mtu );

    /* Validate that the link is BLE, not BR/EDR */
    if (p_tcb->transport != BT_TRANSPORT_LE) {
        return GATT_ERROR;
    }

    if ( (p_tcb == NULL) || (p_reg == NULL) || (mtu < GATT_DEF_BLE_MTU_SIZE) || (mtu > GATT_MAX_MTU_SIZE)) {
        return GATT_ILLEGAL_PARAMETER;
    }

    if (gatt_is_clcb_allocated(conn_id)) {
        GATT_TRACE_ERROR("GATTC_ConfigureMTU GATT_BUSY conn_id = %d", conn_id);
        return GATT_BUSY;
    }

    if ((p_clcb = gatt_clcb_alloc(conn_id)) != NULL) {
        p_clcb->p_tcb->payload_size = mtu;
        p_clcb->operation = GATTC_OPTYPE_CONFIG;

        ret = attp_send_cl_msg (p_clcb->p_tcb, p_clcb->clcb_idx, GATT_REQ_MTU, (tGATT_CL_MSG *)&mtu);
    }

    return ret;
}

/*******************************************************************************
**
** Function         GATTC_Discover
**
** Description      This function is called to do a discovery procedure on ATT server.
**
** Parameters       conn_id: connection identifier.
**                  disc_type:discovery type.
**                  p_param: parameters of discovery requirement.
**
** Returns          GATT_SUCCESS if command received/sent successfully.
**
*******************************************************************************/
tGATT_STATUS GATTC_Discover (UINT16 conn_id, tGATT_DISC_TYPE disc_type,
                             tGATT_DISC_PARAM *p_param)
{
    tGATT_STATUS    status = GATT_SUCCESS;
    tGATT_CLCB      *p_clcb;
    tGATT_IF        gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);


    GATT_TRACE_API ("GATTC_Discover conn_id=%d disc_type=%d", conn_id, disc_type);

    if ( (p_tcb == NULL) || (p_reg == NULL) || (p_param == NULL) ||
            (disc_type >= GATT_DISC_MAX)) {
        GATT_TRACE_ERROR("GATTC_Discover Illegal param: disc_type %d conn_id = %d", disc_type, conn_id);
        return GATT_ILLEGAL_PARAMETER;
    }


    if (gatt_is_clcb_allocated(conn_id)) {
        GATT_TRACE_ERROR("GATTC_Discover GATT_BUSY conn_id = %d", conn_id);
        return GATT_BUSY;
    }


    if ((p_clcb = gatt_clcb_alloc(conn_id)) != NULL ) {
        if (!GATT_HANDLE_IS_VALID(p_param->s_handle) ||
                !GATT_HANDLE_IS_VALID(p_param->e_handle) ||
                /* search by type does not have a valid UUID param */
                (disc_type == GATT_DISC_SRVC_BY_UUID &&
                 p_param->service.len == 0)) {
            gatt_clcb_dealloc(p_clcb);
            return GATT_ILLEGAL_PARAMETER;
        }

        p_clcb->operation  = GATTC_OPTYPE_DISCOVERY;
        p_clcb->op_subtype = disc_type;
        p_clcb->s_handle   = p_param->s_handle;
        p_clcb->e_handle   = p_param->e_handle;
        p_clcb->uuid       = p_param->service;

        gatt_act_discovery(p_clcb);
    } else {
        status = GATT_NO_RESOURCES;
    }
    return status;
}

/*******************************************************************************
**
** Function         GATTC_Read
**
** Description      This function is called to read the value of an attribute from
**                  the server.
**
** Parameters       conn_id: connection identifier.
**                  type    - attribute read type.
**                  p_read  - read operation parameters.
**
** Returns          GATT_SUCCESS if command started successfully.
**
*******************************************************************************/
tGATT_STATUS GATTC_Read (UINT16 conn_id, tGATT_READ_TYPE type, tGATT_READ_PARAM *p_read)
{
    tGATT_STATUS status = GATT_SUCCESS;
    tGATT_CLCB          *p_clcb;
    tGATT_READ_MULTI    *p_read_multi;
    tGATT_IF            gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8               tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB           *p_tcb = gatt_get_tcb_by_idx(tcb_idx);
    tGATT_REG           *p_reg = gatt_get_regcb(gatt_if);


    GATT_TRACE_API ("GATTC_Read conn_id=%d type=%d", conn_id, type);

    if ( (p_tcb == NULL) || (p_reg == NULL) || (p_read == NULL) || ((type >= GATT_READ_MAX) || (type == 0))) {
        GATT_TRACE_ERROR("GATT_Read Illegal param: conn_id %d, type 0%d,", conn_id, type);
        return GATT_ILLEGAL_PARAMETER;
    }

    if (gatt_is_clcb_allocated(conn_id)) {
        GATT_TRACE_ERROR("GATTC_Read GATT_BUSY conn_id = %d", conn_id);
        return GATT_BUSY;
    }

    if ( (p_clcb = gatt_clcb_alloc(conn_id)) != NULL  ) {
        p_clcb->operation = GATTC_OPTYPE_READ;
        p_clcb->op_subtype = type;
        p_clcb->auth_req = p_read->by_handle.auth_req;
        p_clcb->counter = 0;

        switch (type) {
        case GATT_READ_BY_TYPE:
        case GATT_READ_CHAR_VALUE:
            p_clcb->s_handle = p_read->service.s_handle;
            p_clcb->e_handle = p_read->service.e_handle;
            memcpy(&p_clcb->uuid, &p_read->service.uuid, sizeof(tBT_UUID));
            break;
        case GATT_READ_MULTIPLE:
            p_clcb->s_handle = 0;
            /* copy multiple handles in CB */
            p_read_multi = (tGATT_READ_MULTI *)osi_malloc(sizeof(tGATT_READ_MULTI));
            p_clcb->p_attr_buf = (UINT8 *)p_read_multi;
            memcpy (p_read_multi, &p_read->read_multiple, sizeof(tGATT_READ_MULTI));
        case GATT_READ_BY_HANDLE:
        case GATT_READ_PARTIAL:
            memset(&p_clcb->uuid, 0, sizeof(tBT_UUID));
            p_clcb->s_handle = p_read->by_handle.handle;

            if (type == GATT_READ_PARTIAL) {
                p_clcb->counter = p_read->partial.offset;
            }

            break;
        default:
            break;
        }
        /* start security check */
        if (gatt_security_check_start(p_clcb) == FALSE) {
            status = GATT_NO_RESOURCES;
            gatt_clcb_dealloc(p_clcb);
        }
    } else {
        status = GATT_NO_RESOURCES;
    }
    return status;
}

/*******************************************************************************
**
** Function         GATTC_Write
**
** Description      This function is called to write the value of an attribute to
**                  the server.
**
** Parameters       conn_id: connection identifier.
**                  type    - attribute write type.
**                  p_write  - write operation parameters.
**
** Returns          GATT_SUCCESS if command started successfully.
**
*******************************************************************************/
tGATT_STATUS GATTC_Write (UINT16 conn_id, tGATT_WRITE_TYPE type, tGATT_VALUE *p_write)
{
    tGATT_STATUS status = GATT_SUCCESS;
    tGATT_CLCB      *p_clcb;
    tGATT_VALUE     *p;
    tGATT_IF        gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);

    if ( (p_tcb == NULL) || (p_reg == NULL) || (p_write == NULL) ||
            ((type != GATT_WRITE) && (type != GATT_WRITE_PREPARE) && (type != GATT_WRITE_NO_RSP)) ) {
        GATT_TRACE_ERROR("GATT_Write Illegal param: conn_id %d, type 0%d,", conn_id, type);
        return GATT_ILLEGAL_PARAMETER;
    }

    if (gatt_is_clcb_allocated(conn_id)) {
        GATT_TRACE_ERROR("GATTC_Write GATT_BUSY conn_id = %d", conn_id);
        return GATT_BUSY;
    }

    if ((p_clcb = gatt_clcb_alloc(conn_id)) != NULL ) {
        p_clcb->operation  = GATTC_OPTYPE_WRITE;
        p_clcb->op_subtype = type;
        p_clcb->auth_req = p_write->auth_req;

        if (( p_clcb->p_attr_buf = (UINT8 *)osi_malloc((UINT16)sizeof(tGATT_VALUE))) != NULL) {
            memcpy(p_clcb->p_attr_buf, (void *)p_write, sizeof(tGATT_VALUE));

            p =  (tGATT_VALUE *)p_clcb->p_attr_buf;
            if (type == GATT_WRITE_PREPARE) {
                p_clcb->start_offset = p_write->offset;
                p->offset = 0;
            }

            if (gatt_security_check_start(p_clcb) == FALSE) {
                status = GATT_NO_RESOURCES;
            }
        } else {
            status = GATT_NO_RESOURCES;
        }

        if (status == GATT_NO_RESOURCES) {
            gatt_clcb_dealloc(p_clcb);
        }
    } else {
        status = GATT_NO_RESOURCES;
    }
    return status;
}


/*******************************************************************************
**
** Function         GATTC_ExecuteWrite
**
** Description      This function is called to send an Execute write request to
**                  the server.
**
** Parameters       conn_id: connection identifier.
**                  is_execute - to execute or cancel the prepare write requet(s)
**
** Returns          GATT_SUCCESS if command started successfully.
**
*******************************************************************************/
tGATT_STATUS GATTC_ExecuteWrite (UINT16 conn_id, BOOLEAN is_execute)
{
    tGATT_STATUS status = GATT_SUCCESS;
    tGATT_CLCB      *p_clcb;
    tGATT_EXEC_FLAG flag;
    tGATT_IF        gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);

    GATT_TRACE_API ("GATTC_ExecuteWrite conn_id=%d is_execute=%d", conn_id, is_execute);

    if ( (p_tcb == NULL) || (p_reg == NULL) ) {
        GATT_TRACE_ERROR("GATTC_ExecuteWrite Illegal param: conn_id %d", conn_id);
        return GATT_ILLEGAL_PARAMETER;
    }

    if (gatt_is_clcb_allocated(conn_id)) {
        GATT_TRACE_ERROR("GATTC_Write GATT_BUSY conn_id = %d", conn_id);
        return GATT_BUSY;
    }

    if ((p_clcb = gatt_clcb_alloc(conn_id)) != NULL) {
        p_clcb->operation  = GATTC_OPTYPE_EXE_WRITE;
        flag = is_execute ? GATT_PREP_WRITE_EXEC : GATT_PREP_WRITE_CANCEL;
        gatt_send_queue_write_cancel (p_clcb->p_tcb, p_clcb, flag);
    } else {
        GATT_TRACE_ERROR("Unable to allocate client CB for conn_id %d ", conn_id);
        status = GATT_NO_RESOURCES;
    }
    return status;
}

/*******************************************************************************
**
** Function         GATTC_SendHandleValueConfirm
**
** Description      This function is called to send a handle value confirmation
**                  as response to a handle value notification from server.
**
** Parameters       conn_id: connection identifier.
**                  handle: the handle of the attribute confirmation.
**
** Returns          GATT_SUCCESS if command started successfully.
**
*******************************************************************************/
tGATT_STATUS GATTC_SendHandleValueConfirm (UINT16 conn_id, UINT16 handle)
{
    tGATT_STATUS    ret = GATT_ILLEGAL_PARAMETER;
    tGATT_TCB     *p_tcb = gatt_get_tcb_by_idx(GATT_GET_TCB_IDX(conn_id));

    GATT_TRACE_API ("GATTC_SendHandleValueConfirm conn_id=%d handle=0x%x", conn_id, handle);

    if (p_tcb) {
        if (p_tcb->ind_count > 0 ) {
            btu_stop_timer (&p_tcb->ind_ack_timer_ent);

            GATT_TRACE_DEBUG ("notif_count=%d ", p_tcb->ind_count);
            /* send confirmation now */
            ret = attp_send_cl_msg(p_tcb, 0, GATT_HANDLE_VALUE_CONF, (tGATT_CL_MSG *)&handle);

            p_tcb->ind_count = 0;

        } else {
            GATT_TRACE_DEBUG ("GATTC_SendHandleValueConfirm - conn_id: %u - ignored not waiting for indicaiton ack", conn_id);
            ret = GATT_SUCCESS;
        }
    } else {
        GATT_TRACE_ERROR ("GATTC_SendHandleValueConfirm - Unknown conn_id: %u", conn_id);
    }
    return ret;
}

#endif  ///GATTC_INCLUDED == TRUE

/*******************************************************************************/
/*                                                                             */
/*                   GATT  APIs                                                */
/*                                                                             */
/*******************************************************************************/
/*******************************************************************************
**
** Function         GATT_SetIdleTimeout
**
** Description      This function (common to both client and server) sets the idle
**                  timeout for a tansport connection
**
** Parameter        bd_addr:   target device bd address.
**                  idle_tout: timeout value in seconds.
**
** Returns          void
**
*******************************************************************************/
void GATT_SetIdleTimeout (BD_ADDR bd_addr, UINT16 idle_tout, tBT_TRANSPORT transport)
{
    tGATT_TCB       *p_tcb;
    BOOLEAN         status = FALSE;

    if ((p_tcb = gatt_find_tcb_by_addr (bd_addr, transport)) != NULL) {
        if (p_tcb->att_lcid == L2CAP_ATT_CID) {
            status = L2CA_SetFixedChannelTout (bd_addr, L2CAP_ATT_CID, idle_tout);

            if (idle_tout == GATT_LINK_IDLE_TIMEOUT_WHEN_NO_APP)
                L2CA_SetIdleTimeoutByBdAddr(p_tcb->peer_bda,
                                            GATT_LINK_IDLE_TIMEOUT_WHEN_NO_APP, BT_TRANSPORT_LE);
        } else {
            status = L2CA_SetIdleTimeout (p_tcb->att_lcid, idle_tout, FALSE);
        }
    }

    GATT_TRACE_API ("GATT_SetIdleTimeout idle_tout=%d status=%d(1-OK 0-not performed)",
                    idle_tout, status);
}


/*******************************************************************************
**
** Function         GATT_Register
**
** Description      This function is called to register an  application
**                  with GATT
**
** Parameter        p_app_uuid128: Application UUID
**                  p_cb_info: callback functions.
**
** Returns          0 for error, otherwise the index of the client registered with GATT
**
*******************************************************************************/
tGATT_IF GATT_Register (tBT_UUID *p_app_uuid128, tGATT_CBACK *p_cb_info)
{
    tGATT_REG    *p_reg;
    UINT8        i_gatt_if = 0;
    tGATT_IF     gatt_if = 0;

    GATT_TRACE_API ("GATT_Register");
    gatt_dbg_display_uuid(*p_app_uuid128);

    for (i_gatt_if = 0, p_reg = gatt_cb.cl_rcb; i_gatt_if < GATT_MAX_APPS; i_gatt_if++, p_reg++) {
        if (p_reg->in_use  && !memcmp(p_app_uuid128->uu.uuid128, p_reg->app_uuid128.uu.uuid128, LEN_UUID_128)) {
            GATT_TRACE_ERROR("application already registered.");
            return 0;
        }
    }

    for (i_gatt_if = 0, p_reg = gatt_cb.cl_rcb; i_gatt_if < GATT_MAX_APPS; i_gatt_if++, p_reg++) {
        if (!p_reg->in_use) {
            memset(p_reg, 0 , sizeof(tGATT_REG));
            i_gatt_if++;              /* one based number */
            p_reg->app_uuid128 =  *p_app_uuid128;
            gatt_if            =
                p_reg->gatt_if     = (tGATT_IF)i_gatt_if;
            p_reg->app_cb      = *p_cb_info;
            p_reg->in_use      = TRUE;

            break;
        }
    }
    GATT_TRACE_API ("allocated gatt_if=%d\n", gatt_if);
    return gatt_if;
}


/*******************************************************************************
**
** Function         GATT_Deregister
**
** Description      This function deregistered the application from GATT.
**
** Parameters       gatt_if: applicaiton interface.
**
** Returns          None.
**
*******************************************************************************/
void GATT_Deregister (tGATT_IF gatt_if)
{
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    tGATT_TCB       *p_tcb;
    tGATT_CLCB      *p_clcb;
    UINT8           i, j;
#if (GATTS_INCLUDED == TRUE)
    UINT8           ii;
    tGATT_SR_REG    *p_sreg;
#endif  ///GATTS_INCLUDED == TRUE
    GATT_TRACE_API ("GATT_Deregister gatt_if=%d", gatt_if);
    /* Index 0 is GAP and is never deregistered */
    if ( (gatt_if == 0) || (p_reg == NULL) ) {
        GATT_TRACE_ERROR ("GATT_Deregister with invalid gatt_if: %u", gatt_if);
        return;
    }

    /* stop all services  */
    /* todo an applcaiton can not be deregistered if its services is also used by other application
      deregisteration need to bed performed in an orderly fashion
      no check for now */
#if (GATTS_INCLUDED == TRUE)
    for (ii = 0, p_sreg = gatt_cb.sr_reg; ii < GATT_MAX_SR_PROFILES; ii++, p_sreg++) {
        if (p_sreg->in_use && (p_sreg->gatt_if == gatt_if)) {
            GATTS_StopService(p_sreg->s_hdl);
        }
    }
    /* free all services db buffers if owned by this application */
    gatt_free_srvc_db_buffer_app_id(&p_reg->app_uuid128);
#endif  ///GATTS_INCLUDED == TRUE
    /* When an application deregisters, check remove the link associated with the app */

    for (i = 0, p_tcb = gatt_cb.tcb; i < GATT_MAX_PHY_CHANNEL; i++, p_tcb++) {
        if (p_tcb->in_use) {
            if (gatt_get_ch_state(p_tcb) != GATT_CH_CLOSE) {
                gatt_update_app_use_link_flag(gatt_if, p_tcb,  FALSE, FALSE);
                if (!gatt_num_apps_hold_link(p_tcb)) {
                    /* this will disconnect the link or cancel the pending connect request at lower layer*/
                    gatt_disconnect(p_tcb);
                }
            }

            for (j = 0, p_clcb = &gatt_cb.clcb[j]; j < GATT_CL_MAX_LCB; j++, p_clcb++) {
                if (p_clcb->in_use &&
                        (p_clcb->p_reg->gatt_if == gatt_if) &&
                        (p_clcb->p_tcb->tcb_idx == p_tcb->tcb_idx)) {
                    btu_stop_timer(&p_clcb->rsp_timer_ent);
                    gatt_clcb_dealloc (p_clcb);
                    break;
                }
            }
        }
    }

    gatt_deregister_bgdev_list(gatt_if);
    /* update the listen mode */
#if (defined(BLE_PERIPHERAL_MODE_SUPPORT) && (BLE_PERIPHERAL_MODE_SUPPORT == TRUE))
    GATT_Listen(gatt_if, FALSE, NULL);
#endif

    memset (p_reg, 0, sizeof(tGATT_REG));
}


/*******************************************************************************
**
** Function         GATT_StartIf
**
** Description      This function is called after registration to start receiving
**                  callbacks for registered interface.  Function may call back
**                  with connection status and queued notifications
**
** Parameter        gatt_if: applicaiton interface.
**
** Returns          None.
**
*******************************************************************************/
void GATT_StartIf (tGATT_IF gatt_if)
{
    tGATT_REG   *p_reg;
    tGATT_TCB   *p_tcb;
    BD_ADDR     bda;
    UINT8       start_idx, found_idx;
    UINT16      conn_id;
    tGATT_TRANSPORT transport ;

    GATT_TRACE_API ("GATT_StartIf gatt_if=%d", gatt_if);
    if ((p_reg = gatt_get_regcb(gatt_if)) != NULL) {
        start_idx = 0;
        while (gatt_find_the_connected_bda(start_idx, bda, &found_idx, &transport)) {
            p_tcb = gatt_find_tcb_by_addr(bda, transport);
            if (p_reg->app_cb.p_conn_cb && p_tcb) {
                conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, gatt_if);
                (*p_reg->app_cb.p_conn_cb)(gatt_if, bda, conn_id, TRUE, 0, transport);
            }
            start_idx = ++found_idx;
        }
    }
}


/*******************************************************************************
**
** Function         GATT_Connect
**
** Description      This function initiate a connecttion to a remote device on GATT
**                  channel.
**
** Parameters       gatt_if: applicaiton interface
**                  bd_addr: peer device address.
**                  is_direct: is a direct conenection or a background auto connection
**
** Returns          TRUE if connection started; FALSE if connection start failure.
**
*******************************************************************************/
BOOLEAN GATT_Connect (tGATT_IF gatt_if, BD_ADDR bd_addr, BOOLEAN is_direct, tBT_TRANSPORT transport)
{
    tGATT_REG    *p_reg;
    BOOLEAN status = FALSE;

    GATT_TRACE_API ("GATT_Connect gatt_if=%d", gatt_if);

    /* Make sure app is registered */
    if ((p_reg = gatt_get_regcb(gatt_if)) == NULL) {
        GATT_TRACE_ERROR("GATT_Connect - gatt_if =%d is not registered", gatt_if);
        return (FALSE);
    }

    if (is_direct) {
        status = gatt_act_connect (p_reg, bd_addr, transport);
    } else {
        if (transport == BT_TRANSPORT_LE) {
            status = gatt_update_auto_connect_dev(gatt_if, TRUE, bd_addr, TRUE);
        } else {
            GATT_TRACE_ERROR("Unsupported transport for background connection");
        }
    }

    return status;

}

/*******************************************************************************
**
** Function         GATT_CancelConnect
**
** Description      This function terminate the connection initaition to a remote
**                  device on GATT channel.
**
** Parameters       gatt_if: client interface. If 0 used as unconditionally disconnect,
**                          typically used for direct connection cancellation.
**                  bd_addr: peer device address.
**
** Returns          TRUE if connection started; FALSE if connection start failure.
**
*******************************************************************************/
BOOLEAN GATT_CancelConnect (tGATT_IF gatt_if, BD_ADDR bd_addr, BOOLEAN is_direct)
{
    tGATT_REG     *p_reg;
    tGATT_TCB     *p_tcb;
    BOOLEAN       status = TRUE;
    tGATT_IF      temp_gatt_if;
    UINT8         start_idx, found_idx;

    GATT_TRACE_API ("GATT_CancelConnect gatt_if=%d", gatt_if);

    if ((gatt_if != 0) && ((p_reg = gatt_get_regcb(gatt_if)) == NULL)) {
        GATT_TRACE_ERROR("GATT_CancelConnect - gatt_if =%d is not registered", gatt_if);
        return (FALSE);
    }

    if (is_direct) {
        if (!gatt_if) {
            GATT_TRACE_DEBUG("GATT_CancelConnect - unconditional");
            start_idx = 0;
            /* only LE connection can be cancelled */
            p_tcb = gatt_find_tcb_by_addr(bd_addr, BT_TRANSPORT_LE);
            if (p_tcb && gatt_num_apps_hold_link(p_tcb)) {
                while (status && gatt_find_app_hold_link(p_tcb, start_idx, &found_idx, &temp_gatt_if)) {
                    status = gatt_cancel_open(temp_gatt_if, bd_addr);
                    start_idx = ++found_idx;
                }
            } else {
                GATT_TRACE_ERROR("GATT_CancelConnect - no app found");
                status = FALSE;
            }
        } else {
            status = gatt_cancel_open(gatt_if, bd_addr);
        }
    } else {
        if (!gatt_if) {
            if (gatt_get_num_apps_for_bg_dev(bd_addr)) {
                while (gatt_find_app_for_bg_dev(bd_addr, &temp_gatt_if)) {
                    gatt_remove_bg_dev_for_app(temp_gatt_if, bd_addr);
                }
            } else {
                GATT_TRACE_ERROR("GATT_CancelConnect -no app associated with the bg device for unconditional removal");
                status = FALSE;
            }
        } else {
            status = gatt_remove_bg_dev_for_app(gatt_if, bd_addr);
        }
    }

    return status;
}

/*******************************************************************************
**
** Function         GATT_Disconnect
**
** Description      This function disconnect the GATT channel for this registered
**                  application.
**
** Parameters       conn_id: connection identifier.
**
** Returns          GATT_SUCCESS if disconnected.
**
*******************************************************************************/
tGATT_STATUS GATT_Disconnect (UINT16 conn_id)
{
    tGATT_STATUS    ret = GATT_ILLEGAL_PARAMETER;
    tGATT_TCB       *p_tcb = NULL;
    tGATT_IF        gatt_if = GATT_GET_GATT_IF(conn_id);
    UINT8          tcb_idx = GATT_GET_TCB_IDX(conn_id);

    GATT_TRACE_API ("GATT_Disconnect conn_id=%d ", conn_id);

    p_tcb = gatt_get_tcb_by_idx(tcb_idx);

    if (p_tcb) {
        gatt_update_app_use_link_flag(gatt_if, p_tcb, FALSE, FALSE);
        if (!gatt_num_apps_hold_link(p_tcb)) {
            gatt_disconnect(p_tcb);
        }
        ret = GATT_SUCCESS;
    }
    return ret;
}


/*******************************************************************************
**
** Function         GATT_GetConnectionInfor
**
** Description      This function use conn_id to find its associated BD address and applciation
**                  interface
**
** Parameters        conn_id: connection id  (input)
**                   p_gatt_if: applicaiton interface (output)
**                   bd_addr: peer device address. (output)
**
** Returns          TRUE the ligical link information is found for conn_id
**
*******************************************************************************/
BOOLEAN GATT_GetConnectionInfor(UINT16 conn_id, tGATT_IF *p_gatt_if, BD_ADDR bd_addr,
                                tBT_TRANSPORT *p_transport)
{

    tGATT_IF        gatt_if = GATT_GET_GATT_IF(conn_id);
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    UINT8           tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB       *p_tcb = gatt_get_tcb_by_idx(tcb_idx);
    BOOLEAN         status = FALSE;

    GATT_TRACE_API ("GATT_GetConnectionInfor conn_id=%d", conn_id);

    if (p_tcb && p_reg ) {
        memcpy(bd_addr, p_tcb->peer_bda, BD_ADDR_LEN);
        *p_gatt_if = gatt_if;
        *p_transport = p_tcb->transport;
        status = TRUE;
    }
    return status;
}


/*******************************************************************************
**
** Function         GATT_GetConnIdIfConnected
**
** Description      This function find the conn_id if the logical link for BD address
**                  and applciation interface is connected
**
** Parameters        gatt_if: applicaiton interface (input)
**                   bd_addr: peer device address. (input)
**                   p_conn_id: connection id  (output)
**                   transport: transport option
**
** Returns          TRUE the logical link is connected
**
*******************************************************************************/
BOOLEAN GATT_GetConnIdIfConnected(tGATT_IF gatt_if, BD_ADDR bd_addr, UINT16 *p_conn_id,
                                  tBT_TRANSPORT transport)
{
    tGATT_REG       *p_reg = gatt_get_regcb(gatt_if);
    tGATT_TCB       *p_tcb = gatt_find_tcb_by_addr(bd_addr, transport);
    BOOLEAN         status = FALSE;

    if (p_reg && p_tcb && (gatt_get_ch_state(p_tcb) == GATT_CH_OPEN) ) {
        *p_conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, gatt_if);
        status = TRUE;
    }

    GATT_TRACE_API ("GATT_GetConnIdIfConnected status=%d\n", status);
    return status;
}


/*******************************************************************************
**
** Function         GATT_Listen
**
** Description      This function start or stop LE advertisement and listen for
**                  connection.
**
** Parameters       gatt_if: applicaiton interface
**                  p_bd_addr: listen for specific address connection, or NULL for
**                             listen to all device connection.
**                  start: start or stop listening.
**
** Returns          TRUE if advertisement is started; FALSE if adv start failure.
**
*******************************************************************************/
BOOLEAN GATT_Listen (tGATT_IF gatt_if, BOOLEAN start, BD_ADDR_PTR bd_addr)
{
    tGATT_REG    *p_reg;

    GATT_TRACE_API ("GATT_Listen gatt_if=%d", gatt_if);

    /* Make sure app is registered */
    if ((p_reg = gatt_get_regcb(gatt_if)) == NULL) {
        GATT_TRACE_ERROR("GATT_Listen - gatt_if =%d is not registered", gatt_if);
        return (FALSE);
    }

    if (bd_addr != NULL) {
        gatt_update_auto_connect_dev(gatt_if, start, bd_addr, FALSE);
    } else {
        p_reg->listening = start ? GATT_LISTEN_TO_ALL : GATT_LISTEN_TO_NONE;
    }

    return gatt_update_listen_mode();
}

#endif

