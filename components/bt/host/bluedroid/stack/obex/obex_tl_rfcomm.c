/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "osi/osi.h"
#include "osi/allocator.h"
#include "common/bt_target.h"

#include "stack/port_api.h"
#include "stack/btm_api.h"
#include "stack/sdpdefs.h"
#include "obex_tl.h"
#include "obex_tl_rfcomm.h"

#if (OBEX_INCLUDED == TRUE && RFCOMM_INCLUDED == TRUE)

#define OBEX_TL_RFCOMM_NUM_CONN         4
#define OBEX_TL_RFCOMM_NUM_SERVER       2

#define OBEX_TL_RFCOMM_EVENT_MARK       (PORT_EV_FC | PORT_EV_FCS)

typedef struct {
    UINT16      rfc_handle;         /* rfcomm handle */
    UINT16      mtu;                /* rfcomm mtu */
    BOOLEAN     initiator;          /* TRUE if is initiator, otherwise FALSE */
    UINT8       scn;                /* service channel number */
    BD_ADDR     addr;               /* peer bluetooth device address */
    UINT8       allocated;          /* 0 if not allocated, otherwise, index + 1, equal to handle */
} tOBEX_TL_RFCOMM_CCB;

typedef struct {
    UINT16      rfc_handle;         /* rfcomm handle */
    UINT8       scn;                /* service channel number */
    UINT8       allocated;          /* 0 if not allocated, otherwise, index + 1, handle of server will left shift 8 bits */
} tOBEX_TL_RFCOMM_SCB;

typedef struct {
    tOBEX_TL_CBACK      *callback;                          /* Upper layer callback */
    tOBEX_TL_RFCOMM_CCB  ccb[OBEX_TL_RFCOMM_NUM_CONN];
    tOBEX_TL_RFCOMM_SCB  scb[OBEX_TL_RFCOMM_NUM_SERVER];
    UINT8               trace_level;                        /* trace level */
} tOBEX_TL_RFCOMM_CB;

#if OBEX_DYNAMIC_MEMORY == FALSE
static tOBEX_TL_RFCOMM_CB obex_tl_rfcomm_cb;
#else
static tOBEX_TL_RFCOMM_CB *obex_tl_rfcomm_cb_ptr = NULL;
#define obex_tl_rfcomm_cb (*obex_tl_rfcomm_cb_ptr)
#endif

