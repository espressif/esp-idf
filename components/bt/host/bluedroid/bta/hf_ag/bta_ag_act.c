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
 *  This file contains action functions for the audio gateway.
 *
 ******************************************************************************/
#include <string.h>
#include "bta_ag_int.h"
#include "bta/bta_sys.h"
#include "bta/bta_ag_api.h"
#include "bta/bta_ag_co.h"
#include "stack/port_api.h"
#include "stack/l2c_api.h"
#include "bta_dm_int.h"
#include "bta/bta_sdp_api.h"
#include "bta/utl.h"
#include "common/bt_trace.h"
#include "osi/allocator.h"

#if (BTA_AG_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

/* maximum length of data to read from RFCOMM */
#define BTA_AG_RFC_READ_MAX     512

/* maximum AT command length */
#define BTA_AG_CMD_MAX          512

const UINT16 bta_ag_uuid[BTA_AG_NUM_IDX] =
{
    UUID_SERVCLASS_HEADSET_AUDIO_GATEWAY,
    UUID_SERVCLASS_AG_HANDSFREE
};

const UINT8 bta_ag_sec_id[BTA_AG_NUM_IDX] =
{
    BTM_SEC_SERVICE_HEADSET_AG,
    BTM_SEC_SERVICE_AG_HANDSFREE
};

const tBTA_SERVICE_ID bta_ag_svc_id[BTA_AG_NUM_IDX] =
{
    BTA_HSP_SERVICE_ID,
    BTA_HFP_SERVICE_ID
};

const tBTA_SERVICE_MASK bta_ag_svc_mask[BTA_AG_NUM_IDX] =
{
    BTA_HSP_SERVICE_MASK,
    BTA_HFP_SERVICE_MASK
};

typedef void (*tBTA_AG_ATCMD_CBACK)(tBTA_AG_SCB *p_scb, UINT16 cmd, UINT8 arg_type,
                                    char *p_arg, INT16 int_arg);

const tBTA_AG_ATCMD_CBACK bta_ag_at_cback_tbl[BTA_AG_NUM_IDX] =
{
    bta_ag_at_hsp_cback,
    bta_ag_at_hfp_cback
};

/*******************************************************************************
**
** Function         bta_ag_cback_open
**
** Description      Send open callback event to application.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_ag_cback_open(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data, tBTA_AG_STATUS status)
{
    tBTA_AG_OPEN    open;
    /* call app callback with open event */
    open.hdr.handle = bta_ag_scb_to_idx(p_scb);
    open.hdr.app_id = p_scb->app_id;
    open.status = status;
    open.service_id = bta_ag_svc_id[p_scb->conn_service];
    if (p_data) {
        /* if p_data is provided then we need to pick the bd address from the open api structure */
        bdcpy(open.bd_addr, p_data->api_open.bd_addr);
    } else {
        bdcpy(open.bd_addr, p_scb->peer_addr);
    }
    (*bta_ag_cb.p_cback)(BTA_AG_OPEN_EVT, (tBTA_AG *) &open);
}

/*******************************************************************************
**
** Function         bta_ag_register
**
** Description      This function initializes values of the AG cb and sets up
**                  the SDP record for the services.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_register(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    tBTA_AG_REGISTER reg;
    /* initialize control block */
    p_scb->reg_services = p_data->api_register.services;
    p_scb->serv_sec_mask = p_data->api_register.sec_mask;
    p_scb->features = p_data->api_register.features;
    p_scb->app_id = p_data->api_register.app_id;
    /* create SDP records */
    bta_ag_create_records(p_scb, p_data);
    /* start RFCOMM servers */
    bta_ag_start_servers(p_scb, p_scb->reg_services);
    /* call app callback with register event */
    reg.hdr.handle = bta_ag_scb_to_idx(p_scb);
    reg.hdr.app_id = p_scb->app_id;
    reg.status = BTA_AG_SUCCESS;
    (*bta_ag_cb.p_cback)(BTA_AG_REGISTER_EVT, (tBTA_AG *) &reg);
}

