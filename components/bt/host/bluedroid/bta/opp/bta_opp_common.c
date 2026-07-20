/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include "osi/allocator.h"
#include "common/bt_target.h"
#include "stack/goep_common.h"
#if BTA_OPP_CLIENT_INCLUDED
#include "stack/goepc_api.h"
#endif
#if BTA_OPP_SERVER_INCLUDED
#include "stack/goeps_api.h"
#endif
#include "bta_opp_int.h"

#if BTA_OPP_INCLUDED

#if BTA_DYNAMIC_MEMORY == FALSE
tBTA_OPP_CB bta_opp_cb;
#else
tBTA_OPP_CB *bta_opp_cb_ptr;
#endif

tBTA_OPP_CCB *bta_opp_allocate_ccb(void)
{
    for (int i = 0; i < BTA_OPP_MAX_CONNECTION; i++) {
        if (bta_opp_cb.ccb[i].allocated == 0) {
            memset(&bta_opp_cb.ccb[i], 0, sizeof(tBTA_OPP_CCB));
            bta_opp_cb.ccb[i].allocated = i + 1;
            bta_opp_cb.ccb[i].handle = i + 1;
            bta_opp_cb.ccb[i].max_tx = OPP_DEFAULT_MTU;
            bta_opp_cb.ccb[i].max_rx = OPP_DEFAULT_MTU;
            return &bta_opp_cb.ccb[i];
        }
    }
    return NULL;
}

tBTA_OPP_CCB *bta_opp_find_ccb_by_handle(UINT16 handle)
{
    if (handle == 0 || handle > BTA_OPP_MAX_CONNECTION) {
        return NULL;
    }
    if (bta_opp_cb.ccb[handle - 1].allocated == handle) {
        return &bta_opp_cb.ccb[handle - 1];
    }
    return NULL;
}

tBTA_OPP_CCB *bta_opp_find_ccb_by_goep_handle(UINT8 role, UINT16 goep_handle)
{
    /* The client (GOEPC) and server (GOEPS) connection handle spaces are
     * independent and both start at 1, so a match must also verify the role
     * to allow simultaneous client and server sessions. */
    for (int i = 0; i < BTA_OPP_MAX_CONNECTION; i++) {
        if (bta_opp_cb.ccb[i].allocated && bta_opp_cb.ccb[i].role == role &&
            bta_opp_cb.ccb[i].goep_handle == goep_handle) {
            return &bta_opp_cb.ccb[i];
        }
    }
    return NULL;
}

UINT8 bta_opp_get_conn_num(void)
{
    UINT8 num = 0;

#if BTA_DYNAMIC_MEMORY == TRUE
    if (bta_opp_cb_ptr == NULL) {
        return 0;
    }
#endif
    for (int i = 0; i < BTA_OPP_MAX_CONNECTION; i++) {
        if (bta_opp_cb.ccb[i].allocated) {
            num++;
        }
    }
    return num;
}

void bta_opp_free_ccb(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL) {
        return;
    }
#if BTA_OPP_CLIENT_INCLUDED
    if (p_ccb->role == BTA_OPP_ROLE_CLIENT) {
        bta_opp_client_cleanup_ccb(p_ccb);
        bta_opp_client_free_sdp_db(p_ccb);
    }
#endif
#if BTA_OPP_SERVER_INCLUDED
    if (p_ccb->role == BTA_OPP_ROLE_SERVER) {
        bta_opp_server_cleanup_ccb(p_ccb);
    }
#endif
    if (p_ccb->name) {
        osi_free(p_ccb->name);
    }
    if (p_ccb->type) {
        osi_free(p_ccb->type);
    }
    memset(p_ccb, 0, sizeof(tBTA_OPP_CCB));
}

static UINT8 bta_opp_sys_id(const tBTA_OPP_CCB *p_ccb)
{
    return (p_ccb->role == BTA_OPP_ROLE_CLIENT) ? BTA_ID_OPC : BTA_ID_OPS;
}

static void bta_opp_pm_conn_open(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL || p_ccb->pm_open) {
        return;
    }
    bta_sys_conn_open(bta_opp_sys_id(p_ccb), p_ccb->allocated, p_ccb->bd_addr);
    p_ccb->pm_open = TRUE;
}

