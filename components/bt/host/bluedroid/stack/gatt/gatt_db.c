/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
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
 *  this file contains GATT database building and query functions
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE

#include "common/bt_trace.h"
#include "osi/allocator.h"

//#include <stdio.h>
#include <string.h>
#include "gatt_int.h"
#include "stack/l2c_api.h"
#include "btm_int.h"

extern tGATT_STATUS gap_proc_read(tGATTS_REQ_TYPE type, tGATT_READ_REQ *p_data, tGATTS_RSP *p_rsp);
extern tGATT_STATUS gatt_proc_read(UINT16 conn_id, tGATTS_REQ_TYPE type, tGATT_READ_REQ *p_data, tGATTS_RSP *p_rsp);

/********************************************************************************
**              L O C A L    F U N C T I O N     P R O T O T Y P E S            *
*********************************************************************************/
static BOOLEAN allocate_svc_db_buf(tGATT_SVC_DB *p_db);
static void *allocate_attr_in_db(tGATT_SVC_DB *p_db, tBT_UUID *p_uuid, tGATT_PERM perm);
static BOOLEAN deallocate_attr_in_db(tGATT_SVC_DB *p_db, void *p_attr);
static BOOLEAN copy_extra_byte_in_db(tGATT_SVC_DB *p_db, void **p_dst, UINT16 len);

static BOOLEAN gatts_db_add_service_declaration(tGATT_SVC_DB *p_db, tBT_UUID *p_service, BOOLEAN is_pri);
static tGATT_STATUS gatts_send_app_read_request(tGATT_TCB *p_tcb, UINT8 op_code,
        UINT16 handle, UINT16 offset, UINT32 trans_id, BOOLEAN need_rsp);
static BOOLEAN gatts_add_char_desc_value_check (tGATT_ATTR_VAL *attr_val, tGATTS_ATTR_CONTROL *control);

/*******************************************************************************
**
** Function         gatts_init_service_db
**
** Description      This function initialize a memory space to be a service database.
**
** Parameter        p_db: database pointer.
**                  len: size of the memory space.
**
** Returns          Status of te operation.
**
*******************************************************************************/
BOOLEAN gatts_init_service_db (tGATT_SVC_DB *p_db, tBT_UUID *p_service,  BOOLEAN is_pri,
                               UINT16 s_hdl, UINT16 num_handle)
{
    if (p_db->svc_buffer == NULL) { //in case already alloc
        p_db->svc_buffer = fixed_queue_new(QUEUE_SIZE_MAX);
    }

    if (!allocate_svc_db_buf(p_db)) {
        GATT_TRACE_ERROR("gatts_init_service_db failed, no resources\n");
        return FALSE;
    }

    GATT_TRACE_DEBUG("gatts_init_service_db\n");
    GATT_TRACE_DEBUG("s_hdl = %d num_handle = %d\n", s_hdl, num_handle );

    /* update service database information */
    p_db->next_handle   = s_hdl;
    p_db->end_handle    = s_hdl + num_handle;

    return gatts_db_add_service_declaration(p_db, p_service, is_pri);
}

/*******************************************************************************
**
** Function         gatts_init_service_db
**
** Description      This function initialize a memory space to be a service database.
**
** Parameter        p_db: database pointer.
**                  len: size of the memory space.
**
** Returns          Status of te operation.
**
*******************************************************************************/
tBT_UUID *gatts_get_service_uuid (tGATT_SVC_DB *p_db)
{
    if (!p_db || !p_db->p_attr_list) {
        GATT_TRACE_ERROR("service DB empty\n");

        return NULL;
    } else {
        return &((tGATT_ATTR16 *)p_db->p_attr_list)->p_value->uuid;
    }
}

/*******************************************************************************
**
** Function         gatts_check_attr_readability
**
** Description      check attribute readability
**
** Returns          status of operation.
**
*******************************************************************************/
static tGATT_STATUS gatts_check_attr_readability(tGATT_ATTR16 *p_attr,
        UINT16 offset,
        BOOLEAN read_long,
        tGATT_SEC_FLAG sec_flag,
        UINT8 key_size)
{
    UINT16          min_key_size;
    tGATT_PERM      perm = p_attr->permission;

    UNUSED(offset);
    min_key_size = (((perm & GATT_ENCRYPT_KEY_SIZE_MASK) >> 12));
    if (min_key_size != 0 ) {
        min_key_size += 6;
    }

    if (!(perm & GATT_READ_ALLOWED)) {
        GATT_TRACE_ERROR( "GATT_READ_NOT_PERMIT\n");
        return GATT_READ_NOT_PERMIT;
    }

    if ((perm & GATT_READ_AUTH_REQUIRED ) && !(sec_flag & GATT_SEC_FLAG_LKEY_UNAUTHED) &&
            !(sec_flag & BTM_SEC_FLAG_ENCRYPTED)) {
        GATT_TRACE_ERROR( "GATT_INSUF_AUTHENTICATION\n");
        return GATT_INSUF_AUTHENTICATION;
    }

    if ((perm & GATT_READ_MITM_REQUIRED ) && !(sec_flag & GATT_SEC_FLAG_LKEY_AUTHED)) {
        GATT_TRACE_ERROR( "GATT_INSUF_AUTHENTICATION: MITM Required\n");
        return GATT_INSUF_AUTHENTICATION;
    }

    if ((perm & GATT_READ_ENCRYPTED_REQUIRED ) && !(sec_flag & GATT_SEC_FLAG_ENCRYPTED)) {
        GATT_TRACE_ERROR( "GATT_INSUF_ENCRYPTION\n");
        return GATT_INSUF_ENCRYPTION;
    }

    if ( (perm & GATT_READ_ENCRYPTED_REQUIRED) && (sec_flag & GATT_SEC_FLAG_ENCRYPTED) && (key_size < min_key_size)) {
        GATT_TRACE_ERROR( "GATT_INSUF_KEY_SIZE\n");
        return GATT_INSUF_KEY_SIZE;
    }
    /* LE Authorization check*/
    if ((perm & GATT_READ_AUTHORIZATION) && (!(sec_flag & GATT_SEC_FLAG_LKEY_AUTHED) || !(sec_flag & GATT_SEC_FLAG_AUTHORIZATION))) {
        GATT_TRACE_ERROR( "GATT_INSUF_AUTHORIZATION\n");
        return GATT_INSUF_AUTHORIZATION;
    }

    if (read_long) {
        switch (p_attr->uuid) {
        case GATT_UUID_PRI_SERVICE:
        case GATT_UUID_SEC_SERVICE:
        case GATT_UUID_CHAR_DECLARE:
        case GATT_UUID_INCLUDE_SERVICE:
        case GATT_UUID_CHAR_EXT_PROP:
        case GATT_UUID_CHAR_CLIENT_CONFIG:
        case GATT_UUID_CHAR_SRVR_CONFIG:
        case GATT_UUID_CHAR_PRESENT_FORMAT:
            GATT_TRACE_ERROR("GATT_NOT_LONG\n");
            return GATT_NOT_LONG;

        default:
            break;
        }
    }

    return GATT_SUCCESS;
}