/*******************************************************************************
**
** Function         bta_ag_deregister
**
** Description      This function removes the sdp records, closes the RFCOMM
**                  servers, and deallocates the service control block.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_deregister(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    /* set dealloc */
    p_scb->dealloc = TRUE;
    /* remove sdp records */
    bta_ag_del_records(p_scb, p_data);
    /* remove rfcomm servers */
    bta_ag_close_servers(p_scb, p_scb->reg_services);
    /* dealloc */
    bta_ag_scb_dealloc(p_scb);
}

/*******************************************************************************
**
** Function         bta_ag_start_dereg
**
** Description      Start a deregister event.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_start_dereg(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    /* set dealloc */
    p_scb->dealloc = TRUE;
    /* remove sdp records */
    bta_ag_del_records(p_scb, p_data);
}

/*******************************************************************************
**
** Function         bta_ag_start_open
**
** Description      This starts an AG open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_start_open(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    BD_ADDR pending_bd_addr;
    /* store parameters */
    if (p_data) {
        bdcpy(p_scb->peer_addr, p_data->api_open.bd_addr);
        p_scb->open_services = p_data->api_open.services;
        p_scb->cli_sec_mask = p_data->api_open.sec_mask;
    }
    /* Check if RFCOMM has any incoming connection to avoid collision. */
    if (PORT_IsOpening (pending_bd_addr)) {
        /* Let the incoming connection goes through.                        */
        /* Issue collision for this scb for now.                            */
        /* We will decide what to do when we find incoming connetion later. */
        bta_ag_collision_cback (0, BTA_ID_AG, 0, p_scb->peer_addr);
        return;
    }
    /* close servers */
    bta_ag_close_servers(p_scb, p_scb->reg_services);
    /* set role */
    p_scb->role = BTA_AG_INT;
    /* do service search */
    bta_ag_do_disc(p_scb, p_scb->open_services);
}

/*******************************************************************************
**
** Function         bta_ag_disc_int_res
**
** Description      This function handles a discovery result when initiator.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_disc_int_res(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UINT16 event = BTA_AG_DISC_FAIL_EVT;
    APPL_TRACE_DEBUG ("bta_ag_disc_int_res: Status: %d", p_data->disc_result.status);

    /* if found service */
    if (p_data->disc_result.status == SDP_SUCCESS ||
        p_data->disc_result.status == SDP_DB_FULL) {
        /* get attributes */
        if (bta_ag_sdp_find_attr(p_scb, p_scb->open_services)) {
            /* set connected service */
            p_scb->conn_service = bta_ag_service_to_idx(p_scb->open_services);
            /* send ourselves sdp ok event */
            event = BTA_AG_DISC_OK_EVT;
        }
    }
    /* free discovery db */
    bta_ag_free_db(p_scb, p_data);
    /* if service not found check if we should search for other service */
    if ((event == BTA_AG_DISC_FAIL_EVT) &&
        (p_data->disc_result.status == SDP_SUCCESS ||
         p_data->disc_result.status == SDP_DB_FULL ||
         p_data->disc_result.status == SDP_NO_RECS_MATCH)) {
        if ((p_scb->open_services & BTA_HFP_SERVICE_MASK) &&
            (p_scb->open_services & BTA_HSP_SERVICE_MASK)) {
            /* search for HSP */
            p_scb->open_services &= ~BTA_HFP_SERVICE_MASK;
            bta_ag_do_disc(p_scb, p_scb->open_services);
        } else if ((p_scb->open_services & BTA_HSP_SERVICE_MASK) &&
                 (p_scb->hsp_version == HSP_VERSION_1_2)) {
            /* search for UUID_SERVCLASS_HEADSET for HSP 1.0 device */
            p_scb->hsp_version = HSP_VERSION_1_0;
            bta_ag_do_disc(p_scb, p_scb->open_services);
        } else {
            /* send ourselves sdp ok/fail event */
            bta_ag_sm_execute(p_scb, event, p_data);
        }
    } else {
        /* send ourselves sdp ok/fail event */
        bta_ag_sm_execute(p_scb, event, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_ag_disc_acp_res
**
** Description      This function handles a discovery result when acceptor.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_disc_acp_res(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    /* if found service */
    if (p_data->disc_result.status == SDP_SUCCESS ||
        p_data->disc_result.status == SDP_DB_FULL) {
        /* get attributes */
        bta_ag_sdp_find_attr(p_scb, bta_ag_svc_mask[p_scb->conn_service]);
    }
    /* free discovery db */
    bta_ag_free_db(p_scb, p_data);
}

/*******************************************************************************
**
** Function         bta_ag_disc_fail
**
** Description      This function handles a discovery failure.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_disc_fail(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UNUSED(p_data);
    /* reopen registered servers */
    bta_ag_start_servers(p_scb, p_scb->reg_services);
    /* reinitialize stuff */
    /* call open cback w. failure */
    bta_ag_cback_open(p_scb, NULL, BTA_AG_FAIL_SDP);
}

/*******************************************************************************
**
** Function         bta_ag_open_fail
**
** Description      open connection failed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_open_fail(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    /* call open cback w. failure */
    bta_ag_cback_open(p_scb, p_data, BTA_AG_FAIL_RESOURCES);
}

