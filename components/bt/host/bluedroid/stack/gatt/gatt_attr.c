/******************************************************************************
 *
 *  Copyright (C) 2008-2012 Broadcom Corporation
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
 *  this file contains the main GATT server attributes access request
 *  handling functions.
 *
 ******************************************************************************/

#include "common/bt_target.h"
//#include "bt_utils.h"

#include "stack/gatt_api.h"
#include "gatt_int.h"
#include "stack/sdpdefs.h"
#include "bta/bta_gatts_co.h"

#if (BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE)

#define BLE_GATT_SR_SUPP_FEAT_EATT_BITMASK 0x01
#define BLE_GATT_CL_SUPP_FEAT_ROBUST_CACHING_BITMASK 0x01
#define BLE_GATT_CL_SUPP_FEAT_EATT_BITMASK 0x02
#define BLE_GATT_CL_SUPP_FEAT_MULTI_NOTIF_BITMASK 0x04
#define BLE_GATT_CL_SUPP_FEAT_BITMASK 0x07

#define GATTP_MAX_NUM_INC_SVR       0

#if GATTS_ROBUST_CACHING_ENABLED
#define GATTP_MAX_CHAR_NUM          5
#else
#define GATTP_MAX_CHAR_NUM          2
#endif /* GATTS_ROBUST_CACHING_ENABLED */

#define GATTP_MAX_ATTR_NUM          (GATTP_MAX_CHAR_NUM * 2 + GATTP_MAX_NUM_INC_SVR + 1)
#define GATTP_MAX_CHAR_VALUE_SIZE   50

#ifndef GATTP_ATTR_DB_SIZE
#define GATTP_ATTR_DB_SIZE      GATT_DB_MEM_SIZE(GATTP_MAX_NUM_INC_SVR, GATTP_MAX_CHAR_NUM, GATTP_MAX_CHAR_VALUE_SIZE)
#endif

static void gatt_request_cback(UINT16 conn_id, UINT32 trans_id, UINT8 op_code, tGATTS_DATA *p_data);
static void gatt_connect_cback(tGATT_IF gatt_if, BD_ADDR bda, UINT16 conn_id, BOOLEAN connected,
                               tGATT_DISCONN_REASON reason, tBT_TRANSPORT transport);
static void gatt_disc_res_cback(UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_DISC_RES *p_data);
static void gatt_disc_cmpl_cback(UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_STATUS status);
static void gatt_cl_op_cmpl_cback(UINT16 conn_id, tGATTC_OPTYPE op, tGATT_STATUS status,
                                  tGATT_CL_COMPLETE *p_data);

static void gatt_cl_start_config_ccc(tGATT_PROFILE_CLCB *p_clcb);


static const tGATT_CBACK gatt_profile_cback = {
    gatt_connect_cback,
    gatt_cl_op_cmpl_cback,
    gatt_disc_res_cback,
    gatt_disc_cmpl_cback,
    gatt_request_cback,
    NULL,
    NULL
} ;

/*******************************************************************************
**
** Function         gatt_profile_find_conn_id_by_bd_addr
**
** Description      Find the connection ID by remote address
**
** Returns          Connection ID
**
*******************************************************************************/
#if (GATTS_INCLUDED == TRUE)
UINT16 gatt_profile_find_conn_id_by_bd_addr(BD_ADDR remote_bda)
{
    UINT16 conn_id = GATT_INVALID_CONN_ID;
    GATT_GetConnIdIfConnected (gatt_cb.gatt_if, remote_bda, &conn_id, BT_TRANSPORT_LE);
    return conn_id;
}
#endif  ///GATTS_INCLUDED == TRUE
/*******************************************************************************
**
** Function         gatt_profile_find_clcb_by_conn_id
**
** Description      find clcb by Connection ID
**
** Returns          Pointer to the found link conenction control block.
**
*******************************************************************************/
static tGATT_PROFILE_CLCB *gatt_profile_find_clcb_by_conn_id(UINT16 conn_id)
{
    UINT8 i_clcb;
    tGATT_PROFILE_CLCB    *p_clcb = NULL;

    for (i_clcb = 0, p_clcb = gatt_cb.profile_clcb; i_clcb < GATT_MAX_APPS; i_clcb++, p_clcb++) {
        if (p_clcb->in_use && p_clcb->conn_id == conn_id) {
            return p_clcb;
        }
    }

    return NULL;
}