/*******************************************************************************
**
** Function         read_attr_value
**
** Description      Utility function to read an attribute value.
**
** Parameter        p_attr: pointer to the attribute to read.
**                  offset: read offset.
**                  p_value: output parameter to carry out the attribute value.
**                  p_len: output parameter to carry out the attribute length.
**                  read_long: this is a read blob request.
**                  mtu: MTU
**                  sec_flag: current link security status.
**                  key_size: encryption key size.
**
** Returns          status of operation.
**
*******************************************************************************/
static tGATT_STATUS read_attr_value (void *p_attr,
                                     UINT16 offset,
                                     UINT8 **p_data,
                                     BOOLEAN read_long,
                                     UINT16 mtu,
                                     UINT16 *p_len,
                                     tGATT_SEC_FLAG sec_flag,
                                     UINT8 key_size)
{
    UINT16          len = 0, uuid16 = 0;
    UINT8           *p = *p_data;
    tGATT_STATUS    status;
    tGATT_ATTR16    *p_attr16  = (tGATT_ATTR16 *)p_attr;

    GATT_TRACE_DEBUG("read_attr_value uuid=0x%04x perm=0x%0x sec_flag=0x%x offset=%d read_long=%d\n",
                     p_attr16->uuid,
                     p_attr16->permission,
                     sec_flag,
                     offset,
                     read_long);

    status = gatts_check_attr_readability((tGATT_ATTR16 *)p_attr, offset, read_long, sec_flag, key_size);

    if (status != GATT_SUCCESS) {
        return status;
    }

    if (p_attr16->uuid_type == GATT_ATTR_UUID_TYPE_16) {
        uuid16 = p_attr16->uuid;
    }

    status = GATT_NO_RESOURCES;

    if (uuid16 == GATT_UUID_PRI_SERVICE || uuid16 == GATT_UUID_SEC_SERVICE) {
        len = p_attr16->p_value->uuid.len;
        if (mtu >= p_attr16->p_value->uuid.len) {
            gatt_build_uuid_to_stream(&p, p_attr16->p_value->uuid);
            status = GATT_SUCCESS;
        }
    } else if (uuid16 == GATT_UUID_CHAR_DECLARE) {
        len = (((tGATT_ATTR16 *)(p_attr16->p_next))->uuid_type == GATT_ATTR_UUID_TYPE_16) ? 5 : 19;

        if (mtu >= len) {
            UINT8_TO_STREAM(p, p_attr16->p_value->char_decl.property);
            UINT16_TO_STREAM(p, p_attr16->p_value->char_decl.char_val_handle);

            if (((tGATT_ATTR16 *)(p_attr16->p_next))->uuid_type == GATT_ATTR_UUID_TYPE_16) {
                UINT16_TO_STREAM(p, ((tGATT_ATTR16 *)(p_attr16->p_next))->uuid);
            }
            /* convert a 32bits UUID to 128 bits */
            else if (((tGATT_ATTR32 *)(p_attr16->p_next))->uuid_type == GATT_ATTR_UUID_TYPE_32) {
                gatt_convert_uuid32_to_uuid128 (p, ((tGATT_ATTR32 *)(p_attr16->p_next))->uuid);
                p += LEN_UUID_128;
            } else {
                ARRAY_TO_STREAM (p, ((tGATT_ATTR128 *)(p_attr16->p_next))->uuid, LEN_UUID_128);
            }
            status = GATT_SUCCESS;
        }

    } else if (uuid16 == GATT_UUID_INCLUDE_SERVICE) {
        if (p_attr16->p_value->incl_handle.service_type.len == LEN_UUID_16) {
            len = 6;
        } else {
            len = 4;
        }

        if (mtu >= len) {
            UINT16_TO_STREAM(p, p_attr16->p_value->incl_handle.s_handle);
            UINT16_TO_STREAM(p, p_attr16->p_value->incl_handle.e_handle);

            if (p_attr16->p_value->incl_handle.service_type.len == LEN_UUID_16) {
                UINT16_TO_STREAM(p, p_attr16->p_value->incl_handle.service_type.uu.uuid16);
            }
            status = GATT_SUCCESS;
        }
    } else { /* characteristic description or characteristic value */
        if (p_attr16->control.auto_rsp == GATT_RSP_BY_STACK) {
            if (p_attr16->p_value == NULL || p_attr16->p_value->attr_val.attr_val == NULL) {
                status = GATT_UNKNOWN_ERROR;
            }
            else if (offset > p_attr16->p_value->attr_val.attr_len){
			/*if offset equal to max_len, should respond with zero byte value
            //if offset is greater than max_len, should respond with an error*/
                status = GATT_INVALID_OFFSET;
            } else {
                UINT8 *value = (UINT8 *)(p_attr16->p_value->attr_val.attr_val) + offset;
                UINT16 len_left = p_attr16->p_value->attr_val.attr_len - offset;
                len = (mtu >= len_left) ? (len_left) : mtu;
                ARRAY_TO_STREAM(p, value, len);
                status = GATT_STACK_RSP;
            }

        } else {
            status = GATT_PENDING;
        }
    }

    *p_len = len;
    *p_data = p;
    return status;
}

/*******************************************************************************
**
** Function         gatts_db_read_attr_value_by_type
**
** Description      Query attribute value by attribute type.
**
** Parameter        p_db: pointer to the attribute database.
**                  p_rsp: Read By type response data.
**                  s_handle: starting handle of the range we are looking for.
**                  e_handle: ending handle of the range we are looking for.
**                  type: Attribute type.
**                  mtu: MTU.
**                  sec_flag: current link security status.
**                  key_size: encryption key size.
**
** Returns          Status of the operation.
**
*******************************************************************************/
tGATT_STATUS gatts_db_read_attr_value_by_type (tGATT_TCB   *p_tcb,
        tGATT_SVC_DB    *p_db,
        UINT8        op_code,
        BT_HDR      *p_rsp,
        UINT16       s_handle,
        UINT16       e_handle,
        tBT_UUID     type,
        UINT16      *p_len,
        tGATT_SEC_FLAG sec_flag,
        UINT8        key_size,
        UINT32       trans_id,
        UINT16       *p_cur_handle)
{
    tGATT_STATUS status = GATT_NOT_FOUND;
    tGATT_ATTR16  *p_attr;
    UINT16      len = 0;
    UINT8       *p = (UINT8 *)(p_rsp + 1) + p_rsp->len + L2CAP_MIN_OFFSET;
    tBT_UUID    attr_uuid;
#if (defined(BLE_DELAY_REQUEST_ENC) && (BLE_DELAY_REQUEST_ENC == TRUE))
    UINT8       flag;
#endif
    BOOLEAN need_rsp;
    BOOLEAN have_send_request = false;

    if (p_db && p_db->p_attr_list) {
        p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;

        while (p_attr && p_attr->handle <= e_handle) {
            if (p_attr->uuid_type == GATT_ATTR_UUID_TYPE_16) {
                attr_uuid.len = LEN_UUID_16;
                attr_uuid.uu.uuid16 = p_attr->uuid;
            } else if (p_attr->uuid_type == GATT_ATTR_UUID_TYPE_32) {
                attr_uuid.len = LEN_UUID_32;
                attr_uuid.uu.uuid32 = ((tGATT_ATTR32 *)p_attr)->uuid;
            } else {
                attr_uuid.len = LEN_UUID_128;
                memcpy(attr_uuid.uu.uuid128, ((tGATT_ATTR128 *)p_attr)->uuid, LEN_UUID_128);
            }

            if (p_attr->handle >= s_handle && gatt_uuid_compare(type, attr_uuid)) {
                if (*p_len <= 2) {
                    status = GATT_NO_RESOURCES;
                    break;
                }

                UINT16_TO_STREAM (p, p_attr->handle);

                status = read_attr_value ((void *)p_attr, 0, &p, FALSE, (UINT16)(*p_len - 2), &len, sec_flag, key_size);
                if (status == GATT_PENDING) {


                    need_rsp = TRUE;
                    status = gatts_send_app_read_request(p_tcb, op_code, p_attr->handle, 0, trans_id, need_rsp);

                    /* one callback at a time */
                    break;
                } else if (status == GATT_SUCCESS || status == GATT_STACK_RSP) {
                    if (status == GATT_STACK_RSP){
                        need_rsp = FALSE;
                        status = gatts_send_app_read_request(p_tcb, op_code, p_attr->handle, 0, trans_id, need_rsp);
                        if(status == GATT_BUSY)
                            break;

                        if (!have_send_request){
                            have_send_request = true;
                            trans_id = p_tcb->sr_cmd.trans_id;
                        }
                    }

                    if (p_rsp->offset == 0) {
                        p_rsp->offset = len + 2;
                    }

                    if (p_rsp->offset == len + 2) {
                        p_rsp->len += (len  + 2);
                        *p_len -= (len + 2);
                    } else {
                        GATT_TRACE_WARNING("format mismatch");
                        status = GATT_NO_RESOURCES;
                        break;
                    }
                } else {
                    *p_cur_handle = p_attr->handle;
                    break;
                }
            }
            p_attr = (tGATT_ATTR16 *)p_attr->p_next;
        }
    }

#if (defined(BLE_DELAY_REQUEST_ENC) && (BLE_DELAY_REQUEST_ENC == TRUE))
    if (BTM_GetSecurityFlags(p_tcb->peer_bda, &flag)) {
        if ((p_tcb->att_lcid == L2CAP_ATT_CID) && (status == GATT_PENDING) &&
                (type.uu.uuid16 == GATT_UUID_GAP_DEVICE_NAME)) {
            if ((flag & (BTM_SEC_LINK_KEY_KNOWN | BTM_SEC_FLAG_ENCRYPTED)) ==
                    BTM_SEC_LINK_KEY_KNOWN) {
                tACL_CONN         *p;
                p = btm_bda_to_acl(p_tcb->peer_bda, BT_TRANSPORT_LE);
                if ((p != NULL) && (p->link_role == BTM_ROLE_MASTER)) {
                    tBTM_BLE_SEC_ACT sec_act = BTM_BLE_SEC_ENCRYPT;
                    btm_ble_set_encryption(p_tcb->peer_bda, &sec_act, p->link_role);
                }
            }
        }
    }
#endif
    return status;
}

