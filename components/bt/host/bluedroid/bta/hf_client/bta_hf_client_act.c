/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
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
 *  This file contains action functions for the handsfree client.
 *
 ******************************************************************************/

#include "bta/bta_api.h"
#include "bta/bta_hf_client_api.h"
#include "bta_hf_client_int.h"
#include "bta_dm_int.h"
#include "stack/l2c_api.h"
#include "stack/port_api.h"
#include "bta/bta_sys.h"
#include "bta/utl.h"
#include "common/bt_defs.h"
#include <string.h>
#include "osi/allocator.h"

#if (BTA_HF_INCLUDED == TRUE)
/*****************************************************************************
**  Constants
*****************************************************************************/

/* maximum length of data to read from RFCOMM */
#define BTA_HF_CLIENT_RFC_READ_MAX     512

/*******************************************************************************
**
** Function         bta_hf_client_register
**
** Description      This function initializes values of the scb and sets up
**                  the SDP record for the services.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_register(tBTA_HF_CLIENT_DATA *p_data)
{
    tBTA_HF_CLIENT_REGISTER evt;
    tBTA_UTL_COD   cod;

    memset(&evt, 0, sizeof(evt));

    /* initialize control block */
    bta_hf_client_scb_init();

    bta_hf_client_cb.scb.serv_sec_mask = p_data->api_register.sec_mask;
    bta_hf_client_cb.scb.features = p_data->api_register.features;

    /* initialize AT control block */
    bta_hf_client_at_init();

    /* create SDP records */
    bta_hf_client_create_record(p_data);

    /* Set the Class of Device (Audio service class bit) */
    cod.service = BTM_COD_SERVICE_AUDIO;
    cod.major = BTM_COD_MAJOR_AUDIO;
    cod.minor = BTM_COD_MINOR_CONFM_HANDSFREE;
    utl_set_device_class(&cod, BTA_UTL_SET_COD_ALL);

    /* start RFCOMM server */
    bta_hf_client_start_server();

    /* call app callback with register event */
    evt.status = BTA_HF_CLIENT_SUCCESS;
    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_REGISTER_EVT, &evt);
}

/*******************************************************************************
**
** Function         bta_hf_client_deregister
**
** Description      This function removes the sdp records, closes the RFCOMM
**                  servers, and deallocates the service control block.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_deregister(tBTA_HF_CLIENT_DATA *p_data)
{
    tBTA_UTL_COD   cod;

    bta_hf_client_cb.scb.deregister = TRUE;

    /* Clear the Audio service class bit */
    cod.service = BTM_COD_SERVICE_AUDIO;
    utl_set_device_class(&cod, BTA_UTL_CLR_COD_SERVICE_CLASS);

    /* remove sdp record */
    bta_hf_client_del_record(p_data);

    /* remove rfcomm server */
    bta_hf_client_close_server();

    /* disable */
    bta_hf_client_scb_disable();
}

/*******************************************************************************
**
** Function         bta_hf_client_start_dereg
**
** Description      Start a deregister event.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_start_dereg(tBTA_HF_CLIENT_DATA *p_data)
{
    bta_hf_client_cb.scb.deregister = TRUE;

    /* remove sdp record */
    bta_hf_client_del_record(p_data);
}

/*******************************************************************************
**
** Function         bta_hf_client_start_close
**
** Description      Start the process of closing SCO and RFCOMM connection.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_start_close(tBTA_HF_CLIENT_DATA *p_data)
{
    /* Take the link out of sniff and set L2C idle time to 0 */
#if (BTA_DM_PM_INCLUDED == TRUE)
    bta_dm_pm_active(bta_hf_client_cb.scb.peer_addr);
#endif /* (BTA_DM_PM_INCLUDED == TRUE) */
    L2CA_SetIdleTimeoutByBdAddr(bta_hf_client_cb.scb.peer_addr, 0, BT_TRANSPORT_BR_EDR);

    /* if SCO is open close SCO and wait on RFCOMM close */
    if (bta_hf_client_cb.scb.sco_state == BTA_HF_CLIENT_SCO_OPEN_ST) {
        bta_hf_client_cb.scb.sco_close_rfc = TRUE;
    } else {
        bta_hf_client_rfc_do_close(p_data);
    }

    /* always do SCO shutdown to handle all SCO corner cases */
    bta_hf_client_sco_shutdown(NULL);
}