static tOBEX_TL_RFCOMM_CCB *allocate_ccb(void)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = NULL;
    for(int i = 0; i < OBEX_TL_RFCOMM_NUM_CONN; ++i) {
        if (obex_tl_rfcomm_cb.ccb[i].allocated == 0) {
            obex_tl_rfcomm_cb.ccb[i].allocated = i + 1;
            p_ccb = &obex_tl_rfcomm_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tOBEX_TL_RFCOMM_SCB *allocate_scb(void)
{
    tOBEX_TL_RFCOMM_SCB *p_scb = NULL;
    for(int i = 0; i < OBEX_TL_RFCOMM_NUM_SERVER; ++i) {
        if (obex_tl_rfcomm_cb.scb[i].allocated == 0) {
            obex_tl_rfcomm_cb.scb[i].allocated = i + 1;
            p_scb = &obex_tl_rfcomm_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

static void free_ccb(tOBEX_TL_RFCOMM_CCB *p_ccb)
{
    memset(p_ccb, 0, sizeof(tOBEX_TL_RFCOMM_CCB));
}

static void free_scb(tOBEX_TL_RFCOMM_SCB *p_scb)
{
    memset(p_scb, 0, sizeof(tOBEX_TL_RFCOMM_SCB));
}

static tOBEX_TL_RFCOMM_CCB *find_ccb_by_handle(UINT16 handle)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = NULL;
    if (handle > 0 && handle <= OBEX_TL_RFCOMM_NUM_CONN) {
        if (obex_tl_rfcomm_cb.ccb[handle-1].allocated == handle) {
            p_ccb = &obex_tl_rfcomm_cb.ccb[handle-1];
        }
    }
    return p_ccb;
}

static tOBEX_TL_RFCOMM_CCB *find_ccb_by_rfc_handle(UINT16 rfc_handle)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = NULL;
    for(int i = 0; i < OBEX_TL_RFCOMM_NUM_CONN; ++i) {
        if (obex_tl_rfcomm_cb.ccb[i].allocated && obex_tl_rfcomm_cb.ccb[i].rfc_handle == rfc_handle) {
            p_ccb = &obex_tl_rfcomm_cb.ccb[i];
            break;
        }
    }
    return p_ccb;
}

static tOBEX_TL_RFCOMM_SCB *find_scb_by_handle(UINT16 handle)
{
    tOBEX_TL_RFCOMM_SCB *p_scb = NULL;
    handle = handle >> 8;
    if (handle > 0 && handle <= OBEX_TL_RFCOMM_NUM_SERVER) {
        if (obex_tl_rfcomm_cb.scb[handle-1].allocated == handle) {
            p_scb = &obex_tl_rfcomm_cb.scb[handle-1];
        }
    }
    return p_scb;
}

static tOBEX_TL_RFCOMM_SCB *find_scb_by_rfc_handle(UINT16 rfc_handle)
{
    tOBEX_TL_RFCOMM_SCB *p_scb = NULL;
    for(int i = 0; i < OBEX_TL_RFCOMM_NUM_SERVER; ++i) {
        if (obex_tl_rfcomm_cb.scb[i].allocated && obex_tl_rfcomm_cb.scb[i].rfc_handle == rfc_handle) {
            p_scb = &obex_tl_rfcomm_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

static tOBEX_TL_RFCOMM_SCB *find_scb_by_scn(UINT16 scn)
{
    tOBEX_TL_RFCOMM_SCB *p_scb = NULL;
    for(int i = 0; i < OBEX_TL_RFCOMM_NUM_SERVER; ++i) {
        if (obex_tl_rfcomm_cb.scb[i].allocated && obex_tl_rfcomm_cb.scb[i].scn == scn) {
            p_scb = &obex_tl_rfcomm_cb.scb[i];
            break;
        }
    }
    return p_scb;
}

static void rfcomm_mgmt_event_handler(tOBEX_TL_RFCOMM_CCB *p_ccb, UINT32 code)
{
    tOBEX_TL_MSG msg = {0};
    msg.any.hdl = p_ccb->allocated;
    switch (code)
    {
    case PORT_SUCCESS:
        /* event already handled, do nothing */
        break;
    default:
        /* other event, disconnect */
        obex_tl_rfcomm_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
        free_ccb(p_ccb);
        break;
    }
}

static void rfcomm_client_mgmt_callback(UINT32 code, UINT16 rfc_handle, void* data)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = find_ccb_by_rfc_handle(rfc_handle);
    if (p_ccb == NULL) {
        OBEX_TL_RFCOMM_TRACE_DEBUG("No ccb to handle rfcomm event\n");
        return;
    }
    /* connection opened, handle event here */
    if (code == PORT_SUCCESS) {
        assert(data != NULL);
        tPORT_MGMT_CL_CALLBACK_ARG *cl_mgmt_cb_arg = (tPORT_MGMT_CL_CALLBACK_ARG *)data;
        p_ccb->mtu = cl_mgmt_cb_arg->peer_mtu;

        tOBEX_TL_MSG msg = {0};
        msg.conn_open.hdl = p_ccb->allocated;
        msg.conn_open.peer_mtu = p_ccb->mtu;
        msg.conn_open.our_mtu = p_ccb->mtu;
        obex_tl_rfcomm_cb.callback(OBEX_TL_CONN_OPEN_EVT, &msg);
    }
    rfcomm_mgmt_event_handler(p_ccb, code);
}

static void rfcomm_server_mgmt_callback(UINT32 code, UINT16 rfc_handle, void* data)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = NULL;
    /* incoming connection, handle event here */
    if (code == PORT_SUCCESS) {
        assert(data != NULL);
        tOBEX_TL_RFCOMM_SCB *p_scb = find_scb_by_rfc_handle(rfc_handle);
        tPORT_MGMT_SR_CALLBACK_ARG *sr_mgmt_cb_arg = (tPORT_MGMT_SR_CALLBACK_ARG *)data;
        if (p_scb == NULL) {
            OBEX_TL_RFCOMM_TRACE_WARNING("No scb to this rfcomm connection\n");
            /* tell rfcomm to reject this connection */
            sr_mgmt_cb_arg->accept = FALSE;
            return;
        }

        /* try to find p_ccb with this rfc_handle, we expect to get a NULL */
        p_ccb = find_ccb_by_rfc_handle(rfc_handle);
        if (p_ccb == NULL) {
            p_ccb = allocate_ccb();
            if (p_ccb == NULL) {
                OBEX_TL_RFCOMM_TRACE_WARNING("can not allocate a ccb for new connection\n");
                sr_mgmt_cb_arg->accept = FALSE;
                return;
            }
        }
        else {
            OBEX_TL_RFCOMM_TRACE_WARNING("found duplicate rfcomm connection\n");
        }

        p_ccb->initiator = FALSE;
        p_ccb->rfc_handle = rfc_handle;
        p_ccb->scn = p_scb->scn;
        p_ccb->mtu = sr_mgmt_cb_arg->peer_mtu;
        /* get peer bd_addr */
        PORT_CheckConnection(rfc_handle, FALSE, p_ccb->addr, NULL);

        tOBEX_TL_MSG msg = {0};
        msg.conn_income.hdl = p_ccb->allocated;
        msg.conn_income.peer_mtu = p_ccb->mtu;
        msg.conn_income.our_mtu = p_ccb->mtu;
        msg.conn_income.svr_hdl = (p_scb->allocated << 8);
        obex_tl_rfcomm_cb.callback(OBEX_TL_CONN_INCOME_EVT, &msg);
    }
    else {
        /* other event, it means server is connected */
        p_ccb = find_ccb_by_rfc_handle(rfc_handle);
        if (p_ccb == NULL) {
            OBEX_TL_RFCOMM_TRACE_DEBUG("No ccb to handle rfcomm event\n");
            return;
        }
    }
    rfcomm_mgmt_event_handler(p_ccb, code);
}

static int rfcomm_data_callback(UINT16 rfc_handle, UINT8 *p_buf, UINT16 len, int type)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = find_ccb_by_rfc_handle(rfc_handle);
    if (p_ccb != NULL && type == DATA_CO_CALLBACK_TYPE_INCOMING) {
        tOBEX_TL_MSG msg = {0};
        msg.data.hdl = p_ccb->allocated;
        msg.data.p_buf = (BT_HDR *)p_buf;
        obex_tl_rfcomm_cb.callback(OBEX_TL_DATA_EVT, &msg);
        PORT_FlowControl_GiveCredit(rfc_handle, TRUE, 1);
    }
    else if(p_buf != NULL) {
        osi_free(p_buf);
    }
    return 1;
}

static void rfcomm_event_callback(UINT32 code, UINT16 rfc_handle)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = find_ccb_by_rfc_handle(rfc_handle);
    if (p_ccb == NULL) {
        OBEX_TL_RFCOMM_TRACE_WARNING("No ccb to handle rfcomm event\n");
        return;
    }

    if (code & PORT_EV_FC) {
        tOBEX_TL_MSG msg = {0};
        msg.any.hdl = p_ccb->allocated;
        if (code & PORT_EV_FCS) {
            obex_tl_rfcomm_cb.callback(OBEX_TL_UNCONGEST_EVT, &msg);
        }
        else {
            obex_tl_rfcomm_cb.callback(OBEX_TL_CONGEST_EVT, &msg);
        }
    }
}

void obex_tl_rfcomm_init(tOBEX_TL_CBACK *callback)
{
    assert(callback != NULL);
#if (OBEX_DYNAMIC_MEMORY)
    if (!obex_tl_rfcomm_cb_ptr) {
        obex_tl_rfcomm_cb_ptr = (tOBEX_TL_RFCOMM_CB *)osi_malloc(sizeof(tOBEX_TL_RFCOMM_CB));
        if (!obex_tl_rfcomm_cb_ptr) {
            OBEX_TL_RFCOMM_TRACE_ERROR("OBEX over RFCOMM transport layer initialize failed, no memory\n");
            assert(0);
        }
    }
#endif /* #if (OBEX_DYNAMIC_MEMORY) */
    memset(&obex_tl_rfcomm_cb, 0, sizeof(tOBEX_TL_RFCOMM_CB));
    obex_tl_rfcomm_cb.callback = callback;
    obex_tl_rfcomm_cb.trace_level = BT_TRACE_LEVEL_ERROR;
}

void obex_tl_rfcomm_deinit(void)
{
#if (OBEX_DYNAMIC_MEMORY)
    if (obex_tl_rfcomm_cb_ptr) {
        osi_free(obex_tl_rfcomm_cb_ptr);
        obex_tl_rfcomm_cb_ptr = NULL;
    }
#endif /* #if (OBEX_DYNAMIC_MEMORY) */
}

UINT16 obex_tl_rfcomm_connect(tOBEX_TL_SVR_INFO *server)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = allocate_ccb();
    if (p_ccb == NULL) {
        return 0;
    }

    BTM_SetSecurityLevel(TRUE, "", BTM_SEC_SERVICE_OBEX, server->rfcomm.sec_mask, BT_PSM_RFCOMM, BTM_SEC_PROTO_RFCOMM, server->rfcomm.scn);
    if (RFCOMM_CreateConnection(UUID_PROTOCOL_OBEX, server->rfcomm.scn, FALSE, server->rfcomm.pref_mtu,
                                server->rfcomm.addr, &p_ccb->rfc_handle, rfcomm_client_mgmt_callback) != PORT_SUCCESS) {
        free_ccb(p_ccb);
        return 0;
    }

    /* set up data callback, event mask and event callback */
    PORT_SetDataCOCallback(p_ccb->rfc_handle, rfcomm_data_callback);
    PORT_SetEventMask(p_ccb->rfc_handle, OBEX_TL_RFCOMM_EVENT_MARK);
    PORT_SetEventCallback(p_ccb->rfc_handle, rfcomm_event_callback);

    bdcpy(p_ccb->addr, server->rfcomm.addr);
    p_ccb->scn = server->rfcomm.scn;
    p_ccb->initiator = TRUE;

    return p_ccb->allocated;
}

void obex_tl_rfcomm_disconnect(UINT16 handle)
{
    tOBEX_TL_RFCOMM_CCB *p_ccb = find_ccb_by_handle(handle);
    if (p_ccb != NULL) {
        RFCOMM_RemoveConnection(p_ccb->rfc_handle);
        free_ccb(p_ccb);
    }
}

UINT16 obex_tl_rfcomm_send(UINT16 handle, BT_HDR *p_buf)
{
    UINT16 ret = OBEX_TL_FAILED;
    tOBEX_TL_RFCOMM_CCB *p_ccb = find_ccb_by_handle(handle);
    do {
        if (p_ccb == NULL) {
            osi_free(p_buf);
            break;
        }

        /* Can not send data size larger than MTU */
        /* Offset should not smaller than OBEX_TL_RFCOMM_BT_HDR_MIN_OFFSET */
        if (p_buf->len > p_ccb->mtu || p_buf->offset < OBEX_TL_RFCOMM_BT_HDR_MIN_OFFSET) {
            osi_free(p_buf);
            break;
        }

        if (PORT_Write(p_ccb->rfc_handle, p_buf) == PORT_SUCCESS) {
            ret = OBEX_TL_SUCCESS;
        }
    } while (0);
    return ret;
}

UINT16 obex_tl_rfcomm_bind(tOBEX_TL_SVR_INFO *server)
{
    tOBEX_TL_RFCOMM_SCB *p_scb = find_scb_by_scn(server->rfcomm.scn);
    if (p_scb != NULL) {
        /* scn already used */
        return 0;
    }

    p_scb = allocate_scb();
    if (p_scb == NULL) {
        OBEX_TL_RFCOMM_TRACE_WARNING("Can not allocate scb, out of number\n");
        return 0;
    }

    BTM_SetSecurityLevel(FALSE, "", BTM_SEC_SERVICE_OBEX, server->rfcomm.sec_mask, BT_PSM_RFCOMM, BTM_SEC_PROTO_RFCOMM, server->rfcomm.scn);
    if (RFCOMM_CreateConnection(UUID_PROTOCOL_OBEX, server->rfcomm.scn, TRUE, server->rfcomm.pref_mtu,
                                server->rfcomm.addr, &p_scb->rfc_handle, rfcomm_server_mgmt_callback) != PORT_SUCCESS) {
        free_scb(p_scb);
        return 0;
    }

    /* set up data callback, event mask and event callback */
    PORT_SetDataCOCallback(p_scb->rfc_handle, rfcomm_data_callback);
    PORT_SetEventMask(p_scb->rfc_handle, OBEX_TL_RFCOMM_EVENT_MARK);
    PORT_SetEventCallback(p_scb->rfc_handle, rfcomm_event_callback);

    p_scb->scn = server->rfcomm.scn;

    /* left shift 8 bits as server handle, avoid confuse with connection handle */
    return (p_scb->allocated << 8);
}

void obex_tl_rfcomm_unbind(UINT16 handle)
{
    tOBEX_TL_RFCOMM_SCB *p_scb = find_scb_by_handle(handle);
    if (p_scb) {
        tOBEX_TL_RFCOMM_CCB *p_ccb = NULL;
        while ((p_ccb = find_ccb_by_rfc_handle(p_scb->rfc_handle)) != NULL) {
            RFCOMM_RemoveConnection(p_ccb->rfc_handle);
            tOBEX_TL_MSG msg = {0};
            msg.any.hdl = p_ccb->allocated;
            obex_tl_rfcomm_cb.callback(OBEX_TL_DIS_CONN_EVT, &msg);
            free_ccb(p_ccb);
        }
        RFCOMM_RemoveServer(p_scb->rfc_handle);
        free_scb(p_scb);
    }
}

static tOBEX_TL_OPS obex_tl_rfcomm_ops = {
    .init = obex_tl_rfcomm_init,
    .deinit = obex_tl_rfcomm_deinit,
    .connect = obex_tl_rfcomm_connect,
    .disconnect = obex_tl_rfcomm_disconnect,
    .bind = obex_tl_rfcomm_bind,
    .unbind = obex_tl_rfcomm_unbind,
    .send = obex_tl_rfcomm_send
};

/*******************************************************************************
**
** Function         obex_tl_rfcomm_ops_get
**
** Description      Get the operation function structure pointer of OBEX over
**                  RFCOMM transport layer
**
** Returns          Pointer to operation function structure
**
*******************************************************************************/
tOBEX_TL_OPS *obex_tl_rfcomm_ops_get(void)
{
    return &obex_tl_rfcomm_ops;
}

#endif /* #if (OBEX_INCLUDED == TRUE && RFCOMM_INCLUDED == TRUE) */