/*******************************************************************************
**
** Function         gatt_profile_find_clcb_by_bd_addr
**
** Description      The function searches all LCBs with macthing bd address.
**
** Returns          Pointer to the found link conenction control block.
**
*******************************************************************************/
static tGATT_PROFILE_CLCB *gatt_profile_find_clcb_by_bd_addr(BD_ADDR bda, tBT_TRANSPORT transport)
{
    UINT8 i_clcb;
    tGATT_PROFILE_CLCB    *p_clcb = NULL;

    for (i_clcb = 0, p_clcb = gatt_cb.profile_clcb; i_clcb < GATT_MAX_APPS; i_clcb++, p_clcb++) {
        if (p_clcb->in_use && p_clcb->transport == transport &&
                p_clcb->connected && !memcmp(p_clcb->bda, bda, BD_ADDR_LEN)) {
            return p_clcb;
        }
    }

    return NULL;
}

/*******************************************************************************
**
** Function         gatt_profile_clcb_alloc
**
** Description      The function allocates a GATT profile  connection link control block
**
** Returns           NULL if not found. Otherwise pointer to the connection link block.
**
*******************************************************************************/
tGATT_PROFILE_CLCB *gatt_profile_clcb_alloc (UINT16 conn_id, BD_ADDR bda, tBT_TRANSPORT tranport)
{
    UINT8                   i_clcb = 0;
    tGATT_PROFILE_CLCB      *p_clcb = NULL;

    for (i_clcb = 0, p_clcb = gatt_cb.profile_clcb; i_clcb < GATT_MAX_APPS; i_clcb++, p_clcb++) {
        if (!p_clcb->in_use) {
            p_clcb->in_use      = TRUE;
            p_clcb->conn_id     = conn_id;
            p_clcb->connected   = TRUE;
            p_clcb->transport   = tranport;
            memcpy (p_clcb->bda, bda, BD_ADDR_LEN);
            break;
        }
    }
    if (i_clcb < GATT_MAX_APPS) {
        return p_clcb;
    }

    return NULL;
}

/*******************************************************************************
**
** Function         gatt_profile_clcb_dealloc
**
** Description      The function deallocates a GATT profile  connection link control block
**
** Returns          void
**
*******************************************************************************/
void gatt_profile_clcb_dealloc (tGATT_PROFILE_CLCB *p_clcb)
{
    memset(p_clcb, 0, sizeof(tGATT_PROFILE_CLCB));
}

