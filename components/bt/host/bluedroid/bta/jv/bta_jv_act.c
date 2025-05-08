/******************************************************************************
 *
 *  Copyright (C) 2006-2012 Broadcom Corporation
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
 *  This file contains action functions for BTA JV APIs.
 *
 ******************************************************************************/

#include <pthread.h>
#include <stdlib.h>

#include "osi/allocator.h"
#include "osi/osi.h"
#include "stack/bt_types.h"
#include "bta/utl.h"
#include "bta/bta_sys.h"
#include "bta/bta_api.h"
#include "bta/bta_jv_api.h"
#include "bta_jv_int.h"
#include "bta/bta_jv_co.h"
#include "stack/btm_api.h"
#include "btm_int.h"
#include "stack/sdp_api.h"
#include "stack/l2c_api.h"
#include "stack/port_api.h"
#include <string.h>
#include "stack/rfcdefs.h"
#include "stack/avct_api.h"
#include "stack/avdt_api.h"
#include "stack/gap_api.h"
#include "stack/l2c_api.h"


#if (defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE)
#if BTA_JV_L2CAP_INCLUDED
/* one of these exists for each client */
struct fc_client {
    struct fc_client    *next_all_list;
    struct fc_client    *next_chan_list;
    BD_ADDR              remote_addr;
    uint32_t             id;
    tBTA_JV_L2CAP_CBACK *p_cback;
    void                *user_data;
    uint16_t             handle;
    uint16_t             chan;
    uint8_t              sec_id;
    unsigned             server      : 1;
    unsigned             init_called : 1;
};

/* one of these exists for each channel we're dealing with */
struct fc_channel {
    struct fc_channel   *next;
    struct fc_client    *clients;
    uint8_t              has_server : 1;
    uint16_t             chan;
};


static struct fc_client *fc_clients;
static struct fc_channel *fc_channels;
static uint32_t fc_next_id;
static pthread_once_t fc_init_once = PTHREAD_ONCE_INIT;


static void fc_init_work(void)
{
    fc_clients = NULL;
    fc_channels = NULL;
    fc_next_id = 0;

    //more init here if needed...
}

static void __attribute__((unused)) fc_init(void)
{
    pthread_once(&fc_init_once,  fc_init_work);
}

static void fcchan_conn_chng_cbk(UINT16 chan, BD_ADDR bd_addr, BOOLEAN connected,
                                 UINT16 reason, tBT_TRANSPORT );
static void fcchan_data_cbk(UINT16 chan, BD_ADDR bd_addr, BT_HDR *p_buf);
#endif /* BTA_JV_L2CAP_INCLUDED */

extern void uuid_to_string_legacy(bt_uuid_t *p_uuid, char *str);
static inline void logu(const char *title, const uint8_t *p_uuid)
{
    char uuids[128];
    uuid_to_string_legacy((bt_uuid_t *)p_uuid, uuids);
    APPL_TRACE_DEBUG("%s: %s", title, uuids);
}


static tBTA_JV_STATUS bta_jv_free_set_pm_profile_cb(UINT32 jv_handle);
static void bta_jv_pm_conn_busy(tBTA_JV_PM_CB *p_cb);
static void bta_jv_pm_conn_idle(tBTA_JV_PM_CB *p_cb);
static void bta_jv_pm_state_change(tBTA_JV_PM_CB *p_cb, const tBTA_JV_CONN_STATE state);
tBTA_JV_STATUS bta_jv_set_pm_conn_state(tBTA_JV_PM_CB *p_cb, const tBTA_JV_CONN_STATE new_st);

#if BT_SDP_BQB_INCLUDED
static BOOLEAN s_sdp_bqb_add_language_attr_flag = FALSE;
#endif /* BT_SDP_BQB_INCLUDED */

#if BTA_JV_RFCOMM_INCLUDED
static tBTA_JV_PCB *bta_jv_add_rfc_port(tBTA_JV_RFC_CB *p_cb, tBTA_JV_PCB *p_pcb_open);
static int find_rfc_pcb(void *user_data, tBTA_JV_RFC_CB **cb, tBTA_JV_PCB **pcb);
static void bta_jv_port_mgmt_sr_cback(UINT32 code, UINT16 port_handle, void* data);
static void bta_jv_port_event_sr_cback(UINT32 code, UINT16 port_handle);
static int bta_jv_port_data_co_cback(UINT16 port_handle, UINT8 *buf, UINT16 len, int type);
#endif /* BTA_JV_RFCOMM_INCLUDED */
/*******************************************************************************
**
** Function     bta_jv_alloc_sec_id
**
** Description  allocate a security id
**
** Returns
**
*******************************************************************************/
UINT8 bta_jv_alloc_sec_id(void)
{
    UINT8 ret = 0;
    int i;
    for (i = 0; i < BTA_JV_NUM_SERVICE_ID; i++) {
        if (0 == bta_jv_cb.sec_id[i]) {
            bta_jv_cb.sec_id[i] = BTA_JV_FIRST_SERVICE_ID + i;
            ret = bta_jv_cb.sec_id[i];
            break;
        }
    }
    return ret;

}
UNUSED_ATTR static int get_sec_id_used(void)
{
    int i;
    int used = 0;
    for (i = 0; i < BTA_JV_NUM_SERVICE_ID; i++) {
        if (bta_jv_cb.sec_id[i]) {
            used++;
        }
    }
    if (used == BTA_JV_NUM_SERVICE_ID) {
        APPL_TRACE_ERROR("get_sec_id_used, sec id exceeds the limit:%d",
                         BTA_JV_NUM_SERVICE_ID);
    }
    return used;
}
#if BTA_JV_RFCOMM_INCLUDED
UNUSED_ATTR static int get_rfc_cb_used(void)
{
    int i;
    int used = 0;
    for (i = 0; i < BTA_JV_MAX_RFC_CONN; i++) {
        if (bta_jv_cb.rfc_cb[i].handle ) {
            used++;
        }
    }
    if (used == BTA_JV_MAX_RFC_CONN) {
        APPL_TRACE_ERROR("get_sec_id_used, rfc ctrl block exceeds the limit:%d",
                         BTA_JV_MAX_RFC_CONN);
    }
    return used;
}
#endif /* BTA_JV_RFCOMM_INCLUDED */

/*******************************************************************************
**
** Function     bta_jv_free_sec_id
**
** Description  free the given security id
**
** Returns
**
*******************************************************************************/
static void bta_jv_free_sec_id(UINT8 *p_sec_id)
{
    UINT8 sec_id = *p_sec_id;
    *p_sec_id = 0;
    if (sec_id >= BTA_JV_FIRST_SERVICE_ID && sec_id <= BTA_JV_LAST_SERVICE_ID) {
        BTM_SecClrService(sec_id);
        bta_jv_cb.sec_id[sec_id - BTA_JV_FIRST_SERVICE_ID] = 0;
    }
}

#if BTA_JV_RFCOMM_INCLUDED
/*******************************************************************************
**
** Function     bta_jv_alloc_rfc_cb
**
** Description  allocate a control block for the given port handle
**
** Returns
**
*******************************************************************************/
tBTA_JV_RFC_CB *bta_jv_alloc_rfc_cb(UINT16 port_handle, tBTA_JV_PCB **pp_pcb)
{
    tBTA_JV_RFC_CB *p_cb = NULL;
    tBTA_JV_PCB *p_pcb;
    int i, j;
    for (i = 0; i < BTA_JV_MAX_RFC_CONN; i++) {
        if (0 == bta_jv_cb.rfc_cb[i].handle ) {
            p_cb = &bta_jv_cb.rfc_cb[i];
            /* mask handle to distinguish it with L2CAP handle */
            p_cb->handle = (i + 1) | BTA_JV_RFCOMM_MASK;

            p_cb->max_sess          = 1;
            p_cb->curr_sess         = 1;
            for (j = 0; j < BTA_JV_MAX_RFC_SR_SESSION; j++) {
                p_cb->rfc_hdl[j] = 0;
            }
            p_cb->rfc_hdl[0]        = port_handle;
            APPL_TRACE_DEBUG( "bta_jv_alloc_rfc_cb port_handle:%d handle:0x%2x",
                              port_handle, p_cb->handle);

            p_pcb = &bta_jv_cb.port_cb[port_handle - 1];
            p_pcb->handle = p_cb->handle;
            p_pcb->port_handle = port_handle;
            p_pcb->p_pm_cb = NULL;
            *pp_pcb = p_pcb;
            break;
        }
    }
    if (p_cb == NULL) {
        APPL_TRACE_ERROR( "bta_jv_alloc_rfc_cb: port_handle:%d, ctrl block exceeds "
                          "limit:%d", port_handle, BTA_JV_MAX_RFC_CONN);
    }
    return p_cb;
}

/*******************************************************************************
**
** Function     bta_jv_rfc_port_to_pcb
**
** Description  find the port control block associated with the given port handle
**
** Returns
**
*******************************************************************************/
tBTA_JV_PCB *bta_jv_rfc_port_to_pcb(UINT16 port_handle)
{
    tBTA_JV_PCB *p_pcb = NULL;

    if ((port_handle > 0) && (port_handle <= MAX_RFC_PORTS)
            && bta_jv_cb.port_cb[port_handle - 1].handle) {
        p_pcb = &bta_jv_cb.port_cb[port_handle - 1];
    }

    return p_pcb;
}

/*******************************************************************************
**
** Function     bta_jv_rfc_port_to_cb
**
** Description  find the RFCOMM control block associated with the given port handle
**
** Returns
**
*******************************************************************************/
tBTA_JV_RFC_CB *bta_jv_rfc_port_to_cb(UINT16 port_handle)
{
    tBTA_JV_RFC_CB *p_cb = NULL;
    UINT32 handle;

    if ((port_handle > 0) && (port_handle <= MAX_RFC_PORTS)
            && bta_jv_cb.port_cb[port_handle - 1].handle) {
        handle = bta_jv_cb.port_cb[port_handle - 1].handle;
        handle &= BTA_JV_RFC_HDL_MASK;
        handle &= ~BTA_JV_RFCOMM_MASK;
        if (handle) {
            p_cb = &bta_jv_cb.rfc_cb[handle - 1];
        }
    } else {
        APPL_TRACE_WARNING("bta_jv_rfc_port_to_cb(port_handle:0x%x):jv handle:0x%x not"
                           " FOUND", port_handle, bta_jv_cb.port_cb[port_handle - 1].handle);
    }
    return p_cb;
}

static tBTA_JV_STATUS bta_jv_free_rfc_cb(tBTA_JV_RFC_CB *p_cb, tBTA_JV_PCB *p_pcb, BOOLEAN close_server, BOOLEAN close_pending)
{
    tBTA_JV_STATUS status = BTA_JV_SUCCESS;
    BOOLEAN remove_server = FALSE;

    UINT8 used = 0, i, listen = 0;
    tPORT_STATE port_state;
    UINT32 event_mask = BTA_JV_RFC_EV_MASK;
    UINT32 scn_num = (UINT32)p_cb->scn;
    tBTA_JV evt_data = {0};

    if (!p_cb || !p_pcb) {
        APPL_TRACE_ERROR("bta_jv_free_sr_rfc_cb, p_cb or p_pcb cannot be null");
        return BTA_JV_FAILURE;
    }
    APPL_TRACE_DEBUG("bta_jv_free_sr_rfc_cb: max_sess:%d, curr_sess:%d, p_pcb:%p, user:"
                     "%p, state:%d, jv handle: 0x%x" , p_cb->max_sess, p_cb->curr_sess, p_pcb,
                     p_pcb->user_data, p_pcb->state, p_pcb->handle);

    if (p_cb->curr_sess <= 0) {
        return BTA_JV_SUCCESS;
    }

    switch (p_pcb->state) {
    case BTA_JV_ST_CL_CLOSING:
    case BTA_JV_ST_SR_CLOSING:
        APPL_TRACE_DEBUG("bta_jv_free_sr_rfc_cb: return on closing, port state:%d, "
                           "scn:%d, p_pcb:%p, user_data:%p", p_pcb->state, p_cb->scn, p_pcb,
                           p_pcb->user_data);
        status = BTA_JV_FAILURE;
        break;
    case BTA_JV_ST_CL_OPEN:
    case BTA_JV_ST_CL_OPENING:
        APPL_TRACE_DEBUG("bta_jv_free_sr_rfc_cb: state: %d, scn:%d,"
                         " user_data:%p", p_pcb->state, p_cb->scn, p_pcb->user_data);
        p_pcb->state = BTA_JV_ST_CL_CLOSING;
        break;
    case BTA_JV_ST_SR_LISTEN:
        p_pcb->state = BTA_JV_ST_SR_CLOSING;
        remove_server = TRUE;
        APPL_TRACE_DEBUG("bta_jv_free_sr_rfc_cb: state: BTA_JV_ST_SR_LISTEN, scn:%d,"
                         " user_data:%p", p_cb->scn, p_pcb->user_data);
        break;
    case BTA_JV_ST_SR_OPEN:
        p_pcb->state = BTA_JV_ST_SR_CLOSING;
        APPL_TRACE_DEBUG("bta_jv_free_sr_rfc_cb: state: BTA_JV_ST_SR_OPEN, scn:%d,"
                         " user_data:%p", p_cb->scn, p_pcb->user_data);
        break;
    default:
        APPL_TRACE_WARNING("bta_jv_free_sr_rfc_cb():failed, ignore port state:%d, scn:"
                           "%d, p_pcb:%p, jv handle: 0x%x, port_handle: %d, user_data:%p",
                           p_pcb->state, p_cb->scn, p_pcb, p_pcb->handle, p_pcb->port_handle,
                           p_pcb->user_data);
        status = BTA_JV_FAILURE;
        break;
    }
    if (BTA_JV_SUCCESS == status) {
        int port_status;

        if (!remove_server) {
            port_status = RFCOMM_RemoveConnection(p_pcb->port_handle);
        } else {
            port_status = RFCOMM_RemoveServer(p_pcb->port_handle);
        }
        if (port_status != PORT_SUCCESS) {
            status = BTA_JV_FAILURE;
            APPL_TRACE_WARNING("bta_jv_free_rfc_cb(jv handle: 0x%x, state %d)::"
                               "port_status: %d, port_handle: %d, close_pending: %d:Remove",
                               p_pcb->handle, p_pcb->state, port_status, p_pcb->port_handle,
                               close_pending);
        }
    }
    if (!close_pending) {
        p_pcb->port_handle = 0;
        p_pcb->state = BTA_JV_ST_NONE;
        bta_jv_free_set_pm_profile_cb(p_pcb->handle);

        //Initialize congestion flags
        p_pcb->cong = FALSE;
        p_pcb->user_data = 0;
        int si = BTA_JV_RFC_HDL_TO_SIDX(p_pcb->handle);
        if (0 <= si && si < BTA_JV_MAX_RFC_SR_SESSION) {
            p_cb->rfc_hdl[si] = 0;
        }
        p_pcb->handle = 0;
        p_cb->curr_sess--;

        /* only needs a listening port when has a server */
        if (!close_server && (p_cb->max_sess > 1) && (p_cb->scn != 0)) {
            for (i = 0; i < p_cb->max_sess; i++) {
                if (p_cb->rfc_hdl[i] != 0) {
                    p_pcb = &bta_jv_cb.port_cb[p_cb->rfc_hdl[i] - 1];
                    if (p_pcb->state == BTA_JV_ST_SR_LISTEN) {
                        listen++;
                    }
                    used++;
                }
            }
            APPL_TRACE_DEBUG("%s max_sess=%d used:%d curr_sess:%d, listen:%d si:%d", __func__, p_cb->max_sess, used,
                             p_cb->curr_sess, listen, si);
            if (used < p_cb->max_sess &&
                listen == 0 &&
                0 <= si &&
                si < BTA_JV_MAX_RFC_SR_SESSION) {
                /* make sure the server has a listen port */
                if ((RFCOMM_CreateConnection(p_cb->sec_id, p_cb->scn, TRUE,
                                             BTA_JV_DEF_RFC_MTU, (UINT8 *)bd_addr_any, &(p_cb->rfc_hdl[si]), bta_jv_port_mgmt_sr_cback) == PORT_SUCCESS) &&
                    (p_cb->rfc_hdl[si] != 0)) {
                    p_cb->curr_sess++;
                    p_pcb = &bta_jv_cb.port_cb[p_cb->rfc_hdl[si] - 1];
                    p_pcb->state = BTA_JV_ST_SR_LISTEN;
                    p_pcb->port_handle = p_cb->rfc_hdl[si];
                    // p_pcb->user_data = p_pcb_open->user_data;

                    PORT_ClearKeepHandleFlag(p_pcb->port_handle);
                    PORT_SetEventCallback(p_pcb->port_handle, bta_jv_port_event_sr_cback);
                    PORT_SetDataCOCallback(p_pcb->port_handle, bta_jv_port_data_co_cback);
                    PORT_SetEventMask(p_pcb->port_handle, event_mask);
                    PORT_GetState(p_pcb->port_handle, &port_state);

                    port_state.fc_type = (PORT_FC_CTS_ON_INPUT | PORT_FC_CTS_ON_OUTPUT);

                    PORT_SetState(p_pcb->port_handle, &port_state);
                    p_pcb->handle = BTA_JV_RFC_H_S_TO_HDL(p_cb->handle, si);
                    APPL_TRACE_DEBUG("%s: p_pcb->handle:0x%x, curr_sess:%d", __func__,
                                     p_pcb->handle, p_cb->curr_sess);

                    evt_data.rfc_srv_open.handle = 0;
                    evt_data.rfc_srv_open.new_listen_handle = p_pcb->handle;
                    evt_data.rfc_srv_open.status = BTA_JV_SUCCESS;
                    p_pcb->user_data = p_cb->p_cback(BTA_JV_RFCOMM_SRV_OPEN_EVT, &evt_data, (void *)scn_num);
                }
            }
        }

        if (p_cb->curr_sess == 0) {
            p_cb->scn = 0;
            bta_jv_free_sec_id(&p_cb->sec_id);
            p_cb->p_cback = NULL;
            p_cb->handle = 0;
            p_cb->curr_sess = -1;
        }
    }
    return status;
}
#endif /* BTA_JV_RFCOMM_INCLUDED */