/*******************************************************************************
**
** Function         bta_ag_rfc_fail
**
** Description      RFCOMM connection failed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_rfc_fail(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UNUSED(p_data);
    /* reinitialize stuff */
    p_scb->conn_handle = 0;
    p_scb->conn_service = 0;
    p_scb->peer_features = 0;
#if (BTM_WBS_INCLUDED == TRUE )
    p_scb->peer_codecs = BTA_AG_CODEC_NONE;
    p_scb->sco_codec = BTA_AG_CODEC_NONE;
#endif
    p_scb->role = 0;
    p_scb->svc_conn = FALSE;
    p_scb->hsp_version = HSP_VERSION_1_2;
    /*Clear the BD address*/
    bdcpy(p_scb->peer_addr, bd_addr_null);
    /* reopen registered servers */
    bta_ag_start_servers(p_scb, p_scb->reg_services);
    /* call open cback w. failure */
    bta_ag_cback_open(p_scb, NULL, BTA_AG_FAIL_RFCOMM);
}

/*******************************************************************************
**
** Function         bta_ag_rfc_close
**
** Description      RFCOMM connection closed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_rfc_close(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    tBTA_AG_CLOSE    close;
    tBTA_SERVICE_MASK services;
    int i, num_active_conn = 0;
    UNUSED(p_data);
    /* reinitialize stuff */
    p_scb->conn_service = 0;
    p_scb->peer_features = 0;
#if (BTM_WBS_INCLUDED == TRUE )
    p_scb->peer_codecs = BTA_AG_CODEC_NONE;
    p_scb->sco_codec = BTA_AG_CODEC_NONE;
    /* Clear these flags upon SLC teardown */
    p_scb->codec_updated = FALSE;
    p_scb->codec_fallback = FALSE;
    p_scb->codec_msbc_settings = BTA_AG_SCO_MSBC_SETTINGS_T2;
#endif
    p_scb->role = 0;
    p_scb->post_sco = BTA_AG_POST_SCO_NONE;
    p_scb->svc_conn = FALSE;
    p_scb->hsp_version = HSP_VERSION_1_2;
    bta_ag_at_reinit(&p_scb->at_cb);
    /* stop timers */
    bta_sys_stop_timer(&p_scb->act_timer);
#if (BTM_WBS_INCLUDED == TRUE)
    bta_sys_stop_timer(&p_scb->cn_timer);