/*******************************************************************************
**
** Function         gatt_proc_read
**
** Description      GATT Attributes Database Read/Read Blob Request process
**
** Returns          GATT_SUCCESS if successfully sent; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS gatt_proc_read (UINT16 conn_id, tGATTS_REQ_TYPE type, tGATT_READ_REQ *p_data, tGATTS_RSP *p_rsp)
{
    tGATT_STATUS    status = GATT_NO_RESOURCES;
    UINT16 len = 0;
    UINT8 *value;
    UNUSED(type);

    GATT_TRACE_DEBUG("%s handle %x", __func__, p_data->handle);

    if (p_data->is_long) {
        p_rsp->attr_value.offset = p_data->offset;
    }

    p_rsp->attr_value.handle = p_data->handle;
#if GATTS_ROBUST_CACHING_ENABLED

    UINT8 tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB *tcb = gatt_get_tcb_by_idx(tcb_idx);

    /* handle request for reading client supported features */
    if (p_data->handle == gatt_cb.handle_of_cl_supported_feat) {
        if (tcb == NULL) {
            return GATT_INSUF_RESOURCE;
        }
        p_rsp->attr_value.len = 1;
        memcpy(p_rsp->attr_value.value, &tcb->cl_supp_feat, 1);
        return GATT_SUCCESS;
    }

    /* handle request for reading database hash */
    if (p_data->handle == gatt_cb.handle_of_database_hash) {
        p_rsp->attr_value.len = BT_OCTET16_LEN;
        memcpy(p_rsp->attr_value.value, gatt_cb.database_hash, BT_OCTET16_LEN);
        gatt_sr_update_cl_status(tcb, true);
        return GATT_SUCCESS;
    }

    /* handle request for reading server supported features */
    if (p_data->handle == gatt_cb.handle_of_sr_supported_feat) {
        p_rsp->attr_value.len = 1;
        memcpy(p_rsp->attr_value.value, &gatt_cb.gatt_sr_supported_feat_mask, 1);
        return GATT_SUCCESS;
    }
#endif /* GATTS_ROBUST_CACHING_ENABLED */
    /* handle request for reading service changed des and the others */
    status = GATTS_GetAttributeValue(p_data->handle, &len, &value);
    if(status == GATT_SUCCESS && len > 0 && value) {
        if(len > GATT_MAX_ATTR_LEN) {
            len = GATT_MAX_ATTR_LEN;
        }
        p_rsp->attr_value.len = len;
        memcpy(p_rsp->attr_value.value, value, len);
    }
    return status;
}
#if GATTS_ROBUST_CACHING_ENABLED
static tGATT_STATUS gatt_sr_write_cl_supp_feat(UINT16 conn_id, tGATT_WRITE_REQ *p_data)
{
    UINT8 val_new;
    UINT8 val_old;
    UINT8 val_xor;
    UINT8 val_and;
    UINT8 *p = p_data->value;
    UINT8 tcb_idx = GATT_GET_TCB_IDX(conn_id);
    tGATT_TCB *p_tcb = gatt_get_tcb_by_idx(tcb_idx);

    GATT_TRACE_DEBUG("%s len %u, feat %x", __func__, p_data->len, *p);

    if (p_tcb == NULL) {
        GATT_TRACE_ERROR("%s no conn", __func__);
        return GATT_NOT_FOUND;
    }

    if (p_data->len != 1) {
        GATT_TRACE_ERROR("%s len %u", __func__, p_data->len);
        return GATT_INVALID_PDU;
    }

    STREAM_TO_UINT8(val_new, p);
    val_new = (val_new & BLE_GATT_CL_SUPP_FEAT_BITMASK);

    if (val_new == 0) {
        GATT_TRACE_ERROR("%s bit cannot be all zero", __func__);
        return GATT_VALUE_NOT_ALLOWED;
    }

    val_old = p_tcb->cl_supp_feat;
    val_xor = val_old ^ val_new;
    val_and = val_xor & val_new;
    if (val_and != val_xor) {
        GATT_TRACE_ERROR("%s bit cannot be reset", __func__);
        return GATT_VALUE_NOT_ALLOWED;
    }

    p_tcb->cl_supp_feat = val_new;
#if (SMP_INCLUDED == TRUE)
    bta_gatts_co_cl_feat_save(p_tcb->peer_bda, &p_tcb->cl_supp_feat);
#endif
    return GATT_SUCCESS;
}
#endif /* GATTS_ROBUST_CACHING_ENABLED */
/******************************************************************************
**
** Function         gatt_proc_write_req
**
** Description      GATT server process a write request.
**
** Returns          GATT_SUCCESS if successfully sent; otherwise error code.
**
*******************************************************************************/
tGATT_STATUS gatt_proc_write_req(UINT16 conn_id, tGATTS_REQ_TYPE type, tGATT_WRITE_REQ *p_data)
{
    if(p_data->len > GATT_MAX_ATTR_LEN) {
        p_data->len = GATT_MAX_ATTR_LEN;
    }
#if GATTS_ROBUST_CACHING_ENABLED
    if (p_data->handle == gatt_cb.handle_of_h_r) {
        return GATT_WRITE_NOT_PERMIT;
    }

    if (p_data->handle == gatt_cb.handle_of_cl_supported_feat) {
        return gatt_sr_write_cl_supp_feat(conn_id, p_data);
    }

    if (p_data->handle == gatt_cb.handle_of_database_hash) {
        return GATT_WRITE_NOT_PERMIT;
    }

    if (p_data->handle == gatt_cb.handle_of_sr_supported_feat) {
        return GATT_WRITE_NOT_PERMIT;
    }
#endif /* GATTS_ROBUST_CACHING_ENABLED */
    return GATTS_SetAttributeValue(p_data->handle,
                           p_data->len,
                           p_data->value);

}