#if BTA_JV_L2CAP_INCLUDED
/*******************************************************************************
**
** Function     bta_jv_free_l2c_cb
**
** Description  free the given L2CAP control block
**
** Returns
**
*******************************************************************************/
tBTA_JV_STATUS bta_jv_free_l2c_cb(tBTA_JV_L2C_CB *p_cb)
{
    tBTA_JV_STATUS status = BTA_JV_SUCCESS;

    if (BTA_JV_ST_NONE != p_cb->state) {
        bta_jv_free_set_pm_profile_cb((UINT32)p_cb->handle);
        if (GAP_ConnClose(p_cb->handle) != BT_PASS) {
            status = BTA_JV_FAILURE;
        }
    }
    p_cb->psm = 0;
    p_cb->state = BTA_JV_ST_NONE;
    bta_jv_free_sec_id(&p_cb->sec_id);
    p_cb->p_cback = NULL;
    return status;
}
#endif /* BTA_JV_L2CAP_INCLUDED */

/*******************************************************************************
**
**
** Function    bta_jv_clear_pm_cb
**
** Description clears jv pm control block and optionally calls bta_sys_conn_close()
**             In general close_conn should be set to TRUE to remove registering with
**             dm pm!
**
** WARNING:    Make sure to clear pointer form port or l2c to this control block too!
**
*******************************************************************************/
static void bta_jv_clear_pm_cb(tBTA_JV_PM_CB *p_pm_cb, BOOLEAN close_conn)
{
    /* needs to be called if registered with bta pm, otherwise we may run out of dm pm slots! */
    if (close_conn) {
        bta_sys_conn_close(BTA_ID_JV, p_pm_cb->app_id, p_pm_cb->peer_bd_addr);
    }
    p_pm_cb->state = BTA_JV_PM_FREE_ST;
    p_pm_cb->app_id = BTA_JV_PM_ALL;
    p_pm_cb->handle = BTA_JV_PM_HANDLE_CLEAR;
    bdcpy(p_pm_cb->peer_bd_addr, bd_addr_null);
}

/*******************************************************************************
 **
 ** Function     bta_jv_free_set_pm_profile_cb
 **
 ** Description  free pm profile control block
 **
 ** Returns     BTA_JV_SUCCESS if cb has been freed correctly,
 **             BTA_JV_FAILURE in case of no profile has been registered or already freed
 **
 *******************************************************************************/