/*******************************************************************************
**
** Function         gatts_add_included_service
**
** Description      This function adds an included service into a database.
**
** Parameter        p_db: database pointer.
**                  inc_srvc_type: included service type.
**
** Returns          Status of the operation.
**
*******************************************************************************/
UINT16 gatts_add_included_service (tGATT_SVC_DB *p_db, UINT16 s_handle, UINT16 e_handle,
                                   tBT_UUID service)
{
    tGATT_ATTR16      *p_attr;
    tBT_UUID         uuid = {LEN_UUID_16, {GATT_UUID_INCLUDE_SERVICE}};

    GATT_TRACE_DEBUG("gatts_add_included_service: s_hdl = 0x%04x e_hdl = 0x%04x uuid = 0x%04x",
                     s_handle, e_handle, service.uu.uuid16);

    if (service.len == 0 || s_handle == 0 || e_handle == 0) {
        GATT_TRACE_ERROR("gatts_add_included_service Illegal Params.");
        return 0;
    }

    BOOLEAN is_include_service_allowed = TRUE;
    // service declaration
    tGATT_ATTR16 *first_attr = (tGATT_ATTR16 *)p_db->p_attr_list;
    if (p_db->p_attr_list != NULL) {
        tGATT_ATTR16 *next_attr = (tGATT_ATTR16 *)first_attr->p_next;
        /* This service already has other attributes */
        while (next_attr != NULL) {
            if (!(next_attr->uuid_type == GATT_ATTR_UUID_TYPE_16 && next_attr->uuid == GATT_UUID_INCLUDE_SERVICE)) {
                is_include_service_allowed = FALSE;
                break;
            }
            next_attr = (tGATT_ATTR16 *)next_attr->p_next;
        }

    }
    if (!is_include_service_allowed) {
        GATT_TRACE_ERROR("%s error, The include service should be added before adding the characteristics", __func__);
        return 0;
    }

    if ((p_attr = (tGATT_ATTR16 *) allocate_attr_in_db(p_db, &uuid, GATT_PERM_READ)) != NULL) {
        if (copy_extra_byte_in_db(p_db, (void **)&p_attr->p_value, sizeof(tGATT_INCL_SRVC))) {
            p_attr->p_value->incl_handle.s_handle = s_handle;
            p_attr->p_value->incl_handle.e_handle = e_handle;
            memcpy(&p_attr->p_value->incl_handle.service_type, &service, sizeof(tBT_UUID));

            return p_attr->handle;
        } else {
            deallocate_attr_in_db(p_db, p_attr);
        }
    }

    return 0;
}

/*******************************************************************************
**
** Function         gatts_add_characteristic
**
** Description      This function add a characteristics and its descriptor into
**                  a servce identified by the service database pointer.
**
** Parameter        p_db: database pointer.
**                  perm: permission (authentication and key size requirements)
**                  property: property of the characteristic.
**                  p_char: characteristic value information.
**
** Returns          Status of te operation.
**
*******************************************************************************/
UINT16 gatts_add_characteristic (tGATT_SVC_DB *p_db, tGATT_PERM perm,
                                 tGATT_CHAR_PROP property,
                                 tBT_UUID *p_char_uuid, tGATT_ATTR_VAL *attr_val, tGATTS_ATTR_CONTROL *control)
{
    tGATT_ATTR16     *p_char_decl, *p_char_val;
    tBT_UUID        uuid = {LEN_UUID_16, {GATT_UUID_CHAR_DECLARE}};
    BOOLEAN status;

    GATT_TRACE_DEBUG("gatts_add_characteristic perm=0x%0x property=0x%0x\n", perm, property);
    /* parameter validation check */
    status = gatts_add_char_desc_value_check(attr_val, control);
    if (status == FALSE){
        return 0;
    }


    if ((p_char_decl = (tGATT_ATTR16 *)allocate_attr_in_db(p_db, &uuid, GATT_PERM_READ)) != NULL) {
        if (!copy_extra_byte_in_db(p_db, (void **)&p_char_decl->p_value, sizeof(tGATT_CHAR_DECL))) {
            deallocate_attr_in_db(p_db, p_char_decl);
            return 0;
        }

        p_char_val = (tGATT_ATTR16 *)allocate_attr_in_db(p_db, p_char_uuid, perm);

        if (p_char_val == NULL) {
            deallocate_attr_in_db(p_db, p_char_decl);
            return 0;
        }

        p_char_decl->p_value->char_decl.property = property;
        p_char_decl->p_value->char_decl.char_val_handle  = p_char_val->handle;
        if (control != NULL) {
            p_char_val->control.auto_rsp  =  control->auto_rsp;
        } else {
            p_char_val->control.auto_rsp = GATT_RSP_DEFAULT;
        }

        if (attr_val != NULL) {
            if (!copy_extra_byte_in_db(p_db, (void **)&p_char_val->p_value, sizeof(tGATT_ATTR_VAL))) {
                deallocate_attr_in_db(p_db, p_char_val);
                return 0;
            }
            GATT_TRACE_DEBUG("attr_val->attr_len = %x, attr_val->attr_max_len = %x\n", attr_val->attr_len, attr_val->attr_max_len);
            GATT_TRACE_DEBUG("attribute handle = %x\n", p_char_val->handle);
            p_char_val->p_value->attr_val.attr_len = attr_val->attr_len;
            p_char_val->p_value->attr_val.attr_max_len = attr_val->attr_max_len;
            p_char_val->p_value->attr_val.attr_val = osi_malloc(attr_val->attr_max_len);
            if (p_char_val->p_value->attr_val.attr_val == NULL) {
               deallocate_attr_in_db(p_db, p_char_decl);
               deallocate_attr_in_db(p_db, p_char_val);
               GATT_TRACE_WARNING("Warning in %s, line=%d, insufficient resource to allocate for attribute value\n", __func__, __LINE__);
               return 0;
            }
            else {
                //add mask to indicate that p_value->attr_val.attr_val is dynamic allocated
                p_char_val->mask |= GATT_ATTR_VALUE_ALLOCATED;
            }

            //initiate characteristic attribute value part
            memset(p_char_val->p_value->attr_val.attr_val, 0, attr_val->attr_max_len);
            if (attr_val->attr_val != NULL) {
                if (attr_val->attr_max_len < attr_val->attr_len){
                    GATT_TRACE_ERROR("Error in %s, Line=%d, attribute actual length (%d) should not larger than max size (%d)\n",
                                    __func__, __LINE__, attr_val->attr_len, attr_val->attr_max_len);
                }
                UINT16 actual_len = (attr_val->attr_max_len < attr_val->attr_len) ? (attr_val->attr_max_len) : (attr_val->attr_len);
                memcpy(p_char_val->p_value->attr_val.attr_val, attr_val->attr_val, actual_len);
            }
        }

        return p_char_val->handle;
    }

    return 0;
}

