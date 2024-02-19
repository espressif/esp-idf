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
 *  This file contains the GATT client utility function.
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if defined(GATTC_INCLUDED) && (GATTC_INCLUDED == TRUE)

#include <string.h>

#include "device/bdaddr.h"
// #include "btif/include/btif_util.h"
#include "bta/utl.h"
#include "bta/bta_sys.h"
#include "bta_gattc_int.h"
#include "stack/l2c_api.h"
#include "osi/allocator.h"

/*****************************************************************************
**  Constants
*****************************************************************************/


static const UINT8  base_uuid[LEN_UUID_128] = {0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
                                               0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                              };

static const BD_ADDR dummy_bda = {0, 0, 0, 0, 0, 0};

#define GATTC_COMMAND_QUEUE_SIZE_MAX    30

/*******************************************************************************
**
** Function         bta_gatt_convert_uuid16_to_uuid128
**
** Description      Convert a 16 bits UUID to be an standard 128 bits one.
**
** Returns          TRUE if two uuid match; FALSE otherwise.
**
*******************************************************************************/
void bta_gatt_convert_uuid16_to_uuid128(UINT8 uuid_128[LEN_UUID_128], UINT16 uuid_16)
{
    UINT8   *p = &uuid_128[LEN_UUID_128 - 4];

    memcpy (uuid_128, base_uuid, LEN_UUID_128);

    UINT16_TO_STREAM(p, uuid_16);
}
/*******************************************************************************
**
** Function         bta_gattc_uuid_compare
**
** Description      Compare two UUID to see if they are the same.
**
** Returns          TRUE if two uuid match; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_gattc_uuid_compare (const tBT_UUID *p_src, const tBT_UUID *p_tar, BOOLEAN is_precise)
{
    UINT8  su[LEN_UUID_128], tu[LEN_UUID_128];
    const UINT8  *ps, *pt;

    /* any of the UUID is unspecified */
    if (p_src == 0 || p_tar == 0) {
        if (is_precise) {
            return FALSE;
        } else {
            return TRUE;
        }
    }

    /* If both are 16-bit, we can do a simple compare */
    if (p_src->len == 2 && p_tar->len == 2) {
        return p_src->uu.uuid16 == p_tar->uu.uuid16;
    }

    /* One or both of the UUIDs is 128-bit */
    if (p_src->len == LEN_UUID_16) {
        /* convert a 16 bits UUID to 128 bits value */
        bta_gatt_convert_uuid16_to_uuid128(su, p_src->uu.uuid16);
        ps = su;
    } else {
        ps = p_src->uu.uuid128;
    }

    if (p_tar->len == LEN_UUID_16) {
        /* convert a 16 bits UUID to 128 bits value */
        bta_gatt_convert_uuid16_to_uuid128(tu, p_tar->uu.uuid16);
        pt = tu;
    } else {
        pt = p_tar->uu.uuid128;
    }

    return (memcmp(ps, pt, LEN_UUID_128) == 0);
}