/*******************************************************************************
**
** Function         gatt_request_cback
**
** Description      GATT profile attribute access request callback.
**
** Returns          void.
**
*******************************************************************************/
static void gatt_request_cback (UINT16 conn_id, UINT32 trans_id, tGATTS_REQ_TYPE type,
                                tGATTS_DATA *p_data)
{
    UINT8       status = GATT_INVALID_PDU;
    tGATTS_RSP   rsp_msg ;
    BOOLEAN     ignore = FALSE;
    GATT_TRACE_DEBUG("%s",__func__);
    memset(&rsp_msg, 0, sizeof(tGATTS_RSP));

    switch (type) {
    case GATTS_REQ_TYPE_READ:
        status = gatt_proc_read(conn_id, type, &p_data->read_req, &rsp_msg);
        break;

    case GATTS_REQ_TYPE_WRITE:
        if (!p_data->write_req.need_rsp) {
            ignore = TRUE;
        }
        status = gatt_proc_write_req(conn_id, type, &p_data->write_req);
        break;

    case GATTS_REQ_TYPE_WRITE_EXEC:
    case GATT_CMD_WRITE:
        ignore = TRUE;
        GATT_TRACE_EVENT("Ignore GATT_REQ_EXEC_WRITE/WRITE_CMD" );
        break;

    case GATTS_REQ_TYPE_MTU:
        GATT_TRACE_EVENT("Get MTU exchange new mtu size: %d", p_data->mtu);
        ignore = TRUE;
        break;

    default:
        GATT_TRACE_EVENT("Unknown/unexpected LE GAP ATT request: 0x%02x", type);
        break;
    }

    if (!ignore) {
        GATTS_SendRsp (conn_id, trans_id, status, &rsp_msg);
    }

}

/*******************************************************************************
**
** Function         gatt_connect_cback
**
** Description      Gatt profile connection callback.
**
** Returns          void
**
*******************************************************************************/
static void gatt_connect_cback (tGATT_IF gatt_if, BD_ADDR bda, UINT16 conn_id,
                                BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                tBT_TRANSPORT transport)
{
    UNUSED(gatt_if);

    GATT_TRACE_DEBUG ("%s: from %08x%04x connected:%d conn_id=%d reason = 0x%04x", __FUNCTION__,
                      (bda[0] << 24) + (bda[1] << 16) + (bda[2] << 8) + bda[3],
                      (bda[4] << 8) + bda[5], connected, conn_id, reason);

    tGATT_PROFILE_CLCB *p_clcb = gatt_profile_find_clcb_by_bd_addr(bda, transport);
    if (p_clcb == NULL) {
        p_clcb = gatt_profile_clcb_alloc (conn_id, bda, transport);
    }

    if (p_clcb == NULL) {
        return;
    }

    if (GATT_GetConnIdIfConnected (gatt_cb.gatt_if, bda, &p_clcb->conn_id, transport)) {
        p_clcb->connected = TRUE;
        p_clcb->conn_id = conn_id;
    }


    if (!p_clcb->connected) {
        /* wait for connection */
        return;
    }

    if (connected) {
        p_clcb->conn_id = conn_id;
        p_clcb->connected = TRUE;

    } else {
        gatt_profile_clcb_dealloc(p_clcb);
    }
}

