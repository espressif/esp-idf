/******************************************************************************
 *
 *  Copyright (C) 2005-2012 Broadcom Corporation
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
 *  This file contains the HID host action functions.
 *
 ******************************************************************************/

#include "common/bt_target.h"

#if defined(BTA_HH_INCLUDED) && (BTA_HH_INCLUDED == TRUE)

#include <string.h>

#include "bta/bta_sys.h"
#include "stack/btm_api.h"
#include "stack/l2c_api.h"
#include "bta_hh_int.h"
#include "bta/bta_hh_co.h"
#include "bta/utl.h"
#include "osi/allocator.h"

/*****************************************************************************
**  Constants
*****************************************************************************/


/*****************************************************************************
**  Local Function prototypes
*****************************************************************************/
static void bta_hh_cback (UINT8 dev_handle, BD_ADDR addr, UINT8 event,
                          UINT32 data, BT_HDR *pdata);
static tBTA_HH_STATUS bta_hh_get_trans_status(UINT32 result);

#if BTA_HH_DEBUG
static char *bta_hh_get_w4_event(UINT16 event);
static char *bta_hh_hid_event_name(UINT16 event);
#endif

/*****************************************************************************
**  Action Functions
*****************************************************************************/
/*******************************************************************************
**
** Function         bta_hh_api_enable
**
** Description      Perform necessary operations to enable HID host.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_api_enable(tBTA_HH_DATA *p_data)
{
    tBTA_HH_STATUS      status = BTA_HH_ERR;
    UINT8               xx;

    /* initialize BTE HID */
    HID_HostInit();

    memset(&bta_hh_cb, 0, sizeof(tBTA_HH_CB));

    HID_HostSetSecurityLevel("", p_data->api_enable.sec_mask);

    /* Register with L2CAP */
    if ( HID_HostRegister (bta_hh_cback) == HID_SUCCESS) {
        /* store parameters */
        bta_hh_cb.p_cback = p_data->api_enable.p_cback;

        status = BTA_HH_OK;
        /* initialize device CB */
        for (xx = 0; xx < BTA_HH_MAX_DEVICE; xx ++) {
            bta_hh_cb.kdev[xx].state        = BTA_HH_IDLE_ST;
            bta_hh_cb.kdev[xx].hid_handle   = BTA_HH_INVALID_HANDLE;
            bta_hh_cb.kdev[xx].index        = xx;
        }

        /* initialize control block map */
        for (xx = 0; xx < BTA_HH_MAX_KNOWN; xx ++) {
            bta_hh_cb.cb_index[xx]          = BTA_HH_IDX_INVALID;
        }
    }

#if (BTA_HH_LE_INCLUDED == TRUE)
    if (status == BTA_HH_OK) {
        bta_hh_le_enable();
    } else
#endif
    {
        /* signal BTA call back event */
        (* bta_hh_cb.p_cback)(BTA_HH_ENABLE_EVT, (tBTA_HH *)&status);
    }
}
/*******************************************************************************
**
** Function         bta_hh_api_disable
**
** Description      Perform necessary operations to disable HID host.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_api_disable(void)
{
    UINT8 xx;

    /* service is not enabled */
    if (bta_hh_cb.p_cback == NULL) {
        return;
    }

    /* no live connection, signal DISC_CMPL_EVT directly */
    if (!bta_hh_cb.cnt_num) {
        bta_hh_disc_cmpl();
    } else { /* otherwise, disconnect all live connections */
        bta_hh_cb.w4_disable = TRUE;

        for (xx = 0; xx < BTA_HH_MAX_DEVICE; xx ++) {
            /* send API_CLOSE event to every connected device */
            if ( bta_hh_cb.kdev[xx].state == BTA_HH_CONN_ST ) {
                /* disconnect all connected devices */
                bta_hh_sm_execute(&bta_hh_cb.kdev[xx],
                                  BTA_HH_API_CLOSE_EVT,
                                  NULL);
            }
        }
    }

    return;
}

/*******************************************************************************
**
** Function         bta_hh_disc_cmpl
**
** Description      All connections have been closed, disable service.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_disc_cmpl(void)
{
    tBTA_HH_STATUS  status = BTA_HH_OK;

    /* Deregister with lower layer */
    if (HID_HostDeregister() != HID_SUCCESS) {
        status = BTA_HH_ERR;
    }

#if (BTA_HH_LE_INCLUDED == TRUE)
    bta_hh_le_deregister();
    UNUSED(status);
#else
    bta_hh_cleanup_disable(status);
#endif
}