/*******************************************************************************
**
** Function         bta_gattc_cl_get_regcb
**
** Description      get registration control block by client interface.
**
** Returns          pointer to the regcb
**
*******************************************************************************/
tBTA_GATTC_RCB *bta_gattc_cl_get_regcb(UINT8 client_if)
{
    UINT8   i = 0;
    tBTA_GATTC_RCB  *p_clrcb = &bta_gattc_cb.cl_rcb[0];

    for (i = 0; i < BTA_GATTC_CL_MAX; i ++, p_clrcb ++) {
        if (p_clrcb->in_use &&
                p_clrcb->client_if == client_if) {
            return p_clrcb;
        }
    }
    return NULL;
}
/*******************************************************************************
**
** Function         bta_gattc_num_reg_app
**
** Description      find the number of registered application.
**
** Returns          pointer to the regcb
**
*******************************************************************************/
UINT8 bta_gattc_num_reg_app(void)
{
    UINT8   i = 0, j = 0;

    for (i = 0; i < BTA_GATTC_CL_MAX; i ++) {
        if (bta_gattc_cb.cl_rcb[i].in_use) {
            j ++;
        }
    }
    return j;
}
/*******************************************************************************
**
** Function         bta_gattc_find_clcb_by_cif
**
** Description      get clcb by client interface and remote bd adddress
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CLCB *bta_gattc_find_clcb_by_cif (UINT8 client_if, BD_ADDR remote_bda,
        tBTA_TRANSPORT transport)
{
    tBTA_GATTC_CLCB *p_clcb = &bta_gattc_cb.clcb[0];
    UINT8   i;

    for (i = 0; i < BTA_GATTC_CLCB_MAX; i ++, p_clcb ++) {
        if (p_clcb->in_use &&
                p_clcb->p_rcb->client_if == client_if &&
                p_clcb->transport == transport &&
                bdcmp(p_clcb->bda, remote_bda) == 0) {
            return p_clcb;
        }
    }
    return NULL;
}
/*******************************************************************************
**
** Function         bta_gattc_find_clcb_by_conn_id
**
** Description      get clcb by connection ID
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CLCB *bta_gattc_find_clcb_by_conn_id (UINT16 conn_id)
{
    tBTA_GATTC_CLCB *p_clcb = &bta_gattc_cb.clcb[0];
    UINT8 i;

    for (i = 0; i < BTA_GATTC_CLCB_MAX; i ++, p_clcb ++) {
        if (p_clcb->in_use &&
                p_clcb->bta_conn_id == conn_id) {
            return p_clcb;
        }
    }
    return NULL;
}

/*******************************************************************************
**
** Function         bta_gattc_clcb_alloc
**
** Description      allocate CLCB
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CLCB *bta_gattc_clcb_alloc(tBTA_GATTC_IF client_if, BD_ADDR remote_bda,
                                      tBTA_TRANSPORT transport)
{
    UINT8               i_clcb = 0;
    tBTA_GATTC_CLCB     *p_clcb = NULL;

    for (i_clcb = 0; i_clcb < BTA_GATTC_CLCB_MAX; i_clcb++) {
        if (!bta_gattc_cb.clcb[i_clcb].in_use) {
#if BTA_GATT_DEBUG == TRUE
            APPL_TRACE_DEBUG("bta_gattc_clcb_alloc: found clcb[%d] available", i_clcb);
#endif
            p_clcb                  = &bta_gattc_cb.clcb[i_clcb];
            p_clcb->in_use          = TRUE;
            p_clcb->status          = BTA_GATT_OK;
            p_clcb->transport       = transport;
            bdcpy(p_clcb->bda, remote_bda);
            p_clcb->searched_service_source = BTA_GATTC_SERVICE_INFO_FROM_UNKNOWN;
            p_clcb->p_rcb = bta_gattc_cl_get_regcb(client_if);
            if (p_clcb->p_cmd_list == NULL) {
                p_clcb->p_cmd_list = list_new(osi_free_func);
            }
            if ((p_clcb->p_srcb = bta_gattc_find_srcb(remote_bda)) == NULL) {
                p_clcb->p_srcb      = bta_gattc_srcb_alloc(remote_bda);
            }

            if (p_clcb->p_rcb != NULL && p_clcb->p_srcb != NULL) {
                p_clcb->p_srcb->num_clcb ++;
                p_clcb->p_rcb->num_clcb ++;
            } else {
                /* release this clcb if clcb or srcb allocation failed */
                p_clcb->in_use = FALSE;
                p_clcb = NULL;
            }
            break;
        }
    }
    return p_clcb;
}
/*******************************************************************************
**
** Function         bta_gattc_find_alloc_clcb
**
** Description      find or allocate CLCB if not found.
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CLCB *bta_gattc_find_alloc_clcb(tBTA_GATTC_IF client_if, BD_ADDR remote_bda,
        tBTA_TRANSPORT transport)
{
    tBTA_GATTC_CLCB *p_clcb ;

    if ((p_clcb = bta_gattc_find_clcb_by_cif(client_if, remote_bda, transport)) == NULL) {
        p_clcb = bta_gattc_clcb_alloc(client_if, remote_bda, transport);
    }
    return p_clcb;
}

/*******************************************************************************
**
** Function         bta_gattc_clcb_dealloc
**
** Description      Deallocte a clcb
**
** Returns          pointer to the clcb
**
*******************************************************************************/
void bta_gattc_clcb_dealloc(tBTA_GATTC_CLCB *p_clcb)
{
    tBTA_GATTC_SERV     *p_srcb = NULL;

    if (p_clcb) {
        p_srcb = p_clcb->p_srcb;
        if (p_srcb->num_clcb) {
            p_srcb->num_clcb --;
        }

        if (p_clcb->p_rcb->num_clcb) {
            p_clcb->p_rcb->num_clcb --;
        }

        /* if the srcb is no longer needed, reset the state */
        if ( p_srcb->num_clcb == 0) {
            p_srcb->connected = FALSE;
            p_srcb->state = BTA_GATTC_SERV_IDLE;
            p_srcb->mtu = 0;

            /* clean up cache */
            if (p_srcb->p_srvc_cache) {
                list_free(p_srcb->p_srvc_cache);
                p_srcb->p_srvc_cache = NULL;
            }
        }

        if ( p_clcb->p_q_cmd != NULL && !list_contains(p_clcb->p_cmd_list, p_clcb->p_q_cmd)){
            osi_free(p_clcb->p_q_cmd);
            p_clcb->p_q_cmd = NULL;
        }
        // don't forget to clear the command queue before dealloc the clcb.
        list_clear(p_clcb->p_cmd_list);
        osi_free((void *)p_clcb->p_cmd_list);
        p_clcb->p_cmd_list = NULL;
        //osi_free_and_reset((void **)&p_clcb->p_q_cmd);
        memset(p_clcb, 0, sizeof(tBTA_GATTC_CLCB));
    } else {
        APPL_TRACE_ERROR("bta_gattc_clcb_dealloc p_clcb=NULL");
    }
}