/*******************************************************************************
**
** Function         gatt_profile_db_init
**
** Description      Initializa the GATT profile attribute database.
**
*******************************************************************************/
void gatt_profile_db_init (void)
{
    tBT_UUID          app_uuid = {LEN_UUID_128, {0}};
    tBT_UUID          uuid = {LEN_UUID_16, {UUID_SERVCLASS_GATT_SERVER}};
    UINT16            service_handle = 0;
    tGATT_STATUS      status;

    /* Fill our internal UUID with a fixed pattern 0x81 */
    memset (&app_uuid.uu.uuid128, 0x81, LEN_UUID_128);


    /* Create a GATT profile service */
    gatt_cb.gatt_if = GATT_Register(&app_uuid, &gatt_profile_cback);
    GATT_StartIf(gatt_cb.gatt_if);

    service_handle = GATTS_CreateService (gatt_cb.gatt_if , &uuid, 0, GATTP_MAX_ATTR_NUM, TRUE);
    GATT_TRACE_DEBUG ("GATTS_CreateService:  handle of service handle%x", service_handle);

    /* add Service Changed characteristic
    */
    uuid.uu.uuid16 = gatt_cb.gattp_attr.uuid = GATT_UUID_GATT_SRV_CHGD;
    gatt_cb.gattp_attr.service_change = 0;
    gatt_cb.gattp_attr.handle   =
    gatt_cb.handle_of_h_r       = GATTS_AddCharacteristic(service_handle, &uuid, 0, GATT_CHAR_PROP_BIT_INDICATE,
    												    NULL, NULL);

    GATT_TRACE_DEBUG ("gatt_profile_db_init:  handle of service changed%d\n",
                      gatt_cb.handle_of_h_r);

    tBT_UUID descr_uuid = {LEN_UUID_16, {GATT_UUID_CHAR_CLIENT_CONFIG}};
    uint8_t ccc_value[2] ={ 0x00, 0x00};

    tGATT_ATTR_VAL  attr_val = {
        .attr_max_len = sizeof(UINT16),
        .attr_len = sizeof(UINT16),
        .attr_val = ccc_value,
    };

    GATTS_AddCharDescriptor (service_handle, GATT_PERM_READ | GATT_PERM_WRITE , &descr_uuid, &attr_val, NULL);
#if GATTS_ROBUST_CACHING_ENABLED
    /* add Client Supported Features characteristic */
    uuid.uu.uuid16 = GATT_UUID_CLIENT_SUP_FEAT;
    gatt_cb.handle_of_cl_supported_feat = GATTS_AddCharacteristic(service_handle, &uuid, GATT_PERM_READ | GATT_PERM_WRITE,
        GATT_CHAR_PROP_BIT_READ | GATT_CHAR_PROP_BIT_WRITE, NULL, NULL);

    /* add Database Hash characteristic */
    uuid.uu.uuid16 = GATT_UUID_GATT_DATABASE_HASH;
    gatt_cb.handle_of_database_hash = GATTS_AddCharacteristic(service_handle, &uuid, GATT_PERM_READ, GATT_CHAR_PROP_BIT_READ, NULL, NULL);

    /* add Server Supported Features characteristic */
    uuid.uu.uuid16 = GATT_UUID_SERVER_SUP_FEAT;
    gatt_cb.handle_of_sr_supported_feat = GATTS_AddCharacteristic(service_handle, &uuid, GATT_PERM_READ, GATT_CHAR_PROP_BIT_READ, NULL, NULL);
#endif /* GATTS_ROBUST_CACHING_ENABLED */
    /* start service */
    status = GATTS_StartService (gatt_cb.gatt_if, service_handle, GATTP_TRANSPORT_SUPPORTED );

#if (CONFIG_BT_STACK_NO_LOG)
    (void) status;
#endif

    GATT_TRACE_DEBUG ("gatt_profile_db_init:  gatt_if=%d   start status%d\n",
                      gatt_cb.gatt_if,  status);
}