static tBTA_JV_STATUS bta_jv_free_set_pm_profile_cb(UINT32 jv_handle)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_PM_CB  **p_cb;
    int i, j, bd_counter = 0, appid_counter = 0;

    for (i = 0; i < BTA_JV_PM_MAX_NUM; i++) {
        p_cb = NULL;
        if ((bta_jv_cb.pm_cb[i].state != BTA_JV_PM_FREE_ST) &&
                (jv_handle == bta_jv_cb.pm_cb[i].handle)) {
            for (j = 0; j < BTA_JV_PM_MAX_NUM; j++) {
                if (bdcmp(bta_jv_cb.pm_cb[j].peer_bd_addr, bta_jv_cb.pm_cb[i].peer_bd_addr) == 0) {
                    bd_counter++;
                }
                if (bta_jv_cb.pm_cb[j].app_id == bta_jv_cb.pm_cb[i].app_id) {
                    appid_counter++;
                }
            }

            APPL_TRACE_API("%s(jv_handle: 0x%2x), idx: %d, app_id: 0x%x", __func__, jv_handle, i, bta_jv_cb.pm_cb[i].app_id);
            APPL_TRACE_API("%s, bd_counter = %d, appid_counter = %d", __func__, bd_counter, appid_counter);
            if (bd_counter > 1) {
                bta_jv_pm_conn_idle(&bta_jv_cb.pm_cb[i]);
            }

            if (bd_counter <= 1 || (appid_counter <= 1)) {
                bta_jv_clear_pm_cb(&bta_jv_cb.pm_cb[i], TRUE);
            } else {
                bta_jv_clear_pm_cb(&bta_jv_cb.pm_cb[i], FALSE);
            }

            if (BTA_JV_RFCOMM_MASK & jv_handle) {
#if BTA_JV_RFCOMM_INCLUDED
                UINT32 hi = ((jv_handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
                UINT32 si = BTA_JV_RFC_HDL_TO_SIDX(jv_handle);
                if (hi < BTA_JV_MAX_RFC_CONN && bta_jv_cb.rfc_cb[hi].p_cback && si
                        < BTA_JV_MAX_RFC_SR_SESSION && bta_jv_cb.rfc_cb[hi].rfc_hdl[si]) {
                    tBTA_JV_PCB *p_pcb = bta_jv_rfc_port_to_pcb(bta_jv_cb.rfc_cb[hi].rfc_hdl[si]);
                    if (p_pcb) {
                        if (NULL == p_pcb->p_pm_cb) {
                            APPL_TRACE_WARNING("%s(jv_handle: 0x%x):port_handle: 0x%x, p_pm_cb: %d: no link to pm_cb?",
                                                 __func__, jv_handle, p_pcb->port_handle, i);
                        }
                        p_cb = &p_pcb->p_pm_cb;
                    }
                }
#endif /* BTA_JV_RFCOMM_INCLUDED */
            }
#if BTA_JV_L2CAP_INCLUDED
            else {
                if (jv_handle < BTA_JV_MAX_L2C_CONN) {
                    tBTA_JV_L2C_CB *p_l2c_cb = &bta_jv_cb.l2c_cb[jv_handle];
                    if (NULL == p_l2c_cb->p_pm_cb) {
                        APPL_TRACE_WARNING("%s(jv_handle: "
                                           "0x%x): p_pm_cb: %d: no link to pm_cb?", __func__, jv_handle, i);
                    }
                    p_cb = &p_l2c_cb->p_pm_cb;
                }
            }
#endif /* BTA_JV_L2CAP_INCLUDED */

            if (p_cb) {
                *p_cb = NULL;
                status = BTA_JV_SUCCESS;
            }
        }
    }
    return status;
}

/*******************************************************************************
 **
 ** Function    bta_jv_alloc_set_pm_profile_cb
 **
 ** Description set PM profile control block
 **
 ** Returns     pointer to allocated cb or NULL in case of failure
 **
 *******************************************************************************/
static tBTA_JV_PM_CB *bta_jv_alloc_set_pm_profile_cb(UINT32 jv_handle, tBTA_JV_PM_ID app_id)
{
    BOOLEAN bRfcHandle = (jv_handle & BTA_JV_RFCOMM_MASK) != 0;
    BD_ADDR peer_bd_addr;
    int i, j;
    tBTA_JV_PM_CB **pp_cb;

    for (i = 0; i < BTA_JV_PM_MAX_NUM; i++) {
        pp_cb = NULL;
        if (bta_jv_cb.pm_cb[i].state == BTA_JV_PM_FREE_ST) {
            /* rfc handle bd addr retrieval requires core stack handle */
            if (bRfcHandle) {
#if BTA_JV_RFCOMM_INCLUDED
                // UINT32 hi = ((jv_handle & BTA_JV_RFC_HDL_MASK) & ~BTA_JV_RFCOMM_MASK) - 1;
                // UINT32 si = BTA_JV_RFC_HDL_TO_SIDX(jv_handle);
                for (j = 0; j < BTA_JV_MAX_RFC_CONN; j++) {
                    if (jv_handle == bta_jv_cb.port_cb[j].handle) {
                        pp_cb = &bta_jv_cb.port_cb[j].p_pm_cb;
                        if (PORT_SUCCESS !=
                            PORT_CheckConnection(bta_jv_cb.port_cb[j].port_handle, FALSE, peer_bd_addr, NULL)) {
                            i = BTA_JV_PM_MAX_NUM;
                        }
                        break;
                    }
                }
#endif /* BTA_JV_RFCOMM_INCLUDED */
            }
#if BTA_JV_L2CAP_INCLUDED
            else {
                /* use jv handle for l2cap bd address retrieval */
                for (j = 0; j < BTA_JV_MAX_L2C_CONN; j++) {
                    if (jv_handle == bta_jv_cb.l2c_cb[j].handle) {
                        pp_cb = &bta_jv_cb.l2c_cb[j].p_pm_cb;
                        UINT8 *p_bd_addr = GAP_ConnGetRemoteAddr((UINT16)jv_handle);
                        if (NULL != p_bd_addr) {
                            bdcpy(peer_bd_addr, p_bd_addr);
                        } else {
                            i = BTA_JV_PM_MAX_NUM;
                        }
                        break;
                    }
                }
            }
#endif /* BTA_JV_L2CAP_INCLUDED */
            APPL_TRACE_API("bta_jv_alloc_set_pm_profile_cb(handle 0x%2x, app_id %d): "
                           "idx: %d, (BTA_JV_PM_MAX_NUM: %d), pp_cb: %p", jv_handle, app_id,
                           i, BTA_JV_PM_MAX_NUM, (void *)pp_cb);
            break;
        }
    }

    if ((i != BTA_JV_PM_MAX_NUM) && (NULL != pp_cb)) {
        *pp_cb = &bta_jv_cb.pm_cb[i];
        bta_jv_cb.pm_cb[i].handle = jv_handle;
        bta_jv_cb.pm_cb[i].app_id = app_id;
        bdcpy(bta_jv_cb.pm_cb[i].peer_bd_addr, peer_bd_addr);
        bta_jv_cb.pm_cb[i].state = BTA_JV_PM_IDLE_ST;
        return &bta_jv_cb.pm_cb[i];
    }
    APPL_TRACE_WARNING("bta_jv_alloc_set_pm_profile_cb(jv_handle: 0x%x, app_id: %d) "
                       "return NULL", jv_handle, app_id);
    return (tBTA_JV_PM_CB *)NULL;
}

/*******************************************************************************
**
** Function     bta_jv_check_psm
**
** Description  for now use only the legal PSM per JSR82 spec
**
** Returns      TRUE, if allowed
**
*******************************************************************************/
BOOLEAN bta_jv_check_psm(UINT16 psm)
{
    BOOLEAN ret = FALSE;

    if (L2C_IS_VALID_PSM(psm)) {
        if (psm < 0x1001) {
            /* see if this is defined by spec */
            switch (psm) {
            case SDP_PSM:           /* 1 */
            case BT_PSM_RFCOMM:     /* 3 */
                /* do not allow java app to use these 2 PSMs */
                break;

            case TCS_PSM_INTERCOM:  /* 5 */
            case TCS_PSM_CORDLESS:  /* 7 */
                if ( FALSE == bta_sys_is_register(BTA_ID_CT) &&
                        FALSE == bta_sys_is_register(BTA_ID_CG) ) {
                    ret = TRUE;
                }
                break;

            case BT_PSM_BNEP:       /* F */
                if (FALSE == bta_sys_is_register(BTA_ID_PAN)) {
                    ret = TRUE;
                }
                break;

            case HID_PSM_CONTROL:   /* 0x11 */
            case HID_PSM_INTERRUPT: /* 0x13 */
                //FIX: allow HID Device and HID Host to coexist
                if ( FALSE == bta_sys_is_register(BTA_ID_HD) ||
                        FALSE == bta_sys_is_register(BTA_ID_HH) ) {
                    ret = TRUE;
                }
                break;

            case AVCT_PSM:          /* 0x17 */
            case AVDT_PSM:          /* 0x19 */
                if ((FALSE == bta_sys_is_register(BTA_ID_AV)) &&
                        (FALSE == bta_sys_is_register(BTA_ID_AVK))) {
                    ret = TRUE;
                }
                break;

            default:
                ret = TRUE;
                break;
            }
        } else {
            ret = TRUE;
        }
    }
    return ret;
}

/*******************************************************************************
**
** Function     bta_jv_enable
**
** Description  Initialises the JAVA I/F
**
** Returns      void
**
*******************************************************************************/
void bta_jv_enable(tBTA_JV_MSG *p_data)
{
    tBTA_UTL_COD   cod;

    tBTA_JV_STATUS status = BTA_JV_SUCCESS;
    bta_jv_cb.p_dm_cback = p_data->enable.p_cback;
    bta_jv_cb.p_dm_cback(BTA_JV_ENABLE_EVT, (tBTA_JV *)&status, 0);
    memset(bta_jv_cb.free_psm_list, 0, sizeof(bta_jv_cb.free_psm_list));

    /* Set the Class of Device */
    cod.major = BTM_COD_MAJOR_UNCLASSIFIED;
    cod.minor = BTM_COD_MINOR_UNCLASSIFIED;
    utl_set_device_class(&cod, BTA_UTL_SET_COD_MAJOR_MINOR);
}

/*******************************************************************************
**
** Function     bta_jv_disable
**
** Description  Disables the BT device manager
**              free the resources used by java
**
** Returns      void
**
*******************************************************************************/
void bta_jv_disable (tBTA_JV_MSG *p_data)
{
    tBTA_JV_STATUS evt_data;
    evt_data = BTA_JV_SUCCESS;
    // UNUSED(p_data);
    if (p_data->disable.p_cback) {
        p_data->disable.p_cback(BTA_JV_DISABLE_EVT, (tBTA_JV *)&evt_data, NULL);
    }
}


/**
 * We keep a list of PSM's that have been freed from JAVA, for reuse.
 * This function will return a free PSM, and delete it from the free
 * list.
 * If no free PSMs exist, 0 will be returned.
 */
static UINT16 bta_jv_get_free_psm(void)
{
    const int cnt = sizeof(bta_jv_cb.free_psm_list) / sizeof(bta_jv_cb.free_psm_list[0]);
    for (int i = 0; i < cnt; i++) {
        UINT16 psm = bta_jv_cb.free_psm_list[i];
        if (psm != 0) {
            APPL_TRACE_DEBUG("%s(): Reusing PSM: 0x%04d", __func__, psm)
            bta_jv_cb.free_psm_list[i] = 0;
            return psm;
        }
    }
    return 0;
}

static void bta_jv_set_free_psm(UINT16 psm)
{
    int free_index = -1;
    const int cnt = sizeof(bta_jv_cb.free_psm_list) / sizeof(bta_jv_cb.free_psm_list[0]);
    for (int i = 0; i < cnt; i++) {
        if (bta_jv_cb.free_psm_list[i] == 0) {
            free_index = i;
        } else if (psm == bta_jv_cb.free_psm_list[i]) {
            return; // PSM already freed?
        }
    }
    if (free_index != -1) {
        bta_jv_cb.free_psm_list[free_index] = psm;
        APPL_TRACE_DEBUG("%s(): Recycling PSM: 0x%04d", __func__, psm)
    } else {
        APPL_TRACE_ERROR("%s unable to free psm 0x%x no more free slots", __func__, psm);
    }
}

/*******************************************************************************
**
** Function     bta_jv_get_channel_id
**
** Description  Obtain a free SCN (Server Channel Number)
**              (RFCOMM channel or L2CAP PSM)
**
** Returns      void
**
*******************************************************************************/
void bta_jv_get_channel_id(tBTA_JV_MSG *p_data)
{
    UINT16   psm = 0;

    switch (p_data->alloc_channel.type) {
    case BTA_JV_CONN_TYPE_RFCOMM: {
        INT32   channel = p_data->alloc_channel.channel;
        UINT8 scn = 0;
        if (channel > 0) {
            if (BTM_TryAllocateSCN(channel) == FALSE) {
                APPL_TRACE_ERROR("rfc channel:%d already in use or invalid", channel);
                channel = 0;
            }
        } else if ((channel = BTM_AllocateSCN()) == 0) {
            APPL_TRACE_ERROR("run out of rfc channels");
            channel = 0;
        }
        if (channel != 0) {
            bta_jv_cb.scn[channel - 1] = TRUE;
            scn = (UINT8) channel;
        }
        if (bta_jv_cb.p_dm_cback) {
            bta_jv_cb.p_dm_cback(BTA_JV_GET_SCN_EVT, (tBTA_JV *)&scn,
                                 p_data->alloc_channel.user_data);
        }
        return;
    }
    case BTA_JV_CONN_TYPE_L2CAP:
        psm = bta_jv_get_free_psm();
        if (psm == 0) {
            psm = L2CA_AllocatePSM();
            APPL_TRACE_DEBUG("%s() returned PSM: 0x%04x", __func__, psm);
        }
        break;
    case BTA_JV_CONN_TYPE_L2CAP_LE:
        break;
    default:
        break;
    }

    if (bta_jv_cb.p_dm_cback) {
        bta_jv_cb.p_dm_cback(BTA_JV_GET_PSM_EVT, (tBTA_JV *)&psm, p_data->alloc_channel.user_data);
    }
}

/*******************************************************************************
**
** Function     bta_jv_free_scn
**
** Description  free a SCN
**
** Returns      void
**
*******************************************************************************/
void bta_jv_free_scn(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_FREE_CHANNEL *fc = &(p_data->free_channel);
    UINT16   scn = fc->scn;
    tBTA_JV_FREE_SCN evt_data = {
        .status = BTA_JV_SUCCESS,
        .server_status = BTA_JV_SERVER_STATUS_MAX,
        .scn = scn
    };

    tBTA_JV_FREE_SCN_USER_DATA *user_data = NULL;
#if BTA_JV_RFCOMM_INCLUDED
    tBTA_JV_RFC_CB *p_cb = NULL;
    tBTA_JV_PCB *p_pcb = NULL;
#endif /* BTA_JV_RFCOMM_INCLUDED */

    switch (p_data->free_channel.type) {
    case BTA_JV_CONN_TYPE_RFCOMM: {
#if BTA_JV_RFCOMM_INCLUDED
        if (scn > 0 && scn <= BTA_JV_MAX_SCN && bta_jv_cb.scn[scn - 1]) {
            /* this scn is used by JV */
            bta_jv_cb.scn[scn - 1] = FALSE;
            BTM_FreeSCN(scn);
        }
        if (fc->user_data) {
            user_data = (tBTA_JV_FREE_SCN_USER_DATA *)fc->user_data;
            evt_data.server_status = user_data->server_status;
            if (user_data->server_status == BTA_JV_SERVER_RUNNING && find_rfc_pcb((void *)user_data->slot_id, &p_cb, &p_pcb)) {
                /* if call bta_jv_rfcomm_stop_server successfully, find_rfc_pcb shall return false */
                evt_data.status = BTA_JV_FAILURE;
            }

            if (fc->p_cback) {
                fc->p_cback(BTA_JV_FREE_SCN_EVT, (tBTA_JV *)&evt_data, (void *)user_data);
            }
        }
#endif /* BTA_JV_RFCOMM_INCLUDED */
        break;
    }
    case BTA_JV_CONN_TYPE_L2CAP:
        bta_jv_set_free_psm(scn);
        user_data = (tBTA_JV_FREE_SCN_USER_DATA *)fc->user_data;
        if (fc->p_cback) {
            fc->p_cback(BTA_JV_FREE_SCN_EVT, (tBTA_JV *)&evt_data, (void *)user_data);
        }
        break;
    case BTA_JV_CONN_TYPE_L2CAP_LE:
        // TODO: Not yet implemented...
        break;
    default:
        break;
    }
}
static inline tBT_UUID shorten_sdp_uuid(const tBT_UUID *u)
{
    static uint8_t bt_base_uuid[] =
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB };

    logu("in, uuid:", u->uu.uuid128);
    APPL_TRACE_DEBUG("uuid len:%d", u->len);
    if (u->len == 16) {
        if (memcmp(&u->uu.uuid128[4], &bt_base_uuid[4], 12) == 0) {
            tBT_UUID su;
            memset(&su, 0, sizeof(su));
            if (u->uu.uuid128[0] == 0 && u->uu.uuid128[1] == 0) {
                su.len = 2;
                uint16_t u16;
                memcpy(&u16, &u->uu.uuid128[2], sizeof(u16));
                su.uu.uuid16 = ntohs(u16);
                APPL_TRACE_DEBUG("shorten to 16 bits uuid: %x", su.uu.uuid16);
            } else {
                su.len = 4;
                uint32_t u32;
                memcpy(&u32, &u->uu.uuid128[0], sizeof(u32));
                su.uu.uuid32 = ntohl(u32);
                APPL_TRACE_DEBUG("shorten to 32 bits uuid: %x", su.uu.uuid32);
            }
            return su;
        }
    }
    APPL_TRACE_DEBUG("cannot shorten none-reserved 128 bits uuid");
    return *u;
}

/*******************************************************************************
**
** Function     bta_jv_start_discovery_cback
**
** Description  Callback for Start Discovery
**
** Returns      void
**
*******************************************************************************/
static void bta_jv_start_discovery_cback(UINT16 result, void *user_data)
{
    tBTA_JV_STATUS status;
    // UINT8          old_sdp_act = bta_jv_cb.sdp_active;

    APPL_TRACE_DEBUG("bta_jv_start_discovery_cback res: 0x%x", result);

    bta_jv_cb.sdp_active = BTA_JV_SDP_ACT_NONE;
    if (bta_jv_cb.p_dm_cback) {
        tBTA_JV_DISCOVERY_COMP dcomp;
        dcomp.scn_num = 0;
        status = BTA_JV_FAILURE;
        if (result == SDP_SUCCESS || result == SDP_DB_FULL) {
            tSDP_DISC_REC       *p_sdp_rec = NULL;
            tSDP_DISC_ATTR *p_attr = NULL;
            tSDP_PROTOCOL_ELEM  pe;
            logu("bta_jv_cb.uuid", bta_jv_cb.uuid.uu.uuid128);
            tBT_UUID su = shorten_sdp_uuid(&bta_jv_cb.uuid);
            logu("shorten uuid:", su.uu.uuid128);
            do{
                p_sdp_rec = SDP_FindServiceUUIDInDb(p_bta_jv_cfg->p_sdp_db, &su, p_sdp_rec);
                APPL_TRACE_DEBUG("p_sdp_rec:%p", p_sdp_rec);
                if (p_sdp_rec && SDP_FindProtocolListElemInRec(p_sdp_rec, UUID_PROTOCOL_RFCOMM, &pe)){
                    dcomp.scn[dcomp.scn_num] = (UINT8) pe.params[0];
                    if ((p_attr = SDP_FindAttributeInRec(p_sdp_rec, ATTR_ID_SERVICE_NAME)) != NULL) {
                        dcomp.service_name[dcomp.scn_num] = (char *)p_attr->attr_value.v.array;
                    } else {
                        dcomp.service_name[dcomp.scn_num] = NULL;
                    }
                    dcomp.scn_num++;
                    status = BTA_JV_SUCCESS;
                }
            } while (p_sdp_rec);
        }

        dcomp.status = status;
        bta_jv_cb.p_dm_cback(BTA_JV_DISCOVERY_COMP_EVT, (tBTA_JV *)&dcomp, user_data);
    }
}

/*******************************************************************************
**
** Function     bta_jv_start_discovery
**
** Description  Discovers services on a remote device
**
** Returns      void
**
*******************************************************************************/
void bta_jv_start_discovery(tBTA_JV_MSG *p_data)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    APPL_TRACE_DEBUG("bta_jv_start_discovery in, sdp_active:%d", bta_jv_cb.sdp_active);
    if (bta_jv_cb.sdp_active != BTA_JV_SDP_ACT_NONE) {
        /* SDP is still in progress */
        status = BTA_JV_BUSY;
        if (bta_jv_cb.p_dm_cback) {
            bta_jv_cb.p_dm_cback(BTA_JV_DISCOVERY_COMP_EVT, (tBTA_JV *)&status, p_data->start_discovery.user_data);
        }
        return;
    }

    /* init the database/set up the filter */
    APPL_TRACE_DEBUG("call SDP_InitDiscoveryDb, p_data->start_discovery.num_uuid:%d",
                     p_data->start_discovery.num_uuid);
    SDP_InitDiscoveryDb (p_bta_jv_cfg->p_sdp_db, p_bta_jv_cfg->sdp_db_size,
                         p_data->start_discovery.num_uuid, p_data->start_discovery.uuid_list, 0, NULL);

    /* tell SDP to keep the raw data */
    p_bta_jv_cfg->p_sdp_db->raw_data = p_bta_jv_cfg->p_sdp_raw_data;
    p_bta_jv_cfg->p_sdp_db->raw_size = p_bta_jv_cfg->sdp_raw_size;

    bta_jv_cb.p_sel_raw_data     = 0;
    bta_jv_cb.uuid = p_data->start_discovery.uuid_list[0];

    bta_jv_cb.sdp_active = BTA_JV_SDP_ACT_YES;
    if (!SDP_ServiceSearchAttributeRequest2(p_data->start_discovery.bd_addr,
                                            p_bta_jv_cfg->p_sdp_db,
                                            bta_jv_start_discovery_cback, p_data->start_discovery.user_data)) {
        bta_jv_cb.sdp_active = BTA_JV_SDP_ACT_NONE;
        /* failed to start SDP. report the failure right away */
        if (bta_jv_cb.p_dm_cback) {
            bta_jv_cb.p_dm_cback(BTA_JV_DISCOVERY_COMP_EVT, (tBTA_JV *)&status, p_data->start_discovery.user_data);
        }
    }
    /*
    else report the result when the cback is called
    */
}

/*******************************************************************************
**
** Function     sdp_bqb_add_language_attr_ctrl
**
** Description  Control adding of the language attribute for SDP BQB test
**
** Returns      void
**
*******************************************************************************/
#if BT_SDP_BQB_INCLUDED
void sdp_bqb_add_language_attr_ctrl(BOOLEAN enable)
{
    s_sdp_bqb_add_language_attr_flag = enable;
}
#endif /* BT_SDP_BQB_INCLUDED */

/**
 * @brief       Adds a protocol list and service name (if provided) to an SDP record given by
 *              sdp_handle, and marks it as browsable. This is a shortcut for defining a
 *              set of protocols that includes L2CAP, RFCOMM, and optionally OBEX.
 *
 * @param[in]   sdp_handle: SDP handle
 * @param[in]   name:       service name
 * @param[in]   channel:    channel
 * @param[in]   with_obex:  if TRUE, then an additional OBEX protocol UUID will be included
 *                          at the end of the protocol list.
 * @return
 *              - TRUE : success
 *              - other  : failed
 */