/*******************************************************************************
**
** Function         bta_hh_sdp_cback
**
** Description      SDP callback function.
**
** Returns          void
**
*******************************************************************************/
static void bta_hh_sdp_cback(UINT16 result, UINT16 attr_mask,
                             tHID_DEV_SDP_INFO *sdp_rec )
{
    tBTA_HH_DEV_CB     *p_cb = bta_hh_cb.p_cur;
    UINT8              hdl = 0;
    tBTA_HH_STATUS    status = BTA_HH_ERR_SDP;

    /* make sure sdp succeeded and hh has not been disabled */
    if ((result == SDP_SUCCESS) && (p_cb != NULL)) {
        /* security is required for the connection, add attr_mask bit*/
        if (p_cb->sec_mask) {
            attr_mask |= HID_SEC_REQUIRED;
        }

#if BTA_HH_DEBUG
        APPL_TRACE_EVENT("bta_hh_sdp_cback: p_cb: %p result 0x%02x, \
                            attr_mask 0x%02x, handle %x", \
                         p_cb, result, attr_mask, p_cb->hid_handle);
#endif

        /* check to see type of device is supported , and should not been added before */
        if (bta_hh_tod_spt(p_cb, sdp_rec->sub_class)) {
            /* if not added before */
            if (p_cb->hid_handle == BTA_HH_INVALID_HANDLE) {
                /*  add device/update attr_mask information */
                if (HID_HostAddDev (p_cb->addr, attr_mask, &hdl) == HID_SUCCESS) {
                    status = BTA_HH_OK;
                    /* update cb_index[] map */
                    bta_hh_cb.cb_index[hdl] = p_cb->index;
                } else {
                    p_cb->app_id = 0;
                }
            } else {
                hdl = p_cb->hid_handle;
            }
            /* else : incoming connection after SDP should update the SDP information as well */

            if (p_cb->app_id != 0) {
                /* update cb information with attr_mask, dscp_info etc. */
                bta_hh_add_device_to_list(p_cb,  hdl, attr_mask,
                                          &sdp_rec->dscp_info,
                                          sdp_rec->sub_class,
                                          sdp_rec->ssr_max_latency,
                                          sdp_rec->ssr_min_tout,
                                          p_cb->app_id);

                p_cb->dscp_info.ctry_code = sdp_rec->ctry_code;

                status = BTA_HH_OK;
            }

        } else { /* type of device is not supported */
            status = BTA_HH_ERR_TOD_UNSPT;
        }
    }

    /* free disc_db when SDP is completed */
    utl_freebuf((void **)&bta_hh_cb.p_disc_db);

    /* send SDP_CMPL_EVT into state machine */
    bta_hh_sm_execute(p_cb, BTA_HH_SDP_CMPL_EVT, (tBTA_HH_DATA *)&status);

    return;
}
/*******************************************************************************
**
** Function         bta_hh_di_sdp_cback
**
** Description      SDP DI callback function.
**
** Returns          void
**
*******************************************************************************/
static void bta_hh_di_sdp_cback(UINT16 result)
{
    tBTA_HH_DEV_CB     *p_cb = bta_hh_cb.p_cur;
    tBTA_HH_STATUS         status = BTA_HH_ERR_SDP;
    tSDP_DI_GET_RECORD  di_rec;
    tHID_STATUS ret;
#if BTA_HH_DEBUG
    APPL_TRACE_EVENT("bta_hh_di_sdp_cback: p_cb: %p result 0x%02x", p_cb, result);
#endif

    /* if DI record does not exist on remote device, vendor_id in tBTA_HH_DEV_DSCP_INFO will be
         * set to 0xffff and we will allow the connection to go through. Spec mandates that DI
         * record be set, but many HID devices do not set this. So for IOP purposes, we allow the
         * connection to go through and update the DI record to invalid DI entry.*/
    if (((result == SDP_SUCCESS) || (result == SDP_NO_RECS_MATCH)) && (p_cb != NULL)) {
        if (result == SDP_SUCCESS && SDP_GetNumDiRecords(bta_hh_cb.p_disc_db) != 0) {
            /* always update information with primary DI record */
            if (SDP_GetDiRecord(1, &di_rec, bta_hh_cb.p_disc_db) == SDP_SUCCESS) {
                bta_hh_update_di_info(p_cb, di_rec.rec.vendor, di_rec.rec.product, di_rec.rec.version, 0);
            }

        } else { /* no DI record available */
            bta_hh_update_di_info(p_cb, BTA_HH_VENDOR_ID_INVALID, 0, 0, 0);
        }

        if ((ret = HID_HostGetSDPRecord(p_cb->addr,
                                        bta_hh_cb.p_disc_db,
                                        p_bta_hh_cfg->sdp_db_size,
                                        bta_hh_sdp_cback)) == HID_SUCCESS) {
            status = BTA_HH_OK;
        } else {
#if BTA_HH_DEBUG
            APPL_TRACE_DEBUG ("bta_hh_di_sdp_cback:  HID_HostGetSDPRecord failed: Status 0x%2x",
                              ret);
#endif
        }
    }


    if (status != BTA_HH_OK) {
        utl_freebuf((void **)&bta_hh_cb.p_disc_db);
        /* send SDP_CMPL_EVT into state machine */
        bta_hh_sm_execute(p_cb, BTA_HH_SDP_CMPL_EVT, (tBTA_HH_DATA *)&status);
    }
    return;

}