/*******************************************************************************
**
** Function         gatt_disc_res_cback
**
** Description      Gatt profile discovery result callback
**
** Returns          void
**
*******************************************************************************/
static void gatt_disc_res_cback (UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_DISC_RES *p_data)
{
    GATT_TRACE_DEBUG("%s, disc_type = %d",__func__, disc_type);
    tGATT_PROFILE_CLCB *p_clcb = gatt_profile_find_clcb_by_conn_id(conn_id);

    if (p_clcb == NULL) {
        return;
    }

    switch (disc_type) {
    case GATT_DISC_SRVC_BY_UUID:/* stage 1 */
        p_clcb->e_handle = p_data->value.group_value.e_handle;
        p_clcb->ccc_result ++;
        break;

    case GATT_DISC_CHAR:/* stage 2 */
        p_clcb->s_handle = p_data->value.dclr_value.val_handle;
        p_clcb->ccc_result ++;
        break;

    case GATT_DISC_CHAR_DSCPT: /* stage 3 */
        if (p_data->type.uu.uuid16 == GATT_UUID_CHAR_CLIENT_CONFIG) {
            p_clcb->s_handle = p_data->handle;
            p_clcb->ccc_result ++;
        }
        break;
    }
}

/*******************************************************************************
**
** Function         gatt_disc_cmpl_cback
**
** Description      Gatt profile discovery complete callback
**
** Returns          void
**
*******************************************************************************/
static void gatt_disc_cmpl_cback (UINT16 conn_id, tGATT_DISC_TYPE disc_type, tGATT_STATUS status)
{
    GATT_TRACE_DEBUG("%s",__func__);
    tGATT_PROFILE_CLCB *p_clcb = gatt_profile_find_clcb_by_conn_id(conn_id);

    if (p_clcb == NULL) {
        return;
    }

    if (status == GATT_SUCCESS && p_clcb->ccc_result > 0) {
        p_clcb->ccc_result = 0;
        p_clcb->ccc_stage ++;
        gatt_cl_start_config_ccc(p_clcb);
    } else {
        GATT_TRACE_ERROR("%s() - Register for service changed indication failure", __FUNCTION__);
    }
}

/*******************************************************************************
**
** Function         gatt_cl_op_cmpl_cback
**
** Description      Gatt profile client operation complete callback
**
** Returns          void
**
*******************************************************************************/
static void gatt_cl_op_cmpl_cback (UINT16 conn_id, tGATTC_OPTYPE op,
                                   tGATT_STATUS status, tGATT_CL_COMPLETE *p_data)
{
    GATT_TRACE_DEBUG("%s",__func__);
    tGATT_PROFILE_CLCB *p_clcb = gatt_profile_find_clcb_by_conn_id(conn_id);

    if (p_clcb == NULL) {
        return;
    }

    if (op == GATTC_OPTYPE_WRITE) {
        GATT_TRACE_DEBUG("%s() - ccc write status : %d", __FUNCTION__, status);
    }

}