static bool create_base_record(const uint32_t sdp_handle, const char *name, const uint16_t channel, const bool with_obex){
    APPL_TRACE_DEBUG("create_base_record: scn: %d, name: %s, with_obex: %d",
                   channel, name, with_obex);

    // Setup the protocol list and add it.
    tSDP_PROTOCOL_ELEM proto_list[SDP_MAX_LIST_ELEMS];
    int num_proto_elements = with_obex ? 3 : 2;

    memset(proto_list, 0, num_proto_elements * sizeof(tSDP_PROTOCOL_ELEM));

    proto_list[0].protocol_uuid = UUID_PROTOCOL_L2CAP;
    proto_list[0].num_params = 0;
    proto_list[1].protocol_uuid = UUID_PROTOCOL_RFCOMM;
    proto_list[1].num_params = 1;
    proto_list[1].params[0] = channel;

    if (with_obex == TRUE) {
        proto_list[2].protocol_uuid = UUID_PROTOCOL_OBEX;
        proto_list[2].num_params = 0;
    }

    const char *stage = "protocol_list";
    if (!SDP_AddProtocolList(sdp_handle, num_proto_elements, proto_list)){
        APPL_TRACE_ERROR("create_base_record: failed to create base service "
                   "record, stage: %s, scn: %d, name: %s, with_obex: %d",
                   stage, channel, name, with_obex);
        return FALSE;
    }

    stage = "profile_descriptor_list";
    if (!SDP_AddProfileDescriptorList(sdp_handle, UUID_SERVCLASS_SERIAL_PORT, SPP_VERSION)){
        APPL_TRACE_ERROR("create_base_record: failed to create base service "
                   "record, stage: %s, scn: %d, name: %s, with_obex: %d",
                   stage, channel, name, with_obex);
        return FALSE;
    }

#if BT_SDP_BQB_INCLUDED
    // SDP/SR/SA/BV-09-C,SDP/SR/SSA/BV-13-C
    if (s_sdp_bqb_add_language_attr_flag == TRUE) {
        stage = "language_base";
        if (!SDP_AddLanguageBaseAttrIDList(sdp_handle, LANG_ID_CODE_ENGLISH, LANG_ID_CHAR_ENCODE_UTF8, LANGUAGE_BASE_ID)) {
            APPL_TRACE_ERROR("create_base_record: failed to create base service "
                    "record, stage: %s, scn: %d, name: %s, with_obex: %d",
                    stage, channel, name, with_obex);
            return FALSE;
        }
    }
#endif /* BT_SDP_BQB_INCLUDED */

    // Add the name to the SDP record.
    if (name[0] != '\0') {
        stage = "service_name";
        if (!SDP_AddAttribute(sdp_handle, ATTR_ID_SERVICE_NAME,
                          TEXT_STR_DESC_TYPE, (uint32_t)(strlen(name) + 1),
                          (uint8_t *)name)){
            APPL_TRACE_ERROR("create_base_record: failed to create base service "
                       "record, stage: %s, scn: %d, name: %s, with_obex: %d",
                       stage, channel, name, with_obex);
            return FALSE;
        }
    }

    // Mark the service as browsable.
    uint16_t list = UUID_SERVCLASS_PUBLIC_BROWSE_GROUP;
    stage = "browsable";
    if (!SDP_AddUuidSequence(sdp_handle, ATTR_ID_BROWSE_GROUP_LIST, 1, &list)){
        APPL_TRACE_ERROR("create_base_record: failed to create base service "
                   "record, stage: %s, scn: %d, name: %s, with_obex: %d",
                   stage, channel, name, with_obex);
        return FALSE;
    }


    APPL_TRACE_DEBUG("create_base_record: successfully created base service "
                   "record, handle: 0x%08x, scn: %d, name: %s, with_obex: %d",
                   sdp_handle, channel, name, with_obex);

    UNUSED(stage);

    return TRUE;
}

static int add_spp_sdp(const char *name, const int channel) {
    APPL_TRACE_DEBUG("add_spp_sdp: scn %d, service_name %s", channel, name);

    int handle = SDP_CreateRecord();
    if (handle == 0) {
        APPL_TRACE_ERROR("add_spp_sdp: failed to create sdp record, "
                     "service_name: %s", name);
        return 0;
    }

    // Create the base SDP record.
    const char *stage = "create_base_record";
    if (!create_base_record(handle, name, channel, FALSE /* with_obex */)){
        SDP_DeleteRecord(handle);
        APPL_TRACE_ERROR("add_spp_sdp: failed to register SPP service, "
                   "stage: %s, service_name: %s", stage, name);
        return 0;
    }

    uint16_t service = UUID_SERVCLASS_SERIAL_PORT;
    stage = "service_class";
    if (!SDP_AddServiceClassIdList(handle, 1, &service)){
        SDP_DeleteRecord(handle);
        APPL_TRACE_ERROR("add_spp_sdp: failed to register SPP service, "
                   "stage: %s, service_name: %s", stage, name);
        return 0;
    }

    APPL_TRACE_DEBUG("add_spp_sdp: service registered successfully, "
                   "service_name: %s, handle 0x%08x)", name, handle);
    UNUSED(stage);

    return handle;
}

/*******************************************************************************
**
** Function     bta_jv_create_record
**
** Description  Create an SDP record with the given attributes
**
** Returns      void
**
*******************************************************************************/
void bta_jv_create_record(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_CREATE_RECORD *cr = &(p_data->create_record);
    tBTA_JV_CREATE_RECORD   evt_data;

    int handle = add_spp_sdp(cr->name, cr->channel);
    evt_data.handle = handle;
    if (handle) {
        evt_data.status = BTA_JV_SUCCESS;
    } else {
        evt_data.status = BTA_JV_FAILURE;
    }

    if(bta_jv_cb.p_dm_cback) {
        //callback user immediately to create his own sdp record in stack thread context
        bta_jv_cb.p_dm_cback(BTA_JV_CREATE_RECORD_EVT, (tBTA_JV *)&evt_data, cr->user_data);
    }
}

/*******************************************************************************
**
** Function     bta_jv_delete_record
**
** Description  Delete an SDP record
**
**
** Returns      void
**
*******************************************************************************/
void bta_jv_delete_record(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_ADD_ATTRIBUTE *dr = &(p_data->add_attr);
    if (dr->handle) {
        /* this is a record created by btif layer*/
        SDP_DeleteRecord(dr->handle);
    }
}

#if BTA_JV_L2CAP_INCLUDED
/*******************************************************************************
**
** Function     bta_jv_l2cap_client_cback
**
** Description  handles the l2cap client events
**
** Returns      void
**
*******************************************************************************/
static void bta_jv_l2cap_client_cback(UINT16 gap_handle, UINT16 event)
{
    if (gap_handle >= BTA_JV_MAX_L2C_CONN) {
        APPL_TRACE_WARNING("Invalid gap_handle: %u", gap_handle);
        return;
    }

    tBTA_JV_L2C_CB  *p_cb = &bta_jv_cb.l2c_cb[gap_handle];
    tBTA_JV evt_data = {0};

    if (!p_cb->p_cback) {
        return;
    }

    APPL_TRACE_DEBUG( "%s: %d evt:x%x", __func__, gap_handle, event);
    evt_data.l2c_open.status = BTA_JV_SUCCESS;
    evt_data.l2c_open.handle = gap_handle;

    switch (event) {
    case GAP_EVT_CONN_OPENED:
        bdcpy(evt_data.l2c_open.rem_bda, GAP_ConnGetRemoteAddr(gap_handle));
        evt_data.l2c_open.tx_mtu = GAP_ConnGetRemMtuSize(gap_handle);
        p_cb->state = BTA_JV_ST_CL_OPEN;
        p_cb->p_cback(BTA_JV_L2CAP_OPEN_EVT, &evt_data, p_cb->user_data);
        break;

    case GAP_EVT_CONN_CLOSED:
        p_cb->state = BTA_JV_ST_NONE;
        bta_jv_free_sec_id(&p_cb->sec_id);
        evt_data.l2c_close.async = TRUE;
        p_cb->p_cback(BTA_JV_L2CAP_CLOSE_EVT, &evt_data, p_cb->user_data);
        p_cb->p_cback = NULL;
        break;

    case GAP_EVT_CONN_DATA_AVAIL:
        evt_data.data_ind.handle = gap_handle;
        /* Reset idle timer to avoid requesting sniff mode while receiving data */
        bta_jv_pm_conn_busy(p_cb->p_pm_cb);
        p_cb->p_cback(BTA_JV_L2CAP_DATA_IND_EVT, &evt_data, p_cb->user_data);
        bta_jv_pm_conn_idle(p_cb->p_pm_cb);
        break;

    case GAP_EVT_CONN_CONGESTED:
    case GAP_EVT_CONN_UNCONGESTED:
        p_cb->cong = (event == GAP_EVT_CONN_CONGESTED) ? TRUE : FALSE;
        evt_data.l2c_cong.cong = p_cb->cong;
        p_cb->p_cback(BTA_JV_L2CAP_CONG_EVT, &evt_data, p_cb->user_data);
        break;

    default:
        break;
    }
}

/*******************************************************************************
**
** Function     bta_jv_l2cap_connect
**
** Description  makes an l2cap client connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_connect(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2C_CB      *p_cb;
    tBTA_JV_L2CAP_CL_INIT  evt_data;
    UINT16  handle = GAP_INVALID_HANDLE;
    UINT8   sec_id;
    tL2CAP_CFG_INFO cfg;
    tBTA_JV_API_L2CAP_CONNECT *cc = &(p_data->l2cap_connect);
    UINT8 chan_mode_mask = GAP_FCR_CHAN_OPT_BASIC;
    tL2CAP_ERTM_INFO    *ertm_info = NULL;

    memset(&cfg, 0, sizeof(tL2CAP_CFG_INFO));

    if (cc->has_cfg == TRUE) {
        cfg = cc->cfg;
        if (cfg.fcr_present && cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
            chan_mode_mask |= GAP_FCR_CHAN_OPT_ERTM;
        }
    }

    if (cc->has_ertm_info == TRUE) {
        ertm_info = &(cc->ertm_info);
    }

    /* We need to use this value for MTU to be able to handle cases where cfg is not set in req. */
    cfg.mtu_present = TRUE;
    cfg.mtu = cc->rx_mtu;

    /* TODO: DM role manager
    L2CA_SetDesireRole(cc->role);
    */

    sec_id = bta_jv_alloc_sec_id();
    evt_data.sec_id = sec_id;
    evt_data.status = BTA_JV_FAILURE;

    if (sec_id) {
        if (bta_jv_check_psm(cc->remote_psm)) { /* allowed */
            if ((handle = GAP_ConnOpen("", sec_id, 0, cc->peer_bd_addr, cc->remote_psm,
                                       &cfg, ertm_info, cc->sec_mask, chan_mode_mask,
                                       bta_jv_l2cap_client_cback)) != GAP_INVALID_HANDLE ) {
                evt_data.status = BTA_JV_SUCCESS;
            }
        }
    }

    if (evt_data.status == BTA_JV_SUCCESS) {
        p_cb = &bta_jv_cb.l2c_cb[handle];
        p_cb->handle = handle;
        p_cb->p_cback = cc->p_cback;
        p_cb->user_data = cc->user_data;
        p_cb->psm = 0;  /* not a server */
        p_cb->sec_id = sec_id;
        p_cb->state = BTA_JV_ST_CL_OPENING;
    } else {
        bta_jv_free_sec_id(&sec_id);
    }

    evt_data.handle = handle;
    cc->p_cback(BTA_JV_L2CAP_CL_INIT_EVT, (tBTA_JV *)&evt_data, cc->user_data);
}


/*******************************************************************************
**
** Function     bta_jv_l2cap_close
**
** Description  Close an L2CAP client connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_close(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2CAP_CLOSE  evt_data;
    tBTA_JV_API_L2CAP_CLOSE *cc = &(p_data->l2cap_close);
    void *user_data = cc->user_data;

    evt_data.handle = cc->handle;
    evt_data.status = bta_jv_free_l2c_cb(cc->p_cb);
    evt_data.async = FALSE;
    evt_data.user_data = (void *)cc->user_data;

    if (cc->p_cback) {
        cc->p_cback(BTA_JV_L2CAP_CLOSE_EVT, (tBTA_JV *)&evt_data, user_data);
    }
}

/*******************************************************************************
**
** Function         bta_jv_l2cap_server_cback
**
** Description      handles the l2cap server callback
**
** Returns          void
**
*******************************************************************************/
static void bta_jv_l2cap_server_cback(UINT16 gap_handle, UINT16 event)
{
    if (gap_handle >= BTA_JV_MAX_L2C_CONN) {
        APPL_TRACE_WARNING("Invalid gap_handle: %u", gap_handle);
        return;
    }

    tBTA_JV_L2C_CB  *p_cb = &bta_jv_cb.l2c_cb[gap_handle];
    tBTA_JV evt_data = {0};
    tBTA_JV_L2CAP_CBACK *p_cback;
    void *user_data;

    if (!p_cb->p_cback) {
        return;
    }

    APPL_TRACE_DEBUG( "%s: %d evt:x%x", __func__, gap_handle, event);
    evt_data.l2c_open.status = BTA_JV_SUCCESS;
    evt_data.l2c_open.handle = gap_handle;

    switch (event) {
    case GAP_EVT_CONN_OPENED:
        bdcpy(evt_data.l2c_open.rem_bda, GAP_ConnGetRemoteAddr(gap_handle));
        evt_data.l2c_open.tx_mtu = GAP_ConnGetRemMtuSize(gap_handle);
        p_cb->state = BTA_JV_ST_SR_OPEN;
        p_cb->p_cback(BTA_JV_L2CAP_OPEN_EVT, &evt_data, p_cb->user_data);
        break;

    case GAP_EVT_CONN_CLOSED:
        evt_data.l2c_close.async = TRUE;
        evt_data.l2c_close.handle = p_cb->handle;
        p_cback = p_cb->p_cback;
        user_data = p_cb->user_data;
        evt_data.l2c_close.status = bta_jv_free_l2c_cb(p_cb);
        p_cback(BTA_JV_L2CAP_CLOSE_EVT, &evt_data, user_data);
        break;

    case GAP_EVT_CONN_DATA_AVAIL:
        evt_data.data_ind.handle = gap_handle;
        /* Reset idle timer to avoid requesting sniff mode while receiving data */
        bta_jv_pm_conn_busy(p_cb->p_pm_cb);
        p_cb->p_cback(BTA_JV_L2CAP_DATA_IND_EVT, &evt_data, p_cb->user_data);
        bta_jv_pm_conn_idle(p_cb->p_pm_cb);
        break;

    case GAP_EVT_CONN_CONGESTED:
    case GAP_EVT_CONN_UNCONGESTED:
        p_cb->cong = (event == GAP_EVT_CONN_CONGESTED) ? TRUE : FALSE;
        evt_data.l2c_cong.cong = p_cb->cong;
        p_cb->p_cback(BTA_JV_L2CAP_CONG_EVT, &evt_data, p_cb->user_data);
        break;

    default:
        break;
    }
}