/*******************************************************************************
**
** Function         bta_hh_start_sdp
**
** Description      Start SDP service search, and obtain necessary SDP records.
**                  Only one SDP service search request is allowed at the same
**                  time. For every BTA_HhOpen API call, do SDP first unless SDP
**                  has been done previously.
**
** Returns          void
**
*******************************************************************************/
void bta_hh_start_sdp(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_STATUS          status = BTA_HH_ERR_SDP;
    UINT8                   hdl;

    p_cb->sec_mask  = p_data->api_conn.sec_mask;
    p_cb->mode      = p_data->api_conn.mode;
    p_cb->new_mode  = p_data->api_conn.mode;
    bta_hh_cb.p_cur = p_cb;

#if (BTA_HH_LE_INCLUDED == TRUE)
    if (bta_hh_is_le_device(p_cb, p_data->api_conn.bd_addr)) {
        bta_hh_le_open_conn(p_cb, p_data->api_conn.bd_addr);
        return;
    }
#endif

    /* if previously virtually cabled device, skip SDP */
    if (p_cb->app_id) {
        status = BTA_HH_OK;
#if BTA_HH_DEBUG
        APPL_TRACE_DEBUG("bta_hh_start_sdp:: skip SDP for known devices");
#endif
        if (p_cb->hid_handle == BTA_HH_INVALID_HANDLE) {
            if (HID_HostAddDev (p_cb->addr, p_cb->attr_mask, &hdl) \
                    == HID_SUCCESS) {
                /* update device CB with newly register device handle */
                bta_hh_add_device_to_list(p_cb,  hdl, p_cb->attr_mask, NULL,
                                          p_cb->sub_class,
                                          p_cb->dscp_info.ssr_max_latency,
                                          p_cb->dscp_info.ssr_min_tout,
                                          p_cb->app_id);
                /* update cb_index[] map */
                bta_hh_cb.cb_index[hdl] = p_cb->index;
            } else {
                status = BTA_HH_ERR_NO_RES;
            }
        }
        bta_hh_sm_execute(p_cb, BTA_HH_SDP_CMPL_EVT, (tBTA_HH_DATA *)&status);

        return;
    }
    /* GetSDPRecord. at one time only one SDP procedure can be active */
    else if (!bta_hh_cb.p_disc_db) {
        bta_hh_cb.p_disc_db = (tSDP_DISCOVERY_DB *) osi_malloc(p_bta_hh_cfg->sdp_db_size);

        if (bta_hh_cb.p_disc_db == NULL) {
            status = BTA_HH_ERR_NO_RES;
        } else {
            bta_hh_cb.p_cur = p_cb;
            /* do DI discovery first */
            if (SDP_DiDiscover(p_data->api_conn.bd_addr,
                               bta_hh_cb.p_disc_db,
                               p_bta_hh_cfg->sdp_db_size,
                               bta_hh_di_sdp_cback) != SDP_SUCCESS) {
#if BTA_HH_DEBUG
                APPL_TRACE_DEBUG ("bta_hh_start_sdp:  SDP_DiDiscover failed: \
                    Status 0x%2X", status);
#endif
                status = BTA_HH_ERR_SDP;
                utl_freebuf((void **)&bta_hh_cb.p_disc_db);
            } else {
                status = BTA_HH_OK;
            }
        }
    }

    if (status != BTA_HH_OK) {
        bta_hh_sm_execute(p_cb, BTA_HH_SDP_CMPL_EVT, (tBTA_HH_DATA *)&status);
    }

    return;

}
/*******************************************************************************
**
** Function         bta_hh_sdp_cmpl
**
** Description      When SDP completed, initiate a connection or report error depend
**                  on SDP result.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_sdp_cmpl(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_CONN            conn_dat;
    tBTA_HH_STATUS          status = p_data->status;

#if BTA_HH_DEBUG
    APPL_TRACE_DEBUG ("bta_hh_sdp_cmpl:  status 0x%2X", p_data->status);
#endif

    /* initialize call back data */
    memset((void *)&conn_dat, 0, sizeof(tBTA_HH_CONN));
    conn_dat.handle = p_cb->hid_handle;
    bdcpy(conn_dat.bda, p_cb->addr);

    /* if SDP compl success */
    if ( status == BTA_HH_OK) {
        /* not incoming connection doing SDP, initiate a HID connection */
        if (!p_cb->incoming_conn) {
            tHID_STATUS ret;
            /* set security level */
            HID_HostSetSecurityLevel("", p_cb->sec_mask);

            /* open HID connection */
            if ((ret = HID_HostOpenDev (p_cb->hid_handle)) != HID_SUCCESS) {
#if BTA_HH_DEBUG
                APPL_TRACE_DEBUG ("bta_hh_sdp_cmpl:  HID_HostOpenDev failed: \
                    Status 0x%2X", ret);
#endif
                conn_dat.is_orig = HID_HostConnectOrig(p_cb->hid_handle);
                /* open fail, remove device from management device list */
                HID_HostRemoveDev( p_cb->hid_handle);
                status = BTA_HH_ERR;
            } else {
                status = BTA_HH_OK;
            }
        } else { /* incoming connection SDP finish */
            bta_hh_sm_execute(p_cb, BTA_HH_OPEN_CMPL_EVT, NULL);
        }
    }

    if (status != BTA_HH_OK) {
        /* Check if this was incoming connection request  from an unknown device
           **and connection failed due to missing HID Device SDP UUID
           **In above condition, disconnect the link as well as remove the
           **device from list of HID devices*/
        if ((status == BTA_HH_ERR_SDP) &&
                (p_cb->incoming_conn) && (p_cb->app_id == 0)) {
            APPL_TRACE_DEBUG ("bta_hh_sdp_cmpl:SDP failed for  incoming conn :hndl %d",
                              p_cb->incoming_hid_handle);
            HID_HostRemoveDev( p_cb->incoming_hid_handle);
        }
        conn_dat.status = status;
        (* bta_hh_cb.p_cback)(BTA_HH_OPEN_EVT, (tBTA_HH *)&conn_dat);

        /* move state machine W4_CONN ->IDLE */
        bta_hh_sm_execute(p_cb, BTA_HH_API_CLOSE_EVT, NULL);

        if (p_cb->app_id == 0) {
            /* clean up device control block */
            bta_hh_clean_up_kdev(p_cb);
        }
#if BTA_HH_DEBUG
        bta_hh_trace_dev_db();
#endif
    }
    return;
}

/*******************************************************************************
**
** Function         bta_hh_api_disc_act
**
** Description      HID Host initiate a disconnection.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_api_disc_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_CBDATA    disc_dat;
    tHID_STATUS     status;

#if BTA_HH_LE_INCLUDED == TRUE
    if (p_cb->is_le_device) {
        bta_hh_le_api_disc_act(p_cb);
    } else
#endif
    {
        /* found an active connection */
        disc_dat.handle = p_data ? (UINT8)p_data->hdr.layer_specific : p_cb->hid_handle;
        disc_dat.status = BTA_HH_ERR;

        status = HID_HostCloseDev(disc_dat.handle);

        if (status) {
            (* bta_hh_cb.p_cback)(BTA_HH_CLOSE_EVT, (tBTA_HH *)&disc_dat);
        }
    }

    return;

}
/*******************************************************************************
**
** Function         bta_hh_open_cmpl_act
**
** Description      HID host connection completed
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_open_cmpl_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_CONN        conn ;
    UINT8   dev_handle = p_data ? (UINT8)p_data->hid_cback.hdr.layer_specific : \
                         p_cb->hid_handle;

    memset((void *)&conn, 0, sizeof (tBTA_HH_CONN));
    conn.handle = dev_handle;
    conn.is_orig = HID_HostConnectOrig(dev_handle);
    bdcpy(conn.bda, p_cb->addr);

    /* increase connection number */
    bta_hh_cb.cnt_num ++;

    /* initialize device driver */
    bta_hh_co_open(p_cb->hid_handle, p_cb->sub_class,
                   p_cb->attr_mask,  p_cb->app_id);