void bta_gattc_clcb_dealloc_by_conn_id(UINT16 conn_id)
{
    tBTA_GATTC_CLCB *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);

    if (p_clcb) {
        bta_gattc_clcb_dealloc(p_clcb);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_find_srcb
**
** Description      find server cache by remote bd address currently in use
**
** Returns          pointer to the server cache.
**
*******************************************************************************/
tBTA_GATTC_SERV *bta_gattc_find_srcb(BD_ADDR bda)
{
    tBTA_GATTC_SERV *p_srcb = &bta_gattc_cb.known_server[0];
    UINT8   i;

    for (i = 0; i < BTA_GATTC_KNOWN_SR_MAX; i ++, p_srcb ++) {
        if (p_srcb->in_use && bdcmp(p_srcb->server_bda, bda) == 0) {
            return p_srcb;
        }
    }
    return NULL;
}

/*******************************************************************************
**
** Function         bta_gattc_find_srvr_cache
**
** Description      find server cache by remote bd address
**
** Returns          pointer to the server cache.
**
*******************************************************************************/
tBTA_GATTC_SERV *bta_gattc_find_srvr_cache(BD_ADDR bda)
{
    tBTA_GATTC_SERV *p_srcb = &bta_gattc_cb.known_server[0];
    UINT8   i;

    for (i = 0; i < BTA_GATTC_KNOWN_SR_MAX; i ++, p_srcb ++) {
        if (bdcmp(p_srcb->server_bda, bda) == 0) {
            return p_srcb;
        }
    }
    return NULL;
}
/*******************************************************************************
**
** Function         bta_gattc_find_scb_by_cid
**
** Description      find server control block by connection ID
**
** Returns          pointer to the server cache.
**
*******************************************************************************/
tBTA_GATTC_SERV *bta_gattc_find_scb_by_cid (UINT16 conn_id)
{
    tBTA_GATTC_CLCB *p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);

    if (p_clcb) {
        return p_clcb->p_srcb;
    } else {
        return NULL;
    }
}
/*******************************************************************************
**
** Function         bta_gattc_srcb_alloc
**
** Description      allocate server cache control block
**
** Returns          pointer to the server cache.
**
*******************************************************************************/
tBTA_GATTC_SERV *bta_gattc_srcb_alloc(BD_ADDR bda)
{
    tBTA_GATTC_SERV *p_tcb = &bta_gattc_cb.known_server[0],
                     *p_recycle = NULL;
    BOOLEAN         found = FALSE;
    UINT8           i;

    for (i = 0; i < BTA_GATTC_KNOWN_SR_MAX; i ++, p_tcb ++) {
        if (!p_tcb->in_use) {
            found = TRUE;
            break;
        } else if (!p_tcb->connected) {
            p_recycle = p_tcb;
        }
    }

    /* if not found, try to recycle one known device */
    if (!found && !p_recycle) {
        p_tcb = NULL;
	}
    else if (!found && p_recycle) {
        p_tcb = p_recycle;
	}

    if (p_tcb != NULL)
    {
        if (p_tcb->p_srvc_cache != NULL) {
            list_free(p_tcb->p_srvc_cache);
            p_tcb->p_srvc_cache = NULL;
        }
        osi_free(p_tcb->p_srvc_list);
        p_tcb->p_srvc_list = NULL;
        //osi_free_and_reset((void **)&p_tcb->p_srvc_list);
        memset(p_tcb, 0 , sizeof(tBTA_GATTC_SERV));

        p_tcb->in_use = TRUE;
        bdcpy(p_tcb->server_bda, bda);
    }
    return p_tcb;
}

