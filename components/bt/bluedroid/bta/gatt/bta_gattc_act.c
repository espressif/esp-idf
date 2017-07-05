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
 *  This file contains the GATT client action functions for the state
 *  machine.
 *
 ******************************************************************************/
#define LOG_TAG "bt_bta_gattc"

#include "bt_target.h"

#include "utl.h"
#include "gki.h"
#include "bta_sys.h"

#include "bta_gattc_int.h"
#include "l2c_api.h"
#include "l2c_int.h"
#include "gatt_int.h"

#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
#include "bta_hh_int.h"
#endif

// #include "btif/include/btif_debug_conn.h"

#include <string.h>

// #include "osi/include/log.h"

#if GATTC_INCLUDED == TRUE && BLE_INCLUDED == TRUE

/*****************************************************************************
**  Constants
*****************************************************************************/
static void bta_gattc_conn_cback(tGATT_IF gattc_if, BD_ADDR bda, UINT16 conn_id,
                                 BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                 tBT_TRANSPORT transport);

static void  bta_gattc_cmpl_cback(UINT16 conn_id, tGATTC_OPTYPE op, tGATT_STATUS status,
                                  tGATT_CL_COMPLETE *p_data);
static void bta_gattc_cmpl_sendmsg(UINT16 conn_id, tGATTC_OPTYPE op,
                                   tBTA_GATT_STATUS status,
                                   tGATT_CL_COMPLETE *p_data);

static void bta_gattc_deregister_cmpl(tBTA_GATTC_RCB *p_clreg);
static void bta_gattc_enc_cmpl_cback(tGATT_IF gattc_if, BD_ADDR bda);
static void bta_gattc_cong_cback (UINT16 conn_id, BOOLEAN congested);
static tBTA_GATTC_FIND_SERVICE_CB bta_gattc_register_service_change_notify(UINT16 conn_id, BD_ADDR remote_bda, BOOLEAN *need_timer);
static void bta_gattc_wait4_service_change_ccc_cback (TIMER_LIST_ENT *p_tle);
static void bta_gattc_start_service_change_ccc_timer(UINT16 conn_id, BD_ADDR bda,UINT32 timeout_ms,
                                              UINT8 timer_cnt, UINT8 last_status, TIMER_LIST_ENT *ccc_timer);

static tGATT_CBACK bta_gattc_cl_cback = {
    bta_gattc_conn_cback,
    bta_gattc_cmpl_cback,
    bta_gattc_disc_res_cback,
    bta_gattc_disc_cmpl_cback,
    NULL,
    bta_gattc_enc_cmpl_cback,
    bta_gattc_cong_cback
};

/* opcode(tGATTC_OPTYPE) order has to be comply with internal event order */
static UINT16 bta_gattc_opcode_to_int_evt[] = {
    BTA_GATTC_API_READ_EVT,
    BTA_GATTC_API_WRITE_EVT,
    BTA_GATTC_API_EXEC_EVT,
    BTA_GATTC_API_CFG_MTU_EVT
};

#if (BT_TRACE_VERBOSE == TRUE)
static const char *bta_gattc_op_code_name[] = {
    "Unknown",
    "Discovery",
    "Read",
    "Write",
    "Exec",
    "Config",
    "Notification",
    "Indication"
};
#endif
/*****************************************************************************
**  Action Functions
*****************************************************************************/

/*******************************************************************************
**
** Function         bta_gattc_enable
**
** Description      Enables GATTC module
**
**
** Returns          void
**
*******************************************************************************/
static void bta_gattc_enable(tBTA_GATTC_CB *p_cb)
{
    APPL_TRACE_DEBUG("bta_gattc_enable");

    if (p_cb->state == BTA_GATTC_STATE_DISABLED) {
        /* initialize control block */
        memset(&bta_gattc_cb, 0, sizeof(tBTA_GATTC_CB));
        p_cb->state = BTA_GATTC_STATE_ENABLED;
    } else {
        APPL_TRACE_DEBUG("GATTC is arelady enabled");
    }
}


/*******************************************************************************
**
** Function         bta_gattc_disable
**
** Description      Disable GATTC module by cleaning up all active connections
**                  and deregister all application.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_disable(tBTA_GATTC_CB *p_cb)
{
    UINT8           i;

    APPL_TRACE_DEBUG("bta_gattc_disable");

    if (p_cb->state != BTA_GATTC_STATE_ENABLED) {
        APPL_TRACE_ERROR("not enabled or disable in pogress");
        return;
    }

    for (i = 0; i < BTA_GATTC_CL_MAX; i ++) {
        if (p_cb->cl_rcb[i].in_use) {
            p_cb->state = BTA_GATTC_STATE_DISABLING;
            /* don't deregister HH GATT IF */
            /* HH GATT IF will be deregistered by bta_hh_le_deregister when disable HH */
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
            if (!bta_hh_le_is_hh_gatt_if(p_cb->cl_rcb[i].client_if)) {
#endif
                bta_gattc_deregister(p_cb, &p_cb->cl_rcb[i]);
#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
            }
#endif
        }
    }

    /* no registered apps, indicate disable completed */
    if (p_cb->state != BTA_GATTC_STATE_DISABLING) {
        p_cb->state = BTA_GATTC_STATE_DISABLED;
        memset(p_cb, 0, sizeof(tBTA_GATTC_CB));
    }
}

/*******************************************************************************
**
** Function         bta_gattc_register
**
** Description      Register a GATT client application with BTA.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_register(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC               cb_data;
    UINT8                    i;
    tBT_UUID                 *p_app_uuid = &p_data->api_reg.app_uuid;
    tBTA_GATTC_INT_START_IF  *p_buf;
    tBTA_GATT_STATUS         status = BTA_GATT_NO_RESOURCES;


    APPL_TRACE_DEBUG("bta_gattc_register state %d\n", p_cb->state);
    memset(&cb_data, 0, sizeof(cb_data));
    cb_data.reg_oper.status = BTA_GATT_NO_RESOURCES;

    /* check if  GATTC module is already enabled . Else enable */
    if (p_cb->state == BTA_GATTC_STATE_DISABLED) {
        bta_gattc_enable (p_cb);
    }
    /* todo need to check duplicate uuid */
    for (i = 0; i < BTA_GATTC_CL_MAX; i ++) {
        if (!p_cb->cl_rcb[i].in_use) {
            if ((p_app_uuid == NULL) || (p_cb->cl_rcb[i].client_if = GATT_Register(p_app_uuid, &bta_gattc_cl_cback)) == 0) {
                APPL_TRACE_ERROR("Register with GATT stack failed.\n");
                status = BTA_GATT_ERROR;
            } else {
                p_cb->cl_rcb[i].in_use = TRUE;
                p_cb->cl_rcb[i].p_cback = p_data->api_reg.p_cback;
                memcpy(&p_cb->cl_rcb[i].app_uuid, p_app_uuid, sizeof(tBT_UUID));

                /* BTA use the same client interface as BTE GATT statck */
                cb_data.reg_oper.client_if = p_cb->cl_rcb[i].client_if;

                if ((p_buf = (tBTA_GATTC_INT_START_IF *) GKI_getbuf(sizeof(tBTA_GATTC_INT_START_IF))) != NULL) {
                    p_buf->hdr.event    = BTA_GATTC_INT_START_IF_EVT;
                    p_buf->client_if    = p_cb->cl_rcb[i].client_if;
                    APPL_TRACE_DEBUG("GATTC getbuf sucess.\n");
                    bta_sys_sendmsg(p_buf);
                    status = BTA_GATT_OK;
                } else {
                    GATT_Deregister(p_cb->cl_rcb[i].client_if);

                    status = BTA_GATT_NO_RESOURCES;
                    memset( &p_cb->cl_rcb[i], 0 , sizeof(tBTA_GATTC_RCB));
                }
                break;
            }
        }
    }

    /* callback with register event */
    if (p_data->api_reg.p_cback) {
        if (p_app_uuid != NULL) {
            memcpy(&(cb_data.reg_oper.app_uuid), p_app_uuid, sizeof(tBT_UUID));
        }
        cb_data.reg_oper.status = status;
        (*p_data->api_reg.p_cback)(BTA_GATTC_REG_EVT,  (tBTA_GATTC *)&cb_data);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_start_if
**
** Description      start an application interface.
**
** Returns          none.
**
*******************************************************************************/
void bta_gattc_start_if(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    UNUSED(p_cb);

    if (bta_gattc_cl_get_regcb(p_msg->int_start_if.client_if) != NULL ) {
        GATT_StartIf(p_msg->int_start_if.client_if);
    } else {
        APPL_TRACE_ERROR("Unable to start app.: Unknown interface =%d", p_msg->int_start_if.client_if );
    }
}

/*******************************************************************************
**
** Function         bta_gattc_deregister
**
** Description      De-Register a GATT client application with BTA.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_deregister(tBTA_GATTC_CB *p_cb, tBTA_GATTC_RCB  *p_clreg)
{
    UINT8               i;
    BT_HDR              buf;

    if (p_clreg != NULL) {
        /* remove bg connection associated with this rcb */
        for (i = 0; i < BTA_GATTC_KNOWN_SR_MAX; i ++) {
            if (p_cb->bg_track[i].in_use) {
                if (p_cb->bg_track[i].cif_mask & (1 << (p_clreg->client_if - 1))) {
                    bta_gattc_mark_bg_conn(p_clreg->client_if, p_cb->bg_track[i].remote_bda, FALSE, FALSE);
                    GATT_CancelConnect(p_clreg->client_if, p_cb->bg_track[i].remote_bda, FALSE);
                }
                if (p_cb->bg_track[i].cif_adv_mask & (1 << (p_clreg->client_if - 1))) {
                    bta_gattc_mark_bg_conn(p_clreg->client_if, p_cb->bg_track[i].remote_bda, FALSE, TRUE);
                }
            }
        }

        if (p_clreg->num_clcb > 0) {
            /* close all CLCB related to this app */
            for (i = 0; i < BTA_GATTC_CLCB_MAX; i ++) {
                if (p_cb->clcb[i].in_use && (p_cb->clcb[i].p_rcb == p_clreg)) {
                    p_clreg->dereg_pending = TRUE;

                    buf.event = BTA_GATTC_API_CLOSE_EVT;
                    buf.layer_specific = p_cb->clcb[i].bta_conn_id;
                    bta_gattc_close(&p_cb->clcb[i], (tBTA_GATTC_DATA *)&buf)  ;
                }
            }
        } else {
            bta_gattc_deregister_cmpl(p_clreg);
        }
    } else {
        APPL_TRACE_ERROR("bta_gattc_deregister Deregister Failedm unknown client cif");
    }
}
/*******************************************************************************
**
** Function         bta_gattc_process_api_open
**
** Description      process connect API request.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_process_api_open (tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    UINT16 event = ((BT_HDR *)p_msg)->event;
    tBTA_GATTC_CLCB *p_clcb = NULL;
    tBTA_GATTC_RCB *p_clreg = bta_gattc_cl_get_regcb(p_msg->api_conn.client_if);
    UNUSED(p_cb);

    if (p_clreg != NULL) {
        if (p_msg->api_conn.is_direct) {
            if ((p_clcb = bta_gattc_find_alloc_clcb(p_msg->api_conn.client_if,
                                                    p_msg->api_conn.remote_bda,
                                                    p_msg->api_conn.transport)) != NULL) {
                bta_gattc_sm_execute(p_clcb, event, p_msg);
            } else {
                APPL_TRACE_ERROR("No resources to open a new connection.");

                bta_gattc_send_open_cback(p_clreg,
                                          BTA_GATT_NO_RESOURCES,
                                          p_msg->api_conn.remote_bda,
                                          BTA_GATT_INVALID_CONN_ID,
                                          p_msg->api_conn.transport, 0);
            }
        } else {
            bta_gattc_init_bk_conn(&p_msg->api_conn, p_clreg);
        }
    } else {
        APPL_TRACE_ERROR("bta_gattc_process_api_open Failed, unknown client_if: %d",
                         p_msg->api_conn.client_if);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_process_api_open_cancel
**
** Description      process connect API request.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_process_api_open_cancel (tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    UINT16 event = ((BT_HDR *)p_msg)->event;
    tBTA_GATTC_CLCB *p_clcb = NULL;
    tBTA_GATTC_RCB *p_clreg;
    tBTA_GATTC cb_data;
    UNUSED(p_cb);

    if (p_msg->api_cancel_conn.is_direct) {
        if ((p_clcb = bta_gattc_find_clcb_by_cif(p_msg->api_cancel_conn.client_if,
                      p_msg->api_cancel_conn.remote_bda,
                      BTA_GATT_TRANSPORT_LE)) != NULL) {
            bta_gattc_sm_execute(p_clcb, event, p_msg);
        } else {
            APPL_TRACE_ERROR("No such connection need to be cancelled");

            p_clreg = bta_gattc_cl_get_regcb(p_msg->api_cancel_conn.client_if);

            if (p_clreg && p_clreg->p_cback) {
                cb_data.status = BTA_GATT_ERROR;
                (*p_clreg->p_cback)(BTA_GATTC_CANCEL_OPEN_EVT, &cb_data);
            }
        }
    } else {
        bta_gattc_cancel_bk_conn(&p_msg->api_cancel_conn);

    }
}

/*******************************************************************************
**
** Function         bta_gattc_process_enc_cmpl
**
** Description      process encryption complete message.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_process_enc_cmpl(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    tBTA_GATTC_RCB *p_clreg;
    tBTA_GATTC cb_data;
    UNUSED(p_cb);

    p_clreg = bta_gattc_cl_get_regcb(p_msg->enc_cmpl.client_if);

    if (p_clreg && p_clreg->p_cback) {
        memset(&cb_data, 0, sizeof(tBTA_GATTC));

        cb_data.enc_cmpl.client_if = p_msg->enc_cmpl.client_if;
        bdcpy(cb_data.enc_cmpl.remote_bda, p_msg->enc_cmpl.remote_bda);

        (*p_clreg->p_cback)(BTA_GATTC_ENC_CMPL_CB_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_cancel_open_error
**
** Description
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_cancel_open_error(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC cb_data;
    UNUSED(p_data);

    cb_data.status = BTA_GATT_ERROR;

    if ( p_clcb && p_clcb->p_rcb && p_clcb->p_rcb->p_cback ) {
        (*p_clcb->p_rcb->p_cback)(BTA_GATTC_CANCEL_OPEN_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_open_error
**
** Description
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_open_error(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    APPL_TRACE_ERROR("Connection already opened. wrong state");

    bta_gattc_send_open_cback(p_clcb->p_rcb,
                              BTA_GATT_OK,
                              p_clcb->bda,
                              p_clcb->bta_conn_id,
                              p_clcb->transport,
                              0);
}
/*******************************************************************************
**
** Function         bta_gattc_open_fail
**
** Description
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_open_fail(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    bta_gattc_send_open_cback(p_clcb->p_rcb,
                              BTA_GATT_ERROR,
                              p_clcb->bda,
                              p_clcb->bta_conn_id,
                              p_clcb->transport,
                              0);
    /* open failure, remove clcb */
    bta_gattc_clcb_dealloc(p_clcb);
}