#endif
    close.hdr.handle = bta_ag_scb_to_idx(p_scb);
    close.hdr.app_id = p_scb->app_id;
    bdcpy(close.bd_addr, p_scb->peer_addr);

    bta_sys_conn_close(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);

    /* call close call-out */
    // bta_ag_sco_co_close(close.hdr.handle);
    bta_ag_sco_co_close();
    /* call close cback */
    (*bta_ag_cb.p_cback)(BTA_AG_CLOSE_EVT, (tBTA_AG *) &close);

    /* if not deregistering (deallocating) reopen registered servers */
    if (p_scb->dealloc == FALSE) {
        /* Clear peer bd_addr so instance can be reused */
        bdcpy(p_scb->peer_addr, bd_addr_null);

        /* start only unopened server */
        services = p_scb->reg_services;
        for (i = 0; i < BTA_AG_NUM_IDX && services != 0; i++) {
            if (p_scb->serv_handle[i]) {
                services &= ~((tBTA_SERVICE_MASK)1 << (BTA_HSP_SERVICE_ID + i));
            }
        }
        bta_ag_start_servers(p_scb, services);
        p_scb->conn_handle = 0;
        /* Make sure SCO state is BTA_AG_SCO_SHUTDOWN_ST */
        bta_ag_sco_shutdown(p_scb, NULL);
        /* Check if all the SLCs are down */
        for (i = 0; i < BTA_AG_NUM_SCB; i++) {
            if (bta_ag_cb.scb[i].in_use && bta_ag_cb.scb[i].svc_conn) {
                num_active_conn++;
            }
        }
        if (!num_active_conn) {
            bta_sys_sco_unuse(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
        }
    } else {
        /* else close port and deallocate scb */
        RFCOMM_RemoveServer(p_scb->conn_handle);
        bta_ag_scb_dealloc(p_scb);
    }
}

/*******************************************************************************
**
** Function         bta_ag_rfc_open
**
** Description      Handle RFCOMM channel open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_rfc_open(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    /* initialize AT feature variables */
    p_scb->clip_enabled = FALSE;
    p_scb->ccwa_enabled = FALSE;
    p_scb->cmer_enabled = FALSE;
    p_scb->cmee_enabled = FALSE;
    p_scb->inband_enabled = ((p_scb->features & BTA_AG_FEAT_INBAND) == BTA_AG_FEAT_INBAND);

    /* set up AT command interpreter */
    p_scb->at_cb.p_at_tbl = (tBTA_AG_AT_CMD *) bta_ag_at_tbl[p_scb->conn_service];
    p_scb->at_cb.p_cmd_cback = (tBTA_AG_AT_CMD_CBACK *) bta_ag_at_cback_tbl[p_scb->conn_service];
    p_scb->at_cb.p_err_cback = (tBTA_AG_AT_ERR_CBACK *) bta_ag_at_err_cback;
    p_scb->at_cb.p_user = p_scb;
    p_scb->at_cb.cmd_max_len = BTA_AG_CMD_MAX;
    bta_ag_at_init(&p_scb->at_cb);

    /* call app open call-out */
    bta_ag_sco_co_open(bta_ag_scb_to_idx(p_scb), p_scb->air_mode, BTA_HFP_SCO_OUT_PKT_SIZE, bta_ag_svc_id[p_scb->conn_service]);
    bta_sys_conn_open(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
    bta_ag_cback_open(p_scb, NULL, BTA_AG_SUCCESS);

    if (p_scb->conn_service == BTA_AG_HFP) {
        /* if hfp start timer for service level conn */
        bta_sys_start_timer(&p_scb->act_timer, BTA_AG_SVC_TOUT_EVT, p_bta_ag_cfg->conn_tout);
    } else {
        /* else service level conn is open */
        bta_ag_svc_conn_open(p_scb, p_data);
    }
}