static BOOLEAN bta_gattc_has_prepare_command_in_queue(tBTA_GATTC_CLCB *p_clcb)
{
    assert(p_clcb != NULL);

    for(list_node_t *sn = list_begin(p_clcb->p_cmd_list);
        sn != list_end(p_clcb->p_cmd_list); sn = list_next(sn)) {

        tBTA_GATTC_DATA *cmd_data = (tBTA_GATTC_DATA *)list_node(sn);
        if (cmd_data != NULL && ((cmd_data->hdr.event == BTA_GATTC_API_WRITE_EVT &&
            cmd_data->api_write.write_type == BTA_GATTC_WRITE_PREPARE) ||
            cmd_data->hdr.event == BTA_GATTC_API_EXEC_EVT)) {
            return TRUE;
        }
    }

    return FALSE;
}
/*******************************************************************************
**
** Function         bta_gattc_enqueue
**
** Description      enqueue a client request in clcb.
**
** Returns          success or failure.
**
*******************************************************************************/
BOOLEAN bta_gattc_enqueue(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC cb_data = {0};

    if (p_clcb->p_q_cmd == NULL) {
        p_clcb->p_q_cmd = p_data;
        return TRUE;
    } else if ((p_data->hdr.event == BTA_GATTC_API_WRITE_EVT &&
               p_data->api_write.write_type == BTA_GATTC_WRITE_PREPARE) &&
               ((p_clcb->p_q_cmd->hdr.event == BTA_GATTC_API_WRITE_EVT &&
               p_clcb->p_q_cmd->api_write.write_type == BTA_GATTC_WRITE_PREPARE) ||
               bta_gattc_has_prepare_command_in_queue(p_clcb))) {
        APPL_TRACE_DEBUG("%s(), prepare offset = %d", __func__, p_data->api_write.offset);
        cb_data.write.status = BTA_GATT_CONGESTED;
        cb_data.write.handle = p_data->api_write.handle;
        cb_data.write.conn_id = p_clcb->bta_conn_id;
        cb_data.write.offset = p_data->api_write.offset;
        /* write complete, callback */
        if (p_clcb->p_rcb->p_cback != NULL) {
            ( *p_clcb->p_rcb->p_cback)(BTA_GATTC_PREP_WRITE_EVT, (tBTA_GATTC *)&cb_data);
        }
        return FALSE;
    }
    else if (p_clcb->p_cmd_list) {
        UINT16 len = 0;
        tBTA_GATTC_DATA *cmd_data = NULL;

        if (list_length(p_clcb->p_cmd_list) >= GATTC_COMMAND_QUEUE_SIZE_MAX) {

            APPL_TRACE_ERROR("%s(), the gattc command queue is full.", __func__);
            cb_data.status = GATT_BUSY;
            cb_data.queue_full.conn_id = p_clcb->bta_conn_id;
            cb_data.queue_full.is_full = TRUE;
            p_clcb->is_full = TRUE;
            if (p_clcb->p_rcb->p_cback != NULL) {
                ( *p_clcb->p_rcb->p_cback)(BTA_GATTC_QUEUE_FULL_EVT, (tBTA_GATTC *)&cb_data);
            }
            return FALSE;
        }

        if (p_data->hdr.event == BTA_GATTC_API_WRITE_EVT) {
            len = p_data->api_write.len;
            if ((cmd_data = (tBTA_GATTC_DATA *)osi_malloc(sizeof(tBTA_GATTC_DATA) + len)) != NULL) {
                memset(cmd_data, 0, sizeof(tBTA_GATTC_DATA) + len);
			    memcpy(cmd_data, p_data, sizeof(tBTA_GATTC_DATA));
                cmd_data->api_write.p_value = (UINT8 *)(cmd_data + 1);
			    memcpy(cmd_data->api_write.p_value, p_data->api_write.p_value, len);
            } else {
                APPL_TRACE_ERROR("%s(), line = %d, alloc fail, no memery.", __func__, __LINE__);
                return FALSE;
            }
        } else {
            if ((cmd_data = (tBTA_GATTC_DATA *)osi_malloc(sizeof(tBTA_GATTC_DATA))) != NULL) {
                memset(cmd_data, 0, sizeof(tBTA_GATTC_DATA));
                memcpy(cmd_data, p_data, sizeof(tBTA_GATTC_DATA));
            } else {
                APPL_TRACE_ERROR("%s(), line = %d, alloc fail, no memery.", __func__, __LINE__);
                return FALSE;
            }
        }

        //store the command to the command list.
        list_append(p_clcb->p_cmd_list, (void *)cmd_data);
        return FALSE;
    }

    return FALSE;
}