/*******************************************************************************
**
** Function     bta_jv_l2cap_start_server
**
** Description  starts an L2CAP server
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_start_server(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2C_CB      *p_cb;
    UINT8   sec_id;
    UINT16  handle;
    tL2CAP_CFG_INFO cfg;
    tBTA_JV_L2CAP_START evt_data;
    tBTA_JV_API_L2CAP_SERVER *ls = &(p_data->l2cap_server);
    // INT32   use_etm = FALSE;
    UINT8 chan_mode_mask = GAP_FCR_CHAN_OPT_BASIC;
    tL2CAP_ERTM_INFO    *ertm_info = NULL;

    memset(&cfg, 0, sizeof(tL2CAP_CFG_INFO));

    if (ls->has_cfg == TRUE) {
        cfg = ls->cfg;
        if (cfg.fcr_present && cfg.fcr.mode == L2CAP_FCR_ERTM_MODE) {
            chan_mode_mask |= GAP_FCR_CHAN_OPT_ERTM;
        }
    }

    if (ls->has_ertm_info == TRUE) {
        ertm_info = &(ls->ertm_info);
    }

    //FIX: MTU=0 means not present
    if (ls->rx_mtu > 0) {
        cfg.mtu_present = TRUE;
        cfg.mtu = ls->rx_mtu;
    } else {
        cfg.mtu_present = FALSE;
        cfg.mtu = 0;
    }

    /* TODO DM role manager
    L2CA_SetDesireRole(ls->role);
    */

    sec_id = bta_jv_alloc_sec_id();
    if (0 == sec_id || (FALSE == bta_jv_check_psm(ls->local_psm)) ||
            (handle = GAP_ConnOpen("JV L2CAP", sec_id, 1, 0, ls->local_psm, &cfg, ertm_info,
                                   ls->sec_mask, chan_mode_mask, bta_jv_l2cap_server_cback)) == GAP_INVALID_HANDLE) {
        bta_jv_free_sec_id(&sec_id);
        evt_data.status = BTA_JV_FAILURE;
    } else {
        p_cb = &bta_jv_cb.l2c_cb[handle];
        evt_data.status = BTA_JV_SUCCESS;
        evt_data.handle = handle;
        evt_data.sec_id = sec_id;
        p_cb->p_cback = ls->p_cback;
        p_cb->user_data = ls->user_data;
        p_cb->handle = handle;
        p_cb->sec_id = sec_id;
        p_cb->state = BTA_JV_ST_SR_LISTEN;
        p_cb->psm = ls->local_psm;
    }

    ls->p_cback(BTA_JV_L2CAP_START_EVT, (tBTA_JV *)&evt_data, ls->user_data);
}

/*******************************************************************************
**
** Function     bta_jv_l2cap_stop_server
**
** Description  stops an L2CAP server
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_stop_server(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2C_CB      *p_cb;
    tBTA_JV_API_L2CAP_SERVER *ls = &(p_data->l2cap_server);

    for (int i = 0; i < BTA_JV_MAX_L2C_CONN; i++) {
        if (bta_jv_cb.l2c_cb[i].psm == ls->local_psm) {
            p_cb = &bta_jv_cb.l2c_cb[i];
            bta_jv_free_l2c_cb(p_cb);
            // Report event when free psm
            break;
        }
    }
}



/*******************************************************************************
**
** Function     bta_jv_l2cap_read
**
** Description  Read data from an L2CAP connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_read(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2CAP_READ evt_data;
    tBTA_JV_API_L2CAP_READ *rc = &(p_data->l2cap_read);

    evt_data.status = BTA_JV_FAILURE;
    evt_data.handle = rc->handle;
    evt_data.req_id = rc->req_id;
    evt_data.p_data = rc->p_data;
    evt_data.len    = 0;

    if (BT_PASS == GAP_ConnReadData(rc->handle, rc->p_data, rc->len, &evt_data.len)) {
        evt_data.status = BTA_JV_SUCCESS;
    }

    rc->p_cback(BTA_JV_L2CAP_READ_EVT, (tBTA_JV *)&evt_data, rc->user_data);
}


/*******************************************************************************
**
** Function     bta_jv_l2cap_write
**
** Description  Write data to an L2CAP connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_write(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2CAP_WRITE evt_data;
    tBTA_JV_API_L2CAP_WRITE *ls = &(p_data->l2cap_write);

    /* As we check this callback exists before the tBTA_JV_API_L2CAP_WRITE can be send through the
     * API this check should not be needed.
     * But the API is not designed to be used (safely at least) in a multi-threaded scheduler, hence
     * if the peer device disconnects the l2cap link after the API is called, but before this
     * message is handled, the ->p_cback will be cleared at this point. At first glanch this seems
     * highly unlikely, but for all obex-profiles with two channels connected - e.g. MAP, this
     * happens around 1 of 4 disconnects, as a disconnect on the server channel causes a disconnect
     * to be send on the client (notification) channel, but at the peer typically disconnects both
     * the OBEX disconnect request crosses the incoming l2cap disconnect.
     * If p_cback is cleared, we simply discard the data.
     * RISK: The caller must handle any cleanup based on another signal than BTA_JV_L2CAP_WRITE_EVT,
     *       which is typically not possible, as the pointer to the allocated buffer is stored
     *       in this message, and can therefore not be freed, hence we have a mem-leak-by-design.*/
    if (ls->p_cb->p_cback != NULL) {
        evt_data.status = BTA_JV_FAILURE;
        evt_data.handle = ls->handle;
        evt_data.req_id = ls->req_id;
        evt_data.cong   = ls->p_cb->cong;
        evt_data.len    = 0;
        bta_jv_pm_conn_busy(ls->p_cb->p_pm_cb);
        if (!evt_data.cong &&
                BT_PASS == GAP_ConnWriteData(ls->handle, ls->p_data, ls->len, &evt_data.len)) {
            evt_data.status = BTA_JV_SUCCESS;
        }
        ls->p_cb->p_cback(BTA_JV_L2CAP_WRITE_EVT, (tBTA_JV *)&evt_data, ls->user_data);
        bta_jv_set_pm_conn_state(ls->p_cb->p_pm_cb, BTA_JV_CONN_IDLE);
    } else {
        /* As this pointer is checked in the API function, this occurs only when the channel is
         * disconnected after the API function is called, but before the message is handled. */
        APPL_TRACE_ERROR("%s() ls->p_cb->p_cback == NULL", __func__);
    }
}

/*******************************************************************************
**
** Function     bta_jv_l2cap_write_fixed
**
** Description  Write data to an L2CAP connection using Fixed channels
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_write_fixed(tBTA_JV_MSG *p_data)
{
    tBTA_JV_L2CAP_WRITE_FIXED evt_data;
    tBTA_JV_API_L2CAP_WRITE_FIXED *ls = &(p_data->l2cap_write_fixed);
    BT_HDR *msg = (BT_HDR *)osi_malloc(sizeof(BT_HDR) + ls->len + L2CAP_MIN_OFFSET);
    if (!msg) {
        APPL_TRACE_ERROR("%s() could not allocate msg buffer", __func__);
        return;
    }
    evt_data.status  = BTA_JV_FAILURE;
    evt_data.channel = ls->channel;
    memcpy(evt_data.addr, ls->addr, sizeof(evt_data.addr));
    evt_data.req_id  = ls->req_id;
    evt_data.len     = 0;


    memcpy(((uint8_t *)(msg + 1)) + L2CAP_MIN_OFFSET, ls->p_data, ls->len);
    msg->len = ls->len;
    msg->offset = L2CAP_MIN_OFFSET;

    L2CA_SendFixedChnlData(ls->channel, ls->addr, msg);

    ls->p_cback(BTA_JV_L2CAP_WRITE_FIXED_EVT, (tBTA_JV *)&evt_data, ls->user_data);
}
#endif /* BTA_JV_L2CAP_INCLUDED */

#if BTA_JV_RFCOMM_INCLUDED
/*******************************************************************************
**
** Function     bta_jv_port_data_co_cback
**
** Description  port data callback function of rfcomm
**              connections
**
** Returns      void
**
*******************************************************************************/
static int bta_jv_port_data_co_cback(UINT16 port_handle, UINT8 *buf, UINT16 len, int type)
{
    tBTA_JV_RFC_CB  *p_cb = bta_jv_rfc_port_to_cb(port_handle);
    tBTA_JV_PCB     *p_pcb = bta_jv_rfc_port_to_pcb(port_handle);
    int ret = 0;
    APPL_TRACE_DEBUG("%s, p_cb:%p, p_pcb:%p, len:%d, type:%d", __func__, p_cb, p_pcb, len, type);
    UNUSED(p_cb);
    if (p_pcb != NULL) {
        switch (type) {
        case DATA_CO_CALLBACK_TYPE_INCOMING:
            bta_jv_pm_conn_busy(p_pcb->p_pm_cb);
            ret = bta_co_rfc_data_incoming(p_pcb->user_data, (BT_HDR *)buf);
            bta_jv_pm_conn_idle(p_pcb->p_pm_cb);
            return ret;
        case DATA_CO_CALLBACK_TYPE_OUTGOING_SIZE:
            return bta_co_rfc_data_outgoing_size(p_pcb->user_data, (int *)buf);
        case DATA_CO_CALLBACK_TYPE_OUTGOING:
            return bta_co_rfc_data_outgoing(p_pcb->user_data, buf, len);
        default:
            APPL_TRACE_ERROR("unknown callout type:%d", type);
            break;
        }
    }
    return 0;
}

/*******************************************************************************
**
** Function     bta_jv_port_mgmt_cl_cback
**
** Description  callback for port mamangement function of rfcomm
**              client connections
**
** Returns      void
**
*******************************************************************************/
static void bta_jv_port_mgmt_cl_cback(UINT32 code, UINT16 port_handle, void* data)
{
    tBTA_JV_RFC_CB  *p_cb = bta_jv_rfc_port_to_cb(port_handle);
    tBTA_JV_PCB     *p_pcb = bta_jv_rfc_port_to_pcb(port_handle);
    tBTA_JV evt_data = {0};
    BD_ADDR rem_bda = {0};
    UINT16 lcid;
    tBTA_JV_RFCOMM_CBACK *p_cback;  /* the callback function */
    tPORT_MGMT_CL_CALLBACK_ARG *p_mgmt_cb_arg = (tPORT_MGMT_CL_CALLBACK_ARG *)data;
    void *user_data = NULL;

    APPL_TRACE_DEBUG( "bta_jv_port_mgmt_cl_cback:code:%d, port_handle%d", code, port_handle);
    if (NULL == p_cb || NULL == p_cb->p_cback) {
        return;
    }

    APPL_TRACE_DEBUG( "bta_jv_port_mgmt_cl_cback code=%d port_handle:%d handle:%d",
                      code, port_handle, p_cb->handle);

    PORT_CheckConnection(port_handle, FALSE, rem_bda, &lcid);

    if (code == PORT_SUCCESS) {
        evt_data.rfc_open.handle = p_pcb->handle;
        evt_data.rfc_open.status = BTA_JV_SUCCESS;
        bdcpy(evt_data.rfc_open.rem_bda, rem_bda);
        p_pcb->state = BTA_JV_ST_CL_OPEN;
        if (p_mgmt_cb_arg) {
            evt_data.rfc_open.peer_mtu = p_mgmt_cb_arg->peer_mtu;
        }
        p_cb->p_cback(BTA_JV_RFCOMM_OPEN_EVT, &evt_data, p_pcb->user_data);
    } else {
        evt_data.rfc_close.handle = p_pcb->handle;
        evt_data.rfc_close.status = BTA_JV_FAILURE;
        evt_data.rfc_close.port_status = code;
        evt_data.rfc_close.async = TRUE;
        evt_data.rfc_close.user_data = p_pcb->user_data;
        if (p_pcb->state == BTA_JV_ST_CL_CLOSING) {
            evt_data.rfc_close.async = FALSE;
            evt_data.rfc_close.status = BTA_JV_SUCCESS;
        }
        p_cback = p_cb->p_cback;
        user_data = p_pcb->user_data;

        // To free up resources.
        p_pcb->state = BTA_JV_ST_CL_CLOSING;
        bta_jv_free_rfc_cb(p_cb, p_pcb, FALSE, FALSE);
        p_cback(BTA_JV_RFCOMM_CLOSE_EVT, &evt_data, user_data);
    }
}