static void bta_opp_pm_conn_close(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL || !p_ccb->pm_open) {
        return;
    }
    bta_sys_conn_close(bta_opp_sys_id(p_ccb), p_ccb->allocated, p_ccb->bd_addr);
    p_ccb->pm_open = FALSE;
}

static void bta_opp_pm_idle(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL || !p_ccb->pm_open) {
        return;
    }
    bta_sys_idle(bta_opp_sys_id(p_ccb), p_ccb->allocated, p_ccb->bd_addr);
}

void bta_opp_pm_busy(tBTA_OPP_CCB *p_ccb)
{
    if (p_ccb == NULL || !p_ccb->pm_open) {
        return;
    }
    bta_sys_busy(bta_opp_sys_id(p_ccb), p_ccb->allocated, p_ccb->bd_addr);
}

void bta_opp_report_conn(tBTA_OPP_CCB *p_ccb, BOOLEAN connected, tBTA_OPP_STATUS status)
{
    if (p_ccb == NULL) {
        return;
    }

    if (connected) {
        bta_opp_pm_conn_open(p_ccb);
    } else {
        bta_opp_pm_conn_close(p_ccb);
    }

    if (bta_opp_cb.p_cback == NULL) {
        return;
    }

    tBTA_OPP data;
    memset(&data, 0, sizeof(tBTA_OPP));
    data.conn.handle = p_ccb->handle;
    data.conn.role = p_ccb->role;
    data.conn.status = status;
    data.conn.connected = connected;
    bdcpy(data.conn.bd_addr, p_ccb->bd_addr);
    bta_opp_cb.p_cback(connected ? BTA_OPP_CONN_OPEN_EVT : BTA_OPP_CONN_CLOSE_EVT, &data);
}

void bta_opp_report_complete(tBTA_OPP_CCB *p_ccb, tBTA_OPP_STATUS status)
{
    if (p_ccb == NULL) {
        return;
    }

    tBTA_OPP data;
    memset(&data, 0, sizeof(tBTA_OPP));
    data.complete.handle = p_ccb->handle;
    data.complete.role = p_ccb->role;
    data.complete.status = status;
    data.complete.transferred = p_ccb->transferred;
    if (bta_opp_cb.p_cback) {
        bta_opp_cb.p_cback(BTA_OPP_TRANSFER_COMPLETE_EVT, &data);
    }
    bta_opp_pm_idle(p_ccb);
}

void bta_opp_report_progress(tBTA_OPP_CCB *p_ccb)
{
    if (bta_opp_cb.p_cback == NULL || p_ccb == NULL) {
        return;
    }

    tBTA_OPP data;
    memset(&data, 0, sizeof(tBTA_OPP));
    data.progress.handle = p_ccb->handle;
    data.progress.role = p_ccb->role;
    data.progress.transferred = p_ccb->transferred;
    data.progress.total = p_ccb->total_len;
    bta_opp_cb.p_cback(BTA_OPP_PROGRESS_EVT, &data);
}

void bta_opp_close_connection(tBTA_OPP_CCB *p_ccb, tBTA_OPP_STATUS status, BOOLEAN report)
{
    if (p_ccb == NULL) {
        return;
    }

    if (p_ccb->goep_handle != 0) {
#if BTA_OPP_CLIENT_INCLUDED
        if (p_ccb->role == BTA_OPP_ROLE_CLIENT) {
            GOEPC_Close(p_ccb->goep_handle);
        }
#endif
#if BTA_OPP_SERVER_INCLUDED
        if (p_ccb->role == BTA_OPP_ROLE_SERVER) {
            GOEPS_CloseConn(p_ccb->goep_handle);
        }
#endif
        p_ccb->goep_handle = 0;
    }

    if (report) {
        bta_opp_report_conn(p_ccb, FALSE, status);
    }
    bta_opp_pm_conn_close(p_ccb);
    bta_opp_free_ccb(p_ccb);
}

#endif /* BTA_OPP_INCLUDED */