/*******************************************************************************
**
** Function         gatt_convertchar_descr_type
**
** Description      This function convert a char descript UUID into descriptor type.
**
** Returns          descriptor type.
**
*******************************************************************************/
UINT8 gatt_convertchar_descr_type(tBT_UUID *p_descr_uuid)
{
    tBT_UUID std_descr = {LEN_UUID_16, {GATT_UUID_CHAR_EXT_PROP}};

    if (gatt_uuid_compare(std_descr, * p_descr_uuid)) {
        return GATT_DESCR_EXT_DSCPTOR;
    }

    std_descr.uu.uuid16 ++;
    if (gatt_uuid_compare(std_descr,  * p_descr_uuid)) {
        return GATT_DESCR_USER_DSCPTOR;
    }

    std_descr.uu.uuid16 ++;
    if (gatt_uuid_compare(std_descr,  * p_descr_uuid)) {
        return GATT_DESCR_CLT_CONFIG;
    }

    std_descr.uu.uuid16 ++;
    if (gatt_uuid_compare(std_descr,  * p_descr_uuid)) {
        return GATT_DESCR_SVR_CONFIG;
    }

    std_descr.uu.uuid16 ++;
    if (gatt_uuid_compare(std_descr,  * p_descr_uuid)) {
        return GATT_DESCR_PRES_FORMAT;
    }

    std_descr.uu.uuid16 ++;
    if (gatt_uuid_compare(std_descr,  * p_descr_uuid)) {
        return GATT_DESCR_AGGR_FORMAT;
    }

    std_descr.uu.uuid16 ++;
    if (gatt_uuid_compare(std_descr,  * p_descr_uuid)) {
        return GATT_DESCR_VALID_RANGE;
    }


    return GATT_DESCR_UNKNOWN;
}

/*******************************************************************************
**
** Function         gatts_add_char_descr
**
** Description      This function add a characteristics descriptor.
**
** Parameter        p_db: database pointer.
**                  perm: characteristic descriptor permission type.
**                  char_dscp_tpye: the characteristic descriptor masks.
**                  p_dscp_params: characteristic descriptors values.
**
** Returns          Status of the operation.
**
*******************************************************************************/
UINT16 gatts_add_char_descr (tGATT_SVC_DB *p_db, tGATT_PERM perm,
                             tBT_UUID  *p_descr_uuid,  tGATT_ATTR_VAL *attr_val, tGATTS_ATTR_CONTROL *control)
{
    tGATT_ATTR16    *p_char_dscptr;
    BOOLEAN status;

    GATT_TRACE_DEBUG("gatts_add_char_descr uuid=0x%04x\n", p_descr_uuid->uu.uuid16);

    /* parameter validation check */
    status = gatts_add_char_desc_value_check(attr_val, control);
    if (status == FALSE){
        return 0;
    }

    /* Add characteristic descriptors */
    if ((p_char_dscptr = (tGATT_ATTR16 *)allocate_attr_in_db(p_db, p_descr_uuid, perm)) == NULL) {
        deallocate_attr_in_db(p_db, p_char_dscptr);
        GATT_TRACE_DEBUG("gatts_add_char_descr Fail for adding char descriptors.");
        return 0;
    } else {
        p_char_dscptr->control.auto_rsp = (control == NULL) ? GATT_RSP_DEFAULT : (control->auto_rsp);
        if (attr_val != NULL) {
            if (!copy_extra_byte_in_db(p_db, (void **)&p_char_dscptr->p_value, sizeof(tGATT_ATTR_VAL))) {
                deallocate_attr_in_db(p_db, p_char_dscptr);
                return 0;
            }
            p_char_dscptr->p_value->attr_val.attr_len = attr_val->attr_len;
            p_char_dscptr->p_value->attr_val.attr_max_len  = attr_val->attr_max_len;
            if (attr_val->attr_max_len != 0) {
                p_char_dscptr->p_value->attr_val.attr_val = osi_malloc(attr_val->attr_max_len);
                if (p_char_dscptr->p_value->attr_val.attr_val == NULL) {
                    deallocate_attr_in_db(p_db, p_char_dscptr);
                    GATT_TRACE_WARNING("Warning in %s, line=%d, insufficient resource to allocate for descriptor value\n", __func__, __LINE__);
                    return 0;
                }
                else {
                    //add mask to indicate that p_value->attr_val.attr_val is dynamic allocated
                    p_char_dscptr->mask |= GATT_ATTR_VALUE_ALLOCATED;
                }

                //initiate characteristic attribute value part
                memset(p_char_dscptr->p_value->attr_val.attr_val, 0, attr_val->attr_max_len);
                if(attr_val->attr_val != NULL) {
                    memcpy(p_char_dscptr->p_value->attr_val.attr_val, attr_val->attr_val, attr_val->attr_len);
                }
            }
        }
        return p_char_dscptr->handle;
    }
}


/*******************************************************************************
**
** Function         gatts_set_attribute_value
**
** Description      This function add the attribute value in the database
**
** Parameter        p_db: database pointer.
**                      attr_handle: the attribute handle
**                      length: the attribute value length
**                      value: the pointer to the data to be set to the attribute value in the database
**
** Returns          Status of the operation.
**
*******************************************************************************/
tGATT_STATUS gatts_set_attribute_value(tGATT_SVC_DB *p_db, UINT16 attr_handle,
                                               UINT16 length, UINT8 *value)
{
    tGATT_ATTR16  *p_cur;

    if (p_db == NULL) {
        GATT_TRACE_DEBUG("gatts_set_attribute_value Fail:p_db is NULL.\n");
        return GATT_INVALID_PDU;
    }
    if (p_db->p_attr_list == NULL) {
        GATT_TRACE_DEBUG("gatts_set_attribute_value Fail:p_db->p_attr_list is NULL.\n");
        return GATT_INVALID_PDU;
    }
    if ((length > 0) && (value == NULL)){
        GATT_TRACE_ERROR("Error in %s, line=%d, value should not be NULL here\n",__func__, __LINE__);
        return GATT_INVALID_PDU;
    }

    p_cur    =  (tGATT_ATTR16 *) p_db->p_attr_list;

    while (p_cur != NULL) {
        if (p_cur->handle == attr_handle) {
            /* for characteristic should not be set, return GATT_NOT_FOUND */
            if (p_cur->uuid_type == GATT_ATTR_UUID_TYPE_16) {
                switch (p_cur->uuid) {
                    case GATT_UUID_PRI_SERVICE:
                    case GATT_UUID_SEC_SERVICE:
                    case GATT_UUID_CHAR_DECLARE:
                        return GATT_NOT_FOUND;
                        break;
                }
            }

            /* in other cases, value can be set*/
            if ((p_cur->p_value == NULL) || (p_cur->p_value->attr_val.attr_val == NULL) \
                    || (p_cur->p_value->attr_val.attr_max_len == 0)){
                GATT_TRACE_ERROR("Error in %s, line=%d, attribute value should not be NULL here\n", __func__, __LINE__);
                return GATT_NOT_FOUND;
            } else if (p_cur->p_value->attr_val.attr_max_len < length) {
                GATT_TRACE_ERROR("gatts_set_attribute_value failed:Invalid value length");
                return GATT_INVALID_ATTR_LEN;
            } else{
                memcpy(p_cur->p_value->attr_val.attr_val, value, length);
                p_cur->p_value->attr_val.attr_len = length;
            }
            break;
        }
        p_cur = p_cur->p_next;
    }

    return GATT_SUCCESS;
}