#if (BTA_HH_LE_INCLUDED == TRUE)
    conn.status = p_cb->status;
    conn.le_hid = p_cb->is_le_device;
    conn.scps_supported = p_cb->scps_supported;

    if (!p_cb->is_le_device)
#endif
    {
        /* inform role manager */
        bta_sys_conn_open( BTA_ID_HH , p_cb->app_id, p_cb->addr);
    }
    /* set protocol mode when not default report mode */
    if ( p_cb->mode != BTA_HH_PROTO_RPT_MODE
#if (BTA_HH_LE_INCLUDED == TRUE)
            && !p_cb->is_le_device
#endif
       ) {
        if ((HID_HostWriteDev(dev_handle,
                              HID_TRANS_SET_PROTOCOL, HID_PAR_PROTOCOL_BOOT_MODE,
                              0,
                              0, NULL)) != HID_SUCCESS) {
            /* HID connection is up, while SET_PROTO fail */
            conn.status = BTA_HH_ERR_PROTO;
            (* bta_hh_cb.p_cback)(BTA_HH_OPEN_EVT, (tBTA_HH *)&conn);
        } else {
            conn.status = BTA_HH_OK;
            p_cb->w4_evt = BTA_HH_OPEN_EVT;
        }
    } else {
        (* bta_hh_cb.p_cback)(BTA_HH_OPEN_EVT, (tBTA_HH *)&conn);
    }

    p_cb->incoming_conn = FALSE;
    p_cb->incoming_hid_handle = BTA_HH_INVALID_HANDLE;

}
/*******************************************************************************
**
** Function         bta_hh_open_act
**
** Description      HID host receive HID_OPEN_EVT .
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_open_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_API_CONN    conn_data;

    UINT8   dev_handle = p_data ? (UINT8)p_data->hid_cback.hdr.layer_specific : \
                         p_cb->hid_handle;

#if BTA_HH_DEBUG
    APPL_TRACE_EVENT ("bta_hh_open_act:  Device[%d] connected", dev_handle);
#endif

    p_cb->incoming_conn = TRUE;
    /* SDP has been done */
    if (p_cb->app_id != 0) {
        bta_hh_sm_execute(p_cb, BTA_HH_OPEN_CMPL_EVT, p_data);
    } else
        /*  app_id == 0 indicates an incoming connection request arrives without SDP
            performed, do it first */
    {
        /* store the handle here in case sdp fails - need to disconnect */
        p_cb->incoming_hid_handle = dev_handle;

        memset(&conn_data, 0, sizeof(tBTA_HH_API_CONN));
        bdcpy(conn_data.bd_addr, p_cb->addr);
        bta_hh_start_sdp(p_cb, (tBTA_HH_DATA *)&conn_data);
    }

    return;
}


/*******************************************************************************
**
** Function         bta_hh_data_act
**
** Description      HID Host process a data report
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_data_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    BT_HDR  *pdata = p_data->hid_cback.p_data;
    UINT8   *p_rpt = (UINT8 *)(pdata + 1) + pdata->offset;

    bta_hh_co_data((UINT8)p_data->hid_cback.hdr.layer_specific, p_rpt, pdata->len,
                   p_cb->mode, p_cb->sub_class, p_cb->dscp_info.ctry_code, p_cb->addr, p_cb->app_id);

    utl_freebuf((void **)&pdata);
}


/*******************************************************************************
**
** Function         bta_hh_handsk_act
**
** Description      HID Host process a handshake acknowledgement.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_handsk_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_CBDATA  cback_data ;
    tBTA_HH_HSDATA  hs_data;
    tBTA_HH_CONN    conn ;

#if BTA_HH_DEBUG
    APPL_TRACE_DEBUG("HANDSHAKE received for: event = %s data= %d",
                     bta_hh_get_w4_event(p_cb->w4_evt), p_data->hid_cback.data);
#endif

    memset(&hs_data, 0, sizeof(tBTA_HH_HSDATA));
    memset(&cback_data, 0, sizeof(tBTA_HH_CBDATA));

    switch (p_cb->w4_evt) {
    /* GET_ transsaction, handshake indicate unsupported request */
    case BTA_HH_GET_PROTO_EVT:
        hs_data.rsp_data.proto_mode = BTA_HH_PROTO_UNKNOWN;
    /* fall through */
    case BTA_HH_GET_RPT_EVT:
    case BTA_HH_GET_IDLE_EVT :
        hs_data.handle = p_cb->hid_handle;
        /* if handshake gives an OK code for these transaction, fill in UNSUPT */
        if ((hs_data.status = bta_hh_get_trans_status(p_data->hid_cback.data)) == BTA_HH_OK) {
            hs_data.status = BTA_HH_HS_TRANS_NOT_SPT;
        }

        (* bta_hh_cb.p_cback)(p_cb->w4_evt, (tBTA_HH *)&hs_data);
        p_cb->w4_evt = 0;
        break;

    /* acknowledgement from HID device for SET_ transaction */
    case BTA_HH_SET_RPT_EVT:
    case BTA_HH_SET_PROTO_EVT:
    case BTA_HH_SET_IDLE_EVT :
        cback_data.handle  = p_cb->hid_handle;
        cback_data.status = bta_hh_get_trans_status(p_data->hid_cback.data);
        if (cback_data.status == BTA_HH_OK) {
            p_cb->mode = p_cb->new_mode;
        } else {
            p_cb->new_mode = p_cb->mode;
        }
        (* bta_hh_cb.p_cback)(p_cb->w4_evt, (tBTA_HH *)&cback_data);
        p_cb->w4_evt = 0;
        break;

    /* SET_PROTOCOL when open connection */
    case BTA_HH_OPEN_EVT:
        conn.status = p_data->hid_cback.data ? BTA_HH_ERR_PROTO : BTA_HH_OK;
        conn.handle = p_cb->hid_handle;
        conn.is_orig = HID_HostConnectOrig(p_cb->hid_handle);
        bdcpy(conn.bda, p_cb->addr);
        (* bta_hh_cb.p_cback)(p_cb->w4_evt, (tBTA_HH *)&conn);