/*******************************************************************************
**
** Function     bta_jv_port_event_cl_cback
**
** Description  Callback for RFCOMM client port events
**
** Returns      void
**
*******************************************************************************/
static void bta_jv_port_event_cl_cback(UINT32 code, UINT16 port_handle)
{
    tBTA_JV_RFC_CB  *p_cb = bta_jv_rfc_port_to_cb(port_handle);
    tBTA_JV_PCB     *p_pcb = bta_jv_rfc_port_to_pcb(port_handle);
    tBTA_JV evt_data = {0};

    APPL_TRACE_DEBUG( "bta_jv_port_event_cl_cback:%d", port_handle);
    if (NULL == p_cb || NULL == p_cb->p_cback) {
        return;
    }

    APPL_TRACE_DEBUG( "bta_jv_port_event_cl_cback code=x%x port_handle:%d handle:%d",
                      code, port_handle, p_cb->handle);
    if (code & PORT_EV_RXCHAR) {
        evt_data.data_ind.handle = p_pcb->handle;
        p_cb->p_cback(BTA_JV_RFCOMM_DATA_IND_EVT, &evt_data, p_pcb->user_data);
    }

    if (code & PORT_EV_FC) {
        p_pcb->cong = (code & PORT_EV_FCS) ? FALSE : TRUE;
        evt_data.rfc_cong.cong = p_pcb->cong;
        evt_data.rfc_cong.handle = p_pcb->handle;
        evt_data.rfc_cong.status = BTA_JV_SUCCESS;
        p_cb->p_cback(BTA_JV_RFCOMM_CONG_EVT, &evt_data, p_pcb->user_data);
    }

    if (code & PORT_EV_TXEMPTY) {
        bta_jv_pm_conn_idle(p_pcb->p_pm_cb);
    }
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_config
**
** Description  Configure RFCOMM
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_config(tBTA_JV_MSG *p_data)
{
    APPL_TRACE_DEBUG("%s enable_l2cap_ertm:%d", __func__, p_data->rfcomm_config.enable_l2cap_ertm);

    PORT_SetL2capErtm(p_data->rfcomm_config.enable_l2cap_ertm);
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_connect
**
** Description  Client initiates an RFCOMM connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_connect(tBTA_JV_MSG *p_data)
{
    UINT16 handle = 0;
    UINT32 event_mask = BTA_JV_RFC_EV_MASK;
    tPORT_STATE port_state;
    UINT8   sec_id = 0;
    tBTA_JV_RFC_CB  *p_cb = NULL;
    tBTA_JV_PCB     *p_pcb;
    tBTA_JV_API_RFCOMM_CONNECT *cc = &(p_data->rfcomm_connect);
    tBTA_JV_RFCOMM_CL_INIT      evt_data = {0};

    /* TODO DM role manager
    L2CA_SetDesireRole(cc->role);
    */

    sec_id = bta_jv_alloc_sec_id();
    evt_data.sec_id = sec_id;
    evt_data.status = BTA_JV_SUCCESS;
    if (0 == sec_id ||
            BTM_SetSecurityLevel(TRUE, "", sec_id,  cc->sec_mask, BT_PSM_RFCOMM,
                                 BTM_SEC_PROTO_RFCOMM, cc->remote_scn) == FALSE) {
        evt_data.status = BTA_JV_FAILURE;
        APPL_TRACE_ERROR("sec_id:%d is zero or BTM_SetSecurityLevel failed, remote_scn:%d", sec_id, cc->remote_scn);
    }

    if (evt_data.status == BTA_JV_SUCCESS &&
            RFCOMM_CreateConnection(UUID_SERVCLASS_SERIAL_PORT, cc->remote_scn, FALSE,
                                    BTA_JV_DEF_RFC_MTU, cc->peer_bd_addr, &handle, bta_jv_port_mgmt_cl_cback) != PORT_SUCCESS) {
        APPL_TRACE_ERROR("bta_jv_rfcomm_connect, RFCOMM_CreateConnection failed");
        evt_data.status = BTA_JV_FAILURE;
    }
    if (evt_data.status == BTA_JV_SUCCESS) {
        p_cb = bta_jv_alloc_rfc_cb(handle, &p_pcb);
        if (p_cb) {
            p_cb->p_cback = cc->p_cback;
            p_cb->sec_id = sec_id;
            p_cb->scn = 0;
            p_pcb->state = BTA_JV_ST_CL_OPENING;
            p_pcb->user_data = cc->user_data;
            evt_data.use_co = TRUE;

            PORT_SetEventCallback(handle, bta_jv_port_event_cl_cback);
            PORT_SetEventMask(handle, event_mask);
            PORT_SetDataCOCallback (handle, bta_jv_port_data_co_cback);

            PORT_GetState(handle, &port_state);

            port_state.fc_type = (PORT_FC_CTS_ON_INPUT | PORT_FC_CTS_ON_OUTPUT);

            /* coverity[uninit_use_in_call]
               FALSE-POSITIVE: port_state is initialized at PORT_GetState() */
            PORT_SetState(handle, &port_state);

            evt_data.handle = p_pcb->handle;
        } else {
            evt_data.status = BTA_JV_FAILURE;
            APPL_TRACE_ERROR("run out of rfc control block");
        }
    }
    cc->p_cback(BTA_JV_RFCOMM_CL_INIT_EVT, (tBTA_JV *)&evt_data, cc->user_data);
    if (evt_data.status == BTA_JV_FAILURE) {
        if (sec_id) {
            bta_jv_free_sec_id(&sec_id);
        }
        if (handle) {
            RFCOMM_RemoveConnection(handle);
        }
    }
}

static int find_rfc_pcb(void *user_data, tBTA_JV_RFC_CB **cb, tBTA_JV_PCB **pcb)
{
    *cb = NULL;
    *pcb = NULL;
    int i;
    for (i = 0; i < MAX_RFC_PORTS; i++) {
        UINT32 rfc_handle = bta_jv_cb.port_cb[i].handle & BTA_JV_RFC_HDL_MASK;
        rfc_handle &= ~BTA_JV_RFCOMM_MASK;
        if (rfc_handle && bta_jv_cb.port_cb[i].user_data == user_data) {
            *pcb = &bta_jv_cb.port_cb[i];
            *cb = &bta_jv_cb.rfc_cb[rfc_handle - 1];
            APPL_TRACE_DEBUG("find_rfc_pcb(): FOUND rfc_cb_handle 0x%x, port.jv_handle:"
                             " 0x%x, state: %d, rfc_cb->handle: 0x%x", rfc_handle, (*pcb)->handle,
                             (*pcb)->state, (*cb)->handle);
            return 1;
        }
    }
    APPL_TRACE_DEBUG("find_rfc_pcb: cannot find rfc_cb from user data:%d", (UINT32)user_data);
    return 0;
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_close
**
** Description  Close an RFCOMM connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_close(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_RFCOMM_CLOSE *cc = &(p_data->rfcomm_close);
    tBTA_JV_RFC_CB           *p_cb = NULL;
    tBTA_JV_PCB              *p_pcb = NULL;
    APPL_TRACE_DEBUG("%s, rfc handle:%d",__func__, cc->handle);
    if (!cc->handle) {
        APPL_TRACE_ERROR("%s, rfc handle is null", __func__);
        return;
    }

    void *user_data = cc->user_data;
    if (!find_rfc_pcb(user_data, &p_cb, &p_pcb)) {
        return;
    }

    bta_jv_free_rfc_cb(p_cb, p_pcb, FALSE, TRUE);

    APPL_TRACE_DEBUG("%s: sec id in use:%d, rfc_cb in use:%d",__func__,
                     get_sec_id_used(), get_rfc_cb_used());
}

/*******************************************************************************
**
** Function     bta_jv_get_num_rfc_listen
**
** Description  when a RFCOMM connection goes down, make sure that there's only
**              one port stays listening on this scn.
**
** Returns
**
*******************************************************************************/
static UINT8 __attribute__((unused)) bta_jv_get_num_rfc_listen(tBTA_JV_RFC_CB *p_cb)
{
    UINT8   listen = 1;

    if (p_cb->max_sess > 1) {
        listen = 0;
        for (UINT8 i = 0; i < p_cb->max_sess; i++) {
            if (p_cb->rfc_hdl[i] != 0) {
                const tBTA_JV_PCB *p_pcb = &bta_jv_cb.port_cb[p_cb->rfc_hdl[i] - 1];
                if (BTA_JV_ST_SR_LISTEN == p_pcb->state) {
                    listen++;
                }
            }
        }
    }
    return listen;
}

/*******************************************************************************
**
** Function     bta_jv_port_mgmt_sr_cback
**
** Description  callback for port mamangement function of rfcomm
**              server connections
**
** Returns      void
**
*******************************************************************************/
static void bta_jv_port_mgmt_sr_cback(UINT32 code, UINT16 port_handle, void *data)
{
    tBTA_JV_PCB     *p_pcb = bta_jv_rfc_port_to_pcb(port_handle);
    tBTA_JV_RFC_CB  *p_cb = bta_jv_rfc_port_to_cb(port_handle);
    tBTA_JV evt_data = {0};
    BD_ADDR rem_bda = {0};
    UINT16 lcid;
    tPORT_MGMT_SR_CALLBACK_ARG *p_mgmt_cb_arg = (tPORT_MGMT_SR_CALLBACK_ARG *)data;
    void *user_data = NULL;
    void *new_user_data = NULL;
    int status;
    int failed = TRUE;

    // APPL_TRACE_DEBUG("bta_jv_port_mgmt_sr_cback, code:0x%x, port_handle:%d", code, (uint16_t)port_handle);
    if (NULL == p_cb || NULL == p_cb->p_cback) {
        // APPL_TRACE_ERROR("bta_jv_port_mgmt_sr_cback, p_cb:%p, p_cb->p_cback%p",
        // p_cb, p_cb ? p_cb->p_cback : NULL);
        return;
    }
    user_data = p_pcb->user_data;
    // APPL_TRACE_DEBUG( "bta_jv_port_mgmt_sr_cback code=%p port_handle:0x%x handle:0x%x, p_pcb:%p, user:%p",
    // code, port_handle, p_cb->handle, p_pcb, p_pcb->user_data);

    if (p_mgmt_cb_arg) {
        if ((status = PORT_CheckConnection(port_handle, p_mgmt_cb_arg->ignore_rfc_state, rem_bda, &lcid)) !=
            PORT_SUCCESS) {
            APPL_TRACE_WARNING("PORT_CheckConnection status:%d", status);
        }
    } else {
        PORT_CheckConnection(port_handle, FALSE, rem_bda, &lcid);
    }

    if (code == PORT_SUCCESS) {
        failed = FALSE;
        /* accept the connection defaulted */
        if (p_mgmt_cb_arg) {
            p_mgmt_cb_arg->accept = TRUE;
        }
        evt_data.rfc_srv_open.handle = p_pcb->handle;
        evt_data.rfc_srv_open.status = BTA_JV_SUCCESS;
        evt_data.rfc_srv_open.peer_mtu = p_mgmt_cb_arg->peer_mtu;
        bdcpy(evt_data.rfc_srv_open.rem_bda, rem_bda);
        tBTA_JV_PCB *p_pcb_new_listen  = bta_jv_add_rfc_port(p_cb, p_pcb);
        if (p_pcb_new_listen) {
            evt_data.rfc_srv_open.new_listen_handle = p_pcb_new_listen->handle;
            new_user_data = p_cb->p_cback(BTA_JV_RFCOMM_SRV_OPEN_EVT, &evt_data, user_data);
            if (new_user_data) {
                p_pcb_new_listen->user_data = new_user_data;
                APPL_TRACE_DEBUG("PORT_SUCCESS: curr_sess:%d, max_sess:%d", p_cb->curr_sess,
                                 p_cb->max_sess);
            } else {
                /**
                 * new_user_data is NULL, which means the upper layer runs out of slot source.
                 * Tells the caller to reject this connection.
                 */
                APPL_TRACE_ERROR("create new listen port, but upper layer reject connection");
                p_pcb_new_listen->user_data = NULL;
                p_pcb->state = BTA_JV_ST_SR_LISTEN;
                bta_jv_free_rfc_cb(p_cb, p_pcb_new_listen, FALSE, FALSE);
                if (p_mgmt_cb_arg) {
                    p_mgmt_cb_arg->accept = FALSE;
                }
            }
        } else {
            evt_data.rfc_srv_open.new_listen_handle = 0;
            new_user_data = p_cb->p_cback(BTA_JV_RFCOMM_SRV_OPEN_EVT, &evt_data, user_data);
            if (new_user_data) {
                APPL_TRACE_DEBUG("PORT_SUCCESS: curr_sess:%d, max_sess:%d", p_cb->curr_sess,
                                 p_cb->max_sess);
            } else {
                /**
                 * new_user_data is NULL, which means the upper layer runs out of slot source.
                 * Tells the caller to reject this connection.
                 */
                APPL_TRACE_ERROR("no listen port, and upper layer reject connection");
                p_pcb->state = BTA_JV_ST_SR_LISTEN;
                if (p_mgmt_cb_arg) {
                    p_mgmt_cb_arg->accept = FALSE;
                }
            }
            APPL_TRACE_ERROR("bta_jv_add_rfc_port failed to create new listen port");
        }
    }

    if (failed) {
        evt_data.rfc_close.handle = p_pcb->handle;
        evt_data.rfc_close.status = BTA_JV_FAILURE;
        evt_data.rfc_close.async = TRUE;
        evt_data.rfc_close.port_status = code;
        evt_data.rfc_close.user_data = user_data;
        p_pcb->cong = FALSE;

        tBTA_JV_RFCOMM_CBACK    *p_cback = p_cb->p_cback;
        APPL_TRACE_DEBUG("PORT_CLOSED before BTA_JV_RFCOMM_CLOSE_EVT: curr_sess:%d, max_sess:%d",
                         p_cb->curr_sess, p_cb->max_sess);
        if (BTA_JV_ST_SR_CLOSING == p_pcb->state) {
            evt_data.rfc_close.async = FALSE;
            evt_data.rfc_close.status = BTA_JV_SUCCESS;
        }

        // To free up resources.
        p_pcb->state = BTA_JV_ST_SR_CLOSING;
        bta_jv_free_rfc_cb(p_cb, p_pcb, FALSE, FALSE);
        p_cback(BTA_JV_RFCOMM_CLOSE_EVT, &evt_data, user_data);
        APPL_TRACE_DEBUG("PORT_CLOSED after BTA_JV_RFCOMM_CLOSE_EVT: curr_sess:%d, max_sess:%d",
                         p_cb->curr_sess, p_cb->max_sess);
    }
}

/*******************************************************************************
**
** Function     bta_jv_port_event_sr_cback
**
** Description  Callback for RFCOMM server port events
**
** Returns      void
**
*******************************************************************************/
static void bta_jv_port_event_sr_cback(UINT32 code, UINT16 port_handle)
{
    tBTA_JV_PCB     *p_pcb = bta_jv_rfc_port_to_pcb(port_handle);
    tBTA_JV_RFC_CB  *p_cb = bta_jv_rfc_port_to_cb(port_handle);
    tBTA_JV evt_data = {0};

    if (NULL == p_cb || NULL == p_cb->p_cback) {
        return;
    }

    APPL_TRACE_DEBUG( "bta_jv_port_event_sr_cback code=x%x port_handle:%d handle:%d",
                      code, port_handle, p_cb->handle);

    void *user_data = p_pcb->user_data;
    if (code & PORT_EV_RXCHAR) {
        evt_data.data_ind.handle = p_pcb->handle;
        p_cb->p_cback(BTA_JV_RFCOMM_DATA_IND_EVT, &evt_data, user_data);
    }

    if (code & PORT_EV_FC) {
        p_pcb->cong = (code & PORT_EV_FCS) ? FALSE : TRUE;
        evt_data.rfc_cong.cong = p_pcb->cong;
        evt_data.rfc_cong.handle = p_pcb->handle;
        evt_data.rfc_cong.status = BTA_JV_SUCCESS;
        p_cb->p_cback(BTA_JV_RFCOMM_CONG_EVT, &evt_data, user_data);
    }

    if (code & PORT_EV_TXEMPTY) {
        bta_jv_pm_conn_idle(p_pcb->p_pm_cb);
    }
}

/*******************************************************************************
**
** Function     bta_jv_add_rfc_port
**
** Description  add a port for server when the existing posts is open
**
** Returns   return a pointer to tBTA_JV_PCB just added
**
*******************************************************************************/
static tBTA_JV_PCB *bta_jv_add_rfc_port(tBTA_JV_RFC_CB *p_cb, tBTA_JV_PCB *p_pcb_open)
{
    UINT8   used = 0, i, listen = 0;
    UINT32  si = 0;
    tPORT_STATE port_state;
    UINT32 event_mask = BTA_JV_RFC_EV_MASK;
    tBTA_JV_PCB *p_pcb = NULL;
    if (p_cb->max_sess > 1) {
        for (i = 0; i < p_cb->max_sess; i++) {
            if (p_cb->rfc_hdl[i] != 0) {
                p_pcb = &bta_jv_cb.port_cb[p_cb->rfc_hdl[i] - 1];
                if (p_pcb->state == BTA_JV_ST_SR_LISTEN) {
                    listen++;
                    if (p_pcb_open == p_pcb) {
                        APPL_TRACE_DEBUG("bta_jv_add_rfc_port, port_handle:%d, change the listen port to open state",
                                         p_pcb->port_handle);
                        p_pcb->state = BTA_JV_ST_SR_OPEN;

                    } else {
                        APPL_TRACE_ERROR("bta_jv_add_rfc_port, open pcb not matching listen one,"
                                         "listen count:%d, listen pcb handle:%d, open pcb:%d",
                                         listen, p_pcb->port_handle, p_pcb_open->handle);
                        return NULL;
                    }
                }
                used++;
            } else if (si == 0) {
                si = i + 1;
            }
        }

        p_pcb = NULL;
        APPL_TRACE_DEBUG("bta_jv_add_rfc_port max_sess=%d used:%d curr_sess:%d, listen:%d si:%d",
                         p_cb->max_sess, used, p_cb->curr_sess, listen, si);
        if (used < p_cb->max_sess && listen == 1 && si) {
            si--;
            if (RFCOMM_CreateConnection(p_cb->sec_id, p_cb->scn, TRUE,
                                        BTA_JV_DEF_RFC_MTU, (UINT8 *) bd_addr_any, &(p_cb->rfc_hdl[si]), bta_jv_port_mgmt_sr_cback) == PORT_SUCCESS) {
                p_cb->curr_sess++;
                p_pcb = &bta_jv_cb.port_cb[p_cb->rfc_hdl[si] - 1];
                p_pcb->state = BTA_JV_ST_SR_LISTEN;
                p_pcb->port_handle = p_cb->rfc_hdl[si];
                p_pcb->user_data = p_pcb_open->user_data;

                PORT_ClearKeepHandleFlag(p_pcb->port_handle);
                PORT_SetEventCallback(p_pcb->port_handle, bta_jv_port_event_sr_cback);
                PORT_SetDataCOCallback (p_pcb->port_handle, bta_jv_port_data_co_cback);
                PORT_SetEventMask(p_pcb->port_handle, event_mask);
                PORT_GetState(p_pcb->port_handle, &port_state);

                port_state.fc_type = (PORT_FC_CTS_ON_INPUT | PORT_FC_CTS_ON_OUTPUT);

                PORT_SetState(p_pcb->port_handle, &port_state);
                p_pcb->handle = BTA_JV_RFC_H_S_TO_HDL(p_cb->handle, si);
                APPL_TRACE_DEBUG("bta_jv_add_rfc_port: p_pcb->handle:0x%x, curr_sess:%d",
                                 p_pcb->handle, p_cb->curr_sess);
            }
        } else {
            /* avoid p_pcb always points to the last element of rfc_hdl */
            APPL_TRACE_ERROR("bta_jv_add_rfc_port, cannot create new rfc listen port");
        }
    }
    APPL_TRACE_DEBUG("bta_jv_add_rfc_port: sec id in use:%d, rfc_cb in use:%d",
                     get_sec_id_used(), get_rfc_cb_used());
    return p_pcb;
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_start_server
**
** Description  waits for an RFCOMM client to connect
**
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_start_server(tBTA_JV_MSG *p_data)
{
    UINT16 handle = 0;
    UINT32 event_mask = BTA_JV_RFC_EV_MASK;
    tPORT_STATE port_state;
    UINT8   sec_id = 0;
    tBTA_JV_RFC_CB  *p_cb = NULL;
    tBTA_JV_PCB     *p_pcb;
    tBTA_JV_API_RFCOMM_SERVER *rs = &(p_data->rfcomm_server);
    tBTA_JV_RFCOMM_START        evt_data = {0};
    /* TODO DM role manager
    L2CA_SetDesireRole(rs->role);
    */
    evt_data.status = BTA_JV_FAILURE;
    APPL_TRACE_DEBUG("bta_jv_rfcomm_start_server: sec id in use:%d, rfc_cb in use:%d",
                     get_sec_id_used(), get_rfc_cb_used());

    do {
        sec_id = bta_jv_alloc_sec_id();

        if (0 == sec_id ||
                BTM_SetSecurityLevel(FALSE, "JV PORT", sec_id,  rs->sec_mask,
                                     BT_PSM_RFCOMM, BTM_SEC_PROTO_RFCOMM, rs->local_scn) == FALSE) {
            APPL_TRACE_ERROR("bta_jv_rfcomm_start_server, run out of sec_id");
            break;
        }

        if (RFCOMM_CreateConnection(sec_id, rs->local_scn, TRUE,
                                    BTA_JV_DEF_RFC_MTU, (UINT8 *) bd_addr_any, &handle, bta_jv_port_mgmt_sr_cback) != PORT_SUCCESS) {
            APPL_TRACE_ERROR("bta_jv_rfcomm_start_server, RFCOMM_CreateConnection failed");
            break;
        }


        p_cb = bta_jv_alloc_rfc_cb(handle, &p_pcb);
        if (!p_cb) {
            APPL_TRACE_ERROR("bta_jv_rfcomm_start_server, run out of rfc control block");
            break;
        }

        p_cb->max_sess = rs->max_session;
        p_cb->p_cback = rs->p_cback;
        p_cb->sec_id = sec_id;
        p_cb->scn = rs->local_scn;
        p_pcb->state = BTA_JV_ST_SR_LISTEN;
        p_pcb->user_data = rs->user_data;
        evt_data.status = BTA_JV_SUCCESS;
        evt_data.handle = p_pcb->handle;
        evt_data.sec_id = sec_id;
        evt_data.scn = rs->local_scn;
        evt_data.use_co = TRUE;

        PORT_ClearKeepHandleFlag(handle);
        PORT_SetEventCallback(handle, bta_jv_port_event_sr_cback);
        PORT_SetEventMask(handle, event_mask);
        PORT_GetState(handle, &port_state);

        port_state.fc_type = (PORT_FC_CTS_ON_INPUT | PORT_FC_CTS_ON_OUTPUT);

        PORT_SetState(handle, &port_state);
    } while (0);

    rs->p_cback(BTA_JV_RFCOMM_START_EVT, (tBTA_JV *)&evt_data, rs->user_data);
    if (evt_data.status == BTA_JV_SUCCESS) {
        PORT_SetDataCOCallback (handle, bta_jv_port_data_co_cback);
    } else {
        if (sec_id) {
            bta_jv_free_sec_id(&sec_id);
        }
        if (handle) {
            RFCOMM_RemoveConnection(handle);
        }
    }
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_stop_server
**
** Description  stops an RFCOMM server
**
** Returns      void
**
*******************************************************************************/

void bta_jv_rfcomm_stop_server(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_RFCOMM_SERVER *ls = &(p_data->rfcomm_server);
    tBTA_JV_RFC_CB           *p_cb = NULL;
    tBTA_JV_PCB              *p_pcb = NULL;
    APPL_TRACE_DEBUG("bta_jv_rfcomm_stop_server");
    if (!ls->handle) {
        APPL_TRACE_ERROR("bta_jv_rfcomm_stop_server, jv handle is null");
        return;
    }
    void *user_data = ls->user_data;
    if (!find_rfc_pcb(user_data, &p_cb, &p_pcb)) {
        return;
    }
    APPL_TRACE_DEBUG("bta_jv_rfcomm_stop_server: p_pcb:%p, p_pcb->port_handle:%d",
                     p_pcb, p_pcb->port_handle);
    bta_jv_free_rfc_cb(p_cb, p_pcb, TRUE, FALSE);
    APPL_TRACE_DEBUG("bta_jv_rfcomm_stop_server: sec id in use:%d, rfc_cb in use:%d",
                     get_sec_id_used(), get_rfc_cb_used());
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_read
**
** Description  Read data from an RFCOMM connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_read(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_RFCOMM_READ *rc = &(p_data->rfcomm_read);
    tBTA_JV_RFC_CB  *p_cb = rc->p_cb;
    tBTA_JV_PCB     *p_pcb = rc->p_pcb;
    tBTA_JV_RFCOMM_READ    evt_data;

    evt_data.status = BTA_JV_FAILURE;
    evt_data.handle = p_pcb->handle;
    evt_data.req_id = rc->req_id;
    evt_data.p_data = rc->p_data;
    if (PORT_ReadData(rc->p_pcb->port_handle, (char *)rc->p_data, rc->len, &evt_data.len) ==
            PORT_SUCCESS) {
        evt_data.status = BTA_JV_SUCCESS;
    }

    p_cb->p_cback(BTA_JV_RFCOMM_READ_EVT, (tBTA_JV *)&evt_data, p_pcb->user_data);
}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_write
**
** Description  write data to an RFCOMM connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_write(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_RFCOMM_WRITE *wc = &(p_data->rfcomm_write);
    tBTA_JV_RFC_CB  *p_cb = wc->p_cb;
    tBTA_JV_PCB     *p_pcb = wc->p_pcb;
    tBTA_JV_RFCOMM_WRITE    evt_data;

    evt_data.status = BTA_JV_FAILURE;
    evt_data.handle = p_pcb->handle;
    evt_data.req_id = wc->req_id;
    evt_data.old_cong = p_pcb->cong;
    bta_jv_pm_conn_busy(p_pcb->p_pm_cb);
    evt_data.len = -1;
    if (!evt_data.old_cong &&
            PORT_WriteDataCO(p_pcb->port_handle, &evt_data.len, wc->len, wc->p_data) ==
            PORT_SUCCESS) {
        evt_data.status = BTA_JV_SUCCESS;
    }
    // update congestion flag
    evt_data.cong = p_pcb->cong;
    if (p_cb->p_cback) {
        p_cb->p_cback(BTA_JV_RFCOMM_WRITE_EVT, (tBTA_JV *)&evt_data, p_pcb->user_data);
    } else {
        APPL_TRACE_ERROR("bta_jv_rfcomm_write :: WARNING ! No JV callback set");
    }

}

/*******************************************************************************
**
** Function     bta_jv_rfcomm_flow_control
**
** Description  give credits to the peer
**
** Returns      void
**
*******************************************************************************/
void bta_jv_rfcomm_flow_control(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_RFCOMM_FLOW_CONTROL *fc = &(p_data->rfcomm_fc);

    tBTA_JV_PCB *p_pcb = fc->p_pcb;
    PORT_FlowControl_GiveCredit(p_pcb->port_handle, TRUE, fc->credits_given);
}
#endif /* BTA_JV_RFCOMM_INCLUDED */

/*******************************************************************************
 **
 ** Function     bta_jv_set_pm_profile
 **
 ** Description  Set or free power mode profile for a JV application
 **
 ** Returns      void
 **
 *******************************************************************************/
void bta_jv_set_pm_profile(tBTA_JV_MSG *p_data)
{
    tBTA_JV_STATUS status;
    tBTA_JV_PM_CB *p_cb;

    APPL_TRACE_API("bta_jv_set_pm_profile(handle: 0x%x, app_id: %d, init_st: %d)",
                   p_data->set_pm.handle, p_data->set_pm.app_id, p_data->set_pm.init_st);

    /* clear PM control block */
    if (p_data->set_pm.app_id == BTA_JV_PM_ID_CLEAR) {
        status = bta_jv_free_set_pm_profile_cb(p_data->set_pm.handle);

        if (status != BTA_JV_SUCCESS) {
            APPL_TRACE_WARNING("bta_jv_set_pm_profile() free pm cb failed: reason %d",
                               status);
        }
    } else { /* set PM control block */
        p_cb = bta_jv_alloc_set_pm_profile_cb(p_data->set_pm.handle,
                                              p_data->set_pm.app_id);

        if (NULL != p_cb) {
            bta_jv_pm_state_change(p_cb, p_data->set_pm.init_st);
        } else {
            APPL_TRACE_WARNING("bta_jv_alloc_set_pm_profile_cb() failed");
        }
    }
}

/*******************************************************************************
 **
 ** Function     bta_jv_change_pm_state
 **
 ** Description  change jv pm connect state, used internally
 **
 ** Returns      void
 **
 *******************************************************************************/
void bta_jv_change_pm_state(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_PM_STATE_CHANGE *p_msg = (tBTA_JV_API_PM_STATE_CHANGE *)p_data;

    if (p_msg->p_cb) {
        bta_jv_pm_state_change(p_msg->p_cb, p_msg->state);
    }
}


/*******************************************************************************
 **
 ** Function    bta_jv_set_pm_conn_state
 **
 ** Description Send pm event state change to jv state machine to serialize jv pm changes
 **             in relation to other jv messages. internal API use mainly.
 **
 ** Params:     p_cb: jv pm control block, NULL pointer returns failure
 **             new_state: new PM connections state, setting is forced by action function
 **
 ** Returns     BTA_JV_SUCCESS, BTA_JV_FAILURE (buffer allocation, or NULL ptr!)
 **
 *******************************************************************************/
tBTA_JV_STATUS bta_jv_set_pm_conn_state(tBTA_JV_PM_CB *p_cb, const tBTA_JV_CONN_STATE
                                        new_st)
{
    tBTA_JV_STATUS status = BTA_JV_FAILURE;
    tBTA_JV_API_PM_STATE_CHANGE *p_msg;

    if (NULL == p_cb) {
        return status;
    }

    APPL_TRACE_API("bta_jv_set_pm_conn_state(handle:0x%x, state: %d)", p_cb->handle,
                   new_st);
    if ((p_msg = (tBTA_JV_API_PM_STATE_CHANGE *)osi_malloc(
                     sizeof(tBTA_JV_API_PM_STATE_CHANGE))) != NULL) {
        p_msg->hdr.event = BTA_JV_API_PM_STATE_CHANGE_EVT;
        p_msg->p_cb = p_cb;
        p_msg->state = new_st;
        bta_sys_sendmsg(p_msg);
        status = BTA_JV_SUCCESS;
    }
    return (status);
}

/*******************************************************************************
 **
 ** Function    bta_jv_pm_conn_busy
 **
 ** Description set pm connection busy state (input param safe)
 **
 ** Params      p_cb: pm control block of jv connection
 **
 ** Returns     void
 **
 *******************************************************************************/
static void bta_jv_pm_conn_busy(tBTA_JV_PM_CB *p_cb)
{
    if ((NULL != p_cb) && (BTA_JV_PM_IDLE_ST == p_cb->state)) {
        bta_jv_pm_state_change(p_cb, BTA_JV_CONN_BUSY);
    }
}

/*******************************************************************************
 **
 ** Function    bta_jv_pm_conn_busy
 **
 ** Description set pm connection busy state (input param safe)
 **
 ** Params      p_cb: pm control block of jv connection
 **
 ** Returns     void
 **
 *******************************************************************************/
static void bta_jv_pm_conn_idle(tBTA_JV_PM_CB *p_cb)
{
    if ((NULL != p_cb) && (BTA_JV_PM_IDLE_ST != p_cb->state)) {
        bta_jv_pm_state_change(p_cb, BTA_JV_CONN_IDLE);
    }
}

/*******************************************************************************
 **
 ** Function     bta_jv_pm_state_change
 **
 ** Description  Notify power manager there is state change
 **
 ** Params      p_cb: must be NONE NULL
 **
 ** Returns      void
 **
 *******************************************************************************/
static void bta_jv_pm_state_change(tBTA_JV_PM_CB *p_cb, const tBTA_JV_CONN_STATE state)
{
    APPL_TRACE_API("bta_jv_pm_state_change(p_cb: 0x%x, handle: 0x%x, busy/idle_state: %d"
                   ", app_id: %d, conn_state: %d)", (uint32_t)p_cb, p_cb->handle, p_cb->state,
                   p_cb->app_id, state);

    switch (state) {
    case BTA_JV_CONN_OPEN:
        bta_sys_conn_open(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_CONN_CLOSE:
        bta_sys_conn_close(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_APP_OPEN:
        bta_sys_app_open(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_APP_CLOSE:
        bta_sys_app_close(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_SCO_OPEN:
        bta_sys_sco_open(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_SCO_CLOSE:
        bta_sys_sco_close(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_CONN_IDLE:
        p_cb->state = BTA_JV_PM_IDLE_ST;
        bta_sys_idle(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    case BTA_JV_CONN_BUSY:
        p_cb->state = BTA_JV_PM_BUSY_ST;
        bta_sys_busy(BTA_ID_JV, p_cb->app_id, p_cb->peer_bd_addr);
        break;

    default:
        APPL_TRACE_WARNING("bta_jv_pm_state_change(state: %d): Invalid state", state);
        break;
    }
}
/**********************************************************************************************/

#if BTA_JV_L2CAP_INCLUDED
static struct fc_channel *fcchan_get(uint16_t chan, char create)
{
    struct fc_channel *t = fc_channels;
    static tL2CAP_FIXED_CHNL_REG fcr = {
        .pL2CA_FixedConn_Cb = fcchan_conn_chng_cbk,
        .pL2CA_FixedData_Cb = fcchan_data_cbk,
        .default_idle_tout  = 0xffff,
        .fixed_chnl_opts = {
            .mode         = L2CAP_FCR_BASIC_MODE,
            .max_transmit = 0xFF,
            .rtrans_tout  = 2000,
            .mon_tout     = 12000,
            .mps          = 670,
            .tx_win_sz    = 1,
        },
    };

    while (t && t->chan != chan) {
        t = t->next;
    }

    if (t) {
        return t;
    } else if (!create) {
        return NULL;    /* we cannot alloc a struct if not asked to */
    }

    t = osi_calloc(sizeof(*t));
    if (!t) {
        return NULL;
    }

    t->chan = chan;

    if (!L2CA_RegisterFixedChannel(chan, &fcr)) {
        osi_free(t);
        return NULL;
    }

    //link it in
    t->next = fc_channels;
    fc_channels = t;

    return t;
}

/* pass NULL to find servers */
static struct fc_client *fcclient_find_by_addr(struct fc_client *start, BD_ADDR addr)
{
    struct fc_client *t = start;

    while (t) {

        /* match client if have addr */
        if (addr && !memcmp(addr, &t->remote_addr, sizeof(t->remote_addr))) {
            break;
        }

        /* match server if do not have addr */
        if (!addr && t->server) {
            break;
        }

        t = t->next_all_list;
    }

    return t;
}

static struct fc_client *fcclient_find_by_id(uint32_t id)
{
    struct fc_client *t = fc_clients;

    while (t && t->id != id) {
        t = t->next_all_list;
    }

    return t;
}

static struct fc_client *fcclient_alloc(uint16_t chan, char server, const uint8_t *sec_id_to_use)
{
    struct fc_channel *fc = fcchan_get(chan, TRUE);
    struct fc_client *t;
    uint8_t sec_id;

    if (!fc) {
        return NULL;
    }

    if (fc->has_server && server) {
        return NULL;    /* no way to have multiple servers on same channel */
    }

    if (sec_id_to_use) {
        sec_id = *sec_id_to_use;
    } else {
        sec_id = bta_jv_alloc_sec_id();
    }

    t = osi_calloc(sizeof(*t));
    if (t) {
        //allocate it a unique ID
        do {
            t->id = ++fc_next_id;
        } while (!t->id || fcclient_find_by_id(t->id));

        //populate some params
        t->chan = chan;
        t->server = server;

        //get a security id
        t->sec_id = sec_id;

        //link it in to global list
        t->next_all_list = fc_clients;
        fc_clients = t;

        //link it in to channel list
        t->next_chan_list = fc->clients;
        fc->clients = t;

        //update channel if needed
        if (server) {
            fc->has_server = TRUE;
        }
    } else if (!sec_id_to_use) {
        bta_jv_free_sec_id(&sec_id);
    }

    return t;
}

static void fcclient_free(struct fc_client *fc)
{
    struct fc_client *t = fc_clients;
    struct fc_channel *tc = fcchan_get(fc->chan, FALSE);

    //remove from global list
    while (t && t->next_all_list != fc) {
        t = t->next_all_list;
    }

    if (!t && fc != fc_clients) {
        return;    /* prevent double-free */
    }

    if (t) {
        t->next_all_list = fc->next_all_list;
    } else {
        fc_clients = fc->next_all_list;
    }

    //remove from channel list
    if (tc) {
        t = tc->clients;

        while (t && t->next_chan_list != fc) {
            t = t->next_chan_list;
        }

        if (t) {
            t->next_chan_list = fc->next_chan_list;
        } else {
            tc->clients = fc->next_chan_list;
        }

        //if was server then channel no longer has a server
        if (fc->server) {
            tc->has_server = FALSE;
        }
    }

    //free security id
    bta_jv_free_sec_id(&fc->sec_id);

    osi_free(fc);
}

static void fcchan_conn_chng_cbk(UINT16 chan, BD_ADDR bd_addr, BOOLEAN connected, UINT16 reason, tBT_TRANSPORT transport)
{
    tBTA_JV init_evt;
    tBTA_JV open_evt;
    struct fc_channel *tc;
    struct fc_client *t = NULL, *new_conn;
    tBTA_JV_L2CAP_CBACK *p_cback = NULL;
    char call_init = FALSE;
    void *user_data = NULL;


    tc = fcchan_get(chan, FALSE);
    if (tc) {
        t = fcclient_find_by_addr(tc->clients, bd_addr); // try to find an open socked for that addr
        if (t) {
            p_cback = t->p_cback;
            user_data = t->user_data;
        } else {
            t = fcclient_find_by_addr(tc->clients, NULL); // try to find a listening socked for that channel
            if (t) {
                //found: create a normal connection socket and assign the connection to it
                new_conn = fcclient_alloc(chan, FALSE, &t->sec_id);
                if (new_conn) {

                    memcpy(&new_conn->remote_addr, bd_addr, sizeof(new_conn->remote_addr));
                    new_conn->p_cback = NULL; //for now
                    new_conn->init_called = TRUE; /*nop need to do it again */

                    p_cback = t->p_cback;
                    user_data = t->user_data;

                    t = new_conn;
                }
            } else {
                //drop it
                return;
            }
        }
    }

    if (t) {

        if (!t->init_called) {

            call_init = TRUE;
            t->init_called = TRUE;

            init_evt.l2c_cl_init.handle = t->id;
            init_evt.l2c_cl_init.status = BTA_JV_SUCCESS;
            init_evt.l2c_cl_init.sec_id = t->sec_id;
        }

        open_evt.l2c_open.handle = t->id;
        open_evt.l2c_open.tx_mtu = 23; /* 23, why not ?*/
        memcpy(&open_evt.l2c_le_open.rem_bda, &t->remote_addr, sizeof(open_evt.l2c_le_open.rem_bda));
        open_evt.l2c_le_open.p_p_cback = (void **)&t->p_cback;
        open_evt.l2c_le_open.p_user_data = &t->user_data;
        open_evt.l2c_le_open.status = BTA_JV_SUCCESS;

        if (connected) {
            open_evt.l2c_open.status = BTA_JV_SUCCESS;
        } else {
            fcclient_free(t);
            open_evt.l2c_open.status = BTA_JV_FAILURE;
        }
    }

    if (call_init) {
        p_cback(BTA_JV_L2CAP_CL_INIT_EVT, &init_evt, user_data);
    }

    //call this with lock taken so socket does not disappear from under us */
    if (p_cback) {
        p_cback(BTA_JV_L2CAP_OPEN_EVT, &open_evt, user_data);
        if (!t->p_cback) { /* no callback set, means they do not want this one... */
            fcclient_free(t);
        }
    }
}

static void fcchan_data_cbk(UINT16 chan, BD_ADDR bd_addr, BT_HDR *p_buf)
{
    tBTA_JV evt_data = {0};
    // tBTA_JV evt_open;
    struct fc_channel *tc;
    struct fc_client *t = NULL;
    tBTA_JV_L2CAP_CBACK *sock_cback = NULL;
    void *sock_user_data;

    tc = fcchan_get(chan, FALSE);
    if (tc) {
        t = fcclient_find_by_addr(tc->clients, bd_addr); // try to find an open socked for that addr and channel
        if (!t) {
            //no socket -> drop it
            return;
        }
    }

    sock_cback = t->p_cback;
    sock_user_data = t->user_data;
    evt_data.le_data_ind.handle = t->id;
    evt_data.le_data_ind.p_buf = p_buf;

    if (sock_cback) {
        sock_cback(BTA_JV_L2CAP_DATA_IND_EVT, &evt_data, sock_user_data);
    }
}


/*******************************************************************************
**
** Function     bta_jv_l2cap_connect_le
**
** Description  makes an le l2cap client connection
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_connect_le(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_L2CAP_CONNECT *cc = &(p_data->l2cap_connect);
    tBTA_JV evt;
    uint32_t id;
    char call_init_f = TRUE;
    struct fc_client *t;

    evt.l2c_cl_init.handle = GAP_INVALID_HANDLE;
    evt.l2c_cl_init.status = BTA_JV_FAILURE;

    t = fcclient_alloc(cc->remote_chan, FALSE, NULL);
    if (!t) {
        cc->p_cback(BTA_JV_L2CAP_CL_INIT_EVT, &evt, cc->user_data);
        return;
    }

    t->p_cback = cc->p_cback;
    t->user_data = cc->user_data;
    memcpy(&t->remote_addr, &cc->peer_bd_addr, sizeof(t->remote_addr));
    id = t->id;
    t->init_called = FALSE;

    if (L2CA_ConnectFixedChnl(t->chan, t->remote_addr, BLE_ADDR_UNKNOWN_TYPE, FALSE)) {

        evt.l2c_cl_init.status = BTA_JV_SUCCESS;
        evt.l2c_cl_init.handle = id;
    }

    //it could have been deleted/moved from under us, so re-find it */
    t = fcclient_find_by_id(id);
    if (t) {
        if (evt.l2c_cl_init.status == BTA_JV_SUCCESS) {
            call_init_f = !t->init_called;
        } else {
            fcclient_free(t);
        }
    }
    if (call_init_f) {
        cc->p_cback(BTA_JV_L2CAP_CL_INIT_EVT, &evt, cc->user_data);
    }
    t->init_called = TRUE;
}


/*******************************************************************************
**
** Function     bta_jv_l2cap_stop_server_le
**
** Description  stops an LE L2CAP server
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_stop_server_le(tBTA_JV_MSG *p_data)
{
    tBTA_JV  evt;
    tBTA_JV_API_L2CAP_SERVER *ls = &(p_data->l2cap_server);
    tBTA_JV_L2CAP_CBACK *p_cback = NULL;
    struct fc_channel *fcchan;
    struct fc_client *fcclient;
    void *user_data;

    evt.l2c_close.status = BTA_JV_FAILURE;
    evt.l2c_close.async = FALSE;
    evt.l2c_close.handle = GAP_INVALID_HANDLE;

    fcchan = fcchan_get(ls->local_chan, FALSE);
    if (fcchan) {
        while ((fcclient = fcchan->clients)) {
            p_cback = fcclient->p_cback;
            user_data = fcclient->user_data;

            evt.l2c_close.handle = fcclient->id;
            evt.l2c_close.status = BTA_JV_SUCCESS;
            evt.l2c_close.async = FALSE;

            fcclient_free(fcclient);

            if (p_cback) {
                p_cback(BTA_JV_L2CAP_CLOSE_EVT, &evt, user_data);
            }
        }
    }
}

/*******************************************************************************
**
** Function     bta_jv_l2cap_start_server_le
**
** Description  starts an LE L2CAP server
**
** Returns      void
**
*******************************************************************************/
void bta_jv_l2cap_start_server_le(tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_L2CAP_SERVER *ss = &(p_data->l2cap_server);
    tBTA_JV_L2CAP_START evt_data;
    struct fc_client *t;
    // uint16_t handle;

    evt_data.handle = GAP_INVALID_HANDLE;
    evt_data.status = BTA_JV_FAILURE;


    t = fcclient_alloc(ss->local_chan, TRUE, NULL);
    if (!t) {
        goto out;
    }

    t->p_cback = ss->p_cback;
    t->user_data = ss->user_data;

    //if we got here, we're registered...
    evt_data.status = BTA_JV_SUCCESS;
    evt_data.handle = t->id;
    evt_data.sec_id = t->sec_id;

out:
    ss->p_cback(BTA_JV_L2CAP_START_EVT, (tBTA_JV *)&evt_data, ss->user_data);
}

/*******************************************************************************
**
** Function     bta_jv_l2cap_close_fixed
**
** Description  close a fixed channel connection. calls no callbacks. idempotent
**
** Returns      void
**
*******************************************************************************/
extern void bta_jv_l2cap_close_fixed (tBTA_JV_MSG *p_data)
{
    tBTA_JV_API_L2CAP_CLOSE *cc = &(p_data->l2cap_close);
    struct fc_client *t;

    t = fcclient_find_by_id(cc->handle);
    if (t) {
        fcclient_free(t);
    }
}
#endif /* BTA_JV_L2CAP_INCLUDED */

#endif  ///defined BTA_JV_INCLUDED && BTA_JV_INCLUDED == TRUE