/*******************************************************************************
**
** Function         gatt_cl_start_config_ccc
**
** Description      Gatt profile start configure service change CCC
**
** Returns          void
**
*******************************************************************************/
static void gatt_cl_start_config_ccc(tGATT_PROFILE_CLCB *p_clcb)
{
    tGATT_DISC_PARAM    srvc_disc_param;
    tGATT_VALUE         ccc_value;

    GATT_TRACE_DEBUG("%s() - stage: %d", __FUNCTION__, p_clcb->ccc_stage);

    memset (&srvc_disc_param, 0 , sizeof(tGATT_DISC_PARAM));
    memset (&ccc_value, 0 , sizeof(tGATT_VALUE));

    switch (p_clcb->ccc_stage) {
    case GATT_SVC_CHANGED_SERVICE: /* discover GATT service */
        srvc_disc_param.s_handle = 1;
        srvc_disc_param.e_handle = 0xffff;
        srvc_disc_param.service.len = 2;
        srvc_disc_param.service.uu.uuid16 = UUID_SERVCLASS_GATT_SERVER;
#if (GATTC_INCLUDED == TRUE)
        if (GATTC_Discover (p_clcb->conn_id, GATT_DISC_SRVC_BY_UUID, &srvc_disc_param) != GATT_SUCCESS) {
            GATT_TRACE_ERROR("%s() - ccc service error", __FUNCTION__);
        }
#endif  ///GATTC_INCLUDED == TRUE
        break;

    case GATT_SVC_CHANGED_CHARACTERISTIC: /* discover service change char */
        srvc_disc_param.s_handle = 1;
        srvc_disc_param.e_handle = p_clcb->e_handle;
        srvc_disc_param.service.len = 2;
        srvc_disc_param.service.uu.uuid16 = GATT_UUID_GATT_SRV_CHGD;
#if (GATTC_INCLUDED == TRUE)
        if (GATTC_Discover (p_clcb->conn_id, GATT_DISC_CHAR, &srvc_disc_param) != GATT_SUCCESS) {
            GATT_TRACE_ERROR("%s() - ccc char error", __FUNCTION__);
        }
#endif  ///GATTC_INCLUDED == TRUE
        break;

    case GATT_SVC_CHANGED_DESCRIPTOR: /* discover service change ccc */
        srvc_disc_param.s_handle = p_clcb->s_handle;
        srvc_disc_param.e_handle = p_clcb->e_handle;
#if (GATTC_INCLUDED == TRUE)
        if (GATTC_Discover (p_clcb->conn_id, GATT_DISC_CHAR_DSCPT, &srvc_disc_param) != GATT_SUCCESS) {
            GATT_TRACE_ERROR("%s() - ccc char descriptor error", __FUNCTION__);
        }
#endif  ///GATTC_INCLUDED == TRUE
        break;

    case GATT_SVC_CHANGED_CONFIGURE_CCCD: /* write ccc */
        ccc_value.handle = p_clcb->s_handle;
        ccc_value.len = 2;
        ccc_value.value[0] = GATT_CLT_CONFIG_INDICATION;
#if (GATTC_INCLUDED == TRUE)
        if (GATTC_Write (p_clcb->conn_id, GATT_WRITE, &ccc_value) != GATT_SUCCESS) {
            GATT_TRACE_ERROR("%s() - write ccc error", __FUNCTION__);
        }
#endif  ///GATTC_INCLUDED == TRUE
        break;
    }
}

/*******************************************************************************
**
** Function         GATT_ConfigServiceChangeCCC
**
** Description      Configure service change indication on remote device
**
** Returns          none
**
*******************************************************************************/
void GATT_ConfigServiceChangeCCC (BD_ADDR remote_bda, BOOLEAN enable, tBT_TRANSPORT transport)
{
    tGATT_PROFILE_CLCB   *p_clcb = gatt_profile_find_clcb_by_bd_addr (remote_bda, transport);

    if (p_clcb == NULL) {
        p_clcb = gatt_profile_clcb_alloc (0, remote_bda, transport);
    }

    if (p_clcb == NULL) {
        return;
    }

    if (GATT_GetConnIdIfConnected (gatt_cb.gatt_if, remote_bda, &p_clcb->conn_id, transport)) {
        p_clcb->connected = TRUE;
    }
    /* hold the link here */
    GATT_Connect(gatt_cb.gatt_if, remote_bda, BLE_ADDR_UNKNOWN_TYPE, TRUE, transport, FALSE);
    p_clcb->ccc_stage = GATT_SVC_CHANGED_CONNECTING;

    if (!p_clcb->connected) {
        /* wait for connection */
        return;
    }

    p_clcb->ccc_stage ++;
    gatt_cl_start_config_ccc(p_clcb);
}