#if BTA_HH_DEBUG
        bta_hh_trace_dev_db();
#endif
        p_cb->w4_evt = 0;
        break;

    default:
        /* unknown transaction handshake response */
        APPL_TRACE_DEBUG("unknown transaction type");
        break;
    }

    /* transaction acknowledgement received, inform PM for mode change */
    bta_sys_idle(BTA_ID_HH, p_cb->app_id, p_cb->addr);
    return;
}
/*******************************************************************************
**
** Function         bta_hh_ctrl_dat_act
**
** Description      HID Host process a data report from control channel.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_ctrl_dat_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    BT_HDR          *pdata = p_data->hid_cback.p_data;
    UINT8           *data = (UINT8 *)(pdata + 1) + pdata->offset;
    tBTA_HH_HSDATA    hs_data;

#if BTA_HH_DEBUG
    APPL_TRACE_DEBUG("Ctrl DATA received w4: event[%s]",
                     bta_hh_get_w4_event(p_cb->w4_evt));
#endif
    hs_data.status  = BTA_HH_OK;
    hs_data.handle  = p_cb->hid_handle;

    switch (p_cb->w4_evt) {
    case BTA_HH_GET_IDLE_EVT:
        hs_data.rsp_data.idle_rate = *data;
        break;
    case BTA_HH_GET_RPT_EVT:
        hs_data.rsp_data.p_rpt_data = pdata;
        break;
    case BTA_HH_GET_PROTO_EVT:
        /* match up BTE/BTA report/boot mode def*/
        hs_data.rsp_data.proto_mode = ((*data) == HID_PAR_PROTOCOL_REPORT) ? \
                                      BTA_HH_PROTO_RPT_MODE : BTA_HH_PROTO_BOOT_MODE;
#if BTA_HH_DEBUG
        APPL_TRACE_DEBUG("GET_PROTOCOL Mode = [%s]",
                         (hs_data.rsp_data.proto_mode == BTA_HH_PROTO_RPT_MODE) ? "Report" : "Boot");
#endif
        break;
    /* should not expect control DATA for SET_ transaction */
    case BTA_HH_SET_PROTO_EVT:
    /* fall through */
    case BTA_HH_SET_RPT_EVT:
    /* fall through */
    case BTA_HH_SET_IDLE_EVT :
    /* fall through */
    default:
#if BTA_HH_DEBUG
        APPL_TRACE_DEBUG("invalid  transaction type for DATA payload: 4_evt[%s]",
                         bta_hh_get_w4_event(p_cb->w4_evt));
#endif
        break;
    }

    /* inform PM for mode change */
    bta_sys_busy(BTA_ID_HH, p_cb->app_id, p_cb->addr);
    bta_sys_idle(BTA_ID_HH, p_cb->app_id, p_cb->addr);

    (* bta_hh_cb.p_cback)(p_cb->w4_evt, (tBTA_HH *)&hs_data);

    p_cb->w4_evt = 0;
    utl_freebuf((void **)&pdata);

}

/*******************************************************************************
**
** Function         bta_hh_open_failure
**
** Description      report HID open failure when at wait for connection state and receive
**                  device close event.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_open_failure(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_CONN            conn_dat ;
    UINT32                  reason = p_data->hid_cback.data;    /* Reason for closing (32-bit) */

    memset(&conn_dat, 0, sizeof(tBTA_HH_CONN));
    conn_dat.handle = p_cb->hid_handle;
    conn_dat.status = (reason == HID_ERR_AUTH_FAILED) ?
                      BTA_HH_ERR_AUTH_FAILED : BTA_HH_ERR;
    /* check if host initiate the connection*/
    conn_dat.is_orig = HID_HostConnectOrig(p_cb->hid_handle);
    bdcpy(conn_dat.bda, p_cb->addr);
    HID_HostCloseDev(p_cb->hid_handle);

    /* Report OPEN fail event */
    (*bta_hh_cb.p_cback)(BTA_HH_OPEN_EVT, (tBTA_HH *)&conn_dat);

#if BTA_HH_DEBUG
    bta_hh_trace_dev_db();
#endif
    /* clean up control block, but retain SDP info and device handle */
    p_cb->vp            = FALSE;
    p_cb->w4_evt        = 0;

    /* if no connection is active and HH disable is signaled, disable service */
    if (bta_hh_cb.cnt_num == 0 && bta_hh_cb.w4_disable) {
        bta_hh_disc_cmpl();
    }

}