/*******************************************************************************
**
** Function         gatts_get_attr_value_internal
**
** Description      This function get the attribute value in gap service and gatt service
**
** Parameter        attr_handle: the attribute handle
**                  length: the attribute value length
**                  value: the pointer to the data to be get to the attribute value in the database
**
** Returns          Status of the operation.
**
*******************************************************************************/
tGATT_STATUS gatts_get_attr_value_internal(UINT16 attr_handle, UINT16 *length, UINT8 **value)
{
    UINT8 i;
    tGATT_READ_REQ read_req;
    tGATT_STATUS status = GATT_NOT_FOUND;
    tGATT_SR_REG *p_rcb = gatt_cb.sr_reg;
    UINT8 service_uuid[LEN_UUID_128] = {0};

    if (length == NULL){
        GATT_TRACE_ERROR("gatts_get_attr_value_internal Fail:length is NULL.\n");
        return GATT_INVALID_PDU;
    }

    if (value == NULL){
        GATT_TRACE_ERROR("gatts_get_attr_value_internal Fail:value is NULL.\n");
        *length = 0;
        return GATT_INVALID_PDU;
    }

    // find the service by handle
    for (i = 0; i < GATT_MAX_SR_PROFILES; i++, p_rcb++) {
        if (p_rcb->in_use && p_rcb->s_hdl <= attr_handle && p_rcb->e_hdl >= attr_handle) {
            break;
        }
    }

    // service cb not found
    if (i == GATT_MAX_SR_PROFILES) {
        return status;
    }

    if (p_rcb->app_uuid.len != LEN_UUID_128) {
        return status;
    }

    memset(&read_req, 0, sizeof(tGATT_READ_REQ));
    read_req.handle = attr_handle;

    // read gatt service attribute value
    memset(service_uuid, 0x81, LEN_UUID_128);
    if (!memcmp(p_rcb->app_uuid.uu.uuid128, service_uuid, LEN_UUID_128)) {
        status = gatt_proc_read(0, GATTS_REQ_TYPE_READ, &read_req, &gatt_cb.rsp);
    }

    // read gap service attribute value
    memset(service_uuid, 0x82, LEN_UUID_128);
    if (!memcmp(p_rcb->app_uuid.uu.uuid128, service_uuid, LEN_UUID_128)) {
        status = gap_proc_read(GATTS_REQ_TYPE_READ, &read_req, &gatt_cb.rsp);
    }

    if (status == GATT_SUCCESS) {
        *length = gatt_cb.rsp.attr_value.len;
        *value = gatt_cb.rsp.attr_value.value;
    }

    return status;
}

/*******************************************************************************
**
** Function         gatts_get_attribute_value
**
** Description      This function get the attribute value in the database
**
** Parameter        p_db: database pointer.
**                      attr_handle: the attribute handle
**                      length: the attribute value length
**                      value: the pointer to the data to be get to the attribute value in the database
**
** Returns          Status of the operation.
**
*******************************************************************************/
tGATT_STATUS gatts_get_attribute_value(tGATT_SVC_DB *p_db, UINT16 attr_handle,
                                       UINT16 *length, UINT8 **value)
{
    tGATT_ATTR16  *p_cur;

    GATT_TRACE_DEBUG("attr_handle = %x\n", attr_handle);

    if (p_db == NULL) {
        GATT_TRACE_ERROR("gatts_get_attribute_value Fail:p_db is NULL.\n");
        *length = 0;
        return GATT_INVALID_PDU;
    }
    if (p_db->p_attr_list == NULL) {
        GATT_TRACE_ERROR("gatts_get_attribute_value Fail:p_db->p_attr_list is NULL.\n");
        *length = 0;
        return GATT_INVALID_PDU;
    }
    if (length == NULL){
        GATT_TRACE_ERROR("gatts_get_attribute_value Fail:length is NULL.\n");
        return GATT_INVALID_PDU;
    }
    if (value == NULL){
        GATT_TRACE_ERROR("gatts_get_attribute_value Fail:value is NULL.\n");
        *length = 0;
        return GATT_INVALID_PDU;
    }

    p_cur = (tGATT_ATTR16 *) p_db->p_attr_list;

    while (p_cur != NULL) {
        if (p_cur->handle == attr_handle) {

            if (p_cur->uuid_type == GATT_ATTR_UUID_TYPE_16) {
                switch (p_cur->uuid) {
                case GATT_UUID_CHAR_DECLARE:
                case GATT_UUID_INCLUDE_SERVICE:
                    break;
                default:
                    if (p_cur->p_value &&  p_cur->p_value->attr_val.attr_len != 0) {
                        *length = p_cur->p_value->attr_val.attr_len;
                        *value = p_cur->p_value->attr_val.attr_val;
                        return GATT_SUCCESS;
                    } else {
                        *length = 0;
                        return GATT_SUCCESS;
                    }
                    break;
                }
            } else {
                if (p_cur->p_value && p_cur->p_value->attr_val.attr_len != 0) {
                    *length = p_cur->p_value->attr_val.attr_len;
                    *value = p_cur->p_value->attr_val.attr_val;
                    return GATT_SUCCESS;
                } else {
                    *length = 0;
                    return GATT_SUCCESS;
                }

            }

            break;

        }

        p_cur = p_cur->p_next;
    }

    return GATT_NOT_FOUND;
}

BOOLEAN gatts_is_auto_response(UINT16 attr_handle)
{
    tGATT_HDL_LIST_ELEM  *p_decl = NULL;
    BOOLEAN rsp = FALSE;
    tGATT_SVC_DB *p_db = NULL;
    if ((p_decl = gatt_find_hdl_buffer_by_attr_handle(attr_handle)) == NULL) {
        GATT_TRACE_DEBUG("Service not created\n");
        return rsp;
    }

    p_db = &p_decl->svc_db;

    tGATT_ATTR16  *p_cur, *p_next;

    if (p_db == NULL) {
        GATT_TRACE_DEBUG("gatts_get_attribute_value Fail:p_db is NULL.\n");
        return rsp;
    }
    if (p_db->p_attr_list == NULL) {
        GATT_TRACE_DEBUG("gatts_get_attribute_value Fail:p_db->p_attr_list is NULL.\n");
        return rsp;
    }

    p_cur    =  (tGATT_ATTR16 *) p_db->p_attr_list;
    p_next  = (tGATT_ATTR16 *) p_cur->p_next;

    for (; p_cur != NULL && p_next != NULL;
            p_cur = p_next, p_next = (tGATT_ATTR16 *)p_next->p_next) {
        if (p_cur->handle == attr_handle) {
            if (p_cur->p_value != NULL && p_cur->control.auto_rsp == GATT_RSP_BY_STACK) {
                rsp = true;
                return rsp;
            }

        }

    }

    return rsp;

}

/*******************************************************************************/
/* Service Attribute Database Query Utility Functions */
/*******************************************************************************/
/*******************************************************************************
**
** Function         gatts_read_attr_value_by_handle
**
** Description      Query attribute value by attribute handle.
**
** Parameter        p_db: pointer to the attribute database.
**                  handle: Attribute handle to read.
**                  offset: Read offset.
**                  p_value: output parameter to carry out the attribute value.
**                  p_len: output parameter as attribute length read.
**                  read_long: this is a read blob request.
**                  mtu: MTU.
**                  sec_flag: current link security status.
**                  key_size: encryption key size
**
** Returns          Status of operation.
**
*******************************************************************************/
tGATT_STATUS gatts_read_attr_value_by_handle(tGATT_TCB *p_tcb,
        tGATT_SVC_DB *p_db,
        UINT8 op_code,
        UINT16 handle, UINT16 offset,
        UINT8 *p_value, UINT16 *p_len,
        UINT16 mtu,
        tGATT_SEC_FLAG sec_flag,
        UINT8 key_size,
        UINT32 trans_id)
{
    tGATT_STATUS status = GATT_NOT_FOUND;
    tGATT_ATTR16  *p_attr;
    UINT8       *pp = p_value;

    if (p_db && p_db->p_attr_list) {
        p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;

        while (p_attr && handle >= p_attr->handle) {
            if (p_attr->handle == handle) {
                status = read_attr_value (p_attr, offset, &pp,
                                          (BOOLEAN)(op_code == GATT_REQ_READ_BLOB),
                                          mtu, p_len, sec_flag, key_size);

                if ((status == GATT_PENDING) || (status == GATT_STACK_RSP)) {
                    BOOLEAN need_rsp = (status != GATT_STACK_RSP);
                    status = gatts_send_app_read_request(p_tcb, op_code, p_attr->handle, offset, trans_id, need_rsp);
                }
                break;
            }
            p_attr = (tGATT_ATTR16 *)p_attr->p_next;
        }
    }


    return status;
}