/*******************************************************************************
**
** Function         bta_ag_rfc_acp_open
**
** Description      Handle RFCOMM channel open when accepting connection.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_rfc_acp_open(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UINT16          lcid;
    int             i;
    tBTA_AG_SCB     *ag_scb, *other_scb;
    BD_ADDR         dev_addr;
    int             status;
    /* set role */
    p_scb->role = BTA_AG_ACP;
    APPL_TRACE_DEBUG ("bta_ag_rfc_acp_open: serv_handle0 = %d serv_handle1 = %d",
                       p_scb->serv_handle[0], p_scb->serv_handle[1]);
    /* get bd addr of peer */
    if (PORT_SUCCESS != (status=PORT_CheckConnection(p_data->rfc.port_handle, dev_addr, &lcid))) {
        APPL_TRACE_DEBUG ("bta_ag_rfc_acp_open error PORT_CheckConnection returned status %d", status);
    }
    /* Collision Handling */
    for (i = 0, ag_scb = &bta_ag_cb.scb[0]; i < BTA_AG_NUM_SCB; i++, ag_scb++) {
        if ((ag_scb->in_use) && (ag_scb->colli_tmr_on)) {
            /* stop collision timer */
            ag_scb->colli_tmr_on = FALSE;
            bta_sys_stop_timer (&ag_scb->colli_timer);

            if (bdcmp (dev_addr, ag_scb->peer_addr) == 0) {
                /* If incoming and outgoing device are same, nothing more to do.            */
                /* Outgoing conn will be aborted because we have successful incoming conn.  */
            } else {
                /* Resume outgoing connection. */
                other_scb = bta_ag_get_other_idle_scb (p_scb);
                if (other_scb) {
                    bdcpy(other_scb->peer_addr, ag_scb->peer_addr);
                    other_scb->open_services = ag_scb->open_services;
                    other_scb->cli_sec_mask = ag_scb->cli_sec_mask;
                    bta_ag_resume_open (other_scb);
                }
            }
            break;
        }
    }
    bdcpy (p_scb->peer_addr, dev_addr);
    /* determine connected service from port handle */
    for (i = 0; i < BTA_AG_NUM_IDX; i++) {
        APPL_TRACE_DEBUG ("bta_ag_rfc_acp_open: i = %d serv_handle = %d port_handle = %d",
                           i, p_scb->serv_handle[i], p_data->rfc.port_handle);
        if (p_scb->serv_handle[i] == p_data->rfc.port_handle) {
            p_scb->conn_service = i;
            p_scb->conn_handle = p_data->rfc.port_handle;
            break;
        }
    }
    APPL_TRACE_DEBUG ("bta_ag_rfc_acp_open: conn_service = %d conn_handle = %d",
                       p_scb->conn_service, p_scb->conn_handle);
    /* close any unopened server */
    bta_ag_close_servers(p_scb, (p_scb->reg_services & ~bta_ag_svc_mask[p_scb->conn_service]));
    /* do service discovery to get features */
    bta_ag_do_disc(p_scb, bta_ag_svc_mask[p_scb->conn_service]);
    /* continue with common open processing */
    bta_ag_rfc_open(p_scb, p_data);
}

/*******************************************************************************
**
** Function         bta_ag_rfc_data
**
** Description      Read and process data from RFCOMM.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_rfc_data(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UINT16  len;
    char    buf[BTA_AG_RFC_READ_MAX];
    UNUSED(p_data);
    memset(buf, 0, BTA_AG_RFC_READ_MAX);

    APPL_TRACE_DEBUG("bta_ag_rfc_data");
    /* do the following */
    for (;;) {
        /* read data from rfcomm; if bad status, we're done */
        if (PORT_ReadData(p_scb->conn_handle, buf, BTA_AG_RFC_READ_MAX, &len) != PORT_SUCCESS) {
            break;
        }
        /* if no data, we're done */
        if (len == 0) {
            break;
        }
        /* run AT command interpreter on data */
        bta_sys_busy(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
        bta_ag_at_parse(&p_scb->at_cb, buf, len);
        bta_sys_idle(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
        /* no more data to read, we're done */
        if (len < BTA_AG_RFC_READ_MAX) {
            break;
        }
    }
}

/*******************************************************************************
**
** Function         bta_ag_start_close
**
** Description      Start the process of closing SCO and RFCOMM connection.
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_start_close(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    /* Take the link out of sniff and set L2C idle time to 0 */
    bta_dm_pm_active(p_scb->peer_addr);
    L2CA_SetIdleTimeoutByBdAddr(p_scb->peer_addr, 0, BT_TRANSPORT_BR_EDR);
    /* if SCO is open close SCO and wait on RFCOMM close */
    if (bta_ag_sco_is_open(p_scb)) {
        p_scb->post_sco = BTA_AG_POST_SCO_CLOSE_RFC;
    } else {
        p_scb->post_sco = BTA_AG_POST_SCO_NONE;
        bta_ag_rfc_do_close(p_scb, p_data);
    }
    /* Always do SCO shutdown to handle all SCO corner cases */
    bta_ag_sco_shutdown(p_scb, p_data);
}