/*******************************************************************************
**
** Function         bta_gattc_open
**
** Description      Process API connection function.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC_DATA gattc_data;

    /* open/hold a connection */
    if (!GATT_Connect(p_clcb->p_rcb->client_if, p_data->api_conn.remote_bda,
                      TRUE, p_data->api_conn.transport)) {
        APPL_TRACE_ERROR("Connection open failure");

        bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_OPEN_FAIL_EVT, p_data);
    } else {
        /* a connected remote device */
        if (GATT_GetConnIdIfConnected(p_clcb->p_rcb->client_if,
                                      p_data->api_conn.remote_bda,
                                      &p_clcb->bta_conn_id,
                                      p_data->api_conn.transport)) {
            gattc_data.int_conn.hdr.layer_specific = p_clcb->bta_conn_id;

            bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_CONN_EVT, &gattc_data);
        }
        /* else wait for the callback event */
    }
}
/*******************************************************************************
**
** Function         bta_gattc_init_bk_conn
**
** Description      Process API Open for a background connection
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_init_bk_conn(tBTA_GATTC_API_OPEN *p_data, tBTA_GATTC_RCB *p_clreg)
{
    tBTA_GATT_STATUS         status = BTA_GATT_NO_RESOURCES;
    UINT16                   conn_id;
    tBTA_GATTC_CLCB         *p_clcb;
    tBTA_GATTC_DATA         gattc_data;

    if (bta_gattc_mark_bg_conn(p_data->client_if, p_data->remote_bda, TRUE, FALSE)) {
        /* always call open to hold a connection */
        if (!GATT_Connect(p_data->client_if, p_data->remote_bda, FALSE, p_data->transport)) {
            uint8_t *bda = (uint8_t *)p_data->remote_bda;
            status = BTA_GATT_ERROR;
            APPL_TRACE_ERROR("%s unable to connect to remote bd_addr:%02x:%02x:%02x:%02x:%02x:%02x",
                             __func__, bda[0], bda[1], bda[2], bda[3], bda[4], bda[5]);

        } else {
            status = BTA_GATT_OK;

            /* if is a connected remote device */
            if (GATT_GetConnIdIfConnected(p_data->client_if,
                                          p_data->remote_bda,
                                          &conn_id,
                                          p_data->transport)) {
                if ((p_clcb = bta_gattc_find_alloc_clcb(p_data->client_if, p_data->remote_bda,
                                                        BTA_GATT_TRANSPORT_LE)) != NULL) {
                    gattc_data.hdr.layer_specific = p_clcb->bta_conn_id = conn_id;

                    /* open connection */
                    bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_CONN_EVT, &gattc_data);
                    status = BTA_GATT_OK;
                }
            }
        }
    }

    /* open failure, report OPEN_EVT */
    if (status != BTA_GATT_OK) {
        bta_gattc_send_open_cback(p_clreg, status, p_data->remote_bda,
                                  BTA_GATT_INVALID_CONN_ID, BTA_GATT_TRANSPORT_LE, 0);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_cancel_bk_conn
**
** Description      Process API Cancel Open for a background connection
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_cancel_bk_conn(tBTA_GATTC_API_CANCEL_OPEN *p_data)
{
    tBTA_GATTC_RCB      *p_clreg;
    tBTA_GATTC          cb_data;
    cb_data.status = BTA_GATT_ERROR;

    /* remove the device from the bg connection mask */
    if (bta_gattc_mark_bg_conn(p_data->client_if, p_data->remote_bda, FALSE, FALSE)) {
        if (GATT_CancelConnect(p_data->client_if, p_data->remote_bda, FALSE)) {
            cb_data.status = BTA_GATT_OK;
        } else {
            APPL_TRACE_ERROR("bta_gattc_cancel_bk_conn failed");
        }
    }
    p_clreg = bta_gattc_cl_get_regcb(p_data->client_if);

    if (p_clreg && p_clreg->p_cback) {
        (*p_clreg->p_cback)(BTA_GATTC_CANCEL_OPEN_EVT, &cb_data);
    }

}
/*******************************************************************************
**
** Function         bta_gattc_int_cancel_open_ok
**
** Description
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_cancel_open_ok(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC          cb_data;
    UNUSED(p_data);

    if ( p_clcb->p_rcb->p_cback ) {
        cb_data.status = BTA_GATT_OK;
        (*p_clcb->p_rcb->p_cback)(BTA_GATTC_CANCEL_OPEN_EVT, &cb_data);
    }

    bta_gattc_clcb_dealloc(p_clcb);
}
/*******************************************************************************
**
** Function         bta_gattc_cancel_open
**
** Description
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_cancel_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC          cb_data;

    if (GATT_CancelConnect(p_clcb->p_rcb->client_if, p_data->api_cancel_conn.remote_bda, TRUE)) {
        bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_CANCEL_OPEN_OK_EVT, p_data);
    } else {
        if ( p_clcb->p_rcb->p_cback ) {
            cb_data.status = BTA_GATT_ERROR;
            (*p_clcb->p_rcb->p_cback)(BTA_GATTC_CANCEL_OPEN_EVT, &cb_data);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_conn
**
** Description      receive connection callback from stack
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_conn(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC_IF   gatt_if;
    APPL_TRACE_DEBUG("bta_gattc_conn server cache state=%d", p_clcb->p_srcb->state);

    if (p_data != NULL) {
        APPL_TRACE_DEBUG("bta_gattc_conn conn_id=%d", p_data->hdr.layer_specific);
        p_clcb->bta_conn_id  = p_data->int_conn.hdr.layer_specific;

        GATT_GetConnectionInfor(p_data->hdr.layer_specific,
                                &gatt_if, p_clcb->bda, &p_clcb->transport);
    }

    p_clcb->p_srcb->connected = TRUE;

    if (p_clcb->p_srcb->mtu == 0) {
        p_clcb->p_srcb->mtu = GATT_DEF_BLE_MTU_SIZE;
    }

    /* start database cache if needed */
    if (p_clcb->p_srcb->p_srvc_cache == NULL ||
            p_clcb->p_srcb->state != BTA_GATTC_SERV_IDLE) {
        if (p_clcb->p_srcb->state == BTA_GATTC_SERV_IDLE) {
            p_clcb->p_srcb->state = BTA_GATTC_SERV_LOAD;
            bta_gattc_sm_execute(p_clcb, BTA_GATTC_START_CACHE_EVT, NULL);
        } else { /* cache is building */
            p_clcb->state = BTA_GATTC_DISCOVER_ST;
        }
    }

    else {
        /* a pending service handle change indication */
        if (p_clcb->p_srcb->srvc_hdl_chg) {
            p_clcb->p_srcb->srvc_hdl_chg = FALSE;
            /* start discovery */
            bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_DISCOVER_EVT, NULL);
        }
    }

    if (p_clcb->p_rcb) {
        /* there is no RM for GATT */
        if (p_clcb->transport == BTA_TRANSPORT_BR_EDR) {
            bta_sys_conn_open(BTA_ID_GATTC, BTA_ALL_APP_ID, p_clcb->bda);
        }

        bta_gattc_send_open_cback(p_clcb->p_rcb,
                                  BTA_GATT_OK,
                                  p_clcb->bda,
                                  p_clcb->bta_conn_id,
                                  p_clcb->transport,
                                  p_clcb->p_srcb->mtu);

    }
}
/*******************************************************************************
**
** Function         bta_gattc_conncback
**
** Description      receive connection callback from stack
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_conncback(tBTA_GATTC_RCB *p_rcb, tBTA_GATTC_DATA *p_data)
{
    if (p_rcb) {
        bta_gattc_send_connect_cback(p_rcb,
                                     BTA_GATT_OK,
                                     p_data->int_conn.remote_bda,
                                     p_data->int_conn.hdr.layer_specific);

    }
}
/*******************************************************************************
**
** Function         bta_gattc_disconncback
**
** Description      receive disconnection callback from stack
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_disconncback(tBTA_GATTC_RCB *p_rcb, tBTA_GATTC_DATA *p_data)
{
    if (p_rcb) {
        bta_gattc_send_disconnect_cback(p_rcb,
                                     BTA_GATT_OK,
                                     p_data->int_conn.remote_bda,
                                     p_data->int_conn.hdr.layer_specific);

    }
}
/*******************************************************************************
**
** Function         bta_gattc_close_fail
**
** Description      close a  connection.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_close_fail(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC           cb_data;

    if ( p_clcb->p_rcb->p_cback ) {
        memset(&cb_data, 0, sizeof(tBTA_GATTC));
        cb_data.close.client_if = p_clcb->p_rcb->client_if;
        cb_data.close.conn_id   = p_data->hdr.layer_specific;
        bdcpy(cb_data.close.remote_bda, p_clcb->bda);
        cb_data.close.status    = BTA_GATT_ERROR;
        cb_data.close.reason    = BTA_GATT_CONN_NONE;


        (*p_clcb->p_rcb->p_cback)(BTA_GATTC_CLOSE_EVT, &cb_data);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_api_close
**
** Description      close a GATTC connection.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_close(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC_CBACK    *p_cback = p_clcb->p_rcb->p_cback;
    tBTA_GATTC_RCB      *p_clreg = p_clcb->p_rcb;
    tBTA_GATTC           cb_data;

    APPL_TRACE_DEBUG("bta_gattc_close conn_id=%d", p_clcb->bta_conn_id);

    cb_data.close.client_if = p_clcb->p_rcb->client_if;
    cb_data.close.conn_id   = p_clcb->bta_conn_id;
    cb_data.close.reason    = p_clcb->reason;
    cb_data.close.status    = p_clcb->status;
    bdcpy(cb_data.close.remote_bda, p_clcb->bda);

    if (p_clcb->transport == BTA_TRANSPORT_BR_EDR) {
        bta_sys_conn_close( BTA_ID_GATTC , BTA_ALL_APP_ID, p_clcb->bda);
    }

    bta_gattc_clcb_dealloc(p_clcb);

    if (p_data->hdr.event == BTA_GATTC_API_CLOSE_EVT) {
        cb_data.close.status = GATT_Disconnect(p_data->hdr.layer_specific);
    } else if (p_data->hdr.event == BTA_GATTC_INT_DISCONN_EVT) {
        cb_data.close.status = p_data->int_conn.reason;
        cb_data.close.reason = p_data->int_conn.reason;
    }

    if (p_cback) {
        (* p_cback)(BTA_GATTC_CLOSE_EVT,   (tBTA_GATTC *)&cb_data);
    }

    if (p_clreg->num_clcb == 0 && p_clreg->dereg_pending) {
        bta_gattc_deregister_cmpl(p_clreg);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_reset_discover_st
**
** Description      when a SRCB finished discovery, tell all related clcb.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_reset_discover_st(tBTA_GATTC_SERV *p_srcb, tBTA_GATT_STATUS status)
{
    tBTA_GATTC_CB   *p_cb = &bta_gattc_cb;
    UINT8 i;

    for (i = 0; i < BTA_GATTC_CLCB_MAX; i ++) {
        if (p_cb->clcb[i].p_srcb == p_srcb) {
            p_cb->clcb[i].status = status;
            bta_gattc_sm_execute(&p_cb->clcb[i], BTA_GATTC_DISCOVER_CMPL_EVT, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_disc_close
**
** Description      close a GATTC connection while in discovery state.
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_disc_close(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    APPL_TRACE_DEBUG("%s: Discovery cancel conn_id=%d", __func__,
                     p_clcb->bta_conn_id);

    if (p_clcb->disc_active) {
        bta_gattc_reset_discover_st(p_clcb->p_srcb, BTA_GATT_ERROR);
    } else {
        p_clcb->state = BTA_GATTC_CONN_ST;
    }

    // This function only gets called as the result of a BTA_GATTC_API_CLOSE_EVT
    // while in the BTA_GATTC_DISCOVER_ST state. Once the state changes, the
    // connection itself still needs to be closed to resolve the original event.
    if (p_clcb->state == BTA_GATTC_CONN_ST) {
        APPL_TRACE_DEBUG("State is back to BTA_GATTC_CONN_ST. "
                         "Trigger connection close");
        bta_gattc_close(p_clcb, p_data);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_set_discover_st
**
** Description      when a SRCB start discovery, tell all related clcb and set
**                  the state.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_set_discover_st(tBTA_GATTC_SERV *p_srcb)
{
    tBTA_GATTC_CB   *p_cb = &bta_gattc_cb;
    UINT8   i;

#if BLE_INCLUDED == TRUE
    L2CA_EnableUpdateBleConnParams(p_srcb->server_bda, FALSE);
#endif
    for (i = 0; i < BTA_GATTC_CLCB_MAX; i ++) {
        if (p_cb->clcb[i].p_srcb == p_srcb) {
            p_cb->clcb[i].status = BTA_GATT_OK;
            p_cb->clcb[i].state = BTA_GATTC_DISCOVER_ST;
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_restart_discover
**
** Description      process service change in discovery state, mark up the auto
**                  update flag and set status to be discovery cancel for current
**                  discovery.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_restart_discover(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    p_clcb->status      = BTA_GATT_CANCEL;
    p_clcb->auto_update = BTA_GATTC_DISC_WAITING;
}

/*******************************************************************************
**
** Function         bta_gattc_cfg_mtu
**
** Description      Configure MTU size on the GATT connection.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_cfg_mtu(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATT_STATUS    status;

    if (bta_gattc_enqueue(p_clcb, p_data)) {
        status = GATTC_ConfigureMTU (p_clcb->bta_conn_id, p_data->api_mtu.mtu);

        /* if failed, return callback here */
        if (status != GATT_SUCCESS && status != GATT_CMD_STARTED) {
            /* Dequeue the data, if it was enqueued */
            if (p_clcb->p_q_cmd == p_data) {
                p_clcb->p_q_cmd = NULL;
            }

            bta_gattc_cmpl_sendmsg(p_clcb->bta_conn_id, GATTC_OPTYPE_CONFIG, status, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_start_discover
**
** Description      Start a discovery send to server.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_start_discover(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_gattc_start_discover conn_id=%d p_clcb->p_srcb->state = %d ",
                     p_clcb->bta_conn_id, p_clcb->p_srcb->state);

    if (((p_clcb->p_q_cmd == NULL || p_clcb->auto_update == BTA_GATTC_REQ_WAITING) &&
            p_clcb->p_srcb->state == BTA_GATTC_SERV_IDLE) ||
            p_clcb->p_srcb->state == BTA_GATTC_SERV_DISC)
        /* no pending operation, start discovery right away */
    {
        p_clcb->auto_update = BTA_GATTC_NO_SCHEDULE;

        if (p_clcb->p_srcb != NULL) {
            /* clear the service change mask */
            p_clcb->p_srcb->srvc_hdl_chg = FALSE;
            p_clcb->p_srcb->update_count = 0;
            p_clcb->p_srcb->state = BTA_GATTC_SERV_DISC_ACT;

            if (p_clcb->transport == BTA_TRANSPORT_LE) {
                L2CA_EnableUpdateBleConnParams(p_clcb->p_srcb->server_bda, FALSE);
            }

            /* set all srcb related clcb into discovery ST */
            bta_gattc_set_discover_st(p_clcb->p_srcb);

            if ((p_clcb->status = bta_gattc_init_cache(p_clcb->p_srcb)) == BTA_GATT_OK) {
                p_clcb->status = bta_gattc_discover_pri_service(p_clcb->bta_conn_id,
                                 p_clcb->p_srcb, GATT_DISC_SRVC_ALL);
            }
            if (p_clcb->status != BTA_GATT_OK) {
                APPL_TRACE_ERROR("discovery on server failed");
                bta_gattc_reset_discover_st(p_clcb->p_srcb, p_clcb->status);
            } else {
                p_clcb->disc_active = TRUE;
            }
        } else {
            APPL_TRACE_ERROR("unknown device, can not start discovery");
        }
    }
    /* pending operation, wait until it finishes */
    else {
        p_clcb->auto_update = BTA_GATTC_DISC_WAITING;

        if (p_clcb->p_srcb->state == BTA_GATTC_SERV_IDLE) {
            p_clcb->state = BTA_GATTC_CONN_ST;    /* set clcb state */
        }
    }

}
/*******************************************************************************
**
** Function         bta_gattc_disc_cmpl
**
** Description      discovery on server is finished
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_disc_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATTC_DATA *p_q_cmd = p_clcb->p_q_cmd;
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_gattc_disc_cmpl conn_id=%d", p_clcb->bta_conn_id);

#if BLE_INCLUDED == TRUE
    if (p_clcb->transport == BTA_TRANSPORT_LE) {
        L2CA_EnableUpdateBleConnParams(p_clcb->p_srcb->server_bda, TRUE);
    }
#endif
    p_clcb->p_srcb->state = BTA_GATTC_SERV_IDLE;
    p_clcb->disc_active = FALSE;

    if (p_clcb->status != GATT_SUCCESS) {
        /* clean up cache */
        if (p_clcb->p_srcb && p_clcb->p_srcb->p_srvc_cache) {
            while (!GKI_queue_is_empty(&p_clcb->p_srcb->cache_buffer)) {
                GKI_freebuf (GKI_dequeue (&p_clcb->p_srcb->cache_buffer));
            }
            p_clcb->p_srcb->p_srvc_cache = NULL;
        }

        /* used to reset cache in application */
        bta_gattc_co_cache_reset(p_clcb->p_srcb->server_bda);
    }
    /* release pending attribute list buffer */
    utl_freebuf((void **)&p_clcb->p_srcb->p_srvc_list);

    if (p_clcb->auto_update == BTA_GATTC_DISC_WAITING) {
        /* start discovery again */
        p_clcb->auto_update = BTA_GATTC_REQ_WAITING;
        bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_DISCOVER_EVT, NULL);
    }
    /* get any queued command to proceed */
    else if (p_q_cmd != NULL) {
        p_clcb->p_q_cmd = NULL;
         /* execute pending operation of link block still present */
        if (l2cu_find_lcb_by_bd_addr(p_clcb->p_srcb->server_bda,
            BT_TRANSPORT_LE) != NULL) {
            bta_gattc_sm_execute(p_clcb, p_q_cmd->hdr.event, p_q_cmd);
        }
        /* if the command executed requeued the cmd, we don't
         * want to free the underlying buffer that's being
         * referenced by p_clcb->p_q_cmd
         */
        if (p_q_cmd != p_clcb->p_q_cmd) {
            utl_freebuf((void **)&p_q_cmd);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_read
**
** Description      Read an attribute
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_read(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UINT16 handle = 0;
    tGATT_READ_PARAM    read_param;
    tBTA_GATT_STATUS    status;

    memset (&read_param, 0 , sizeof(tGATT_READ_PARAM));

    if (bta_gattc_enqueue(p_clcb, p_data)) {
        if ((handle = bta_gattc_id2handle(p_clcb->p_srcb,
                                          &p_data->api_read.srvc_id,
                                          &p_data->api_read.char_id,
                                          p_data->api_read.p_descr_type)) == 0) {
            status = BTA_GATT_ERROR;
        } else {
            read_param.by_handle.handle = handle;
            read_param.by_handle.auth_req = p_data->api_read.auth_req;

            status = GATTC_Read(p_clcb->bta_conn_id, GATT_READ_BY_HANDLE, &read_param);
        }

        /* read fail */
        if (status != BTA_GATT_OK) {
            /* Dequeue the data, if it was enqueued */
            if (p_clcb->p_q_cmd == p_data) {
                p_clcb->p_q_cmd = NULL;
            }

            bta_gattc_cmpl_sendmsg(p_clcb->bta_conn_id, GATTC_OPTYPE_READ, status, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_read_multi
**
** Description      read multiple
**
** Returns          None.
*********************************************************************************/
void bta_gattc_read_multi(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UINT16              i, handle;
    tBTA_GATT_STATUS    status = BTA_GATT_OK;
    tGATT_READ_PARAM    read_param;
    tBTA_GATTC_ATTR_ID  *p_id;

    if (bta_gattc_enqueue(p_clcb, p_data)) {
        memset(&read_param, 0, sizeof(tGATT_READ_PARAM));

        p_id = p_data->api_read_multi.p_id_list;

        for (i = 0; i < p_data->api_read_multi.num_attr && p_id; i ++, p_id ++) {
            handle = 0;

            if (p_id->id_type == BTA_GATT_TYPE_CHAR) {
                handle = bta_gattc_id2handle(p_clcb->p_srcb,
                                             &p_id->id_value.char_id.srvc_id,
                                             &p_id->id_value.char_id.char_id,
                                             NULL);
            } else if (p_id->id_type == BTA_GATT_TYPE_CHAR_DESCR) {
                handle = bta_gattc_id2handle(p_clcb->p_srcb,
                                             &p_id->id_value.char_descr_id.char_id.srvc_id,
                                             &p_id->id_value.char_descr_id.char_id.char_id,
                                             &p_id->id_value.char_descr_id.descr_id);
            } else {
                APPL_TRACE_ERROR("invalud ID type: %d", p_id->id_type);
            }

            if (handle == 0) {
                status = BTA_GATT_ERROR;
                break;
            }
        }
        if (status == BTA_GATT_OK) {
            read_param.read_multiple.num_handles = p_data->api_read_multi.num_attr;
            read_param.read_multiple.auth_req = p_data->api_read_multi.auth_req;

            status = GATTC_Read(p_clcb->bta_conn_id, GATT_READ_MULTIPLE, &read_param);
        }

        /* read fail */
        if (status != BTA_GATT_OK) {
            /* Dequeue the data, if it was enqueued */
            if (p_clcb->p_q_cmd == p_data) {
                p_clcb->p_q_cmd = NULL;
            }

            bta_gattc_cmpl_sendmsg(p_clcb->bta_conn_id, GATTC_OPTYPE_READ, status, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_write
**
** Description      Write an attribute
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_write(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UINT16              handle = 0;
    tGATT_VALUE         attr = {0};
    tBTA_GATT_STATUS    status = BTA_GATT_OK;

    if (bta_gattc_enqueue(p_clcb, p_data)) {
        if ((handle = bta_gattc_id2handle(p_clcb->p_srcb,
                                          &p_data->api_write.srvc_id,
                                          &p_data->api_write.char_id,
                                          p_data->api_write.p_descr_type)) == 0) {
            status = BTA_GATT_ERROR;
        } else {
            attr.handle = handle;
            attr.offset = p_data->api_write.offset;
            attr.len    = p_data->api_write.len;
            attr.auth_req = p_data->api_write.auth_req;

            if (p_data->api_write.p_value) {
                memcpy(attr.value, p_data->api_write.p_value, p_data->api_write.len);
            }

            status = GATTC_Write(p_clcb->bta_conn_id, p_data->api_write.write_type, &attr);
        }

        /* write fail */
        if (status != BTA_GATT_OK) {
            /* Dequeue the data, if it was enqueued */
            if (p_clcb->p_q_cmd == p_data) {
                p_clcb->p_q_cmd = NULL;
            }

            bta_gattc_cmpl_sendmsg(p_clcb->bta_conn_id, GATTC_OPTYPE_WRITE, status, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_execute
**
** Description      send execute write
**
** Returns          None.
*********************************************************************************/
void bta_gattc_execute(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATT_STATUS    status;

    if (bta_gattc_enqueue(p_clcb, p_data)) {
        status = GATTC_ExecuteWrite(p_clcb->bta_conn_id, p_data->api_exec.is_execute);

        if (status != BTA_GATT_OK) {
            /* Dequeue the data, if it was enqueued */
            if (p_clcb->p_q_cmd == p_data) {
                p_clcb->p_q_cmd = NULL;
            }

            bta_gattc_cmpl_sendmsg(p_clcb->bta_conn_id, GATTC_OPTYPE_EXE_WRITE, status, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_confirm
**
** Description      send handle value confirmation
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_confirm(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UINT16 handle;

    if ((handle = bta_gattc_id2handle(p_clcb->p_srcb,
                                      &p_data->api_confirm.srvc_id,
                                      &p_data->api_confirm.char_id,
                                      NULL)) == 0) {
        APPL_TRACE_ERROR("Can not map service/char ID into valid handle");
    } else {
        if (GATTC_SendHandleValueConfirm(p_data->api_confirm.hdr.layer_specific, handle)
                != GATT_SUCCESS) {
            APPL_TRACE_ERROR("bta_gattc_confirm to handle [0x%04x] failed", handle);
        } else {
            /* if over BR_EDR, inform PM for mode change */
            if (p_clcb->transport == BTA_TRANSPORT_BR_EDR) {
                bta_sys_busy(BTA_ID_GATTC, BTA_ALL_APP_ID, p_clcb->bda);
                bta_sys_idle(BTA_ID_GATTC, BTA_ALL_APP_ID, p_clcb->bda);
            }
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_read_cmpl
**
** Description      read complete
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_read_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_OP_CMPL *p_data)
{
    UINT8               event;
    tBTA_GATTC          cb_data;
    tBTA_GATT_READ_VAL  read_value;

    memset(&cb_data, 0, sizeof(tBTA_GATTC));
    memset(&read_value, 0, sizeof(tBTA_GATT_READ_VAL));

    cb_data.read.status     = p_data->status;

    if (p_data->p_cmpl != NULL && p_data->status == BTA_GATT_OK) {
        if (bta_gattc_handle2id(p_clcb->p_srcb,
                                p_data->p_cmpl->att_value.handle,
                                &cb_data.read.srvc_id,
                                &cb_data.read.char_id,
                                &cb_data.read.descr_type) == FALSE) {
            cb_data.read.status = BTA_GATT_INTERNAL_ERROR;
            APPL_TRACE_ERROR("can not map to GATT ID. handle = 0x%04x",
                             p_data->p_cmpl->att_value.handle);
        } else {
            cb_data.read.status = bta_gattc_pack_read_cb_data(p_clcb->p_srcb,
                                  &cb_data.read.descr_type.uuid,
                                  &p_data->p_cmpl->att_value,
                                  &read_value);
            cb_data.read.p_value = &read_value;
        }
    } else {
        cb_data.read.srvc_id = p_clcb->p_q_cmd->api_read.srvc_id;
        cb_data.read.char_id = p_clcb->p_q_cmd->api_read.char_id;
        if (p_clcb->p_q_cmd->api_read.p_descr_type)
            memcpy(&cb_data.read.descr_type, p_clcb->p_q_cmd->api_read.p_descr_type,
                   sizeof(tBTA_GATT_ID));
    }

    event = (p_clcb->p_q_cmd->api_read.p_descr_type == NULL) ?
            BTA_GATTC_READ_CHAR_EVT : BTA_GATTC_READ_DESCR_EVT;
    cb_data.read.conn_id = p_clcb->bta_conn_id;

    utl_freebuf((void **)&p_clcb->p_q_cmd);
    /* read complete, callback */
    ( *p_clcb->p_rcb->p_cback)(event, (tBTA_GATTC *)&cb_data);

}
/*******************************************************************************
**
** Function         bta_gattc_write_cmpl
**
** Description      write complete
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_write_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_OP_CMPL *p_data)
{
    tBTA_GATTC      cb_data = {0};
    UINT8          event;

    memset(&cb_data, 0, sizeof(tBTA_GATTC));

    cb_data.write.status     = p_data->status;

    if (p_data->p_cmpl != NULL) {
        bta_gattc_handle2id(p_clcb->p_srcb, p_data->p_cmpl->att_value.handle,
                            &cb_data.write.srvc_id, &cb_data.write.char_id,
                            &cb_data.write.descr_type);
    } else {
        memcpy(&cb_data.write.srvc_id, &p_clcb->p_q_cmd->api_write.srvc_id,
               sizeof(tBTA_GATT_SRVC_ID));
        memcpy(&cb_data.write.char_id, &p_clcb->p_q_cmd->api_write.char_id,
               sizeof(tBTA_GATT_ID));
        if (p_clcb->p_q_cmd->api_write.p_descr_type)
            memcpy(&cb_data.write.descr_type, p_clcb->p_q_cmd->api_write.p_descr_type,
                   sizeof(tBTA_GATT_ID));
    }

    if (p_clcb->p_q_cmd->api_write.hdr.event == BTA_GATTC_API_WRITE_EVT &&
            p_clcb->p_q_cmd->api_write.write_type == BTA_GATTC_WRITE_PREPARE)

    {
        event = BTA_GATTC_PREP_WRITE_EVT;
    }

    else if (p_clcb->p_q_cmd->api_write.p_descr_type == NULL)

    {
        event = BTA_GATTC_WRITE_CHAR_EVT;
    }

    else {
        event = BTA_GATTC_WRITE_DESCR_EVT;
    }

    utl_freebuf((void **)&p_clcb->p_q_cmd);
    cb_data.write.conn_id = p_clcb->bta_conn_id;
    /* write complete, callback */
    ( *p_clcb->p_rcb->p_cback)(event, (tBTA_GATTC *)&cb_data);

}
/*******************************************************************************
**
** Function         bta_gattc_exec_cmpl
**
** Description      execute write complete
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_exec_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_OP_CMPL *p_data)
{
    tBTA_GATTC          cb_data;

    utl_freebuf((void **)&p_clcb->p_q_cmd);

    p_clcb->status      = BTA_GATT_OK;

    /* execute complete, callback */
    cb_data.exec_cmpl.conn_id = p_clcb->bta_conn_id;
    cb_data.exec_cmpl.status = p_data->status;

    ( *p_clcb->p_rcb->p_cback)(BTA_GATTC_EXEC_EVT,  &cb_data);

}

/*******************************************************************************
**
** Function         bta_gattc_cfg_mtu_cmpl
**
** Description      configure MTU operation complete
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_cfg_mtu_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_OP_CMPL *p_data)
{
    tBTA_GATTC          cb_data;

    utl_freebuf((void **)&p_clcb->p_q_cmd);


    if (p_data->p_cmpl  &&  p_data->status == BTA_GATT_OK) {
        p_clcb->p_srcb->mtu  = p_data->p_cmpl->mtu;
    }

    /* configure MTU complete, callback */
    p_clcb->status          = p_data->status;
    cb_data.cfg_mtu.conn_id = p_clcb->bta_conn_id;
    cb_data.cfg_mtu.status  = p_data->status;
    cb_data.cfg_mtu.mtu     = p_clcb->p_srcb->mtu;

    (*p_clcb->p_rcb->p_cback) (BTA_GATTC_CFG_MTU_EVT,  &cb_data);

}
/*******************************************************************************
**
** Function         bta_gattc_op_cmpl
**
** Description      operation completed.
**
** Returns          None.
**
*******************************************************************************/
void  bta_gattc_op_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UINT8           op = (UINT8)p_data->op_cmpl.op_code;
    UINT8           mapped_op = 0;

    APPL_TRACE_DEBUG("bta_gattc_op_cmpl op = %d", op);

    if (op == GATTC_OPTYPE_INDICATION || op == GATTC_OPTYPE_NOTIFICATION) {
        APPL_TRACE_ERROR("unexpected operation, ignored");
    } else if (op >= GATTC_OPTYPE_READ) {
        if (p_clcb->p_q_cmd == NULL) {
            APPL_TRACE_ERROR("No pending command");
            return;
        }
        if (p_clcb->p_q_cmd->hdr.event != bta_gattc_opcode_to_int_evt[op - GATTC_OPTYPE_READ]) {
            mapped_op = p_clcb->p_q_cmd->hdr.event - BTA_GATTC_API_READ_EVT + GATTC_OPTYPE_READ;
            if ( mapped_op > GATTC_OPTYPE_INDICATION) {
                mapped_op = 0;
            }

#if (BT_TRACE_VERBOSE == TRUE)
            APPL_TRACE_ERROR("expect op:(%s :0x%04x), receive unexpected operation (%s).",
                             bta_gattc_op_code_name[mapped_op] , p_clcb->p_q_cmd->hdr.event,
                             bta_gattc_op_code_name[op]);
#else
            APPL_TRACE_ERROR("expect op:(%u :0x%04x), receive unexpected operation (%u).",
                             mapped_op , p_clcb->p_q_cmd->hdr.event, op);
#endif
            return;
        }

        /* discard responses if service change indication is received before operation completed */
        if (p_clcb->auto_update == BTA_GATTC_DISC_WAITING && p_clcb->p_srcb->srvc_hdl_chg) {
            APPL_TRACE_DEBUG("Discard all responses when service change indication is received.");
            p_data->op_cmpl.status = GATT_ERROR;
        }

        /* service handle change void the response, discard it */
        if (op == GATTC_OPTYPE_READ) {
            bta_gattc_read_cmpl(p_clcb, &p_data->op_cmpl);
        }

        else if (op == GATTC_OPTYPE_WRITE) {
            bta_gattc_write_cmpl(p_clcb, &p_data->op_cmpl);
        }

        else if (op == GATTC_OPTYPE_EXE_WRITE) {
            bta_gattc_exec_cmpl(p_clcb, &p_data->op_cmpl);
        }

        else if (op == GATTC_OPTYPE_CONFIG) {
            bta_gattc_cfg_mtu_cmpl(p_clcb, &p_data->op_cmpl);
        }

        if (p_clcb->auto_update == BTA_GATTC_DISC_WAITING) {
            p_clcb->auto_update = BTA_GATTC_REQ_WAITING;
            bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_DISCOVER_EVT, NULL);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_op_cmpl
**
** Description      operation completed.
**
** Returns          None.
**
*******************************************************************************/
void  bta_gattc_ignore_op_cmpl(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_clcb);

    /* receive op complete when discovery is started, ignore the response,
        and wait for discovery finish and resent */
    APPL_TRACE_DEBUG("bta_gattc_ignore_op_cmpl op = %d", p_data->hdr.layer_specific);

}
/*******************************************************************************
**
** Function         bta_gattc_search
**
** Description      start a search in the local server cache
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_search(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    tBTA_GATT_STATUS    status = GATT_INTERNAL_ERROR;
    tBTA_GATTC cb_data;
    APPL_TRACE_DEBUG("bta_gattc_search conn_id=%d", p_clcb->bta_conn_id);
    if (p_clcb->p_srcb && p_clcb->p_srcb->p_srvc_cache) {
        status = BTA_GATT_OK;
        /* search the local cache of a server device */
        bta_gattc_search_service(p_clcb, p_data->api_search.p_srvc_uuid);
    }
    cb_data.search_cmpl.status  = status;
    cb_data.search_cmpl.conn_id = p_clcb->bta_conn_id;

    /* end of search or no server cache available */
    ( *p_clcb->p_rcb->p_cback)(BTA_GATTC_SEARCH_CMPL_EVT,  &cb_data);
}
/*******************************************************************************
**
** Function         bta_gattc_q_cmd
**
** Description      enqueue a command into control block, usually because discovery
**                  operation is busy.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_q_cmd(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    bta_gattc_enqueue(p_clcb, p_data);
}
/*******************************************************************************
**
** Function         bta_gattc_cache_open
**
** Description      open a NV cache for loading
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_cache_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    bta_gattc_set_discover_st(p_clcb->p_srcb);

    APPL_TRACE_DEBUG("bta_gattc_cache_open conn_id=%d", p_clcb->bta_conn_id);
    bta_gattc_co_cache_open(p_clcb->p_srcb->server_bda, BTA_GATTC_CI_CACHE_OPEN_EVT,
                            p_clcb->bta_conn_id, FALSE);
}
/*******************************************************************************
**
** Function         bta_gattc_start_load
**
** Description      start cache loading by sending callout open cache
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_ci_open(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    APPL_TRACE_DEBUG("bta_gattc_ci_open conn_id=%d server state=%d" ,
                     p_clcb->bta_conn_id, p_clcb->p_srcb->state);
    if (p_clcb->p_srcb->state == BTA_GATTC_SERV_LOAD) {
        if (p_data->ci_open.status == BTA_GATT_OK) {
            p_clcb->p_srcb->attr_index = 0;
            bta_gattc_co_cache_load(p_clcb->p_srcb->server_bda,
                                    BTA_GATTC_CI_CACHE_LOAD_EVT,
                                    p_clcb->p_srcb->attr_index,
                                    p_clcb->bta_conn_id);
        } else {
            p_clcb->p_srcb->state = BTA_GATTC_SERV_DISC;
            /* cache open failure, start discovery */
            bta_gattc_start_discover(p_clcb, NULL);
        }
    }
    if (p_clcb->p_srcb->state == BTA_GATTC_SERV_SAVE) {
        if (p_data->ci_open.status == BTA_GATT_OK) {
            if (!bta_gattc_cache_save(p_clcb->p_srcb, p_clcb->bta_conn_id)) {
                p_data->ci_open.status = BTA_GATT_ERROR;
            }
        }
        if (p_data->ci_open.status != BTA_GATT_OK) {
            p_clcb->p_srcb->attr_index = 0;
            bta_gattc_co_cache_close(p_clcb->p_srcb->server_bda, p_clcb->bta_conn_id);
            bta_gattc_reset_discover_st(p_clcb->p_srcb, p_clcb->status);

        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_ci_load
**
** Description      cache loading received.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_ci_load(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{

    APPL_TRACE_DEBUG("bta_gattc_ci_load conn_id=%d load status=%d",
                     p_clcb->bta_conn_id, p_data->ci_load.status);

    if (p_data->ci_load.status == BTA_GATT_OK ||
            p_data->ci_load.status == BTA_GATT_MORE) {
        if (p_data->ci_load.num_attr != 0)
            bta_gattc_rebuild_cache(p_clcb->p_srcb, p_data->ci_load.num_attr,
                                    p_data->ci_load.attr, p_clcb->p_srcb->attr_index);

        if (p_data->ci_load.status == BTA_GATT_OK) {
            p_clcb->p_srcb->attr_index = 0;
            bta_gattc_reset_discover_st(p_clcb->p_srcb, BTA_GATT_OK);
            bta_gattc_co_cache_close(p_clcb->p_srcb->server_bda, 0);
        } else { /* load more */
            p_clcb->p_srcb->attr_index += p_data->ci_load.num_attr;

            bta_gattc_co_cache_load(p_clcb->p_srcb->server_bda,
                                    BTA_GATTC_CI_CACHE_LOAD_EVT,
                                    p_clcb->p_srcb->attr_index,
                                    p_clcb->bta_conn_id);
        }
    } else {
        bta_gattc_co_cache_close(p_clcb->p_srcb->server_bda, 0);
        p_clcb->p_srcb->state = BTA_GATTC_SERV_DISC;
        p_clcb->p_srcb->attr_index = 0;
        /* cache load failure, start discovery */
        bta_gattc_start_discover(p_clcb, NULL);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_ci_save
**
** Description      cache loading received.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_ci_save(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    APPL_TRACE_DEBUG("bta_gattc_ci_save conn_id=%d  " ,
                     p_clcb->bta_conn_id   );

    if (!bta_gattc_cache_save(p_clcb->p_srcb, p_clcb->bta_conn_id)) {
        p_clcb->p_srcb->attr_index = 0;
        bta_gattc_co_cache_close(p_clcb->p_srcb->server_bda, 0);
        bta_gattc_reset_discover_st(p_clcb->p_srcb, p_clcb->status);
    }
}
/*******************************************************************************
**
** Function         bta_gattc_fail
**
** Description      report API call failure back to apps
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_fail(tBTA_GATTC_CLCB *p_clcb, tBTA_GATTC_DATA *p_data)
{
    UNUSED(p_data);

    if (p_clcb->status == BTA_GATT_OK) {
        APPL_TRACE_ERROR("operation not supported at current state [%d]", p_clcb->state);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_deregister_cmpl
**
** Description      De-Register a GATT client application with BTA completed.
**
** Returns          void
**
*******************************************************************************/
static void bta_gattc_deregister_cmpl(tBTA_GATTC_RCB *p_clreg)
{
    tBTA_GATTC_CB       *p_cb = &bta_gattc_cb;
    tBTA_GATTC_IF       client_if = p_clreg->client_if;
    tBTA_GATTC          cb_data;
    tBTA_GATTC_CBACK    *p_cback = p_clreg->p_cback;

    memset(&cb_data, 0, sizeof(tBTA_GATTC));

    GATT_Deregister(p_clreg->client_if);
    memset(p_clreg, 0, sizeof(tBTA_GATTC_RCB));

    cb_data.reg_oper.client_if = client_if;
    cb_data.reg_oper.status    = BTA_GATT_OK;

    if (p_cback)
        /* callback with de-register event */
    {
        (*p_cback)(BTA_GATTC_DEREG_EVT,  (tBTA_GATTC *)&cb_data);
    }

    if (bta_gattc_num_reg_app() == 0 && p_cb->state == BTA_GATTC_STATE_DISABLING) {
        p_cb->state = BTA_GATTC_STATE_DISABLED;
    }
}

/*******************************************************************************
**
** Function         bta_gattc_conn_cback
**
** Description      callback functions to GATT client stack.
**
** Returns          void
**
*******************************************************************************/
static void bta_gattc_conn_cback(tGATT_IF gattc_if, BD_ADDR bda, UINT16 conn_id,
                                 BOOLEAN connected, tGATT_DISCONN_REASON reason,
                                 tBT_TRANSPORT transport)
{
    tBTA_GATTC_DATA *p_buf;
    BOOLEAN start_ccc_timer = FALSE;
    tBTA_GATTC_CONN *p_conn = NULL;
    tBTA_GATTC_FIND_SERVICE_CB    result;

    if (reason != 0) {
        APPL_TRACE_WARNING("%s() - cif=%d connected=%d conn_id=%d reason=0x%04x",
                           __FUNCTION__, gattc_if, connected, conn_id, reason);
    }

    if (connected == TRUE){
        p_conn = bta_gattc_conn_find_alloc(bda);
    }
    else if (connected == FALSE){
        p_conn = bta_gattc_conn_find(bda);
    }

    if (p_conn == NULL){
        APPL_TRACE_ERROR("p_conn is NULL in %s\n", __func__);
    }

    if ((transport == BT_TRANSPORT_LE) && (connected == TRUE) && (p_conn != NULL) \
         && (p_conn->service_change_ccc_written == FALSE) && (p_conn->ccc_timer_used == FALSE)){
        result = bta_gattc_register_service_change_notify(conn_id, bda, &start_ccc_timer);
        if (start_ccc_timer == TRUE){
            TIMER_LIST_ENT *ccc_timer = &(p_conn->service_change_ccc_timer);
            /* start a 1000ms timer to wait for service discovery finished */
            bta_gattc_start_service_change_ccc_timer(conn_id, bda, 1000, 0, result, ccc_timer);
            p_conn->ccc_timer_used = TRUE;
        }
        else {
            /* Has written service change ccc; or service change ccc doesn't exist in remote device's gatt database */
            p_conn->service_change_ccc_written = TRUE;
            p_conn->ccc_timer_used = FALSE;
        }

    }
    else if ((transport == BT_TRANSPORT_LE) && (connected == FALSE) && (p_conn != NULL)){
            p_conn->service_change_ccc_written = FALSE;
            if (p_conn->ccc_timer_used == TRUE){
                GKI_freebuf((void *)p_conn->service_change_ccc_timer.param);
                bta_sys_stop_timer(&(p_conn->service_change_ccc_timer));
                p_conn->ccc_timer_used = FALSE;
            }
    }

    bt_bdaddr_t bdaddr;
    bdcpy(bdaddr.address, bda);

    if ((p_buf = (tBTA_GATTC_DATA *) GKI_getbuf(sizeof(tBTA_GATTC_DATA))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_DATA));

        p_buf->int_conn.hdr.event            = connected ? BTA_GATTC_INT_CONN_EVT :
                                               BTA_GATTC_INT_DISCONN_EVT;
        p_buf->int_conn.hdr.layer_specific   = conn_id;
        p_buf->int_conn.client_if            = gattc_if;
        p_buf->int_conn.role                 = L2CA_GetBleConnRole(bda);
        p_buf->int_conn.reason               = reason;
        p_buf->int_conn.transport            = transport;
        bdcpy(p_buf->int_conn.remote_bda, bda);

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_enc_cmpl_cback
**
** Description      encryption complete callback function to GATT client stack.
**
** Returns          void
**
*******************************************************************************/
static void bta_gattc_enc_cmpl_cback(tGATT_IF gattc_if, BD_ADDR bda)
{
    tBTA_GATTC_DATA *p_buf;
    tBTA_GATTC_CLCB *p_clcb = NULL;

    if ((p_clcb = bta_gattc_find_clcb_by_cif(gattc_if, bda, BTA_GATT_TRANSPORT_LE)) == NULL) {
        return;
    }

#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
    /* filter this event just for BTA HH LE GATT client,
       In the future, if we want to enable encryption complete event
       for all GATT clients, we can remove this code */
    if (!bta_hh_le_is_hh_gatt_if(gattc_if)) {
        return;
    }
#endif

    APPL_TRACE_DEBUG("bta_gattc_enc_cmpl_cback: cif = %d", gattc_if);

    if ((p_buf = (tBTA_GATTC_DATA *) GKI_getbuf(sizeof(tBTA_GATTC_DATA))) != NULL) {
        memset(p_buf, 0, sizeof(tBTA_GATTC_DATA));

        p_buf->enc_cmpl.hdr.event            = BTA_GATTC_ENC_CMPL_EVT;
        p_buf->enc_cmpl.hdr.layer_specific   = p_clcb->bta_conn_id;
        p_buf->enc_cmpl.client_if            = gattc_if;
        bdcpy(p_buf->enc_cmpl.remote_bda, bda);

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_process_api_refresh
**
** Description      process refresh API to delete cache and start a new discovery
**                  if currently connected.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_process_api_refresh(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    tBTA_GATTC_SERV *p_srvc_cb = bta_gattc_find_srvr_cache(p_msg->api_conn.remote_bda);
    tBTA_GATTC_CLCB      *p_clcb = &bta_gattc_cb.clcb[0];
    BOOLEAN         found = FALSE;
    UINT8           i;
    UNUSED(p_cb);

    if (p_srvc_cb != NULL) {
        /* try to find a CLCB */
        if (p_srvc_cb->connected && p_srvc_cb->num_clcb != 0) {
            for (i = 0; i < BTA_GATTC_CLCB_MAX; i ++, p_clcb ++) {
                if (p_clcb->in_use && p_clcb->p_srcb == p_srvc_cb) {
                    found = TRUE;
                    break;
                }
            }
            if (found) {
                bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_DISCOVER_EVT, NULL);
                return;
            }
        }
        /* in all other cases, mark it and delete the cache */
        if (p_srvc_cb->p_srvc_cache != NULL) {
            while (!GKI_queue_is_empty(&p_srvc_cb->cache_buffer)) {
                GKI_freebuf (GKI_dequeue (&p_srvc_cb->cache_buffer));
            }

            p_srvc_cb->p_srvc_cache = NULL;
        }
    }
    /* used to reset cache in application */
    bta_gattc_co_cache_reset(p_msg->api_conn.remote_bda);

}
/*******************************************************************************
**
** Function         bta_gattc_process_srvc_chg_ind
**
** Description      process service change indication.
**
** Returns          None.
**
*******************************************************************************/
BOOLEAN bta_gattc_process_srvc_chg_ind(UINT16 conn_id,
                                       tBTA_GATTC_RCB      *p_clrcb,
                                       tBTA_GATTC_SERV     *p_srcb,
                                       tBTA_GATTC_CLCB      *p_clcb,
                                       tBTA_GATTC_NOTIFY    *p_notify,
                                       UINT16 handle)
{
    tBT_UUID        gattp_uuid, srvc_chg_uuid;
    BOOLEAN         processed = FALSE;
    UINT8           i;

    gattp_uuid.len = 2;
    gattp_uuid.uu.uuid16 = UUID_SERVCLASS_GATT_SERVER;

    srvc_chg_uuid.len = 2;
    srvc_chg_uuid.uu.uuid16 = GATT_UUID_GATT_SRV_CHGD;

    if (bta_gattc_uuid_compare(&p_notify->char_id.srvc_id.id.uuid, &gattp_uuid, TRUE) &&
            bta_gattc_uuid_compare(&p_notify->char_id.char_id.uuid, &srvc_chg_uuid, TRUE)) {
        processed = TRUE;
        /* mark service handle change pending */
        p_srcb->srvc_hdl_chg = TRUE;
        /* clear up all notification/indication registration */
        bta_gattc_clear_notif_registration(conn_id);
        /* service change indication all received, do discovery update */
        if ( ++ p_srcb->update_count == bta_gattc_num_reg_app()) {
            /* not an opened connection; or connection busy */
            /* search for first available clcb and start discovery */
            if (p_clcb == NULL || (p_clcb && p_clcb->p_q_cmd != NULL)) {
                for (i = 0 ; i < BTA_GATTC_CLCB_MAX; i ++) {
                    if (bta_gattc_cb.clcb[i].in_use &&
                            bta_gattc_cb.clcb[i].p_srcb == p_srcb &&
                            bta_gattc_cb.clcb[i].p_q_cmd == NULL) {
                        p_clcb = &bta_gattc_cb.clcb[i];
                        break;
                    }
                }
            }
            /* send confirmation here if this is an indication, it should always be */
            GATTC_SendHandleValueConfirm(conn_id, handle);

            /* if connection available, refresh cache by doing discovery now */
            if (p_clcb != NULL) {
                bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_DISCOVER_EVT, NULL);
            }
        }
        /* notify applicationf or service change */
        if (p_clrcb->p_cback != NULL) {
            (* p_clrcb->p_cback)(BTA_GATTC_SRVC_CHG_EVT, (tBTA_GATTC *)p_srcb->server_bda);
        }

    }

    return processed;

}
/*******************************************************************************
**
** Function         bta_gattc_proc_other_indication
**
** Description      process all non-service change indication/notification.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_proc_other_indication(tBTA_GATTC_CLCB *p_clcb, UINT8 op,
                                     tGATT_CL_COMPLETE *p_data,
                                     tBTA_GATTC_NOTIFY *p_notify)
{
    APPL_TRACE_DEBUG("bta_gattc_proc_other_indication check \
                       p_data->att_value.handle=%d p_data->handle=%d",
                     p_data->att_value.handle, p_data->handle);
    APPL_TRACE_DEBUG("is_notify %d", p_notify->is_notify);

    p_notify->is_notify = (op == GATTC_OPTYPE_INDICATION) ? FALSE : TRUE;
    p_notify->len = p_data->att_value.len;
    bdcpy(p_notify->bda, p_clcb->bda);
    memcpy(p_notify->value, p_data->att_value.value, p_data->att_value.len);
    p_notify->conn_id = p_clcb->bta_conn_id;

    if (p_clcb->p_rcb->p_cback) {
        (*p_clcb->p_rcb->p_cback)(BTA_GATTC_NOTIF_EVT,  (tBTA_GATTC *)p_notify);
    }

}
/*******************************************************************************
**
** Function         bta_gattc_process_indicate
**
** Description      process indication/notification.
**
** Returns          None.
**
*******************************************************************************/
void bta_gattc_process_indicate(UINT16 conn_id, tGATTC_OPTYPE op, tGATT_CL_COMPLETE *p_data)
{
    UINT16              handle = p_data->att_value.handle;
    tBTA_GATTC_CLCB     *p_clcb ;
    tBTA_GATTC_RCB      *p_clrcb = NULL;
    tBTA_GATTC_SERV     *p_srcb = NULL;
    tBTA_GATTC_NOTIFY   notify;
    BD_ADDR             remote_bda;
    tBTA_GATTC_IF       gatt_if;
    tBTA_TRANSPORT transport;

    if (!GATT_GetConnectionInfor(conn_id, &gatt_if, remote_bda, &transport)) {
        APPL_TRACE_ERROR("%s indication/notif for unknown app", __func__);
        if (op == GATTC_OPTYPE_INDICATION) {
            GATTC_SendHandleValueConfirm(conn_id, handle);
        }
        return;
    }

    if ((p_clrcb = bta_gattc_cl_get_regcb(gatt_if)) == NULL) {
        APPL_TRACE_ERROR("%s indication/notif for unregistered app", __func__);
        if (op == GATTC_OPTYPE_INDICATION) {
            GATTC_SendHandleValueConfirm(conn_id, handle);
        }
        return;
    }

    if ((p_srcb = bta_gattc_find_srcb(remote_bda)) == NULL) {
        APPL_TRACE_ERROR("%s indication/notif for unknown device, ignore", __func__);
        if (op == GATTC_OPTYPE_INDICATION) {
            GATTC_SendHandleValueConfirm(conn_id, handle);
        }
        return;
    }

    p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id);

    if (bta_gattc_handle2id(p_srcb, handle,
                            &notify.char_id.srvc_id,
                            &notify.char_id.char_id,
                            &notify.descr_type)) {
        /* if non-service change indication/notification, forward to application */
        if (!bta_gattc_process_srvc_chg_ind(conn_id, p_clrcb, p_srcb, p_clcb, &notify, handle)) {
            /* if app registered for the notification */
            if (bta_gattc_check_notif_registry(p_clrcb, p_srcb, &notify)) {
                /* connection not open yet */
                if (p_clcb == NULL) {
                    if ((p_clcb = bta_gattc_clcb_alloc(gatt_if, remote_bda, transport)) != NULL) {
                        p_clcb->bta_conn_id = conn_id;
                        p_clcb->transport   = transport;

                        bta_gattc_sm_execute(p_clcb, BTA_GATTC_INT_CONN_EVT, NULL);
                    } else {
                        APPL_TRACE_ERROR("No resources");
                    }
                }

                if (p_clcb != NULL) {
                    bta_gattc_proc_other_indication(p_clcb, op, p_data, &notify);
                }
            }
            /* no one intersted and need ack? */
            else if (op == GATTC_OPTYPE_INDICATION) {
                APPL_TRACE_DEBUG("%s no one interested, ack now", __func__);
                GATTC_SendHandleValueConfirm(conn_id, handle);
            }
        }
    } else {
        APPL_TRACE_ERROR("%s Indi/Notif for Unknown handle[0x%04x], can not find in local cache.",
                         __func__, handle);
        if (op == GATTC_OPTYPE_INDICATION) {
            GATTC_SendHandleValueConfirm(conn_id, handle);
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_cmpl_cback
**
** Description      client operation complete callback register with BTE GATT.
**
** Returns          None.
**
*******************************************************************************/
static void  bta_gattc_cmpl_cback(UINT16 conn_id, tGATTC_OPTYPE op, tGATT_STATUS status,
                                  tGATT_CL_COMPLETE *p_data)
{
    tBTA_GATTC_CLCB     *p_clcb;
    APPL_TRACE_DEBUG("bta_gattc_cmpl_cback: conn_id = %d op = %d status = %d",
                     conn_id, op, status);

    /* notification and indication processed right away */
    if (op == GATTC_OPTYPE_NOTIFICATION || op == GATTC_OPTYPE_INDICATION) {
        bta_gattc_process_indicate(conn_id, op, p_data);
        return;
    }
    /* for all other operation, not expected if w/o connection */
    else if ((p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id)) == NULL) {
        APPL_TRACE_ERROR("bta_gattc_cmpl_cback unknown conn_id =  %d, ignore data", conn_id);
        return;
    }

    /* if over BR_EDR, inform PM for mode change */
    if (p_clcb->transport == BTA_TRANSPORT_BR_EDR) {
        bta_sys_busy(BTA_ID_GATTC, BTA_ALL_APP_ID, p_clcb->bda);
        bta_sys_idle(BTA_ID_GATTC, BTA_ALL_APP_ID, p_clcb->bda);
    }

    bta_gattc_cmpl_sendmsg(conn_id, op, status, p_data);
}

/*******************************************************************************
**
** Function         bta_gattc_cmpl_sendmsg
**
** Description      client operation complete send message
**
** Returns          None.
**
*******************************************************************************/
static void bta_gattc_cmpl_sendmsg(UINT16 conn_id, tGATTC_OPTYPE op,
                                   tBTA_GATT_STATUS status,
                                   tGATT_CL_COMPLETE *p_data)
{
    const UINT16         len = sizeof(tBTA_GATTC_OP_CMPL) + sizeof(tGATT_CL_COMPLETE);
    tBTA_GATTC_OP_CMPL  *p_buf = (tBTA_GATTC_OP_CMPL *) GKI_getbuf(len);

    if (p_buf != NULL) {
        memset(p_buf, 0, len);
        p_buf->hdr.event = BTA_GATTC_OP_CMPL_EVT;
        p_buf->hdr.layer_specific = conn_id;
        p_buf->status = status;
        p_buf->op_code = op;

        if (p_data != NULL) {
            p_buf->p_cmpl = (tGATT_CL_COMPLETE *)(p_buf + 1);
            memcpy(p_buf->p_cmpl, p_data, sizeof(tGATT_CL_COMPLETE));
        }

        bta_sys_sendmsg(p_buf);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_cong_cback
**
** Description      congestion callback for BTA GATT client.
**
** Returns          void
**
********************************************************************************/
static void bta_gattc_cong_cback (UINT16 conn_id, BOOLEAN congested)
{
    tBTA_GATTC_CLCB *p_clcb;
    tBTA_GATTC cb_data;

    if ((p_clcb = bta_gattc_find_clcb_by_conn_id(conn_id)) != NULL) {
        if (p_clcb->p_rcb->p_cback) {
            cb_data.congest.conn_id = conn_id;
            cb_data.congest.congested = congested;

            (*p_clcb->p_rcb->p_cback)(BTA_GATTC_CONGEST_EVT, &cb_data);
        }
    }
}

#if BLE_INCLUDED == TRUE
/*******************************************************************************
**
** Function         bta_gattc_init_clcb_conn
**
** Description      Initaite a BTA CLCB connection
**
** Returns          void
**
********************************************************************************/
void bta_gattc_init_clcb_conn(UINT8 cif, BD_ADDR remote_bda)
{
    tBTA_GATTC_CLCB     *p_clcb = NULL;
    tBTA_GATTC_DATA     gattc_data;
    UINT16              conn_id;

    /* should always get the connection ID */
    if (GATT_GetConnIdIfConnected(cif, remote_bda, &conn_id, BTA_GATT_TRANSPORT_LE) == FALSE) {
        APPL_TRACE_ERROR("bta_gattc_init_clcb_conn ERROR: not a connected device");
        return;
    }

    /* initaite a new connection here */
    if ((p_clcb = bta_gattc_clcb_alloc(cif, remote_bda, BTA_GATT_TRANSPORT_LE)) != NULL) {
        gattc_data.hdr.layer_specific = p_clcb->bta_conn_id = conn_id;

        gattc_data.api_conn.client_if = cif;
        memcpy(gattc_data.api_conn.remote_bda, remote_bda, BD_ADDR_LEN);
        gattc_data.api_conn.is_direct = TRUE;

        bta_gattc_sm_execute(p_clcb, BTA_GATTC_API_OPEN_EVT, &gattc_data);
    } else {
        APPL_TRACE_ERROR("No resources");
    }
}
/*******************************************************************************
**
** Function         bta_gattc_process_listen_all
**
** Description      process listen all, send open callback to application for all
**                  connected slave LE link.
**
** Returns          void
**
********************************************************************************/
void bta_gattc_process_listen_all(UINT8 cif)
{
    UINT8               i_conn = 0;
    tBTA_GATTC_CONN     *p_conn = &bta_gattc_cb.conn_track[0];

    for (i_conn = 0; i_conn < BTA_GATTC_CONN_MAX; i_conn++, p_conn ++) {
        if (p_conn->in_use ) {
            if (bta_gattc_find_clcb_by_cif(cif, p_conn->remote_bda, BTA_GATT_TRANSPORT_LE) == NULL) {
                bta_gattc_init_clcb_conn(cif, p_conn->remote_bda);
            }
            /* else already connected */
        }
    }
}
/*******************************************************************************
**
** Function         bta_gattc_listen
**
** Description      Start or stop a listen for connection
**
** Returns          void
**
********************************************************************************/
void bta_gattc_listen(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    tBTA_GATTC_RCB      *p_clreg = bta_gattc_cl_get_regcb(p_msg->api_listen.client_if);
    tBTA_GATTC          cb_data;
    UNUSED(p_cb);

    cb_data.reg_oper.status = BTA_GATT_ERROR;
    cb_data.reg_oper.client_if = p_msg->api_listen.client_if;

    if (p_clreg == NULL) {
        APPL_TRACE_ERROR("bta_gattc_listen failed, unknown client_if: %d",
                         p_msg->api_listen.client_if);
        return;
    }
    /* mark bg conn record */
    if (bta_gattc_mark_bg_conn(p_msg->api_listen.client_if,
                               (BD_ADDR_PTR) p_msg->api_listen.remote_bda,
                               p_msg->api_listen.start,
                               TRUE)) {
        if (!GATT_Listen(p_msg->api_listen.client_if,
                         p_msg->api_listen.start,
                         p_msg->api_listen.remote_bda)) {
            APPL_TRACE_ERROR("Listen failure");
            (*p_clreg->p_cback)(BTA_GATTC_LISTEN_EVT, &cb_data);
        } else {
            cb_data.status = BTA_GATT_OK;

            (*p_clreg->p_cback)(BTA_GATTC_LISTEN_EVT, &cb_data);

            if (p_msg->api_listen.start) {
                /* if listen to a specific target */
                if (p_msg->api_listen.remote_bda != NULL) {

                    /* if is a connected remote device */
                    if (L2CA_GetBleConnRole(p_msg->api_listen.remote_bda) == HCI_ROLE_SLAVE &&
                            bta_gattc_find_clcb_by_cif(p_msg->api_listen.client_if,
                                                       p_msg->api_listen.remote_bda,
                                                       BTA_GATT_TRANSPORT_LE) == NULL) {

                        bta_gattc_init_clcb_conn(p_msg->api_listen.client_if,
                                                 p_msg->api_listen.remote_bda);
                    }
                }
                /* if listen to all */
                else {
                    LOG_DEBUG("Listen For All now");
                    /* go through all connected device and send
                    callback for all connected slave connection */
                    bta_gattc_process_listen_all(p_msg->api_listen.client_if);
                }
            }
        }
    }
}

/*******************************************************************************
**
** Function         bta_gattc_broadcast
**
** Description      Start or stop broadcasting
**
** Returns          void
**
********************************************************************************/
void bta_gattc_broadcast(tBTA_GATTC_CB *p_cb, tBTA_GATTC_DATA *p_msg)
{
    tBTA_GATTC_RCB      *p_clreg = bta_gattc_cl_get_regcb(p_msg->api_listen.client_if);
    tBTA_GATTC          cb_data;
    UNUSED(p_cb);

    cb_data.reg_oper.client_if = p_msg->api_listen.client_if;
    cb_data.reg_oper.status = BTM_BleBroadcast(p_msg->api_listen.start);

    if (p_clreg && p_clreg->p_cback) {
        (*p_clreg->p_cback)(BTA_GATTC_LISTEN_EVT, &cb_data);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_start_service_change_ccc_timer
**
** Description      start a timer to wait for service change ccc discovered
**
** Returns          void
**
*******************************************************************************/
void bta_gattc_start_service_change_ccc_timer(UINT16 conn_id, BD_ADDR bda,UINT32 timeout_ms,
                                              UINT8 timer_cnt, UINT8 last_status, TIMER_LIST_ENT *ccc_timer)
{
    tBTA_GATTC_WAIT_CCC_TIMER *p_timer_param = (tBTA_GATTC_WAIT_CCC_TIMER*) GKI_getbuf(sizeof(tBTA_GATTC_WAIT_CCC_TIMER));
    if (p_timer_param != NULL){
        p_timer_param->conn_id = conn_id;
        memcpy(p_timer_param->remote_bda, bda, sizeof(BD_ADDR));
        p_timer_param->count = timer_cnt;
        p_timer_param->last_status = last_status;
        ccc_timer->param = (UINT32)p_timer_param;
        ccc_timer->p_cback = (TIMER_CBACK *)&bta_gattc_wait4_service_change_ccc_cback;
        bta_sys_start_timer(ccc_timer, 0, timeout_ms);
    }
    else {
        APPL_TRACE_ERROR("%s, allocate p_timer_param failed\n", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_gattc_register_service_change_notify
**
** Description      Find remote device's gatt service change characteristic ccc's handle and write 2 to this
**                  this ccc. If not found, start a timer to wait for service discovery finished.
**
** Returns          Return result of service change ccc service discovery result result and written operate result
**
*******************************************************************************/
tBTA_GATTC_FIND_SERVICE_CB bta_gattc_register_service_change_notify(UINT16 conn_id, BD_ADDR remote_bda, BOOLEAN *need_timer){
    tBTA_GATTC_SERV     *p_srcb = NULL;
    tBTA_GATTC_CACHE    *p_cache = NULL;
    tBTA_GATT_ID        result_id;
    tBTA_GATT_ID        *p_result = &result_id;
    tBTA_GATTC_CACHE_ATTR   *p_attr = NULL;
    tGATT_STATUS        write_status;
    tGATT_VALUE         ccc_value;
    tBTA_GATTC_FIND_SERVICE_CB    result;
    BOOLEAN             gatt_cache_found = FALSE;
    BOOLEAN             gatt_service_found = FALSE;
    BOOLEAN             gatt_service_change_found = FALSE;
    BOOLEAN             gatt_ccc_found = FALSE;
    BOOLEAN             start_find_ccc_timer = FALSE;

    tBT_UUID gatt_service_uuid = {LEN_UUID_16, {UUID_SERVCLASS_GATT_SERVER}};
    tBT_UUID gatt_service_change_uuid = {LEN_UUID_16, {GATT_UUID_GATT_SRV_CHGD}};
    tBT_UUID gatt_ccc_uuid = {LEN_UUID_16, {GATT_UUID_CHAR_CLIENT_CONFIG}};

    p_srcb = bta_gattc_find_srcb(remote_bda);
    if ((p_srcb != NULL) && (p_srcb->p_srvc_cache != NULL)){
        p_cache = p_srcb->p_srvc_cache;
        gatt_cache_found = TRUE;
    }
    else {
        start_find_ccc_timer = TRUE;
        result = SERVICE_CHANGE_CACHE_NOT_FOUND;
    }

    /* start to find gatt service */
    if (gatt_cache_found == TRUE){
        while (p_cache) {
            if (bta_gattc_uuid_compare(&gatt_service_uuid, &p_cache->service_uuid.id.uuid, TRUE)) {
                gatt_service_found = TRUE;
                break;
            }
            p_cache = p_cache->p_next;
        }
    }
    else {
        start_find_ccc_timer = TRUE;
        result = SERVICE_CHANGE_CACHE_NOT_FOUND;
    }

    /* start to find gatt service change characteristic */
    if (gatt_service_found == TRUE){
        p_attr = p_cache->p_attr;

        while(p_attr){
            bta_gattc_pack_attr_uuid(p_attr, &p_result->uuid);
            if (bta_gattc_uuid_compare(&gatt_service_change_uuid, &p_result->uuid, TRUE)){
                gatt_service_change_found = TRUE;
                break;
            }
            p_attr = p_attr->p_next;
        }
    }
    else if (gatt_cache_found == TRUE){
        /* Gatt service not found, start a timer to wait for service discovery */
        start_find_ccc_timer = TRUE;
        result = SERVICE_CHANGE_SERVICE_NOT_FOUND;
    }

    /* start to find gatt service change characteristic ccc */
    if (gatt_service_change_found == TRUE){
        p_attr = p_attr->p_next;

        while(p_attr && p_attr->attr_type != BTA_GATTC_ATTR_TYPE_CHAR){
            bta_gattc_pack_attr_uuid(p_attr, &p_result->uuid);
            if (bta_gattc_uuid_compare(&gatt_ccc_uuid, &p_result->uuid, TRUE)){
                gatt_ccc_found = TRUE;
                break;
            }
            p_attr = p_attr->p_next;
        }
    }
    else if (gatt_service_found ==TRUE){
        /* Gatt service found, but service change char not found,
         * Case1: remote device doesn't have service change char, we don't need to start a timer here to
         * wait for service discovery
         * Case2: remote device exist service change char, we have found gatt service, but have not found
         * service change char, we need to start a timer here*/
        start_find_ccc_timer = TRUE;
        result = SERVICE_CHANGE_CHAR_NOT_FOUND;
    }

    if (gatt_ccc_found == TRUE){
        ccc_value.handle = p_attr->attr_handle;
        ccc_value.len = 2;
        ccc_value.value[0] = GATT_CLT_CONFIG_INDICATION;
        ccc_value.auth_req = GATT_AUTH_REQ_NONE;
        if (gatt_is_clcb_allocated(conn_id)) {
            APPL_TRACE_DEBUG("%s, GATTC_Write GATT_BUSY conn_id = %d", __func__, conn_id);
            write_status = GATT_BUSY;
        } else {
            write_status = GATTC_Write (conn_id, GATT_WRITE, &ccc_value);
        }
        if (write_status != GATT_SUCCESS) {
            start_find_ccc_timer = TRUE;
            result = SERVICE_CHANGE_WRITE_CCC_FAILED;
        }
        else {
            start_find_ccc_timer = FALSE;
            result = SERVICE_CHANGE_CCC_WRITTEN_SUCCESS;
        }
    }
    else if (gatt_service_change_found == TRUE){
        /* Gatt service char found, but service change char ccc not found,
         * Case1: remote device doesn't have service change char ccc, we don't need to start a timer here to
         * wait for service discovery
         * Case2: remote device exist service change char ccc, we have found gatt service change char, but have not found
         * service change char ccc, we need to start a timer here*/
        start_find_ccc_timer = TRUE;
        result = SERVICE_CHANGE_CCC_NOT_FOUND;
    }

    if (need_timer != NULL){
        *need_timer = start_find_ccc_timer;
    }

    return result;
}

/*******************************************************************************
**
** Function         bta_gattc_wait4_service_change_ccc_cback
**
** Description      callback function of service_change_ccc_timer
**
** Returns          None
**
*******************************************************************************/
static void bta_gattc_wait4_service_change_ccc_cback (TIMER_LIST_ENT *p_tle)
{
    tBTA_GATTC_FIND_SERVICE_CB result;
    BOOLEAN start_ccc_timer = FALSE;
    UINT32 new_timeout;

    tBTA_GATTC_WAIT_CCC_TIMER *p_timer_param = (tBTA_GATTC_WAIT_CCC_TIMER*) p_tle->param;
    if (p_timer_param == NULL){
        APPL_TRACE_ERROR("p_timer_param is NULL in %s\n", __func__);
        return;
    }

    tBTA_GATTC_CONN *p_conn = bta_gattc_conn_find(p_timer_param->remote_bda);
    if (p_conn == NULL){
        APPL_TRACE_ERROR("p_conn is NULL in %s\n", __func__);
        GKI_freebuf(p_timer_param);
        return;
    }

    result = bta_gattc_register_service_change_notify(p_timer_param->conn_id, p_timer_param->remote_bda, &start_ccc_timer);
    /* If return SERVICE_CHANGE_CHAR_NOT_FOUND or SERVICE_CHANGE_CCC_NOT_FOUND twice, means remote device doesn't have
     * service change char or ccc, stop timer */
    if ((result == p_timer_param->last_status) \
            && ((result == SERVICE_CHANGE_CHAR_NOT_FOUND) || (result == SERVICE_CHANGE_CCC_NOT_FOUND))){
        start_ccc_timer = FALSE;
    }

    if ((start_ccc_timer == TRUE) && (p_timer_param->count < 10)){
        TIMER_LIST_ENT *ccc_timer = &(p_conn->service_change_ccc_timer);
        if (result == SERVICE_CHANGE_WRITE_CCC_FAILED){
            /* retry to write service change ccc, needn't to add counter */
            new_timeout = 200;
        }
        else {
            /* retry to find service change ccc */
            new_timeout = 1000;
            p_timer_param->count ++;
        }
        bta_gattc_start_service_change_ccc_timer(p_timer_param->conn_id, p_timer_param->remote_bda, \
                                                 new_timeout, p_timer_param->count, result, ccc_timer);
    }
    else {
        p_conn->ccc_timer_used = FALSE;
        p_conn->service_change_ccc_written = TRUE;
    }

    GKI_freebuf(p_timer_param);
}

#endif
#endif  ///GATTC_INCLUDED == TRUE && BLE_INCLUDED == TRUE