/*******************************************************************************
**
** Function         bta_gattc_check_notif_registry
**
** Description      check if the service notificaition has been registered.
**
** Returns
**
*******************************************************************************/
BOOLEAN bta_gattc_check_notif_registry(tBTA_GATTC_RCB  *p_clreg, tBTA_GATTC_SERV *p_srcb,
                                       tBTA_GATTC_NOTIFY  *p_notify)
{
    UINT8           i;

    for (i = 0 ; i < BTA_GATTC_NOTIF_REG_MAX; i ++)
    {
        if (p_clreg->notif_reg[i].in_use &&
            bdcmp(p_clreg->notif_reg[i].remote_bda, p_srcb->server_bda) == 0 &&
            p_clreg->notif_reg[i].handle == p_notify->handle)
        {
            APPL_TRACE_DEBUG("Notification registered!");
            return TRUE;
        }
    }
    return FALSE;

}
/*******************************************************************************
**
** Function         bta_gattc_clear_notif_registration
**
** Description      Clear up the notification registration information by BD_ADDR.
**                  Where handle is between start_handle and end_handle, and
**                  start_handle and end_handle are boundaries of service
**                  containing characteristic.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_clear_notif_registration(tBTA_GATTC_SERV *p_srcb, UINT16 conn_id,
                                        UINT16 start_handle, UINT16 end_handle)
{
    BD_ADDR             remote_bda;
    tBTA_GATTC_IF       gatt_if;
    tBTA_GATTC_RCB      *p_clrcb ;
    UINT8       i;
    tGATT_TRANSPORT     transport;
    UINT16              handle = 0;

    if (GATT_GetConnectionInfor(conn_id, &gatt_if, remote_bda, &transport)) {
        if ((p_clrcb = bta_gattc_cl_get_regcb(gatt_if)) != NULL) {
            for (i = 0 ; i < BTA_GATTC_NOTIF_REG_MAX; i ++) {
                if (p_clrcb->notif_reg[i].in_use &&
                    !bdcmp(p_clrcb->notif_reg[i].remote_bda, remote_bda))
                {
                    /* It's enough to get service or characteristic handle, as
                     * clear boundaries are always around service.
                     */
                    handle = p_clrcb->notif_reg[i].handle;
                    if (handle >= start_handle && handle <= end_handle) {
                        memset(&p_clrcb->notif_reg[i], 0, sizeof(tBTA_GATTC_NOTIF_REG));
                    }
                }
            }
        }
    } else {
        APPL_TRACE_ERROR("can not clear indication/notif registration for unknown app");
    }
    return;
}