/*******************************************************************************
**
** Function         bta_ag_post_sco_open
**
** Description      Perform post-SCO open action, if any
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_post_sco_open(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    switch (p_scb->post_sco) {
        case BTA_AG_POST_SCO_RING:
            bta_ag_send_ring(p_scb, p_data);
            p_scb->post_sco = BTA_AG_POST_SCO_NONE;
            break;

        case BTA_AG_POST_SCO_CALL_CONN:
            bta_ag_send_call_inds(p_scb, BTA_AG_IN_CALL_CONN_RES);
            p_scb->post_sco = BTA_AG_POST_SCO_NONE;
            break;

        default:
            break;
    }
}

/*******************************************************************************
**
** Function         bta_ag_post_sco_close
**
** Description      Perform post-SCO close action, if any
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_post_sco_close(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    switch (p_scb->post_sco) {
        case BTA_AG_POST_SCO_CLOSE_RFC:
            bta_ag_rfc_do_close(p_scb, p_data);
            p_scb->post_sco = BTA_AG_POST_SCO_NONE;
            break;

        case BTA_AG_POST_SCO_CALL_CONN:
            bta_ag_send_call_inds(p_scb, BTA_AG_IN_CALL_CONN_RES);
            p_scb->post_sco = BTA_AG_POST_SCO_NONE;
            break;

        case BTA_AG_POST_SCO_CALL_ORIG:
            bta_ag_send_call_inds(p_scb, BTA_AG_OUT_CALL_ORIG_RES);
            p_scb->post_sco = BTA_AG_POST_SCO_NONE;
            break;

        case BTA_AG_POST_SCO_CALL_END:
            bta_ag_send_call_inds(p_scb, BTA_AG_END_CALL_RES);
            p_scb->post_sco = BTA_AG_POST_SCO_NONE;
            break;

        case BTA_AG_POST_SCO_CALL_END_INCALL:
        {
            bta_ag_send_call_inds(p_scb, BTA_AG_END_CALL_RES);
            /* Sending callsetup IND and Ring were defered to after SCO close. */
            bta_ag_send_call_inds(p_scb, BTA_AG_IN_CALL_RES);

            if (bta_ag_inband_enabled(p_scb) && !(p_scb->features & BTA_AG_FEAT_NOSCO)) {
                p_scb->post_sco = BTA_AG_POST_SCO_RING;
                bta_ag_sco_open(p_scb, p_data);
            } else {
                p_scb->post_sco = BTA_AG_POST_SCO_NONE;
                bta_ag_send_ring(p_scb, p_data);
            }
            break;
        }

        default:
            break;
    }
}

/*******************************************************************************
**
** Function         bta_ag_svc_conn_open
**
** Description      Service level connection opened
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_svc_conn_open(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    tBTA_AG_CONN evt;
    UNUSED(p_data);

    if (!p_scb->svc_conn) {
        /* set state variable */
        p_scb->svc_conn = TRUE;
        /* Clear AT+BIA mask from previous SLC if any. */
        p_scb->bia_masked_out = 0;
        /* stop timer */
        bta_sys_stop_timer(&p_scb->act_timer);
        /* call callback */
        evt.hdr.handle = bta_ag_scb_to_idx(p_scb);
        evt.hdr.app_id = p_scb->app_id;
        evt.peer_feat = p_scb->peer_features;
        bdcpy(evt.bd_addr, p_scb->peer_addr);