/*******************************************************************************
**
** Function         bta_hf_client_start_open
**
** Description      This starts an HF Client open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_start_open(tBTA_HF_CLIENT_DATA *p_data)
{
    BD_ADDR pending_bd_addr;

    /* store parameters */
    if (p_data) {
        bdcpy(bta_hf_client_cb.scb.peer_addr, p_data->api_open.bd_addr);
        bta_hf_client_cb.scb.cli_sec_mask = p_data->api_open.sec_mask;
    }

    /* Check if RFCOMM has any incoming connection to avoid collision. */
    if (PORT_IsOpening (pending_bd_addr)) {
        /* Let the incoming connection goes through.                        */
        /* Issue collision for now.                                         */
        /* We will decide what to do when we find incoming connection later.*/
        bta_hf_client_collision_cback (0, BTA_ID_HS, 0, bta_hf_client_cb.scb.peer_addr);
        return;
    }

    /* close server */
    bta_hf_client_close_server();

    /* set role */
    bta_hf_client_cb.scb.role = BTA_HF_CLIENT_INT;

    /* do service search */
    bta_hf_client_do_disc();
}

/*******************************************************************************
**
** Function         bta_hf_client_cback_open
**
** Description      Send open callback event to application.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hf_client_cback_open(tBTA_HF_CLIENT_DATA *p_data, tBTA_HF_CLIENT_STATUS status)
{
    tBTA_HF_CLIENT_OPEN evt;

    memset(&evt, 0, sizeof(evt));

    /* call app callback with open event */
    evt.status = status;
    if (p_data) {
        /* if p_data is provided then we need to pick the bd address from the open api structure */
        bdcpy(evt.bd_addr, p_data->api_open.bd_addr);
    } else {
        bdcpy(evt.bd_addr, bta_hf_client_cb.scb.peer_addr);
    }

    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_OPEN_EVT, &evt);
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_open
**
** Description      Handle RFCOMM channel open.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_open(tBTA_HF_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    bta_sys_conn_open(BTA_ID_HS, 1, bta_hf_client_cb.scb.peer_addr);

    bta_hf_client_cback_open(NULL, BTA_HF_CLIENT_SUCCESS);

    /* start SLC procedure */
    bta_hf_client_slc_seq(FALSE);
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_acp_open
**
** Description      Handle RFCOMM channel open when accepting connection.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_acp_open(tBTA_HF_CLIENT_DATA *p_data)
{
    UINT16          lcid;
    BD_ADDR         dev_addr = {0};
    int             status;

    /* set role */
    bta_hf_client_cb.scb.role = BTA_HF_CLIENT_ACP;

    APPL_TRACE_DEBUG ("bta_hf_client_rfc_acp_open: serv_handle = %d rfc.port_handle = %d",
                      bta_hf_client_cb.scb.serv_handle, p_data->rfc.port_handle);

    /* get bd addr of peer */
    if (PORT_SUCCESS != (status = PORT_CheckConnection(p_data->rfc.port_handle, FALSE, dev_addr, &lcid))) {
        APPL_TRACE_DEBUG ("bta_hf_client_rfc_acp_open error PORT_CheckConnection returned status %d", status);
    }

    /* Collision Handling */
    if (bta_hf_client_cb.scb.colli_tmr_on) {
        /* stop collision timer */
        bta_hf_client_cb.scb.colli_tmr_on = FALSE;
        bta_sys_free_timer (&bta_hf_client_cb.scb.colli_timer);

        if (bdcmp (dev_addr, bta_hf_client_cb.scb.peer_addr) == 0) {
            /* If incoming and outgoing device are same, nothing more to do.            */
            /* Outgoing conn will be aborted because we have successful incoming conn.  */
        } else {
            /* Resume outgoing connection. */
            bta_hf_client_resume_open ();
        }
    }

    bdcpy (bta_hf_client_cb.scb.peer_addr, dev_addr);
    bta_hf_client_cb.scb.conn_handle = p_data->rfc.port_handle;

    /* do service discovery to get features */
    bta_hf_client_do_disc();

    /* continue with open processing */
    bta_hf_client_rfc_open(p_data);
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_fail
**
** Description      RFCOMM connection failed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_fail(tBTA_HF_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    /* reinitialize stuff */
    bta_hf_client_cb.scb.conn_handle = 0;
    bta_hf_client_cb.scb.peer_features = 0;
    bta_hf_client_cb.scb.chld_features = 0;
    bta_hf_client_cb.scb.role = BTA_HF_CLIENT_ACP;
    bta_hf_client_cb.scb.svc_conn = FALSE;
    bta_hf_client_cb.scb.send_at_reply = FALSE;
    bta_hf_client_cb.scb.negotiated_codec = BTM_SCO_CODEC_CVSD;

    bta_hf_client_at_reset();

    /* reopen server */
    bta_hf_client_start_server();

    /* call open cback w. failure */
    bta_hf_client_cback_open(NULL, BTA_HF_CLIENT_FAIL_RFCOMM);
}

/*******************************************************************************
**
** Function         bta_hf_client_disc_fail
**
** Description      This function handles a discovery failure.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_disc_fail(tBTA_HF_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    /* reopen server */
    bta_hf_client_start_server();

    /* reinitialize stuff */

    /* call open cback w. failure */
    bta_hf_client_cback_open(NULL, BTA_HF_CLIENT_FAIL_SDP);
}

/*******************************************************************************
**
** Function         bta_hf_client_open_fail
**
** Description      open connection failed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_open_fail(tBTA_HF_CLIENT_DATA *p_data)
{
    /* call open cback w. failure */
    bta_hf_client_cback_open(p_data, BTA_HF_CLIENT_FAIL_RESOURCES);
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_close
**
** Description      RFCOMM connection closed.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_close(tBTA_HF_CLIENT_DATA *p_data)
{
    UNUSED(p_data);

    /* reinitialize stuff */
    bta_hf_client_cb.scb.peer_features = 0;
    bta_hf_client_cb.scb.chld_features = 0;
    bta_hf_client_cb.scb.role = BTA_HF_CLIENT_ACP;
    bta_hf_client_cb.scb.svc_conn = FALSE;
    bta_hf_client_cb.scb.send_at_reply = FALSE;
    bta_hf_client_cb.scb.negotiated_codec = BTM_SCO_CODEC_CVSD;

    bta_hf_client_at_reset();

    bta_sys_conn_close(BTA_ID_HS, 1, bta_hf_client_cb.scb.peer_addr);

    /* call close cback */
    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_CLOSE_EVT, NULL);

    /* if not deregistering reopen server */
    if (bta_hf_client_cb.scb.deregister == FALSE) {
        /* Clear peer bd_addr so instance can be reused */
        bdcpy(bta_hf_client_cb.scb.peer_addr, bd_addr_null);

        /* start server as it might got closed on open*/
        bta_hf_client_start_server();

        bta_hf_client_cb.scb.conn_handle = 0;

        /* Make sure SCO is shutdown */
        bta_hf_client_sco_shutdown(NULL);

        bta_sys_sco_unuse(BTA_ID_HS, 1, bta_hf_client_cb.scb.peer_addr);
    }
    /* else close port and deallocate scb */
    else {
        bta_hf_client_close_server();
        bta_hf_client_scb_disable();
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_disc_int_res
**
** Description      This function handles a discovery result when initiator.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_disc_int_res(tBTA_HF_CLIENT_DATA *p_data)
{
    UINT16 event = BTA_HF_CLIENT_DISC_FAIL_EVT;

    APPL_TRACE_DEBUG ("bta_hf_client_disc_int_res: Status: %d", p_data->disc_result.status);

    /* if found service */
    if (p_data->disc_result.status == SDP_SUCCESS ||
            p_data->disc_result.status == SDP_DB_FULL) {
        /* get attributes */
        if (bta_hf_client_sdp_find_attr()) {
            event = BTA_HF_CLIENT_DISC_OK_EVT;
        }
    }

    /* free discovery db */
    bta_hf_client_free_db(p_data);

    /* send ourselves sdp ok/fail event */
    bta_hf_client_sm_execute(event, p_data);
}

/*******************************************************************************
**
** Function         bta_hf_client_disc_acp_res
**
** Description      This function handles a discovery result when acceptor.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_disc_acp_res(tBTA_HF_CLIENT_DATA *p_data)
{
    /* if found service */
    if (p_data->disc_result.status == SDP_SUCCESS ||
            p_data->disc_result.status == SDP_DB_FULL) {
        /* get attributes */
        bta_hf_client_sdp_find_attr();
    }

    /* free discovery db */
    bta_hf_client_free_db(p_data);
}

/*******************************************************************************
**
** Function         bta_hf_client_rfc_data
**
** Description      Read and process data from RFCOMM.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_rfc_data(tBTA_HF_CLIENT_DATA *p_data)
{
    UINT16  len;
    char    *buf = osi_calloc(BTA_HF_CLIENT_RFC_READ_MAX);
    if (buf == NULL) {
        APPL_TRACE_ERROR("No mem %s", __FUNCTION__);
        return;
    }

    UNUSED(p_data);

    /* read data from rfcomm; if bad status, we're done */
    while (PORT_ReadData(bta_hf_client_cb.scb.conn_handle, buf, BTA_HF_CLIENT_RFC_READ_MAX, &len) == PORT_SUCCESS) {
        /* if no data, we're done */
        if (len == 0) {
            break;
        }

        bta_hf_client_at_parse(buf, len);

        /* no more data to read, we're done */
        if (len < BTA_HF_CLIENT_RFC_READ_MAX) {
            break;
        }
    }
    osi_free(buf);
}