/*******************************************************************************
**
** Function         bta_hh_close_act
**
** Description      HID Host process a close event
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_close_act (tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_CONN            conn_dat ;
    tBTA_HH_CBDATA          disc_dat = {BTA_HH_OK, 0};
    UINT32                  reason = p_data->hid_cback.data;    /* Reason for closing (32-bit) */

    /* if HID_HDEV_EVT_VC_UNPLUG was received, report BTA_HH_VC_UNPLUG_EVT */
    UINT16     event = p_cb->vp ? BTA_HH_VC_UNPLUG_EVT : BTA_HH_CLOSE_EVT;

    disc_dat.handle = p_cb->hid_handle;
    disc_dat.status = p_data->hid_cback.data;

    /* Check reason for closing */
    if ((reason & (HID_L2CAP_CONN_FAIL | HID_L2CAP_REQ_FAIL)) || /* Failure to initialize connection (page timeout or l2cap error) */
            (reason == HID_ERR_AUTH_FAILED) ||                      /* Authentication error (while initiating) */
            (reason == HID_ERR_L2CAP_FAILED)) {                     /* Failure creating l2cap connection */
        /* Failure in opening connection */
        conn_dat.handle = p_cb->hid_handle;
        conn_dat.status = (reason == HID_ERR_AUTH_FAILED) ? BTA_HH_ERR_AUTH_FAILED : BTA_HH_ERR;
        /* check if host initiate the connection*/
        conn_dat.is_orig = HID_HostConnectOrig(p_cb->hid_handle);
        bdcpy(conn_dat.bda, p_cb->addr);
        HID_HostCloseDev(p_cb->hid_handle);

        /* Report OPEN fail event */
        (*bta_hh_cb.p_cback)(BTA_HH_OPEN_EVT, (tBTA_HH *)&conn_dat);

#if BTA_HH_DEBUG
        bta_hh_trace_dev_db();
#endif
        return;
    }
    /* otherwise report CLOSE/VC_UNPLUG event */
    else {
        /* finaliza device driver */
        bta_hh_co_close(p_cb->hid_handle, p_cb->app_id);
        /* inform role manager */
        bta_sys_conn_close( BTA_ID_HH , p_cb->app_id, p_cb->addr);
        /* update total conn number */
        bta_hh_cb.cnt_num --;

        if (disc_dat.status) {
            disc_dat.status = BTA_HH_ERR;
        }

        (*bta_hh_cb.p_cback)(event, (tBTA_HH *)&disc_dat);

        /* if virtually unplug, remove device */
        if (p_cb->vp ) {
            HID_HostRemoveDev( p_cb->hid_handle);
            bta_hh_clean_up_kdev(p_cb);
        }

#if BTA_HH_DEBUG
        bta_hh_trace_dev_db();
#endif
    }

    /* clean up control block, but retain SDP info and device handle */
    p_cb->vp            = FALSE;
    p_cb->w4_evt        = 0;

    /* if no connection is active and HH disable is signaled, disable service */
    if (bta_hh_cb.cnt_num == 0 && bta_hh_cb.w4_disable) {
        bta_hh_disc_cmpl();
    }

    return;
}