#if (BTM_WBS_INCLUDED == TRUE )
        evt.peer_codec  = p_scb->peer_codecs;
#endif
        if ((p_scb->call_ind != BTA_AG_CALL_INACTIVE) ||
            (p_scb->callsetup_ind != BTA_AG_CALLSETUP_NONE)) {
            bta_sys_sco_use(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
        }
        (*bta_ag_cb.p_cback)(BTA_AG_CONN_EVT, (tBTA_AG *) &evt);
    }
}

/*******************************************************************************
**
** Function         bta_ag_ci_rx_data
**
** Description      Send result code to RFCOMM
**
** Returns          void
**
*******************************************************************************/
void bta_ag_ci_rx_data(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UINT16 len;
    tBTA_AG_CI_RX_WRITE *p_rx_write_msg = (tBTA_AG_CI_RX_WRITE *)p_data;
    char *p_data_area = (char *)(p_rx_write_msg+1);     /* Point to data area after header */

    APPL_TRACE_DEBUG("bta_ag_ci_rx_data:");
    /* send to RFCOMM */
    bta_sys_busy(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
    PORT_WriteData(p_scb->conn_handle, p_data_area, strlen(p_data_area), &len);
    bta_sys_idle(BTA_ID_AG, p_scb->app_id, p_scb->peer_addr);
}

/*******************************************************************************
**
** Function         bta_ag_rcvd_slc_ready
**
** Description      Handles SLC ready call-in in case of pass-through mode.
**
** Returns          void
**
*******************************************************************************/
void bta_ag_rcvd_slc_ready(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
    UNUSED(p_data);
    APPL_TRACE_DEBUG("bta_ag_rcvd_slc_ready: handle = %d", bta_ag_scb_to_idx(p_scb));

    if (bta_ag_cb.parse_mode == BTA_AG_PASS_THROUGH) {
        /* In pass-through mode, BTA knows that SLC is ready only through call-in. */
        bta_ag_svc_conn_open(p_scb, NULL);
    }
}

/*******************************************************************************
**
** Function         bta_ag_setcodec
**
** Description      Handle API SetCodec
**
**
** Returns          void
**
*******************************************************************************/
void bta_ag_setcodec(tBTA_AG_SCB *p_scb, tBTA_AG_DATA *p_data)
{
#if (BTM_WBS_INCLUDED == TRUE )
    tBTA_AG_PEER_CODEC codec_type = p_data->api_setcodec.codec;
    tBTA_AG_VAL        val;

    /* Check if the requested codec type is valid */
    if((codec_type != BTA_AG_CODEC_NONE) &&
       (codec_type != BTA_AG_CODEC_CVSD) &&
       (codec_type != BTA_AG_CODEC_MSBC)) {
        val.num = codec_type;
        val.hdr.status = BTA_AG_FAIL_RESOURCES;
        APPL_TRACE_ERROR("bta_ag_setcodec error: unsupported codec type %d", codec_type);
        (*bta_ag_cb.p_cback)(BTA_AG_WBS_EVT, (tBTA_AG *) &val);
        return;
    }

    if((p_scb->peer_codecs & codec_type) || (codec_type == BTA_AG_CODEC_NONE) ||
        (codec_type == BTA_AG_CODEC_CVSD)) {
        p_scb->sco_codec = codec_type;
        p_scb->codec_updated = TRUE;
        val.num = codec_type;
        val.hdr.status = BTA_AG_SUCCESS;
        APPL_TRACE_DEBUG("bta_ag_setcodec: Updated codec type %d", codec_type);
    } else {
        val.num = codec_type;
        val.hdr.status = BTA_AG_FAIL_RESOURCES;
        APPL_TRACE_ERROR("bta_ag_setcodec error: unsupported codec type %d", codec_type);
    }
    (*bta_ag_cb.p_cback)(BTA_AG_WBS_EVT, (tBTA_AG *) &val);
#endif
}

#endif /* #if (BTA_AG_INCLUDED == TRUE) */