#if GATTS_ROBUST_CACHING_ENABLED
/*******************************************************************************
**
** Function         gatt_sr_is_cl_robust_caching_supported
**
** Description      Check if Robust Caching is supported for the connection
**
** Returns          true if enabled by client side, otherwise false
**
*******************************************************************************/
static BOOLEAN gatt_sr_is_cl_robust_caching_supported(tGATT_TCB *p_tcb)
{
    return (p_tcb->cl_supp_feat & BLE_GATT_CL_SUPP_FEAT_ROBUST_CACHING_BITMASK);
}
/*******************************************************************************
**
** Function         gatt_sr_is_cl_change_aware
**
** Description      Check if the connection is change-aware
**
** Returns          true if change aware, otherwise false
**
*******************************************************************************/
BOOLEAN gatt_sr_is_cl_change_aware(tGATT_TCB *p_tcb)
{
    // If robust caching is not supported, should always return true by default
    if (!gatt_sr_is_cl_robust_caching_supported(p_tcb)) {
        return true;
    }

    return p_tcb->is_robust_cache_change_aware;
}

/*******************************************************************************
**
** Function         gatt_sr_init_cl_status
**
** Description      Restore status for trusted device
**
** Returns          none
**
*******************************************************************************/
void gatt_sr_init_cl_status(tGATT_TCB *p_tcb)
{
#if (SMP_INCLUDED == TRUE)
    bta_gatts_co_cl_feat_load(p_tcb->peer_bda, &p_tcb->cl_supp_feat);
#endif

    // This is used to reset bit when robust caching is disabled
    if (!GATTS_ROBUST_CACHING_ENABLED) {
        p_tcb->cl_supp_feat &= ~BLE_GATT_CL_SUPP_FEAT_ROBUST_CACHING_BITMASK;
    }

    if (gatt_sr_is_cl_robust_caching_supported(p_tcb)) {
        BT_OCTET16 stored_hash = {0};
#if (SMP_INCLUDED == TRUE)
        bta_gatts_co_db_hash_load(p_tcb->peer_bda, stored_hash);
#endif
        p_tcb->is_robust_cache_change_aware = (memcmp(stored_hash, gatt_cb.database_hash, BT_OCTET16_LEN) == 0);
    } else {
        p_tcb->is_robust_cache_change_aware = true;
    }

    GATT_TRACE_DEBUG("%s feat %x aware %d", __func__, p_tcb->cl_supp_feat, p_tcb->is_robust_cache_change_aware);
}

/*******************************************************************************
**
** Function         gatt_sr_update_cl_status
**
** Description      Update change-aware status for the remote device
**
** Returns          none
**
*******************************************************************************/
void gatt_sr_update_cl_status(tGATT_TCB *p_tcb, BOOLEAN chg_aware)
{
    if (p_tcb == NULL) {
        return;
    }

    // if robust caching is not supported, do nothing
    if (!gatt_sr_is_cl_robust_caching_supported(p_tcb)) {
        return;
    }

    // only when client status is changed from unaware to aware, we should store database hash
    if (!p_tcb->is_robust_cache_change_aware && chg_aware) {
#if (SMP_INCLUDED == TRUE)
        bta_gatts_co_db_hash_save(p_tcb->peer_bda, gatt_cb.database_hash);
#endif
    }

    p_tcb->is_robust_cache_change_aware = chg_aware;

    GATT_TRACE_DEBUG("%s status %d", __func__, chg_aware);
}
#endif /* GATTS_ROBUST_CACHING_ENABLED */
#endif  /* BLE_INCLUDED == TRUE && GATTS_INCLUDED == TRUE */