tGATT_STATUS gatts_write_attr_value_by_handle(tGATT_SVC_DB *p_db,
        UINT16 handle, UINT16 offset,
        UINT8 *p_value, UINT16 len)
{
    tGATT_STATUS status = GATT_NOT_FOUND;
    tGATT_ATTR16  *p_attr;

    if (p_db && p_db->p_attr_list) {
        p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;

        while (p_attr && handle >= p_attr->handle) {
            if (p_attr->handle == handle ) {
                if (p_attr->control.auto_rsp == GATT_RSP_BY_APP) {
                    return GATT_APP_RSP;
                }

                if ((p_attr->p_value != NULL) &&
                    (p_attr->p_value->attr_val.attr_max_len >= offset + len) &&
                    p_attr->p_value->attr_val.attr_val != NULL) {
                    memcpy(p_attr->p_value->attr_val.attr_val + offset, p_value, len);
                    p_attr->p_value->attr_val.attr_len = len + offset;
                    return GATT_SUCCESS;
                } else if (p_attr->p_value && p_attr->p_value->attr_val.attr_max_len < offset + len){
                    GATT_TRACE_DEBUG("Remote device try to write with a length larger then attribute's max length\n");
                    return GATT_INVALID_ATTR_LEN;
                } else if ((p_attr->p_value == NULL) || (p_attr->p_value->attr_val.attr_val == NULL)){
                    GATT_TRACE_ERROR("Error in %s, line=%d, %s should not be NULL here\n", __func__, __LINE__, \
                                    (p_attr->p_value == NULL) ? "p_value" : "attr_val.attr_val");
                    return GATT_UNKNOWN_ERROR;
                }
            }

            p_attr = (tGATT_ATTR16 *)p_attr->p_next;

        }
    }

    return status;
}

/*******************************************************************************
**
** Function         gatts_read_attr_perm_check
**
** Description      Check attribute readability.
**
** Parameter        p_db: pointer to the attribute database.
**                  handle: Attribute handle to read.
**                  offset: Read offset.
**                  p_value: output parameter to carry out the attribute value.
**                  p_len: output parameter as attribute length read.
**                  read_long: this is a read blob request.
**                  mtu: MTU.
**                  sec_flag: current link security status.
**                  key_size: encryption key size
**
** Returns          Status of operation.
**
*******************************************************************************/
tGATT_STATUS gatts_read_attr_perm_check(tGATT_SVC_DB *p_db,
                                        BOOLEAN is_long,
                                        UINT16 handle,
                                        tGATT_SEC_FLAG sec_flag,
                                        UINT8 key_size)
{
    tGATT_STATUS status = GATT_NOT_FOUND;
    tGATT_ATTR16  *p_attr;

    if (p_db && p_db->p_attr_list) {
        p_attr = (tGATT_ATTR16 *)p_db->p_attr_list;

        while (p_attr && handle >= p_attr->handle) {
            if (p_attr->handle == handle) {
                status = gatts_check_attr_readability (p_attr, 0,
                                                       is_long,
                                                       sec_flag, key_size);
                break;
            }
            p_attr = (tGATT_ATTR16 *) p_attr->p_next;
        }
    }

    return status;
}


/*******************************************************************************
**
** Function         gatts_write_attr_perm_check
**
** Description      Write attribute value into database.
**
** Parameter        p_db: pointer to the attribute database.
**                  op_code:op code of this write.
**                  handle: handle of the attribute to write.
**                  offset: Write offset if write op code is write blob.
**                  p_data: Attribute value to write.
**                  len: attribute data length.
**                  sec_flag: current link security status.
**                  key_size: encryption key size
**
** Returns          Status of the operation.
**
*******************************************************************************/
tGATT_STATUS gatts_write_attr_perm_check (tGATT_SVC_DB *p_db, UINT8 op_code,
        UINT16 handle, UINT16 offset, UINT8 *p_data,
        UINT16 len, tGATT_SEC_FLAG sec_flag, UINT8 key_size)
{
    tGATT_STATUS    status = GATT_NOT_FOUND;
    tGATT_ATTR16    *p_attr;
    UINT16          max_size = 0;
    tGATT_PERM      perm;
    UINT16          min_key_size;

    GATT_TRACE_DEBUG( "gatts_write_attr_perm_check op_code=0x%0x handle=0x%04x offset=%d len=%d sec_flag=0x%0x key_size=%d",
                      op_code, handle, offset, len, sec_flag, key_size);

    if (p_db != NULL) {
        p_attr = (tGATT_ATTR16 *) p_db->p_attr_list;

        while (p_attr != NULL) {
            if (p_attr->handle == handle) {
                perm = p_attr->permission;
                min_key_size = (((perm & GATT_ENCRYPT_KEY_SIZE_MASK) >> 12));
                if (min_key_size != 0 ) {
                    min_key_size += 6;
                }
                GATT_TRACE_DEBUG( "gatts_write_attr_perm_check p_attr->permission =0x%04x min_key_size==0x%04x",
                                  p_attr->permission,
                                  min_key_size);

                if ((op_code == GATT_CMD_WRITE || op_code == GATT_REQ_WRITE)
                        && (perm & GATT_WRITE_SIGNED_PERM)) {
                    /* use the rules for the mixed security see section 10.2.3*/
                    /* use security mode 1 level 2 when the following condition follows */
                    /* LE security mode 2 level 1 and LE security mode 1 level 2 */
                    if ((perm & GATT_PERM_WRITE_SIGNED) && (perm & GATT_PERM_WRITE_ENCRYPTED)) {
                        perm = GATT_PERM_WRITE_ENCRYPTED;
                    }
                    /* use security mode 1 level 3 when the following condition follows */
                    /* LE security mode 2 level 2 and security mode 1 and LE */
                    else if (((perm & GATT_PERM_WRITE_SIGNED_MITM) && (perm & GATT_PERM_WRITE_ENCRYPTED)) ||
                             /* LE security mode 2 and security mode 1 level 3 */
                             ((perm & GATT_WRITE_SIGNED_PERM) && (perm & GATT_PERM_WRITE_ENC_MITM))) {
                        perm = GATT_PERM_WRITE_ENC_MITM;
                    }
                }

                if ((op_code == GATT_SIGN_CMD_WRITE) && !(perm & GATT_WRITE_SIGNED_PERM)) {
                    status = GATT_WRITE_NOT_PERMIT;
                    GATT_TRACE_DEBUG( "gatts_write_attr_perm_check - sign cmd write not allowed,handle:0x%04x",handle);
                }
                if ((op_code == GATT_SIGN_CMD_WRITE) && (sec_flag & GATT_SEC_FLAG_ENCRYPTED)) {
                    status = GATT_INVALID_PDU;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - Error!! sign cmd write sent on a encypted link,handle:0x%04x",handle);
                } else if (!(perm & GATT_WRITE_ALLOWED)) {
                    status = GATT_WRITE_NOT_PERMIT;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_WRITE_NOT_PERMIT,handle:0x%04x",handle);
                }
                /* require authentication, but not been authenticated */
                else if ((perm & GATT_WRITE_AUTH_REQUIRED ) && !(sec_flag & GATT_SEC_FLAG_LKEY_UNAUTHED)) {
                    status = GATT_INSUF_AUTHENTICATION;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INSUF_AUTHENTICATION,handle:0x%04x",handle);
                } else if ((perm & GATT_WRITE_MITM_REQUIRED ) && !(sec_flag & GATT_SEC_FLAG_LKEY_AUTHED)) {
                    status = GATT_INSUF_AUTHENTICATION;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INSUF_AUTHENTICATION: MITM required,handle:0x%04x",handle);
                } else if ((perm & GATT_WRITE_ENCRYPTED_PERM ) && !(sec_flag & GATT_SEC_FLAG_ENCRYPTED)) {
                    status = GATT_INSUF_ENCRYPTION;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INSUF_ENCRYPTION,handle:0x%04x",handle);
                } else if ((perm & GATT_WRITE_ENCRYPTED_PERM ) && (sec_flag & GATT_SEC_FLAG_ENCRYPTED) && (key_size < min_key_size)) {
                    status = GATT_INSUF_KEY_SIZE;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INSUF_KEY_SIZE,handle:0x%04x",handle);
                }
                /* LE Authorization check*/
                else if ((perm & GATT_WRITE_AUTHORIZATION) && (!(sec_flag & GATT_SEC_FLAG_LKEY_AUTHED) || !(sec_flag & GATT_SEC_FLAG_AUTHORIZATION))){
                    status = GATT_INSUF_AUTHORIZATION;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INSUF_AUTHORIZATION,handle:0x%04x",handle);
                }
                /* LE security mode 2 attribute  */
                else if (perm & GATT_WRITE_SIGNED_PERM && op_code != GATT_SIGN_CMD_WRITE && !(sec_flag & GATT_SEC_FLAG_ENCRYPTED)
                         &&  (perm & GATT_WRITE_ALLOWED) == 0) {
                    status = GATT_INSUF_AUTHENTICATION;
                    GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INSUF_AUTHENTICATION: LE security mode 2 required,handle:0x%04x",handle);
                } else { /* writable: must be char value declaration or char descritpors */
                    if (p_attr->uuid_type == GATT_ATTR_UUID_TYPE_16) {
                        switch (p_attr->uuid) {
                        case GATT_UUID_CHAR_PRESENT_FORMAT:/* should be readable only */
                        case GATT_UUID_CHAR_EXT_PROP:/* should be readable only */
                        case GATT_UUID_CHAR_AGG_FORMAT: /* should be readable only */
                        case GATT_UUID_CHAR_VALID_RANGE:
                            status = GATT_WRITE_NOT_PERMIT;
                            break;

                        case GATT_UUID_CHAR_CLIENT_CONFIG:
                        /* coverity[MISSING_BREAK] */
                        /* intnended fall through, ignored */
                        /* fall through */
                        case GATT_UUID_CHAR_SRVR_CONFIG:
                            max_size = 2;
                        case GATT_UUID_CHAR_DESCRIPTION:
                        default: /* any other must be character value declaration */
                            status = GATT_SUCCESS;
                            break;
                        }
                    } else if (p_attr->uuid_type == GATT_ATTR_UUID_TYPE_128 ||
                               p_attr->uuid_type == GATT_ATTR_UUID_TYPE_32) {
                        status = GATT_SUCCESS;
                    } else {
                        status = GATT_INVALID_PDU;
                    }

                    if (p_data == NULL && len  > 0) {
                        status = GATT_INVALID_PDU;
                    }
                    /* these attribute does not allow write blob */
// btla-specific ++
                    else if ( (p_attr->uuid_type == GATT_ATTR_UUID_TYPE_16) &&
                              (p_attr->uuid == GATT_UUID_CHAR_CLIENT_CONFIG ||
                               p_attr->uuid == GATT_UUID_CHAR_SRVR_CONFIG   ||
                               p_attr->uuid == GATT_UUID_CLIENT_SUP_FEAT    || 
                               p_attr->uuid == GATT_UUID_GAP_ICON
                               ) )
// btla-specific --
                    {
                        if (op_code == GATT_REQ_PREPARE_WRITE) { /* does not allow write blob */
                            status = GATT_REQ_NOT_SUPPORTED;
                            GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_REQ_NOT_SUPPORTED,handle:0x%04x",handle);
                        } else if (len != max_size) { /* data does not match the required format */
                            status = GATT_INVALID_ATTR_LEN;
                            GATT_TRACE_ERROR( "gatts_write_attr_perm_check - GATT_INVALID_ATTR_LEN,handle:0x%04x",handle);
                        } else {
                            status = GATT_SUCCESS;
                        }
                    }
                }
                break;
            } else {
                p_attr = (tGATT_ATTR16 *)p_attr->p_next;
            }
        }
    }

    return status;
}