/*******************************************************************************
**
** Function         bta_hf_client_svc_conn_open
**
** Description      Service level connection opened
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_svc_conn_open(tBTA_HF_CLIENT_DATA *p_data)
{
    tBTA_HF_CLIENT_CONN evt;
    UNUSED(p_data);

    memset(&evt, 0, sizeof(evt));

    if (!bta_hf_client_cb.scb.svc_conn) {
        /* set state variable */
        bta_hf_client_cb.scb.svc_conn = TRUE;

        /* call callback */
        evt.peer_feat = bta_hf_client_cb.scb.peer_features;
        evt.chld_feat = bta_hf_client_cb.scb.chld_features;

        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_CONN_EVT, &evt);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_cback_ind
**
** Description      Send indicator callback event to application.
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_ind(tBTA_HF_CLIENT_IND_TYPE type, UINT16 value)
{
    tBTA_HF_CLIENT_IND evt;

    memset(&evt, 0, sizeof(evt));

    evt.type = type;
    evt.value = value;

    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_IND_EVT, &evt);
}

/*******************************************************************************
**
** Function         bta_hf_client_evt_val
**
** Description      Send event to application.
**                  This is a generic helper for events with common data.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_evt_val(tBTA_HF_CLIENT_EVT type, UINT16 value)
{
    tBTA_HF_CLIENT_VAL evt;

    memset(&evt, 0, sizeof(evt));

    evt.value = value;

    (*bta_hf_client_cb.p_cback)(type, &evt);
}

/*******************************************************************************
**
** Function         bta_hf_client_operator_name
**
** Description      Send operator name event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_operator_name(char *name)
{
    tBTA_HF_CLIENT_OPERATOR_NAME *evt;

    if ((evt = osi_calloc(sizeof(tBTA_HF_CLIENT_OPERATOR_NAME))) != NULL) {
        strlcpy(evt->name, name, BTA_HF_CLIENT_OPERATOR_NAME_LEN + 1);
        evt->name[BTA_HF_CLIENT_OPERATOR_NAME_LEN] = '\0';

        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_OPERATOR_NAME_EVT, evt);
        osi_free(evt);
    } else {
        APPL_TRACE_ERROR("No mem: %s", __func__);
    }
}


/*******************************************************************************
**
** Function         bta_hf_client_clip
**
** Description      Send CLIP event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_clip(char *number)
{
    tBTA_HF_CLIENT_NUMBER *evt;

    if ((evt = osi_calloc(sizeof(tBTA_HF_CLIENT_NUMBER))) != NULL) {
        strlcpy(evt->number, number, BTA_HF_CLIENT_NUMBER_LEN + 1);
        evt->number[BTA_HF_CLIENT_NUMBER_LEN] = '\0';

        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_CLIP_EVT, evt);
        osi_free(evt);
    } else {
        APPL_TRACE_ERROR("No mem: %s", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_ccwa
**
** Description      Send CLIP event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_ccwa(char *number)
{
    tBTA_HF_CLIENT_NUMBER *evt;

    if ((evt = osi_calloc(sizeof(tBTA_HF_CLIENT_NUMBER))) != NULL) {
        strlcpy(evt->number, number, BTA_HF_CLIENT_NUMBER_LEN + 1);
        evt->number[BTA_HF_CLIENT_NUMBER_LEN] = '\0';


        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_CCWA_EVT, evt);
        osi_free(evt);
    } else {
        APPL_TRACE_ERROR("No mem: %s", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_at_result
**
** Description      Send AT result event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_at_result(tBTA_HF_CLIENT_AT_RESULT_TYPE type, UINT16 cme)
{
    tBTA_HF_CLIENT_AT_RESULT evt;

    memset(&evt, 0, sizeof(evt));

    evt.type = type;
    evt.cme = cme;

    (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_AT_RESULT_EVT, &evt);
}

/*******************************************************************************
**
** Function         bta_hf_client_clcc
**
** Description      Send clcc event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_clcc(UINT32 idx, BOOLEAN incoming, UINT8 status, BOOLEAN mpty, char *number)
{
    tBTA_HF_CLIENT_CLCC *evt;

    if ((evt = osi_calloc(sizeof(tBTA_HF_CLIENT_CLCC))) != NULL) {
        evt->idx = idx;
        evt->inc = incoming;
        evt->status = status;
        evt->mpty = mpty;

        if (number) {
            evt->number_present = TRUE;
            strlcpy(evt->number, number, BTA_HF_CLIENT_NUMBER_LEN + 1);
            evt->number[BTA_HF_CLIENT_NUMBER_LEN] = '\0';
        }

        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_CLCC_EVT, evt);
        osi_free(evt);
    } else {
        APPL_TRACE_ERROR("No mem, %s\n", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_cnum
**
** Description      Send cnum event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_cnum(char *number, UINT16 service)
{
    tBTA_HF_CLIENT_CNUM *evt;

    if ((evt = osi_calloc(sizeof(tBTA_HF_CLIENT_CNUM))) != NULL) {

        evt->service = service;
        strlcpy(evt->number, number, BTA_HF_CLIENT_NUMBER_LEN + 1);
        evt->number[BTA_HF_CLIENT_NUMBER_LEN] = '\0';

        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_CNUM_EVT, evt);
        osi_free(evt);
    } else {
        APPL_TRACE_ERROR("No mem, %s", __func__);
    }
}

/*******************************************************************************
**
** Function         bta_hf_client_binp
**
** Description      Send BINP event to application.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hf_client_binp(char *number)
{
    tBTA_HF_CLIENT_NUMBER *evt;

    if ((evt = osi_calloc(sizeof(tBTA_HF_CLIENT_NUMBER))) != NULL) {
        strlcpy(evt->number, number, BTA_HF_CLIENT_NUMBER_LEN + 1);
        evt->number[BTA_HF_CLIENT_NUMBER_LEN] = '\0';

        (*bta_hf_client_cb.p_cback)(BTA_HF_CLIENT_BINP_EVT, evt);
        osi_free(evt);
    } else {
        APPL_TRACE_ERROR("No mem: %s", __func__);
    }
}

#endif /* #if (BTA_HF_INCLUDED == TRUE) */