/*******************************************************************************
**
** Function         bta_hh_get_dscp_act
**
** Description      Get device report descriptor
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_get_dscp_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    UNUSED(p_data);

#if (BTA_HH_LE_INCLUDED == TRUE)
    if (p_cb->is_le_device) {
        bta_hh_le_get_dscp_act(p_cb);
    } else
#endif
    {
        (*bta_hh_cb.p_cback)(BTA_HH_GET_DSCP_EVT, (tBTA_HH *)&p_cb->dscp_info);
    }
}

/*******************************************************************************
**
** Function         bta_hh_maint_dev_act
**
** Description      HID Host maintain device list.
**
**
** Returns          void
**
*******************************************************************************/
void bta_hh_maint_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tBTA_HH_MAINT_DEV       *p_dev_info = &p_data->api_maintdev;
    tBTA_HH_DEV_INFO        dev_info ;
    UINT8                   dev_handle;

    dev_info.status = BTA_HH_ERR;
    dev_info.handle = BTA_HH_INVALID_HANDLE;

    switch (p_dev_info->sub_event) {
    case BTA_HH_ADD_DEV_EVT:    /* add a device */
        bdcpy(dev_info.bda, p_dev_info->bda);
        /* initialize callback data */
        if (p_cb->hid_handle == BTA_HH_INVALID_HANDLE) {
#if (BTA_HH_LE_INCLUDED == TRUE)
            if (bta_hh_is_le_device(p_cb, p_data->api_conn.bd_addr)) {
                dev_info.handle   = bta_hh_le_add_device(p_cb, p_dev_info);
                dev_info.status   = BTA_HH_OK;
            } else
#endif
            {
                if (HID_HostAddDev(p_dev_info->bda, p_dev_info->attr_mask, &dev_handle)\
                        == HID_SUCCESS) {
                    dev_info.handle   = dev_handle;

#if (defined BTA_HH_LE_INCLUDED && BTA_HH_LE_INCLUDED == TRUE)
                    /* update DI information */
                    bta_hh_update_di_info(p_cb,
                                          p_dev_info->dscp_info.vendor_id,
                                          p_dev_info->dscp_info.product_id,
                                          p_dev_info->dscp_info.version,
                                          p_dev_info->dscp_info.flag);
#else
                    bta_hh_update_di_info(p_cb,
                                          p_dev_info->dscp_info.vendor_id,
                                          p_dev_info->dscp_info.product_id,
                                          p_dev_info->dscp_info.version,
                                          0);

#endif
                    /* add to BTA device list */
                    bta_hh_add_device_to_list(p_cb, dev_handle,
                                              p_dev_info->attr_mask,
                                              &p_dev_info->dscp_info.descriptor,
                                              p_dev_info->sub_class,
                                              p_dev_info->dscp_info.ssr_max_latency,
                                              p_dev_info->dscp_info.ssr_min_tout,
                                              p_dev_info->app_id);
                    /* update cb_index[] map */
                    bta_hh_cb.cb_index[dev_handle] = p_cb->index;
                }
            }
        } else { /* device already been added */
            dev_info.handle = p_cb->hid_handle;
            dev_info.status = BTA_HH_OK;
        }
#if BTA_HH_DEBUG
        bta_hh_trace_dev_db();
#endif

        break;
    case BTA_HH_RMV_DEV_EVT:    /* remove device */
        dev_info.handle = (UINT8)p_dev_info->hdr.layer_specific;
        bdcpy(dev_info.bda, p_cb->addr);

#if BTA_HH_LE_INCLUDED == TRUE
        if (p_cb->is_le_device) {
            bta_hh_le_remove_dev_bg_conn(p_cb);
            bta_hh_sm_execute(p_cb, BTA_HH_API_CLOSE_EVT, NULL);
            bta_hh_clean_up_kdev(p_cb);
        } else
#endif
        {
            if (HID_HostRemoveDev( dev_info.handle ) == HID_SUCCESS) {
                dev_info.status  = BTA_HH_OK;

                /* remove from known device list in BTA */
                bta_hh_clean_up_kdev(p_cb);
            }
        }
        break;

    default:
        APPL_TRACE_DEBUG("invalid command");
        break;
    }

    (* bta_hh_cb.p_cback)(p_dev_info->sub_event, (tBTA_HH *)&dev_info);
}
/*******************************************************************************
**
** Function         bta_hh_write_dev_act
**
** Description      Write device action. can be SET/GET/DATA transaction.
**
** Returns          void
**
*******************************************************************************/
void bta_hh_write_dev_act(tBTA_HH_DEV_CB *p_cb, tBTA_HH_DATA *p_data)
{
    tHID_STATUS status;
    tBTA_HH_CBDATA     cbdata = {BTA_HH_OK, 0};
    tBTA_HH_API_SENDDATA send_data = {BTA_HH_OK, 0, 0};
    UINT16  event = (p_data->api_sndcmd.t_type - BTA_HH_FST_BTE_TRANS_EVT) +
                    BTA_HH_FST_TRANS_CB_EVT;

#if BTA_HH_LE_INCLUDED == TRUE
    if (p_cb->is_le_device) {
        bta_hh_le_write_dev_act(p_cb, p_data);
    } else
#endif
    {

        cbdata.handle = p_cb->hid_handle;
        send_data.handle = p_cb->hid_handle;

        /* match up BTE/BTA report/boot mode def */
        if (p_data->api_sndcmd.t_type == HID_TRANS_SET_PROTOCOL) {
            p_cb->new_mode = p_data->api_sndcmd.param;
            p_data->api_sndcmd.param = ( p_data->api_sndcmd.param == BTA_HH_PROTO_RPT_MODE) ? \
                                       HID_PAR_PROTOCOL_REPORT : HID_PAR_PROTOCOL_BOOT_MODE;
        }

        status = HID_HostWriteDev(p_cb->hid_handle, p_data->api_sndcmd.t_type, p_data->api_sndcmd.param,
                                  p_data->api_sndcmd.data, p_data->api_sndcmd.rpt_id, p_data->api_sndcmd.p_data);
        if (status != HID_SUCCESS) {
            APPL_TRACE_ERROR("HID_HostWriteDev status:%d", status);
            cbdata.status = BTA_HH_ERR;
            send_data.status = BTA_HH_ERR;

            if (p_data->api_sndcmd.t_type != HID_TRANS_CONTROL) {
                switch (p_data->api_sndcmd.t_type) {
                case HID_TRANS_DATA:
                    event = BTA_HH_DATA_EVT;
                    send_data.reason = status;
                    (*bta_hh_cb.p_cback)(event, (tBTA_HH *)&send_data);
                    break;
                default:
                    (*bta_hh_cb.p_cback)(event, (tBTA_HH *)&cbdata);
                    break;
                }
            } else if (p_data->api_sndcmd.param == BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG) {
                (* bta_hh_cb.p_cback)(BTA_HH_VC_UNPLUG_EVT, (tBTA_HH *)&cbdata);
            }
        } else {

            switch (p_data->api_sndcmd.t_type) {
            case HID_TRANS_SET_PROTOCOL:
            /* fall through */
            case HID_TRANS_GET_REPORT:
            /* fall through */
            case HID_TRANS_SET_REPORT:
            /* fall through */
            case HID_TRANS_GET_PROTOCOL:
            /* fall through */
            case HID_TRANS_GET_IDLE:
            /* fall through */
            case HID_TRANS_SET_IDLE:/* set w4_handsk event name for callback function use */
                p_cb->w4_evt = event;
                break;
            case HID_TRANS_DATA:  /* output report */
                (*bta_hh_cb.p_cback)(BTA_HH_DATA_EVT, (tBTA_HH *)&send_data);
            /* fall through */
            case HID_TRANS_CONTROL:
                /* no handshake event will be generated */
                /* if VC_UNPLUG is issued, set flag */
                if (p_data->api_sndcmd.param == BTA_HH_CTRL_VIRTUAL_CABLE_UNPLUG) {
                    p_cb->vp = TRUE;
                }

                break;
            /* currently not expected */
            case HID_TRANS_DATAC:
            default:
                APPL_TRACE_DEBUG("bta_hh_write_dev_act:: cmd type = %d",
                                 p_data->api_sndcmd.t_type);
                break;
            }

            /* if not control type transaction, notify PM for energy control */
            if (p_data->api_sndcmd.t_type != HID_TRANS_CONTROL) {
                /* inform PM for mode change */
                bta_sys_busy(BTA_ID_HH, p_cb->app_id, p_cb->addr);
                bta_sys_idle(BTA_ID_HH, p_cb->app_id, p_cb->addr);
            } else if (p_data->api_sndcmd.param == BTA_HH_CTRL_SUSPEND) {
                bta_sys_sco_close(BTA_ID_HH, p_cb->app_id, p_cb->addr);
            } else if (p_data->api_sndcmd.param == BTA_HH_CTRL_EXIT_SUSPEND) {
                bta_sys_busy(BTA_ID_HH, p_cb->app_id, p_cb->addr);
            }
        }
    }
    return;
}