/*******************************************************************************
**
** Function         allocate_attr_in_db
**
** Description      Allocate a memory space for a new attribute, and link this
**                  attribute into the database attribute list.
**
**
** Parameter        p_db    : database pointer.
**                  p_uuid:     pointer to attribute UUID
**                  service : type of attribute to be added.
**
** Returns          pointer to the newly allocated attribute.
**
*******************************************************************************/
static void *allocate_attr_in_db(tGATT_SVC_DB *p_db, tBT_UUID *p_uuid, tGATT_PERM perm)
{
    tGATT_ATTR16    *p_attr16 = NULL, *p_last;
    tGATT_ATTR32    *p_attr32 = NULL;
    tGATT_ATTR128   *p_attr128 = NULL;
    UINT16      len = sizeof(tGATT_ATTR128);

    if (p_uuid == NULL) {
        GATT_TRACE_ERROR("illegal UUID\n");
        return NULL;
    }

    if (p_uuid->len == LEN_UUID_16) {
        len = sizeof(tGATT_ATTR16);
    } else if (p_uuid->len == LEN_UUID_32) {
        len = sizeof(tGATT_ATTR32);
    }

    GATT_TRACE_DEBUG("allocate attr %d bytes\n", len);

    if (p_db->end_handle <= p_db->next_handle) {
        GATT_TRACE_DEBUG("handle space full. handle_max = %d next_handle = %d\n",
                         p_db->end_handle, p_db->next_handle);
        return NULL;
    }

    if (p_db->mem_free < len) {
        if (!allocate_svc_db_buf(p_db)) {
            GATT_TRACE_ERROR("allocate_attr_in_db failed, no resources\n");
            return NULL;
        }
    }
    memset(p_db->p_free_mem, 0, len);
    p_attr16 = (tGATT_ATTR16 *) p_db->p_free_mem;

    if (p_uuid->len == LEN_UUID_16 && p_uuid->uu.uuid16 != GATT_ILLEGAL_UUID) {
        p_attr16->uuid_type = GATT_ATTR_UUID_TYPE_16;
        p_attr16->uuid = p_uuid->uu.uuid16;
    } else if (p_uuid->len == LEN_UUID_32) {
        p_attr32 = (tGATT_ATTR32 *) p_db->p_free_mem;
        p_attr32->uuid_type = GATT_ATTR_UUID_TYPE_32;
        p_attr32->uuid = p_uuid->uu.uuid32;
    } else if (p_uuid->len == LEN_UUID_128) {
        p_attr128 = (tGATT_ATTR128 *) p_db->p_free_mem;
        p_attr128->uuid_type = GATT_ATTR_UUID_TYPE_128;
        memcpy(p_attr128->uuid, p_uuid->uu.uuid128, LEN_UUID_128);
    }

    p_db->p_free_mem += len;
    p_db->mem_free -= len;

    p_attr16->handle = p_db->next_handle++;
    p_attr16->permission = perm;
    p_attr16->p_next = NULL;

    /* link the attribute record into the end of DB */
    if (p_db->p_attr_list == NULL) {
        p_db->p_attr_list = p_attr16;
    } else {
        p_last = (tGATT_ATTR16 *)p_db->p_attr_list;

        while (p_last != NULL && p_last->p_next != NULL) {
            p_last = (tGATT_ATTR16 *)p_last->p_next;
        }

        p_last->p_next = p_attr16;
    }

    if (p_attr16->uuid_type == GATT_ATTR_UUID_TYPE_16) {
        GATT_TRACE_DEBUG("=====> handle = [0x%04x] uuid16 = [0x%04x] perm=0x%02x\n",
                         p_attr16->handle, p_attr16->uuid, p_attr16->permission);
    } else if (p_attr16->uuid_type == GATT_ATTR_UUID_TYPE_32) {
        GATT_TRACE_DEBUG("=====> handle = [0x%04x] uuid32 = [0x%08x] perm=0x%02x\n",
                         p_attr32->handle, p_attr32->uuid, p_attr32->permission);
    } else {
        GATT_TRACE_DEBUG("=====> handle = [0x%04x] uuid128 = [0x%02x:0x%02x] perm=0x%02x\n",
                         p_attr128->handle, p_attr128->uuid[0], p_attr128->uuid[1],
                         p_attr128->permission);
    }
    return (void *)p_attr16;
}



/*******************************************************************************
**
** Function         deallocate_attr_in_db
**
** Description      Free an attribute within the database.
**
** Parameter        p_db: database pointer.
**                  p_attr: pointer to the attribute record to be freed.
**
** Returns          BOOLEAN: success
**
*******************************************************************************/
static BOOLEAN deallocate_attr_in_db(tGATT_SVC_DB *p_db, void *p_attr)
{
    tGATT_ATTR16  *p_cur, *p_next;
    BOOLEAN     found = FALSE;

    if (p_db->p_attr_list == NULL) {
        return found;
    }

    p_cur   = (tGATT_ATTR16 *) p_db->p_attr_list;
    p_next  = (tGATT_ATTR16 *) p_cur->p_next;

    for (; p_cur != NULL && p_next != NULL;
            p_cur = p_next, p_next = (tGATT_ATTR16 *)p_next->p_next) {
        if (p_next == p_attr) {
            p_cur->p_next = p_next->p_next;
            found = TRUE;
        }
    }
    if (p_cur == p_attr && p_cur == p_db->p_attr_list) {
        p_db->p_attr_list = p_cur->p_next;
        found = TRUE;
    }
    /* else attr not found */
    if ( found) {
        p_db->next_handle --;
    }

    return found;
}