/*******************************************************************************
**
** Function         bta_gattc_clear_notif_registration_by_bda
**
** Description      Clear up the notification registration information by BD_ADDR.
**
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_clear_notif_registration_by_bda(tBTA_GATTC_RCB *p_clrcb, BD_ADDR remote_bda)
{
    if(p_clrcb == NULL) {
        return;
    }
    for (uint8_t i = 0 ; i < BTA_GATTC_NOTIF_REG_MAX; i ++) {
        if (p_clrcb->notif_reg[i].in_use &&
            !bdcmp(p_clrcb->notif_reg[i].remote_bda, remote_bda))
        {
            memset(&p_clrcb->notif_reg[i], 0, sizeof(tBTA_GATTC_NOTIF_REG));
        }
    }
}

/*******************************************************************************
**
** Function         bta_gattc_mark_bg_conn
**
** Description      mark background connection status when a bg connection is initiated
**                  or terminated.
**
** Returns          TRUE if success; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_gattc_mark_bg_conn (tBTA_GATTC_IF client_if,  BD_ADDR_PTR remote_bda_ptr,
                                BOOLEAN add, BOOLEAN is_listen)
{
    tBTA_GATTC_BG_TCK   *p_bg_tck = &bta_gattc_cb.bg_track[0];
    UINT8   i = 0;
    tBTA_GATTC_CIF_MASK  *p_cif_mask;

    for (i = 0; i < BTA_GATTC_KNOWN_SR_MAX; i ++, p_bg_tck ++) {
        if (p_bg_tck->in_use &&
                ((remote_bda_ptr != NULL && bdcmp(p_bg_tck->remote_bda, remote_bda_ptr) == 0) ||
                 (remote_bda_ptr == NULL && bdcmp(p_bg_tck->remote_bda, dummy_bda) == 0))) {
            p_cif_mask = is_listen ? &p_bg_tck->cif_adv_mask : &p_bg_tck->cif_mask;

            if (add)
                /* mask on the cif bit */
            {
                *p_cif_mask |= (1 << (client_if - 1));
            } else {
                if (client_if != 0) {
                    *p_cif_mask &= (~(1 << (client_if - 1)));
                } else {
                    *p_cif_mask = 0;
                }
            }
            /* no BG connection for this device, make it available */
            if (p_bg_tck->cif_mask == 0 && p_bg_tck->cif_adv_mask == 0) {
                memset(p_bg_tck, 0, sizeof(tBTA_GATTC_BG_TCK));
            }
            return TRUE;
        }
    }
    if (!add) {
        if (remote_bda_ptr) {
#if (!CONFIG_BT_STACK_NO_LOG)
            char bdstr[18] = {0};
#endif
            APPL_TRACE_WARNING("%s unable to find the bg connection mask for: %s", __func__,
                             bdaddr_to_string((bt_bdaddr_t *)remote_bda_ptr, bdstr, sizeof(bdstr)));
        }
        return FALSE;
    } else { /* adding a new device mask */
        for (i = 0, p_bg_tck = &bta_gattc_cb.bg_track[0];
                i < BTA_GATTC_KNOWN_SR_MAX; i ++, p_bg_tck ++) {
            if (!p_bg_tck->in_use) {
                p_bg_tck->in_use = TRUE;
                if (remote_bda_ptr) {
                    bdcpy(p_bg_tck->remote_bda, remote_bda_ptr);
                } else {
                    bdcpy(p_bg_tck->remote_bda, dummy_bda);
                }

                p_cif_mask = is_listen ? &p_bg_tck->cif_adv_mask : &p_bg_tck->cif_mask;

                *p_cif_mask = (1 << (client_if - 1));
                return TRUE;
            }
        }
        APPL_TRACE_ERROR("no available space to mark the bg connection status");
        return FALSE;
    }
}
/*******************************************************************************
**
** Function         bta_gattc_check_bg_conn
**
** Description      check if this is a background connection background connection.
**
** Returns          TRUE if success; FALSE otherwise.
**
*******************************************************************************/
BOOLEAN bta_gattc_check_bg_conn (tBTA_GATTC_IF client_if,  BD_ADDR remote_bda, UINT8 role)
{
    tBTA_GATTC_BG_TCK   *p_bg_tck = &bta_gattc_cb.bg_track[0];
    UINT8       i = 0;
    BOOLEAN     is_bg_conn = FALSE;

    for (i = 0; i < BTA_GATTC_KNOWN_SR_MAX && !is_bg_conn; i ++, p_bg_tck ++) {
        if (p_bg_tck->in_use &&
                (bdcmp(p_bg_tck->remote_bda, remote_bda) == 0 ||
                 bdcmp(p_bg_tck->remote_bda, dummy_bda) == 0)) {
            if (((p_bg_tck->cif_mask & (1 << (client_if - 1))) != 0) &&
                    role == HCI_ROLE_MASTER) {
                is_bg_conn = TRUE;
            }

            if (((p_bg_tck->cif_adv_mask & (1 << (client_if - 1))) != 0) &&
                    role == HCI_ROLE_SLAVE) {
                is_bg_conn = TRUE;
            }
        }
    }
    return is_bg_conn;
}
/*******************************************************************************
**
** Function         bta_gattc_send_open_cback
**
** Description      send open callback
**
** Returns
**
*******************************************************************************/
void bta_gattc_send_open_cback( tBTA_GATTC_RCB *p_clreg, tBTA_GATT_STATUS status,
                                BD_ADDR remote_bda, UINT16 conn_id,
                                tBTA_TRANSPORT transport, UINT16 mtu)
{

    tBTA_GATTC      cb_data;

    if (p_clreg->p_cback) {
        memset(&cb_data, 0, sizeof(tBTA_GATTC));

        cb_data.open.status = status;
        cb_data.open.client_if = p_clreg->client_if;
        cb_data.open.conn_id = conn_id;
        cb_data.open.mtu = mtu;
        cb_data.open.transport = transport;
        bdcpy(cb_data.open.remote_bda, remote_bda);

        (*p_clreg->p_cback)(BTA_GATTC_OPEN_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_send_connect_cback
**
** Description      send connect callback
**
** Returns
**
*******************************************************************************/
void bta_gattc_send_connect_cback( tBTA_GATTC_RCB *p_clreg, BD_ADDR remote_bda, UINT16 conn_id,
                                tBTA_GATT_CONN_PARAMS conn_params, UINT8 link_role, UINT8 ble_addr_type, UINT16 conn_handle)
{
    tBTA_GATTC      cb_data;

    if (p_clreg->p_cback) {
        memset(&cb_data, 0, sizeof(tBTA_GATTC));

        cb_data.connect.client_if = p_clreg->client_if;
        cb_data.connect.conn_id = conn_id;
        cb_data.connect.link_role = link_role;
        cb_data.connect.conn_params.interval = conn_params.interval;
        cb_data.connect.conn_params.latency = conn_params.latency;
        cb_data.connect.conn_params.timeout = conn_params.timeout;
        bdcpy(cb_data.connect.remote_bda, remote_bda);
        cb_data.connect.ble_addr_type = ble_addr_type;
        cb_data.connect.conn_handle = conn_handle;

        (*p_clreg->p_cback)(BTA_GATTC_CONNECT_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_send_disconnect_cback
**
** Description      send disconnect callback
**
** Returns
**
*******************************************************************************/
void bta_gattc_send_disconnect_cback( tBTA_GATTC_RCB *p_clreg, tGATT_DISCONN_REASON reason,
                                BD_ADDR remote_bda, UINT16 conn_id)
{
    tBTA_GATTC      cb_data;

    if (p_clreg->p_cback) {
        memset(&cb_data, 0, sizeof(tBTA_GATTC));

        cb_data.disconnect.reason = reason;
        cb_data.disconnect.client_if = p_clreg->client_if;
        cb_data.disconnect.conn_id = conn_id;
        bdcpy(cb_data.disconnect.remote_bda, remote_bda);

        (*p_clreg->p_cback)(BTA_GATTC_DISCONNECT_EVT, &cb_data);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_conn_alloc
**
** Description      allocate connection tracking spot
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CONN *bta_gattc_conn_alloc(BD_ADDR remote_bda)
{
    UINT8               i_conn = 0;
    tBTA_GATTC_CONN     *p_conn = &bta_gattc_cb.conn_track[0];

    for (i_conn = 0; i_conn < BTA_GATTC_CONN_MAX; i_conn++, p_conn ++) {
        if (!p_conn->in_use) {
#if BTA_GATT_DEBUG == TRUE
            APPL_TRACE_DEBUG("bta_gattc_conn_alloc: found conn_track[%d] available", i_conn);
#endif
            p_conn->in_use          = TRUE;
            bdcpy(p_conn->remote_bda, remote_bda);
            return p_conn;
        }
    }
    return NULL;
}

/*******************************************************************************
**
** Function         bta_gattc_conn_find
**
** Description      allocate connection tracking spot
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CONN *bta_gattc_conn_find(BD_ADDR remote_bda)
{
    UINT8               i_conn = 0;
    tBTA_GATTC_CONN     *p_conn = &bta_gattc_cb.conn_track[0];

    for (i_conn = 0; i_conn < BTA_GATTC_CONN_MAX; i_conn++, p_conn ++) {
        if (p_conn->in_use && bdcmp(remote_bda, p_conn->remote_bda) == 0) {
#if BTA_GATT_DEBUG == TRUE
            APPL_TRACE_DEBUG("bta_gattc_conn_find: found conn_track[%d] matched", i_conn);
#endif
            return p_conn;
        }
    }
    return NULL;
}


/*******************************************************************************
**
** Function         bta_gattc_conn_find_alloc
**
** Description      find or allocate connection tracking spot
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CONN *bta_gattc_conn_find_alloc(BD_ADDR remote_bda)
{
    tBTA_GATTC_CONN     *p_conn = bta_gattc_conn_find (remote_bda);

    if (p_conn == NULL) {
        p_conn = bta_gattc_conn_alloc(remote_bda);
    }
    return p_conn;
}

/*******************************************************************************
**
** Function         bta_gattc_conn_dealloc
**
** Description      de-allocate connection tracking spot
**
** Returns          pointer to the clcb
**
*******************************************************************************/
BOOLEAN bta_gattc_conn_dealloc(BD_ADDR remote_bda)
{
    tBTA_GATTC_CONN     *p_conn = bta_gattc_conn_find (remote_bda);

    if (p_conn != NULL) {
        p_conn->in_use = FALSE;
        memset(p_conn->remote_bda, 0, BD_ADDR_LEN);
        return TRUE;
    }
    return FALSE;
}

/*******************************************************************************
**
** Function         bta_gattc_find_int_conn_clcb
**
** Description      try to locate a clcb when an internal connecion event arrives.
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CLCB *bta_gattc_find_int_conn_clcb(tBTA_GATTC_DATA *p_msg)
{
    tBTA_GATTC_CLCB *p_clcb = NULL;

    if (p_msg->int_conn.role == HCI_ROLE_SLAVE) {
        bta_gattc_conn_find_alloc(p_msg->int_conn.remote_bda);
    }

    /* try to locate a logic channel */
    if ((p_clcb = bta_gattc_find_clcb_by_cif(p_msg->int_conn.client_if,
                  p_msg->int_conn.remote_bda,
                  p_msg->int_conn.transport)) == NULL) {
        /* for a background connection or listening connection */
        if (/*p_msg->int_conn.role == HCI_ROLE_SLAVE ||  */
            bta_gattc_check_bg_conn(p_msg->int_conn.client_if,
                                    p_msg->int_conn.remote_bda,
                                    p_msg->int_conn.role)) {
            /* allocate a new channel */
            p_clcb = bta_gattc_clcb_alloc(p_msg->int_conn.client_if,
                                          p_msg->int_conn.remote_bda,
                                          p_msg->int_conn.transport);
        }
    }
    return p_clcb;
}

/*******************************************************************************
**
** Function         bta_gattc_find_int_disconn_clcb
**
** Description      try to locate a clcb when an internal disconnect callback arrives.
**
** Returns          pointer to the clcb
**
*******************************************************************************/
tBTA_GATTC_CLCB *bta_gattc_find_int_disconn_clcb(tBTA_GATTC_DATA *p_msg)
{
    tBTA_GATTC_CLCB         *p_clcb = NULL;

    bta_gattc_conn_dealloc(p_msg->int_conn.remote_bda);
    if ((p_clcb = bta_gattc_find_clcb_by_conn_id(p_msg->int_conn.hdr.layer_specific)) == NULL) {
        /* connection attempt failed, send connection callback event */
        p_clcb = bta_gattc_find_clcb_by_cif(p_msg->int_conn.client_if,
                                            p_msg->int_conn.remote_bda,
                                            p_msg->int_conn.transport);
    }
    if (p_clcb == NULL) {
        APPL_TRACE_DEBUG(" disconnection ID: [%d] not used by BTA",
                         p_msg->int_conn.hdr.layer_specific);
    }
    return p_clcb;
}

void bta_to_btif_uuid(bt_uuid_t *p_dest, tBT_UUID *p_src)
{
    int i = 0;

    if (p_src->len == LEN_UUID_16 || p_src->len == LEN_UUID_32)
    {
        for(i=0; i != 16; ++i) {
            p_dest->uu[i] = base_uuid[i];
        }
    }

    switch (p_src->len)
    {
        case 0:
            break;

        case LEN_UUID_16:
            p_dest->uu[12] = p_src->uu.uuid16 & 0xff;
            p_dest->uu[13] = (p_src->uu.uuid16 >> 8) & 0xff;
            break;

        case LEN_UUID_32:
            p_dest->uu[12] = p_src->uu.uuid16 & 0xff;
            p_dest->uu[13] = (p_src->uu.uuid16 >> 8) & 0xff;
            p_dest->uu[14] = (p_src->uu.uuid32 >> 16) & 0xff;
            p_dest->uu[15] = (p_src->uu.uuid32 >> 24) & 0xff;
            break;

        case LEN_UUID_128:
            for(i=0; i != 16; ++i)
                p_dest->uu[i] = p_src->uu.uuid128[i];
            break;

        default:
            APPL_TRACE_ERROR("%s: Unknown UUID length %d!", __FUNCTION__, p_src->len);
            break;
    }
}


#endif /* BTA_GATT_INCLUDED */