/*****************************************************************************
**  Static Function
*****************************************************************************/
/*******************************************************************************
**
** Function         bta_hh_cback
**
** Description      BTA HH callback function.
**
**
** Returns          void
**
*******************************************************************************/
static void bta_hh_cback (UINT8 dev_handle, BD_ADDR addr, UINT8 event,
                          UINT32 data, BT_HDR *pdata)
{
    tBTA_HH_CBACK_DATA    *p_buf = NULL;
    UINT16  sm_event = BTA_HH_INVALID_EVT;
    UINT8   xx = 0;

#if BTA_HH_DEBUG
    APPL_TRACE_DEBUG("bta_hh_cback::HID_event [%s]", bta_hh_hid_event_name(event));
#endif

    switch (event) {
    case HID_HDEV_EVT_OPEN:
        sm_event = BTA_HH_INT_OPEN_EVT;
        break;
    case HID_HDEV_EVT_CLOSE:
        sm_event = BTA_HH_INT_CLOSE_EVT;
        break;
    case HID_HDEV_EVT_INTR_DATA:
        sm_event = BTA_HH_INT_DATA_EVT;
        break;
    case HID_HDEV_EVT_HANDSHAKE:
        sm_event = BTA_HH_INT_HANDSK_EVT;
        break;
    case HID_HDEV_EVT_CTRL_DATA:
        sm_event = BTA_HH_INT_CTRL_DATA;
        break;
    case HID_HDEV_EVT_RETRYING:
        break;
    case HID_HDEV_EVT_INTR_DATC:
    case HID_HDEV_EVT_CTRL_DATC:
        /* Unhandled events: Free buffer for DATAC */
        utl_freebuf((void **)&pdata);
        break;
    case HID_HDEV_EVT_VC_UNPLUG:
        for (xx = 0; xx < BTA_HH_MAX_DEVICE; xx++) {
            if (bta_hh_cb.kdev[xx].hid_handle == dev_handle) {
                bta_hh_cb.kdev[xx].vp = TRUE;
                break;
            }
        }
        break;
    }

    if (sm_event != BTA_HH_INVALID_EVT &&
            (p_buf = (tBTA_HH_CBACK_DATA *)osi_malloc(sizeof(tBTA_HH_CBACK_DATA) +
                     sizeof(BT_HDR))) != NULL) {
        p_buf->hdr.event  = sm_event;
        p_buf->hdr.layer_specific = (UINT16)dev_handle;
        p_buf->data       = data;
        bdcpy(p_buf->addr, addr);
        p_buf->p_data     = pdata;

        bta_sys_sendmsg(p_buf);
    }

}
/*******************************************************************************
**
** Function         bta_hh_get_trans_status
**
** Description      translate a handshake result code into BTA HH
**                  status code
**
*******************************************************************************/
static tBTA_HH_STATUS bta_hh_get_trans_status(UINT32 result)
{
    switch (result) {
    case HID_PAR_HANDSHAKE_RSP_SUCCESS :                /*   (0) */
        return BTA_HH_OK;
    case HID_PAR_HANDSHAKE_RSP_NOT_READY :              /*   (1) */
    case HID_PAR_HANDSHAKE_RSP_ERR_INVALID_REP_ID:      /*   (2) */
    case HID_PAR_HANDSHAKE_RSP_ERR_UNSUPPORTED_REQ :    /*   (3) */
    case HID_PAR_HANDSHAKE_RSP_ERR_INVALID_PARAM :      /*   (4) */
        return (tBTA_HH_STATUS)result;
    case HID_PAR_HANDSHAKE_RSP_ERR_UNKNOWN :            /*   (14) */
    case HID_PAR_HANDSHAKE_RSP_ERR_FATAL  :             /*   (15) */
    default:
        return BTA_HH_HS_ERROR;
        break;
    }
}
/*****************************************************************************
**  Debug Functions
*****************************************************************************/

#if (defined BTA_HH_DEBUG && BTA_HH_DEBUG == TRUE)
static char *bta_hh_get_w4_event(UINT16 event)
{
    switch (event) {
    case BTA_HH_GET_RPT_EVT:
        return "BTA_HH_GET_RPT_EVT";
    case BTA_HH_SET_RPT_EVT:
        return "BTA_HH_SET_RPT_EVT";
    case BTA_HH_GET_PROTO_EVT:
        return "BTA_HH_GET_PROTO_EVT";
    case BTA_HH_SET_PROTO_EVT:
        return "BTA_HH_SET_PROTO_EVT";
    case BTA_HH_GET_IDLE_EVT:
        return "BTA_HH_GET_IDLE_EVT";
    case BTA_HH_SET_IDLE_EVT:
        return "BTA_HH_SET_IDLE_EVT";
    case BTA_HH_OPEN_EVT:
        return "BTA_HH_OPEN_EVT";
    default:
        return "Unknown event";
    }

}

static char *bta_hh_hid_event_name(UINT16 event)
{
    switch (event) {
    case HID_HDEV_EVT_OPEN:
        return "HID_HDEV_EVT_OPEN";
    case HID_HDEV_EVT_CLOSE:
        return "HID_HDEV_EVT_CLOSE";
    case HID_HDEV_EVT_RETRYING:
        return "HID_HDEV_EVT_RETRYING";
    case HID_HDEV_EVT_INTR_DATA:
        return "HID_HDEV_EVT_INTR_DATA";
    case HID_HDEV_EVT_INTR_DATC:
        return "HID_HDEV_EVT_INTR_DATC";
    case HID_HDEV_EVT_CTRL_DATA:
        return "HID_HDEV_EVT_CTRL_DATA";
    case HID_HDEV_EVT_CTRL_DATC:
        return "HID_HDEV_EVT_CTRL_DATC";
    case HID_HDEV_EVT_HANDSHAKE:
        return "HID_HDEV_EVT_HANDSHAKE";
    case HID_HDEV_EVT_VC_UNPLUG:
        return "HID_HDEV_EVT_VC_UNPLUG";
    default:
        return "Unknown HID event";
    }
}
#endif
#endif /* BTA_HH_INCLUDED */