/*******************************************************************************
**
** Function         copy_extra_byte_in_db
**
** Description      Utility function to allocate extra bytes memory in DB and copy
**                  the value from a source place.
**
**
** Parameter        p_db: database pointer.
**                  p_dst: destination data pointer.
**                  p_src: source data pointer.
**                  len: data length to be copied.
**
** Returns          None.
**
*******************************************************************************/
static BOOLEAN copy_extra_byte_in_db(tGATT_SVC_DB *p_db, void **p_dst, UINT16 len)
{
    UINT8 *p = (UINT8 *)*p_dst;

    if (p_db->mem_free < len) {
        if (!allocate_svc_db_buf(p_db)) {
            GATT_TRACE_ERROR("copy_extra_byte_in_db failed, no resources\n");
            return FALSE;
        }
    }

    p = p_db->p_free_mem;
    p_db->p_free_mem += len;
    p_db->mem_free -= len;
    memset((void *)p, 0, len);
    *p_dst = (void *)p;

    return TRUE;
}

/*******************************************************************************
**
** Function         allocate_svc_db_buf
**
** Description      Utility function to allocate extra buffer for service database.
**
** Returns          TRUE if allocation succeed, otherwise FALSE.
**
*******************************************************************************/
static BOOLEAN allocate_svc_db_buf(tGATT_SVC_DB *p_db)
{
    BT_HDR  *p_buf;

    GATT_TRACE_DEBUG("allocate_svc_db_buf allocating extra buffer");

    if ((p_buf = (BT_HDR *)osi_calloc(GATT_DB_BUF_SIZE)) == NULL) {
        GATT_TRACE_ERROR("allocate_svc_db_buf failed, no resources");
        return FALSE;
    }

    p_db->p_free_mem    = (UINT8 *) p_buf;
    p_db->mem_free = GATT_DB_BUF_SIZE;

    fixed_queue_enqueue(p_db->svc_buffer, p_buf, FIXED_QUEUE_MAX_TIMEOUT);

    return TRUE;

}

/*******************************************************************************
**
** Function         gatts_send_app_read_request
**
** Description      Send application read request callback
**
** Returns          status of operation.
**
*******************************************************************************/
static tGATT_STATUS gatts_send_app_read_request(tGATT_TCB *p_tcb, UINT8 op_code,
        UINT16 handle, UINT16 offset, UINT32 trans_id, BOOLEAN need_rsp)
{
    tGATTS_DATA   sr_data;
    UINT8       i_rcb;
    tGATT_SR_REG *p_sreg;
    UINT16   conn_id;

    i_rcb = gatt_sr_find_i_rcb_by_handle(handle);
    if (i_rcb == GATT_MAX_SR_PROFILES) {
        GATT_TRACE_ERROR("Failed to find i_rcb,Error in %s, line=%d, \n", __func__, __LINE__);
        return (tGATT_STATUS) GATT_ERROR;
    }
    p_sreg = &gatt_cb.sr_reg[i_rcb];
    conn_id = GATT_CREATE_CONN_ID(p_tcb->tcb_idx, p_sreg->gatt_if);

    if (trans_id == 0) {
        trans_id = gatt_sr_enqueue_cmd(p_tcb, op_code, handle);
        gatt_sr_update_cback_cnt(p_tcb, p_sreg->gatt_if, TRUE, TRUE);
    }

    if (trans_id != 0 ) {
        memset(&sr_data, 0, sizeof(tGATTS_DATA));

        sr_data.read_req.handle = handle;
        sr_data.read_req.is_long = (BOOLEAN)(op_code == GATT_REQ_READ_BLOB);
        sr_data.read_req.offset = offset;
        sr_data.read_req.need_rsp = need_rsp;

        gatt_sr_send_req_callback(conn_id,
                                  trans_id, GATTS_REQ_TYPE_READ, &sr_data);

        if (need_rsp) {
            return (tGATT_STATUS) GATT_PENDING;
        }
        else{
            return (tGATT_STATUS) GATT_STACK_RSP;
        }
    } else {
        return (tGATT_STATUS) GATT_BUSY;    /* max pending command, application error */
    }

}

/*******************************************************************************
**
** Function         gatts_db_add_service_declaration
**
** Description      Update a service database service declaration record.
**
** Parameter        p_db: database pointer.
**                  service: UUID of the service.
**
** Returns          void
**
*******************************************************************************/
static BOOLEAN gatts_db_add_service_declaration(tGATT_SVC_DB *p_db, tBT_UUID *p_service, BOOLEAN is_pri)
{
    tGATT_ATTR16  *p_attr;
    tBT_UUID    uuid = {LEN_UUID_16, {0}};
    BOOLEAN     rt = FALSE;

    GATT_TRACE_DEBUG( "add_service_declaration");

    if (is_pri) {
        uuid.uu.uuid16 = GATT_UUID_PRI_SERVICE;
    } else {
        uuid.uu.uuid16 = GATT_UUID_SEC_SERVICE;
    }

    /* add service declration record */
    if ((p_attr = (tGATT_ATTR16 *)(allocate_attr_in_db(p_db, &uuid, GATT_PERM_READ))) != NULL) {
        if (copy_extra_byte_in_db (p_db, (void **)&p_attr->p_value, sizeof(tBT_UUID))) {
            if (p_service->len == LEN_UUID_16) {
                p_attr->p_value->uuid.len = LEN_UUID_16;
                p_attr->p_value->uuid.uu.uuid16 = p_service->uu.uuid16;
            } else if (p_service->len == LEN_UUID_32) {
                p_attr->p_value->uuid.len = LEN_UUID_128;
                gatt_convert_uuid32_to_uuid128(p_attr->p_value->uuid.uu.uuid128, p_service->uu.uuid32);
            } else {
                p_attr->p_value->uuid.len = LEN_UUID_128;
                memcpy(p_attr->p_value->uuid.uu.uuid128, p_service->uu.uuid128, LEN_UUID_128);
            }
            rt = TRUE;
        }

    }
    return rt;
}

/*******************************************************************************
**
** Function         gatts_add_char_desc_value_check
**
** Description      parameters validation check for gatts add char/descriptor functions
**
** Parameter        attr_val: attribute value for char/descriptor.
**                  control: control variable for char/descriptor.
**
** Returns          void
**
*******************************************************************************/
static BOOLEAN gatts_add_char_desc_value_check (tGATT_ATTR_VAL *attr_val, tGATTS_ATTR_CONTROL *control)
{
    if ((control != NULL) && ((control->auto_rsp != GATT_RSP_BY_APP) && (control->auto_rsp != GATT_RSP_BY_STACK))){
            GATT_TRACE_ERROR("Error in %s, line=%d, control->auto_rsp should be set to GATT_RSP_BY_APP or GATT_RSP_BY_STACK here\n",\
                    __func__, __LINE__);
            return FALSE;
    }

    if ((control != NULL) && (control->auto_rsp == GATT_RSP_BY_STACK)){
        if (attr_val == NULL){
            GATT_TRACE_ERROR("Error in %s, line=%d, for stack respond attribute, attr_val should not be NULL here\n",\
                            __func__, __LINE__);
            return FALSE;
        } else if (attr_val->attr_max_len == 0){
            GATT_TRACE_ERROR("Error in %s, line=%d, for stack respond attribute,  attribute max length should not be 0\n",\
                            __func__, __LINE__);
            return FALSE;
        }
    }

    if (attr_val != NULL){
        if (attr_val->attr_len > attr_val->attr_max_len){
            GATT_TRACE_ERROR("Error in %s, line=%d,attribute actual length should not be larger than max length\n",\
                            __func__, __LINE__);
            return FALSE;
        }
    }

    return TRUE ;
}

#endif /* BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
